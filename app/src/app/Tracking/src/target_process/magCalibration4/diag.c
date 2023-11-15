/*
 * File: diag.c
 *
 * MATLAB Coder version            : 4.2
 * C/C++ source code generated on  : 15-Jan-2023 14:17:59
 */

/* Include Files */
#include "rt_nonfinite.h"
#include "magCalibration4.h"
#include "diag.h"

/* Function Definitions */

/*
 * Arguments    : const creal_T v[9]
 *                creal_T d[3]
 * Return Type  : void
 */
void b_diag(const creal_T v[9], creal_T d[3])
{
  d[0] = v[0];
  d[1] = v[4];
  d[2] = v[8];
}

/*
 * Arguments    : const double v[9]
 *                double d[3]
 * Return Type  : void
 */
void diag(const double v[9], double d[3])
{
  d[0] = v[0];
  d[1] = v[4];
  d[2] = v[8];
}

/*
 * File trailer for diag.c
 *
 * [EOF]
 */
