#include "sensor.h"

Sensor::Sensor(QObject *parent) : QThread(parent) {
    setup();
    usleep(100000);
}

void Sensor::loop() {
    // if programming failed, don't try to do anything
    if (dmpReady) {
        // get current FIFO count
        fifoCount = mpu.getFIFOCount();

        if (fifoCount == 1024) {
            // reset so we can continue cleanly
            mpu.resetFIFO();
            printf("FIFO overflow!\n");

        // otherwise, check for DMP data ready interrupt (this should happen frequently)
        } else if (fifoCount >= 42) {
            // read a packet from FIFO
            mpu.getFIFOBytes(fifoBuffer, packetSize);

            #ifdef OUTPUT_READABLE_QUATERNION
                // display quaternion values in easy matrix form: w x y z
                mpu.dmpGetQuaternion(&q, fifoBuffer);
                printf("quat %7.2f %7.2f %7.2f %7.2f    ", q.w,q.x,q.y,q.z);
            #endif

            #ifdef OUTPUT_READABLE_EULER
                // display Euler angles in degrees
                mpu.dmpGetQuaternion(&q, fifoBuffer);
                mpu.dmpGetEuler(euler, &q);
                printf("euler %7.2f %7.2f %7.2f    ", euler[0] * 180/M_PI, euler[1] * 180/M_PI, euler[2] * 180/M_PI);
            #endif

            #ifdef OUTPUT_READABLE_YAWPITCHROLL
                // display Euler angles in degrees
                mpu.dmpGetQuaternion(&q, fifoBuffer);
                mpu.dmpGetGravity(&gravity, &q);
                mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
                //printf("t %ld ypr  %7.2f %7.2f %7.2f    ", time(0), ypr[0] * 180/M_PI, ypr[1] * 180/M_PI, ypr[2] * 180/M_PI);
                ypr[0]*=180/M_PI;
                ypr[1]*=180/M_PI;
                ypr[2]*=180/M_PI;
            #endif

            #ifdef OUTPUT_READABLE_REALACCEL
                // display real acceleration, adjusted to remove gravity
                mpu.dmpGetQuaternion(&q, fifoBuffer);
                mpu.dmpGetAccel(&aa, fifoBuffer);
                mpu.dmpGetGravity(&gravity, &q);
                mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
                printf("areal %6d %6d %6d    ", aaReal.x, aaReal.y, aaReal.z);
            #endif

            #ifdef OUTPUT_READABLE_WORLDACCEL
                // display initial world-frame acceleration, adjusted to remove gravity
                // and rotated based on known orientation from quaternion
                mpu.dmpGetQuaternion(&q, fifoBuffer);
                mpu.dmpGetAccel(&aa, fifoBuffer);
                mpu.dmpGetGravity(&gravity, &q);
                mpu.dmpGetLinearAccelInWorld(&aaWorld, &aaReal, &q);
                printf("aworld %6d %6d %6d    ", aaWorld.x, aaWorld.y, aaWorld.z);
            #endif

            #ifdef OUTPUT_TEAPOT
                // display quaternion values in InvenSense Teapot demo format:
                teapotPacket[2] = fifoBuffer[0];
                teapotPacket[3] = fifoBuffer[1];
                teapotPacket[4] = fifoBuffer[4];
                teapotPacket[5] = fifoBuffer[5];
                teapotPacket[6] = fifoBuffer[8];
                teapotPacket[7] = fifoBuffer[9];
                teapotPacket[8] = fifoBuffer[12];
                teapotPacket[9] = fifoBuffer[13];
                Serial.write(teapotPacket, 14);
                teapotPacket[11]++; // packetCount, loops at 0xFF on purpose
            #endif
            printf("\n");
        }
    }
}

void Sensor::run() {
    while(true) {
        loop();
    }
}

std::tuple<float, float, float> Sensor::get_angles() {
    return std::make_tuple(ypr[0], ypr[1], ypr[2]);
}

void Sensor::setup() {
    // initialize device
    printf("Initializing I2C devices...\n");
    mpu.initialize();

    // verify connection
    printf("Testing device connections...\n");
    printf(mpu.testConnection() ? "MPU6050 connection successful\n" : "MPU6050 connection failed\n");

    // load and configure the DMP
    printf("Initializing DMP...\n");
    devStatus = mpu.dmpInitialize();

    // make sure it worked (returns 0 if so)
    if (devStatus == 0) {
        // turn on the DMP, now that it's ready
        printf("Enabling DMP...\n");
        mpu.setDMPEnabled(true);

        // enable Arduino interrupt detection
        //Serial.println(F("Enabling interrupt detection (Arduino external interrupt 0)..."));
        //attachInterrupt(0, dmpDataReady, RISING);
        mpuIntStatus = mpu.getIntStatus();

        // set our DMP Ready flag so the main loop() function knows it's okay to use it
        printf("DMP ready!\n");
        dmpReady = true;

        // get expected DMP packet size for later comparison
        packetSize = mpu.dmpGetFIFOPacketSize();
    } else {
        // ERROR!
        // 1 = initial memory load failed
        // 2 = DMP configuration updates failed
        // (if it's going to break, usually the code will be 1)
        printf("DMP Initialization failed (code %d)\n", devStatus);
    }
}
