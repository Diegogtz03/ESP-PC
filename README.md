# ESP-PC
ESP-PC is the arduino part of the project [PCPower](https://github.com/Diegogtz03/PCPower) which uses WebSockets to communicate to this ESP32/ESP8266 and execute actions remotely on a PC.

The idea for this project originated out of me having to move out for a few months without the chance of taking my PC. I still wanted to use it, so I decided to make this in order to remotely 
power on my PC and be able to remotely access it via **Parsec** or **MRD**.

## Parts Used
- 1 ESP8266
- 1 USB to MicroUSB to power the ESP
- 1 330Ω Resistance
- 1 100KΩ Resistance
- 1 200KΩ Resistance
- 1 4N25 Octocoupler
- Some cables

## Schematics

In progress...

## Code Notes
- A **secrets.h** file needs to be created within **/include** which should contain the following:
  - #define WIFI_SSID "YOUR_VALUE_HERE"
  - #define WIFI_PASSWORD "YOUR_VALUE_HERE"
  - #define WS_HOST "YOUR_VALUE_HERE"
  - #define WS_PORT YOUR_PORT_HERE
  - #define WS_URL "YOUR_VALUE_HERE"

- The code currently accepts the values "GET STATUS", "ON", or "OFF" which is what I needed for this project ([PCPower](https://github.com/Diegogtz03/PCPower)).
- In the case of "ON", the octocoupler gets activated for 400ms to simulate a quick but realistic button press
- In the case of "OFF", the octocoupler gets activated for 4.5s to simulate a long press needed to force shut down the PC. (This may vary depending on your motherboard).
