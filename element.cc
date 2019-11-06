#include <stdio.h>
#include <vector>
#include <map>
#include <random>
#include <chrono>

#define CHARGE_DECREASE 0.001
#define CHARGE_THRESHOLD 64

using namespace std;

long long getNanos () {
    return chrono::high_resolution_clock::now();
}

namechars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890"

class structure {
public:
    void structure () {
        title = "";
        for (int i = 0; i < 256; i++) {
            title += namechars[random () % 64];
        }
        nanosAtLastUpdate = getNanos();
    }


    string title;
    map<string, int> outgoingConnections;

    int activeCharge = 0;
    long long nanosAtLastUpdate
    string instructionSequence;

    void update (int addingCharge = 0) {
        if (activeCharge > 0) {
            activeCharge -= (getNanos()-nanosAtLastUpdate)*CHARGE_DECREASE;
        }
        if (activeCharge < 0) activeCharge = 0;
        activeCharge += addingCharge;
        if (activeCharge >= CHARGE_THRESHOLD) {
            // TODO: execute
            activeCharge -= CHARGE_THRESHOLD;
        }
        nanosAtLastUpdate = getNanos();
    }
};

int main () {
    structure s = structure ();
}