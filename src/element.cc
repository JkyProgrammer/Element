#include <iostream>
#include <random>
#include <chrono>
#include <string>
#include <vector>
#include <thread>

using namespace std;

#include "element.h"

string namechars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";

long long getNanos () {
    return chrono::duration_cast<chrono::nanoseconds>(chrono::time_point_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now()).time_since_epoch()).count();
}

int countOccurrences (char c, string s) {
    int n = 0;
    for (int i = 0; i < s.size(); i++) if (s[i] == c) n++;
    return n;
}

charge_i::charge_i (structure *s, int c) {
	chargee = s;
	charge = c;
}

void structure::i_charge (vector<string> cmdparts) {
    if (cmdparts.size() < 3) return;
    int chargee = stoi (cmdparts[1]);
    int charge = stoi (cmdparts[2]);

    if (charge <= 0) return;
    if (chargee >= outgoingConnections.size()) return;


    connectionStrengths[chargee] += CONNECTION_STRENGTH_INCREASE;
    q->push (charge_i (outgoingConnections[chargee], charge));
}

void structure::i_wait () {
    this_thread::sleep_for (chrono::milliseconds(100));
}

string makeInstruction (structure forStruct) {
    int r = random () % 2;
    if (r == 0) {
        return "charge|" + to_string(random() % forStruct.outgoingConnections.size()) + "|" + to_string((random() % 64) + 1);
    } else if (r == 1) {
        return "wait";
    }
    return "";
}


void structure::executeInstruction (string instr) {
    // Split instruction into parts
    vector<string> parts = getInstructionParts (instr);

    string cmd = parts[0];

    if (cmd == "charge") {
        i_charge (parts);
    } else if (cmd == "wait") {
        i_wait ();
    }
}

vector<string> structure::getInstructions () {
    vector<string> queue;
    string tmp = "";
    for (int i = 0; i < instructionSequence.size(); i++) {
        if (instructionSequence[i] == ' ') {
            if (tmp.size() > 0) queue.push_back (tmp);
            tmp = "";
        } else {
            tmp.push_back (instructionSequence[i]);
        }
    }
    if (tmp.size() > 0) queue.push_back (tmp);
    return queue;
}

void structure::setInstructions (vector<string> v) {
    string s = "";
    vector<bool> encounteredConnections;
    for (int x = 0; x < outgoingConnections.size(); x++) encounteredConnections.push_back (false);
    for (int i = 0; i < v.size(); i++) {
        s += v[i] + " ";
        vector<string> prts = getInstructionParts (v[i]);
        if (prts[0] == "charge") {
            encounteredConnections[stoi (prts[1])] = true;
        }
    }
    instructionSequence = s;
    for (int j = 0; j < encounteredConnections.size(); j++) if (!encounteredConnections[j]) removeConnection (j);
}

vector<string> structure::getInstructionParts (string instruction) {
    vector<string> queue;
    string tmp = "";
    for (int i = 0; i < instruction.size(); i++) {
        if (instruction[i] == '|') {
            if (tmp.length() > 0) queue.push_back (tmp);
            tmp = "";
        } else {
            tmp.push_back (instruction[i]);
        }
    }
    if (tmp.length() > 0) queue.push_back (tmp);

    return queue;
}

void structure::executeInstructions () {
	cout << "Executing instructions on " << title << endl;
    // Split instructions
    vector<string> queue = getInstructions();

    // Iterate over instructions
    for (string instruction : queue) {
        executeInstruction (instruction);
    }
    cout << "Execution of " << title << " finished successfully." << endl;
}

void structure::actuate () {
	cout << title << " is actuating!" << endl;
    actuationHandle (motorNum);
}

void structure::removeConnection (int connNum) {
    cout << "Removing connection " << connNum << endl;
    vector<string> instrs = getInstructions();
    string newInstrs;
    for (string instruction : instrs) {
        vector<string> parts = getInstructionParts (instruction);
        bool wantsRemoving = false;
        if (parts[0] == "charge" && stoi (parts[1]) == connNum) wantsRemoving = true;
        if (!wantsRemoving) {
            newInstrs += instruction + " ";
        }
    }
    vector<structure *> newo;
    vector<int> newc;
    for (int i = 0; i < outgoingConnections.size(); i++) {
        if (i != connNum) {
            newo.push_back (outgoingConnections[i]);
            newc.push_back (connectionStrengths[i]);
        }
    }

    outgoingConnections = newo;
    connectionStrengths = newc;
}

structure::structure (actionqueue *queue) {
	q = queue;
    activeCharge = 0;
    title = "";
    for (int i = 0; i < 32; i++) {
        title += namechars[random () % namechars.size()];
    }
    nanosAtLastUpdate = getNanos();
}

void structure::update (int addingCharge) {
    // Recalculate active charge
    if (activeCharge > 0) {
        activeCharge -= (getNanos()-nanosAtLastUpdate)*CHARGE_DECREASE;
    }
    if (activeCharge < 0) activeCharge = 0;
    activeCharge += addingCharge;
    if (activeCharge >= CHARGE_THRESHOLD) {
        if (!isMotor) {
            executeInstructions();
        } else {
            actuate ();
        }
        activeCharge -= CHARGE_THRESHOLD;
    }
    
    // Update connection strengths
    for (int i = 0; i < connectionStrengths.size(); i++) {
        connectionStrengths[i] = connectionStrengths[i] - (getNanos()-nanosAtLastUpdate)*CONNECTION_STRENGTH_DECREASE;
        if (connectionStrengths[i] < 0) {
            removeConnection (i);
            i--;
        }
    }

    // ALWAYS LAST
    nanosAtLastUpdate = getNanos();
}

structurebuffer::structurebuffer (bool m) {
	q = new actionqueue ();
    modifyOnInput = m;
}

structurebuffer::structurebuffer (bool m, string path) {
	q = new actionqueue ();
    modifyOnInput = m;

    // TODO: Read in
}

void structurebuffer::addSensor (structure *s) {
    buffer.push_back (s);
    sensors.push_back (s);
}

void structurebuffer::addMotor (void (*ah) (int)) {
    structure *s = new structure(q);
    s->isMotor = true;
    s->motorNum = motors.size();
    s->actuationHandle = ah;
    buffer.push_back (s);
    motors.push_back (s);
}

void structurebuffer::writeOut (string path) {
    // TODO: Write out
}

void structurebuffer::insertRandomNode () {
    // TODO: Insert random node
}

void structurebuffer::modify (int iterations) {
    cout << "Modifying net!" << endl;
    for (int i = 0; i < iterations; i++) {
        int operation = random () % 3;
        int nodeIndex = random () % buffer.size();
        if (operation == 0) { // Change instructions
            int innerOperation = random () % 3;
            vector<string> instrs = buffer[nodeIndex]->getInstructions ();
            if (innerOperation == 0) { // Modify
                instrs[random() % instrs.size()] = makeInstruction(*buffer[nodeIndex]);
            } else if (innerOperation == 1) { // Remove
                vector<string> newI;
                int remove = random () % instrs.size();
                for (int n = 0; n < instrs.size(); n++) if (n != remove) newI.push_back (instrs[n]);
                instrs = newI;
            } else if (innerOperation == 2) { // Insert
                vector<string> newI;
                int add = random () % instrs.size();
                for (int n = 0; n < instrs.size(); n++) { 
                    newI.push_back (instrs[n]);
                    if (n == add) newI.push_back (makeInstruction(*buffer[nodeIndex]));
                }
                instrs = newI;
            }
            buffer[nodeIndex]->setInstructions (instrs);
        } else if (operation == 1) { // Change outgoing connections
            int innerOperation = random () % 2;
            if (innerOperation == 0) { // Change connection
                buffer[nodeIndex]->outgoingConnections[random() % buffer[nodeIndex]->outgoingConnections.size()] = (buffer[random() % buffer.size()]);
            } else if (innerOperation == 1) { // Remove connection
                buffer[nodeIndex]->removeConnection (random() % buffer[nodeIndex]->outgoingConnections.size());
            }
        } else if (operation == 2) { // Insert new node
            insertRandomNode ();
        }
    }
}

void motorTest (int i) {
	cout << "We did it!" << endl;
    return;
}

int main () {
    structurebuffer test = structurebuffer (false);

    test.addMotor (&motorTest);

    structure *sensor = new structure (test.q);
    sensor->outgoingConnections.push_back (test.motors[0]);
    sensor->connectionStrengths.push_back (100000);
    sensor->instructionSequence = "charge|0|1000";
    test.addSensor (sensor);

    cout << "Sensor: " << test.sensors[0]->title << endl;
    
    cout << "Motor: " << test.motors[0]->title << endl;

    cout << "Commence test:" << endl << endl;
    
    while (1) {
        test.sensors[0]->update (1000);
        this_thread::sleep_for (chrono::milliseconds(500));
    }
    //mainloop_pc ();
}

// TODO: Write positive improvement system
