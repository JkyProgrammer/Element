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

// Input feeds: 12310
// 12288 (1 camera, 64 * 64 pixels with 3 colour based triggers per pixel)
// 12 pressure sensors (1 on each foot with 3 levels of pressure)
// 10 infrared distance sensors (2, one on each side, both facing forwards, each with 5 levels of sensitivity)

// Output feeds: 96
// 24 servos (4 hips, 4 knees, 4 ankles, with a feed for going in each direction)
// 72 (12 neopixel ring with on and off feeds for red, green, and blue)


void handle_quad (int i) {
    // TODO: Handle actuators here
    if (i < 24) { // It's a servo
        int servoNum = i / 2;
        int odd = i % 2;
        int pin = servoPins[servoNum];
        if (odd) {
            servoPWMs[servoNum] -= 8;
            if (servoPWMs[servoNum] < 0) servoPWMs[servoNum] = 0;
        } else {
            servoPWMs[servoNum] += 8;
            if (servoPWMs[servoNum] > 1023) servoPWMs[servoNum] = 1023;
        }
        system ("gpio pwm " + string(pin) + " " + string (servoPWMs[servoNum]));
    }
}

int[] servoPins = {5, 6, 12, 13, 17, 20, 21, 22, 23, 24, 25, 27};
int[12] servoPWMs = {512};

int mainloop_quad () {
    structurebuffer quadsb = structurebuffer (true, "quad.sb");
    if (quadsb.buffer.size() < 1) {
        // TODO: Make a proper net
        // Generate a new net
        for (int i = 0; i < 10; i++) quadsb.addSensor (makeStructure(quadsb)); // For now only the 10 distance sensors have been implemented
        for (int j = 0; j < 24; j++) quadsb.addMotor (handle_quad); // For now only the 24 servos have been implemented
    }
    
    system ("gpio mode 5 pwm");
    system ("gpio mode 6 pwm");
    system ("gpio mode 12 pwm");
    system ("gpio mode 13 pwm");
    system ("gpio mode 17 pwm");
    system ("gpio mode 20 pwm");
    system ("gpio mode 21 pwm");
    system ("gpio mode 22 pwm");
    system ("gpio mode 23 pwm");
    system ("gpio mode 24 pwm");
    system ("gpio mode 25 pwm");
    system ("gpio mode 27 pwm");

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
