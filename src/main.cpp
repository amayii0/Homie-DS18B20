/*********************************************************************************************************************
 ** 2017-01-24, RDU: First draft to monitor temperature using a single DS18B20 sensor
 **                  D2 = Data bus (this actually supports multiple sensors using OneWire)
 **
 ** 2017-02-14, RDU: Aligned code format on Homie-HVAC 0.17.2.7
 **
 ** TODOs
 ** - Support multiple sensors as a HomieRange
 **
 *********************************************************************************************************************/
#include <Homie.h>


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Globals
  // Software specifications
    #define FW_NAME    "D1Mini-DS18B20"
    #define FW_VERSION "0.17.2.14"


  // DS18b20
    #include <OneWire.h>
    #include <DallasTemperature.h>
    #define ONE_WIRE_BUS D2 // Data wire is plugged into port 2 on the Arduino

    OneWire oneWire(ONE_WIRE_BUS); // Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
    DallasTemperature dtsensors(&oneWire); // Pass our oneWire reference to Dallas Temperature.

    // Sensor consts/vars
      HomieNode temperatureNode("temperature", "temperature");

    // Measure loop
        const int MEASURE_INTERVAL = 10; // How often to poll DHT22 for temperature and humidity
        unsigned long lastMeasureSent = 0;


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Homie Setup Handler
void setupHandler() {
  // Nodes part
  temperatureNode.setProperty("unit").send("c");

  // Hardware part
  dtsensors.begin();
  Homie.getLogger() << "DS18B20 OneWire bus on pin " << ONE_WIRE_BUS << endl;
  Homie.getLogger() << "Found " << dtsensors.getDeviceCount() << " device(s) on bus" << endl;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Homie Setup Handler
void loopHandler() {
  if (millis() - lastMeasureSent >= MEASURE_INTERVAL * 1000UL || lastMeasureSent == 0) {
    dtsensors.requestTemperatures();
    float rawTemp = dtsensors.getTempCByIndex(0); // TODO, Supports only one device!
    if (isnan(rawTemp)) {
      Homie.getLogger() << F("Failed to read from sensor!");
    } else {
      Homie.getLogger() << F("Temperature: ") << rawTemp << " °C" << endl;
      temperatureNode.setProperty("degrees").send(String(rawTemp));
    }
    lastMeasureSent = millis();
  }
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Arduino Setup Handler
void setup() {
  Serial.begin(115200); // Required to enable serial output

  Homie_setFirmware(FW_NAME, FW_VERSION);
  Homie.setSetupFunction(setupHandler).setLoopFunction(loopHandler);

  // DS18B20 sensor
    temperatureNode.advertise("unit");
    temperatureNode.advertise("degrees");

  Homie.setup();
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Arduino Loop Handler
void loop() {
  Homie.loop();
}
