/*
 * File: xgemv.c
 *
 * MATLAB Coder version            : 4.2
 * C/C++ source code generated on  : 15-Jan-2023 14:17:59
 */

/* Include Files */
#include <string.h>
#include "rt_nonfinite.h"
#include "magCalibration4.h"
#include "xgemv.h"

/* Function Definitions */

/*
 * Arguments    : int m
 *                int n
 *                const double A[9]
 *                int ia0
 *                const double x[9]
 *                int ix0
 *                double y[3]
 * Return Type  : void
 */
void xgemv(int m, int n, const double A[9], int ia0, const double x[9], int ix0,
           double y[3])
{
  int iy;
  int i9;
  int iac;
  int ix;
  double c;
  int i10;
  int ia;
  if ((m == 0) || (n == 0)) {
  } else {
    if (0 <= n - 1) {
      memset(&y[0], 0, (unsigned int)(n * (int)sizeof(double)));
    }

    iy = 0;
    i9 = ia0 + 3 * (n - 1);
    for (iac = ia0; iac <= i9; iac += 3) {
      ix = ix0;
      c = 0.0;
      i10 = (iac + m) - 1;
      for (ia = iac; ia <= i10; ia++) {
        c += A[ia - 1] * x[ix - 1];
        ix++;
      }

      y[iy] += c;
      iy++;
    }
  }
}

/*
 * File trailer for xgemv.c
 *
 * [EOF]
 */
