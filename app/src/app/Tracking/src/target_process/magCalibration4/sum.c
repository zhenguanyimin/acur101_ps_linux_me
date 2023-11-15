/*
 * File: sum.c
 *
 * MATLAB Coder version            : 4.2
 * C/C++ source code generated on  : 15-Jan-2023 14:17:59
 */

/* Include Files */
#include <string.h>
#include "rt_nonfinite.h"
#include "magCalibration4.h"
#include "sum.h"

/* Function Definitions */

/*
 * Arguments    : const double x[120]
 * Return Type  : double
 */
double b_sum(const double x[120])
{
  double y;
  int k;
  y = x[0];
  for (k = 0; k < 119; k++) {
    y += x[k + 1];
  }

  return y;
}

/*
 * Arguments    : const creal_T x[360]
 *                creal_T y[120]
 * Return Type  : void
 */
void sum(const creal_T x[360], creal_T y[120])
{
  int k;
  int xoffset;
  int j;
  memcpy(&y[0], &x[0], 120U * sizeof(creal_T));
  for (k = 0; k < 2; k++) {
    xoffset = (k + 1) * 120;
    for (j = 0; j < 120; j++) {
      y[j].re += x[xoffset + j].re;
      y[j].im += x[xoffset + j].im;
    }
  }
}

/*
 * File trailer for sum.c
 *
 * [EOF]
 */
