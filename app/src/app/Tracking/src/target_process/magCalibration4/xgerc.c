/*
 * File: xgerc.c
 *
 * MATLAB Coder version            : 4.2
 * C/C++ source code generated on  : 15-Jan-2023 14:17:59
 */

/* Include Files */
#include "rt_nonfinite.h"
#include "magCalibration4.h"
#include "xgerc.h"

/* Function Definitions */

/*
 * Arguments    : int m
 *                int n
 *                double alpha1
 *                int ix0
 *                const double y[3]
 *                double A[9]
 *                int ia0
 * Return Type  : void
 */
void xgerc(int m, int n, double alpha1, int ix0, const double y[3], double A[9],
           int ia0)
{
  int jA;
  int jy;
  int j;
  double temp;
  int ix;
  int i11;
  int i12;
  int ijA;
  if (!(alpha1 == 0.0)) {
    jA = ia0 - 1;
    jy = 0;
    for (j = 0; j < n; j++) {
      if (y[jy] != 0.0) {
        temp = y[jy] * alpha1;
        ix = ix0;
        i11 = jA + 1;
        i12 = m + jA;
        for (ijA = i11; ijA <= i12; ijA++) {
          A[ijA - 1] += A[ix - 1] * temp;
          ix++;
        }
      }

      jy++;
      jA += 3;
    }
  }
}

/*
 * File trailer for xgerc.c
 *
 * [EOF]
 */
