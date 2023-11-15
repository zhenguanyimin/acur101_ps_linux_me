/*
 * File: xzlarf.c
 *
 * MATLAB Coder version            : 4.2
 * C/C++ source code generated on  : 15-Jan-2023 14:17:59
 */

/* Include Files */
#include "rt_nonfinite.h"
#include "magCalibration4.h"
#include "xzlarf.h"
#include "xgerc.h"
#include "xgemv.h"

/* Function Definitions */

/*
 * Arguments    : int m
 *                int n
 *                int iv0
 *                double tau
 *                double C[9]
 *                int ic0
 *                double work[3]
 * Return Type  : void
 */
void xzlarf(int m, int n, int iv0, double tau, double C[9], int ic0, double
            work[3])
{
  int lastv;
  int lastc;
  boolean_T exitg2;
  int coltop;
  int ia;
  int exitg1;
  if (tau != 0.0) {
    lastv = m;
    lastc = iv0 + m;
    while ((lastv > 0) && (C[lastc - 2] == 0.0)) {
      lastv--;
      lastc--;
    }

    lastc = n;
    exitg2 = false;
    while ((!exitg2) && (lastc > 0)) {
      coltop = ic0 + (lastc - 1) * 3;
      ia = coltop;
      do {
        exitg1 = 0;
        if (ia <= (coltop + lastv) - 1) {
          if (C[ia - 1] != 0.0) {
            exitg1 = 1;
          } else {
            ia++;
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
    lastv = 0;
    lastc = 0;
  }

  if (lastv > 0) {
    xgemv(lastv, lastc, C, ic0, C, iv0, work);
    xgerc(lastv, lastc, -tau, iv0, work, C, ic0);
  }
}

/*
 * File trailer for xzlarf.c
 *
 * [EOF]
 */
