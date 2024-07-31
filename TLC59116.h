#include "Arduino.h"
#include <Wire.h>
#include "TLC59116_REGDEF.h"

// Auto increment bit masks:
#define TLC59116_AI_NONE 0b00000000
#define TLC59116_AI_ALL 0b10000000
#define TLC59116_AI_PWM 0b10100000
#define TLC59116_AI_GRP 0b11000000
#define TLC59116_AI_PWM_GRP 0b11100000

class TLC59116
{
  private:
    unsigned char addr;
    //const unsigned short modeon[3] = {0b01, 0b10, 0b11};
    unsigned short offon[2] = {0b00, 0b01};  // {OFF, ON}
    union LStype
    {
       unsigned char LSbyte[4];
       unsigned int LS;
    } LS;
    unsigned char read_byte(unsigned char regbyte);
    void set_bit(unsigned int *number, unsigned char n, bool x);
    void write_byte(unsigned char regbyte, unsigned char databyte);
    void write_LS();
    void write_PWMALL(unsigned char value);
    void write_IREFALL(unsigned char value);
  public:
    static unsigned const char SOLID=0b01, PWM0=0b10, PWM1=0b11;
    // PCA9955(unsigned char address, unsigned char display_type);
    TLC59116(unsigned char address);
    void mode(unsigned char nmode);
    void set_leds(unsigned short leds);
    // void set(unsigned char number, unsigned char decimal);
    void clear();
    void dutycycle(unsigned char ontime);
    void dutycycle(unsigned char nled, unsigned char ontime);
    void period(unsigned char prescaler);
    void set_allcall(unsigned char allcalladr);
    void clear_allcall();
    unsigned char get_addr();
};
