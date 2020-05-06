#include "elementv5.h"
bool hasOperationsLeft (string s) {
    for (char c : s) {
        if (c == '+') return true;
        if (c == '-') return true;
        if (c == '*') return true;
        if (c == '/') return true;
        if (c == '^') return true;
        if (c == '%') return true;
        if (c == '$') return true;
        if (c == '!') return true;
    }
    return false;
}

int factorial(int n) { 
    return (n==1 || n==0) ? 1: n * factorial(n - 1);  
} 

int evaluate (string s) {
    string fixed = s;

    int result;

    int cl = 0;
    int bracketDepth = 0;
    int brackStart = -1;
    while (cl < s.size()) {
        if (fixed[cl] == '(') {
            bracketDepth++;
            if (bracketDepth == 1) brackStart = cl;
        }
        else if (fixed[cl] == ')') {
            bracketDepth--;
            if (bracketDepth == 0) {
                int res = evaluate (fixed.substr (brackStart+1, cl-1));
                fixed.replace (brackStart, cl, to_string(res));
                brackStart = -1;
            }
            if (bracketDepth < 0) return rand();
        }
    }
    if (bracketDepth != 0) return rand();


    while (hasOperationsLeft(fixed)) {
        string op1 = 0;
        string op2 = 0;
        int operation = -1;
        cl = 0;
        while (cl < s.size()) {
            if (fixed[cl] == '+') {
                if (operation != -1) break;
                operation = 0;
            }
            else if (fixed[cl] == '-') {
                if (operation != -1) break;
                operation = 1;
            }
            else if (fixed[cl] == '*') {
                if (operation != -1) break;
                operation = 2;
            }
            else if (fixed[cl] == '/') {
                if (operation != -1) break;
                operation = 3;
            }
            else if (fixed[cl] == '^') {
                if (operation != -1) break;
                operation = 4;
            }
            else if (fixed[cl] == '%') {
                if (operation != -1) break;
                operation = 5;
            }
            else if (fixed[cl] == '$') {
                if (operation != -1) break;
                operation = 6;
            }
            else if (fixed[cl] == '!') {
                if (operation != -1) break;
                operation = 7;
            }
            else {
                if (operation == -1) op1 += fixed[cl];
                else if (operation == 7 || operation == 6) break;
                else op2 += fixed[cl];
            }
        }
        if (operation == 0) {
            fixed.replace (0, cl-1, to_string (stoi (op1) + stoi (op2)));
        } else if (operation == 1) {
            fixed.replace (0, cl-1, to_string (stoi (op1) - stoi (op2)));
        } else if (operation == 2) {
            fixed.replace (0, cl-1, to_string (stoi (op1) * stoi (op2)));
        } else if (operation == 3) {
            fixed.replace (0, cl-1, to_string (stoi (op1) / stoi (op2)));
        } else if (operation == 4) {
            fixed.replace (0, cl-1, to_string (pow(stoi (op1), stoi (op2))));
        } else if (operation == 5) {
            fixed.replace (0, cl-1, to_string (stoi (op1) % stoi (op2)));
        } else if (operation == 6) {
            fixed.replace (0, cl-1, to_string (sin (stoi (op1)*M_PI/180)));
        } else if (operation == 7) {
            fixed.replace (0, cl-1, to_string (factorial (stoi (op1))));
        }
    }

    return stoi (fixed);
}

void Node::compute() {
    vector<char> input;
    int inputN = -1;
    for (Node *req : requirements) {
        inputN++;
        if (!req->computedThisCycle) req->compute();
        input.push_back(req->value);
    }
    string str = operation;
    for (char c = 'a'; c <= 'z'; c++) {
        if (c-'a' >= input.size()) break;
        size_t s = str.find(c);
        if (s < 0) continue;
        str.replace (s, s, to_string(input[c-'a']));
    }
    string sstr = "";
    for (char c : str) {
        if (c != ' ') sstr += c;
    }
    value = evaluate (str);
    computedThisCycle = true;
}

Node::Node (string op) {
    operation = op;
    operation.shrink_to_fit();
    requirements.shrink_to_fit();
}

Node::Node () {
    regenerateOperation();
    requirements.shrink_to_fit();
}

void clearComputedFlags () {
    for (Node *n : nodes) {
        n->computedThisCycle = false;
    }
}

void Node::regenerateOperation () {
    int numInputs = requirements.size();

    string oper;

    int n = 8;
    if (numInputs > 1) n = 6;
    int op = rand () % n;

    for (int i = 0; i < numInputs-1; i++) {
        oper += ('a' + i);
        oper += to_string(op);
    }

    operation = oper;
    operation.shrink_to_fit();
}

void Node::addInput (Node *n, bool regen) {
    if (requirements.size() >= 26) return;
    requirements.push_back (n);
    requirements.shrink_to_fit();
    if (regen) regenerateOperation();
}

// void generateNodes () {
//     cout << "Generated node pattern..." << endl;
//     buffer.clear();
//     sensors.clear();
//     motors.clear();
//     for (int i = 0; i < NUM_INPUTS; i++) {
//         structure *s = new structure (this);
//         buffer.push_back (s);
//         sensors.push_back (s);
//     }

//     for (int i = 0; i < NUM_OUTPUTS; i++) {
//         structure *s = new structure (this, i);
//         buffer.push_back (s);
//         motors.push_back (s);
//     }

//     for (int i,o = 0; i < NUM_INPUTS; i++) {
//         if (o == motors.size()) o = 0;
//         sensors[i]->addLink (motors[o]);
//     }

//     int numRelays = 0;
//     while (numRelays <= NUM_RELAYS) {
//         structure *o = buffer[rand() % buffer.size()];
//         if (o->isMotor()) continue;
//         int num = o->numLinks() > 0;
//         if (num) {
//             int connectionNum = rand () % o->numLinks();
//             structure *r = new structure (this);
//             r->addLink (o->links[connectionNum]);
//             o->links[connectionNum] = r;
//         }
//         if (!(rand() % 3)) {
//             o->addLink (buffer[(rand() % buffer.size()-(1 + NUM_INPUTS)) + NUM_INPUTS]);
//         }
//         numRelays++;
//     }
//     cout << "Done." << endl;
// }


bool mazeData[MAZE_SIZE][MAZE_SIZE] = {false};
int entranceX, entranceY, exitX, exitY;
int xPos, yPos;
void generateMaze () {
    for (int i = 0; i < MAZE_SIZE; i++) {
        for (int j = 0; j < MAZE_SIZE; j++) {
            mazeData[i][j] = true;
        }
    }
    
    int x = (rand () % (MAZE_SIZE - 2)) + 1;
    int y = (rand () % (MAZE_SIZE - 2)) + 1;
    int direction = rand () % 4;

    for (int it = 0; it < MAZE_ITS; it++) {
        int op = rand () % 6;
        if (op == 4) {
            direction++;
            if (direction > 3) direction = 0;
        } else if (op == 5) {
            direction--;
            if (direction < 0) direction = 3;
        } else {
            int nx = x;
            int ny = y;
            if (direction == 0) ny++;
            if (direction == 1) nx++;
            if (direction == 2) ny--;
            if (direction == 3) nx--;
            if (nx != 0 && nx != MAZE_SIZE-1 && ny != 0 && ny != MAZE_SIZE-1) {
                x = nx;
                y = ny;
                mazeData[y][x] = false;
            } else {
                direction++;
                if (direction > 3) direction = 0; 
            }
        }
    }

    entranceX = rand () % MAZE_SIZE;
    entranceY = rand () % MAZE_SIZE;
    while (mazeData[entranceY][entranceX]) {
        entranceX = rand () % MAZE_SIZE;
        entranceY = rand () % MAZE_SIZE;
    }
    exitX = rand () % MAZE_SIZE;
    exitY = rand () % MAZE_SIZE;
    while (mazeData[exitY][exitX] || (abs (exitY-entranceY) < 4) || abs (exitX-entranceX) < 4) {
        exitX = rand () % MAZE_SIZE;
        exitY = rand () % MAZE_SIZE;
    }
}

void displayMaze () {
    cout.flush();
    system("clear");

    for (int i = 0; i < MAZE_SIZE; i++) {
        for (int j = 0; j < MAZE_SIZE; j++) {
            if (i == yPos && j == xPos) cout << "+";
            else if (mazeData[i][j]) cout << "0";
            else cout << " ";
        }
        cout << endl;
    }
}

// TODO: Maze overview behaviour training

int main() {
    generateMaze();
    xPos = entranceX;
    yPos = entranceY;

    // TODO: GEnerate nodes
    //generateNodes();
    
    chrono::milliseconds timespan(1000); // or whatever

    while (true) {
        displayMaze();
        this_thread::sleep_for(timespan);
    }

    return 0;
}