/*
 * File: xzggev.c
 *
 * MATLAB Coder version            : 4.2
 * C/C++ source code generated on  : 15-Jan-2023 14:17:59
 */

/* Include Files */
#include <math.h>
#include "rt_nonfinite.h"
#include "magCalibration4.h"
#include "xzggev.h"
#include "xzlartg.h"
#include "xztgevc.h"
#include "xzhgeqz.h"
#include "inv.h"
#include "sqrt.h"
#include "magCalibration4_rtwutil.h"

/* Function Definitions */

/*
 * Arguments    : creal_T A[9]
 *                int *info
 *                creal_T alpha1[3]
 *                creal_T beta1[3]
 *                creal_T V[9]
 * Return Type  : void
 */
void xzggev(creal_T A[9], int *info, creal_T alpha1[3], creal_T beta1[3],
            creal_T V[9])
{
  double absxk;
  double SMLNUM;
  double BIGNUM;
  double anrm;
  int k;
  boolean_T exitg1;
  boolean_T ilascl;
  double anrmto;
  int rscale[3];
  boolean_T notdone;
  int ilo;
  int ihi;
  double cfrom1;
  int jcol;
  double stemp_re;
  int exitg3;
  int i;
  double stemp_im;
  int j;
  int ii;
  int nzcount;
  creal_T atmp;
  boolean_T exitg4;
  int exitg2;
  signed char b_I[9];
  int jcolp1;
  int jrow;
  *info = 0;
  absxk = 2.2250738585072014E-308;
  b_sqrt(&absxk);
  SMLNUM = absxk / 2.2204460492503131E-16;
  BIGNUM = 1.0 / SMLNUM;
  anrm = 0.0;
  k = 0;
  exitg1 = false;
  while ((!exitg1) && (k < 9)) {
    absxk = rt_hypotd_snf(A[k].re, A[k].im);
    if (rtIsNaN(absxk)) {
      anrm = rtNaN;
      exitg1 = true;
    } else {
      if (absxk > anrm) {
        anrm = absxk;
      }

      k++;
    }
  }

  if (rtIsInf(anrm) || rtIsNaN(anrm)) {
    alpha1[0].re = rtNaN;
    alpha1[0].im = 0.0;
    beta1[0].re = rtNaN;
    beta1[0].im = 0.0;
    alpha1[1].re = rtNaN;
    alpha1[1].im = 0.0;
    beta1[1].re = rtNaN;
    beta1[1].im = 0.0;
    alpha1[2].re = rtNaN;
    alpha1[2].im = 0.0;
    beta1[2].re = rtNaN;
    beta1[2].im = 0.0;
    for (jcol = 0; jcol < 9; jcol++) {
      V[jcol].re = rtNaN;
      V[jcol].im = 0.0;
    }
  } else {
    ilascl = false;
    anrmto = anrm;
    if ((anrm > 0.0) && (anrm < SMLNUM)) {
      anrmto = SMLNUM;
      ilascl = true;
    } else {
      if (anrm > BIGNUM) {
        anrmto = BIGNUM;
        ilascl = true;
      }
    }

    if (ilascl) {
      absxk = anrm;
      BIGNUM = anrmto;
      notdone = true;
      while (notdone) {
        cfrom1 = absxk * 2.0041683600089728E-292;
        stemp_re = BIGNUM / 4.9896007738368E+291;
        if ((cfrom1 > fabs(BIGNUM)) && (BIGNUM != 0.0)) {
          stemp_im = 2.0041683600089728E-292;
          absxk = cfrom1;
        } else if (fabs(stemp_re) > absxk) {
          stemp_im = 4.9896007738368E+291;
          BIGNUM = stemp_re;
        } else {
          stemp_im = BIGNUM / absxk;
          notdone = false;
        }

        for (jcol = 0; jcol < 9; jcol++) {
          A[jcol].re *= stemp_im;
          A[jcol].im *= stemp_im;
        }
      }
    }

    rscale[0] = 1;
    rscale[1] = 1;
    rscale[2] = 1;
    ilo = 1;
    ihi = 3;
    do {
      exitg3 = 0;
      i = 0;
      j = -1;
      notdone = false;
      ii = ihi;
      exitg1 = false;
      while ((!exitg1) && (ii > 0)) {
        nzcount = 0;
        i = ii;
        j = ihi - 1;
        k = 0;
        exitg4 = false;
        while ((!exitg4) && (k <= ihi - 1)) {
          if ((A[(ii + 3 * k) - 1].re != 0.0) || (A[(ii + 3 * k) - 1].im != 0.0)
              || (ii == k + 1)) {
            if (nzcount == 0) {
              j = k;
              nzcount = 1;
              k++;
            } else {
              nzcount = 2;
              exitg4 = true;
            }
          } else {
            k++;
          }
        }

        if (nzcount < 2) {
          notdone = true;
          exitg1 = true;
        } else {
          ii--;
        }
      }

      if (!notdone) {
        exitg3 = 2;
      } else {
        if (i != ihi) {
          atmp = A[i - 1];
          A[i - 1] = A[ihi - 1];
          A[ihi - 1] = atmp;
          atmp = A[i + 2];
          A[i + 2] = A[ihi + 2];
          A[ihi + 2] = atmp;
          atmp = A[i + 5];
          A[i + 5] = A[ihi + 5];
          A[ihi + 5] = atmp;
        }

        if (j + 1 != ihi) {
          for (k = 0; k < ihi; k++) {
            ii = k + 3 * j;
            atmp = A[ii];
            jcol = k + 3 * (ihi - 1);
            A[ii] = A[jcol];
            A[jcol] = atmp;
          }
        }

        rscale[ihi - 1] = j + 1;
        ihi--;
        if (ihi == 1) {
          rscale[0] = 1;
          exitg3 = 1;
        }
      }
    } while (exitg3 == 0);

    if (exitg3 == 1) {
    } else {
      do {
        exitg2 = 0;
        i = 0;
        j = 0;
        notdone = false;
        k = ilo;
        exitg1 = false;
        while ((!exitg1) && (k <= ihi)) {
          nzcount = 0;
          i = ihi;
          j = k;
          ii = ilo;
          exitg4 = false;
          while ((!exitg4) && (ii <= ihi)) {
            if ((A[(ii + 3 * (k - 1)) - 1].re != 0.0) || (A[(ii + 3 * (k - 1)) -
                 1].im != 0.0) || (ii == k)) {
              if (nzcount == 0) {
                i = ii;
                nzcount = 1;
                ii++;
              } else {
                nzcount = 2;
                exitg4 = true;
              }
            } else {
              ii++;
            }
          }

          if (nzcount < 2) {
            notdone = true;
            exitg1 = true;
          } else {
            k++;
          }
        }

        if (!notdone) {
          exitg2 = 1;
        } else {
          if (i != ilo) {
            for (k = ilo; k < 4; k++) {
              ii = 3 * (k - 1);
              nzcount = (i + ii) - 1;
              atmp = A[nzcount];
              jcol = (ilo + ii) - 1;
              A[nzcount] = A[jcol];
              A[jcol] = atmp;
            }
          }

          if (j != ilo) {
            for (k = 0; k < ihi; k++) {
              ii = k + 3 * (j - 1);
              atmp = A[ii];
              jcol = k + 3 * (ilo - 1);
              A[ii] = A[jcol];
              A[jcol] = atmp;
            }
          }

          rscale[ilo - 1] = j;
          ilo++;
          if (ilo == ihi) {
            rscale[ilo - 1] = ilo;
            exitg2 = 1;
          }
        }
      } while (exitg2 == 0);
    }

    for (jcol = 0; jcol < 9; jcol++) {
      b_I[jcol] = 0;
    }

    b_I[0] = 1;
    b_I[4] = 1;
    b_I[8] = 1;
    for (jcol = 0; jcol < 9; jcol++) {
      V[jcol].re = b_I[jcol];
      V[jcol].im = 0.0;
    }

    if (ihi >= ilo + 2) {
      for (jcol = ilo - 1; jcol + 1 < ihi - 1; jcol++) {
        jcolp1 = jcol + 2;
        for (jrow = ihi - 1; jrow + 1 > jcol + 2; jrow--) {
          k = jrow + 3 * jcol;
          xzlartg(A[k - 1], A[k], &cfrom1, &atmp, &A[(jrow + 3 * jcol) - 1]);
          A[k].re = 0.0;
          A[k].im = 0.0;
          for (j = jcolp1; j < 4; j++) {
            nzcount = jrow + 3 * (j - 1);
            k = nzcount - 1;
            stemp_re = cfrom1 * A[k].re + (atmp.re * A[nzcount].re - atmp.im *
              A[jrow + 3 * (j - 1)].im);
            stemp_im = cfrom1 * A[(jrow + 3 * (j - 1)) - 1].im + (atmp.re *
              A[jrow + 3 * (j - 1)].im + atmp.im * A[jrow + 3 * (j - 1)].re);
            absxk = A[(jrow + 3 * (j - 1)) - 1].im;
            BIGNUM = A[(jrow + 3 * (j - 1)) - 1].re;
            A[nzcount].re = cfrom1 * A[jrow + 3 * (j - 1)].re - (atmp.re * A
              [(jrow + 3 * (j - 1)) - 1].re + atmp.im * A[(jrow + 3 * (j - 1)) -
              1].im);
            A[nzcount].im = cfrom1 * A[jrow + 3 * (j - 1)].im - (atmp.re * absxk
              - atmp.im * BIGNUM);
            A[k].re = stemp_re;
            A[k].im = stemp_im;
          }

          atmp.re = -atmp.re;
          atmp.im = -atmp.im;
          for (i = 1; i <= ihi; i++) {
            nzcount = (i + 3 * (jrow - 1)) - 1;
            k = (i + 3 * jrow) - 1;
            stemp_re = cfrom1 * A[k].re + (atmp.re * A[nzcount].re - atmp.im *
              A[(i + 3 * (jrow - 1)) - 1].im);
            stemp_im = cfrom1 * A[(i + 3 * jrow) - 1].im + (atmp.re * A[(i + 3 *
              (jrow - 1)) - 1].im + atmp.im * A[(i + 3 * (jrow - 1)) - 1].re);
            absxk = A[(i + 3 * jrow) - 1].im;
            BIGNUM = A[(i + 3 * jrow) - 1].re;
            A[nzcount].re = cfrom1 * A[(i + 3 * (jrow - 1)) - 1].re - (atmp.re *
              A[(i + 3 * jrow) - 1].re + atmp.im * A[(i + 3 * jrow) - 1].im);
            A[nzcount].im = cfrom1 * A[(i + 3 * (jrow - 1)) - 1].im - (atmp.re *
              absxk - atmp.im * BIGNUM);
            A[k].re = stemp_re;
            A[k].im = stemp_im;
          }

          nzcount = 3 * (jrow - 1);
          stemp_re = cfrom1 * V[3 * jrow].re + (atmp.re * V[nzcount].re -
            atmp.im * V[3 * (jrow - 1)].im);
          stemp_im = cfrom1 * V[3 * jrow].im + (atmp.re * V[nzcount].im +
            atmp.im * V[3 * (jrow - 1)].re);
          absxk = V[3 * jrow].re;
          V[nzcount].re = cfrom1 * V[nzcount].re - (atmp.re * V[3 * jrow].re +
            atmp.im * V[3 * jrow].im);
          V[nzcount].im = cfrom1 * V[nzcount].im - (atmp.re * V[3 * jrow].im -
            atmp.im * absxk);
          V[3 * jrow].re = stemp_re;
          V[3 * jrow].im = stemp_im;
          ii = 1 + nzcount;
          k = 1 + 3 * jrow;
          stemp_re = cfrom1 * V[k].re + (atmp.re * V[ii].re - atmp.im * V[1 + 3 *
            (jrow - 1)].im);
          stemp_im = cfrom1 * V[1 + 3 * jrow].im + (atmp.re * V[1 + 3 * (jrow -
            1)].im + atmp.im * V[1 + 3 * (jrow - 1)].re);
          absxk = V[1 + 3 * jrow].re;
          V[ii].re = cfrom1 * V[1 + 3 * (jrow - 1)].re - (atmp.re * V[1 + 3 *
            jrow].re + atmp.im * V[1 + 3 * jrow].im);
          V[ii].im = cfrom1 * V[ii].im - (atmp.re * V[1 + 3 * jrow].im - atmp.im
            * absxk);
          V[k].re = stemp_re;
          V[k].im = stemp_im;
          nzcount += 2;
          k = 2 + 3 * jrow;
          stemp_re = cfrom1 * V[k].re + (atmp.re * V[nzcount].re - atmp.im * V[2
            + 3 * (jrow - 1)].im);
          stemp_im = cfrom1 * V[2 + 3 * jrow].im + (atmp.re * V[2 + 3 * (jrow -
            1)].im + atmp.im * V[2 + 3 * (jrow - 1)].re);
          absxk = V[2 + 3 * jrow].re;
          V[nzcount].re = cfrom1 * V[2 + 3 * (jrow - 1)].re - (atmp.re * V[2 + 3
            * jrow].re + atmp.im * V[2 + 3 * jrow].im);
          V[nzcount].im = cfrom1 * V[nzcount].im - (atmp.re * V[2 + 3 * jrow].im
            - atmp.im * absxk);
          V[k].re = stemp_re;
          V[k].im = stemp_im;
        }
      }
    }

    xzhgeqz(A, ilo, ihi, V, info, alpha1, beta1);
    if (*info == 0) {
      xztgevc(A, V);
      if (ilo > 1) {
        for (i = ilo - 2; i + 1 >= 1; i--) {
          k = rscale[i] - 1;
          if (rscale[i] != i + 1) {
            atmp = V[i];
            V[i] = V[k];
            V[k] = atmp;
            atmp = V[i + 3];
            V[i + 3] = V[k + 3];
            V[k + 3] = atmp;
            atmp = V[i + 6];
            V[i + 6] = V[k + 6];
            V[k + 6] = atmp;
          }
        }
      }

      if (ihi < 3) {
        jcol = ihi + 1;
        for (i = jcol; i < 4; i++) {
          k = rscale[i - 1];
          if (k != i) {
            atmp = V[i - 1];
            V[i - 1] = V[k - 1];
            V[k - 1] = atmp;
            atmp = V[i + 2];
            V[i + 2] = V[k + 2];
            V[k + 2] = atmp;
            atmp = V[i + 5];
            V[i + 5] = V[k + 5];
            V[k + 5] = atmp;
          }
        }
      }

      for (k = 0; k < 3; k++) {
        absxk = fabs(V[3 * k].re) + fabs(V[3 * k].im);
        ii = 3 * k + 1;
        BIGNUM = fabs(V[ii].re) + fabs(V[3 * k + 1].im);
        if (BIGNUM > absxk) {
          absxk = BIGNUM;
        }

        nzcount = 3 * k + 2;
        BIGNUM = fabs(V[nzcount].re) + fabs(V[nzcount].im);
        if (BIGNUM > absxk) {
          absxk = BIGNUM;
        }

        if (absxk >= SMLNUM) {
          absxk = 1.0 / absxk;
          V[3 * k].re *= absxk;
          V[3 * k].im *= absxk;
          V[ii].re *= absxk;
          V[ii].im *= absxk;
          V[nzcount].re *= absxk;
          V[nzcount].im *= absxk;
        }
      }

      if (ilascl) {
        notdone = true;
        while (notdone) {
          cfrom1 = anrmto * 2.0041683600089728E-292;
          stemp_re = anrm / 4.9896007738368E+291;
          if ((fabs(cfrom1) > anrm) && (anrm != 0.0)) {
            stemp_im = 2.0041683600089728E-292;
            anrmto = cfrom1;
          } else if (stemp_re > fabs(anrmto)) {
            stemp_im = 4.9896007738368E+291;
            anrm = stemp_re;
          } else {
            stemp_im = anrm / anrmto;
            notdone = false;
          }

          alpha1[0].re *= stemp_im;
          alpha1[0].im *= stemp_im;
          alpha1[1].re *= stemp_im;
          alpha1[1].im *= stemp_im;
          alpha1[2].re *= stemp_im;
          alpha1[2].im *= stemp_im;
        }
      }
    }
  }
}

/*
 * File trailer for xzggev.c
 *
 * [EOF]
 */
