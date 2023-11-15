/*
 * File: repmat.c
 *
 * MATLAB Coder version            : 4.2
 * C/C++ source code generated on  : 15-Jan-2023 14:17:59
 */

/* Include Files */
#include "rt_nonfinite.h"
#include "magCalibration4.h"
#include "repmat.h"

/* Function Definitions */

/*
 * Arguments    : const creal_T a[3]
 *                creal_T b[360]
 * Return Type  : void
 */
void repmat(const creal_T a[3], creal_T b[360])
{
  int jcol;
  int ibmat;
  int itilerow;
  for (jcol = 0; jcol < 3; jcol++) {
    ibmat = jcol * 120;
    for (itilerow = 0; itilerow < 120; itilerow++) {
      b[ibmat + itilerow] = a[jcol];
    }
  }
}

/*
 * File trailer for repmat.c
 *
 * [EOF]
 */
