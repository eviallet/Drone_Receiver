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


/*
#include <QByteArray>

class Command {
public:
    unsigned short motor_H_G;
    unsigned short motor_H_D;
    unsigned short motor_B_G;
    unsigned short motor_B_D;

    static Command from_data(QByteArray bytes) {
        Command cmd;
        cmd.motor_H_G = bytes.left(sizeof(short)).toShort();
        cmd.motor_H_D = bytes.mid(sizeof(short),sizeof(short)).toShort();
        cmd.motor_B_G = bytes.mid(2*sizeof(short)).toShort();
        cmd.motor_B_D = bytes.right(sizeof(short)).toShort();

        return cmd;
    }
};
*/


#endif // PACKET_H
