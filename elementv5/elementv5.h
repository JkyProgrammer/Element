#include <iostream>
#include <vector>
#include <math.h>
#include <chrono>
#include <thread>

#ifndef ELEMENT

#define ELEMENT

#define MAZE_SIZE 64
#define MAZE_ITS 800

using namespace std;

#pragma pack(1)
class Node {
private:
    string operation = "";
    vector<Node *>requirements;
public:
    bool computedThisCycle;
    char value;

    void compute ();
    Node (string);
    Node ();

    void regenerateOperation ();
    void addInput (Node *, bool);
};
#pragma pack()

vector<Node *> nodes;
vector<Node *> inputs;
vector<Node *> outputs;


#endif