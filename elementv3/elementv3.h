#include <string>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>

using namespace std;

#ifndef ELEMENT

#define ELEMENT

#define THRESHOLD 10
#define MAX_LINKS

class structure {
public:
    void call (int);				// Send an impulse to this structure

    structure ();					// Default constructor
private:
    unsigned char charge;			// The amount of charge hanging around in the structure
    structure* links[MAX_LINKS];	// List of outgoing connections to other structure
};

struct charge_i {
	int c;
	structure *t;
};

// TODO: Write up
class structurebuffer {
public:
	mutex queueLock;
	queue<charge_i> queue;

    vector<structure*> buffer; // The base list of pointers to structures associated with this structurebuffer
    vector<structure*> sensors; // List of sensor nodes associated with this structurebuffer, always is a subset of buffer
    vector<structure*> motors; // List of motor nodes associated with this structurebuffer, always is a subset of buffer

	vector<thread *> threads;
	bool waitingForClose = false;

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
	void readIn (string path);

    // Randomly change the net slightly
    void modify (int iterations);

    // Insert a new node randomly into the net
    void insertRandomNode ();

    // Trigger an input on a sensor node with a given strength value
    void triggerInput (int sensorNum, int charge);
};

#endif