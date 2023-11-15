/*
 * File: xdlanv2.c
 *
 * MATLAB Coder version            : 4.2
 * C/C++ source code generated on  : 15-Jan-2023 14:17:59
 */

/* Include Files */
#include <math.h>
#include "rt_nonfinite.h"
#include "magCalibration4.h"
#include "xdlanv2.h"
#include "inv.h"
#include "magCalibration4_rtwutil.h"

/* Function Definitions */

/*
 * Arguments    : double *a
 *                double *b
 *                double *c
 *                double *d
 *                double *rt1r
 *                double *rt1i
 *                double *rt2r
 *                double *rt2i
 *                double *cs
 *                double *sn
 * Return Type  : void
 */
void xdlanv2(double *a, double *b, double *c, double *d, double *rt1r, double
             *rt1i, double *rt2r, double *rt2i, double *cs, double *sn)
{
  double d4;
  double p;
  double bcmis;
  double scale;
  double bcmax;
  double b_scale;
  int b_b;
  int b_c;
  double z;
  double tau;
  double b_p;
  int c_scale;
  if (*c == 0.0) {
    *cs = 1.0;
    *sn = 0.0;
  } else if (*b == 0.0) {
    *cs = 0.0;
    *sn = 1.0;
    bcmis = *d;
    *d = *a;
    *a = bcmis;
    *b = -*c;
    *c = 0.0;
  } else {
    d4 = *a - *d;
    if ((d4 == 0.0) && ((*b < 0.0) != (*c < 0.0))) {
      *cs = 1.0;
      *sn = 0.0;
    } else {
      p = 0.5 * d4;
      scale = fabs(*b);
      bcmis = fabs(*c);
      if ((scale > bcmis) || rtIsNaN(bcmis)) {
        bcmax = scale;
      } else {
        bcmax = bcmis;
      }

      if ((scale < bcmis) || rtIsNaN(bcmis)) {
        b_scale = scale;
      } else {
        b_scale = bcmis;
      }

      if (!(*b < 0.0)) {
        b_b = 1;
      } else {
        b_b = -1;
      }

      if (!(*c < 0.0)) {
        b_c = 1;
      } else {
        b_c = -1;
      }

      bcmis = b_scale * (double)b_b * (double)b_c;
      scale = fabs(p);
      if ((scale > bcmax) || rtIsNaN(bcmax)) {
      } else {
        scale = bcmax;
      }

      z = p / scale * p + bcmax / scale * bcmis;
      if (z >= 8.8817841970012523E-16) {
        *a = sqrt(scale) * sqrt(z);
        if (!(p < 0.0)) {
          b_p = *a;
        } else {
          b_p = -*a;
        }

        z = p + b_p;
        *a = *d + z;
        *d -= bcmax / z * bcmis;
        tau = rt_hypotd_snf(*c, z);
        *cs = z / tau;
        *sn = *c / tau;
        *b -= *c;
        *c = 0.0;
      } else {
        scale = *b + *c;
        tau = rt_hypotd_snf(scale, d4);
        *cs = sqrt(0.5 * (1.0 + fabs(scale) / tau));
        if (!(scale < 0.0)) {
          c_scale = 1;
        } else {
          c_scale = -1;
        }

        *sn = -(p / (tau * *cs)) * (double)c_scale;
        bcmax = *a * *cs + *b * *sn;
        bcmis = -*a * *sn + *b * *cs;
        z = *c * *cs + *d * *sn;
        scale = -*c * *sn + *d * *cs;
        *b = bcmis * *cs + scale * *sn;
        *c = -bcmax * *sn + z * *cs;
        bcmis = 0.5 * ((bcmax * *cs + z * *sn) + (-bcmis * *sn + scale * *cs));
        *a = bcmis;
        *d = bcmis;
        if (*c != 0.0) {
          if (*b != 0.0) {
            if ((*b < 0.0) == (*c < 0.0)) {
              d4 = sqrt(fabs(*b));
              scale = sqrt(fabs(*c));
              *a = d4 * scale;
              if (!(*c < 0.0)) {
                p = *a;
              } else {
                p = -*a;
              }

              tau = 1.0 / sqrt(fabs(*b + *c));
              *a = bcmis + p;
              *d = bcmis - p;
              *b -= *c;
              *c = 0.0;
              bcmax = d4 * tau;
              scale *= tau;
              bcmis = *cs * bcmax - *sn * scale;
              *sn = *cs * scale + *sn * bcmax;
              *cs = bcmis;
            }
          } else {
            *b = -*c;
            *c = 0.0;
            bcmis = *cs;
            *cs = -*sn;
            *sn = bcmis;
          }
        }
      }
    }
  }

  *rt1r = *a;
  *rt2r = *d;
  if (*c == 0.0) {
    *rt1i = 0.0;
    *rt2i = 0.0;
  } else {
    *rt1i = sqrt(fabs(*b)) * sqrt(fabs(*c));
    *rt2i = -*rt1i;
  }
}

/*
 * File trailer for xdlanv2.c
 *
 * [EOF]
 */
