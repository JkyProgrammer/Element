// TODO: Write up
#include "elementv3.h"

void structure::call (int i) {
    if (motorNum != -1) {
        if (i > THRESHOLD) {
            buffer->motorCall (motorNum);
        } else {
            charge = i;
        }
        return;
    }
    if (i > THRESHOLD) {
        int numLinks = sizeof (links)/sizeof(structure*);
        int chargePerLink = (i + charge)/numLinks;
        for (structure *s : links) {
            buffer->queueLock.lock();
            queue<int> q;
            charge_i ci;
            ci.c = chargePerLink;
            ci.t = s;
            buffer->queue.push (ci);
            buffer->queueLock.unlock();
        }
    } else {
        charge = i;
    }
}

void structure::setLinks (structure *ls[MAX_LINKS]) {
    for (int i = 0; i < MAX_LINKS; i++) links[i] = ls[i];
}

structure::structure (structurebuffer* buf) {
    buffer = buf;
}
structure::structure (structurebuffer* buf, int mot) {
    motorNum = mot;
    buffer = buf;
}

structurebuffer::structurebuffer () {
    for (int i = 0; i < NUM_WORKER_THREADS; i++) {
        thread *t = new thread (threadStart);
        threads.push_back (t);
    }
    generateNodes ();
}

structurebuffer::structurebuffer (string path) {
    for (int i = 0; i < NUM_WORKER_THREADS; i++) {
        thread *t = new thread (threadStart);
        threads.push_back (t);
    }
    readIn (path);
}

void structurebuffer::threadStart () {
    while (!waitingForClose) {
        queueLock.lock();
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

void structurebuffer::readIn (string path) {
    ifstream file;
    file.open (path);
    if (!file.is_open) return;
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
}

void structurebuffer::generateNodes () {
    // TODO: Generate nodes
}

void structurebuffer::modify (int) {
    // TODO: Modify the net
}

void structurebuffer::insertRandomNode () {
    // TODO: Insert a random node
}

void structurebuffer::triggerInput (int, int) {
    // TODO: Pass input
}
void structurebuffer::motorCall (int) {
    // TODO: Receive a motor call
}