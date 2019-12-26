#include "element.h"

void handle_pc (int i) {
    // TODO: Handle actuators here
}

int mainloop_pc () {
    structurebuffer pcsb = structurebuffer (true);
    for (int i = 0; i < 5; i++) pcsb.addSensor (new structure(pcsb.q));
    for (int j = 0; j < 5; j++) pcsb.addMotor (handle_pc);


    while (true) { // Sensor mainloop
        // TODO: Pass data to sensors here
    }
}

// ======== CODE FOR QUAD ========

// Input feeds: 12300
// 12288 (1 camera, 64 * 64 pixels with 3 colour based triggers per pixel)
// 12 pressure sensors (1 on each foot with 3 levels of pressure)

// Output feeds: 96
// 24 servos (4 hips, 4 knees, 4 ankles, with a feed for going in each direction)
// 72 (12 neopixel ring with on and off feeds for red, green, and blue)


void handle_quad (int i) {
    // TODO: Handle actuators here
}

int mainloop_quad () {
    structurebuffer quadsb = structurebuffer (true, "quad.sb");
    if (quadsb.buffer.size < 1) {
        // Generate a new net
        for (int i = 0; i < 12300; i++) quadsb.addSensor (makeStructure(quadsb));
        for (int j = 0; j < 96; j++) quadsb.addMotor (handle_quad);
    }

    int saveWait = 0;
    while (true) { // Sensor mainloop
        // TODO: Pass data to the 12 thousand sensors here (thats gonna be fun implementing)
        saveWait++;
        if (saveWait >= SAVE_LOOP_WAIT) {
            saveWait = 0;
            quadsb.writeOut ("quad.sb");
        }
    }
}