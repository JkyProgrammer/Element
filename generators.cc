#include "element.h"

void handle_pc (int i) {
    // TODO: Handle actuators here
}

int mainloop_pc () {
    structurebuffer pcsb = structurebuffer (true);
    for (int i = 0; i < 5; i++) pcsb.addSensor ();
    for (int j = 0; j < 5; j++) pcsb.addMotor (handle_pc);


    while (true) { // Sensor mainloop
        // TODO: Pass data to sensors here
    }
}