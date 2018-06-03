#ifndef PACKET_H
#define PACKET_H


struct Command {
    union {
        char Bytes[8];
        struct {
            unsigned short motor_H_G;
            unsigned short motor_H_D;
            unsigned short motor_B_G;
            unsigned short motor_B_D;
        };
    };
};


#endif // PACKET_H
