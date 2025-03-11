#ifdef CORE_DEBUG_LEVEL
#undef CORE_DEBUG_LEVEL
#endif

#define CORE_DEBUG_LEVEL 3
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG

#include <M5NanoC6.h>
#include <Adafruit_NeoPixel.h>
#include <DFRobot_BMP280.h>
#include <SHT40AD1BSensor.h>
#include <Wire.h>
#include <WiFi.h>
#include <MQTT.h>
#include <esp_system.h>
#include <logger_settings.h>

WiFiClient net;
MQTTClient client;
IPAddress MQTT_SERVER_ADDR(192,168,11,124);


#define SEA_LEVEL_PRESSURE    1015.0f
#define NUM_LEDS 1
Adafruit_NeoPixel strip(NUM_LEDS, M5NANO_C6_RGB_LED_DATA_PIN,
                        NEO_GRB + NEO_KHZ800);

DFRobot_BMP280_IIC bmp280_barometer(&Wire, DFRobot_BMP280_IIC::eSdoLow);
SHT40AD1BSensor sht30_humidity(&Wire);
logger_settings settings;

// show last sensor operate status
void printLastOperateStatus(DFRobot_BMP280_IIC::eStatus_t eStatus)
{
  switch(eStatus) {
  case DFRobot_BMP280_IIC::eStatusOK:    Serial.println("everything ok"); break;
  case DFRobot_BMP280_IIC::eStatusErr:   Serial.println("unknow error"); break;
  case DFRobot_BMP280_IIC::eStatusErrDeviceNotDetected:    Serial.println("device not detected"); break;
  case DFRobot_BMP280_IIC::eStatusErrParameter:    Serial.println("parameter error"); break;
  default: Serial.println("unknow status"); break;
  }
}

void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.print("\nconnecting...");
  while (!client.connect("arduino")) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnected!");

}

void showPreferences() {
}

void configure_loop(void)
{
  settings.begin();
  while(!settings.setter_interface_update()){
    vTaskDelay(10);
  }
}

void reporting_loop(void)
{
  // Enable RGB LED Power
  pinMode(M5NANO_C6_RGB_LED_PWR_PIN, OUTPUT);
  digitalWrite(M5NANO_C6_RGB_LED_PWR_PIN, HIGH);
  strip.begin();
  strip.show();

  Serial.println("starting WIFI");
  char ssid[32] = "";
  char passwd[32] = "";
  settings.get_SSID(ssid, sizeof(ssid));
  settings.get_WiFi_Password(ssid, sizeof(passwd));
  while (WiFi.begin(ssid, passwd) != WL_CONNECTED)
  {
    /* display network fault in red */
    strip.setPixelColor(255, 0, 0);
    strip.show();
    settings.get_SSID(ssid, sizeof(ssid));
    settings.get_WiFi_Password(ssid, sizeof(passwd));
  }

  Serial.println("starting MQTT");
  uint8_t ip[4];
  char name[32] = "";
  settings.get_MQTT_IP(ip);
  client.begin(IPAdress(ip[0], ip[1], ip[2], ip[3]), net);
  settings.get_sensor_name(name, sizeof(name));
  while (!client.connect(name))
  {
    strip.setPixelColor(255, 0, 0);
    strip.show();
    settings.get_MQTT_IP(ip);
    client.begin(IPAdress(ip[0], ip[1], ip[2], ip[3]), net);
    settings.get_sensor_name(name, sizeof(name));
  }

  Serial.println("starting I2C");
  Wire.begin(2, 1);
}
void setup()
{
  Serial.begin(115200);
  xTaskCreate(configure_loop, "conf", 1000, NULL, 1, NULL);
  xTaskCreate(reporting_loop, "report", 1000, NULL, 1, NULL);

  while (bmp280_barometer.begin() != DFRobot_BMP280_IIC::eStatusOK)
  {
    Serial.println("bmp begin faild");
    printLastOperateStatus(bmp280_barometer.lastOperateStatus);
    delay(2000);
  }
  Serial.println("bmp begin success");
  delay(100);
  showPreferences();
}

static void print_sht40()
{
  float temperature_c;
  float humidity_rh;
#if 0
  Serial.print("[TMP]: ");  
  sht30_humidity.GetTemperature(&temperature_c);
  Serial.print(temperature_c);
  Serial.print(" C [HUM]: ");  
  sht30_humidity.GetHumidity(&humidity_rh);
  Serial.print(humidity_rh);
  Serial.println(" %");
#endif
  static int i = 0;
  i++;
  if (i > 10)
  {
    Serial.println("publishing to topics");
    if (!client.connected())
    {
      connect();
    }
    char s[64];
    sprintf(s, "%d", (int)temperature_c);
    client.publish("/arduino/temperature", s);
    sprintf(s, "%d", (int)humidity_rh);
    client.publish("/relative_humidity", s);
    i = 0;
  }
}

static void print_bmp280()
{
  float   temp = bmp280_barometer.getTemperature();
  uint32_t    press = bmp280_barometer.getPressure();
  float   alti = bmp280_barometer.calAltitude(SEA_LEVEL_PRESSURE, press);
#if 0
  Serial.println();
  Serial.println("======== start print ========");
  Serial.print("temperature (unit Celsius): "); Serial.println(temp);
  Serial.print("pressure (unit pa):         "); Serial.println(press);
  Serial.print("altitude (unit meter):      "); Serial.println(alti);
  Serial.println("========  end print  ========");
#endif
}

void loop() {
    for(int i = 0; i < 127; i++)
    {
      strip.setPixelColor(0, strip.Color(2*i, 2*(127-i), 0));
      strip.show();
      delay(1);
    }
    settings.setter_interface_update();
    for(int i = 0; i < 127; i++)
    {
      strip.setPixelColor(0, strip.Color(2*(127-i), 0, 2*i));
      strip.show();
      delay(1);
    }
    settings.setter_interface_update();
    for(int i = 0; i < 127; i++)
    {
      strip.setPixelColor(0, strip.Color(0, 2*i, 2*(127-i)));
      strip.show();
      delay(1);
    }
    settings.setter_interface_update();

    // 蓝色
    strip.setPixelColor(0, strip.Color(0, 0, 255));
    strip.show();
    delay(100);
    print_sht40();
    print_bmp280();
}