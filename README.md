# Blueberry - Personal Assistant

### Components Required <br>
1) ESP8266 (Any arduino supported microcontroller boards with wifi capabilities) <br>
2) OLED 1306 <br>
3) Pushbutton switch <br>
4) Resistor (< 300 ohms) <br>


### Information:

##### 1) Connection for OLED to ESP8266
<img src="https://github.com/santoshbalaji/blueberry-public/blob/master/images/oled_connection_diagram.png" />

##### 2) Connecting switch to ESP8266
<img src="https://github.com/santoshbalaji/blueberry-public/blob/master/images/esp8266_buttonswitch.png" />

##### 3) Dependencies for arduino modules <br>
   <a href="https://github.com/arduino-libraries/NTPClient.git">NTPClient - 3.1.0 (For time synchronization)</a><br>
   <a href="https://github.com/ThingPulse/esp8266-oled-ssd1306.git">SSD1306Wire (For writing to oled display)</a><br>
   <a href="https://arduinojson.org/">ArduinoJSON (For parsing json from API)</a><br>

##### 4) API Used <br>
<a href="https://www.mytransport.sg/content/mytransport/home/dataMall.html">LTA API </a><br>
<a href="https://data.gov.sg/developer">NEA Weather API </a> <br>
<a href="https://exchangeratesapi.io/">ExchangeRates API</a> <br>
