/*
 * File: abs.c
 *
 * MATLAB Coder version            : 4.2
 * C/C++ source code generated on  : 15-Jan-2023 14:17:59
 */

/* Include Files */
#include "rt_nonfinite.h"
#include "magCalibration4.h"
#include "abs.h"
#include "inv.h"
#include "magCalibration4_rtwutil.h"

/* Function Definitions */

/*
 * Arguments    : const creal_T x[9]
 *                double y[9]
 * Return Type  : void
 */
void b_abs(const creal_T x[9], double y[9])
{
  int k;
  for (k = 0; k < 9; k++) {
    y[k] = rt_hypotd_snf(x[k].re, x[k].im);
  }
}

/*
 * Arguments    : const creal_T x[120]
 *                double y[120]
 * Return Type  : void
 */
void c_abs(const creal_T x[120], double y[120])
{
  int k;
  for (k = 0; k < 120; k++) {
    y[k] = rt_hypotd_snf(x[k].re, x[k].im);
  }
}

/*
 * File trailer for abs.c
 *
 * [EOF]
 */
