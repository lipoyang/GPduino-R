#include <Arduino.h>

#define _DC_MOTOR_C_
#include <DcMotor.h>

// Pin Number
#define MOTOR_R_PWM    14
#define MOTOR_R_IN2    12
#define MOTOR_R_IN1    13
#define MOTOR_L_PWM    0
#define MOTOR_L_IN2    2
#define MOTOR_L_IN1    15

// begin the DC motor driver
void DcMotor_t::begin()
{
    // initialize PWM
    analogWrite(MOTOR_L_PWM, 0);
    analogWrite(MOTOR_R_PWM, 0);
    
    // initialize GPIO
    digitalWrite(MOTOR_L_IN1, LOW);
    digitalWrite(MOTOR_L_IN2, LOW);
    digitalWrite(MOTOR_R_IN1, LOW);
    digitalWrite(MOTOR_R_IN2, LOW);
    pinMode(MOTOR_L_IN1, OUTPUT);
    pinMode(MOTOR_L_IN2, OUTPUT);
    pinMode(MOTOR_R_IN1, OUTPUT);
    pinMode(MOTOR_R_IN2, OUTPUT);
}

// drive the motor
// ch: channel. MOTOR_LEFT(=0) or MOTOR_RIGHT(=1)
// value: output value (-1023 to +1023)
void DcMotor_t::out(int ch, int value)
{
    static const int IN1[]={MOTOR_L_IN1, MOTOR_R_IN1};
    static const int IN2[]={MOTOR_L_IN2, MOTOR_R_IN2};
    static const int PWM[]={MOTOR_L_PWM, MOTOR_R_PWM};
    
    int lpwm,rpwm,lin1,lin2,rin1,rin2;
    
    int pwm = abs(value);
    if(pwm<0) pwm = 0;
    if(pwm>1023) pwm = 1023;
    
    int in1,in2;
    if(value > 0){
        in1 = LOW;
        in2 = HIGH;
    }else if(value < 0){
        in1 = HIGH;
        in2 = LOW;
    }else{
        in1 = LOW;
        in2 = LOW;
        pwm = 0;
    }

    digitalWrite(IN1[ch], in1);
    digitalWrite(IN2[ch], in2);
    analogWrite(PWM[ch], pwm);
}

// drive the both motors (as a tank)
// common: common output of both motors, or translation (-1023 to +1023)
// diff: differential output of both motors, or rotation (-1023 to + 1023)
void DcMotor_t::outDiff(int common, int diff)
{
    int l,r;
    
    l = common + diff;
    r = common - diff;
    
    if(l> 1023) l= 1023;
    if(l<-1023) l=-1023;
    if(r> 1023) r= 1023;
    if(r<-1023) r=-1023;
    
    this->out(MOTOR_LEFT,  l);
    this->out(MOTOR_RIGHT, r);
}

