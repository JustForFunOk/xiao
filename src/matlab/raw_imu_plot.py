import matplotlib.pyplot as plt
import numpy as np
import math
import csv

# filename = open('./raw_imu_data/imu_xyz90_rotation.csv', 'r')
# filename = open('./raw_imu_data/imu_shaking.csv', 'r')
# filename = open('./raw_imu_data/imu_high_frequency_vibration.csv', 'r')
# filename = open('./raw_imu_data/imu_stationary.csv', 'r')
# filename = open('./raw_imu_data/imu_plane_movement.csv', 'r')
filename = open('./raw_imu_data/imu_free_fall.csv', 'r')

file = csv.DictReader(filename)

sequence_num = []
accel_x = []
accel_y = []
accel_z = []
gyro_x = []
gyro_y = []
gyro_z = []

G : float = 9.8

for row in file:
    sequence_num.append(int(row['seq_num']))
    accel_x.append(G * float(row['accel_x_g']))
    accel_y.append(G * float(row['accel_y_g']))
    accel_z.append(G * float(row['accel_z_g']))
    gyro_x.append(math.radians(float(row['gyro_x_dps'])))
    gyro_y.append(math.radians(float(row['gyro_y_dps'])))
    gyro_z.append(math.radians(float(row['gyro_z_dps'])))


plt.plot(sequence_num, accel_x, 'r', label='accel_x$(m/s^2)$')
plt.plot(sequence_num, accel_y, 'g', label='accel_y$(m/s^2)$')
plt.plot(sequence_num, accel_z, 'b', label='accel_z$(m/s^2)$')
plt.plot(sequence_num, gyro_x, 'c', label='gyro_x$(rad/s)$')
plt.plot(sequence_num, gyro_y, 'm', label='gyro_y$(rad/s)$')
plt.plot(sequence_num, gyro_z, 'y', label='gyro_z$(rad/s)$')
plt.xlabel('Sequence Num (100Hz)')
plt.legend()
plt.show()
