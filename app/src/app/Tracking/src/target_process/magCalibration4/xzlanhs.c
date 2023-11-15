/*
 * File: xzlanhs.c
 *
 * MATLAB Coder version            : 4.2
 * C/C++ source code generated on  : 15-Jan-2023 14:17:59
 */

/* Include Files */
#include <math.h>
#include "rt_nonfinite.h"
#include "magCalibration4.h"
#include "xzlanhs.h"

/* Function Definitions */

/*
 * Arguments    : const creal_T A[9]
 *                int ilo
 *                int ihi
 * Return Type  : double
 */
double xzlanhs(const creal_T A[9], int ilo, int ihi)
{
  double f;
  double scale;
  double sumsq;
  boolean_T firstNonZero;
  int j;
  int i7;
  int i;
  double re;
  double im;
  double temp1;
  f = 0.0;
  if (ilo <= ihi) {
    scale = 0.0;
    sumsq = 0.0;
    firstNonZero = true;
    for (j = ilo; j <= ihi; j++) {
      i7 = j + 1;
      if (ihi < j + 1) {
        i7 = ihi;
      }

      for (i = ilo; i <= i7; i++) {
        re = A[(i + 3 * (j - 1)) - 1].re;
        im = A[(i + 3 * (j - 1)) - 1].im;
        if (re != 0.0) {
          temp1 = fabs(re);
          if (firstNonZero) {
            sumsq = 1.0;
            scale = temp1;
            firstNonZero = false;
          } else if (scale < temp1) {
            re = scale / temp1;
            sumsq = 1.0 + sumsq * re * re;
            scale = temp1;
          } else {
            re = temp1 / scale;
            sumsq += re * re;
          }
        }

        if (im != 0.0) {
          temp1 = fabs(im);
          if (firstNonZero) {
            sumsq = 1.0;
            scale = temp1;
            firstNonZero = false;
          } else if (scale < temp1) {
            re = scale / temp1;
            sumsq = 1.0 + sumsq * re * re;
            scale = temp1;
          } else {
            re = temp1 / scale;
            sumsq += re * re;
          }
        }
      }
    }

    f = scale * sqrt(sumsq);
  }

  return f;
}

/*
 * File trailer for xzlanhs.c
 *
 * [EOF]
 */
