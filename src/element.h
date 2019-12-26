#include <string>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>

using namespace std;

#ifndef ELEMENT

#define CHARGE_DECREASE 0.001 // The amount by which charge decreases per nanosecond
#define CHARGE_THRESHOLD 64 // The minimum charge required for the node to be able to execute/actuate
#define CONNECTION_STRENGTH_DECREASE 0.00000000000001 // The amount by which connection strength decreases per nanosecond
#define CONNECTION_STRENGTH_INCREASE 10000 // The amount by which connection strength increases each time the connection is used

#define WORKER_THREADS_NUM 4 // The number of worker threads availble for updating waiting structures
#define WORKER_UPDATE_DELAY 10 // The delay in milliseconds between checking the queue per worker thread (i.e. there will be WORKER_THREADS_NUM concurrent updates in WORKER_UPDATE_DELAY milliseconds)

#define SAVE_LOOP_WAIT 50 // The number of sensor loop updates to perform before the net is autosaved (use dependent upon mainloop implementation)

// Mainloop function for running on a desktop
int mainloop_pc ();

// Mainloop function for running on the designed quad system
int mainloop_quad ();

// Quickly grab the number of nanoseconds since start time
long long getNanos ();

// Count the number of times a character appears in a given string
int countOccurrences (char c, string s);

class actionqueue;

// Class representing a single node in the complete net
class structure {
private:
    // Reference to the actionqueue attached to the structurebuffer of which this structure is a part
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

    // If this is a motor node, call whatever function is attached to the node
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

    // Remove an outgoing connection, and any instructions which relate to it, and fixes references in other instructions
    void removeConnection (int connNum);

    // Quickly get a list of individual instructions
    vector<string> getInstructions ();

    // Quickly set the instruction sequence from a list of individual instructions
    void setInstructions (vector<string> v);
    
    // Return a list of parts from a given instruction
    vector<string> getInstructionParts (string instruction);

    bool isMotor = false; // Determines if this node is a motor node or not
    int motorNum = -1; // Reference for the motor handler function to decide what operation to perform, -1 if the node is not a motor node
    void (*actuationHandle) (int); // Function pointer to the motor handler function, unset if the node is not a motor node
};

// Class containing data about a charge instruction that needs to be performed
class charge_i {
public:
    // The target structure
	structure *chargee;
    // The amount of charge to add
	int charge;

    // Constructor for convenience
	charge_i (structure *s, int c);
};

class actionqueue {
private:
    // The base queue containing the instructions which need to be executed
	queue<charge_i> queue;
    // List maintaining references to the worker threads associated with this actionqueue object
	vector<thread*> threads;

public:
    // Mutex maintaining thread-safe-ness
	mutex m;
    
    // Safely push a single new charge instruction onto the queue
	void push (charge_i c);

    // Safely pop a single charge instruction from the queue, returns a charge object with charge=-1 if there are no items in the queue
	charge_i pop ();

    // Initialiser
	actionqueue ();

    // Compiler made me write this
	actionqueue (actionqueue const& other);
};

class structurebuffer {
private:
    // Whether or not the structurebuffer should modify the net each time input is received
    bool modifyOnInput;

public:
    actionqueue *q; // Reference to the actionqueue object associated with this structurebuffer

    vector<structure*> buffer; // The base list of pointers to structures associated with this structurebuffer
    vector<structure*> sensors; // List of sensor nodes associated with this structurebuffer, always is a subset of buffer
    vector<structure*> motors; // List of motor nodes associated with this structurebuffer, always is a subset of buffer

    // Initialiser taking argument for whether or not to automatically modify the net on update
    structurebuffer (bool m);

    // Initialiser taking arguments for whether or not to automatically modify the net on update, and for a path to an existing net file to read in
    structurebuffer (bool m, string path);

    // Add a new, premade sensor node to the structurebuffer
    void addSensor (structure *s);
    // Add a new motor node to the structurebuffer, automatically created, assigning the argument as a pointer to the motor handler function
    void addMotor (void (*ah) (int));

    // Write out the entire net to a file which can later be read back in (resuming as before)
    void writeOut (string path);

    // Randomly change the net slightly
    void modify (int iterations);

    // Insert a new node randomly into the net
    void insertRandomNode ();

    // Trigger an input on a sensor node with a given strength value
    void triggerInput (int sensorNum, int charge);
};

// Generate a random instruction given a structure's available connections
string makeInstruction (structure forStruct);

// Generate a new node for a given structurebuffer
structure * makeStructure (structurebuffer buffer);

#define ELEMENT
#endif
