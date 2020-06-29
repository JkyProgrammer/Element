#include "elementv5.h"

bool hasOperationsLeft (string s) {
    for (char c : s) {
        if (c == '<') return true;
        if (c == '-') return true;
        if (c == '*') return true;
        if (c == '>') return true;
        if (c == '/') return true;
    }
    return false;
}

int evaluate (string s) {
    string fixed = s;
    if (s.size () < 1) return 0;
    int result;

    int cl = 0;
    int bracketDepth = 0;
    int brackStart = -1;
    while (cl < s.size()) {
        if (fixed[cl] == '(') {
            bracketDepth++;
            if (bracketDepth == 1) brackStart = cl;
        }
        else if (fixed[cl] == ')') {
            bracketDepth--;
            if (bracketDepth == 0) {
                int res = evaluate (fixed.substr (brackStart+1, cl-1));
                fixed.replace (brackStart, cl, to_string(res));
                brackStart = -1;
            }
            if (bracketDepth < 0) return rand();
        }
        cl++;
    }
    if (bracketDepth != 0) return rand();

    while (hasOperationsLeft(fixed)) {
        string op1 = 0;
        string op2 = 0;
        int operation = -1;
        cl = 0;
        while (cl < s.size()) {
            if (fixed[cl] == '<') {
                if (operation != -1) break;
                operation = 0;
            }
            else if (fixed[cl] == '-') {
                if (operation != -1) break;
                operation = 1;
            }
            else if (fixed[cl] == '*') {
                if (operation != -1) break;
                operation = 2;
            }
            else if (fixed[cl] == '>') {
                if (operation != -1) break;
                operation = 3;
            }
            else if (fixed[cl] == '/') {
                if (operation != -1) break;
                operation = 4;
            }
            else {
                if (operation == -1) op1 += fixed[cl];
                else if (operation == 4) break;
                else op2 += fixed[cl];
            }
        }
        if (operation == 0) {
            fixed.replace (0, cl-1, to_string ( (stoi (op1) < stoi (op2))*255 ));
        } else if (operation == 1) {
            fixed.replace (0, cl-1, to_string ( abs (stoi (op1) - stoi (op2)) ));
        } else if (operation == 2) {
            fixed.replace (0, cl-1, to_string ( (stoi (op1) + stoi (op2))/2 ));
        } else if (operation == 3) {
            fixed.replace (0, cl-1, to_string ( (stoi (op1) > stoi (op2))*255 ));
        } else if (operation == 4) {
            fixed.replace (0, cl-1, to_string ( stoi (op1) / 2 ));
        }
    }
    return stoi (fixed);
}

void Node::compute() {
    if (operation.size() < 1) { value = 0; return; }
    vector<unsigned char> input;
    int inputN = -1;
    cout << "Requirements: " << endl;
    for (Node *req : requirements) {
        inputN++;
        if (!req->computedThisCycle) req->compute();
        input.push_back(req->value);
        cout << to_string (req->value) << endl;
    }
    string str = operation;
    for (char c = 'a'; c <= 'z'; c++) {
        if (c-'a' >= input.size()) break;
        size_t s = str.find(c);
        if (s == string::npos) continue;
        str.replace (s, s-1, to_string(input[c-'a']));
    }
    string sstr = "";
    for (char c : str) {
        if (c != ' ') sstr += c;
    }
    value = evaluate (str);
    cout << to_string(value) << endl;
    computedThisCycle = true;
}

Node::Node (string op) {
    operation = op;
    operation.shrink_to_fit();
    requirements.shrink_to_fit();
}

Node::Node () {
    regenerateOperation();
    requirements.shrink_to_fit();
}

void clearComputedFlags () {
    for (Node *n : nodes) {
        n->computedThisCycle = false;
    }
}

void Node::regenerateOperation () {
    int numInputs = requirements.size();
    if (numInputs < 1) return;
    string oper;

    char ops[8] = {'<', '-', '*', '>', '/'};
    if (numInputs == 1) {
        if (rand() % 2) {
            oper = "a/";
        } else {
            oper = "a";
        }
    } else {
        for (int i = 0; i < numInputs-1; i++) {
            int op = rand () % 4;
            oper += ('a' + i);
            oper += ops[op];
        }
        oper += ('a'+(numInputs-1));
        int final = rand() % 3;
        if (final == 0) {
            oper += '/';
        }
    }
    operation = oper;
    cout << operation << endl;
    operation.shrink_to_fit();
}

void Node::addInput (Node *n, bool regen) {
    if (requirements.size() >= 26) return;
    requirements.push_back (n);
    requirements.shrink_to_fit();
    if (regen) regenerateOperation();
}

int Node::numLinks () {
    return requirements.size();
}

void generateNodes () {
    cout << "Generated node pattern..." << endl;
    nodes.clear();
    sensors.clear();
    motors.clear();
    
    for (int i = 0; i < NUM_OUTPUTS; i++) {
        Node *n = new Node ();
        nodes.push_back (n);
        motors.push_back (n);
    }

    for (int i = 0; i < NUM_INPUTS; i++) {
        Node *n = new Node ();
        nodes.push_back (n);
        sensors.push_back (n);
    }

    int numInLayer = NUM_INPUTS;
    vector<Node *> lastLayer = sensors;
    vector<Node *> nextLayer;
    while (numInLayer > 2*NUM_OUTPUTS) {
        for (int i = 0; i < numInLayer; i+=2) {
            Node *n = new Node ();
            n->addInput (lastLayer[i], false);
            if (i+1 < lastLayer.size())
                n->addInput (lastLayer[i+1], true);
            nodes.push_back(n);
            nextLayer.push_back(n);
        }
        numInLayer = nextLayer.size();
        lastLayer = nextLayer;
        nextLayer.clear();
    }

    for (int i,o = 0; i < numInLayer; i++) {
        if (o == motors.size()) o = 0;
        motors[o]->addInput (lastLayer[i], false);
    }

    for (int o = 0; o < NUM_OUTPUTS; o++) {
        motors[o]->regenerateOperation();
    }

    /*
    int numRelays = 0;
    while (numRelays <= NUM_RELAYS) {
        Node *o = nodes[rand() % nodes.size()];
        int num = o->numLinks() > 0;
        if (num) {
            int connectionNum = rand () % num;
            Node *r = new Node ();
            r->addInput (o->requirements[connectionNum], true);
            o->requirements[connectionNum] = r;
        }
        if (!(rand() % 3)) {
            Node *target = nodes[(rand() % nodes.size()-(1 + NUM_OUTPUTS)) + NUM_OUTPUTS];
            if (target != o) {
                o->addInput (target, true);
            }
        }

        numRelays++;
    }
    */
    cout << "Done." << endl;
}

bool mazeData[MAZE_SIZE][MAZE_SIZE] = {false};
int entranceX, entranceY, exitX, exitY;
int xPos, yPos;
void generateMaze () {
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

void displayMaze () {
    cout.flush();
    system("clear");

    for (int i = 0; i < MAZE_SIZE; i++) {
        for (int j = 0; j < MAZE_SIZE; j++) {
            if (i == yPos && j == xPos) cout << "+";
            else if (mazeData[i][j]) cout << "0";
            else cout << " ";
        }
        cout << endl;
    }
}

void updateNet () {
    int a = 0;
    int b = 0;
    for (int i = yPos-VISION_RADIUS; i < yPos+VISION_RADIUS; i++) {
        if (i < 0 || i >= MAZE_SIZE) continue;
        for (int j = xPos-VISION_RADIUS; j < xPos+VISION_RADIUS; j++) {
            if (j < 0 || j >= MAZE_SIZE) continue;
            unsigned char val = 0;
            if (mazeData[i][j]) val = 254;
            if (i == yPos && j == xPos) val = 64;
            if (i == exitY && j == exitX) val = 128;
            Node *s = sensors[(a*VISION_RADIUS*2) + b];
            s->value = val;
            s->computedThisCycle = true;
            cout << "Looked at " << to_string (j) << " " << to_string(i) << endl;
            cout << "Assigned " << to_string(s->value) << " to node at " << to_string(b) << " " << to_string(a) << endl;
            b++;
        }
        a++;
        b = 0;
    }

    for (int i = 0; i < 4; i++) {
        sensors[(VISION_RADIUS*VISION_RADIUS)+i]->value = rand () % 256;
        sensors[(VISION_RADIUS*VISION_RADIUS)+i]->computedThisCycle = true;
    }

    int vals[NUM_OUTPUTS] = {0};

    for (int o = 0; o < NUM_OUTPUTS; o++) {
        cout << "Computing motor " << o << endl;
        motors[o]->compute();
        vals[o] = motors[o]->value;
        cout << "Val: " << vals[0] << endl;
        cout << "Done." << endl;

    }

    // Calculate motion direction
    int totalWeight = vals[0] + vals[1] + vals[2] + vals[3];
    if (totalWeight == 0) return;
    int x = ((1 * (vals[0]/totalWeight)) + (1 * (vals[1]/totalWeight)) + (-1 * (vals[2]/totalWeight)) + (-1 * (vals[3]/totalWeight)))/4;
    int y = ((1 * (vals[0]/totalWeight)) + (-1 * (vals[1]/totalWeight)) + (1 * (vals[2]/totalWeight)) + (-1 * (vals[3]/totalWeight)))/4;
    xPos += x;
    yPos += y;
    cout << x << endl;
    cout << y << endl;
}


int main() {
    srand (time(NULL));
    generateMaze();
    xPos = entranceX;
    yPos = entranceY;

    generateNodes();
    chrono::milliseconds timespan(100);

    while (xPos != exitX || yPos != exitY) {
        clearComputedFlags();
        updateNet();
        displayMaze();
        this_thread::sleep_for(timespan);
    }

    return 0;
}