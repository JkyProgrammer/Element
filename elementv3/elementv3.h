#include <string>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <iostream>
#include <fstream>

using namespace std;

#ifndef ELEMENT

#define ELEMENT

#define THRESHOLD 10
#define MAX_LINKS 8
#define NUM_WORKER_THREADS 8
#define WORKER_DELAY 10

class structure {
public:
    void call (int);				// Send an impulse to this structure

    structure (structurebuffer *);	// Default constructor
	structure (structurebuffer*, int);
	void setLinks (structure *[MAX_LINKS]);
	structure* links[MAX_LINKS];	// List of outgoing connections to other structure
protected:
    unsigned char charge;			// The amount of charge hanging around in the structure
	structurebuffer *buffer;
	int motorNum = -1;
};

struct charge_i {
	int c;
	structure *t;
};

class structurebuffer {
public:
	mutex queueLock;
	queue<charge_i> queue;

    vector<structure*> buffer; // The base list of pointers to structures associated with this structurebuffer
    vector<structure*> sensors; // List of sensor nodes associated with this structurebuffer, always is a subset of buffer
    vector<structure*> motors; // List of motor nodes associated with this structurebuffer, always is a subset of buffer

	vector<thread *> threads; // Keeps the threads allocated
	bool waitingForClose = false; // Allows us to turn off the net and safely close all the threads

    // Initialiser taking argument for whether or not to automatically modify the net on update
    structurebuffer ();

    // Initialiser taking arguments for whether or not to automatically modify the net on update, and for a path to an existing net file to read in
    structurebuffer (string);

	int indexOfNodeInNet (structure*);

    // Write out the entire net to a file which can later be read back in (resuming as before)
    void writeOut (string);
	void readIn (string);

    void generateNodes (); // Generate a new net
    void modify (int); // Randomly change the net slightly
    void insertRandomNode (); // Insert a new node randomly into the net

    // Trigger an input on a sensor node with a given strength value
    void triggerInput (int, int);
	void motorCall (int);

	void threadStart ();
};

#endif