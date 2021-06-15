#ifndef NODE
#define NODE

#include <map>

class node {
public:
    std::map<char, double> slots;
    bool calculated;
    int calculatedValue;

    int calculate ();
};

#endif