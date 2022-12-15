#include <ArduinoBLE.h>
#include <LSM6DS3.h>
#include <Wire.h>
#include <arduino-timer.h>

/// API References
// Serial
// https://www.arduino.cc/reference/en/language/functions/communication/serial/
// IMU
// https://github.com/Seeed-Studio/Seeed_Arduino_LSM6DS3
// BLE
// https://www.arduino.cc/reference/en/libraries/arduinoble/
// Timer
// https://github.com/contrem/arduino-timer

/// Status
// red led on: init failed
// blue led on: wait BLE connect
// blue led blink: BLE is transferring data
// green led on: serial connected

/// Macros
// Fireware version, change version every time modify code
#define VERSION "1.0.0"

// 3 LED
#define LED_RED LED_BUILTIN
#define LED_GREEN LEDG
#define LED_BLUE LEDB

// Read IMU data every 10 millisecond (100Hz)
#define IMU_RAW_FREQUENCY_HZ 100

/// Typedef
// 28 Bytes
typedef struct {
  uint32_t sequence_num;
  float accel_x_g;  // g
  float accel_y_g;
  float accel_z_g;
  float gyro_x_dps;  // degree per second
  float gyro_y_dps;
  float gyro_z_dps;
} IMU_DATA;

/// Global Variable
/// IMU setting
// Create a instance of class LSM6DS3
LSM6DS3 myIMU(I2C_MODE, 0x6A);  // I2C device address 0x6A

/// BLE setting
// BLEService ledService("19B10000-E8F2-537E-4F6C-D104768A1214"); // Bluetooth®
// Low Energy LED Service Matlab need 16bit UUID instead of 128bit UUID
BLEService imuService("aaaa");

// Bluetooth® Low Energy LED Switch Characteristic - custom 128-bit UUID, read
// and writable by central Attention: use BLEWriteWithoutResponse instead of
// BLEWrite (Reference:
// https://ww2.mathworks.cn/matlabcentral/answers/556798-ble-write-to-arduino-nano-33-ble-is-unsuccessful)
// BLENotify: remote clients will be able to get notifications if this
// characteristic changes
BLECharacteristic imuCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214",
                                    BLERead | BLENotify,
                                    sizeof(IMU_DATA));

BLEDescriptor imuDescriptor("2901", "imu");

/// Timer setting
auto timer = timer_create_default();  // create a timer with default settings

static IMU_DATA imu_data;

static unsigned int count = 0;

void init_led() {
  // set LED pin to output mode
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
}

void turn_on_red_led() {
  digitalWrite(LED_RED, LOW);  // LED on
}

void turn_off_red_led() {
  digitalWrite(LED_RED, HIGH);  // LED off
}

void turn_on_green_led() {
  digitalWrite(LED_GREEN, LOW);  // LED on
}

void turn_off_green_led() {
  digitalWrite(LED_GREEN, HIGH);  // LED off
}

void turn_on_blue_led() {
  digitalWrite(LED_BLUE, LOW);  // LED on
}

void turn_off_blue_led() {
  digitalWrite(LED_BLUE, HIGH);  // LED off
}

bool timer_callback(void *) {
  // read data
  imu_data.accel_x_g  = myIMU.readFloatAccelX();
  imu_data.accel_y_g  = myIMU.readFloatAccelY();
  imu_data.accel_z_g  = myIMU.readFloatAccelZ();
  imu_data.gyro_x_dps = myIMU.readFloatGyroX();
  imu_data.gyro_y_dps = myIMU.readFloatGyroY();
  imu_data.gyro_z_dps = myIMU.readFloatGyroZ();

  // send data through BLE
  imuCharacteristic.writeValue(&imu_data, sizeof(IMU_DATA));

  // print every 1s for debug
  if (imu_data.sequence_num % 100 == 0) {
    if (Serial) {
      // turn on green led if serial connected
      turn_on_green_led();

      Serial.println("\n---IMU RAW DATA---");
      Serial.print("sequence num:");
      Serial.println(imu_data.sequence_num);
      // 4 means precision, for example, xx.1234
      Serial.println(imu_data.accel_x_g, 4);
      Serial.println(imu_data.accel_y_g, 4);
      Serial.println(imu_data.accel_z_g, 4);
      Serial.println(imu_data.gyro_x_dps, 4);
      Serial.println(imu_data.gyro_y_dps, 4);
      Serial.println(imu_data.gyro_z_dps, 4);
    } else {
      // turn off green led if serial disconnected
      turn_off_green_led();
    }

    // Toggle LED status every 1 second, blue led blinks means BLE is
    // transferring data
    if (imu_data.sequence_num / 100 % 2 == 1) {
      turn_on_blue_led();
    } else {
      turn_off_blue_led();
    }
  }

  imu_data.sequence_num++;
}

void setup() {
  /// LED
  init_led();

  // init start, turn on red led
  turn_off_green_led();
  turn_off_blue_led();
  turn_on_red_led();

  /// Serial
  Serial.begin(115200, SERIAL_8N1);

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
    while (1)
      ;
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
  timer.every(1000 / IMU_RAW_FREQUENCY_HZ, timer_callback);

  Serial.println("BLE IMU Service Init Finish");

  // init success, turn off red led
  turn_off_red_led();
}

void loop() {
  // use green led indicate serial connect status
  if (Serial) {
    turn_on_green_led();
  } else {
    turn_off_green_led();
  }

  // wait BLE connect, turn on blue led
  turn_on_blue_led();

  // listen for Bluetooth® Low Energy peripherals to connect:
  BLEDevice central = BLE.central();

  // if a central is connected to peripheral:
  if (central) {
    // BLE connected, turn off blue led
    turn_off_blue_led();

    // print info only when serial port is connected
    if (Serial) {
      // print fireware version
      Serial.print("Arduino fireware version: ");
      Serial.println(VERSION);
      // print the central's MAC address:
      Serial.print("Connected to central: ");
      Serial.println(central.address());
    }

    // while the central is still connected to peripheral:
    while (central.connected()) {
      // start timer, execute timer_callback() at timer callback
      timer.tick();
    }

    // TODO: this will disable timer after disconnect once
    // timer.cancel();

    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
}
