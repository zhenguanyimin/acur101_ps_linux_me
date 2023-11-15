/*
 * File: magCalibration4.c
 *
 * MATLAB Coder version            : 4.2
 * C/C++ source code generated on  : 15-Jan-2023 14:17:59
 */

/* Include Files */
#include <string.h>
#include "rt_nonfinite.h"
#include "magCalibration4.h"
#include "inv.h"
#include "ellipsoid_fit2.h"

/* Function Definitions */

/*
 * Arguments    : const double mag_x[120]
 *                const double mag_y[120]
 *                const double mag_z[120]
 *                double GT_mag
 *                creal_T C[9]
 *                double center[3]
 *                double *chi2
 * Return Type  : void
 */
void magCalibration4(const double mag_x[120], const double mag_y[120], const
                     double mag_z[120], double GT_mag, creal_T C[9], double
                     center[3], double *chi2)
{
  creal_T radii[3];
  creal_T evecs[9];
  double v[10];
  creal_T dcv0[9];
  creal_T b_radii[9];
  int i0;
  int i1;
  int i2;
  creal_T b_evecs[9];
  ellipsoid_fit2(mag_x, mag_y, mag_z, center, radii, evecs, v, chi2);
  inv(evecs, dcv0);
  if (radii[0].im == 0.0) {
    b_radii[0].re = radii[0].re / GT_mag;
    b_radii[0].im = 0.0;
  } else if (radii[0].re == 0.0) {
    b_radii[0].re = 0.0;
    b_radii[0].im = radii[0].im / GT_mag;
  } else {
    b_radii[0].re = radii[0].re / GT_mag;
    b_radii[0].im = radii[0].im / GT_mag;
  }

  b_radii[3].re = 0.0;
  b_radii[3].im = 0.0;
  b_radii[6].re = 0.0;
  b_radii[6].im = 0.0;
  b_radii[1].re = 0.0;
  b_radii[1].im = 0.0;
  if (radii[1].im == 0.0) {
    b_radii[4].re = radii[1].re / GT_mag;
    b_radii[4].im = 0.0;
  } else if (radii[1].re == 0.0) {
    b_radii[4].re = 0.0;
    b_radii[4].im = radii[1].im / GT_mag;
  } else {
    b_radii[4].re = radii[1].re / GT_mag;
    b_radii[4].im = radii[1].im / GT_mag;
  }

  b_radii[7].re = 0.0;
  b_radii[7].im = 0.0;
  b_radii[2].re = 0.0;
  b_radii[2].im = 0.0;
  b_radii[5].re = 0.0;
  b_radii[5].im = 0.0;
  if (radii[2].im == 0.0) {
    b_radii[8].re = radii[2].re / GT_mag;
    b_radii[8].im = 0.0;
  } else if (radii[2].re == 0.0) {
    b_radii[8].re = 0.0;
    b_radii[8].im = radii[2].im / GT_mag;
  } else {
    b_radii[8].re = radii[2].re / GT_mag;
    b_radii[8].im = radii[2].im / GT_mag;
  }

  for (i0 = 0; i0 < 3; i0++) {
    for (i1 = 0; i1 < 3; i1++) {
      i2 = i0 + 3 * i1;
      b_evecs[i2].re = ((evecs[i0].re * b_radii[3 * i1].re - evecs[i0].im *
                         b_radii[3 * i1].im) + (evecs[i0 + 3].re * b_radii[1 + 3
        * i1].re - evecs[i0 + 3].im * b_radii[1 + 3 * i1].im)) + (evecs[i0 + 6].
        re * b_radii[2 + 3 * i1].re - evecs[i0 + 6].im * b_radii[2 + 3 * i1].im);
      b_evecs[i2].im = ((evecs[i0].re * b_radii[3 * i1].im + evecs[i0].im *
                         b_radii[3 * i1].re) + (evecs[i0 + 3].re * b_radii[1 + 3
        * i1].im + evecs[i0 + 3].im * b_radii[1 + 3 * i1].re)) + (evecs[i0 + 6].
        re * b_radii[2 + 3 * i1].im + evecs[i0 + 6].im * b_radii[2 + 3 * i1].re);
    }

    for (i1 = 0; i1 < 3; i1++) {
      i2 = i0 + 3 * i1;
      evecs[i2].re = ((b_evecs[i0].re * dcv0[3 * i1].re - b_evecs[i0].im * dcv0
                       [3 * i1].im) + (b_evecs[i0 + 3].re * dcv0[1 + 3 * i1].re
        - b_evecs[i0 + 3].im * dcv0[1 + 3 * i1].im)) + (b_evecs[i0 + 6].re *
        dcv0[2 + 3 * i1].re - b_evecs[i0 + 6].im * dcv0[2 + 3 * i1].im);
      evecs[i2].im = ((b_evecs[i0].re * dcv0[3 * i1].im + b_evecs[i0].im * dcv0
                       [3 * i1].re) + (b_evecs[i0 + 3].re * dcv0[1 + 3 * i1].im
        + b_evecs[i0 + 3].im * dcv0[1 + 3 * i1].re)) + (b_evecs[i0 + 6].re *
        dcv0[2 + 3 * i1].im + b_evecs[i0 + 6].im * dcv0[2 + 3 * i1].re);
    }
  }

  inv(evecs, dcv0);
  for (i0 = 0; i0 < 3; i0++) {
    evecs[3 * i0].re = dcv0[i0].re;
    evecs[3 * i0].im = -dcv0[i0].im;
    i1 = 1 + 3 * i0;
    evecs[i1].re = dcv0[i0 + 3].re;
    evecs[i1].im = -dcv0[i0 + 3].im;
    i1 = 2 + 3 * i0;
    evecs[i1].re = dcv0[i0 + 6].re;
    evecs[i1].im = -dcv0[i0 + 6].im;
  }

  memcpy(&C[0], &evecs[0], 9U * sizeof(creal_T));
}

/*
 * File trailer for magCalibration4.c
 *
 * [EOF]
 */
