#!/usr/bin/env python3

from pybind11_imufilter import ImuEskfFilter, Vector3d, RotationMatrix3d


filter = ImuEskfFilter()

output_matrix = RotationMatrix3d()
output_angular_velocity = Vector3d()

# Round 1
acc = Vector3d(2.7066, 1.1768, 9.9636)
gyro = Vector3d(-0.4268, -1.8550, -0.2162)

filter.Step(acc, gyro, output_matrix, output_angular_velocity)

# | 0.960598 -0.004287 0.277909|
# | -0.027320 0.993583 0.109757|
# | -0.276596 -0.113025 0.954317|
print(output_matrix)
# (-0.426800, -1.855000, -0.216200)
print(output_angular_velocity)

# Round 2
acc = Vector3d(3.4127, 1.4906, 11.6503)
gyro = Vector3d(-0.4962, -1.6732, -0.1400)

filter.Step(acc, gyro, output_matrix, output_angular_velocity)

# | 0.955880 -0.007543 0.293662|
# | -0.024562 0.994117 0.105486|
# | -0.292730 -0.108045 0.950071|
print(output_matrix)
# (-0.496199, -1.673193, -0.140001)
print(output_angular_velocity)
