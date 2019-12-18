#include <iostream>
#include <random>
#include <chrono>
#include <string>
#include <vector>
#include <thread>

#include "element.h"
#include "generators.cc"

using namespace std;

long long getNanos () {
    return chrono::duration_cast<chrono::nanoseconds>(chrono::time_point_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now()).time_since_epoch()).count();
}

int countOccurrences (char c, string s) {
    int n = 0;
    for (int i = 0; i < s.size(); i++) if (s[i] == c) n++;
    return n;
}

void structure::i_charge (vector<string> cmdparts) {
    if (cmdparts.size() < 3) return;
    int chargee = stoi (cmdparts[1]);
    int charge = stoi (cmdparts[2]);

    if (charge <= 0) return;
    if (chargee >= outgoingConnections.size()) return;

    connectionStrengths[chargee] += 100;
    thread t = thread(&structure::update, ref (&outgoingConnections[chargee]), charge);
}

string makeInstruction (structure forStruct) {
    int r = random () % 2;
    if (r == 0) {
        return "charge|" + to_string(random() % forStruct.outgoingConnections.size());
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
            if (tmp.length() > 0) queue.push_back (tmp);
            tmp = "";
        } else {
            tmp.push_back (instructionSequence[i]);
        }
    }

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
    // Split instructions
    vector<string> queue = getInstructions();
    
    // Iterate over instructions
    for (string instruction : queue) {
        executeInstruction (instruction);
    } 
}

void structure::removeConnection (int connNum) {
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

structure::structure () {
    activeCharge = 0;
    title = "";
    for (int i = 0; i < 256; i++) {
        title += namechars[random () % 64];
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
        executeInstructions();
        activeCharge -= CHARGE_THRESHOLD;
    }
    
    // Update connnection strengths
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
    modifyOnInput = m;
}

structurebuffer::structurebuffer (bool m, string path) {
    modifyOnInput = m;

    // TODO: Read in
}

void structurebuffer::addSensor (structure s) {
    buffer.push_back (s);
    sensors.push_back (&buffer[buffer.size()-1]);
}

void structurebuffer::writeOut (string path) {
    // TODO: Write out
}

void structurebuffer::modify (int iterations) {
    for (int i = 0; i < iterations; i++) {
        // TODO: Randomly modify net
        int operation = random () % 3;
        int nodeIndex = random () % buffer.size();
        if (operation == 0) { // Change instructions
            int innerOperation = random () % 3;
            vector<string> instrs = buffer[nodeIndex].getInstructions ();
            if (innerOperation == 0) { // Modify
                instrs[random() % instrs.size()] = makeInstruction(buffer[nodeIndex]);
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
                    if (n == add) newI.push_back (makeInstruction(buffer[nodeIndex]));
                }
                instrs = newI;
            }
            buffer[nodeIndex].setInstructions (instrs);
        } else if (operation == 1) { // Change outgoing connections
            int innerOperation = random () % 2;
            if (innerOperation == 0) { // Change connection
                buffer[nodeIndex].outgoingConnections[random() % buffer[nodeIndex].outgoingConnections.size()] = &(buffer[random() % buffer.size()]);
            } else if (innerOperation == 1) { // Remove connection
                buffer[nodeIndex].removeConnection (random() % buffer[nodeIndex].outgoingConnections.size());
            }
        } else if (operation == 2) { // Insert new node
            // TODO: 
        }
    }
}



int main () {
    structurebuffer sb = setupForPC ();
}

// TODO: Write positive improvement system