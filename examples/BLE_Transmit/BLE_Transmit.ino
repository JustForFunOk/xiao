// Send data to central device through BLE

#include <ArduinoBLE.h>

// BLEService TransmitService("19B10000-E8F2-537E-4F6C-D104768A1214"); // Bluetooth® Low Energy LED Service
// Matlab need 16bit UUID instead of 128bit UUID
BLEService TransmitService("aaaa");

// Bluetooth® Low Energy LED Switch Characteristic - custom 128-bit UUID, read and writable by central
// Attention: use BLEWriteWithoutResponse instead of BLEWrite (Reference: https://ww2.mathworks.cn/matlabcentral/answers/556798-ble-write-to-arduino-nano-33-ble-is-unsuccessful)
BLEFloatCharacteristic switchCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWriteWithoutResponse);

const int ledPin = LED_BUILTIN; // pin to use for the LED
static int count = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // set LED pin to output mode
  pinMode(ledPin, OUTPUT);

  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy module failed!");

    while (1);
  }

  // set advertised local name and service UUID:
  BLE.setLocalName("LED");
  BLE.setAdvertisedService(TransmitService);

  // add the characteristic to the service
  TransmitService.addCharacteristic(switchCharacteristic);

  // add service
  BLE.addService(TransmitService);

  // start advertising
  BLE.advertise();

  Serial.println("BLE LED Peripheral");
}

void loop() {
  // listen for Bluetooth® Low Energy peripherals to connect:
  BLEDevice central = BLE.central();

  // if a central is connected to peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());

    // while the central is still connected to peripheral:
    while (central.connected()) {
      switchCharacteristic.writeValue(1.1*count);

      // Query the current value of the specified BLECharacteristic.
      if (count % 2 == 1) {
        Serial.println("LED on");
        digitalWrite(ledPin, LOW); // changed from HIGH to LOW
      } else {
        Serial.println(F("LED off"));
        digitalWrite(ledPin, HIGH); // changed from LOW to HIGH
      }

      count++;

      delay(1000);
    }

    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
}