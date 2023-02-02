#include "imufilter.h"

#include <iostream>

int main() {
  filter::ImuEskfFilter filter;

  filter::RotationMatrix3d output_orientation;
  filter::Vector3d output_angle_velocity;

  // round1
  filter::Vector3d input_acc{2.7066, 1.1768, 9.9636};
  filter::Vector3d input_gyro{-0.4268, -1.8550, -0.2162};

  filter.Step(input_acc, input_gyro, output_orientation, output_angle_velocity);

  // | 0.960598 -0.004287 0.277909|
  // | -0.027320 0.993583 0.109757|
  // | -0.276596 -0.113025 0.954317|
  std::cout << output_orientation.ToString() << std::endl;
  // (-0.426800, -1.855000, -0.216200)
  std::cout << output_angle_velocity.ToString() << std::endl;

  // round2
  input_acc  = {3.4127, 1.4906, 11.6503};
  input_gyro = {-0.4962, -1.6732, -0.1400};

  filter.Step(input_acc, input_gyro, output_orientation, output_angle_velocity);

  // | 0.955880 -0.007543 0.293662|
  // | -0.024562 0.994117 0.105486|
  // | -0.292730 -0.108045 0.950071|
  std::cout << output_orientation.ToString() << std::endl;
  // (-0.496199, -1.673193, -0.140001)
  std::cout << output_angle_velocity.ToString() << std::endl;

  return 0;
}

#if 0
//
// function [orientationMatrix,angularVelocity] = imufilter_wrapper(acc,gyro)
// ESKF fuse acc and gyro
//    acc  Nx3  m/s2
//    gyro Nx3  rad/s
// Arguments    : const coder::array<double, 2U> *acc
//                const coder::array<double, 2U> *gyro
//                coder::array<double, 3U> *orientationMatrix
//                coder::array<double, 2U> *angularVelocity
// Return Type  : void
//
void ImuFilterWrapper::imufilter_wrapper(const coder::array<double, 2U> &acc,
  const coder::array<double, 2U> &gyro, coder::array<double, 3U>
  &orientationMatrix, coder::array<double, 2U> &angularVelocity)
{
  c_fusion_internal_coder_imufilt ifilt;
  int i;
  fusion_internal_frames_NED lobj_1;
  double varargout_1[9];
  double varargout_2[3];

  //  initialize fiter
  // 'imufilter_wrapper:7' rawDataFreq = 100;
  // 'imufilter_wrapper:8' ifilt = imufilter('SampleRate', rawDataFreq, 'OrientationFormat', 'Rotation matrix'); 
  ifilt.init();

  // 'imufilter_wrapper:10' [sampleSize,~] = size(acc);
  // 'imufilter_wrapper:10' ~
  //  preallocate
  //  orientation = zeros([sampleSize, 1],'quaternion');  % not support in coder 
  // 'imufilter_wrapper:15' orientationMatrix = zeros(3,3,sampleSize);
  // 'imufilter_wrapper:16' angularVelocity = zeros(sampleSize, 3);
  // 'imufilter_wrapper:18' for i = 1:sampleSize
  i = acc.size(0);
  orientationMatrix.set_size(3, 3, acc.size(0));
  angularVelocity.set_size(acc.size(0), 3);
  for (int b_i = 0; b_i < i; b_i++) {
    // 'imufilter_wrapper:19' inputAcc = acc(i, :);
    // 'imufilter_wrapper:20' inputGyro = gyro(i, :);
    // 'imufilter_wrapper:22' [outputOrientation,outputAngularVelocity] = ifilt(inputAcc, inputGyro); 
    ifilt.step((*(double (*)[3])&acc[3 * b_i]), (*(double (*)[3])&gyro[3 * b_i]),
               (&lobj_1), varargout_1, varargout_2);

    // 'imufilter_wrapper:24' orientationMatrix(:,:,i) = outputOrientation;
    // 'imufilter_wrapper:25' angularVelocity(i,:) = outputAngularVelocity;
    for (int i1 = 0; i1 < 3; i1++) {
      orientationMatrix[b_i + orientationMatrix.size(2) * 3 * i1] = varargout_1
        [3 * i1];
      orientationMatrix[(b_i + orientationMatrix.size(2)) +
        orientationMatrix.size(2) * 3 * i1] = varargout_1[3 * i1 + 1];
      orientationMatrix[(b_i + orientationMatrix.size(2) * 2) +
        orientationMatrix.size(2) * 3 * i1] = varargout_1[3 * i1 + 2];
      angularVelocity[i1 + 3 * b_i] = varargout_2[i1];
    }
  }
}

// Function Definitions

//
// Arguments    : void
// Return Type  : void
//
ImuFilterWrapper::~ImuFilterWrapper()
{
  // (no terminate code required)
}

//
// Arguments    : void
// Return Type  : void
//
ImuFilterWrapper::ImuFilterWrapper()
{
  rt_InitInfAndNaN();
}

// Type Definitions
class ImuFilterWrapper
{
 public:
  ImuFilterWrapper();
  ~ImuFilterWrapper();
  void imufilter_wrapper(const coder::array<double, 2U> &acc, const coder::array<
    double, 2U> &gyro, coder::array<double, 3U> &orientationMatrix, coder::array<
    double, 2U> &angularVelocity);
};

#endif