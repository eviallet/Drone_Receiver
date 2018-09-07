#ifndef PACKET_H
#define PACKET_H

#include <QDebug>
#include <QByteArray>
#include <math.h>

//#define SENSOR_THRESHOLD 0.75    // relative variation before updating remote graph

struct Command {
    union {
        char Bytes[8];
        struct {
            unsigned short speed;
            unsigned short yaw;
            unsigned short pitch;
            unsigned short roll;
        };
    };
};

struct SensorData {
    float alt;
    float yaw;
    float pitch;
    float roll;
};

// https://stackoverflow.com/a/2774014/83085
static QByteArray sensor_to_bytes(SensorData s) {
    QByteArray bytes;
    bytes = bytes
            .append(reinterpret_cast<const char*>(&s.alt), sizeof(float))
            .append(reinterpret_cast<const char*>(&s.yaw), sizeof(float))
            .append(reinterpret_cast<const char*>(&s.pitch), sizeof(float))
            .append(reinterpret_cast<const char*>(&s.roll), sizeof(float));
    return bytes;
}

/*
// returns true if s changed enough since last
static bool sensor_variation(SensorData s, SensorData last) {
    //double altVar = abs(s.alt-last.alt)/s.alt;
    double altVar = 0;
    double yawVar = abs(s.yaw-last.yaw)/s.yaw;
    double pitchVar = abs(s.pitch-last.pitch)/s.pitch;
    double rollVar = abs(s.roll-last.roll)/s.roll;

    return altVar>
SENSOR_THRESHOLD || yawVar>SENSOR_THRESHOLD || pitchVar>SENSOR_THRESHOLD || rollVar>SENSOR_THRESHOLD;
}
*/

#endif // PACKET_H
