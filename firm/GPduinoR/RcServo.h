#ifndef    _RC_SERVO_H_
#define    _RC_SERVO_H_

#include <Adafruit_PWMServoDriver.h>

#ifdef _RC_SERVO_C_
#define EXTERN
#else
#define EXTERN extern
#endif

// total number of servos
#define SERVO_NUM  16

// RC servo driver class
class RcServo_t
{
public:
    // begin the RC servo driver
    void begin();
    // turn on the power of all servos
    void powerOn();
    // turn off the power of all servos
    void powerOff();
    // control the servo
    void out(int ch, int value);
    // set polarity
    void setPolarity(int ch, int polarity);
    // set offset
    void setOffset(int ch, int offset);
    // get polarity
    int* getPolarity();
    // get offset
    int* getOffset();
    // save settings
    void saveSettings(int base);
    // load settings
    void loadSettings(int base);
    
private:
    Adafruit_PWMServoDriver Pwm;// PCA9685 PWM driver
    int Polarity[SERVO_NUM];    // polarity of servos
    int Offset[SERVO_NUM];      // offset of servos
};

// Global object
EXTERN RcServo_t RcServo;

#endif
