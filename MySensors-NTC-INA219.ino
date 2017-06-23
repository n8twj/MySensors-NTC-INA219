// Enable debug prints to serial monitor
#define MY_DEBUG
// Enable and select radio type attached
#define MY_RADIO_NRF24

#include <MySensors.h>
#include <Wire.h>
#include <Adafruit_INA219.h>

#define ID_S_TEMP 1
#define ID_S_MULTIMETER 2

int NTC_PIN = A0;
unsigned long SLEEP_TIME = 60000;

MyMessage msg_S_TEMP(ID_S_TEMP,V_TEMP);
MyMessage msg_S_MULTIMETER_V_VOLTAGE(ID_S_MULTIMETER,V_VOLTAGE);
MyMessage msg_S_MULTIMETER_V_CURRENT(ID_S_MULTIMETER,V_CURRENT);

Adafruit_INA219 ina219(0x40);

double convert_adc_reading(int RawADC) {
 double Temp;
 Temp = log(10000.0*((1024.0/RawADC-1))); 
//    = log(10000.0/(1024.0/RawADC-1)) // for pull-up configuration
 Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp ))* Temp );
 Temp = Temp - 273.15;            // Convert Kelvin to Celcius
 Temp = (Temp * 9.0)/ 5.0 + 32.0; // Convert Celcius to Fahrenheit
 return Temp;
}

void read_ina219() {
  float shuntVoltage = 0;
  float busVoltage = 0;
  float currentMa = 0;
  float loadVoltage = 0;
  
  shuntVoltage = ina219.getShuntVoltage_mV();
  busVoltage = ina219.getBusVoltage_V();
  currentMa = ina219.getCurrent_mA();
  loadVoltage = busVoltage + (shuntVoltage / 1000);

  Serial.print("Voltage: ");
  Serial.println(loadVoltage);
  send(msg_S_MULTIMETER_V_VOLTAGE.set(loadVoltage,2));
  Serial.print("Current: " );
  Serial.println(currentMa);
  send(msg_S_MULTIMETER_V_CURRENT.set(currentMa,2));

}

void presentation() {
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo("NTC-INA219 Sensor", "1.0");  
  Serial.println("S_TEMP");
  present(ID_S_TEMP,S_TEMP,"Temperature");
  wait(50);
  Serial.println("INA219");
  present(ID_S_MULTIMETER,S_MULTIMETER,"INA219");
  wait(50);
}

void setup() {
  ina219.begin();
}

void loop() { 
  int adc_reading = analogRead(NTC_PIN);
  double temperature = convert_adc_reading(adc_reading);
#ifdef MY_DEBUG
  Serial.print("Temperature: ");
  Serial.println(temperature);
#endif
  read_ina219();
  send(msg_S_TEMP.set(int(temperature)));
  smartSleep(SLEEP_TIME);
}

