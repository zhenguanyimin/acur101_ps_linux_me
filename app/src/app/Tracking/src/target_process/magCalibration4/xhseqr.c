/*
 * File: xhseqr.c
 *
 * MATLAB Coder version            : 4.2
 * C/C++ source code generated on  : 15-Jan-2023 14:17:59
 */

/* Include Files */
#include <math.h>
#include "rt_nonfinite.h"
#include "magCalibration4.h"
#include "xhseqr.h"
#include "xdlanv2.h"
#include "xzlarfg.h"
#include "sqrt.h"

/* Function Definitions */

/*
 * Arguments    : double h[9]
 *                double z[9]
 * Return Type  : int
 */
int xhseqr(double h[9], double z[9])
{
  int info;
  double v[3];
  int i;
  boolean_T exitg1;
  int L;
  boolean_T goto150;
  int its;
  boolean_T exitg2;
  int k;
  boolean_T exitg3;
  int i13;
  int nr;
  double d2;
  int m;
  int ix;
  double s;
  double tst;
  double htmp1;
  double aa;
  double ab;
  double ba;
  double h22;
  double rt1r;
  int iy;
  double d3;
  int j;
  info = 0;
  v[0] = 0.0;
  v[1] = 0.0;
  v[2] = 0.0;
  h[2] = 0.0;
  i = 2;
  exitg1 = false;
  while ((!exitg1) && (i + 1 >= 1)) {
    L = 1;
    goto150 = false;
    its = 0;
    exitg2 = false;
    while ((!exitg2) && (its < 301)) {
      k = i;
      exitg3 = false;
      while ((!exitg3) && (k + 1 > L)) {
        i13 = k + 3 * (k - 1);
        d2 = fabs(h[i13]);
        if (d2 <= 3.0062525400134592E-292) {
          exitg3 = true;
        } else {
          nr = k + 3 * k;
          ix = i13 - 1;
          tst = fabs(h[ix]) + fabs(h[nr]);
          if (tst == 0.0) {
            if (k - 1 >= 1) {
              tst = fabs(h[(k + 3 * (k - 2)) - 1]);
            }

            if (k + 2 <= 3) {
              tst += fabs(h[nr + 1]);
            }
          }

          if (d2 <= 2.2204460492503131E-16 * tst) {
            htmp1 = fabs(h[i13]);
            tst = fabs(h[nr - 1]);
            if (htmp1 > tst) {
              ab = htmp1;
              ba = tst;
            } else {
              ab = tst;
              ba = htmp1;
            }

            htmp1 = fabs(h[nr]);
            tst = fabs(h[ix] - h[k + 3 * k]);
            if (htmp1 > tst) {
              aa = htmp1;
              htmp1 = tst;
            } else {
              aa = tst;
            }

            s = aa + ab;
            tst = 2.2204460492503131E-16 * (htmp1 * (aa / s));
            if ((3.0062525400134592E-292 > tst) || rtIsNaN(tst)) {
              d3 = 3.0062525400134592E-292;
            } else {
              d3 = tst;
            }

            if (ba * (ab / s) <= d3) {
              exitg3 = true;
            } else {
              k--;
            }
          } else {
            k--;
          }
        }
      }

      L = k + 1;
      if (k + 1 > 1) {
        h[k + 3 * (k - 1)] = 0.0;
      }

      if (k + 1 >= i) {
        goto150 = true;
        exitg2 = true;
      } else {
        if (its == 10) {
          s = fabs(h[1]) + fabs(h[5]);
          tst = 0.75 * s + h[0];
          aa = -0.4375 * s;
          htmp1 = s;
          h22 = tst;
        } else if (its == 20) {
          s = fabs(h[i + 3 * (i - 1)]) + fabs(h[(i + 3 * (i - 2)) - 1]);
          tst = 0.75 * s + h[i + 3 * i];
          aa = -0.4375 * s;
          htmp1 = s;
          h22 = tst;
        } else {
          tst = h[(i + 3 * (i - 1)) - 1];
          htmp1 = h[i + 3 * (i - 1)];
          aa = h[(i + 3 * i) - 1];
          h22 = h[i + 3 * i];
        }

        s = ((fabs(tst) + fabs(aa)) + fabs(htmp1)) + fabs(h22);
        if (s == 0.0) {
          rt1r = 0.0;
          tst = 0.0;
          ba = 0.0;
          htmp1 = 0.0;
        } else {
          tst /= s;
          htmp1 /= s;
          aa /= s;
          h22 /= s;
          ab = (tst + h22) / 2.0;
          tst = (tst - ab) * (h22 - ab) - aa * htmp1;
          htmp1 = fabs(tst);
          b_sqrt(&htmp1);
          if (tst >= 0.0) {
            rt1r = ab * s;
            ba = rt1r;
            tst = htmp1 * s;
            htmp1 = -tst;
          } else {
            rt1r = ab + htmp1;
            ba = ab - htmp1;
            if (fabs(rt1r - h22) <= fabs(ba - h22)) {
              rt1r *= s;
              ba = rt1r;
            } else {
              ba *= s;
              rt1r = ba;
            }

            tst = 0.0;
            htmp1 = 0.0;
          }
        }

        m = i - 1;
        if (i - 1 >= 1) {
          aa = h[0] - ba;
          s = (fabs(aa) + fabs(htmp1)) + fabs(h[1]);
          ab = h[1] / s;
          v[0] = (ab * h[3] + (h[0] - rt1r) * (aa / s)) - tst * (htmp1 / s);
          v[1] = ab * (((h[0] + h[4]) - rt1r) - ba);
          v[2] = ab * h[5];
          s = (fabs(v[0]) + fabs(v[1])) + fabs(v[2]);
          v[0] /= s;
          v[1] /= s;
          v[2] /= s;
        }

        for (k = m; k <= i; k++) {
          nr = (i - k) + 2;
          if (3 < nr) {
            nr = 3;
          }

          if (k > i - 1) {
            ix = (k + 3 * (k - 2)) - 1;
            for (j = 0; j < nr; j++) {
              v[j] = h[j + ix];
            }
          }

          tst = v[0];
          ab = xzlarfg(nr, &tst, v);
          v[0] = tst;
          if (k > i - 1) {
            h[(k + 3 * (k - 2)) - 1] = tst;
            h[k + 3 * (k - 2)] = 0.0;
            if (k < i) {
              h[(k + 3 * (k - 2)) + 1] = 0.0;
            }
          }

          d2 = v[1];
          htmp1 = ab * v[1];
          if (nr == 3) {
            s = v[2];
            tst = ab * v[2];
            for (j = k; j < 4; j++) {
              nr = k + 3 * (j - 1);
              ix = nr - 1;
              iy = nr + 1;
              aa = (h[ix] + d2 * h[nr]) + s * h[iy];
              h[ix] -= aa * ab;
              h[nr] -= aa * htmp1;
              h[iy] -= aa * tst;
            }

            if (k + 3 < i + 1) {
              i13 = k + 2;
            } else {
              i13 = i;
            }

            for (j = 0; j <= i13; j++) {
              nr = j + 3 * (k - 1);
              ix = j + 3 * k;
              iy = j + 3 * (k + 1);
              aa = (h[nr] + d2 * h[ix]) + s * h[iy];
              h[nr] -= aa * ab;
              h[ix] -= aa * htmp1;
              h[iy] -= aa * tst;
            }

            for (j = 0; j < 3; j++) {
              nr = j + 3 * (k - 1);
              ix = j + 3 * k;
              iy = j + 3 * (k + 1);
              aa = (z[nr] + d2 * z[ix]) + s * z[iy];
              z[nr] -= aa * ab;
              z[ix] -= aa * htmp1;
              z[iy] -= aa * tst;
            }
          } else {
            if (nr == 2) {
              for (j = k; j < 4; j++) {
                nr = k + 3 * (j - 1);
                ix = nr - 1;
                tst = h[ix];
                aa = tst + d2 * h[nr];
                h[ix] = tst - aa * ab;
                h[nr] -= aa * htmp1;
              }

              for (j = 0; j <= i; j++) {
                nr = j + 3 * (k - 1);
                ix = j + 3 * k;
                aa = h[nr] + d2 * h[ix];
                h[nr] -= aa * ab;
                h[ix] -= aa * htmp1;
              }

              for (j = 0; j < 3; j++) {
                ix = j + 3 * (k - 1);
                tst = z[ix];
                nr = j + 3 * k;
                aa = tst + d2 * z[nr];
                z[ix] = tst - aa * ab;
                z[nr] -= aa * htmp1;
              }
            }
          }
        }

        its++;
      }
    }

    if (!goto150) {
      info = i + 1;
      exitg1 = true;
    } else {
      if ((L != i + 1) && (L == i)) {
        i13 = i + 3 * i;
        nr = i13 - 1;
        d2 = h[nr];
        m = 3 * (i - 1);
        ix = i + m;
        s = h[ix];
        tst = h[i13];
        xdlanv2(&h[(i + 3 * (i - 1)) - 1], &d2, &s, &tst, &htmp1, &aa, &ab, &ba,
                &h22, &rt1r);
        h[nr] = d2;
        h[ix] = s;
        h[i13] = tst;
        if (3 > i + 1) {
          nr = 1 - i;
          iy = i + (i + 1) * 3;
          ix = iy - 1;
          for (k = 0; k <= nr; k++) {
            tst = h22 * h[ix] + rt1r * h[iy];
            h[iy] = h22 * h[iy] - rt1r * h[ix];
            h[ix] = tst;
            iy += 3;
            ix += 3;
          }
        }

        if (i - 1 >= 1) {
          ix = m;
          iy = i * 3;
          for (k = 0; k <= i - 2; k++) {
            tst = h22 * h[ix] + rt1r * h[iy];
            h[iy] = h22 * h[iy] - rt1r * h[ix];
            h[ix] = tst;
            iy++;
            ix++;
          }
        }

        iy = i * 3;
        tst = h22 * z[m] + rt1r * z[iy];
        z[iy] = h22 * z[iy] - rt1r * z[m];
        z[m] = tst;
        iy++;
        ix = m + 1;
        tst = h22 * z[ix] + rt1r * z[iy];
        z[iy] = h22 * z[iy] - rt1r * z[ix];
        z[ix] = tst;
        iy++;
        ix++;
        tst = h22 * z[ix] + rt1r * z[iy];
        z[iy] = h22 * z[iy] - rt1r * z[ix];
        z[ix] = tst;
      }

      i = L - 2;
    }
  }

  return info;
}

/*
 * File trailer for xhseqr.c
 *
 * [EOF]
 */
