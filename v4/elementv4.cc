#include "elementv4.h"

// ===== UTILS =====
long long getNanos () {
    return chrono::duration_cast<chrono::nanoseconds>(chrono::time_point_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now()).time_since_epoch()).count();
}

vector<int> getIndices (string s) {
    vector<int> arr;
    arr.reserve (MAX_LINKS);
    int j = 0;
    for (int i = 0; i < s.size(); i++) {
        if (s[i] == ':') j++;
        else arr[j] += s[i];
    }
    return arr;
}

// ===== STRUCTURE =====
void structure::call (int i) {
    if (motorNum != -1) {
        if (i > THRESHOLD) {
            buffer->motorCall (motorNum);
        } else {
            charge = i;
        }
        return;
    }
    if (i+charge > THRESHOLD) {
        int n = numLinks();
        if (!n) return;
        int chargePerLink = (i + charge)/n;
        charge = 0;
        for (structure *s : links) {
            if (s == NULL) break;
            buffer->queueLock.lock();
            queue<int> q;
            charge_i ci;
            ci.c = chargePerLink;
            ci.t = s;
            buffer->queue.push (ci);
            buffer->queueLock.unlock();
        }
    } else {
        charge += i;
    }
}
void structure::setLinks (structure *ls[MAX_LINKS]) {
    for (int i = 0; i < MAX_LINKS; i++) links[i] = ls[i];
}
int structure::numLinks () {
    int n = 8;
    for (int i = 0; i < MAX_LINKS; i++) {
        if (links[i] == NULL) {
            n = i;
            break;
        }
    }
    return n;
}
void structure::addLink (structure *s) {
    int n = 8;
    for (int i = 0; i < MAX_LINKS; i++) {
        if (links[i] == NULL) {
            n = i;
            break;
        }
        if (links[i] == s) return;
    }
    if (n == 8) return;
    links[n] = s;
}
bool structure::isMotor () {return (motorNum != -1);}

structure::structure (structurebuffer* buf) {
    buffer = buf;
}
structure::structure (structurebuffer* buf, int mot) {
    motorNum = mot;
    buffer = buf;
}

// ===== STRUCTUREBUFFER =====
structurebuffer::structurebuffer () {
    long nanos = getNanos();
    #ifdef MIM_MODE
    env = new mim_environment (this);
    #endif
    generateNodes ();
    long end = getNanos();
    float ms = (end-nanos)/1000000.0;
    cout << "Setup took " << ms << endl;
    for (int i = 0; i < NUM_WORKER_THREADS; i++) {
        thread *t = new thread (&structurebuffer::threadStart, this);
        threads.push_back (t);
    }
    #ifdef MIM_MODE
    env->start();
    #endif

    while (!waitingForClose);
}
structurebuffer::structurebuffer (string path) {
    long nanos = getNanos();
    #ifdef MIM_MODE
    env = new mim_environment (this);
    #endif
    if (!readIn (path)) generateNodes();
    writeOut(path);
    long end = getNanos();
    float ms = (end-nanos)/1000000.0;
    cout << "Setup took " << ms << endl;
    for (int i = 0; i < NUM_WORKER_THREADS; i++) {
        thread *t = new thread (&structurebuffer::threadStart, this);
        threads.push_back (t);
    }
    #ifdef MIM_MODE
    env->start();
    #endif

    while (!waitingForClose);
}

void structurebuffer::threadStart () {
    while (!waitingForClose) {
        queueLock.lock();
        if (queue.empty()) { queueLock.unlock(); continue; }
        charge_i i = queue.front();
        queue.pop();
        queueLock.unlock();
        i.t->call(i.c);
        this_thread::sleep_for (chrono::milliseconds(WORKER_DELAY));
    }
}
int structurebuffer::indexOfNodeInNet (structure* s) {
	if (s == NULL) return -1; 
    int i = 0;
    for (structure *ss : buffer) {
        if (ss == s) return i;
        i++;
    }
    return -1;
}
void structurebuffer::writeOut (string path) {
    cout << "Writing out node pattern to path..." << endl;
    ofstream file;
    file.open (path);
    for (structure *struc : buffer) {
        for (structure *link : struc->links) {
            file << to_string (indexOfNodeInNet (link)) << ":";
        }
        file << "\n";
    }
    file << "<i";
    for (structure *istr : sensors) {
        file << to_string(indexOfNodeInNet(istr)) << ":";
    }
    file << "\n";

    file << "<o";
    for (structure *ostr : motors) {
        file << to_string(indexOfNodeInNet(ostr)) << ":";
    }
    file << "\n";

    file.close();
    cout << "Done." << endl;
}
bool structurebuffer::readIn (string path) {
    cout << "Reading node pattern from path..." << endl;
    ifstream file;
    file.open (path);
    if (!file.is_open()) {
        cout << "Failed." << endl;
        return false;
    }
    string line;
    vector<string> nodelines;
    string ins;
    string outs;
    while (getline (file, line)) {
        if (line[0] != '<') {
            nodelines.push_back(line);
            buffer.push_back (new structure (this));
        } else {
            if (line[1] == 'i') {
                ins = line.substr (2);
            } else if (line[1] == 'o') {
                outs = line.substr (2);
            }
        }
    }
    file.close();
    for (int i = 0; i < nodelines.size(); i++) {
        structure *links[MAX_LINKS];
        int j = 0;
        for (int n : getIndices (nodelines[i])) {
            links[j] = buffer[n];
            j++;
        }
        buffer[i]->setLinks (links);
    }
    for (int in : getIndices(ins)) {
        sensors.push_back (buffer[in]);
    }
    int x = 0;
    for (int on : getIndices(outs)) {
        motors.push_back (buffer[on]);
        structure *swapout = buffer[on];
        structure *n = new structure (this, x);
        n->setLinks (swapout->links);
        buffer[on] = n;
        x++;
    }
    cout << "Done." << endl;
    return true;
}
void structurebuffer::generateNodes () {
    cout << "Generated node pattern..." << endl;
    buffer.clear();
    sensors.clear();
    motors.clear();
    for (int i = 0; i < NUM_INPUTS; i++) {
        structure *s = new structure (this);
        buffer.push_back (s);
        sensors.push_back (s);
    }

    for (int i = 0; i < NUM_OUTPUTS; i++) {
        structure *s = new structure (this, i);
        buffer.push_back (s);
        motors.push_back (s);
    }

    for (int i,o = 0; i < NUM_INPUTS; i++) {
        if (o == motors.size()) o = 0;
        sensors[i]->addLink (motors[o]);
    }

    int numRelays = 0;
    while (numRelays <= NUM_RELAYS) {
        structure *o = buffer[rand() % buffer.size()];
        if (o->isMotor()) continue;
        int num = o->numLinks() > 0;
        if (num) {
            int connectionNum = rand () % o->numLinks();
            structure *r = new structure (this);
            r->addLink (o->links[connectionNum]);
            o->links[connectionNum] = r;
        }
        if (!(rand() % 3)) {
            o->addLink (buffer[(rand() % buffer.size()-(1 + NUM_INPUTS)) + NUM_INPUTS]);
        }
        numRelays++;
    }
    cout << "Done." << endl;
}

void structurebuffer::modify (int) {
    // TODO: Modify the net
}

void structurebuffer::triggerInput (int n, int v) {
    sensors[n]->call (v);
}
void structurebuffer::motorCall (int i) {
    env->motorCall (i);
}

// ===== ENVIRONMENT =====
mim_environment::mim_environment (structurebuffer *b) {
    buffer = b;
    xPos = entranceX = rand() % MAZE_SIZE;
    yPos = entranceY = rand() % MAZE_SIZE;
    exitX = rand() % MAZE_SIZE;
    exitY = rand() % MAZE_SIZE;
    generateMaze();
}

void mim_environment::start () {
    sensorUpdate();
    looper = thread (&mim_environment::loop, this);
}
void mim_environment::loop () {
    while (true) {
        buffer->triggerInput (16, 128);
        if (tmp) buffer->triggerInput (17, 128);
        tmp = !tmp;
        if (getNanos() - nanosAtLastUpdate > SENSOR_UPDATE_THRESHOLD)
            sensorUpdate();
    }
}

void mim_environment::sensorUpdate() {
    nanosAtLastUpdate = getNanos();

    int x,y = 0;
    int distance = -1;
    bool valueUnderCursor = false;
    while (!valueUnderCursor) {
        distance++;
        
        if (direction == 0) y++;
        if (direction == 1) x++;
        if (direction == 2) y--;
        if (direction == 3) x--;
        if (x > MAZE_SIZE || x < 0) break;
        if (y > MAZE_SIZE || y < 0) break;
        valueUnderCursor = mazeData[y][x];
    }

    cout << distance << endl; // FIXME:

    if (distance > 0) buffer->triggerInput (0, 128);
    if (distance > 1) buffer->triggerInput (1, 128);
    if (distance > 2) buffer->triggerInput (2, 128);
    if (distance > 4) buffer->triggerInput (3, 128);
    if (distance > 8) buffer->triggerInput (4, 128);
    if (distance > 16) buffer->triggerInput (5, 128);
    if (distance > 32) buffer->triggerInput (6, 128);
    if (distance > 48) buffer->triggerInput (7, 128);
    if (!(distance > 0)) buffer->triggerInput (8, 128);
    if (!(distance > 1)) buffer->triggerInput (9, 128);
    if (!(distance > 2)) buffer->triggerInput (10, 128);
    if (!(distance > 4)) buffer->triggerInput (11, 128);
    if (!(distance > 8)) buffer->triggerInput (12, 128);
    if (!(distance > 16)) buffer->triggerInput (13, 128);
    if (!(distance > 32)) buffer->triggerInput (14, 128);
    if (!(distance > 48)) buffer->triggerInput (15, 128);

    nanosAtLastUpdate = getNanos();
}

void mim_environment::motorCall(int i) {
    if (i == 0) {
        int tx = xPos;
        int ty = yPos;
        if (direction == 0) yPos++;
        if (direction == 1) xPos++;
        if (direction == 2) yPos--;
        if (direction == 3) xPos--;
        if (mazeData[xPos][yPos]) {
            xPos = tx;
            yPos = ty;
        }
    } else if (i == 1) {
        if (direction == 0) yPos--;
        if (direction == 1) xPos--;
        if (direction == 2) yPos++;
        if (direction == 3) xPos++;
    } else if (i == 2) {
        direction++;
        if (direction > 3) direction = 0;
    } else if (i == 3) {
        direction--;
        if (direction < 0) direction = 3;
    }
    sensorUpdate();
    if (xPos == exitX && yPos == exitY) buffer->waitingForClose = true;
}

void mim_environment::generateMaze () {
    /*bool dat[MAZE_SIZE][MAZE_SIZE] = {{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
                                      {1,0,0,0,0,0,1,0,0,1,0,0,1,1,0,1},
                                      {1,1,1,1,1,0,1,1,0,1,1,0,1,1,0,1},
                                      {1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1},
                                      {1,0,0,0,1,1,0,1,1,1,1,0,1,1,1,1},
                                      {1,0,1,0,1,1,0,0,0,0,0,0,1,1,1,1},
                                      {1,0,1,0,1,1,1,1,0,1,1,1,1,0,1,1},
                                      {1,0,1,0,0,0,1,1,0,1,1,1,0,0,1,1},
                                      {1,1,1,0,1,1,1,0,0,0,0,0,0,0,1,1},
                                      {1,0,0,0,1,1,0,0,0,1,1,0,1,1,1,1},
                                      {1,1,0,1,1,1,0,1,1,1,1,0,0,0,1,1},
                                      {1,1,0,0,0,1,0,1,0,1,1,0,1,1,1,1},
                                      {1,1,1,1,0,1,0,1,0,0,0,0,1,1,1,1},
                                      {1,0,0,0,0,1,0,1,1,1,0,1,1,1,1,1},
                                      {1,0,1,1,0,0,0,0,0,0,0,1,1,1,1,1},
                                      {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}};
    */

    for (int i = 0; i < MAZE_SIZE; i++) {
        for (int j = 0; j < MAZE_SIZE; j++) {
            mazeData[i][j] = true;
        }
    }
    
    int x = (rand () % (MAZE_SIZE - 2)) + 1;
    int y = (rand () % (MAZE_SIZE - 2)) + 1;
    int direction = rand () % 4;

    for (int it = 0; it < MAZE_ITS; it++) {
        int op = rand () % 6;
        if (op == 4) {
            direction++;
            if (direction > 3) direction = 0;
        } else if (op == 5) {
            direction--;
            if (direction < 0) direction = 3;
        } else {
            int nx = x;
            int ny = y;
            if (direction == 0) ny++;
            if (direction == 1) nx++;
            if (direction == 2) ny--;
            if (direction == 3) nx--;
            if (nx != 0 && nx != MAZE_SIZE-1 && ny != 0 && ny != MAZE_SIZE-1) {
                x = nx;
                y = ny;
                mazeData[y][x] = false;
            } else {
                direction++;
                if (direction > 3) direction = 0; 
            }
        }
    }

    // for (int i = 0; i < MAZE_SIZE; i++)
    //     for (int j = 0; j < MAZE_SIZE; j++)
    //         mazeData[i][j] = dat[i][j];

    
    entranceX = rand () % MAZE_SIZE;
    entranceY = rand () % MAZE_SIZE;
    while (mazeData[entranceY][entranceX]) {
        entranceX = rand () % MAZE_SIZE;
        entranceY = rand () % MAZE_SIZE;
    }
    exitX = rand () % MAZE_SIZE;
    exitY = rand () % MAZE_SIZE;
    while (mazeData[exitY][exitX] || (abs (exitY-entranceY) < 4) || abs (exitX-entranceX) < 4) {
        exitX = rand () % MAZE_SIZE;
        exitY = rand () % MAZE_SIZE;
    }
}

// ===== MAIN =====
int main () {
    long nanos = getNanos();
    structurebuffer *sb = new structurebuffer ();
    long end = getNanos();
    float ms = (end-nanos)/1000000.0;
    cout << "I finished the maze!" << endl;
    cout << "It took " << ms << " miliseconds" << endl;
    return 0;
}
// TODO: Add comments
// TODO: Connection strengths probability
// TODO: Mutation

// TODO: Simpler interface : file system nav