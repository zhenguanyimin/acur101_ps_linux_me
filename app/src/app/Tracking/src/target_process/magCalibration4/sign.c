/*
 * File: sign.c
 *
 * MATLAB Coder version            : 4.2
 * C/C++ source code generated on  : 15-Jan-2023 14:17:59
 */

/* Include Files */
#include <math.h>
#include "rt_nonfinite.h"
#include "magCalibration4.h"
#include "sign.h"
#include "inv.h"
#include "magCalibration4_rtwutil.h"

/* Function Definitions */

/*
 * Arguments    : creal_T x[3]
 * Return Type  : void
 */
void b_sign(creal_T x[3])
{
  double xr;
  double xi;
  double absx;
  xr = x[0].re;
  xi = x[0].im;
  if (xi == 0.0) {
    if (xr < 0.0) {
      xr = -1.0;
    } else if (xr > 0.0) {
      xr = 1.0;
    } else {
      if (xr == 0.0) {
        xr = 0.0;
      }
    }

    x[0].re = xr;
    x[0].im = 0.0;
  } else {
    if ((fabs(xr) > 8.9884656743115785E+307) || (fabs(xi) >
         8.9884656743115785E+307)) {
      xr /= 2.0;
      xi /= 2.0;
    }

    absx = rt_hypotd_snf(xr, xi);
    if (absx == 0.0) {
      x[0].re = 0.0;
      x[0].im = 0.0;
    } else {
      x[0].re = xr / absx;
      x[0].im = xi / absx;
    }
  }

  xr = x[1].re;
  xi = x[1].im;
  if (xi == 0.0) {
    if (xr < 0.0) {
      xr = -1.0;
    } else if (xr > 0.0) {
      xr = 1.0;
    } else {
      if (xr == 0.0) {
        xr = 0.0;
      }
    }

    x[1].re = xr;
    x[1].im = 0.0;
  } else {
    if ((fabs(xr) > 8.9884656743115785E+307) || (fabs(xi) >
         8.9884656743115785E+307)) {
      xr /= 2.0;
      xi /= 2.0;
    }

    absx = rt_hypotd_snf(xr, xi);
    if (absx == 0.0) {
      x[1].re = 0.0;
      x[1].im = 0.0;
    } else {
      x[1].re = xr / absx;
      x[1].im = xi / absx;
    }
  }

  xr = x[2].re;
  xi = x[2].im;
  if (xi == 0.0) {
    if (xr < 0.0) {
      xr = -1.0;
    } else if (xr > 0.0) {
      xr = 1.0;
    } else {
      if (xr == 0.0) {
        xr = 0.0;
      }
    }

    x[2].re = xr;
    x[2].im = 0.0;
  } else {
    if ((fabs(xr) > 8.9884656743115785E+307) || (fabs(xi) >
         8.9884656743115785E+307)) {
      xr /= 2.0;
      xi /= 2.0;
    }

    absx = rt_hypotd_snf(xr, xi);
    if (absx == 0.0) {
      x[2].re = 0.0;
      x[2].im = 0.0;
    } else {
      x[2].re = xr / absx;
      x[2].im = xi / absx;
    }
  }
}

/*
 * Arguments    : double *x
 * Return Type  : void
 */
void c_sign(double *x)
{
  if (*x < 0.0) {
    *x = -1.0;
  } else if (*x > 0.0) {
    *x = 1.0;
  } else {
    if (*x == 0.0) {
      *x = 0.0;
    }
  }
}

/*
 * File trailer for sign.c
 *
 * [EOF]
 */
