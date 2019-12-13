#include <iostream>
#include <map>
#include <random>
#include <chrono>
#include <string>
#include <vector>
#include <thread>

#define CHARGE_DECREASE 0.001
#define CHARGE_THRESHOLD 64
#define CONNECTION_STRENGTH_DECREASE 0.1

using namespace std;

long long getNanos () {
    return chrono::duration_cast<chrono::nanoseconds>(chrono::time_point_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now()).time_since_epoch()).count();
}

string namechars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";

class structure {
private:
    // ====== Instruction Funcitons ======

    // Instruction : Charge : Send a charge impulse to another structure by calling its update method
    void i_charge (vector<string> cmdparts) {
        if (cmdparts.size() < 3) return;
        int chargee = stoi (cmdparts[1]);
        int charge = stoi (cmdparts[2]);

        if (charge <= 0) return;
        if (chargee >= outgoingConnections.size()) return;

        connectionStrengths[chargee] += 100;
        thread t = thread(&structure::update, ref (&outgoingConnections[chargee]), charge);
    }

    // ===================================

    // Interpret and execute a single instruction
    void executeInstruction (string instr) {
        // Split instruction into parts
        vector<string> parts = getInstructionParts (instr);

        string cmd = parts[0];

        if (cmd == "charge") {
            i_charge (parts);
        }
    }

    vector<string> getInstructions () {
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

    vector<string> getInstructionParts (string instruction) {
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

    // Iterate over the instructions in the instruction sequence and execute them all
    void executeInstructions () {
        // Split instructions
        vector<string> queue = getInstructions();
        
        // Iterate over instructions
        for (string instruction : queue) {
            executeInstruction (instruction);
        } 
    }

    void removeReferencesTo (int connNum) {
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
public:

    // Constructor
    structure () {
        activeCharge = 0;
        title = "";
        for (int i = 0; i < 256; i++) {
            title += namechars[random () % 64];
        }
        nanosAtLastUpdate = getNanos();
    }

    
    string title; // Identifier for this structure
    vector<structure *> outgoingConnections; // List of outgoing connections to other structures
    vector<int> connectionStrengths; // Outgoing connection strenghts

    int activeCharge; // Charge currently present in this structure
    long long nanosAtLastUpdate; // Used to calculate charge dropoff between updates
    string instructionSequence; // Sequence of instructions to execute when the CHARGE_THRESHOLD is satisfied

    // Incoming-only function which updates the charge of the node and executes the instruction sequence if needed
    void update (int addingCharge = 0) {
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
};

class structurebuffer {
    vector<structure> buffer;
    vector<structure*> sensors;
public:
    structurebuffer () {

    }

    void addSensor (structure s) {
        buffer.push_back (s);
        sensors.push_back (&buffer[buffer.size()-1]);
    }
};

int main () {
    structure s = structure ();
    cout << s.title << endl;
}

// TODO: Implement reading and writing systems
// TODO: Write connection alteration system
// TODO: Write positive improvement system
// TODO: Write IO effectors and sensors for computer
