#ifndef OPENAG_MHZ16
#define OPENAG_MHZ16

#include "Arduino.h"
#include <Wire.h>
#include "NDIR_I2C.h"
#include <openag_module.h>
#include <std_msgs/Int32.h>

class MHZ16 : public Module {
  public:
    MHZ16(int i2c_address);
    void begin();
    void update();
    bool get_air_carbon_dioxide(std_msgs::Int32 &msg);

  private:
    NDIR_I2C _sensor;
    bool _send_air_carbon_dioxide;
    uint32_t _init_time;
    bool _initializing;
    uint32_t _time_of_last_reading;
    const static uint32_t _min_update_interval = 2000;

    void readData();
};

#endif
