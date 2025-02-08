# Monitoring of sensor data with ESP32

Project will allow logging of sensor data via MQTT, with USB and OTA configuration of sensor nodes.

## Folder structure (not yet implemented)

 - src: firmware (currently Arduino, may switch to platformIO in the future)
   - secrets.h: if you want default data including SSID/password, you'll need to update this
 - docker: configuration for a test MQTT broker docker image
 - viewer: page to view test data 

## Use Case

For the initial implementation, the following scenario is desirable.
A multi-site facility has coolrooms and work areas and wants to monitor the temperature and humidity in each, with an alarm if the temperature goes beyond certain bounds, and a warning if the temperature is drifting out of normal bounds.

## TODO
 
 - Add initial sensors
 - Add configuration storage, USB settings
 - Add MQTT settings method
 - Add test docker configuration script
 - Add test page using websockets
 - Implement warnings based on Six Sigma Out of Control Charts
