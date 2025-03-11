#include <Preferences.h>
#ifndef _LOGGER_SETTINGS_H_
#define _LOGGER_SETTINGS_H_

typedef enum 
{
  settings_unopened,
  settings_menu,
  settings_entering_string,
  settings_entering_number,
  settings_entering_ip,
  settings_max
} current_display_state_t;

class logger_settings {
private:
  current_display_state_t current_display_state;
  int current_setting;
  void show_menu();
  void start_change(int setting);

protected:
  Preferences pref;

public:
  logger_settings();
  void begin(void);

  size_t set_SSID(const char *value);
  size_t get_SSID(char *value, size_t maxLen);
  size_t set_WiFi_Password(const char *value);
  size_t get_WiFi_Password(char *value, size_t maxLen);
  
  size_t set_MQTT_IP(uint8_t *value);
  size_t get_MQTT_IP(uint8_t *value);

  size_t set_sensor_name(const char *value);
  size_t get_sensor_name(char *value, size_t maxLen);

  uint32_t set_temperature_period_ms(uint32_t period_ms);
  uint32_t get_temperature_period_ms(uint32_t defaultValue = 30000);

  uint32_t set_humidity_period_ms(uint32_t period_ms);
  uint32_t get_humidity_period_ms(uint32_t defaultValue = 30000);

  int8_t set_temperature_lower_limit_c(int8_t limit_c);
  int8_t get_temperature_lower_limit_c(int8_t limit_c = 1);

  int8_t set_temperature_upper_limit_c(int8_t limit_c);
  int8_t get_temperature_upper_limit_c(int8_t limit_c = 30);

  void setter_interface_update(void);
};

#endif