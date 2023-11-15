/*
 * File: inv.c
 *
 * MATLAB Coder version            : 4.2
 * C/C++ source code generated on  : 15-Jan-2023 14:17:59
 */

/* Include Files */
#include <math.h>
#include <string.h>
#include "rt_nonfinite.h"
#include "magCalibration4.h"
#include "inv.h"
#include "magCalibration4_rtwutil.h"

/* Function Definitions */

/*
 * Arguments    : const creal_T x[9]
 *                creal_T y[9]
 * Return Type  : void
 */
void inv(const creal_T x[9], creal_T y[9])
{
  creal_T b_x[9];
  int p1;
  int p2;
  int p3;
  double absx11;
  double absx21;
  double absx31;
  double t2_im;
  int itmp;
  double t1_re;
  double t1_im;
  double ar;
  double ai;
  double t2_re;
  double x_im;
  memcpy(&b_x[0], &x[0], 9U * sizeof(creal_T));
  p1 = 0;
  p2 = 3;
  p3 = 6;
  absx11 = rt_hypotd_snf(x[0].re, x[0].im);
  absx21 = rt_hypotd_snf(x[1].re, x[1].im);
  absx31 = rt_hypotd_snf(x[2].re, x[2].im);
  if ((absx21 > absx11) && (absx21 > absx31)) {
    p1 = 3;
    p2 = 0;
    b_x[0] = x[1];
    b_x[1] = x[0];
    b_x[3] = x[4];
    b_x[4] = x[3];
    b_x[6] = x[7];
    b_x[7] = x[6];
  } else {
    if (absx31 > absx11) {
      p1 = 6;
      p3 = 0;
      b_x[0] = x[2];
      b_x[2] = x[0];
      b_x[3] = x[5];
      b_x[5] = x[3];
      b_x[6] = x[8];
      b_x[8] = x[6];
    }
  }

  t2_im = b_x[1].re;
  if (b_x[0].im == 0.0) {
    if (b_x[1].im == 0.0) {
      b_x[1].re /= b_x[0].re;
      b_x[1].im = 0.0;
    } else if (b_x[1].re == 0.0) {
      b_x[1].re = 0.0;
      b_x[1].im /= b_x[0].re;
    } else {
      b_x[1].re /= b_x[0].re;
      b_x[1].im /= b_x[0].re;
    }
  } else if (b_x[0].re == 0.0) {
    if (b_x[1].re == 0.0) {
      b_x[1].re = b_x[1].im / b_x[0].im;
      b_x[1].im = 0.0;
    } else if (b_x[1].im == 0.0) {
      b_x[1].re = 0.0;
      b_x[1].im = -(t2_im / b_x[0].im);
    } else {
      b_x[1].re = b_x[1].im / b_x[0].im;
      b_x[1].im = -(t2_im / b_x[0].im);
    }
  } else {
    absx31 = fabs(b_x[0].re);
    absx11 = fabs(b_x[0].im);
    if (absx31 > absx11) {
      absx21 = b_x[0].im / b_x[0].re;
      absx31 = b_x[0].re + absx21 * b_x[0].im;
      b_x[1].re = (b_x[1].re + absx21 * b_x[1].im) / absx31;
      b_x[1].im = (b_x[1].im - absx21 * t2_im) / absx31;
    } else if (absx11 == absx31) {
      if (b_x[0].re > 0.0) {
        absx11 = 0.5;
      } else {
        absx11 = -0.5;
      }

      if (b_x[0].im > 0.0) {
        absx21 = 0.5;
      } else {
        absx21 = -0.5;
      }

      b_x[1].re = (b_x[1].re * absx11 + b_x[1].im * absx21) / absx31;
      b_x[1].im = (b_x[1].im * absx11 - t2_im * absx21) / absx31;
    } else {
      absx21 = b_x[0].re / b_x[0].im;
      absx31 = b_x[0].im + absx21 * b_x[0].re;
      absx11 = absx21 * b_x[1].im - b_x[1].re;
      b_x[1].re = (absx21 * b_x[1].re + b_x[1].im) / absx31;
      b_x[1].im = absx11 / absx31;
    }
  }

  t2_im = b_x[2].re;
  if (b_x[0].im == 0.0) {
    if (b_x[2].im == 0.0) {
      b_x[2].re /= b_x[0].re;
      b_x[2].im = 0.0;
    } else if (b_x[2].re == 0.0) {
      b_x[2].re = 0.0;
      b_x[2].im /= b_x[0].re;
    } else {
      b_x[2].re /= b_x[0].re;
      b_x[2].im /= b_x[0].re;
    }
  } else if (b_x[0].re == 0.0) {
    if (b_x[2].re == 0.0) {
      b_x[2].re = b_x[2].im / b_x[0].im;
      b_x[2].im = 0.0;
    } else if (b_x[2].im == 0.0) {
      b_x[2].re = 0.0;
      b_x[2].im = -(t2_im / b_x[0].im);
    } else {
      b_x[2].re = b_x[2].im / b_x[0].im;
      b_x[2].im = -(t2_im / b_x[0].im);
    }
  } else {
    absx31 = fabs(b_x[0].re);
    absx11 = fabs(b_x[0].im);
    if (absx31 > absx11) {
      absx21 = b_x[0].im / b_x[0].re;
      absx31 = b_x[0].re + absx21 * b_x[0].im;
      b_x[2].re = (b_x[2].re + absx21 * b_x[2].im) / absx31;
      b_x[2].im = (b_x[2].im - absx21 * t2_im) / absx31;
    } else if (absx11 == absx31) {
      if (b_x[0].re > 0.0) {
        absx11 = 0.5;
      } else {
        absx11 = -0.5;
      }

      if (b_x[0].im > 0.0) {
        absx21 = 0.5;
      } else {
        absx21 = -0.5;
      }

      b_x[2].re = (b_x[2].re * absx11 + b_x[2].im * absx21) / absx31;
      b_x[2].im = (b_x[2].im * absx11 - t2_im * absx21) / absx31;
    } else {
      absx21 = b_x[0].re / b_x[0].im;
      absx31 = b_x[0].im + absx21 * b_x[0].re;
      absx11 = absx21 * b_x[2].im - b_x[2].re;
      b_x[2].re = (absx21 * b_x[2].re + b_x[2].im) / absx31;
      b_x[2].im = absx11 / absx31;
    }
  }

  b_x[4].re -= b_x[1].re * b_x[3].re - b_x[1].im * b_x[3].im;
  b_x[4].im -= b_x[1].re * b_x[3].im + b_x[1].im * b_x[3].re;
  b_x[5].re -= b_x[2].re * b_x[3].re - b_x[2].im * b_x[3].im;
  b_x[5].im -= b_x[2].re * b_x[3].im + b_x[2].im * b_x[3].re;
  b_x[7].re -= b_x[1].re * b_x[6].re - b_x[1].im * b_x[6].im;
  b_x[7].im -= b_x[1].re * b_x[6].im + b_x[1].im * b_x[6].re;
  b_x[8].re -= b_x[2].re * b_x[6].re - b_x[2].im * b_x[6].im;
  b_x[8].im -= b_x[2].re * b_x[6].im + b_x[2].im * b_x[6].re;
  if (rt_hypotd_snf(b_x[5].re, b_x[5].im) > rt_hypotd_snf(b_x[4].re, b_x[4].im))
  {
    itmp = p2;
    p2 = p3;
    p3 = itmp;
    t1_re = b_x[1].re;
    t1_im = b_x[1].im;
    b_x[1] = b_x[2];
    b_x[2].re = t1_re;
    b_x[2].im = t1_im;
    t1_re = b_x[4].re;
    t1_im = b_x[4].im;
    b_x[4] = b_x[5];
    b_x[5].re = t1_re;
    b_x[5].im = t1_im;
    t1_re = b_x[7].re;
    t1_im = b_x[7].im;
    b_x[7] = b_x[8];
    b_x[8].re = t1_re;
    b_x[8].im = t1_im;
  }

  t2_im = b_x[5].re;
  if (b_x[4].im == 0.0) {
    if (b_x[5].im == 0.0) {
      b_x[5].re /= b_x[4].re;
      b_x[5].im = 0.0;
    } else if (b_x[5].re == 0.0) {
      b_x[5].re = 0.0;
      b_x[5].im /= b_x[4].re;
    } else {
      b_x[5].re /= b_x[4].re;
      b_x[5].im /= b_x[4].re;
    }
  } else if (b_x[4].re == 0.0) {
    if (b_x[5].re == 0.0) {
      b_x[5].re = b_x[5].im / b_x[4].im;
      b_x[5].im = 0.0;
    } else if (b_x[5].im == 0.0) {
      b_x[5].re = 0.0;
      b_x[5].im = -(t2_im / b_x[4].im);
    } else {
      b_x[5].re = b_x[5].im / b_x[4].im;
      b_x[5].im = -(t2_im / b_x[4].im);
    }
  } else {
    absx31 = fabs(b_x[4].re);
    absx11 = fabs(b_x[4].im);
    if (absx31 > absx11) {
      absx21 = b_x[4].im / b_x[4].re;
      absx31 = b_x[4].re + absx21 * b_x[4].im;
      b_x[5].re = (b_x[5].re + absx21 * b_x[5].im) / absx31;
      b_x[5].im = (b_x[5].im - absx21 * t2_im) / absx31;
    } else if (absx11 == absx31) {
      if (b_x[4].re > 0.0) {
        absx11 = 0.5;
      } else {
        absx11 = -0.5;
      }

      if (b_x[4].im > 0.0) {
        absx21 = 0.5;
      } else {
        absx21 = -0.5;
      }

      b_x[5].re = (b_x[5].re * absx11 + b_x[5].im * absx21) / absx31;
      b_x[5].im = (b_x[5].im * absx11 - t2_im * absx21) / absx31;
    } else {
      absx21 = b_x[4].re / b_x[4].im;
      absx31 = b_x[4].im + absx21 * b_x[4].re;
      absx11 = absx21 * b_x[5].im - b_x[5].re;
      b_x[5].re = (absx21 * b_x[5].re + b_x[5].im) / absx31;
      b_x[5].im = absx11 / absx31;
    }
  }

  b_x[8].re -= b_x[5].re * b_x[7].re - b_x[5].im * b_x[7].im;
  b_x[8].im -= b_x[5].re * b_x[7].im + b_x[5].im * b_x[7].re;
  ar = (b_x[5].re * b_x[1].re - b_x[5].im * b_x[1].im) - b_x[2].re;
  ai = (b_x[5].re * b_x[1].im + b_x[5].im * b_x[1].re) - b_x[2].im;
  if (b_x[8].im == 0.0) {
    if (ai == 0.0) {
      t1_re = ar / b_x[8].re;
      t1_im = 0.0;
    } else if (ar == 0.0) {
      t1_re = 0.0;
      t1_im = ai / b_x[8].re;
    } else {
      t1_re = ar / b_x[8].re;
      t1_im = ai / b_x[8].re;
    }
  } else if (b_x[8].re == 0.0) {
    if (ar == 0.0) {
      t1_re = ai / b_x[8].im;
      t1_im = 0.0;
    } else if (ai == 0.0) {
      t1_re = 0.0;
      t1_im = -(ar / b_x[8].im);
    } else {
      t1_re = ai / b_x[8].im;
      t1_im = -(ar / b_x[8].im);
    }
  } else {
    absx31 = fabs(b_x[8].re);
    absx11 = fabs(b_x[8].im);
    if (absx31 > absx11) {
      absx21 = b_x[8].im / b_x[8].re;
      absx31 = b_x[8].re + absx21 * b_x[8].im;
      t1_re = (ar + absx21 * ai) / absx31;
      t1_im = (ai - absx21 * ar) / absx31;
    } else if (absx11 == absx31) {
      if (b_x[8].re > 0.0) {
        absx11 = 0.5;
      } else {
        absx11 = -0.5;
      }

      if (b_x[8].im > 0.0) {
        absx21 = 0.5;
      } else {
        absx21 = -0.5;
      }

      t1_re = (ar * absx11 + ai * absx21) / absx31;
      t1_im = (ai * absx11 - ar * absx21) / absx31;
    } else {
      absx21 = b_x[8].re / b_x[8].im;
      absx31 = b_x[8].im + absx21 * b_x[8].re;
      t1_re = (absx21 * ar + ai) / absx31;
      t1_im = (absx21 * ai - ar) / absx31;
    }
  }

  ar = -(b_x[1].re + (b_x[7].re * t1_re - b_x[7].im * t1_im));
  ai = -(b_x[1].im + (b_x[7].re * t1_im + b_x[7].im * t1_re));
  if (b_x[4].im == 0.0) {
    if (ai == 0.0) {
      t2_re = ar / b_x[4].re;
      t2_im = 0.0;
    } else if (ar == 0.0) {
      t2_re = 0.0;
      t2_im = ai / b_x[4].re;
    } else {
      t2_re = ar / b_x[4].re;
      t2_im = ai / b_x[4].re;
    }
  } else if (b_x[4].re == 0.0) {
    if (ar == 0.0) {
      t2_re = ai / b_x[4].im;
      t2_im = 0.0;
    } else if (ai == 0.0) {
      t2_re = 0.0;
      t2_im = -(ar / b_x[4].im);
    } else {
      t2_re = ai / b_x[4].im;
      t2_im = -(ar / b_x[4].im);
    }
  } else {
    absx31 = fabs(b_x[4].re);
    absx11 = fabs(b_x[4].im);
    if (absx31 > absx11) {
      absx21 = b_x[4].im / b_x[4].re;
      absx31 = b_x[4].re + absx21 * b_x[4].im;
      t2_re = (ar + absx21 * ai) / absx31;
      t2_im = (ai - absx21 * ar) / absx31;
    } else if (absx11 == absx31) {
      if (b_x[4].re > 0.0) {
        absx11 = 0.5;
      } else {
        absx11 = -0.5;
      }

      if (b_x[4].im > 0.0) {
        absx21 = 0.5;
      } else {
        absx21 = -0.5;
      }

      t2_re = (ar * absx11 + ai * absx21) / absx31;
      t2_im = (ai * absx11 - ar * absx21) / absx31;
    } else {
      absx21 = b_x[4].re / b_x[4].im;
      absx31 = b_x[4].im + absx21 * b_x[4].re;
      t2_re = (absx21 * ar + ai) / absx31;
      t2_im = (absx21 * ai - ar) / absx31;
    }
  }

  ar = (1.0 - (b_x[3].re * t2_re - b_x[3].im * t2_im)) - (b_x[6].re * t1_re -
    b_x[6].im * t1_im);
  ai = (0.0 - (b_x[3].re * t2_im + b_x[3].im * t2_re)) - (b_x[6].re * t1_im +
    b_x[6].im * t1_re);
  if (b_x[0].im == 0.0) {
    if (ai == 0.0) {
      y[p1].re = ar / b_x[0].re;
      y[p1].im = 0.0;
    } else if (ar == 0.0) {
      y[p1].re = 0.0;
      y[p1].im = ai / b_x[0].re;
    } else {
      y[p1].re = ar / b_x[0].re;
      y[p1].im = ai / b_x[0].re;
    }
  } else if (b_x[0].re == 0.0) {
    if (ar == 0.0) {
      y[p1].re = ai / b_x[0].im;
      y[p1].im = 0.0;
    } else if (ai == 0.0) {
      y[p1].re = 0.0;
      y[p1].im = -(ar / b_x[0].im);
    } else {
      y[p1].re = ai / b_x[0].im;
      y[p1].im = -(ar / b_x[0].im);
    }
  } else {
    absx31 = fabs(b_x[0].re);
    absx11 = fabs(b_x[0].im);
    if (absx31 > absx11) {
      absx21 = b_x[0].im / b_x[0].re;
      absx31 = b_x[0].re + absx21 * b_x[0].im;
      y[p1].re = (ar + absx21 * ai) / absx31;
      y[p1].im = (ai - absx21 * ar) / absx31;
    } else if (absx11 == absx31) {
      if (b_x[0].re > 0.0) {
        absx11 = 0.5;
      } else {
        absx11 = -0.5;
      }

      if (b_x[0].im > 0.0) {
        absx21 = 0.5;
      } else {
        absx21 = -0.5;
      }

      y[p1].re = (ar * absx11 + ai * absx21) / absx31;
      y[p1].im = (ai * absx11 - ar * absx21) / absx31;
    } else {
      absx21 = b_x[0].re / b_x[0].im;
      absx31 = b_x[0].im + absx21 * b_x[0].re;
      y[p1].re = (absx21 * ar + ai) / absx31;
      y[p1].im = (absx21 * ai - ar) / absx31;
    }
  }

  y[p1 + 1].re = t2_re;
  y[p1 + 1].im = t2_im;
  y[p1 + 2].re = t1_re;
  y[p1 + 2].im = t1_im;
  if (b_x[8].im == 0.0) {
    if (-b_x[5].im == 0.0) {
      t1_re = -b_x[5].re / b_x[8].re;
      t1_im = 0.0;
    } else if (-b_x[5].re == 0.0) {
      t1_re = 0.0;
      t1_im = -b_x[5].im / b_x[8].re;
    } else {
      t1_re = -b_x[5].re / b_x[8].re;
      t1_im = -b_x[5].im / b_x[8].re;
    }
  } else if (b_x[8].re == 0.0) {
    if (-b_x[5].re == 0.0) {
      t1_re = -b_x[5].im / b_x[8].im;
      t1_im = 0.0;
    } else if (-b_x[5].im == 0.0) {
      t1_re = 0.0;
      t1_im = -(-b_x[5].re / b_x[8].im);
    } else {
      t1_re = -b_x[5].im / b_x[8].im;
      t1_im = -(-b_x[5].re / b_x[8].im);
    }
  } else {
    absx31 = fabs(b_x[8].re);
    absx11 = fabs(b_x[8].im);
    if (absx31 > absx11) {
      absx21 = b_x[8].im / b_x[8].re;
      absx31 = b_x[8].re + absx21 * b_x[8].im;
      t1_re = (-b_x[5].re + absx21 * -b_x[5].im) / absx31;
      t1_im = (-b_x[5].im - absx21 * -b_x[5].re) / absx31;
    } else if (absx11 == absx31) {
      if (b_x[8].re > 0.0) {
        absx11 = 0.5;
      } else {
        absx11 = -0.5;
      }

      if (b_x[8].im > 0.0) {
        absx21 = 0.5;
      } else {
        absx21 = -0.5;
      }

      t1_re = (-b_x[5].re * absx11 + -b_x[5].im * absx21) / absx31;
      t1_im = (-b_x[5].im * absx11 - -b_x[5].re * absx21) / absx31;
    } else {
      absx21 = b_x[8].re / b_x[8].im;
      absx31 = b_x[8].im + absx21 * b_x[8].re;
      t1_re = (absx21 * -b_x[5].re + -b_x[5].im) / absx31;
      t1_im = (absx21 * -b_x[5].im - (-b_x[5].re)) / absx31;
    }
  }

  t2_im = b_x[7].re * t1_re - b_x[7].im * t1_im;
  x_im = b_x[7].re * t1_im + b_x[7].im * t1_re;
  ar = 1.0 - (b_x[7].re * t1_re - b_x[7].im * t1_im);
  ai = 0.0 - (b_x[7].re * t1_im + b_x[7].im * t1_re);
  if (b_x[4].im == 0.0) {
    if (0.0 - x_im == 0.0) {
      t2_re = ar / b_x[4].re;
      t2_im = 0.0;
    } else if (1.0 - t2_im == 0.0) {
      t2_re = 0.0;
      t2_im = ai / b_x[4].re;
    } else {
      t2_re = ar / b_x[4].re;
      t2_im = ai / b_x[4].re;
    }
  } else if (b_x[4].re == 0.0) {
    if (1.0 - t2_im == 0.0) {
      t2_re = ai / b_x[4].im;
      t2_im = 0.0;
    } else if (0.0 - x_im == 0.0) {
      t2_re = 0.0;
      t2_im = -(ar / b_x[4].im);
    } else {
      t2_re = ai / b_x[4].im;
      t2_im = -(ar / b_x[4].im);
    }
  } else {
    absx31 = fabs(b_x[4].re);
    absx11 = fabs(b_x[4].im);
    if (absx31 > absx11) {
      absx21 = b_x[4].im / b_x[4].re;
      absx31 = b_x[4].re + absx21 * b_x[4].im;
      t2_re = ((1.0 - t2_im) + absx21 * ai) / absx31;
      t2_im = ((0.0 - x_im) - absx21 * ar) / absx31;
    } else if (absx11 == absx31) {
      if (b_x[4].re > 0.0) {
        absx11 = 0.5;
      } else {
        absx11 = -0.5;
      }

      if (b_x[4].im > 0.0) {
        absx21 = 0.5;
      } else {
        absx21 = -0.5;
      }

      t2_re = (ar * absx11 + ai * absx21) / absx31;
      t2_im = (ai * absx11 - ar * absx21) / absx31;
    } else {
      absx21 = b_x[4].re / b_x[4].im;
      absx31 = b_x[4].im + absx21 * b_x[4].re;
      t2_re = (absx21 * ar + (0.0 - x_im)) / absx31;
      t2_im = (absx21 * ai - (1.0 - t2_im)) / absx31;
    }
  }

  ar = -((b_x[3].re * t2_re - b_x[3].im * t2_im) + (b_x[6].re * t1_re - b_x[6].
          im * t1_im));
  ai = -((b_x[3].re * t2_im + b_x[3].im * t2_re) + (b_x[6].re * t1_im + b_x[6].
          im * t1_re));
  if (b_x[0].im == 0.0) {
    if (ai == 0.0) {
      y[p2].re = ar / b_x[0].re;
      y[p2].im = 0.0;
    } else if (ar == 0.0) {
      y[p2].re = 0.0;
      y[p2].im = ai / b_x[0].re;
    } else {
      y[p2].re = ar / b_x[0].re;
      y[p2].im = ai / b_x[0].re;
    }
  } else if (b_x[0].re == 0.0) {
    if (ar == 0.0) {
      y[p2].re = ai / b_x[0].im;
      y[p2].im = 0.0;
    } else if (ai == 0.0) {
      y[p2].re = 0.0;
      y[p2].im = -(ar / b_x[0].im);
    } else {
      y[p2].re = ai / b_x[0].im;
      y[p2].im = -(ar / b_x[0].im);
    }
  } else {
    absx31 = fabs(b_x[0].re);
    absx11 = fabs(b_x[0].im);
    if (absx31 > absx11) {
      absx21 = b_x[0].im / b_x[0].re;
      absx31 = b_x[0].re + absx21 * b_x[0].im;
      y[p2].re = (ar + absx21 * ai) / absx31;
      y[p2].im = (ai - absx21 * ar) / absx31;
    } else if (absx11 == absx31) {
      if (b_x[0].re > 0.0) {
        absx11 = 0.5;
      } else {
        absx11 = -0.5;
      }

      if (b_x[0].im > 0.0) {
        absx21 = 0.5;
      } else {
        absx21 = -0.5;
      }

      y[p2].re = (ar * absx11 + ai * absx21) / absx31;
      y[p2].im = (ai * absx11 - ar * absx21) / absx31;
    } else {
      absx21 = b_x[0].re / b_x[0].im;
      absx31 = b_x[0].im + absx21 * b_x[0].re;
      y[p2].re = (absx21 * ar + ai) / absx31;
      y[p2].im = (absx21 * ai - ar) / absx31;
    }
  }

  y[p2 + 1].re = t2_re;
  y[p2 + 1].im = t2_im;
  y[p2 + 2].re = t1_re;
  y[p2 + 2].im = t1_im;
  if (b_x[8].im == 0.0) {
    t1_re = 1.0 / b_x[8].re;
    t1_im = 0.0;
  } else if (b_x[8].re == 0.0) {
    t1_re = 0.0;
    t1_im = -(1.0 / b_x[8].im);
  } else {
    absx31 = fabs(b_x[8].re);
    absx11 = fabs(b_x[8].im);
    if (absx31 > absx11) {
      absx21 = b_x[8].im / b_x[8].re;
      absx31 = b_x[8].re + absx21 * b_x[8].im;
      t1_re = (1.0 + absx21 * 0.0) / absx31;
      t1_im = (0.0 - absx21) / absx31;
    } else if (absx11 == absx31) {
      if (b_x[8].re > 0.0) {
        absx11 = 0.5;
      } else {
        absx11 = -0.5;
      }

      if (b_x[8].im > 0.0) {
        absx21 = 0.5;
      } else {
        absx21 = -0.5;
      }

      t1_re = (absx11 + 0.0 * absx21) / absx31;
      t1_im = (0.0 * absx11 - absx21) / absx31;
    } else {
      absx21 = b_x[8].re / b_x[8].im;
      absx31 = b_x[8].im + absx21 * b_x[8].re;
      t1_re = absx21 / absx31;
      t1_im = (absx21 * 0.0 - 1.0) / absx31;
    }
  }

  t2_im = -b_x[7].re * t1_re - -b_x[7].im * t1_im;
  x_im = -b_x[7].re * t1_im + -b_x[7].im * t1_re;
  if (b_x[4].im == 0.0) {
    if (x_im == 0.0) {
      t2_re = t2_im / b_x[4].re;
      t2_im = 0.0;
    } else if (t2_im == 0.0) {
      t2_re = 0.0;
      t2_im = x_im / b_x[4].re;
    } else {
      t2_re = t2_im / b_x[4].re;
      t2_im = x_im / b_x[4].re;
    }
  } else if (b_x[4].re == 0.0) {
    if (t2_im == 0.0) {
      t2_re = x_im / b_x[4].im;
      t2_im = 0.0;
    } else if (x_im == 0.0) {
      t2_re = 0.0;
      t2_im = -(t2_im / b_x[4].im);
    } else {
      t2_re = x_im / b_x[4].im;
      t2_im = -(t2_im / b_x[4].im);
    }
  } else {
    absx31 = fabs(b_x[4].re);
    absx11 = fabs(b_x[4].im);
    if (absx31 > absx11) {
      absx21 = b_x[4].im / b_x[4].re;
      absx31 = b_x[4].re + absx21 * b_x[4].im;
      t2_re = (t2_im + absx21 * x_im) / absx31;
      t2_im = (x_im - absx21 * t2_im) / absx31;
    } else if (absx11 == absx31) {
      if (b_x[4].re > 0.0) {
        absx11 = 0.5;
      } else {
        absx11 = -0.5;
      }

      if (b_x[4].im > 0.0) {
        absx21 = 0.5;
      } else {
        absx21 = -0.5;
      }

      t2_re = (t2_im * absx11 + x_im * absx21) / absx31;
      t2_im = (x_im * absx11 - t2_im * absx21) / absx31;
    } else {
      absx21 = b_x[4].re / b_x[4].im;
      absx31 = b_x[4].im + absx21 * b_x[4].re;
      t2_re = (absx21 * t2_im + x_im) / absx31;
      t2_im = (absx21 * x_im - t2_im) / absx31;
    }
  }

  ar = -((b_x[3].re * t2_re - b_x[3].im * t2_im) + (b_x[6].re * t1_re - b_x[6].
          im * t1_im));
  ai = -((b_x[3].re * t2_im + b_x[3].im * t2_re) + (b_x[6].re * t1_im + b_x[6].
          im * t1_re));
  if (b_x[0].im == 0.0) {
    if (ai == 0.0) {
      y[p3].re = ar / b_x[0].re;
      y[p3].im = 0.0;
    } else if (ar == 0.0) {
      y[p3].re = 0.0;
      y[p3].im = ai / b_x[0].re;
    } else {
      y[p3].re = ar / b_x[0].re;
      y[p3].im = ai / b_x[0].re;
    }
  } else if (b_x[0].re == 0.0) {
    if (ar == 0.0) {
      y[p3].re = ai / b_x[0].im;
      y[p3].im = 0.0;
    } else if (ai == 0.0) {
      y[p3].re = 0.0;
      y[p3].im = -(ar / b_x[0].im);
    } else {
      y[p3].re = ai / b_x[0].im;
      y[p3].im = -(ar / b_x[0].im);
    }
  } else {
    absx31 = fabs(b_x[0].re);
    absx11 = fabs(b_x[0].im);
    if (absx31 > absx11) {
      absx21 = b_x[0].im / b_x[0].re;
      absx31 = b_x[0].re + absx21 * b_x[0].im;
      y[p3].re = (ar + absx21 * ai) / absx31;
      y[p3].im = (ai - absx21 * ar) / absx31;
    } else if (absx11 == absx31) {
      if (b_x[0].re > 0.0) {
        absx11 = 0.5;
      } else {
        absx11 = -0.5;
      }

      if (b_x[0].im > 0.0) {
        absx21 = 0.5;
      } else {
        absx21 = -0.5;
      }

      y[p3].re = (ar * absx11 + ai * absx21) / absx31;
      y[p3].im = (ai * absx11 - ar * absx21) / absx31;
    } else {
      absx21 = b_x[0].re / b_x[0].im;
      absx31 = b_x[0].im + absx21 * b_x[0].re;
      y[p3].re = (absx21 * ar + ai) / absx31;
      y[p3].im = (absx21 * ai - ar) / absx31;
    }
  }

  y[p3 + 1].re = t2_re;
  y[p3 + 1].im = t2_im;
  y[p3 + 2].re = t1_re;
  y[p3 + 2].im = t1_im;
}

/*
 * File trailer for inv.c
 *
 * [EOF]
 */
