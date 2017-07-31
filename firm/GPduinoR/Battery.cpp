extern "C" {
#include "user_interface.h"
}

#define _BATTERY_C_
#include <Battery.h>

// begin
void Battery_t::begin()
{
    isAverageEnable = true;
    Average.init();
}

// get the battery voltage (ADC raw value)
int Battery_t::getAdc()
{
    unsigned short Vbat = system_adc_read();
    
    if(isAverageEnable){
        Vbat = Average.pop(Vbat);
    }
    
    return (int)Vbat;
}

// get the battery voltage [mV]
int Battery_t::get_mV()
{
    int adc = this->getAdc();
    
    int mV = (adc * 11000) / 1023;
    return mV;
}

// convert ADC raw value to voltage value[mV]
// adc: ADC raw value
int Battery_t::get_mV(int adc)
{
    int mV = (adc * 11000) / 1023;
    return mV;
}

// get the battery voltage [V]
double Battery_t::getV()
{
    int adc = this->getAdc();
    
    double V = ((double)adc * 11.0) / 1023.0;
    return V;
}

// convert ADC raw value to voltage value[V]
// adc: ADC raw value
double Battery_t::getV(int adc)
{
    double V = ((double)adc * 11.0) / 1023.0;
    return V;
}

// enable the averaging filter
void Battery_t::averageOn()
{
    isAverageEnable = true;
}

// disable the averaging filter
void Battery_t::averageOff()
{
    isAverageEnable = false;
}


