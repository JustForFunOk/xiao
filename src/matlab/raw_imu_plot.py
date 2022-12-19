import matplotlib.pyplot as plt
import numpy as np
import math
import csv

# filename = open('./raw_imu_data/imu_xyz90_rotation.csv', 'r')
# filename = open('./raw_imu_data/imu_shaking.csv', 'r')
filename = open('./raw_imu_data/imu_high_frequency_vibration.csv', 'r')

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


plt.plot(sequence_num, accel_x, 'r', label='accel_x_m_s2')
plt.plot(sequence_num, accel_y, 'g', label='accel_y_m_s2')
plt.plot(sequence_num, accel_z, 'b', label='accel_z_m_s2')
plt.plot(sequence_num, gyro_x, 'c', label='gyro_x_radps')
plt.plot(sequence_num, gyro_y, 'm', label='gyro_y_radps')
plt.plot(sequence_num, gyro_z, 'y', label='gyro_z_radps')
plt.legend()
plt.show()
