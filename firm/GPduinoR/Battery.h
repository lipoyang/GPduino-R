#ifndef    _BATTERY_H_
#define    _BATTERY_H_

#ifdef _BATTERY_C_
#define EXTERN
#else
#define EXTERN extern
#endif

#include <MovingAverage.h>

// battery measurement class
class Battery_t
{
public:
    // begin
    void begin();
    // get the battery voltage (ADC raw value)
    int getAdc();
    // get the battery voltage [mV]
    int get_mV();
    // convert ADC raw value to voltage value[mV]
    int get_mV(int adc);
    // get the battery voltage [V]
    double getV();
    // convert ADC raw value to voltage value[V]
    double getV(int adc);
    // enable the averaging filter
    void averageOn();
    // disable the averaging filter
    void averageOff();
private:
    // averaging filter
    MovingAverage Average;
    // is averaging enable?
    bool isAverageEnable;
};

// Global object
EXTERN Battery_t Battery;

#endif
