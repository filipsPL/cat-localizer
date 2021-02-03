// esp32 lite 4mb
// wemos lolin 32
// speed 460k
// Partition - no OTA

#include <HTTPClient.h>

#include <WiFiUdp.h>
#include <NTPClient.h>

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>


#include <WiFiUdp.h>

// sensors and extras
//#include <OneWire.h>
//#include <DallasTemperature.h>
//#define ONE_WIRE_BUS 19
//
//
//OneWire oneWire(ONE_WIRE_BUS);
//DallasTemperature sensors(&oneWire);



// --------------------------------------------------------------------- //
// WiFi settings
const char* ssid = "***** ***";
const char* password = "***** ***";

// api url for the influx gateway + token
String apiurl = "http://your_api_url/&message=";

// host name
const char* esphost = "esp32-living_room";

// MAC address(es) of the BLE beacons
const char* cat1Mac = "xx:xx:cc:dd:ee:ff";
const char* cat2Mac = "yy:yy:cc:dd:ff:gg";

// RSSI values in case no beacons are found
// here, we are looking for two cats, so two values
int cat1RSSI = -110;
int cat2RSSI = -110;

// BLE scan time
int scanTime = 11; //BLE scanning time, In seconds

#include "include.h"

// ------------------------------------------------------------------------------- //


// class for BLE scanning
BLEScan* pBLEScan;

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {

    void onResult(BLEAdvertisedDevice advertisedDevice) {
      Serial.printf("Advertised Device: %s \n", advertisedDevice.toString().c_str());
      Serial.println("");
      Serial.printf("Advertised Device mac %s \n", advertisedDevice.getAddress().toString().c_str());
      Serial.println("");
      Serial.printf("Advertised Device rssi %i \n", advertisedDevice.getRSSI());
    }
};


// ------------------------------------------------------------------------------- //



void setup() {

  delay(10);
  unsigned long startTime = millis();
  Serial.begin(115200);

  // turn off blinking
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  // sensory
  //  sensors.begin();

  // wifi
  WiFi.setHostname(esphost);

  int attemptsCount = 0;
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    attemptsCount++;
    if (attemptsCount >= 10) {
      ESP.restart();
    }
  }


  // time server settings
  WiFiUDP ntpUDP;
  NTPClient timeClient(ntpUDP, "de.pool.ntp.org", 7200, 60000);
  timeClient.begin();
  timeClient.update();


 // temperature reading from ds18b20
//  sensors.requestTemperatures(); // Send the command to get temperatures
//  delay(900);
//  float temperature = sensors.getTempCByIndex(0);
//
//  if ((temperature > -80) and (temperature < 80)) {
//    Serial.println(temperatura);
//    // send influx data
//    sendData("temp,location=home,source=esp32-living_room,room=living_room", temperature);
//  }
//  else {
//    Serial.print("doesnt work: ");
//    Serial.println(temperature);
//  }


  // skan for BLE
  doBLEScans();

  // go to sleep
  int sleepTimeS = 59-timeClient.getSeconds();
//  int sleepTimeS = 45;
  ESP.deepSleep(1e6 * sleepTimeS);
}



void loop() {
}


void doBLEScans() {
  Serial.println("Scanning...");

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);  // less or equal setInterval value

  // put your main code here, to run repeatedly:
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  Serial.print("Devices found: ");
  Serial.println(foundDevices.getCount());
  Serial.println("Scan done!");
  BLEDevice::deinit(false);  // turn off ble to make wifi great again.

  int count = foundDevices.getCount();
  if (count > 0) {

    for (int i = 0; i < count; i++) {
      BLEAdvertisedDevice d = foundDevices.getDevice(i);
      if (d.getAddress().toString() == cat1Mac) {
        cat1RSSI = d.getRSSI();
      }

        if (d.getAddress().toString() == cat2Mac) {
        cat2RSSI = d.getRSSI();
      }

    } // for each device

    // talk with server
    sendData("rssi,location=home,source=esp32-living_room,room=living_room,kto=cat1", cat1RSSI);
    sendData("rssi,location=home,source=esp32-living_room,room=living_room,kto=cat2", cat2RSSI);


  } // non-zero devices found
  pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory

  delay(100);

}
