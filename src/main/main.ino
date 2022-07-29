#include <ArduinoBLE.h>
#include <LSM6DS3.h>
#include <Wire.h>
#include <arduino-timer.h>

/// Macros
// Read IMU data every 10 millisecond (100Hz)
#define IMU_RAW_FREQUENCY_HZ  100

/// Typedef
// 28 Bytes
typedef struct {
  uint32_t sequence_num;
  float accel_x;
  float accel_y;
  float accel_z;
  float gyro_x;
  float gyro_y;
  float gyro_z;
} IMU_DATA;

/// Global Variable
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
BLECharacteristic imuCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify, sizeof(IMU_DATA));

BLEDescriptor imuDescriptor("2901", "imu");

/// Timer setting
auto timer = timer_create_default(); // create a timer with default settings

const int ledPin = LED_BUILTIN; // pin to use for the LED

static IMU_DATA imu_data;

static unsigned int count = 0;

bool timer_callback(void *) {
  // read data
  imu_data.accel_x = myIMU.readFloatAccelX();
  imu_data.accel_y = myIMU.readFloatAccelY();
  imu_data.accel_z = myIMU.readFloatAccelZ();
  imu_data.gyro_x  = myIMU.readFloatGyroX();
  imu_data.gyro_y  = myIMU.readFloatGyroY();
  imu_data.gyro_z  = myIMU.readFloatGyroZ();

  // send data
  imuCharacteristic.writeValue(&imu_data, sizeof(IMU_DATA));

  // print every 1s for debug
  if(imu_data.sequence_num % 100 == 0) {
    Serial.println("\n---IMU RAW DATA---");
    Serial.print("sequence num:");
    Serial.println(imu_data.sequence_num);
    // 4 means precision, for example, xx.1234
    Serial.println(imu_data.accel_x, 4);
    Serial.println(imu_data.accel_y, 4);
    Serial.println(imu_data.accel_z, 4);
    Serial.println(imu_data.gyro_x, 4);
    Serial.println(imu_data.gyro_y, 4);
    Serial.println(imu_data.gyro_z, 4);

    // Toggle LED status every 1 second
    if (imu_data.sequence_num / 100 % 2 == 1) {
      digitalWrite(ledPin, LOW); // changed from HIGH to LOW
    } else {
      digitalWrite(ledPin, HIGH); // changed from LOW to HIGH
    }
  }

  imu_data.sequence_num++;
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

    // TODO: this will disable timer after disconnect once
    // timer.cancel();

    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
}
