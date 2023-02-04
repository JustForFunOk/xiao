#!/usr/bin/env python3
import matplotlib.pyplot as plt
import numpy as np
import math
import csv
from scipy.spatial.transform import Rotation
import os

from pybind11_imufilter import ImuEskfFilter, Vector3d, RotationMatrix3d

current_dir = os.path.dirname(__file__)

# RELATIVE_PATH = '../../data/raw_imu_data/imu_plane_movement.csv'
# RELATIVE_PATH = '../../data/raw_imu_data/imu_shaking.csv'
# RELATIVE_PATH = '../../data/raw_imu_data/imu_high_frequency_vibration.csv'
RELATIVE_PATH = '../../data/raw_imu_data/imu_stationary.csv'
# RELATIVE_PATH = '../../data/raw_imu_data/imu_plane_movement.csv'
# RELATIVE_PATH = '../../data/raw_imu_data/imu_free_fall.csv'

full_filename = os.path.join(current_dir, RELATIVE_PATH)
_, tail_name = os.path.split(full_filename)

file_handler = open(full_filename, mode='r', encoding='utf8')
file = csv.DictReader(file_handler)

G : float = 9.8

sequence_num_list = []
input_accel_x_list = []
input_accel_y_list = []
input_accel_z_list = []
input_gyro_x_list = []
input_gyro_y_list = []
input_gyro_z_list = []

output_orientation_x_list = []
output_orientation_y_list = []
output_orientation_z_list = []
output_angular_velocity_x_list = []
output_angular_velocity_y_list = []
output_angular_velocity_z_list = []

# init filter
imu_filter = ImuEskfFilter()

# read raw data and process
for row in file:
    # read from csv file
    sequence_num = int(row['seq_num'])
    input_acc_x = G * float(row['accel_x_g'])
    input_acc_y = G * float(row['accel_y_g'])
    input_acc_z = G * float(row['accel_z_g'])
    input_gyro_x = math.radians(float(row['gyro_x_dps']))
    input_gyro_y = math.radians(float(row['gyro_y_dps']))
    input_gyro_z = math.radians(float(row['gyro_z_dps']))

    # construct input and output
    input_acc_m_s2 = Vector3d(input_acc_x, input_acc_y, input_acc_z)
    input_gyro_rad_s = Vector3d(input_gyro_x, input_gyro_y, input_gyro_z)
    output_matrix = RotationMatrix3d()
    output_angular_velocity = Vector3d()

    # TODO: handle frame loss by sequence number

    # filter core step
    imu_filter.Step(input_acc_m_s2, input_gyro_rad_s, output_matrix, output_angular_velocity)

    # convert rotation matrix to euler angle
    r =  Rotation.from_matrix(np.array(output_matrix.data).reshape(3,3))
    angles = r.as_euler("xyz",degrees=True)
    output_orientation_x = angles[0]
    output_orientation_y = angles[1]
    output_orientation_z = angles[2]

    # store data to visualize
    sequence_num_list.append(sequence_num)
    input_accel_x_list.append(input_acc_x)
    input_accel_y_list.append(input_acc_y)
    input_accel_z_list.append(input_acc_z)
    input_gyro_x_list.append(input_gyro_x)
    input_gyro_y_list.append(input_gyro_x)
    input_gyro_z_list.append(input_gyro_x)
    output_orientation_x_list.append(output_orientation_x)
    output_orientation_y_list.append(output_orientation_y)
    output_orientation_z_list.append(output_orientation_z)
    output_angular_velocity_x_list.append(output_angular_velocity.x)
    output_angular_velocity_y_list.append(output_angular_velocity.y)
    output_angular_velocity_z_list.append(output_angular_velocity.z)

# visualize result
plt.figure(tail_name)

plt.subplot(2, 1, 1)
plt.plot(sequence_num_list, input_accel_x_list, 'r', label='raw_accel_x$(m/s^2)$')
plt.plot(sequence_num_list, input_accel_y_list, 'g', label='raw_accel_y$(m/s^2)$')
plt.plot(sequence_num_list, input_accel_z_list, 'b', label='raw_accel_z$(m/s^2)$')
plt.plot(sequence_num_list, output_orientation_x_list, 'r--', label='orientation_x$(\degree)$')
plt.plot(sequence_num_list, output_orientation_y_list, 'g--', label='orientation_y$(\degree)$')
plt.plot(sequence_num_list, output_orientation_z_list, 'b--', label='orientation_z$(\degree)$')
plt.xlabel('Sequence Num (100Hz)')
plt.legend()

plt.subplot(2, 3, 4)
plt.plot(sequence_num_list, input_gyro_x_list, 'r', label='raw_gyro_x$(rad/s)$')
plt.plot(sequence_num_list, output_angular_velocity_x_list, 'c', label='remove_bias_gyro_x$(rad/s)$')
plt.xlabel('Sequence Num (100Hz)')
plt.legend()

plt.subplot(2, 3, 5)
plt.plot(sequence_num_list, input_gyro_y_list, 'g', label='raw_gyro_y$(rad/s)$')
plt.plot(sequence_num_list, output_angular_velocity_y_list, 'm', label='remove_bias_gyro_y$(rad/s)$')
plt.xlabel('Sequence Num (100Hz)')
plt.legend()

plt.subplot(2, 3, 6)
plt.plot(sequence_num_list, input_gyro_z_list, 'b', label='raw_gyro_z$(rad/s)$')
plt.plot(sequence_num_list, output_angular_velocity_z_list, 'y', label='remove_bias_gyro_z$(rad/s)$')
plt.xlabel('Sequence Num (100Hz)')
plt.legend()

plt.subplots_adjust(left=0.05,
                    bottom=0.05,
                    right=0.95,
                    top=0.95,
                    wspace=0.2,
                    hspace=0.3)
plt.show()
