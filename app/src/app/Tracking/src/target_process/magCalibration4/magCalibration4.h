/*
 * File: magCalibration4.h
 *
 * MATLAB Coder version            : 4.2
 * C/C++ source code generated on  : 15-Jan-2023 14:17:59
 */

#ifndef MAGCALIBRATION4_H
#define MAGCALIBRATION4_H

/* Include Files */
#include <stddef.h>
#include <stdlib.h>
#include "rtwtypes.h"
#include "magCalibration4_types.h"

/* Function Declarations */
extern void magCalibration4(const double mag_x[120], const double mag_y[120],
  const double mag_z[120], double GT_mag, creal_T C[9], double center[3], double
  *chi2);

#endif

/*
 * File trailer for magCalibration4.h
 *
 * [EOF]
 */
