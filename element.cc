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

void structure::i_charge (vector<string> cmdparts) {
    if (cmdparts.size() < 3) return;
    int chargee = stoi (cmdparts[1]);
    int charge = stoi (cmdparts[2]);

    if (charge <= 0) return;
    if (chargee >= outgoingConnections.size()) return;

    connectionStrengths[chargee] += 100;
    thread t = thread(&structure::update, ref (&outgoingConnections[chargee]), charge);
}


void structure::executeInstruction (string instr) {
    // Split instruction into parts
    vector<string> parts = getInstructionParts (instr);

    string cmd = parts[0];

    if (cmd == "charge") {
        i_charge (parts);
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

void structure::removeReferencesTo (int connNum) {
    vector<string> instrs = getInstructions();
    string newInstrs;
    for (string instruction : instrs) {
        vector<string> parts = getInstructionParts (instruction);
        bool wantsRemoving = false;
        for (string part : parts) if (stoi (part) == connNum) wantsRemoving = true;
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
            removeReferencesTo (i);
            i--;
        }
    }

    // ALWAYS LAST
    nanosAtLastUpdate = getNanos();
}

structurebuffer::structurebuffer () {

}

structurebuffer::structurebuffer (string path) {
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
    }
}




int main () {
    structurebuffer sb = setupForPC ();
}

// TODO: Write positive improvement system