/*
 * File: xgemv.h
 *
 * MATLAB Coder version            : 4.2
 * C/C++ source code generated on  : 15-Jan-2023 14:17:59
 */

#ifndef XGEMV_H
#define XGEMV_H

/* Include Files */
#include <stddef.h>
#include <stdlib.h>
#include "rtwtypes.h"
#include "magCalibration4_types.h"

/* Function Declarations */
extern void xgemv(int m, int n, const double A[9], int ia0, const double x[9],
                  int ix0, double y[3]);

#endif

/*
 * File trailer for xgemv.h
 *
 * [EOF]
 */
