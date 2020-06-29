#include <iostream>
#include <vector>
#include <math.h>
#include <chrono>
#include <thread>
#include <time.h>

#ifndef ELEMENT

#define ELEMENT

#define MAZE_SIZE 32
#define MAZE_ITS MAZE_SIZE*10

#define VISION_RADIUS 2
#define NUM_INPUTS (((VISION_RADIUS*2)+1) * ((VISION_RADIUS*2))+1) + 4
#define NUM_OUTPUTS 4
#define NUM_RELAYS 500

using namespace std;

#pragma pack(1)
class Node {
private:
    string operation = "";
public:
    bool computedThisCycle;
    unsigned char value;

    void compute ();
    Node (string);
    Node ();

    void regenerateOperation ();
    void addInput (Node *, bool);

    int numLinks ();
    vector<Node *>requirements;
};
#pragma pack()

vector<Node *> nodes;
vector<Node *> sensors;
vector<Node *> motors;


#endif