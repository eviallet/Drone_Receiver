#ifndef PACKET_H
#define PACKET_H

#include <QDebug>
#include <QByteArray>
#include <math.h>


// IN DATA (with the first twp bytes being TYPE (SETPOINT or PIDPARAMS)

const static char SETPOINT = 's';
struct SetPoint {
    unsigned short speed;
    short yaw;
    short pitch;
    short roll;
    short checksum;
};

static SetPoint decode_setpoint(QByteArray bytes) {
    SetPoint sp;
    sp.speed = *reinterpret_cast<const short*>(bytes.mid(2,2).data());
    sp.yaw = *reinterpret_cast<const short*>(bytes.mid(4,2).data());
    sp.pitch = *reinterpret_cast<const short*>(bytes.mid(6,2).data());
    sp.roll = *reinterpret_cast<const short*>(bytes.mid(8,2).data());
    sp.checksum = *reinterpret_cast<const short*>(bytes.mid(10,2).data());

    return sp;
}

static bool check_setpoint_integrity(SetPoint sp) {
    return sp.checksum == sp.speed+sp.yaw+sp.pitch+sp.roll;
}




const static char PIDPARAMS = 'p';
struct PIDParams {
    float ykp;
    float yti;
    float ytd;
    float pkp;
    float pti;
    float ptd;
    float rkp;
    float rti;
    float rtd;
    float checksum;
};

static PIDParams decode_pid_params(QByteArray bytes) {
        PIDParams p;
        p.ykp = *reinterpret_cast<const float*>(bytes.mid(2,4).data());
        p.yti = *reinterpret_cast<const float*>(bytes.mid(6,4).data());
        p.ytd = *reinterpret_cast<const float*>(bytes.mid(10,4).data());
        p.pkp = *reinterpret_cast<const float*>(bytes.mid(14,4).data());
        p.pti = *reinterpret_cast<const float*>(bytes.mid(18,4).data());
        p.ptd = *reinterpret_cast<const float*>(bytes.mid(22,4).data());
        p.rkp = *reinterpret_cast<const float*>(bytes.mid(26,4).data());
        p.rti = *reinterpret_cast<const float*>(bytes.mid(30,4).data());
        p.rtd = *reinterpret_cast<const float*>(bytes.mid(34,4).data());
        p.checksum = *reinterpret_cast<const float*>(bytes.mid(38,4).data());

        return p;
}


static bool check_pid_params_integrity(PIDParams p) {
    return p.checksum == p.ykp+p.yti+p.ytd+p.pkp+p.pti+p.ptd+p.rkp+p.rti+p.rtd;
}




const static char SETTINGS = 't';
struct Settings {
    bool HG;
    bool HD;
    bool BG;
    bool BD;
    char checksum;
};

static Settings decode_settings(QByteArray bytes) {
    Settings t;
    t.HG = bytes.mid(1,1).data()[0]=='y';
    t.HD = bytes.mid(2,1).data()[0]=='y';
    t.BG = bytes.mid(3,1).data()[0]=='y';
    t.BD = bytes.mid(4,1).data()[0]=='y';
    t.checksum = bytes.mid(5,1).data()[0];

    return t;
}

static bool check_settings_integrity(Settings t) {
    char HG = t.HG?'y':'n';
    char HD = t.HD?'y':'n';
    char BG = t.BG?'y':'n';
    char BD = t.BD?'y':'n';
    char checksum = HG+HD+BG+BD;
    return t.checksum==checksum;
}


// OUT DATA

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
