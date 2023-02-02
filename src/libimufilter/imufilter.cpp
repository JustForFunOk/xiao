//
// File: imufilter_wrapper.cpp
//
// MATLAB Coder version            : 5.0
// C/C++ source code generated on  : 15-Jan-2023 18:29:27
//

// Include Files
#include "imufilter.h"
#include "coder_array.h"
#include "rt_nonfinite.h"

#include <cmath>
#include <cstring>

// Type Definitions
class fusion_internal_frames_NED {
 public:
  fusion_internal_frames_NED *init();
  static void ecompass(const double a[3], double R[9]);
};

static fusion_internal_frames_NED lobj_1;

class ReferenceFrame {
 public:
  static fusion_internal_frames_NED *getMathObject(
      fusion_internal_frames_NED *iobj_0);
};

class c_matlabshared_rotations_intern {
 public:
  void init(double varargin_1,
            double varargin_2,
            double varargin_3,
            double varargin_4);
  void init(const double varargin_1[9]);
  void b_init(const double varargin_1[3]);
  c_matlabshared_rotations_intern mtimes(
      const c_matlabshared_rotations_intern y) const;
  c_matlabshared_rotations_intern times(
      const c_matlabshared_rotations_intern y) const;
  double parts() const;
  void rotmat(double r[9]) const;
  void b_init(double t0_a, double t0_b, double t0_c, double t0_d);
  void uminus();
  void normalize();
  void b_conj();

 protected:
  double a;
  double b;
  double c;
  double d;
};

struct cell_wrap_3 {
  unsigned int f1[8];
};

class c_fusion_internal_coder_imufilt {
 public:
  c_fusion_internal_coder_imufilt *init();
  void step(const double varargin_1[3],
            const double varargin_2[3],
            fusion_internal_frames_NED *iobj_0,
            double varargout_1[9],
            double varargout_2[3]);
  void setupAndReset(const double varargin_1[3],
                     fusion_internal_frames_NED *iobj_0);
  void setup(const double varargin_1[3], fusion_internal_frames_NED *iobj_0);
  void checkTunableProps();
  double AccelerometerNoise;
  double GyroscopeNoise;
  double GyroscopeDriftNoise;
  double LinearAccelerationNoise;
  double LinearAccelerationDecayFactor;

 protected:
  double pQw[81];
  double pQv[9];
  c_matlabshared_rotations_intern pOrientPost;
  c_matlabshared_rotations_intern pOrientPrior;
  boolean_T pFirstTime;
  fusion_internal_frames_NED *pRefSys;
  double pSensorPeriod;
  double pKalmanPeriod;
  double pGyroOffset[3];
  double pLinAccelPrior[3];
  double pLinAccelPost[3];
  double pInputPrototype[3];

 private:
  int isInitialized;
  boolean_T TunablePropsChanged;
  cell_wrap_3 inputVarSize[2];
};

static c_fusion_internal_coder_imufilt ifilt_;

// Function Declarations
static double rt_powd_snf(double u0, double u1);

//
// Arguments    : void
// Return Type  : void
//
void c_matlabshared_rotations_intern::b_conj() {
  this->b = -this->b;
  this->c = -this->c;
  this->d = -this->d;
}

//
// Arguments    : const double varargin_1[3]
// Return Type  : void
//
void c_matlabshared_rotations_intern::b_init(const double varargin_1[3]) {
  double theta;
  double st;
  this->a = 1.0;
  this->b = 0.0;
  this->c = 0.0;
  this->d = 0.0;
  theta   = std::sqrt(
      (rt_powd_snf(varargin_1[0], 2.0) + rt_powd_snf(varargin_1[1], 2.0)) +
      rt_powd_snf(varargin_1[2], 2.0));
  st = std::sin(theta / 2.0);
  if (theta != 0.0) {
    this->a = std::cos(theta / 2.0);
    this->b = varargin_1[0] / theta * st;
    this->c = varargin_1[1] / theta * st;
    this->d = varargin_1[2] / theta * st;
  }
}

//
// Arguments    : double t0_a
//                double t0_b
//                double t0_c
//                double t0_d
// Return Type  : void
//
void c_matlabshared_rotations_intern::b_init(double t0_a,
                                             double t0_b,
                                             double t0_c,
                                             double t0_d) {
  this->a = t0_a;
  this->b = t0_b;
  this->c = t0_c;
  this->d = t0_d;
}

//
// Arguments    : void
// Return Type  : void
//
void c_fusion_internal_coder_imufilt::checkTunableProps() {
  signed char b_I[9];
  if (this->TunablePropsChanged) {
    double accelMeasNoiseVar;
    int i;
    this->TunablePropsChanged = false;
    accelMeasNoiseVar         = this->pKalmanPeriod;
    accelMeasNoiseVar =
        (this->AccelerometerNoise + this->LinearAccelerationNoise) +
        accelMeasNoiseVar * accelMeasNoiseVar *
            (this->GyroscopeDriftNoise + this->GyroscopeNoise);
    for (i = 0; i < 9; i++) {
      b_I[i] = 0;
    }

    b_I[0] = 1;
    b_I[4] = 1;
    b_I[8] = 1;
    for (i = 0; i < 9; i++) {
      this->pQv[i] = accelMeasNoiseVar * static_cast<double>(b_I[i]);
    }
  }
}

//
// Arguments    : const double a[3]
//                double R[9]
// Return Type  : void
//
void fusion_internal_frames_NED::ecompass(const double a[3], double R[9]) {
  double Reast_idx_0;
  double Reast_idx_1;
  double Reast_idx_2;
  int Reast_tmp;
  double x[9];
  int k;
  boolean_T y[3];
  boolean_T b[9];
  boolean_T exitg1;
  boolean_T nanPageIdx;
  Reast_idx_0 = a[1] * 0.0 - a[2] * 0.0;
  Reast_idx_1 = a[2] - a[0] * 0.0;
  Reast_idx_2 = a[0] * 0.0 - a[1];
  R[2]        = a[0];
  R[1]        = Reast_idx_0;
  R[5]        = a[1];
  R[4]        = Reast_idx_1;
  R[8]        = a[2];
  R[7]        = Reast_idx_2;
  R[0]        = Reast_idx_1 * a[2] - Reast_idx_2 * a[1];
  R[3]        = Reast_idx_2 * a[0] - Reast_idx_0 * a[2];
  R[6]        = Reast_idx_0 * a[1] - Reast_idx_1 * a[0];
  for (Reast_tmp = 0; Reast_tmp < 9; Reast_tmp++) {
    x[Reast_tmp] = R[Reast_tmp] * R[Reast_tmp];
  }

  Reast_idx_0 = x[0];
  Reast_idx_1 = x[1];
  Reast_idx_2 = x[2];
  for (k = 0; k < 2; k++) {
    Reast_tmp = 3 * (k + 1);
    Reast_idx_0 += x[Reast_tmp];
    Reast_idx_1 += x[Reast_tmp + 1];
    Reast_idx_2 += x[Reast_tmp + 2];
  }

  Reast_idx_0 = std::sqrt(Reast_idx_0);
  Reast_idx_1 = std::sqrt(Reast_idx_1);
  Reast_idx_2 = std::sqrt(Reast_idx_2);
  std::memcpy(&x[0], &R[0], 9U * sizeof(double));
  for (k = 0; k < 3; k++) {
    R[3 * k]     = x[3 * k] / Reast_idx_0;
    Reast_tmp    = 3 * k + 1;
    R[Reast_tmp] = x[Reast_tmp] / Reast_idx_1;
    Reast_tmp    = 3 * k + 2;
    R[Reast_tmp] = x[Reast_tmp] / Reast_idx_2;
  }

  for (Reast_tmp = 0; Reast_tmp < 9; Reast_tmp++) {
    b[Reast_tmp] = std::isnan(R[Reast_tmp]);
  }

  y[0]   = false;
  k      = 0;
  exitg1 = false;
  while ((!exitg1) && (k < 3)) {
    if (b[3 * k]) {
      y[0]   = true;
      exitg1 = true;
    } else {
      k++;
    }
  }

  y[1]   = false;
  k      = 0;
  exitg1 = false;
  while ((!exitg1) && (k < 3)) {
    if (b[3 * k + 1]) {
      y[1]   = true;
      exitg1 = true;
    } else {
      k++;
    }
  }

  y[2]   = false;
  k      = 0;
  exitg1 = false;
  while ((!exitg1) && (k < 3)) {
    if (b[3 * k + 2]) {
      y[2]   = true;
      exitg1 = true;
    } else {
      k++;
    }
  }

  nanPageIdx = false;
  k          = 0;
  exitg1     = false;
  while ((!exitg1) && (k < 3)) {
    if (y[k]) {
      nanPageIdx = true;
      exitg1     = true;
    } else {
      k++;
    }
  }

  if (nanPageIdx) {
    std::memset(&R[0], 0, 9U * sizeof(double));
    R[0] = 1.0;
    R[4] = 1.0;
    R[8] = 1.0;
  }
}

//
// Arguments    : fusion_internal_frames_NED *iobj_0
// Return Type  : fusion_internal_frames_NED *
//
fusion_internal_frames_NED *ReferenceFrame::getMathObject(
    fusion_internal_frames_NED *iobj_0) {
  return iobj_0->init();
}

//
// Arguments    : double varargin_1
//                double varargin_2
//                double varargin_3
//                double varargin_4
// Return Type  : void
//
void c_matlabshared_rotations_intern::init(double varargin_1,
                                           double varargin_2,
                                           double varargin_3,
                                           double varargin_4) {
  this->a = varargin_1;
  this->b = varargin_2;
  this->c = varargin_3;
  this->d = varargin_4;
}

//
// Arguments    : const double varargin_1[9]
// Return Type  : void
//
void c_matlabshared_rotations_intern::init(const double varargin_1[9]) {
  double pd;
  double psquared[4];
  int idx;
  int k;
  double b_d;
  double d1;
  double d2;
  double d3;
  pd          = (varargin_1[0] + varargin_1[4]) + varargin_1[8];
  psquared[0] = (2.0 * pd + 1.0) - pd;
  psquared[1] = (2.0 * varargin_1[0] + 1.0) - pd;
  psquared[2] = (2.0 * varargin_1[4] + 1.0) - pd;
  psquared[3] = (2.0 * varargin_1[8] + 1.0) - pd;
  if (!std::isnan(psquared[0])) {
    idx = 1;
  } else {
    boolean_T exitg1;
    idx    = 0;
    k      = 2;
    exitg1 = false;
    while ((!exitg1) && (k < 5)) {
      if (!std::isnan(psquared[k - 1])) {
        idx    = k;
        exitg1 = true;
      } else {
        k++;
      }
    }
  }

  if (idx == 0) {
    pd  = psquared[0];
    idx = 1;
  } else {
    int i;
    pd = psquared[idx - 1];
    i  = idx + 1;
    for (k = i; k < 5; k++) {
      b_d = psquared[k - 1];
      if (pd < b_d) {
        pd  = b_d;
        idx = k;
      }
    }
  }

  switch (idx) {
    case 1:
      pd  = std::sqrt(pd);
      d1  = 0.5 * pd;
      pd  = 0.5 / pd;
      d2  = pd * (varargin_1[5] - varargin_1[7]);
      d3  = pd * (varargin_1[6] - varargin_1[2]);
      b_d = pd * (varargin_1[1] - varargin_1[3]);
      break;

    case 2:
      pd  = std::sqrt(pd);
      d2  = 0.5 * pd;
      pd  = 0.5 / pd;
      d1  = pd * (varargin_1[5] - varargin_1[7]);
      d3  = pd * (varargin_1[1] + varargin_1[3]);
      b_d = pd * (varargin_1[6] + varargin_1[2]);
      break;

    case 3:
      pd  = std::sqrt(pd);
      d3  = 0.5 * pd;
      pd  = 0.5 / pd;
      d1  = pd * (varargin_1[6] - varargin_1[2]);
      d2  = pd * (varargin_1[1] + varargin_1[3]);
      b_d = pd * (varargin_1[5] + varargin_1[7]);
      break;

    default:
      pd  = std::sqrt(pd);
      b_d = 0.5 * pd;
      pd  = 0.5 / pd;
      d1  = pd * (varargin_1[1] - varargin_1[3]);
      d2  = pd * (varargin_1[6] + varargin_1[2]);
      d3  = pd * (varargin_1[5] + varargin_1[7]);
      break;
  }

  if (d1 < 0.0) {
    d1  = -d1;
    d2  = -d2;
    d3  = -d3;
    b_d = -b_d;
  }

  this->a = d1;
  this->b = d2;
  this->c = d3;
  this->d = b_d;
}

//
// Arguments    : void
// Return Type  : fusion_internal_frames_NED *
//
fusion_internal_frames_NED *fusion_internal_frames_NED::init() { return this; }

//
// Arguments    : void
// Return Type  : c_fusion_internal_coder_imufilt *
//
c_fusion_internal_coder_imufilt *c_fusion_internal_coder_imufilt::init() {
  c_fusion_internal_coder_imufilt *obj;
  obj                                = this;
  obj->AccelerometerNoise            = 0.0001924722;
  obj->GyroscopeNoise                = 9.1385E-5;
  obj->GyroscopeDriftNoise           = 3.0462E-13;
  obj->LinearAccelerationNoise       = 0.0096236100000000012;
  obj->LinearAccelerationDecayFactor = 0.5;
  obj->isInitialized                 = 0;
  return obj;
}

//
// Arguments    : const c_matlabshared_rotations_intern y
// Return Type  : c_matlabshared_rotations_intern
//
c_matlabshared_rotations_intern c_matlabshared_rotations_intern::mtimes(
    const c_matlabshared_rotations_intern y) const {
  return this->times(y);
}

//
// Arguments    : void
// Return Type  : void
//
void c_matlabshared_rotations_intern::normalize() {
  double n;
  n = std::sqrt(((this->a * this->a + this->b * this->b) + this->c * this->c) +
                this->d * this->d);
  this->a /= n;
  this->b /= n;
  this->c /= n;
  this->d /= n;
}

//
// Arguments    : void
// Return Type  : double
//
double c_matlabshared_rotations_intern::parts() const { return this->a; }

//
// Arguments    : double r[9]
// Return Type  : void
//
void c_matlabshared_rotations_intern::rotmat(double r[9]) const {
  double ab2;
  double ac2;
  double ad2;
  double bc2;
  double bd2;
  double cd2;
  double aasq;
  c_matlabshared_rotations_intern localCopyOfThis;
  localCopyOfThis = *this;
  localCopyOfThis.normalize();
  ab2  = localCopyOfThis.a * localCopyOfThis.b * 2.0;
  ac2  = localCopyOfThis.a * localCopyOfThis.c * 2.0;
  ad2  = localCopyOfThis.a * localCopyOfThis.d * 2.0;
  bc2  = localCopyOfThis.b * localCopyOfThis.c * 2.0;
  bd2  = localCopyOfThis.b * localCopyOfThis.d * 2.0;
  cd2  = localCopyOfThis.c * localCopyOfThis.d * 2.0;
  aasq = localCopyOfThis.a * localCopyOfThis.a * 2.0 - 1.0;
  r[0] = aasq + localCopyOfThis.b * localCopyOfThis.b * 2.0;
  r[1] = bc2 + ad2;
  r[2] = bd2 - ac2;
  r[3] = bc2 - ad2;
  r[4] = aasq + localCopyOfThis.c * localCopyOfThis.c * 2.0;
  r[5] = cd2 + ab2;
  r[6] = bd2 + ac2;
  r[7] = cd2 - ab2;
  r[8] = aasq + localCopyOfThis.d * localCopyOfThis.d * 2.0;
}

//
// Arguments    : const double varargin_1[3]
//                fusion_internal_frames_NED *iobj_0
// Return Type  : void
//
void c_fusion_internal_coder_imufilt::setup(
    const double varargin_1[3], fusion_internal_frames_NED *iobj_0) {
  fusion_internal_frames_NED *ref;
  this->isInitialized       = 1;
  this->pInputPrototype[0]  = varargin_1[0];
  this->pInputPrototype[1]  = varargin_1[1];
  this->pInputPrototype[2]  = varargin_1[2];
  this->pSensorPeriod       = 0.01;
  this->pKalmanPeriod       = this->pSensorPeriod;
  ref                       = ReferenceFrame::getMathObject((iobj_0));
  this->pRefSys             = ref;
  this->TunablePropsChanged = false;
}

//
// Arguments    : const double varargin_1[3]
//                fusion_internal_frames_NED *iobj_0
// Return Type  : void
//
void c_fusion_internal_coder_imufilt::setupAndReset(
    const double varargin_1[3], fusion_internal_frames_NED *iobj_0) {
  double accelMeasNoiseVar;
  int i;
  signed char b_I[9];
  static const double dv[81] = {
      6.0923483957341713E-6, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
      6.0923483957341713E-6, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
      6.0923483957341713E-6, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
      7.6154354946677142E-5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
      7.6154354946677142E-5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
      7.6154354946677142E-5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
      0.0096236100000000012, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
      0.0096236100000000012, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
      0.0096236100000000012};

  this->setup(varargin_1, iobj_0);
  this->pOrientPost.b_init(1.0, 0.0, 0.0, 0.0);
  this->pGyroOffset[0] = 0.0;
  this->pGyroOffset[1] = 0.0;
  this->pGyroOffset[2] = 0.0;
  accelMeasNoiseVar    = this->pKalmanPeriod;
  accelMeasNoiseVar =
      (this->AccelerometerNoise + this->LinearAccelerationNoise) +
      accelMeasNoiseVar * accelMeasNoiseVar *
          (this->GyroscopeDriftNoise + this->GyroscopeNoise);
  for (i = 0; i < 9; i++) {
    b_I[i] = 0;
  }

  b_I[0] = 1;
  b_I[4] = 1;
  b_I[8] = 1;
  for (i = 0; i < 9; i++) {
    this->pQv[i] = accelMeasNoiseVar * static_cast<double>(b_I[i]);
  }

  for (i = 0; i < 81; i++) {
    this->pQw[i] = dv[i];
  }

  this->pLinAccelPost[0] = 0.0;
  this->pLinAccelPost[1] = 0.0;
  this->pLinAccelPost[2] = 0.0;
  this->pFirstTime       = true;
}

//
// Arguments    : const double varargin_1[3]    【输入】加速度计数据
//                const double varargin_2[3]    【输入】陀螺仪数据
//                fusion_internal_frames_NED *iobj_0
//                double varargout_1[9]         【输出】角度 3x3旋转矩阵
//                double varargout_2[3]         【输出】去除了陀螺仪bias的角速度
// Return Type  : void
//
void c_fusion_internal_coder_imufilt::step(const double varargin_1[3],
                                           const double varargin_2[3],
                                           fusion_internal_frames_NED *iobj_0,
                                           double varargout_1[9],
                                           double varargout_2[3]) {
  double gravityAccelMeas_idx_0;
  double gravityAccelMeas_idx_1;
  double gravityAccelMeas_idx_2;
  double Rpost[9];
  c_matlabshared_rotations_intern qorient;
  double deltaAng[3];
  double Rprior[9];
  double a21;
  int h3_tmp;
  double h3[9];
  int b_h3_tmp;
  double H[27];
  static const signed char iv[9] = {1, 0, 0, 0, 1, 0, 0, 0, 1};

  int H_tmp;
  double Qw[81];
  int c_h3_tmp;
  int r1;
  double d;
  int r2;
  int rtemp;
  int r3;
  double maxval;
  double tmp_tmp[27];
  double y_tmp[27];
  double y[27];
  int d_h3_tmp;
  int e_h3_tmp;
  double Ppost[81];
  c_matlabshared_rotations_intern b;
  if (this->isInitialized != 1) {
    this->setupAndReset(varargin_1, iobj_0);  // 第一次迭代，初始化滤波器参数
  }

  this->checkTunableProps();  // 检查是否有参数发生变化，因为有些参数可以在运行过程中人工调整
  gravityAccelMeas_idx_0 = this->pGyroOffset[0];
  gravityAccelMeas_idx_1 = this->pGyroOffset[1];
  gravityAccelMeas_idx_2 = this->pGyroOffset[2];
  varargout_2[0]         = varargin_2[0] - gravityAccelMeas_idx_0;
  varargout_2[1]         = varargin_2[1] - gravityAccelMeas_idx_1;
  varargout_2[2]         = varargin_2[2] - gravityAccelMeas_idx_2;
  if (this->pFirstTime) {
    fusion_internal_frames_NED::ecompass((varargin_1), (Rpost));
    this->pFirstTime = false;
    this->pOrientPost.init(Rpost);
  }

  gravityAccelMeas_idx_0 = this->pGyroOffset[0];
  gravityAccelMeas_idx_1 = this->pGyroOffset[1];
  gravityAccelMeas_idx_2 = this->pGyroOffset[2];
  qorient                = this->pOrientPost;
  deltaAng[0]            = varargin_2[0] - gravityAccelMeas_idx_0;
  deltaAng[1]            = varargin_2[1] - gravityAccelMeas_idx_1;
  deltaAng[2]            = varargin_2[2] - gravityAccelMeas_idx_2;
  deltaAng[0] *= this->pSensorPeriod;
  deltaAng[1] *= this->pSensorPeriod;
  deltaAng[2] *= this->pSensorPeriod;
  b.b_init(deltaAng);
  qorient = qorient.mtimes(b);
  if (qorient.parts() < 0.0) {
    qorient.uminus();
  }

  this->pOrientPrior = qorient;
  this->pOrientPrior.rotmat(Rprior);
  a21                     = this->LinearAccelerationDecayFactor;
  gravityAccelMeas_idx_0  = this->pLinAccelPost[0];
  gravityAccelMeas_idx_1  = this->pLinAccelPost[1];
  gravityAccelMeas_idx_2  = this->pLinAccelPost[2];
  this->pLinAccelPrior[0] = a21 * gravityAccelMeas_idx_0;
  this->pLinAccelPrior[1] = a21 * gravityAccelMeas_idx_1;
  this->pLinAccelPrior[2] = a21 * gravityAccelMeas_idx_2;
  gravityAccelMeas_idx_0  = varargin_1[0] + this->pLinAccelPrior[0];
  gravityAccelMeas_idx_1  = varargin_1[1] + this->pLinAccelPrior[1];
  gravityAccelMeas_idx_2  = varargin_1[2] + this->pLinAccelPrior[2];
  std::memset(&Rpost[0], 0, 9U * sizeof(double));
  Rpost[1] = Rprior[8];
  Rpost[2] = -Rprior[5];
  Rpost[5] = Rprior[2];
  for (h3_tmp = 0; h3_tmp < 3; h3_tmp++) {
    h3[3 * h3_tmp] = Rpost[3 * h3_tmp] - Rpost[h3_tmp];
    b_h3_tmp       = 3 * h3_tmp + 1;
    h3[b_h3_tmp]   = Rpost[b_h3_tmp] - Rpost[h3_tmp + 3];
    b_h3_tmp       = 3 * h3_tmp + 2;
    h3[b_h3_tmp]   = Rpost[b_h3_tmp];
  }

  std::memcpy(&Rpost[0], &h3[0], 9U * sizeof(double));
  for (h3_tmp = 0; h3_tmp < 9; h3_tmp++) {
    h3[h3_tmp] = -Rpost[h3_tmp] * this->pKalmanPeriod;
  }

  for (h3_tmp = 0; h3_tmp < 3; h3_tmp++) {
    H[9 * h3_tmp]     = Rpost[3 * h3_tmp];
    H[9 * h3_tmp + 3] = h3[3 * h3_tmp];
    H[9 * h3_tmp + 6] = iv[3 * h3_tmp];
    H_tmp             = 3 * h3_tmp + 1;
    H[9 * h3_tmp + 1] = Rpost[H_tmp];
    H[9 * h3_tmp + 4] = h3[H_tmp];
    H[9 * h3_tmp + 7] = iv[H_tmp];
    H_tmp             = 3 * h3_tmp + 2;
    H[9 * h3_tmp + 2] = Rpost[H_tmp];
    H[9 * h3_tmp + 5] = h3[H_tmp];
    H[9 * h3_tmp + 8] = iv[H_tmp];
  }

  for (h3_tmp = 0; h3_tmp < 9; h3_tmp++) {
    Rpost[h3_tmp] = this->pQv[h3_tmp];
  }

  for (h3_tmp = 0; h3_tmp < 81; h3_tmp++) {
    Qw[h3_tmp] = this->pQw[h3_tmp];
  }

  for (h3_tmp = 0; h3_tmp < 9; h3_tmp++) {
    for (c_h3_tmp = 0; c_h3_tmp < 3; c_h3_tmp++) {
      d = 0.0;
      for (rtemp = 0; rtemp < 9; rtemp++) {
        d += Qw[h3_tmp + 9 * rtemp] * H[rtemp + 9 * c_h3_tmp];
      }

      rtemp                        = h3_tmp + 9 * c_h3_tmp;
      tmp_tmp[rtemp]               = d;
      y_tmp[c_h3_tmp + 3 * h3_tmp] = H[rtemp];
    }
  }

  for (h3_tmp = 0; h3_tmp < 3; h3_tmp++) {
    for (c_h3_tmp = 0; c_h3_tmp < 3; c_h3_tmp++) {
      d = 0.0;
      for (rtemp = 0; rtemp < 9; rtemp++) {
        d += y_tmp[c_h3_tmp + 3 * rtemp] * tmp_tmp[rtemp + 9 * h3_tmp];
      }

      h3[h3_tmp + 3 * c_h3_tmp] = d + Rpost[c_h3_tmp + 3 * h3_tmp];
    }

    for (c_h3_tmp = 0; c_h3_tmp < 9; c_h3_tmp++) {
      d = 0.0;
      for (rtemp = 0; rtemp < 9; rtemp++) {
        d += y_tmp[h3_tmp + 3 * rtemp] * Qw[rtemp + 9 * c_h3_tmp];
      }

      y[h3_tmp + 3 * c_h3_tmp] = d;
    }
  }

  r1     = 0;
  r2     = 1;
  r3     = 2;
  maxval = std::abs(h3[0]);
  a21    = std::abs(h3[3]);
  if (a21 > maxval) {
    maxval = a21;
    r1     = 1;
    r2     = 0;
  }

  if (std::abs(h3[6]) > maxval) {
    r1 = 2;
    r2 = 1;
    r3 = 0;
  }

  h3[3 * r2] /= h3[3 * r1];
  h3[3 * r3] /= h3[3 * r1];
  b_h3_tmp = 3 * r2 + 1;
  d_h3_tmp = 3 * r1 + 1;
  h3[b_h3_tmp] -= h3[3 * r2] * h3[d_h3_tmp];
  rtemp = 3 * r3 + 1;
  h3[rtemp] -= h3[3 * r3] * h3[d_h3_tmp];
  c_h3_tmp = 3 * r2 + 2;
  e_h3_tmp = 3 * r1 + 2;
  h3[c_h3_tmp] -= h3[3 * r2] * h3[e_h3_tmp];
  c_h3_tmp = 3 * r3 + 2;
  h3[c_h3_tmp] -= h3[3 * r3] * h3[e_h3_tmp];
  if (std::abs(h3[rtemp]) > std::abs(h3[b_h3_tmp])) {
    rtemp = r2;
    r2    = r3;
    r3    = rtemp;
  }

  b_h3_tmp = 3 * r3 + 1;
  rtemp    = 3 * r2 + 1;
  h3[b_h3_tmp] /= h3[rtemp];
  c_h3_tmp = 3 * r3 + 2;
  h3_tmp   = 3 * r2 + 2;
  h3[c_h3_tmp] -= h3[b_h3_tmp] * h3[h3_tmp];
  for (int k = 0; k < 9; k++) {
    int b_H_tmp;
    int c_H_tmp;
    H_tmp      = r1 + 3 * k;
    H[H_tmp]   = y[3 * k] / h3[3 * r1];
    b_H_tmp    = r2 + 3 * k;
    H[b_H_tmp] = y[3 * k + 1] - H[H_tmp] * h3[d_h3_tmp];
    c_H_tmp    = r3 + 3 * k;
    H[c_H_tmp] = y[3 * k + 2] - H[H_tmp] * h3[e_h3_tmp];
    H[b_H_tmp] /= h3[rtemp];
    H[c_H_tmp] -= H[b_H_tmp] * h3[h3_tmp];
    H[c_H_tmp] /= h3[c_h3_tmp];
    H[b_H_tmp] -= H[c_H_tmp] * h3[b_h3_tmp];
    H[H_tmp] -= H[c_H_tmp] * h3[3 * r3];
    H[H_tmp] -= H[b_H_tmp] * h3[3 * r2];
  }

  gravityAccelMeas_idx_0 -= Rprior[2];
  gravityAccelMeas_idx_1 -= Rprior[5];
  gravityAccelMeas_idx_2 -= Rprior[8];
  for (h3_tmp = 0; h3_tmp < 9; h3_tmp++) {
    Rpost[h3_tmp] = (gravityAccelMeas_idx_0 * H[3 * h3_tmp] +
                     gravityAccelMeas_idx_1 * H[3 * h3_tmp + 1]) +
                    gravityAccelMeas_idx_2 * H[3 * h3_tmp + 2];
  }

  qorient.b_init((*(double(*)[3]) & Rpost[0]));
  qorient.b_conj();
  this->pOrientPost = this->pOrientPrior.mtimes(qorient);
  if (this->pOrientPost.parts() < 0.0) {
    qorient = this->pOrientPost;
    qorient.uminus();
    this->pOrientPost = qorient;
  }

  qorient = this->pOrientPost;
  qorient.normalize();
  this->pOrientPost = qorient;
  this->pGyroOffset[0] -= Rpost[3];
  this->pGyroOffset[1] -= Rpost[4];
  this->pGyroOffset[2] -= Rpost[5];
  this->pLinAccelPost[0] = this->pLinAccelPrior[0] - Rpost[6];
  this->pLinAccelPost[1] = this->pLinAccelPrior[1] - Rpost[7];
  this->pLinAccelPost[2] = this->pLinAccelPrior[2] - Rpost[8];
  for (h3_tmp = 0; h3_tmp < 9; h3_tmp++) {
    d      = tmp_tmp[h3_tmp + 9];
    maxval = tmp_tmp[h3_tmp + 18];
    for (c_h3_tmp = 0; c_h3_tmp < 9; c_h3_tmp++) {
      rtemp = h3_tmp + 9 * c_h3_tmp;
      Ppost[rtemp] =
          Qw[rtemp] -
          ((tmp_tmp[h3_tmp] * H[3 * c_h3_tmp] + d * H[3 * c_h3_tmp + 1]) +
           maxval * H[3 * c_h3_tmp + 2]);
    }
  }

  std::memset(&Qw[0], 0, 81U * sizeof(double));
  a21 = this->pKalmanPeriod;
  a21 *= a21;
  maxval = this->GyroscopeDriftNoise + this->GyroscopeNoise;
  Qw[0]  = Ppost[0] + a21 * (Ppost[30] + maxval);
  Qw[10] = Ppost[10] + a21 * (Ppost[40] + maxval);
  Qw[20] = Ppost[20] + a21 * (Ppost[50] + maxval);
  Qw[30] = Ppost[30] + this->GyroscopeDriftNoise;
  Qw[40] = Ppost[40] + this->GyroscopeDriftNoise;
  Qw[50] = Ppost[50] + this->GyroscopeDriftNoise;
  a21    = -this->pKalmanPeriod;
  d      = a21 * Qw[30];
  Qw[27] = d;
  Qw[3]  = d;
  d      = a21 * Qw[40];
  Qw[37] = d;
  Qw[13] = d;
  d      = a21 * Qw[50];
  Qw[47] = d;
  Qw[23] = d;
  a21    = this->LinearAccelerationDecayFactor;
  a21 *= a21;
  Qw[60] = a21 * Ppost[60] + this->LinearAccelerationNoise;
  Qw[70] = a21 * Ppost[70] + this->LinearAccelerationNoise;
  Qw[80] = a21 * Ppost[80] + this->LinearAccelerationNoise;
  for (h3_tmp = 0; h3_tmp < 81; h3_tmp++) {
    this->pQw[h3_tmp] = Qw[h3_tmp];
  }

  this->pOrientPost.rotmat(varargout_1);
}

//
// Arguments    : const c_matlabshared_rotations_intern y
// Return Type  : c_matlabshared_rotations_intern
//
c_matlabshared_rotations_intern c_matlabshared_rotations_intern::times(
    const c_matlabshared_rotations_intern y) const {
  c_matlabshared_rotations_intern o;
  o.init((((this->a * y.a - this->b * y.b) - this->c * y.c) - this->d * y.d),
         (((this->a * y.b + this->b * y.a) + this->c * y.d) - this->d * y.c),
         (((this->a * y.c - this->b * y.d) + this->c * y.a) + this->d * y.b),
         (((this->a * y.d + this->b * y.c) - this->c * y.b) + this->d * y.a));
  return o;
}

//
// Arguments    : void
// Return Type  : void
//
void c_matlabshared_rotations_intern::uminus() {
  this->a = -this->a;
  this->b = -this->b;
  this->c = -this->c;
  this->d = -this->d;
}

//
// Arguments    : double u0
//                double u1
// Return Type  : double
//
static double rt_powd_snf(double u0, double u1) {
  double y;
  if (std::isnan(u0) || std::isnan(u1)) {
    y = rtNaN;
  } else {
    double d;
    double d1;
    d  = std::abs(u0);
    d1 = std::abs(u1);
    if (std::isinf(u1)) {
      if (d == 1.0) {
        y = 1.0;
      } else if (d > 1.0) {
        if (u1 > 0.0) {
          y = rtInf;
        } else {
          y = 0.0;
        }
      } else if (u1 > 0.0) {
        y = 0.0;
      } else {
        y = rtInf;
      }
    } else if (d1 == 0.0) {
      y = 1.0;
    } else if (d1 == 1.0) {
      if (u1 > 0.0) {
        y = u0;
      } else {
        y = 1.0 / u0;
      }
    } else if (u1 == 2.0) {
      y = u0 * u0;
    } else if ((u1 == 0.5) && (u0 >= 0.0)) {
      y = std::sqrt(u0);
    } else if ((u0 < 0.0) && (u1 > std::floor(u1))) {
      y = rtNaN;
    } else {
      y = std::pow(u0, u1);
    }
  }

  return y;
}

namespace filter {
//
// Arguments    : void
// Return Type  : void
//
ImuEskfFilter::ImuEskfFilter() {
  rt_InitInfAndNaN();

  ifilt_.init();
}

void ImuEskfFilter::Step(const Vector3d &input_acc,
                         const Vector3d &input_gyro,
                         RotationMatrix3d &output_rot,
                         Vector3d &output_angular_velocity) {
  double acc[3];
  acc[0] = input_acc.x;
  acc[1] = input_acc.y;
  acc[2] = input_acc.z;

  double gyro[3];
  gyro[0] = input_gyro.x;
  gyro[1] = input_gyro.y;
  gyro[2] = input_gyro.z;

  double varargout_2[3];

  ifilt_.step((*(double(*)[3]) & acc[0]),
              (*(double(*)[3]) & gyro[0]),
              (&lobj_1),
              output_rot.data.data(),
              varargout_2);

  output_angular_velocity.x = varargout_2[0];
  output_angular_velocity.y = varargout_2[1];
  output_angular_velocity.z = varargout_2[2];
}

}  // namespace filter