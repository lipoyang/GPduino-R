#ifndef    _DC_MOTOR_H_
#define    _DC_MOTOR_H_

#ifdef _DC_MOTOR_C_
#define EXTERN
#else
#define EXTERN extern
#endif

// Motor Number
#define MOTOR_LEFT  0
#define MOTOR_RIGHT 1

// DC motor driver class
class DcMotor_t
{
public:
    // begin the DC motor driver
    void begin();
    // drive the motor
    void out(int ch, int value);
    // drive the both motors (as a tank)
    void outDiff(int common, int diff);
};

// Global object
EXTERN DcMotor_t DcMotor;

#endif
