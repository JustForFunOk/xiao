# https://tutorialedge.net/python/concurrency/asyncio-event-loops-tutorial/

# Reference: https://github.com/Ladvien/arduino_ble_sense

import os, sys
import asyncio
import platform
import numpy as np
from datetime import datetime
from typing import Callable, Any

from aioconsole import ainput
from bleak import BleakClient, BleakScanner

selected_device = []


class BLEDataFormat:
    """The format of data transmit from ble service.
    """

    def __init__(self) -> None:
        self.sequence_num = np.uint32(0)
        self.accel_x_g = np.float32(0.0)
        self.accel_y_g = np.float32(0.0)
        self.accel_z_g = np.float32(0.0)
        self.gyro_x_dps = np.float32(0.0)
        self.gyro_y_dps = np.float32(0.0)
        self.gyro_z_dps = np.float32(0.0)

    def __str__(self) -> str:
        """Define the output of print().

        Refer to https://peps.python.org/pep-0498/ for f-string.
        Refer to https://docs.python.org/3/library/string.html for string format.
        """
        return f'seq_num:{self.sequence_num :d} accel_x_g:{self.accel_x_g :+.3f} accel_y_g:{self.accel_y_g :+.3f} accel_z_g:{self.accel_z_g :+.3f} gyro_x_dps:{self.gyro_x_dps :+.3f} gyro_y_dps:{self.gyro_y_dps :+.3f} gyro_z_dps:{self.gyro_z_dps :+.3f}'

    def csv_header(self) -> str:
        """Return csv header.
        """
        return 'seq_num,accel_x_g,accel_y_g,accel_z_g,gyro_x_dps,gyro_y_dps,gyro_z_dps\n'

    def csv_data(self) -> str:
        """Return data csv format.
        """
        return f'{self.sequence_num :d},{self.accel_x_g :f},{self.accel_y_g :f},{self.accel_z_g :f},{self.gyro_x_dps :f},{self.gyro_y_dps :f},{self.gyro_z_dps :f}\n'


class DataParser:
    """Parse binary data stream into meaningful data.

    /// Message format definition
    // 28 Bytes
    typedef struct {
      uint32_t sequence_num;
      float accel_x_g;
      float accel_y_g;
      float accel_z_g;
      float gyro_x_dps;
      float gyro_y_dps;
      float gyro_z_dps;
    } IMU_DATA;

    """

    def __init__(self) -> None:
        self.parsed_data: BLEDataFormat = BLEDataFormat()

    def parse(self, data: bytearray) -> BLEDataFormat:
        """Parse binary ble data.

        Args:
            data:
                The input binary ble data.

        Returns:
            The parsed data.
        """
        # sequence number
        sequence_bytes = np.array(data[0:4], dtype=np.uint8)
        self.parsed_data.sequence_num = sequence_bytes.view(dtype=np.uint32)[0]

        # IMU data
        data_bytes = np.array(data[4:], dtype=np.uint8)
        data_as_float = data_bytes.view(dtype=np.float32)
        self.parsed_data.accel_x_g = data_as_float[0]
        self.parsed_data.accel_y_g = data_as_float[1]
        self.parsed_data.accel_z_g = data_as_float[2]
        self.parsed_data.gyro_x_dps = data_as_float[3]
        self.parsed_data.gyro_y_dps = data_as_float[4]
        self.parsed_data.gyro_z_dps = data_as_float[5]

        return self.parsed_data


class DataToFile:
    """Store IMU data to csv file.
    """

    def __init__(self, write_path):
        self.file_path = write_path
        self.file_handler = None

    def create_csv_file(self):
        self.file_handler = open(self.file_path, "w", encoding='utf_8')
        print(f'Created file: {self.file_path}.')

    def write_to_csv(self, data_values: list[str]):
        """Write string to csv file.
        """
        if self.file_handler is None:
            print("Please create csv file first.")
            return

        for data in data_values:
            self.file_handler.write(data)


class Connection:
    """ BLE connection manager.
    """

    def __init__(
        self,
        loop: asyncio.AbstractEventLoop,
        read_characteristic: str,
        write_characteristic: str,
        data_parser: DataParser,
        data_dump_handler: DataToFile,
        data_dump_size: int = 50,
    ):
        """Construction function.

        Args:
            data_dump_size: The size of data cache.
        """
        self.loop = loop
        self.read_characteristic = read_characteristic
        self.write_characteristic = write_characteristic
        self.data_parser = data_parser
        self.data_dump_handler = data_dump_handler

        self.last_packet_time = datetime.now()
        self.dump_size = data_dump_size
        self.connected_device = None

        self.rx_data: list[str] = []
        self.parsed_data = BLEDataFormat()
        self.client: BleakClient = None

    # execute at the last
    def disconnect_callback(self, client: BleakClient):
        # Put code here to handle what happens on disconnect.
        print(f"Disconnected from {self.connected_device.name}!")

    async def cleanup(self):
        if self.client:
            # Notifications are stopped automatically on disconnect,
            # so this method does not need to be called unless
            # notifications need to be stopped some time before the
            # device disconnects.
            # await self.client.stop_notify(read_characteristic)
            await self.client.disconnect()

    async def manager(self):
        print("Starting scan bluetooth devices...")
        while True:
            if self.client:
                await self.connect()
            else:
                await self.scan_and_select_device()

    async def connect(self):
        if self.client.is_connected:
            return
        try:
            await self.client.connect()
            if self.client.is_connected:
                print(F"Connected to {self.connected_device.name}")

                # Connect success, create csv file
                self.data_dump_handler.create_csv_file()
                # Write csv header.
                self.data_dump_handler.write_to_csv(self.parsed_data.csv_header())

                await self.client.start_notify(
                    self.read_characteristic,
                    self.ble_notify_msg_callback,
                )
                while True:
                    if not self.client.is_connected:
                        break
                    await asyncio.sleep(3.0)
            else:
                print(f"Failed to connect to {self.connected_device.name}")
        except Exception as e:
            print(e)

    async def scan_and_select_device(self):
        devices = await BleakScanner.discover(timeout=3.0, return_adv=False)

        # Remove 'None' name devices
        # Must create copy of list when remove element from list while iterating,
        # because remove() changes the index of remain elements.
        for device in list(devices):
            if device.name is None:
                devices.remove(device)

        # Sort devices list by name
        devices.sort(key=lambda d: d.name)

        print("Please select device: ")
        for i, device in enumerate(devices):
            print(f"{i}: {device.name}")

        response = -1
        while True:
            response = await ainput("Select device: ")
            try:
                response = int(response.strip())
            except:
                print("Please make valid selection.")

            if response > -1 and response < len(devices):
                break
            else:
                print("Please make valid selection.")

        print(f"Connecting to {devices[response].name} ...")
        self.connected_device = devices[response]
        self.client = BleakClient(
            devices[response].address,
            disconnected_callback=self.disconnect_callback)

    def ble_notify_msg_callback(self, sender: str, data: bytearray):
        """Callback function of ble notify.
        """
        # Parse data.
        self.parsed_data = self.data_parser.parse(data)
        # print(self.parsed_data)

        # Append to data cache list.
        self.rx_data.append(self.parsed_data.csv_data())

        # Write to csv file.
        if len(self.rx_data) >= self.dump_size:
            self.data_dump_handler.write_to_csv(self.rx_data)
            self.rx_data.clear()


#############
# Send msg to ble server
#############
# async def user_console_manager(connection: Connection):
#     while True:
#         if connection.client and connection.connected:
#             input_str = await ainput("Enter string: ")
#             bytes_to_send = bytearray(map(ord, input_str))
#             await connection.client.write_gatt_char(write_characteristic,
#                                                     bytes_to_send)
#             print(f"Sent: {input_str}")
#         else:
#             await asyncio.sleep(2.0, loop=loop)

#############
# App Main
#############
read_characteristic = "19B10001-E8F2-537E-4F6C-D104768A1214"
write_characteristic = ""


async def main():
    while True:
        # YOUR APP CODE WOULD GO HERE.
        await asyncio.sleep(5)


if __name__ == "__main__":

    # Create the event loop.
    loop = asyncio.get_event_loop()

    # Save msg to file
    root_path = os.environ["HOME"]
    time = datetime.now().strftime('%y%m%d_%H%M%S')  # 221211_151220
    output_file = f'{root_path}/Desktop/imu_{time}.csv'
    data_to_file = DataToFile(output_file)

    # Data parser
    parser = DataParser()

    connection = Connection(loop, read_characteristic, write_characteristic,
                            parser, data_to_file)
    try:
        asyncio.ensure_future(connection.manager())

        # disable send feature, only receive msg from service
        # asyncio.ensure_future(user_console_manager(connection))

        asyncio.ensure_future(main())
        loop.run_forever()
    except KeyboardInterrupt:
        print()
        print("User stopped program.")
    finally:
        print("Disconnecting...")
        loop.run_until_complete(connection.cleanup())
