/*
 * File: eig.c
 *
 * MATLAB Coder version            : 4.2
 * C/C++ source code generated on  : 15-Jan-2023 14:17:59
 */

/* Include Files */
#include <math.h>
#include "rt_nonfinite.h"
#include "magCalibration4.h"
#include "eig.h"
#include "schur.h"
#include "xzggev.h"

/* Function Definitions */

/*
 * Arguments    : const double A[9]
 *                creal_T V[9]
 *                creal_T D[9]
 * Return Type  : void
 */
void eig(const double A[9], creal_T V[9], creal_T D[9])
{
  boolean_T p;
  int info;
  int kend;
  boolean_T exitg2;
  int exitg1;
  creal_T At[9];
  creal_T alpha1[3];
  creal_T beta1[3];
  int coltop;
  double colnorm;
  double scale;
  double t;
  double absxk;
  p = true;
  for (info = 0; info < 9; info++) {
    if (p && ((!rtIsInf(A[info])) && (!rtIsNaN(A[info])))) {
      p = true;
    } else {
      p = false;
    }
  }

  if (!p) {
    for (info = 0; info < 9; info++) {
      V[info].re = rtNaN;
      V[info].im = 0.0;
      D[info].re = 0.0;
      D[info].im = 0.0;
    }

    D[0].re = rtNaN;
    D[0].im = 0.0;
    D[4].re = rtNaN;
    D[4].im = 0.0;
    D[8].re = rtNaN;
    D[8].im = 0.0;
  } else {
    p = true;
    kend = 0;
    exitg2 = false;
    while ((!exitg2) && (kend < 3)) {
      info = 0;
      do {
        exitg1 = 0;
        if (info <= kend) {
          if (!(A[info + 3 * kend] == A[kend + 3 * info])) {
            p = false;
            exitg1 = 1;
          } else {
            info++;
          }
        } else {
          kend++;
          exitg1 = 2;
        }
      } while (exitg1 == 0);

      if (exitg1 == 1) {
        exitg2 = true;
      }
    }

    if (p) {
      schur(A, V, D);
      D[0].im = 0.0;
      D[4].im = 0.0;
      D[1].re = 0.0;
      D[1].im = 0.0;
      for (info = 0; info < 1; info++) {
        D[3].re = 0.0;
        D[3].im = 0.0;
      }

      D[8].im = 0.0;
      D[5].re = 0.0;
      D[5].im = 0.0;
      for (info = 0; info < 2; info++) {
        D[info + 6].re = 0.0;
        D[info + 6].im = 0.0;
      }
    } else {
      for (info = 0; info < 9; info++) {
        At[info].re = A[info];
        At[info].im = 0.0;
      }

      xzggev(At, &info, alpha1, beta1, V);
      for (coltop = 0; coltop <= 6; coltop += 3) {
        colnorm = 0.0;
        scale = 3.3121686421112381E-170;
        kend = coltop + 3;
        for (info = coltop + 1; info <= kend; info++) {
          absxk = fabs(V[info - 1].re);
          if (absxk > scale) {
            t = scale / absxk;
            colnorm = 1.0 + colnorm * t * t;
            scale = absxk;
          } else {
            t = absxk / scale;
            colnorm += t * t;
          }

          absxk = fabs(V[info - 1].im);
          if (absxk > scale) {
            t = scale / absxk;
            colnorm = 1.0 + colnorm * t * t;
            scale = absxk;
          } else {
            t = absxk / scale;
            colnorm += t * t;
          }
        }

        colnorm = scale * sqrt(colnorm);
        info = coltop + 3;
        for (kend = coltop + 1; kend <= info; kend++) {
          scale = V[kend - 1].re;
          absxk = V[kend - 1].im;
          if (absxk == 0.0) {
            V[kend - 1].re = scale / colnorm;
            V[kend - 1].im = 0.0;
          } else if (scale == 0.0) {
            V[kend - 1].re = 0.0;
            V[kend - 1].im = absxk / colnorm;
          } else {
            V[kend - 1].re = scale / colnorm;
            V[kend - 1].im = absxk / colnorm;
          }
        }
      }

      for (info = 0; info < 9; info++) {
        D[info].re = 0.0;
        D[info].im = 0.0;
      }

      if (beta1[0].im == 0.0) {
        if (alpha1[0].im == 0.0) {
          D[0].re = alpha1[0].re / beta1[0].re;
          D[0].im = 0.0;
        } else if (alpha1[0].re == 0.0) {
          D[0].re = 0.0;
          D[0].im = alpha1[0].im / beta1[0].re;
        } else {
          D[0].re = alpha1[0].re / beta1[0].re;
          D[0].im = alpha1[0].im / beta1[0].re;
        }
      } else if (beta1[0].re == 0.0) {
        if (alpha1[0].re == 0.0) {
          D[0].re = alpha1[0].im / beta1[0].im;
          D[0].im = 0.0;
        } else if (alpha1[0].im == 0.0) {
          D[0].re = 0.0;
          D[0].im = -(alpha1[0].re / beta1[0].im);
        } else {
          D[0].re = alpha1[0].im / beta1[0].im;
          D[0].im = -(alpha1[0].re / beta1[0].im);
        }
      } else {
        t = fabs(beta1[0].re);
        scale = fabs(beta1[0].im);
        if (t > scale) {
          scale = beta1[0].im / beta1[0].re;
          absxk = beta1[0].re + scale * beta1[0].im;
          D[0].re = (alpha1[0].re + scale * alpha1[0].im) / absxk;
          D[0].im = (alpha1[0].im - scale * alpha1[0].re) / absxk;
        } else if (scale == t) {
          if (beta1[0].re > 0.0) {
            scale = 0.5;
          } else {
            scale = -0.5;
          }

          if (beta1[0].im > 0.0) {
            absxk = 0.5;
          } else {
            absxk = -0.5;
          }

          D[0].re = (alpha1[0].re * scale + alpha1[0].im * absxk) / t;
          D[0].im = (alpha1[0].im * scale - alpha1[0].re * absxk) / t;
        } else {
          scale = beta1[0].re / beta1[0].im;
          absxk = beta1[0].im + scale * beta1[0].re;
          D[0].re = (scale * alpha1[0].re + alpha1[0].im) / absxk;
          D[0].im = (scale * alpha1[0].im - alpha1[0].re) / absxk;
        }
      }

      if (beta1[1].im == 0.0) {
        if (alpha1[1].im == 0.0) {
          D[4].re = alpha1[1].re / beta1[1].re;
          D[4].im = 0.0;
        } else if (alpha1[1].re == 0.0) {
          D[4].re = 0.0;
          D[4].im = alpha1[1].im / beta1[1].re;
        } else {
          D[4].re = alpha1[1].re / beta1[1].re;
          D[4].im = alpha1[1].im / beta1[1].re;
        }
      } else if (beta1[1].re == 0.0) {
        if (alpha1[1].re == 0.0) {
          D[4].re = alpha1[1].im / beta1[1].im;
          D[4].im = 0.0;
        } else if (alpha1[1].im == 0.0) {
          D[4].re = 0.0;
          D[4].im = -(alpha1[1].re / beta1[1].im);
        } else {
          D[4].re = alpha1[1].im / beta1[1].im;
          D[4].im = -(alpha1[1].re / beta1[1].im);
        }
      } else {
        t = fabs(beta1[1].re);
        scale = fabs(beta1[1].im);
        if (t > scale) {
          scale = beta1[1].im / beta1[1].re;
          absxk = beta1[1].re + scale * beta1[1].im;
          D[4].re = (alpha1[1].re + scale * alpha1[1].im) / absxk;
          D[4].im = (alpha1[1].im - scale * alpha1[1].re) / absxk;
        } else if (scale == t) {
          if (beta1[1].re > 0.0) {
            scale = 0.5;
          } else {
            scale = -0.5;
          }

          if (beta1[1].im > 0.0) {
            absxk = 0.5;
          } else {
            absxk = -0.5;
          }

          D[4].re = (alpha1[1].re * scale + alpha1[1].im * absxk) / t;
          D[4].im = (alpha1[1].im * scale - alpha1[1].re * absxk) / t;
        } else {
          scale = beta1[1].re / beta1[1].im;
          absxk = beta1[1].im + scale * beta1[1].re;
          D[4].re = (scale * alpha1[1].re + alpha1[1].im) / absxk;
          D[4].im = (scale * alpha1[1].im - alpha1[1].re) / absxk;
        }
      }

      if (beta1[2].im == 0.0) {
        if (alpha1[2].im == 0.0) {
          D[8].re = alpha1[2].re / beta1[2].re;
          D[8].im = 0.0;
        } else if (alpha1[2].re == 0.0) {
          D[8].re = 0.0;
          D[8].im = alpha1[2].im / beta1[2].re;
        } else {
          D[8].re = alpha1[2].re / beta1[2].re;
          D[8].im = alpha1[2].im / beta1[2].re;
        }
      } else if (beta1[2].re == 0.0) {
        if (alpha1[2].re == 0.0) {
          D[8].re = alpha1[2].im / beta1[2].im;
          D[8].im = 0.0;
        } else if (alpha1[2].im == 0.0) {
          D[8].re = 0.0;
          D[8].im = -(alpha1[2].re / beta1[2].im);
        } else {
          D[8].re = alpha1[2].im / beta1[2].im;
          D[8].im = -(alpha1[2].re / beta1[2].im);
        }
      } else {
        t = fabs(beta1[2].re);
        scale = fabs(beta1[2].im);
        if (t > scale) {
          scale = beta1[2].im / beta1[2].re;
          absxk = beta1[2].re + scale * beta1[2].im;
          D[8].re = (alpha1[2].re + scale * alpha1[2].im) / absxk;
          D[8].im = (alpha1[2].im - scale * alpha1[2].re) / absxk;
        } else if (scale == t) {
          if (beta1[2].re > 0.0) {
            scale = 0.5;
          } else {
            scale = -0.5;
          }

          if (beta1[2].im > 0.0) {
            absxk = 0.5;
          } else {
            absxk = -0.5;
          }

          D[8].re = (alpha1[2].re * scale + alpha1[2].im * absxk) / t;
          D[8].im = (alpha1[2].im * scale - alpha1[2].re * absxk) / t;
        } else {
          scale = beta1[2].re / beta1[2].im;
          absxk = beta1[2].im + scale * beta1[2].re;
          D[8].re = (scale * alpha1[2].re + alpha1[2].im) / absxk;
          D[8].im = (scale * alpha1[2].im - alpha1[2].re) / absxk;
        }
      }
    }
  }
}

/*
 * File trailer for eig.c
 *
 * [EOF]
 */
