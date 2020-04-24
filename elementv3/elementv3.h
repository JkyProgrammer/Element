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

// TODO: Buffer
// TODO: Write up
class structurebuffer {

};

#endif