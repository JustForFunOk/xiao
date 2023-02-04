//
//  File: rt_nonfinite.h
//
//  MATLAB Coder version            : 5.0
//  C/C++ source code generated on  : 15-Jan-2023 18:29:27

#pragma once

#include "rtwtypes.h"

// typedef double real_T;
// typedef float real32_T;
// typedef bool boolean_T;

#ifdef __cplusplus

extern "C" {

#endif

  extern real_T rtInf;
  extern real_T rtMinusInf;
  extern real_T rtNaN;
  extern real32_T rtInfF;
  extern real32_T rtMinusInfF;
  extern real32_T rtNaNF;
  /// 初始化极值和无效值
  extern void rt_InitInfAndNaN();

  /// 判断double是否是极大值或极小值, 如1.0/0.0, -1.0/0.0
  extern boolean_T rtIsInf(real_T value);

  /// 判断float是否是极大值或极小值, 如1.0/0.0, -1.0/0.0
  extern boolean_T rtIsInfF(real32_T value);

  /// 判断double是否是无效值, 如负数开方
  extern boolean_T rtIsNaN(real_T value);

  /// 判断float是否是无效值, 如负数开方
  extern boolean_T rtIsNaNF(real32_T value);

#ifdef __cplusplus

}
#endif

//
//  File trailer for rt_nonfinite.h
//
//  [EOF]

