#include <ArduinoBLE.h>
#include "LSM6DS3.h"
#include "Wire.h"

//Create a instance of class LSM6DS3
LSM6DS3 myIMU(I2C_MODE, 0x6A);    //I2C device address 0x6A

// BLEService ledService("19B10000-E8F2-537E-4F6C-D104768A1214"); // Bluetooth® Low Energy LED Service
// Matlab need 16bit UUID instead of 128bit UUID
BLEService imuService("aaaa");

// Bluetooth® Low Energy LED Switch Characteristic - custom 128-bit UUID, read and writable by central
// Attention: use BLEWriteWithoutResponse instead of BLEWrite (Reference: https://ww2.mathworks.cn/matlabcentral/answers/556798-ble-write-to-arduino-nano-33-ble-is-unsuccessful)
// BLENotify: remote clients will be able to get notifications if this characteristic changes
BLEFloatCharacteristic imuCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify);

BLEDescriptor imuDescriptor("2901", "imu");

const int ledPin = LED_BUILTIN; // pin to use for the LED

// Global variables
static float accel_x;
static float accel_y;
static float accel_z;
static float gyro_x;
static float gyro_y;
static float gyro_z;

static int count = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  //Call .begin() to configure the IMUs
  if (myIMU.begin() != 0) {
      Serial.println("IMU Device error");
  } else {
      Serial.println("IMU Device OK!");
  }

  // begin initialization BLE
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy module failed!");
    while (1);
  }

  // set advertised local name and service UUID:
  BLE.setLocalName("IMU");
  BLE.setAdvertisedService(imuService);

  imuCharacteristic.addDescriptor(imuDescriptor);

  // add the characteristic to the service
  imuService.addCharacteristic(imuCharacteristic);

  // add service
  BLE.addService(imuService);

  // start advertising
  BLE.advertise();

  Serial.println("BLE IMU Service Init Finish");
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
      accel_x = myIMU.readFloatAccelX();
      accel_y = myIMU.readFloatAccelY();
      accel_z = myIMU.readFloatAccelZ();
      gyro_x  = myIMU.readFloatGyroX();
      gyro_y  = myIMU.readFloatGyroY();
      gyro_z  = myIMU.readFloatGyroZ();
      imuCharacteristic.writeValue(accel_x);
      imuCharacteristic.writeValue(accel_y);
      imuCharacteristic.writeValue(accel_z);
      imuCharacteristic.writeValue(gyro_x);
      imuCharacteristic.writeValue(gyro_y);
      imuCharacteristic.writeValue(gyro_z);

      Serial.print("\nIMU:\n");
      Serial.println(accel_x, 4);
      Serial.println(accel_y, 4);
      Serial.println(accel_z, 4);
      Serial.println(gyro_x, 4);
      Serial.println(gyro_y, 4);
      Serial.println(gyro_z, 4);

      if (count % 2 == 1) {
        digitalWrite(ledPin, LOW); // changed from HIGH to LOW
      } else {
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