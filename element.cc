#include <iostream>
#include <map>
#include <random>
#include <chrono>
#include <string>
#include <vector>
#include <thread>

#define CHARGE_DECREASE 0.001
#define CHARGE_THRESHOLD 64

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
        thread t = thread(&structure::update, ref (&outgoingConnections[chargee]), charge);
    }

    // ===================================

    // Interpret and execute a single instruction
    void executeInstruction (string instr) {
        // Split instruction into parts
        vector<string> parts;
        string tmp = "";
        for (int i = 0; i < instr.length(); i++) {
            if (instr[i] == '|') {
                if (tmp.length() > 0) parts.push_back (tmp);
                tmp = "";
            } else {
                tmp.push_back (instr[i]);
            }
        }

        string cmd = parts[0];

        if (cmd == "charge") {
            i_charge (parts);
        }
    }

    // Iterate over the instructions in the instruction sequence and execute them all
    void executeInstructions () {
        // Split instructions
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
        
        // Iterate over instructions
        for (string instruction : queue) {
            executeInstruction (instruction);
        } 
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

    int activeCharge; // Charge currently present in this structure
    long long nanosAtLastUpdate; // Used to calculate charge dropoff between updates
    string instructionSequence; // Sequence of instructions to execute when the CHARGE_THRESHOLD is satisfied

    // Incoming-only function which updates the charge of the node and executes the instruction sequence if needed
    void update (int addingCharge = 0) {
        if (activeCharge > 0) {
            activeCharge -= (getNanos()-nanosAtLastUpdate)*CHARGE_DECREASE;
        }
        if (activeCharge < 0) activeCharge = 0;
        activeCharge += addingCharge;
        if (activeCharge >= CHARGE_THRESHOLD) {
            executeInstructions();
            activeCharge -= CHARGE_THRESHOLD;
        }
        nanosAtLastUpdate = getNanos();
    }
};

int main () {
    structure s = structure ();
    cout << s.title << endl;
}
