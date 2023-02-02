// https://www.mathworks.com/help/nav/ref/imufilter-system-object.html
#pragma once

#include <array>
#include <string>

namespace filter {

struct Vector3d {
  double x;
  double y;
  double z;

  Vector3d() : x(0.0), y(0.0), z(0.0) {}
  Vector3d(double _x, double _y, double _z) : x(_x), y(_y), z(_z) {}

  std::string ToString() const {
    return "(" + std::to_string(x) + ", " + std::to_string(y) + ", " +
           std::to_string(z) + ")\n";
  }
};

struct RotationMatrix3d {
  std::array<double, 9> data;

  std::string ToString() const {
    return "| " + std::to_string(data[0]) + " " + std::to_string(data[1]) +
           " " + std::to_string(data[2]) + "|\n" + "| " +
           std::to_string(data[3]) + " " + std::to_string(data[4]) + " " +
           std::to_string(data[5]) + "|\n" + "| " + std::to_string(data[6]) +
           " " + std::to_string(data[7]) + " " + std::to_string(data[8]) +
           "|\n";
  }
};

class ImuEskfFilter {
 public:
  /// @brief Init parameter.
  ImuEskfFilter();

  /// @brief handle input acc and gyro data and output orientation and angle
  /// velocity.
  /// @param input_acc description
  /// @param input_gyro description
  /// @param output_rpy description
  /// @param output_angular_velocity description
  void Step(const Vector3d& input_acc,
            const Vector3d& input_gyro,
            RotationMatrix3d& output_rot,
            Vector3d& output_angular_velocity);
};

}  // namespace filter
