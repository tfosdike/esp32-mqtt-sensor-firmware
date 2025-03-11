#include <logger_settings.h>
  
logger_settings::logger_settings() {
}

void logger_settings::begin()
{
  pref.begin("logger",false);
  current_display_state = settings_unopened;
}

size_t logger_settings::set_SSID(const char *value)
{
  size_t ret = pref.putString("SSID",value);
  Serial.printf("wrote %d bytes\r\n",ret);
  return ret;
}
size_t logger_settings::get_SSID(char *value, size_t maxLen)
{
  return pref.getString("SSID",value,maxLen);
}
size_t logger_settings::set_WiFi_Password(const char *value)
{
  return pref.putString("WFPWD",value);
}
size_t logger_settings::get_WiFi_Password(char *value, size_t maxLen)
{
  return pref.getString("WFPWD",value,maxLen);
}

void logger_settings::show_menu()
{
  Serial.printf("Change settings\r\n");
  Serial.printf("===============\r\n");
  char ssid[64];
  get_SSID(ssid,sizeof(ssid)-1);
  Serial.printf("1) SSID %s\r\n",ssid);
  Serial.printf("setting to change >");
  current_display_state = settings_menu;
}

void logger_settings::start_change(int setting)
{
  current_setting = setting;
  switch(setting)
  {
    case 1:
    case 2:
      current_display_state = settings_entering_string;
      break;
    case 3:
      current_display_state = settings_entering_ip;
      break;
    case 4:
    case 5:
    case 6:
    case 7:
      current_display_state = settings_entering_number;
      break;
    default:
      Serial.printf("invalid option %d\r\n",setting);
      show_menu();
      break;
  }
}

void logger_settings::setter_interface_update()
{
  int nbytes = Serial.available();
  if(nbytes != 0)
  {
    switch(current_display_state)
    {
      case settings_unopened:
        show_menu();
        for(int i = 0; i< nbytes; i++)
        {
          Serial.read();
        }
        break;
      case settings_menu:
        // this is blocking.  Perhaps in future can be fixed to allow reading during this.
        start_change(Serial.parseInt());
        break;
      case settings_entering_string:
        {
          String reading = Serial.readStringUntil('\n');
          reading.trim();
          if(reading.length() > 0)
          {
            if(current_setting == 1)
            {
              Serial.printf("setting ssid to %s\r\n",reading.c_str());
              set_SSID(reading.c_str());
            }
            else if(current_setting == 2)
              set_WiFi_Password(reading.c_str());
            show_menu();
          }
        }
        break;
      case settings_entering_number:
        break;
      case settings_entering_ip:
        break;
      default:
        show_menu();
        for(int i = 0; i< nbytes; i++)
        {
          Serial.read();
        }
        break;
    }
  }
}
