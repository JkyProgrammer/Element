#include <stdio.h>
#include <vector>
#include <map>
#include <random>

using namespace std;

namechars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890"

class structure {
public:
    void structure () {
        title = "";
        for (int i = 0; i < 256; i++) {
            title += namechars[random () % 64];
        }
    }


    string title;
    map<string, int> outgoingConnections;

    vector<bool> incomingConnections;
    string connectionHandling;
    string instructionSequence;

    void update () {
        // Parse connnectionHandling and put int all the values from incomingConnections
    }

    void execute () {
        // Execute the instructionSequence attached
    }

    int addConnection (int ctype) {
        if (ctype == 0) {
            connectionHandling += "|" + incomingConnections.size; // OR
        } else if (ctype == 1) {
            connectionHandling += "&" + incomingConnections.size; // AND
        } else if (ctype == 2) {
            connectionHandling += "^" + incomingConnections.size; // UNLESS
        }
        incomingConnections.push_back (false);
        return incomingConnections.size-1;
    }
};

int main () {
    structure s = structure ();
}