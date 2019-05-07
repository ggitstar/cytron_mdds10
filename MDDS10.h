/* library for Cytron SmartDrive Duo 10 (MDDS10)
 * see details https://www.robotshop.com/media/files/pdf2/mdds10_users_manual.pdf
 * 
 * this libaray only supports SERIAL SIMPLIFIED MODE/SERIAL PACKETIZED MODE
 
 *
*/
#ifndef MDDS10_H
#define MDDS10_H

#include "mbed.h"

enum{
    SIMPLIFIED,
    PACKETIZED
};
enum{
    FORWARD=0,
    REVERSE=1,
    M_RIGHT=1,
    M_LEFT=0
};

typedef struct{
    int speed;
    bool dir;
    uint8_t cmd;
}moter_cmd_t;

class MDDS10:public RawSerial,public DigitalIn{
public:
    MDDS10(PinName tx,PinName rx,uint32_t baud,int mode,uint8_t addr,PinName pwrpin);
    MDDS10(PinName tx,PinName rx,uint32_t baud,int mode,PinName pwrpin);
    void setRightMoter(int per);
    void setLeftMoter(int per);
    void setBothMoter(int r_per,int l_per);
    void cmdRightMoter(bool dir,uint8_t cmd);
    void cmdLeftMoter(bool dir,uint8_t cmd);
    void cmdBothMoter(bool rdir,uint8_t rcmd,bool ldir,uint8_t lcmd);
    int getRightPer();
    int getLeftPer();
    int getRightCmd();
    int getLeftCmd();
    int getMode();
    void setDrive();
    void setDrive(int,int);
    void setDrive(bool,uint8_t,bool,uint8_t);
    bool isReady();
    void demo();
private:
    // RawSerial _serial;
    int _mode;
    int _packetsize;
    int _step;
    int _checksum;
    uint8_t _addr;
    moter_cmd_t moter_t[2];
    uint8_t *_packets;
};
    
#endif