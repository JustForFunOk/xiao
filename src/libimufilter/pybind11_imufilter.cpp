#include <pybind11/pybind11.h>

#include "imufilter.h"

namespace py = pybind11;

PYBIND11_MODULE(pybind11_imufilter, m) {
  // struct Vector3d {
  //   double x;
  //   double y;
  //   double z;

  //   std::string ToString() const;
  // };
  py::class_<::filter::Vector3d>(m, "Vector3d")
      .def(py::init<>())  // C++会默认生成构造函数等，但是这里要显式的声明
      .def_readwrite("x", &::filter::Vector3d::x)
      .def_readwrite("y", &::filter::Vector3d::y)
      .def_readwrite("z", &::filter::Vector3d::z)
      .def("__str__", &::filter::Vector3d::ToString);

  // struct RotationMatrix3d {
  //   double data[9];

  //   std::string ToString() const;
  // };

  py::class_<::filter::RotationMatrix3d>(m, "RotationMatrix3d")
      .def(py::init<>())  // C++会默认生成构造函数等，但是这里要显式的声明
      .def_readwrite("data", &::filter::RotationMatrix3d::data)
      .def("__str__", &::filter::RotationMatrix3d::ToString);

  // class ImuEskfFilter {
  //  public:
  //   /// @brief Init parameter.
  //   ImuEskfFilter();

  //   void Step(const Vector3d& input_acc,
  //             const Vector3d& input_gyro,
  //             RotationMatrix3d& output_rot,
  //             Vector3d& output_angular_velocity);
  // };
  py::class_<::filter::ImuEskfFilter>(m, "ImuEskfFilter")
      .def(py::init<>())
      .def("Step", &::filter::ImuEskfFilter::Step);
}