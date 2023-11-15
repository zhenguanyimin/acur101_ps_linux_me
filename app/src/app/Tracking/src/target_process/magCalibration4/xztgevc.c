/*
 * File: xztgevc.c
 *
 * MATLAB Coder version            : 4.2
 * C/C++ source code generated on  : 15-Jan-2023 14:17:59
 */

/* Include Files */
#include <math.h>
#include "rt_nonfinite.h"
#include "magCalibration4.h"
#include "xztgevc.h"

/* Function Definitions */

/*
 * Arguments    : const creal_T A[9]
 *                creal_T V[9]
 * Return Type  : void
 */
void xztgevc(const creal_T A[9], creal_T V[9])
{
  double rworka[3];
  double anorm;
  int i;
  double d_re;
  double xmx;
  double ascale;
  int je;
  int x_tmp;
  double temp;
  double salpha_re;
  double salpha_im;
  double acoeff;
  boolean_T lscalea;
  double z;
  boolean_T lscaleb;
  double scale;
  creal_T work1[3];
  double dmin;
  int i14;
  int j;
  creal_T work2[3];
  int b_j;
  double d_im;
  int i15;
  rworka[1] = 0.0;
  rworka[2] = 0.0;
  anorm = fabs(A[0].re) + fabs(A[0].im);
  for (i = 0; i < 1; i++) {
    rworka[1] += fabs(A[3].re) + fabs(A[3].im);
  }

  d_re = rworka[1] + (fabs(A[4].re) + fabs(A[4].im));
  if (d_re > anorm) {
    anorm = d_re;
  }

  for (i = 0; i < 2; i++) {
    rworka[2] += fabs(A[i + 6].re) + fabs(A[i + 6].im);
  }

  d_re = rworka[2] + (fabs(A[8].re) + fabs(A[8].im));
  if (d_re > anorm) {
    anorm = d_re;
  }

  xmx = anorm;
  if (2.2250738585072014E-308 > anorm) {
    xmx = 2.2250738585072014E-308;
  }

  ascale = 1.0 / xmx;
  for (je = 0; je < 3; je++) {
    x_tmp = 3 * (2 - je);
    xmx = (fabs(A[(x_tmp - je) + 2].re) + fabs(A[(3 * (2 - je) - je) + 2].im)) *
      ascale;
    if (1.0 > xmx) {
      xmx = 1.0;
    }

    temp = 1.0 / xmx;
    salpha_re = ascale * (temp * A[(3 * (2 - je) - je) + 2].re);
    salpha_im = ascale * (temp * A[(3 * (2 - je) - je) + 2].im);
    acoeff = temp * ascale;
    if ((temp >= 2.2250738585072014E-308) && (acoeff < 3.0062525400134592E-292))
    {
      lscalea = true;
    } else {
      lscalea = false;
    }

    z = fabs(salpha_re) + fabs(salpha_im);
    if ((z >= 2.2250738585072014E-308) && (z < 3.0062525400134592E-292)) {
      lscaleb = true;
    } else {
      lscaleb = false;
    }

    scale = 1.0;
    if (lscalea) {
      xmx = anorm;
      if (3.3264005158911995E+291 < anorm) {
        xmx = 3.3264005158911995E+291;
      }

      scale = 3.0062525400134592E-292 / temp * xmx;
    }

    if (lscaleb) {
      d_re = 3.0062525400134592E-292 / z;
      if (d_re > scale) {
        scale = d_re;
      }
    }

    if (lscalea || lscaleb) {
      xmx = acoeff;
      if (1.0 > acoeff) {
        xmx = 1.0;
      }

      if (z > xmx) {
        xmx = z;
      }

      d_re = 1.0 / (2.2250738585072014E-308 * xmx);
      if (d_re < scale) {
        scale = d_re;
      }

      if (lscalea) {
        acoeff = ascale * (scale * temp);
      } else {
        acoeff *= scale;
      }

      salpha_re *= scale;
      salpha_im *= scale;
    }

    work1[0].re = 0.0;
    work1[0].im = 0.0;
    work1[1].re = 0.0;
    work1[1].im = 0.0;
    work1[2].re = 0.0;
    work1[2].im = 0.0;
    work1[2 - je].re = 1.0;
    work1[2 - je].im = 0.0;
    dmin = 2.2204460492503131E-16 * acoeff * anorm;
    d_re = 2.2204460492503131E-16 * (fabs(salpha_re) + fabs(salpha_im));
    if (d_re > dmin) {
      dmin = d_re;
    }

    if (2.2250738585072014E-308 > dmin) {
      dmin = 2.2250738585072014E-308;
    }

    i14 = 1 - je;
    for (i = 0; i <= i14; i++) {
      work1[i].re = acoeff * A[i + x_tmp].re;
      work1[i].im = acoeff * A[i + 3 * (2 - je)].im;
    }

    work1[2 - je].re = 1.0;
    work1[2 - je].im = 0.0;
    i14 = (int)((1.0 + (-1.0 - ((3.0 + -(double)je) - 1.0))) / -1.0);
    for (j = 0; j < i14; j++) {
      b_j = 1 - (je + j);
      d_re = acoeff * A[b_j + 3 * b_j].re - salpha_re;
      d_im = acoeff * A[b_j + 3 * b_j].im - salpha_im;
      if (fabs(d_re) + fabs(d_im) <= dmin) {
        d_re = dmin;
        d_im = 0.0;
      }

      xmx = fabs(d_re) + fabs(d_im);
      if (xmx < 1.0) {
        z = fabs(work1[b_j].re) + fabs(work1[b_j].im);
        if (z >= 1.4980776123852632E+307 * xmx) {
          temp = 1.0 / z;
          i15 = 2 - je;
          for (i = 0; i <= i15; i++) {
            work1[i].re *= temp;
            work1[i].im *= temp;
          }
        }
      }

      temp = -work1[b_j].re;
      if (d_im == 0.0) {
        if (-work1[b_j].im == 0.0) {
          work1[b_j].re = -work1[b_j].re / d_re;
          work1[b_j].im = 0.0;
        } else if (-work1[b_j].re == 0.0) {
          work1[b_j].re = 0.0;
          work1[b_j].im = -work1[b_j].im / d_re;
        } else {
          work1[b_j].re = -work1[b_j].re / d_re;
          work1[b_j].im = -work1[b_j].im / d_re;
        }
      } else if (d_re == 0.0) {
        if (-work1[b_j].re == 0.0) {
          work1[b_j].re = -work1[b_j].im / d_im;
          work1[b_j].im = 0.0;
        } else if (-work1[b_j].im == 0.0) {
          work1[b_j].re = 0.0;
          work1[b_j].im = -(temp / d_im);
        } else {
          work1[b_j].re = -work1[b_j].im / d_im;
          work1[b_j].im = -(temp / d_im);
        }
      } else {
        scale = fabs(d_re);
        xmx = fabs(d_im);
        if (scale > xmx) {
          scale = d_im / d_re;
          z = d_re + scale * d_im;
          work1[b_j].re = (-work1[b_j].re + scale * -work1[b_j].im) / z;
          work1[b_j].im = (-work1[b_j].im - scale * temp) / z;
        } else if (xmx == scale) {
          if (d_re > 0.0) {
            z = 0.5;
          } else {
            z = -0.5;
          }

          if (d_im > 0.0) {
            xmx = 0.5;
          } else {
            xmx = -0.5;
          }

          work1[b_j].re = (-work1[b_j].re * z + -work1[b_j].im * xmx) / scale;
          work1[b_j].im = (-work1[b_j].im * z - temp * xmx) / scale;
        } else {
          scale = d_re / d_im;
          z = d_im + scale * d_re;
          xmx = scale * -work1[b_j].im - (-work1[b_j].re);
          work1[b_j].re = (scale * -work1[b_j].re + -work1[b_j].im) / z;
          work1[b_j].im = xmx / z;
        }
      }

      if (b_j + 1 > 1) {
        xmx = fabs(work1[1].re) + fabs(work1[1].im);
        if (xmx > 1.0) {
          temp = 1.0 / xmx;
          if (acoeff * rworka[1] >= 1.4980776123852632E+307 * temp) {
            i15 = 2 - je;
            for (i = 0; i <= i15; i++) {
              work1[i].re *= temp;
              work1[i].im *= temp;
            }
          }
        }

        d_re = acoeff * work1[1].re;
        d_im = acoeff * work1[1].im;
        work1[0].re += d_re * A[3].re - d_im * A[3].im;
        work1[0].im += d_re * A[3].im + d_im * A[3].re;
      }
    }

    work2[0].re = 0.0;
    work2[0].im = 0.0;
    work2[1].re = 0.0;
    work2[1].im = 0.0;
    work2[2].re = 0.0;
    work2[2].im = 0.0;
    i14 = 2 - je;
    for (i = 0; i <= i14; i++) {
      work2[0].re += V[3 * i].re * work1[i].re - V[3 * i].im * work1[i].im;
      work2[0].im += V[3 * i].re * work1[i].im + V[3 * i].im * work1[i].re;
      work2[1].re += V[1 + 3 * i].re * work1[i].re - V[1 + 3 * i].im * work1[i].
        im;
      work2[1].im += V[1 + 3 * i].re * work1[i].im + V[1 + 3 * i].im * work1[i].
        re;
      work2[2].re += V[2 + 3 * i].re * work1[i].re - V[2 + 3 * i].im * work1[i].
        im;
      work2[2].im += V[2 + 3 * i].re * work1[i].im + V[2 + 3 * i].im * work1[i].
        re;
    }

    xmx = fabs(work2[0].re) + fabs(work2[0].im);
    d_re = fabs(work2[1].re) + fabs(work2[1].im);
    if (d_re > xmx) {
      xmx = d_re;
    }

    d_re = fabs(work2[2].re) + fabs(work2[2].im);
    if (d_re > xmx) {
      xmx = d_re;
    }

    if (xmx > 2.2250738585072014E-308) {
      temp = 1.0 / xmx;
      V[x_tmp].re = temp * work2[0].re;
      V[x_tmp].im = temp * work2[0].im;
      i14 = 1 + x_tmp;
      V[i14].re = temp * work2[1].re;
      V[i14].im = temp * work2[1].im;
      i14 = 2 + x_tmp;
      V[i14].re = temp * work2[2].re;
      V[i14].im = temp * work2[2].im;
    } else {
      V[x_tmp].re = 0.0;
      V[x_tmp].im = 0.0;
      V[1 + 3 * (2 - je)].re = 0.0;
      V[1 + 3 * (2 - je)].im = 0.0;
      V[2 + 3 * (2 - je)].re = 0.0;
      V[2 + 3 * (2 - je)].im = 0.0;
    }
  }
}

/*
 * File trailer for xztgevc.c
 *
 * [EOF]
 */
