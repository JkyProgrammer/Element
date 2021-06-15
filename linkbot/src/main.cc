#include "link.h"
#include "node.h"

#include <vector>

std::vector<link> linkBank;
std::map<int, node> nodeBankPrimary;
std::map<int, node> nodeBankSecondary;
std::vector<int> inputs;
std::vector<int> outputs;
bool altCycle = false;

int main () {
    while (true) {
        // TODO: Send inputs to active nodebank
        
        // TODO: Multithreading
        for (link l : linkBank) {
            // Get calculated values from both ends of the link, acting on the active nodebank
            int v1 = (altCycle ? nodeBankSecondary : nodeBankPrimary).at (l.aNode).calculate();
            int v2 = (altCycle ? nodeBankSecondary : nodeBankPrimary).at (l.bNode).calculate();
            // Calculate the difference between them
            int r = v1 - v2;
            // Apply that value to the relevant node on the inactive nodebank
            if (r > 0) (altCycle ? nodeBankPrimary : nodeBankSecondary).at(l.bNode).slots.emplace (l.bSlot, r);
            if (r < 0) (altCycle ? nodeBankPrimary : nodeBankSecondary).at(l.aNode).slots.emplace (l.aSlot, -r);
        }

        for (auto n : (altCycle ? nodeBankSecondary : nodeBankPrimary)) n.second.calculated = false;

        altCycle = !altCycle;
        // TODO: Read outputs from active nodebank
    }


    return 0;
}


// TODO: Create initial net
// TODO: Mutate function