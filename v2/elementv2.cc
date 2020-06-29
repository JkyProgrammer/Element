#include <iostream>
#include <vector>
#include <math.h>
#include <ostream>
#include <fstream>

using namespace std;

/* Operation spec

00 = Constant Sum (varg0, carg0)
01 = Variable Sum (varg0, varg1)
02 = Constant Difference (varg0, carg0)
03 = Variable Difference (varg0, varg1)
04 = Constant Product (varg0, carg0)
05 = Variable Product (varg0, varg1)
06 = Constant Divide (varg0, carg0)
07 = Variable Divide (varg0, varg1)
08 = Constant Modulus (varg0, carg0)
09 = Variable Modulus (varg0, varg1)
10 = Constant Maximum (varg0, carg0)
11 = Variable Maximum (varg0, varg1)
12 = Constant Minimum (varg0, carg0)
13 = Variable Minimum (varg0, varg1)
14 = Constant Exponent (varg0, carg0)
15 = Varaible Exponent (varg0, varg1)
16 = Absolute (varg0)
17 = Mean (varg0, varg1)
18 = Constant Equals (varg0, carg0)
19 = Variable Equals (varg0, varg1)
20 = Constant More than (varg0, carg0)
21 = Variable More than (varg0, varg1)
22 = Constant Less than (varg0, carg0)
23 = Variable Less than (varg0, varg1)
24 = Input Node (carg0)

*/

/* IO Spec
In:
00 = Front Rangefinder
01 = Back Rangefinder

Out:
00 = Front Right Hip
01 = Front Right Knee
02 = Front Right Ankle
03 = Front Left Hip
04 = Front Left Knee
05 = Front Left Ankle
06 = Back Right Hip
07 = Back Right Knee
08 = Back Right Ankle
09 = Back Left Hip
10 = Back Left Knee
11 = Back Left Ankle

*/

class node {
    public:
    char operation;

    // Variable arguments
    node* varg0;
    node* varg1;

    // Constant arguments
    int carg0;

    int lastComputedValue;
    bool hasBeenRecomputed;
    
    node (node *v0, node *v1, int c, char op) {
    	varg0 = v0;
    	varg1 = v1;
    	carg0 = c;
    	operation = op;
    }
};

vector<int> inputs;
vector<int> outputValues;
vector<node*> outputs;
vector<node*> queue;
vector<node*> allNodes;

// Determine whether the queue already has a node registered with it
bool nodeAlreadyInQueue (node* n) {
    for (node* no : queue) if (no == n) return true;
    return false;
}

int *getValuesFrom (string nodestr) {
    int returnVal[4];
    int cinds[3];
    int cx = 0;
    for (int i = 0; i < nodestr.size(); i++) {
        if (nodestr[i] == ',') { cinds[cx] = i; cx++; }
        if (cx == 3) break;
    }

    //TODO
}

// Reset the computation state of the net ready for computation
void clearAllNodeComputations () {
    for (node* no : queue) no->hasBeenRecomputed = false;
}

// Find the immediate requirements for a node to be computed
node** getNodeFirstLevelRequirements (node* n) {
    node** out = new node*[2];
    if ((n->operation % 2) == 1) out[1] = n->varg1;
    else out[1] = NULL;
    if (n->operation != 24)
        out[0] = n->varg0;
    return out;
}

// Register the requirements for the computation of a node recursively with the queue, only as long as it is not already there
void registerNodeRequirements (node* n) {
    // Find out what we need
    node** req = getNodeFirstLevelRequirements (n);
    // Recursively register requirements with the queue
    if (req[0] != NULL) registerNodeRequirements (req[0]);
    if (req[1] != NULL) registerNodeRequirements (req[1]);

    // Finally, add ourselves to the queue
    if (!nodeAlreadyInQueue (n))
        queue.push_back (n);
}

// Compute a single node, returning status false for failure, true for success
bool computeNodeOnly (node* n) {
    // First work out what nodes we need
    node** requirements = getNodeFirstLevelRequirements (n);
    // Check if these have been computed yet
    if (requirements[0] != NULL && !requirements[0]->hasBeenRecomputed) return false;
    if (requirements[1] != NULL && !requirements[1]->hasBeenRecomputed) return false;

    // Grab the values quickly
    int v[2] = {requirements[0]->lastComputedValue, requirements[1]->lastComputedValue};
    int c = n->carg0;

    // Complete the operation
    int finalValue;
    switch (n->operation) {
    case 0:
        finalValue = v[0] + c;
        break;
    case 1:
        finalValue = v[0] + v[1];
        break;
    case 2:
        finalValue = v[0] - c;
        break;
    case 3:
        finalValue = v[0] - v[1];
        break;
    case 4:
        finalValue = v[0] * c;
        break;
    case 5:
        finalValue = v[0] * v[1];
        break;
    case 6:
        finalValue = v[0] / c;
        break;
    case 7:
        finalValue = v[0] / v[1];
        break;
    case 8:
        finalValue = v[0] % c;
        break;
    case 9:
        finalValue = v[0] % v[1];
        break;
    case 10:
        finalValue = (v[0] > c) ? v[0] : c;
        break;
    case 11:
        finalValue = (v[0] > v[1]) ? v[0] : v[1];
        break;
    case 12:
        finalValue = (v[0] < c) ? v[0] : c;
        break;
    case 13:
        finalValue = (v[0] < v[1]) ? v[0] : v[1];
        break;
    case 14:
        finalValue = pow (v[0], c);
        break;
    case 15:
        finalValue = pow (v[0], v[1]);
        break;
    case 16:
        finalValue = abs (v[0]);
        break;
    case 17:
        finalValue = (v[0] + v[1])/2;
        break;
    case 18:
        finalValue = (v[0] == c);
        break;
    case 19:
        finalValue = (v[0] == v[1]);
        break;
    case 20:
        finalValue = (v[0] > c);
        break;
    case 21:
        finalValue = (v[0] > v[1]);
        break;
    case 22:
        finalValue = (v[0] < c);
        break;
    case 23:
        finalValue = (v[0] < v[1]);
        break;
    case 24:
        finalValue = inputs[c];
        break;
    default:
        // Unsupported operation
        break;
    }

    // Set the node's computed status
    n->hasBeenRecomputed = true;
    n->lastComputedValue = finalValue;
    return true;
}

// Compute the entire net and dump the outputs it produces
void computeNetOutputs () {
    // Compute all nodes in order
    // TODO: Make this faster (concurrency, workers, etc)
    for (node* tbc : queue) {
        computeNodeOnly (tbc);
    }

    // Dump output values
    for (node* output : outputs) {
        outputValues.push_back (output->lastComputedValue);
    }

    // Reset ready for next time
    clearAllNodeComputations();
}

void updateComputationOrder () {
    queue.clear();
    for (node* output : outputs) {
        registerNodeRequirements (output);
    }
}


// TODO: Main
// TODO: Loading and saving
// TODO: Learning
// TODO: Real IO

void makeRandomNet () {
    node *i1 = new node (NULL, NULL, 0, 24);
    node *i2 = new node (NULL, NULL, 1, 24);
    allNodes.clear();
    allNodes.push_back (i1);
    allNodes.push_back (i2);
    vector<node*> tmp;
    tmp.push_back (i1);
    tmp.push_back (i2);
    for (int i = 0; i < 60; i++) {
    	node *x1 = tmp[random() % tmp.size()];
    	node *x2 = tmp[random() % tmp.size()];
        node *newN = new node (x1, x2, random () % 256, random () % 24);
    	tmp.push_back (newN);
        allNodes.push_back (newN);
    }
    for (int i = tmp.size()-12; i < tmp.size(); i++) {
    	outputs.clear ();
    	outputs.push_back (tmp[i]);
    }
}

int indexOfNodeInNet (node *n) {
    int i = -1;
    for (node *a : allNodes) {
        i++;
        if (n == a) break;
    }
    return i;
}

void writeToDisk (string filepath) {
    string fileContent = "<begin-element-nodestore>\n";
    for (node *n : allNodes) {
        string line = "";
        line += "{";
        line += to_string (n->operation);
        line += ",";
        line += to_string (n->carg0);
        line += ",";
        line += to_string (indexOfNodeInNet (n->varg0));
        line += ",";
        line += to_string (indexOfNodeInNet (n->varg1));
        line += "}";
        fileContent += line + "\n";
    }
    ofstream f (filepath);
    f << fileContent << "<end-element-nodestore>";
    f.close();
}

void readFromDisk (string filepath) {
    vector<string> fileContent;
    ifstream f (filepath);
    string t;
    f >> t;
    if (t != "<begin-element-nodestore>") return;
    while (t != "<end-element-nodestore>") {
        f >> t;
        fileContent.push_back (t);
    }
    fileContent.pop_back ();

    allNodes.clear();

    // First pass
    for (string nod : fileContent) {
        int *values = getValuesFrom (nod);
        allNodes.push_back (new node (NULL, NULL, values[1], values[0]));
    }

    // Second pass
    for (int i = 0; i < fileContent.size(); i++) {
        int *values = getValuesFrom (fileContent[i]);
        allNodes[i]->varg0 = allNodes[values[2]];
        allNodes[i]->varg1 = allNodes[values[3]];
    }
}

int main () {
    readFromDisk ("element.net");


    return 0;
    makeRandomNet ();
    writeToDisk ("element.net");

    return 0;

    updateComputationOrder();

    while (true) {
        // TODO: Update inputs
        computeNetOutputs();
        // TODO: Apply output
    }

    return 0;
}
