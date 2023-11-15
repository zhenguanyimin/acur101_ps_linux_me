/*
 * File: xzhgeqz.c
 *
 * MATLAB Coder version            : 4.2
 * C/C++ source code generated on  : 15-Jan-2023 14:17:59
 */

/* Include Files */
#include <math.h>
#include "rt_nonfinite.h"
#include "magCalibration4.h"
#include "xzhgeqz.h"
#include "xzlartg.h"
#include "inv.h"
#include "sqrt.h"
#include "mod.h"
#include "xzlanhs.h"
#include "magCalibration4_rtwutil.h"

/* Function Definitions */

/*
 * Arguments    : creal_T A[9]
 *                int ilo
 *                int ihi
 *                creal_T Z[9]
 *                int *info
 *                creal_T alpha1[3]
 *                creal_T beta1[3]
 * Return Type  : void
 */
void xzhgeqz(creal_T A[9], int ilo, int ihi, creal_T Z[9], int *info, creal_T
             alpha1[3], creal_T beta1[3])
{
  double eshift_re;
  double eshift_im;
  double ctemp_re;
  double ctemp_im;
  double anorm;
  double tempr;
  double b_atol;
  double temp2;
  double ascale;
  boolean_T failed;
  int jp1;
  int j;
  boolean_T guard1 = false;
  boolean_T guard2 = false;
  int ifirst;
  int istart;
  int ilast;
  int ilastm1;
  int iiter;
  boolean_T goto60;
  boolean_T goto70;
  boolean_T goto90;
  int jiter;
  int exitg1;
  boolean_T b_guard1 = false;
  boolean_T guard3 = false;
  boolean_T exitg2;
  double stemp_re;
  double stemp_im;
  double t1_re;
  double ascale_re;
  creal_T shift;
  double t1_im;
  double g2;
  double c;
  int A_tmp;
  double shift_im;
  int shift_re_tmp;
  int stemp_re_tmp;
  *info = 0;
  alpha1[0].re = 0.0;
  alpha1[0].im = 0.0;
  beta1[0].re = 1.0;
  beta1[0].im = 0.0;
  alpha1[1].re = 0.0;
  alpha1[1].im = 0.0;
  beta1[1].re = 1.0;
  beta1[1].im = 0.0;
  alpha1[2].re = 0.0;
  alpha1[2].im = 0.0;
  beta1[2].re = 1.0;
  beta1[2].im = 0.0;
  eshift_re = 0.0;
  eshift_im = 0.0;
  ctemp_re = 0.0;
  ctemp_im = 0.0;
  anorm = xzlanhs(A, ilo, ihi);
  tempr = 2.2204460492503131E-16 * anorm;
  b_atol = 2.2250738585072014E-308;
  if (tempr > 2.2250738585072014E-308) {
    b_atol = tempr;
  }

  temp2 = 2.2250738585072014E-308;
  if (anorm > 2.2250738585072014E-308) {
    temp2 = anorm;
  }

  ascale = 1.0 / temp2;
  failed = true;
  jp1 = ihi + 1;
  for (j = jp1; j < 4; j++) {
    alpha1[j - 1] = A[(j + 3 * (j - 1)) - 1];
  }

  guard1 = false;
  guard2 = false;
  if (ihi >= ilo) {
    ifirst = ilo;
    istart = ilo;
    ilast = ihi - 1;
    ilastm1 = ihi - 2;
    iiter = 0;
    goto60 = false;
    goto70 = false;
    goto90 = false;
    jiter = 0;
    do {
      exitg1 = 0;
      if (jiter <= 30 * ((ihi - ilo) + 1) - 1) {
        b_guard1 = false;
        if (ilast + 1 == ilo) {
          goto60 = true;
          b_guard1 = true;
        } else {
          jp1 = ilast + 3 * ilastm1;
          if (fabs(A[jp1].re) + fabs(A[ilast + 3 * ilastm1].im) <= b_atol) {
            A[jp1].re = 0.0;
            A[jp1].im = 0.0;
            goto60 = true;
            b_guard1 = true;
          } else {
            j = ilastm1;
            guard3 = false;
            exitg2 = false;
            while ((!exitg2) && (j + 1 >= ilo)) {
              if (j + 1 == ilo) {
                guard3 = true;
                exitg2 = true;
              } else {
                jp1 = j + 3 * (j - 1);
                if (fabs(A[jp1].re) + fabs(A[j + 3 * (j - 1)].im) <= b_atol) {
                  A[jp1].re = 0.0;
                  A[jp1].im = 0.0;
                  guard3 = true;
                  exitg2 = true;
                } else {
                  j--;
                  guard3 = false;
                }
              }
            }

            if (guard3) {
              ifirst = j + 1;
              goto70 = true;
            }

            if (goto70) {
              b_guard1 = true;
            } else {
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
              for (jp1 = 0; jp1 < 9; jp1++) {
                Z[jp1].re = rtNaN;
                Z[jp1].im = 0.0;
              }

              *info = 1;
              exitg1 = 1;
            }
          }
        }

        if (b_guard1) {
          if (goto60) {
            goto60 = false;
            alpha1[ilast] = A[ilast + 3 * ilast];
            ilast = ilastm1;
            ilastm1--;
            if (ilast + 1 < ilo) {
              failed = false;
              guard2 = true;
              exitg1 = 1;
            } else {
              iiter = 0;
              eshift_re = 0.0;
              eshift_im = 0.0;
              jiter++;
            }
          } else {
            if (goto70) {
              goto70 = false;
              iiter++;
              if (b_mod(iiter) != 0) {
                anorm = ascale * A[ilastm1 + 3 * ilastm1].re;
                temp2 = ascale * A[ilastm1 + 3 * ilastm1].im;
                if (temp2 == 0.0) {
                  shift.re = anorm / 0.57735026918962584;
                  shift.im = 0.0;
                } else if (anorm == 0.0) {
                  shift.re = 0.0;
                  shift.im = temp2 / 0.57735026918962584;
                } else {
                  shift.re = anorm / 0.57735026918962584;
                  shift.im = temp2 / 0.57735026918962584;
                }

                anorm = ascale * A[ilast + 3 * ilast].re;
                temp2 = ascale * A[ilast + 3 * ilast].im;
                if (temp2 == 0.0) {
                  stemp_re = anorm / 0.57735026918962584;
                  stemp_im = 0.0;
                } else if (anorm == 0.0) {
                  stemp_re = 0.0;
                  stemp_im = temp2 / 0.57735026918962584;
                } else {
                  stemp_re = anorm / 0.57735026918962584;
                  stemp_im = temp2 / 0.57735026918962584;
                }

                t1_re = 0.5 * (shift.re + stemp_re);
                t1_im = 0.5 * (shift.im + stemp_im);
                c = t1_re * t1_im;
                anorm = ascale * A[ilastm1 + 3 * ilast].re;
                temp2 = ascale * A[ilastm1 + 3 * ilast].im;
                if (temp2 == 0.0) {
                  ascale_re = anorm / 0.57735026918962584;
                  g2 = 0.0;
                } else if (anorm == 0.0) {
                  ascale_re = 0.0;
                  g2 = temp2 / 0.57735026918962584;
                } else {
                  ascale_re = anorm / 0.57735026918962584;
                  g2 = temp2 / 0.57735026918962584;
                }

                anorm = ascale * A[ilast + 3 * ilastm1].re;
                temp2 = ascale * A[ilast + 3 * ilastm1].im;
                if (temp2 == 0.0) {
                  tempr = anorm / 0.57735026918962584;
                  anorm = 0.0;
                } else if (anorm == 0.0) {
                  tempr = 0.0;
                  anorm = temp2 / 0.57735026918962584;
                } else {
                  tempr = anorm / 0.57735026918962584;
                  anorm = temp2 / 0.57735026918962584;
                }

                temp2 = shift.re * stemp_re - shift.im * stemp_im;
                shift_im = shift.re * stemp_im + shift.im * stemp_re;
                shift.re = ((t1_re * t1_re - t1_im * t1_im) + (ascale_re * tempr
                  - g2 * anorm)) - temp2;
                shift.im = ((c + c) + (ascale_re * anorm + g2 * tempr)) -
                  shift_im;
                c_sqrt(&shift);
                if ((t1_re - stemp_re) * shift.re + (t1_im - stemp_im) *
                    shift.im <= 0.0) {
                  shift.re += t1_re;
                  shift.im += t1_im;
                } else {
                  shift.re = t1_re - shift.re;
                  shift.im = t1_im - shift.im;
                }
              } else {
                anorm = ascale * A[ilast + 3 * ilastm1].re;
                temp2 = ascale * A[ilast + 3 * ilastm1].im;
                if (temp2 == 0.0) {
                  ascale_re = anorm / 0.57735026918962584;
                  g2 = 0.0;
                } else if (anorm == 0.0) {
                  ascale_re = 0.0;
                  g2 = temp2 / 0.57735026918962584;
                } else {
                  ascale_re = anorm / 0.57735026918962584;
                  g2 = temp2 / 0.57735026918962584;
                }

                eshift_re += ascale_re;
                eshift_im += g2;
                shift.re = eshift_re;
                shift.im = eshift_im;
              }

              j = ilastm1;
              jp1 = ilastm1 + 1;
              exitg2 = false;
              while ((!exitg2) && (j + 1 > ifirst)) {
                istart = j + 1;
                ctemp_re = ascale * A[j + 3 * j].re - shift.re *
                  0.57735026918962584;
                ctemp_im = ascale * A[j + 3 * j].im - shift.im *
                  0.57735026918962584;
                anorm = fabs(ctemp_re) + fabs(ctemp_im);
                temp2 = ascale * (fabs(A[jp1 + 3 * j].re) + fabs(A[jp1 + 3 * j].
                  im));
                tempr = anorm;
                if (temp2 > anorm) {
                  tempr = temp2;
                }

                if ((tempr < 1.0) && (tempr != 0.0)) {
                  anorm /= tempr;
                  temp2 /= tempr;
                }

                if ((fabs(A[j + 3 * (j - 1)].re) + fabs(A[j + 3 * (j - 1)].im)) *
                    temp2 <= anorm * b_atol) {
                  goto90 = true;
                  exitg2 = true;
                } else {
                  jp1 = j;
                  j--;
                }
              }

              if (!goto90) {
                istart = ifirst;
                ctemp_re = ascale * A[(ifirst + 3 * (ifirst - 1)) - 1].re -
                  shift.re * 0.57735026918962584;
                ctemp_im = ascale * A[(ifirst + 3 * (ifirst - 1)) - 1].im -
                  shift.im * 0.57735026918962584;
                goto90 = true;
              }
            }

            if (goto90) {
              goto90 = false;
              stemp_re = ascale * A[istart + 3 * (istart - 1)].re;
              stemp_im = ascale * A[istart + 3 * (istart - 1)].im;
              t1_re = fabs(ctemp_re);
              temp2 = t1_re;
              t1_im = fabs(ctemp_im);
              if (t1_im > t1_re) {
                temp2 = t1_im;
              }

              tempr = fabs(stemp_re);
              anorm = fabs(stemp_im);
              if (anorm > tempr) {
                tempr = anorm;
              }

              if (tempr > temp2) {
                temp2 = tempr;
              }

              shift.re = ctemp_re;
              shift.im = ctemp_im;
              guard3 = false;
              if (temp2 >= 7.4428285367870146E+137) {
                do {
                  shift.re *= 1.3435752215134178E-138;
                  shift.im *= 1.3435752215134178E-138;
                  stemp_re *= 1.3435752215134178E-138;
                  stemp_im *= 1.3435752215134178E-138;
                  temp2 *= 1.3435752215134178E-138;
                } while (!(temp2 < 7.4428285367870146E+137));

                guard3 = true;
              } else if (temp2 <= 1.3435752215134178E-138) {
                if ((stemp_re == 0.0) && (stemp_im == 0.0)) {
                  c = 1.0;
                  shift.re = 0.0;
                  shift.im = 0.0;
                } else {
                  do {
                    shift.re *= 7.4428285367870146E+137;
                    shift.im *= 7.4428285367870146E+137;
                    stemp_re *= 7.4428285367870146E+137;
                    stemp_im *= 7.4428285367870146E+137;
                    temp2 *= 7.4428285367870146E+137;
                  } while (!(temp2 > 1.3435752215134178E-138));

                  guard3 = true;
                }
              } else {
                guard3 = true;
              }

              if (guard3) {
                tempr = shift.re * shift.re + shift.im * shift.im;
                g2 = stemp_re * stemp_re + stemp_im * stemp_im;
                temp2 = g2;
                if (1.0 > g2) {
                  temp2 = 1.0;
                }

                if (tempr <= temp2 * 2.0041683600089728E-292) {
                  if ((ctemp_re == 0.0) && (ctemp_im == 0.0)) {
                    c = 0.0;
                    g2 = rt_hypotd_snf(stemp_re, stemp_im);
                    shift.re = stemp_re / g2;
                    shift.im = -stemp_im / g2;
                  } else {
                    tempr = sqrt(g2);
                    c = rt_hypotd_snf(shift.re, shift.im) / tempr;
                    if (t1_im > t1_re) {
                      t1_re = t1_im;
                    }

                    if (t1_re > 1.0) {
                      g2 = rt_hypotd_snf(ctemp_re, ctemp_im);
                      shift.re = ctemp_re / g2;
                      shift.im = ctemp_im / g2;
                    } else {
                      anorm = 7.4428285367870146E+137 * ctemp_re;
                      temp2 = 7.4428285367870146E+137 * ctemp_im;
                      g2 = rt_hypotd_snf(anorm, temp2);
                      shift.re = anorm / g2;
                      shift.im = temp2 / g2;
                    }

                    stemp_re /= tempr;
                    stemp_im = -stemp_im / tempr;
                    temp2 = shift.re;
                    shift_im = shift.im;
                    shift.re = temp2 * stemp_re - shift_im * stemp_im;
                    shift.im = temp2 * stemp_im + shift_im * stemp_re;
                  }
                } else {
                  anorm = sqrt(1.0 + g2 / tempr);
                  temp2 = shift.re;
                  shift_im = shift.im;
                  c = 1.0 / anorm;
                  g2 += tempr;
                  temp2 = anorm * temp2 / g2;
                  shift_im = anorm * shift_im / g2;
                  shift.re = temp2 * stemp_re - shift_im * -stemp_im;
                  shift.im = temp2 * -stemp_im + shift_im * stemp_re;
                }
              }

              j = istart;
              jp1 = istart - 2;
              while (j < ilast + 1) {
                if (j > istart) {
                  A_tmp = j + 3 * jp1;
                  xzlartg(A[A_tmp - 1], A[A_tmp], &c, &shift, &A[(j + 3 * jp1) -
                          1]);
                  A[A_tmp].re = 0.0;
                  A[A_tmp].im = 0.0;
                }

                for (jp1 = j; jp1 < 4; jp1++) {
                  shift_re_tmp = j + 3 * (jp1 - 1);
                  stemp_re_tmp = shift_re_tmp - 1;
                  stemp_re = c * A[stemp_re_tmp].re + (shift.re * A[shift_re_tmp]
                    .re - shift.im * A[j + 3 * (jp1 - 1)].im);
                  stemp_im = c * A[(j + 3 * (jp1 - 1)) - 1].im + (shift.re * A[j
                    + 3 * (jp1 - 1)].im + shift.im * A[j + 3 * (jp1 - 1)].re);
                  anorm = A[(j + 3 * (jp1 - 1)) - 1].im;
                  temp2 = A[(j + 3 * (jp1 - 1)) - 1].re;
                  A[shift_re_tmp].re = c * A[j + 3 * (jp1 - 1)].re - (shift.re *
                    A[(j + 3 * (jp1 - 1)) - 1].re + shift.im * A[(j + 3 * (jp1 -
                    1)) - 1].im);
                  A[shift_re_tmp].im = c * A[j + 3 * (jp1 - 1)].im - (shift.re *
                    anorm - shift.im * temp2);
                  A[stemp_re_tmp].re = stemp_re;
                  A[stemp_re_tmp].im = stemp_im;
                }

                shift.re = -shift.re;
                shift.im = -shift.im;
                jp1 = j;
                if (ilast + 1 < j + 2) {
                  jp1 = ilast - 1;
                }

                for (A_tmp = 1; A_tmp <= jp1 + 2; A_tmp++) {
                  shift_re_tmp = (A_tmp + 3 * (j - 1)) - 1;
                  stemp_re_tmp = (A_tmp + 3 * j) - 1;
                  stemp_re = c * A[stemp_re_tmp].re + (shift.re * A[shift_re_tmp]
                    .re - shift.im * A[(A_tmp + 3 * (j - 1)) - 1].im);
                  stemp_im = c * A[(A_tmp + 3 * j) - 1].im + (shift.re * A
                    [(A_tmp + 3 * (j - 1)) - 1].im + shift.im * A[(A_tmp + 3 *
                    (j - 1)) - 1].re);
                  anorm = A[(A_tmp + 3 * j) - 1].im;
                  temp2 = A[(A_tmp + 3 * j) - 1].re;
                  A[shift_re_tmp].re = c * A[(A_tmp + 3 * (j - 1)) - 1].re -
                    (shift.re * A[(A_tmp + 3 * j) - 1].re + shift.im * A[(A_tmp
                      + 3 * j) - 1].im);
                  A[shift_re_tmp].im = c * A[(A_tmp + 3 * (j - 1)) - 1].im -
                    (shift.re * anorm - shift.im * temp2);
                  A[stemp_re_tmp].re = stemp_re;
                  A[stemp_re_tmp].im = stemp_im;
                }

                shift_re_tmp = 3 * (j - 1);
                stemp_re = c * Z[3 * j].re + (shift.re * Z[shift_re_tmp].re -
                  shift.im * Z[3 * (j - 1)].im);
                stemp_im = c * Z[3 * j].im + (shift.re * Z[3 * (j - 1)].im +
                  shift.im * Z[3 * (j - 1)].re);
                anorm = Z[3 * j].im;
                temp2 = Z[3 * j].re;
                Z[shift_re_tmp].re = c * Z[3 * (j - 1)].re - (shift.re * Z[3 * j]
                  .re + shift.im * Z[3 * j].im);
                Z[shift_re_tmp].im = c * Z[3 * (j - 1)].im - (shift.re * anorm -
                  shift.im * temp2);
                Z[3 * j].re = stemp_re;
                Z[3 * j].im = stemp_im;
                jp1 = 1 + shift_re_tmp;
                stemp_re_tmp = 1 + 3 * j;
                stemp_re = c * Z[stemp_re_tmp].re + (shift.re * Z[jp1].re -
                  shift.im * Z[1 + 3 * (j - 1)].im);
                stemp_im = c * Z[1 + 3 * j].im + (shift.re * Z[1 + 3 * (j - 1)].
                  im + shift.im * Z[1 + 3 * (j - 1)].re);
                anorm = Z[1 + 3 * j].im;
                temp2 = Z[1 + 3 * j].re;
                Z[jp1].re = c * Z[1 + 3 * (j - 1)].re - (shift.re * Z[1 + 3 * j]
                  .re + shift.im * Z[1 + 3 * j].im);
                Z[jp1].im = c * Z[1 + 3 * (j - 1)].im - (shift.re * anorm -
                  shift.im * temp2);
                Z[stemp_re_tmp].re = stemp_re;
                Z[stemp_re_tmp].im = stemp_im;
                shift_re_tmp += 2;
                stemp_re_tmp = 2 + 3 * j;
                stemp_re = c * Z[stemp_re_tmp].re + (shift.re * Z[shift_re_tmp].
                  re - shift.im * Z[2 + 3 * (j - 1)].im);
                stemp_im = c * Z[2 + 3 * j].im + (shift.re * Z[2 + 3 * (j - 1)].
                  im + shift.im * Z[2 + 3 * (j - 1)].re);
                anorm = Z[2 + 3 * j].im;
                temp2 = Z[2 + 3 * j].re;
                Z[shift_re_tmp].re = c * Z[2 + 3 * (j - 1)].re - (shift.re * Z[2
                  + 3 * j].re + shift.im * Z[2 + 3 * j].im);
                Z[shift_re_tmp].im = c * Z[2 + 3 * (j - 1)].im - (shift.re *
                  anorm - shift.im * temp2);
                Z[stemp_re_tmp].re = stemp_re;
                Z[stemp_re_tmp].im = stemp_im;
                jp1 = j - 1;
                j++;
              }
            }

            jiter++;
          }
        }
      } else {
        guard2 = true;
        exitg1 = 1;
      }
    } while (exitg1 == 0);
  } else {
    guard1 = true;
  }

  if (guard2) {
    if (failed) {
      *info = ilast + 1;
      for (jp1 = 0; jp1 <= ilast; jp1++) {
        alpha1[jp1].re = rtNaN;
        alpha1[jp1].im = 0.0;
        beta1[jp1].re = rtNaN;
        beta1[jp1].im = 0.0;
      }

      for (jp1 = 0; jp1 < 9; jp1++) {
        Z[jp1].re = rtNaN;
        Z[jp1].im = 0.0;
      }
    } else {
      guard1 = true;
    }
  }

  if (guard1) {
    for (j = 0; j <= ilo - 2; j++) {
      alpha1[j] = A[j + 3 * j];
    }
  }
}

/*
 * File trailer for xzhgeqz.c
 *
 * [EOF]
 */
