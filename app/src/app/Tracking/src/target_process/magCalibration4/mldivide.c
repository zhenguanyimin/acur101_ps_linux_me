/*
 * File: mldivide.c
 *
 * MATLAB Coder version            : 4.2
 * C/C++ source code generated on  : 15-Jan-2023 14:17:59
 */

/* Include Files */
#include <math.h>
#include <string.h>
#include "rt_nonfinite.h"
#include "magCalibration4.h"
#include "mldivide.h"

/* Function Definitions */

/*
 * Arguments    : const double A[81]
 *                double B[9]
 * Return Type  : void
 */
void b_mldivide(const double A[81], double B[9])
{
  double b_A[81];
  int i8;
  int j;
  signed char ipiv[9];
  int k;
  int b;
  int jj;
  int iy;
  int jp1j;
  int n;
  int jA;
  int ix;
  double smax;
  double s;
  memcpy(&b_A[0], &A[0], 81U * sizeof(double));
  for (i8 = 0; i8 < 9; i8++) {
    ipiv[i8] = (signed char)(1 + i8);
  }

  for (j = 0; j < 8; j++) {
    b = j * 10;
    jj = j * 10;
    jp1j = b + 2;
    n = 9 - j;
    iy = 0;
    ix = b;
    smax = fabs(b_A[b]);
    for (k = 2; k <= n; k++) {
      ix++;
      s = fabs(b_A[ix]);
      if (s > smax) {
        iy = k - 1;
        smax = s;
      }
    }

    if (b_A[jj + iy] != 0.0) {
      if (iy != 0) {
        iy += j;
        ipiv[j] = (signed char)(iy + 1);
        ix = j;
        for (k = 0; k < 9; k++) {
          smax = b_A[ix];
          b_A[ix] = b_A[iy];
          b_A[iy] = smax;
          ix += 9;
          iy += 9;
        }
      }

      i8 = (jj - j) + 9;
      for (jA = jp1j; jA <= i8; jA++) {
        b_A[jA - 1] /= b_A[jj];
      }
    }

    n = 7 - j;
    iy = b + 9;
    jA = jj;
    for (b = 0; b <= n; b++) {
      smax = b_A[iy];
      if (b_A[iy] != 0.0) {
        ix = jj + 1;
        i8 = jA + 11;
        jp1j = (jA - j) + 18;
        for (k = i8; k <= jp1j; k++) {
          b_A[k - 1] += b_A[ix] * -smax;
          ix++;
        }
      }

      iy += 9;
      jA += 9;
    }

    if (ipiv[j] != j + 1) {
      smax = B[j];
      i8 = ipiv[j] - 1;
      B[j] = B[i8];
      B[i8] = smax;
    }
  }

  for (k = 0; k < 9; k++) {
    iy = 9 * k;
    if (B[k] != 0.0) {
      i8 = k + 2;
      for (jA = i8; jA < 10; jA++) {
        B[jA - 1] -= B[k] * b_A[(jA + iy) - 1];
      }
    }
  }

  for (k = 8; k >= 0; k--) {
    iy = 9 * k;
    if (B[k] != 0.0) {
      B[k] /= b_A[k + iy];
      for (jA = 0; jA < k; jA++) {
        B[jA] -= B[k] * b_A[jA + iy];
      }
    }
  }
}

/*
 * Arguments    : const double A[9]
 *                const double B[3]
 *                double Y[3]
 * Return Type  : void
 */
void mldivide(const double A[9], const double B[3], double Y[3])
{
  double b_A[9];
  int r1;
  int r2;
  int r3;
  double maxval;
  double a21;
  int rtemp;
  memcpy(&b_A[0], &A[0], 9U * sizeof(double));
  r1 = 0;
  r2 = 1;
  r3 = 2;
  maxval = fabs(A[0]);
  a21 = fabs(A[1]);
  if (a21 > maxval) {
    maxval = a21;
    r1 = 1;
    r2 = 0;
  }

  if (fabs(A[2]) > maxval) {
    r1 = 2;
    r2 = 1;
    r3 = 0;
  }

  b_A[r2] = A[r2] / A[r1];
  b_A[r3] /= b_A[r1];
  b_A[3 + r2] -= b_A[r2] * b_A[3 + r1];
  b_A[3 + r3] -= b_A[r3] * b_A[3 + r1];
  b_A[6 + r2] -= b_A[r2] * b_A[6 + r1];
  b_A[6 + r3] -= b_A[r3] * b_A[6 + r1];
  if (fabs(b_A[3 + r3]) > fabs(b_A[3 + r2])) {
    rtemp = r2;
    r2 = r3;
    r3 = rtemp;
  }

  b_A[3 + r3] /= b_A[3 + r2];
  b_A[6 + r3] -= b_A[3 + r3] * b_A[6 + r2];
  Y[1] = B[r2] - B[r1] * b_A[r2];
  Y[2] = (B[r3] - B[r1] * b_A[r3]) - Y[1] * b_A[3 + r3];
  Y[2] /= b_A[6 + r3];
  Y[0] = B[r1] - Y[2] * b_A[6 + r1];
  Y[1] -= Y[2] * b_A[6 + r2];
  Y[1] /= b_A[3 + r2];
  Y[0] -= Y[1] * b_A[3 + r1];
  Y[0] /= b_A[r1];
}

/*
 * File trailer for mldivide.c
 *
 * [EOF]
 */
