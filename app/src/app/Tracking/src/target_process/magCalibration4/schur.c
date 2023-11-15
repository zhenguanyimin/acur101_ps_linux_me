/*
 * File: schur.c
 *
 * MATLAB Coder version            : 4.2
 * C/C++ source code generated on  : 15-Jan-2023 14:17:59
 */

/* Include Files */
#include <math.h>
#include <string.h>
#include "rt_nonfinite.h"
#include "magCalibration4.h"
#include "schur.h"
#include "inv.h"
#include "xdlanv2.h"
#include "xhseqr.h"
#include "xzlarf.h"
#include "xnrm2.h"
#include "magCalibration4_rtwutil.h"

/* Function Definitions */

/*
 * Arguments    : const double A[9]
 *                creal_T V[9]
 *                creal_T T[9]
 * Return Type  : void
 */
void schur(const double A[9], creal_T V[9], creal_T T[9])
{
  boolean_T p;
  int rowleft;
  double b_A[9];
  int i5;
  double work[3];
  double alpha1;
  double tau[2];
  double xnorm;
  int lastv;
  int lastc;
  int knt;
  double a;
  boolean_T exitg2;
  int ix;
  int j;
  int ia;
  int i6;
  int exitg1;
  double Vr[9];
  double s;
  double t1_re;
  double t1_im;
  double rt1i;
  double rt2r;
  double rt2i;
  double mu1_im;
  double mu1_re;
  p = true;
  for (rowleft = 0; rowleft < 9; rowleft++) {
    if (p && ((!rtIsInf(A[rowleft])) && (!rtIsNaN(A[rowleft])))) {
      p = true;
    } else {
      p = false;
    }
  }

  if (!p) {
    for (i5 = 0; i5 < 9; i5++) {
      V[i5].re = rtNaN;
      V[i5].im = 0.0;
    }

    V[2].re = 0.0;
    V[2].im = 0.0;
    for (i5 = 0; i5 < 9; i5++) {
      T[i5].re = rtNaN;
      T[i5].im = 0.0;
    }
  } else {
    memcpy(&b_A[0], &A[0], 9U * sizeof(double));
    work[0] = 0.0;
    work[1] = 0.0;
    work[2] = 0.0;
    alpha1 = b_A[1];
    tau[0] = 0.0;
    xnorm = xnrm2(1, b_A, 3);
    if (xnorm != 0.0) {
      xnorm = rt_hypotd_snf(b_A[1], xnorm);
      if (b_A[1] >= 0.0) {
        xnorm = -xnorm;
      }

      if (fabs(xnorm) < 1.0020841800044864E-292) {
        knt = -1;
        do {
          knt++;
          for (rowleft = 3; rowleft < 4; rowleft++) {
            b_A[2] *= 9.9792015476736E+291;
          }

          xnorm *= 9.9792015476736E+291;
          alpha1 *= 9.9792015476736E+291;
        } while (!(fabs(xnorm) >= 1.0020841800044864E-292));

        xnorm = rt_hypotd_snf(alpha1, xnrm2(1, b_A, 3));
        if (alpha1 >= 0.0) {
          xnorm = -xnorm;
        }

        tau[0] = (xnorm - alpha1) / xnorm;
        a = 1.0 / (alpha1 - xnorm);
        for (rowleft = 3; rowleft < 4; rowleft++) {
          b_A[2] *= a;
        }

        for (rowleft = 0; rowleft <= knt; rowleft++) {
          xnorm *= 1.0020841800044864E-292;
        }

        alpha1 = xnorm;
      } else {
        tau[0] = (xnorm - b_A[1]) / xnorm;
        a = 1.0 / (b_A[1] - xnorm);
        for (rowleft = 3; rowleft < 4; rowleft++) {
          b_A[2] *= a;
        }

        alpha1 = xnorm;
      }
    }

    b_A[1] = 1.0;
    if (tau[0] != 0.0) {
      lastv = 1;
      knt = 2;
      while ((lastv + 1 > 0) && (b_A[knt] == 0.0)) {
        lastv--;
        knt--;
      }

      lastc = 3;
      exitg2 = false;
      while ((!exitg2) && (lastc > 0)) {
        rowleft = 3 + lastc;
        ia = rowleft;
        do {
          exitg1 = 0;
          if (ia <= rowleft + lastv * 3) {
            if (b_A[ia - 1] != 0.0) {
              exitg1 = 1;
            } else {
              ia += 3;
            }
          } else {
            lastc--;
            exitg1 = 2;
          }
        } while (exitg1 == 0);

        if (exitg1 == 1) {
          exitg2 = true;
        }
      }
    } else {
      lastv = -1;
      lastc = 0;
    }

    if (lastv + 1 > 0) {
      if (lastc != 0) {
        memset(&work[0], 0, (unsigned int)(lastc * (int)sizeof(double)));
        ix = 1;
        i5 = 3 * lastv + 4;
        for (knt = 4; knt <= i5; knt += 3) {
          rowleft = 0;
          i6 = (knt + lastc) - 1;
          for (ia = knt; ia <= i6; ia++) {
            work[rowleft] += b_A[ia - 1] * b_A[ix];
            rowleft++;
          }

          ix++;
        }
      }

      if (!(-tau[0] == 0.0)) {
        knt = 3;
        rowleft = 1;
        for (j = 0; j <= lastv; j++) {
          if (b_A[rowleft] != 0.0) {
            xnorm = b_A[rowleft] * -tau[0];
            ix = 0;
            i5 = knt + 1;
            i6 = lastc + knt;
            for (ia = i5; ia <= i6; ia++) {
              b_A[ia - 1] += work[ix] * xnorm;
              ix++;
            }
          }

          rowleft++;
          knt += 3;
        }
      }
    }

    xzlarf(2, 2, 2, tau[0], b_A, 5, work);
    b_A[1] = alpha1;
    alpha1 = b_A[5];
    tau[1] = 0.0;
    xnorm = xnrm2(0, b_A, 6);
    if (xnorm != 0.0) {
      xnorm = rt_hypotd_snf(b_A[5], xnorm);
      if (b_A[5] >= 0.0) {
        xnorm = -xnorm;
      }

      if (fabs(xnorm) < 1.0020841800044864E-292) {
        knt = -1;
        do {
          knt++;
          xnorm *= 9.9792015476736E+291;
          alpha1 *= 9.9792015476736E+291;
        } while (!(fabs(xnorm) >= 1.0020841800044864E-292));

        xnorm = rt_hypotd_snf(alpha1, xnrm2(0, b_A, 6));
        if (alpha1 >= 0.0) {
          xnorm = -xnorm;
        }

        tau[1] = (xnorm - alpha1) / xnorm;
        for (rowleft = 0; rowleft <= knt; rowleft++) {
          xnorm *= 1.0020841800044864E-292;
        }

        alpha1 = xnorm;
      } else {
        tau[1] = (xnorm - b_A[5]) / xnorm;
        alpha1 = xnorm;
      }
    }

    b_A[5] = 1.0;
    if (tau[1] != 0.0) {
      lastv = 0;
      knt = 5;
      while ((lastv + 1 > 0) && (b_A[knt] == 0.0)) {
        lastv--;
        knt--;
      }

      lastc = 3;
      exitg2 = false;
      while ((!exitg2) && (lastc > 0)) {
        rowleft = 6 + lastc;
        ia = rowleft;
        do {
          exitg1 = 0;
          if (ia <= rowleft + lastv * 3) {
            if (b_A[ia - 1] != 0.0) {
              exitg1 = 1;
            } else {
              ia += 3;
            }
          } else {
            lastc--;
            exitg1 = 2;
          }
        } while (exitg1 == 0);

        if (exitg1 == 1) {
          exitg2 = true;
        }
      }
    } else {
      lastv = -1;
      lastc = 0;
    }

    if (lastv + 1 > 0) {
      if (lastc != 0) {
        memset(&work[0], 0, (unsigned int)(lastc * (int)sizeof(double)));
        ix = 5;
        i5 = 3 * lastv + 7;
        for (knt = 7; knt <= i5; knt += 3) {
          rowleft = 0;
          i6 = (knt + lastc) - 1;
          for (ia = knt; ia <= i6; ia++) {
            work[rowleft] += b_A[ia - 1] * b_A[ix];
            rowleft++;
          }

          ix++;
        }
      }

      if (!(-tau[1] == 0.0)) {
        knt = 6;
        rowleft = 5;
        for (j = 0; j <= lastv; j++) {
          if (b_A[rowleft] != 0.0) {
            xnorm = b_A[rowleft] * -tau[1];
            ix = 0;
            i5 = knt + 1;
            i6 = lastc + knt;
            for (ia = i5; ia <= i6; ia++) {
              b_A[ia - 1] += work[ix] * xnorm;
              ix++;
            }
          }

          rowleft++;
          knt += 3;
        }
      }
    }

    xzlarf(1, 1, 6, tau[1], b_A, 9, work);
    b_A[5] = alpha1;
    memcpy(&Vr[0], &b_A[0], 9U * sizeof(double));
    for (knt = 0; knt < 2; knt++) {
      Vr[6 + knt] = 0.0;
    }

    for (knt = 0; knt < 1; knt++) {
      Vr[3] = 0.0;
    }

    for (knt = 3; knt < 4; knt++) {
      Vr[5] = Vr[2];
    }

    Vr[1] = 0.0;
    Vr[2] = 0.0;
    Vr[0] = 1.0;
    work[0] = 0.0;
    work[1] = 0.0;
    work[2] = 0.0;
    Vr[8] = 1.0 - tau[1];
    Vr[7] = 0.0;
    Vr[4] = 1.0;
    xzlarf(2, 1, 5, tau[0], Vr, 8, work);
    for (rowleft = 6; rowleft < 7; rowleft++) {
      Vr[5] *= -tau[0];
    }

    Vr[4] = 1.0 - tau[0];
    xhseqr(b_A, Vr);
    for (i5 = 0; i5 < 9; i5++) {
      T[i5].re = b_A[i5];
      T[i5].im = 0.0;
      V[i5].re = Vr[i5];
      V[i5].im = 0.0;
    }

    for (lastv = 1; lastv >= 0; lastv--) {
      ix = lastv + 1;
      i5 = lastv + 3 * lastv;
      i6 = i5 + 1;
      if (b_A[i6] != 0.0) {
        a = b_A[i5];
        lastc = 3 * (lastv + 1);
        knt = lastv + lastc;
        xnorm = b_A[knt];
        alpha1 = b_A[i6];
        s = b_A[knt + 1];
        t1_re = s;
        xdlanv2(&a, &xnorm, &alpha1, &t1_re, &t1_im, &rt1i, &rt2r, &rt2i,
                &mu1_im, &mu1_re);
        mu1_re = t1_im - s;
        xnorm = rt_hypotd_snf(rt_hypotd_snf(mu1_re, rt1i), b_A[i6]);
        if (rt1i == 0.0) {
          mu1_re /= xnorm;
          mu1_im = 0.0;
        } else if (mu1_re == 0.0) {
          mu1_re = 0.0;
          mu1_im = rt1i / xnorm;
        } else {
          mu1_re /= xnorm;
          mu1_im = rt1i / xnorm;
        }

        s = b_A[i6] / xnorm;
        for (j = ix; j < 4; j++) {
          rowleft = lastv + 3 * (j - 1);
          t1_im = T[lastv + 3 * (j - 1)].im;
          t1_re = T[rowleft].re;
          ia = rowleft + 1;
          rt2r = T[ia].re;
          rt2i = T[(lastv + 3 * (j - 1)) + 1].im;
          xnorm = T[rowleft].re;
          T[rowleft].re = (mu1_re * T[rowleft].re + mu1_im * t1_im) + s * T[ia].
            re;
          T[rowleft].im = (mu1_re * t1_im - mu1_im * xnorm) + s * rt2i;
          xnorm = mu1_re * rt2r - mu1_im * rt2i;
          alpha1 = mu1_re * rt2i + mu1_im * rt2r;
          T[ia].re = xnorm - s * t1_re;
          T[ia].im = alpha1 - s * t1_im;
        }

        for (knt = 0; knt <= lastv + 1; knt++) {
          rowleft = knt + 3 * lastv;
          t1_im = T[knt + 3 * lastv].im;
          t1_re = T[rowleft].re;
          ia = knt + lastc;
          rt2r = T[ia].re;
          rt2i = T[knt + 3 * (lastv + 1)].im;
          xnorm = mu1_re * T[rowleft].re - mu1_im * t1_im;
          alpha1 = mu1_re * t1_im + mu1_im * T[rowleft].re;
          T[rowleft].re = xnorm + s * T[ia].re;
          T[rowleft].im = alpha1 + s * rt2i;
          T[ia].re = (mu1_re * rt2r + mu1_im * rt2i) - s * t1_re;
          T[ia].im = (mu1_re * rt2i - mu1_im * rt2r) - s * t1_im;
        }

        t1_re = V[3 * lastv].re;
        t1_im = V[3 * lastv].im;
        rt2r = V[lastc].re;
        rt2i = V[3 * (lastv + 1)].im;
        xnorm = mu1_re * t1_re - mu1_im * t1_im;
        alpha1 = mu1_re * t1_im + mu1_im * t1_re;
        V[3 * lastv].re = xnorm + s * V[lastc].re;
        V[3 * lastv].im = alpha1 + s * rt2i;
        V[lastc].re = (mu1_re * rt2r + mu1_im * rt2i) - s * t1_re;
        V[lastc].im = (mu1_re * rt2i - mu1_im * rt2r) - s * t1_im;
        rowleft = 1 + 3 * lastv;
        t1_im = V[1 + 3 * lastv].im;
        t1_re = V[rowleft].re;
        ia = 1 + lastc;
        rt2r = V[ia].re;
        rt2i = V[1 + 3 * (lastv + 1)].im;
        xnorm = mu1_re * V[rowleft].re - mu1_im * t1_im;
        alpha1 = mu1_re * t1_im + mu1_im * V[rowleft].re;
        V[rowleft].re = xnorm + s * V[ia].re;
        V[rowleft].im = alpha1 + s * rt2i;
        V[ia].re = (mu1_re * rt2r + mu1_im * rt2i) - s * t1_re;
        V[ia].im = (mu1_re * rt2i - mu1_im * rt2r) - s * t1_im;
        rowleft = 2 + 3 * lastv;
        t1_im = V[2 + 3 * lastv].im;
        t1_re = V[rowleft].re;
        ia = 2 + lastc;
        rt2r = V[ia].re;
        rt2i = V[2 + 3 * (lastv + 1)].im;
        xnorm = mu1_re * V[rowleft].re - mu1_im * t1_im;
        alpha1 = mu1_re * t1_im + mu1_im * V[rowleft].re;
        V[rowleft].re = xnorm + s * V[ia].re;
        V[rowleft].im = alpha1 + s * rt2i;
        V[ia].re = (mu1_re * rt2r + mu1_im * rt2i) - s * t1_re;
        V[ia].im = (mu1_re * rt2i - mu1_im * rt2r) - s * t1_im;
        T[i6].re = 0.0;
        T[i6].im = 0.0;
      }
    }
  }
}

/*
 * File trailer for schur.c
 *
 * [EOF]
 */
