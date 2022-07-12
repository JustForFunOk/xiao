#include <ArduinoBLE.h>
#include <LSM6DS3.h>
#include <Wire.h>
#include <arduino-timer.h>

// Read IMU data every 10 millisecond (100Hz)
#define IMU_RAW_FREQUENCY_HZ  100

/// IMU setting
// Create a instance of class LSM6DS3
LSM6DS3 myIMU(I2C_MODE, 0x6A);    //I2C device address 0x6A

/// BLE setting
// BLEService ledService("19B10000-E8F2-537E-4F6C-D104768A1214"); // Bluetooth® Low Energy LED Service
// Matlab need 16bit UUID instead of 128bit UUID
BLEService imuService("aaaa");

// Bluetooth® Low Energy LED Switch Characteristic - custom 128-bit UUID, read and writable by central
// Attention: use BLEWriteWithoutResponse instead of BLEWrite (Reference: https://ww2.mathworks.cn/matlabcentral/answers/556798-ble-write-to-arduino-nano-33-ble-is-unsuccessful)
// BLENotify: remote clients will be able to get notifications if this characteristic changes
BLEFloatCharacteristic imuCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify);

BLEDescriptor imuDescriptor("2901", "imu");

/// Timer setting
auto timer = timer_create_default(); // create a timer with default settings

const int ledPin = LED_BUILTIN; // pin to use for the LED

// Global variables
static float accel_x;
static float accel_y;
static float accel_z;
static float gyro_x;
static float gyro_y;
static float gyro_z;

static unsigned int count = 0;

bool timer_callback(void *) {
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

  // print every 1s for debug
  if(count % 100 == 0) {
    Serial.println("\n---IMU RAW DATA---");
    Serial.print("sequence num:");
    Serial.println(count);
    // 4 means precision, for example, xx.1234
    Serial.println(accel_x, 4);
    Serial.println(accel_y, 4);
    Serial.println(accel_z, 4);
    Serial.println(gyro_x, 4);
    Serial.println(gyro_y, 4);
    Serial.println(gyro_z, 4);

    // Toggle LED status every 1 second
    if (count / 100 % 2 == 1) {
      digitalWrite(ledPin, LOW); // changed from HIGH to LOW
    } else {
      digitalWrite(ledPin, HIGH); // changed from LOW to HIGH
    }
  }

  count++;
}

void setup() {
  Serial.begin(9600);
  // For loop until open serial monitor
  while (!Serial);

  /// IMU
  // Call .begin() to configure the IMUs
  if (myIMU.begin() != 0) {
      Serial.println("IMU Device error");
  } else {
      Serial.println("IMU Device OK!");
  }

  /// BLE
  // Begin initialization BLE
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy module failed!");
    while (1);
  }

  // Set advertised local name and service UUID:
  BLE.setLocalName("IMU");
  BLE.setAdvertisedService(imuService);

  imuCharacteristic.addDescriptor(imuDescriptor);

  // Add the characteristic to the service
  imuService.addCharacteristic(imuCharacteristic);

  // Add service
  BLE.addService(imuService);

  // Start advertising
  BLE.advertise();

  /// Timer
  timer.every(1000/IMU_RAW_FREQUENCY_HZ, timer_callback);

  /// LED
  // set LED pin to output mode
  pinMode(ledPin, OUTPUT);

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
      // start timer
      timer.tick();
    }

    timer.cancel();

    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
}