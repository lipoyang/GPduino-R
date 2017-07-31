#define _RC_SERVO_C_
#include <RcServo.h>
#include <EEPROM.h>

// Pin Number
#define SERVO_POWON    16

// pulth width = 0.5ms to 2.5ms (neutral position = 1.5ms)
#define PULSE_MIN  102 // 102/4096 * 20ms = 0.5ms
#define PULSE_MAX  512 // 512/4096 * 20ms = 2.5ms

// begin the RC servo driver
void RcServo_t::begin()
{
    // turn off the power of all servos
    digitalWrite(SERVO_POWON, LOW);
    pinMode(SERVO_POWON, OUTPUT);
    
    // setup PCA9685 PWM driver
    Pwm.begin();
    Pwm.setPWMFreq(50);  // cycle = 20msec
}

// turn on the power of all servos
void RcServo_t::powerOn()
{
    digitalWrite(SERVO_POWON, HIGH);
}

// turn off the power of all servos
void RcServo_t::powerOff()
{
    digitalWrite(SERVO_POWON, LOW);
}

// control the servo
// ch: channel
// value: output value (-90 to +90 [deg])
void RcServo_t::out(int ch, int value)
{
    // convert (-90,+90)[deg] -> (0,180)[deg] with offset and polarity
    uint32_t deg = 90 + Offset[ch] + Polarity[ch] * value;
    // convet deg value -> timer count value
    uint16_t cnt = PULSE_MIN + (uint16_t)(deg*(PULSE_MAX-PULSE_MIN)/180);
    
    Pwm.setPWM(ch, 0, cnt);
}

// set polarity
// ch: channel
// polarity: polarity of output (+1 or -1)
void RcServo_t::setPolarity(int ch, int polarity)
{
    Polarity[ch] = polarity;
}

// set offset
// ch: channel
// offset: offset of output
void RcServo_t::setOffset(int ch, int offset)
{
    Offset[ch] = offset;
}

// get polarity
int* RcServo_t::getPolarity()
{
    return Polarity;
}

// get offset
int* RcServo_t::getOffset()
{
    return Offset;
}

// save settings
// base: base address of EEPROM
void RcServo_t::saveSettings(int base)
{
    int ch;
    for(ch=0; ch<SERVO_NUM; ch++){
        EEPROM.write(base + ch*2 + 0, Polarity[ch]);
        EEPROM.write(base + ch*2 + 1, Offset  [ch]);
    }
    EEPROM.commit();
}

// load settings
// base: base address of EEPROM
void RcServo_t::loadSettings(int base)
{
    int ch;
    for(ch=0; ch<SERVO_NUM; ch++){
        Polarity[ch] = (int)((signed char)EEPROM.read(base + ch*2 + 0));
        Offset  [ch] = (int)((signed char)EEPROM.read(base + ch*2 + 1));
    }
}

