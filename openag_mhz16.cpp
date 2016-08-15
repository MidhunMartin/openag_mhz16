#include "openag_mhz16.h"

MHZ16::MHZ16(int i2c_address) : _sensor(i2c_address) {
  status_level = OK;
  status_msg = "";
  _send_air_carbon_dioxide = false;
  _time_of_last_reading = 0;
  _time_of_last_power_cycle = 0;
}

void MHZ16::begin() {
  _sensor.power_on();
  _is_on = true;
  if (!_sensor.begin()) {
    status_level = ERROR;
    status_msg = "Failed to initialize sensor";
  }
  else {
    status_level = WARN;
    status_msg = "Initializing";
    _init_time = millis();
    _initializing = true;
  }
}

void MHZ16::update() {
  if (!_is_on) {
    if (millis() - _time_of_last_power_cycle > _leave_off_for) {
      begin();
    }
    return;
  }
  if (_initializing) {
    if (millis() - _init_time < 10000) {
      return;
    }
    else {
      _initializing = false;
      status_level = OK;
      status_msg = "";
    }
  }
  if (millis() - _time_of_last_power_cycle > _power_cycle_interval) {
    _sensor.power_off();
    _is_on = false;
    _time_of_last_power_cycle = millis();
    status_level = WARN;
    status_msg = "Powered off to prevent autocalibration";
  }
  if (millis() - _time_of_last_reading > _min_update_interval) {
    if (_sensor.measure()) {
      if (status_level == OK) {
        _send_air_carbon_dioxide = true;
        _time_of_last_reading = millis();
      }
      else {
        begin();
      }
    }
    else {
      if (status_level != ERROR) {
        status_level = ERROR;
        status_msg = "Failed to read from sensor";
      }
    }
  }
}

bool MHZ16::get_air_carbon_dioxide(std_msgs::Int32 &msg) {
  msg.data = _sensor.ppm;
  bool res = _send_air_carbon_dioxide;
  _send_air_carbon_dioxide = false;
  return res;
}
