#include "TLC59116.h"
// #define PRINTBIN(Num) for (uint32_t t = (1UL<< (sizeof(Num)*8)-1); t; t >>= 1) Serial.write(Num  & t ? '1' : '0'); // Prints a binary number with leading zeros (Automatic Handling)

TLC59116::TLC59116(unsigned char address)
{
  Wire.begin(21,22);
  addr = address;
  // if (display_type==LARGE)
  //   Seg2LED = Seg2LED_large;
  // else if (display_type==SMALL)
  //   Seg2LED = Seg2LED_small;
  mode(PWM0);
  //write_byte(TLC59116_GRPPWM, 0xFF);
  //write_byte(TLC59116_IREF, 0x16); // Imax = 10mA @ Rext = 1k
  write_byte(TLC59116_MODE1,0b00000001); // 000 read-only, 0 no sleep, 000 no subaddresses, 1 respond to All Call
  write_byte(TLC59116_MODE2,0b00001000); // 0 enable error status flag, 0 reserved, 0 GRP dimming, 0 reserved, 0 outputs change on ACK, 000 reserved

}

void TLC59116::mode(unsigned char nmode)
{
  offon[1] = nmode;
}

void TLC59116::write_byte(unsigned char regbyte, unsigned char databyte)
{
  Wire.beginTransmission(addr); // transmit to device #addr
  Wire.write(byte(regbyte));            // sends instruction byte
  Wire.write(byte(databyte));             // sends value byte
  Wire.endTransmission();     // stop transmitting
}

void TLC59116::write_LS()
{
  Wire.beginTransmission(addr); // transmit to device #addr
  Wire.write(byte(TLC59116_LEDOUT0 | TLC59116_AI_ALL)); // sends instruction byte including auto increment set
  Wire.write(byte(LS.LSbyte[0])); // sends value bytes
  Wire.write(byte(LS.LSbyte[1]));
  Wire.write(byte(LS.LSbyte[2]));
  Wire.write(byte(LS.LSbyte[3]));
  Wire.endTransmission();     // stop transmitting
}

void TLC59116::write_GRPPWM(unsigned char value)
{
  Wire.beginTransmission(addr); // transmit to device #addr
  Wire.write(byte(TLC59116_GRPPWM)); // sends instruction byte
  Wire.write(byte(value)); // sends value byte
  Wire.endTransmission();     // stop transmitting
}

void TLC59116::write_IREFALL(unsigned char value)
{
  Wire.beginTransmission(addr); // transmit to device #addr
  Wire.write(byte(TLC59116_IREF)); // sends instruction byte including auto increment set
  Wire.write(byte(value)); // sends value bytes
  Wire.endTransmission();     // stop transmitting
}

unsigned char TLC59116::read_byte(unsigned char regbyte)
{
  Wire.beginTransmission(addr);
  Wire.write(byte(regbyte));
  Wire.endTransmission(false);
  Wire.requestFrom(addr, (byte)1);
  unsigned char databyte = Wire.read();
  return databyte;
}

// Function set_bit sets a bit in a number pointed to:
// - The number to be worked on is represented by a pointer to it, type is unsigned int*
// - The bit to be changed is given by n, tyoe is an unsigned char; LSB is 0
// - The target state of the bit is x, type is boolean
void TLC59116::set_bit(unsigned int *number, unsigned char n, bool x)
{
  unsigned int newbit = !!x;    // Booleanize to force 0 or 1
  *number ^= (-newbit ^ *number) & (1UL << n);
}

void TLC59116::set_leds(unsigned short leds)
{
  // Serial.println(bitRead(leds,0));
  // Serial.println(offon[1]);

  for (int n=0; n<16; n++)
  {
    set_bit(&LS.LS, 2*n, offon[bitRead(leds,n)] & 1);
    set_bit(&LS.LS, 2*n+1, offon[bitRead(leds,n)] & 2);
  }
  // PRINTBIN(LS.LS);
  // Serial.println();
  write_LS();
}

// void PCA9955::set(unsigned char number, unsigned char decimal)
// {
//   set_segments(Numbers[number]+decimal);
// }

void TLC59116::clear()
{
  set_leds(0);
}

void TLC59116::dutycycle(unsigned char ontime)
{
    Wire.beginTransmission(addr); // transmit to device #addr
    Wire.write(byte(TLC59116_PWMX_OFFSET | TLC59116_AI_ALL)); // sends instruction byte including auto increment set
    for (int n=0; n<16; n++)
      Wire.write(byte(ontime)); // sends value bytes
    Wire.endTransmission();     // stop transmitting
}

void TLC59116::dutycycle(unsigned char nled, unsigned char ontime)
{
  write_byte(TLC59116_PWMX_OFFSET + nled, ontime);
}

void TLC59116::period(unsigned char prescaler)
{
    write_byte(TLC59116_GRPFREQ, prescaler);
}

void TLC59116::set_allcall(unsigned char allcalladr)
{
  write_byte(TLC59116_ALLCALLADR, allcalladr);
  write_byte(TLC59116_MODE1, read_byte(TLC59116_MODE1) | 0x01);
}

void TLC59116::clear_allcall()
{
  write_byte(TLC59116_MODE1, read_byte(TLC59116_MODE1) & 0xFE);
}

unsigned char TLC59116::get_addr()
{
  return addr;
}
