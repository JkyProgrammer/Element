#include "node.h"

int node::calculate () {
    if (calculated) calculatedValue;

    int eTot = 0;
    int eNum = 0;

    int oTot = 0;
    int oNum = 0;

    for (auto p : slots) {
        if (p.first % 2) {
            oTot++;
            oNum += p.second;
        } else {
            eTot++;
            eNum += p.second;
        }
    }
    
    int eAvg = eNum ? eTot/eNum : 0;
    int oAvg = oNum ? oTot/oNum : 0;

    calculatedValue = (eAvg*oAvg)/(eAvg+oAvg);
    calculated = true;

    slots.clear();

    return calculatedValue;
}