/*
 * File: sqrt.c
 *
 * MATLAB Coder version            : 4.2
 * C/C++ source code generated on  : 15-Jan-2023 14:17:59
 */

/* Include Files */
#include <math.h>
#include "rt_nonfinite.h"
#include "magCalibration4.h"
#include "sqrt.h"
#include "inv.h"
#include "magCalibration4_rtwutil.h"

/* Function Definitions */

/*
 * Arguments    : double *x
 * Return Type  : void
 */
void b_sqrt(double *x)
{
  *x = sqrt(*x);
}

/*
 * Arguments    : creal_T *x
 * Return Type  : void
 */
void c_sqrt(creal_T *x)
{
  double xr;
  double xi;
  double yr;
  double absxr;
  xr = x->re;
  xi = x->im;
  if (xi == 0.0) {
    if (xr < 0.0) {
      yr = 0.0;
      xr = sqrt(-xr);
    } else {
      yr = sqrt(xr);
      xr = 0.0;
    }
  } else if (xr == 0.0) {
    if (xi < 0.0) {
      yr = sqrt(-xi / 2.0);
      xr = -yr;
    } else {
      yr = sqrt(xi / 2.0);
      xr = yr;
    }
  } else if (rtIsNaN(xr)) {
    yr = xr;
  } else if (rtIsNaN(xi)) {
    yr = xi;
    xr = xi;
  } else if (rtIsInf(xi)) {
    yr = fabs(xi);
    xr = xi;
  } else if (rtIsInf(xr)) {
    if (xr < 0.0) {
      yr = 0.0;
      xr = xi * -xr;
    } else {
      yr = xr;
      xr = 0.0;
    }
  } else {
    absxr = fabs(xr);
    yr = fabs(xi);
    if ((absxr > 4.4942328371557893E+307) || (yr > 4.4942328371557893E+307)) {
      absxr *= 0.5;
      yr = rt_hypotd_snf(absxr, yr * 0.5);
      if (yr > absxr) {
        yr = sqrt(yr) * sqrt(1.0 + absxr / yr);
      } else {
        yr = sqrt(yr) * 1.4142135623730951;
      }
    } else {
      yr = sqrt((rt_hypotd_snf(absxr, yr) + absxr) * 0.5);
    }

    if (xr > 0.0) {
      xr = 0.5 * (xi / yr);
    } else {
      if (xi < 0.0) {
        xr = -yr;
      } else {
        xr = yr;
      }

      yr = 0.5 * (xi / xr);
    }
  }

  x->re = yr;
  x->im = xr;
}

/*
 * Arguments    : double x[3]
 * Return Type  : void
 */
void d_sqrt(double x[3])
{
  x[0] = sqrt(x[0]);
  x[1] = sqrt(x[1]);
  x[2] = sqrt(x[2]);
}

/*
 * File trailer for sqrt.c
 *
 * [EOF]
 */
