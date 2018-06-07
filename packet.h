#ifndef PACKET_H
#define PACKET_H


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


#endif // PACKET_H
