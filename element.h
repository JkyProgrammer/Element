#include <string>
#include <vector>

using namespace std;

#ifndef ELEMENT
#define CHARGE_DECREASE 0.001
#define CHARGE_THRESHOLD 64
#define CONNECTION_STRENGTH_DECREASE 0.1

string namechars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";

long long getNanos ();
int countOccurrences (char c, string s);
string makeInstruction (structure forStruct);

class structure {
private:
    // ====== Instruction Funcitons ======

    // Instruction : Charge : Send a charge impulse to another structure by calling its update method
    void i_charge (vector<string> cmdparts);

    // Instruction : Wait : Delay for a short period of time
    void i_wait ();

    // ===================================

    // Interpret and execute a single instruction
    void executeInstruction (string instr);

    // Iterate over the instructions in the instruction sequence and execute them all
    void executeInstructions ();

    void removeReferencesTo (int connNum);
public:

    // Constructor
    structure ();

    
    string title; // Identifier for this structure
    vector<structure *> outgoingConnections; // List of outgoing connections to other structures
    vector<int> connectionStrengths; // Outgoing connection strenghts

    int activeCharge; // Charge currently present in this structure
    long long nanosAtLastUpdate; // Used to calculate charge dropoff between updates
    string instructionSequence; // Sequence of instructions to execute when the CHARGE_THRESHOLD is satisfied

    // Incoming-only function which updates the charge of the node and executes the instruction sequence if needed
    void update (int addingCharge = 0);

    vector<string> getInstructions ();

    void setInstructions (vector<string> v);

    vector<string> getInstructionParts (string instruction);
};

class structurebuffer {
    vector<structure> buffer;
    vector<structure*> sensors;
    bool modifyOnInput;

public:
    structurebuffer (bool m);

    structurebuffer (bool m, string path);

    void addSensor (structure s);

    void writeOut (string path);

    void modify (int iterations);
};

#define ELEMENT
#endif