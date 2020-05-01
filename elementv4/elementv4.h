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
#define SENSOR_UPDATE_THRESHOLD 10

#define MIM_MODE // Set the node setup to be compatible with the mouse-in-maze environment

#ifdef MIM_MODE
#define NUM_INPUTS 16
#define NUM_OUTPUTS 4
#define NUM_RELAYS 500
#endif

class structurebuffer;
class environment;

#ifdef MIM_MODE
class mim_environment;
#endif

class structure {
public:
    void call (int);				// Send an impulse to this structure

    structure (structurebuffer*);	// Default constructor
	structure (structurebuffer*, int);
	void setLinks (structure *[MAX_LINKS]);
    void addLink (structure *);
	structure* links[MAX_LINKS] = {NULL};	// List of outgoing connections to other structure
    bool isMotor ();
    int numLinks ();
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
private:
    #ifdef MIM_MODE
    mim_environment *env;
    #endif
	vector<thread *> threads; // Keeps the threads allocated
public:
	mutex queueLock;
	queue<charge_i> queue;

    vector<structure*> buffer; // The base list of pointers to structures associated with this structurebuffer
    vector<structure*> sensors; // List of sensor nodes associated with this structurebuffer, always is a subset of buffer
    vector<structure*> motors; // List of motor nodes associated with this structurebuffer, always is a subset of buffer

	bool waitingForClose = false; // Allows us to turn off the net and safely close all the threads

    // Initialiser 
    structurebuffer ();

    // Initialiser taking an argument for a path to read/write nodes to/from
    structurebuffer (string);

	int indexOfNodeInNet (structure*);

    // Write out the entire net to a file which can later be read back in (resuming as before)
    void writeOut (string);
	bool readIn (string);

    void generateNodes (); // Generate a new net
    void modify (int); // Randomly change the net slightly
    void insertRandomNode (); // Insert a new node randomly into the net

    // Trigger an input on a sensor node with a given strength value
    void triggerInput (int, int);
	void motorCall (int);

	void threadStart ();
};

class environment {
protected:
    structurebuffer *buffer;

    thread *looper;
    void loop ();
public:
    void motorCall (int);
    void start ();
};

#ifdef MIM_MODE
#define MAZE_SIZE 16
class mim_environment : public environment {
private:
    int xPos, yPos;
    int exitX, exitY;
    int entranceX, entranceY;
    int direction;

    bool mazeData[MAZE_SIZE][MAZE_SIZE];

    void sensorUpdate ();
    void generateMaze ();

    void loop ();

    long long nanosAtLastUpdate;
public:
    mim_environment (structurebuffer *);
    void motorCall (int);
    void start ();
};
#endif

#endif