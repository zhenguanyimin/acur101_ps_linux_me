/*
 * File: power.c
 *
 * MATLAB Coder version            : 4.2
 * C/C++ source code generated on  : 15-Jan-2023 14:17:59
 */

/* Include Files */
#include "rt_nonfinite.h"
#include "magCalibration4.h"
#include "power.h"

/* Function Definitions */

/*
 * Arguments    : const creal_T a[360]
 *                creal_T y[360]
 * Return Type  : void
 */
void power(const creal_T a[360], creal_T y[360])
{
  int k;
  double d1;
  for (k = 0; k < 360; k++) {
    y[k].re = a[k].re * a[k].re - a[k].im * a[k].im;
    d1 = a[k].re * a[k].im;
    y[k].im = d1 + d1;
  }
}

/*
 * File trailer for power.c
 *
 * [EOF]
 */
