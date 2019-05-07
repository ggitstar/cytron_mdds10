#include "MDDS10.h"

MDDS10::MDDS10(PinName tx, PinName rx, uint32_t baud, int mode, uint8_t addr,PinName pwrpin) :RawSerial(tx,rx,baud),DigitalIn(pwrpin)
{
    _addr = addr & 0x07;
    _packetsize = 4;
    _mode = mode;
    _step = 127;
    _checksum=0;
    _packets = new uint8_t[4];
}
MDDS10::MDDS10(PinName tx, PinName rx, uint32_t baud, int mode,PinName pwrpin) :RawSerial(tx,rx,baud),DigitalIn(pwrpin)
{
    _packetsize = 1;
    _mode = mode;
    _step = 63;
    _packets = new uint8_t[1];
}

void MDDS10::setRightMoter(int per)
{
    if (per < 0){
        moter_t[M_RIGHT].dir = 1;
        moter_t[M_RIGHT].cmd = abs(per) * _step / 100; 
    }
    else{
        moter_t[M_RIGHT].dir = 0;
        // moter_t[M_RIGHT].cmd = 100-per * _step / 100;
        moter_t[M_RIGHT].cmd = (_mode?100-per:per) * _step / 100;
    }
}
void MDDS10::setLeftMoter(int per)
{
    if (per < 0){
        moter_t[M_LEFT].dir = 1;
        moter_t[M_LEFT].cmd = abs(per) * _step / 100;
    }
    else{
        moter_t[M_LEFT].dir = 0;
        // moter_t[M_LEFT].cmd = 100-per * _step / 100;
        moter_t[M_LEFT].cmd = (_mode?100-per:per) * _step / 100;
    }
}

void MDDS10::setBothMoter(int r_per, int l_per)
{
    setRightMoter(r_per);
    setLeftMoter(l_per);
}
void MDDS10::cmdRightMoter(bool dir, uint8_t cmd)
{
    moter_t[M_RIGHT].dir = dir;
    moter_t[M_RIGHT].cmd = cmd;
}
void MDDS10::cmdLeftMoter(bool dir, uint8_t cmd)
{
    moter_t[M_LEFT].dir = dir;
    moter_t[M_LEFT].cmd = cmd;
}
void MDDS10::cmdBothMoter(bool rdir, uint8_t rcmd, bool ldir, uint8_t lcmd)
{
    moter_t[M_RIGHT].dir = rdir;
    moter_t[M_RIGHT].cmd = rcmd;
    moter_t[M_LEFT].dir = ldir;
    moter_t[M_LEFT].cmd = lcmd;
}
void MDDS10::setDrive()
{
    switch (_mode)
    {
    case SIMPLIFIED:
        for (int ch = 0; ch < 2; ch++)
        {
            _packets[0] = (ch << 7) | (moter_t[ch].dir << 6) | (moter_t[ch].cmd);
            for (int i = 0; i < _packetsize; i++)
                RawSerial::putc(_packets[i]);
                // _serial.putc(_packets[i]);
        }
        break;
    case PACKETIZED:
        _packets[0] = 85;
        for (int ch = 0; ch < 2; ch++)
        {
            _packets[1] = (ch << 3) | _addr;
            _packets[2] = (moter_t[ch].dir << 7) | (moter_t[ch].cmd);
            _checksum=0;
            for(int i=0;i<3;i++){
                _checksum+=(int)(_packets[i]);
            }
            _packets[3] = (uint8_t)(_checksum);
            for (int i = 0; i < _packetsize; i++)
                RawSerial::putc(_packets[i]);
                // _serial.putc(_packets[i]);
        }
        break;
    default:
        break;
    }
}
void MDDS10::setDrive(int r_per, int l_per){
    setBothMoter(r_per,l_per);
    setDrive();
}
void MDDS10::setDrive(bool rdir, uint8_t rcmd, bool ldir, uint8_t lcmd){
    cmdBothMoter(rdir,rcmd,ldir,lcmd);
    setDrive();
}
int MDDS10::getRightPer()
{
    moter_t[M_RIGHT].speed = (int)(moter_t[M_RIGHT].cmd) * 100 / _step;
    if (moter_t[M_RIGHT].dir)
        moter_t[M_RIGHT].speed *= -1;
    return moter_t[M_RIGHT].speed;
}
int MDDS10::getLeftPer()
{
    moter_t[M_LEFT].speed = (int)(moter_t[M_LEFT].cmd) * 100 / _step;
    if (moter_t[M_LEFT].dir)
        moter_t[M_LEFT].speed *= -1;
    return moter_t[M_LEFT].speed;
}
int MDDS10::getRightCmd()
{
    return moter_t[M_RIGHT].dir ? (-1) * (int)(moter_t[M_RIGHT].cmd) : (int)(moter_t[M_RIGHT].cmd);
}
int MDDS10::getLeftCmd()
{
    return moter_t[M_LEFT].dir ? (-1) * (int)(moter_t[M_LEFT].cmd) : (int)(moter_t[M_LEFT].cmd);
}
int MDDS10::getMode()
{
    return _mode;
}
bool MDDS10::isReady(){
    if(_mode==SIMPLIFIED)return 1;
    if(DigitalIn::read()==1){
        wait_ms(1000);
        RawSerial::putc(85);    //dummy byte auto baud settting
        return 1;
    }
    return 0;
}
void MDDS10::demo(){
    RawSerial::putc(85);
    RawSerial::putc(0);
    RawSerial::putc(63);
    RawSerial::putc(85+63);
    wait_ms(2000);
    RawSerial::putc(85);
    RawSerial::putc(0);
    RawSerial::putc(192);
    RawSerial::putc(85+92);
    wait_ms(2000);
    RawSerial::putc(85);
    RawSerial::putc(0);
    RawSerial::putc(127);
    RawSerial::putc(85+127);
    wait_ms(1000);
    RawSerial::putc(85);
    RawSerial::putc(8);
    RawSerial::putc(63);
    RawSerial::putc(85+8+63);
    wait_ms(2000);
    RawSerial::putc(85);
    RawSerial::putc(8);
    RawSerial::putc(192);
    RawSerial::putc(85+8+192);
    wait_ms(2000);
    RawSerial::putc(85);
    RawSerial::putc(8);
    RawSerial::putc(127);
    RawSerial::putc(85+8+127);
    wait_ms(1000);
}