#include <string>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>

using namespace std;

#ifndef ELEMENT

#define CHARGE_DECREASE 0.001
#define CHARGE_THRESHOLD 64
#define CONNECTION_STRENGTH_DECREASE 0.00000000000001
#define CONNECTION_STRENGTH_INCREASE 10000

#define WORKER_THREADS_NUM 4
#define WORKER_UPDATE_DELAY 10

long long getNanos ();
int countOccurrences (char c, string s);

class actionqueue;

class structure {
private:
	actionqueue *q;

    // ====== Instruction Functions ======

    // Instruction : Charge : Send a charge impulse to another structure by calling its update method
    void i_charge (vector<string> cmdparts);

    // Instruction : Wait : Delay for a short period of time
    void i_wait ();

    // ===================================

    // Interpret and execute a single instruction
    void executeInstruction (string instr);

    // Iterate over the instructions in the instruction sequence and execute them all
    void executeInstructions ();

    void actuate ();
public:

    // Constructor
    structure (actionqueue *queue);

    
    string title; // Identifier for this structure
    vector<structure *> outgoingConnections; // List of outgoing connections to other structures
    vector<int> connectionStrengths; // Outgoing connection strengths

    int activeCharge; // Charge currently present in this structure
    long long nanosAtLastUpdate; // Used to calculate charge dropoff between updates
    string instructionSequence; // Sequence of instructions to execute when the CHARGE_THRESHOLD is satisfied

    // Incoming-only function which updates the charge of the node and executes the instruction sequence if needed
    void update (int addingCharge = 0);

    void removeConnection (int connNum);

    vector<string> getInstructions ();
    void setInstructions (vector<string> v);
    
    vector<string> getInstructionParts (string instruction);

    bool isMotor = false;
    int motorNum = -1;
    void (*actuationHandle) (int);
};

class charge_i {
public:
	structure *chargee;
	int charge;

	charge_i (structure *s, int c);
};

class actionqueue {
private:
	queue<charge_i> queue;
	vector<thread*> threads;

public:
	mutex m;
	void push (charge_i c);

	charge_i pop ();

	actionqueue ();

	actionqueue (actionqueue const& other);
};

class structurebuffer {
    bool modifyOnInput;

public:
    actionqueue *q;

    vector<structure*> buffer;
    vector<structure*> sensors;
    vector<structure*> motors;

    structurebuffer (bool m);

    structurebuffer (bool m, string path);

    void addSensor (structure *s);
    void addMotor (void (*ah) (int));

    void writeOut (string path);

    void modify (int iterations);

    void insertRandomNode ();
};

string makeInstruction (structure forStruct);

#define ELEMENT
#endif
