/*
 * File: ellipsoid_fit2.h
 *
 * MATLAB Coder version            : 4.2
 * C/C++ source code generated on  : 15-Jan-2023 14:17:59
 */

#ifndef ELLIPSOID_FIT2_H
#define ELLIPSOID_FIT2_H

/* Include Files */
#include <stddef.h>
#include <stdlib.h>
#include "rtwtypes.h"
#include "magCalibration4_types.h"

/* Function Declarations */
extern void ellipsoid_fit2(const double mag_x[120], const double mag_y[120],
  const double mag_z[120], double center[3], creal_T radii[3], creal_T evecs[9],
  double v[10], double *chi2);

#endif

/*
 * File trailer for ellipsoid_fit2.h
 *
 * [EOF]
 */
