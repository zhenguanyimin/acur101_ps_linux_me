/*
 * File: ellipsoid_fit2.c
 *
 * MATLAB Coder version            : 4.2
 * C/C++ source code generated on  : 15-Jan-2023 14:17:59
 */

/* Include Files */
#include <math.h>
#include <string.h>
#include "rt_nonfinite.h"
#include "magCalibration4.h"
#include "ellipsoid_fit2.h"
#include "sign.h"
#include "sum.h"
#include "abs.h"
#include "repmat.h"
#include "power.h"
#include "sqrt.h"
#include "diag.h"
#include "eig.h"
#include "mldivide.h"

/* Function Definitions */

/*
 * function [ center, radii, evecs, v, chi2 ] = ellipsoid_fit2( mag_x,mag_y, mag_z, equals )
 *
 *  Fit an ellispoid/sphere/paraboloid/hyperboloid to a set of xyz data points:
 *
 *    [center, radii, evecs, pars ] = ellipsoid_fit( X )
 *    [center, radii, evecs, pars ] = ellipsoid_fit( [x y z] );
 *    [center, radii, evecs, pars ] = ellipsoid_fit( X, 1 );
 *    [center, radii, evecs, pars ] = ellipsoid_fit( X, 2, 'xz' );
 *    [center, radii, evecs, pars ] = ellipsoid_fit( X, 3 );
 *
 *  Parameters:
 *  * X, [x y z]   - Cartesian data, n x 3 matrix or three n x 1 vectors
 *  * flag         - '' or empty fits an arbitrary ellipsoid (default),
 *                 - 'xy' fits a spheroid with x- and y- radii equal
 *                 - 'xz' fits a spheroid with x- and z- radii equal
 *                 - 'xyz' fits a sphere
 *                 - '0' fits an ellipsoid with its axes aligned along [x y z] axes
 *                 - '0xy' the same with x- and y- radii equal
 *                 - '0xz' the same with x- and z- radii equal
 *
 *  Output:
 *  * center    -  ellispoid or other conic center coordinates [xc; yc; zc]
 *  * radii     -  ellipsoid or other conic radii [a; b; c]
 *  * evecs     -  the radii directions as columns of the 3x3 matrix
 *  * v         -  the 10 parameters describing the ellipsoid / conic algebraically:
 *                 Ax^2 + By^2 + Cz^2 + 2Dxy + 2Exz + 2Fyz + 2Gx + 2Hy + 2Iz + J = 0
 *  * chi2      -  residual sum of squared errors (chi^2), this chi2 is in the
 *                 coordinate frame in which the ellipsoid is a unit sphere.
 *
 *  Author:
 *  Yury Petrov, Oculus VR
 *  Date:
 *  September, 2015
 * Arguments    : const double mag_x[120]
 *                const double mag_y[120]
 *                const double mag_z[120]
 *                double center[3]
 *                creal_T radii[3]
 *                creal_T evecs[9]
 *                double v[10]
 *                double *chi2
 * Return Type  : void
 */
void ellipsoid_fit2(const double mag_x[120], const double mag_y[120], const
                    double mag_z[120], double center[3], creal_T radii[3],
                    creal_T evecs[9], double v[10], double *chi2)
{
  int i;
  int i3;
  double d0;
  double bim;
  double a_tmp[81];
  double u[9];
  double d;
  double brm;
  double ar;
  double b_a_tmp[1080];
  double D_tmp[120];
  double ai;
  double D[1080];
  int i4;
  double A[16];
  double T[16];
  static const signed char iv0[16] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0,
    0, 1 };

  double b_T[16];
  creal_T evals[9];
  creal_T sgns[3];
  double dv0[3];
  creal_T b_mag_x[360];
  creal_T b_d[360];
  creal_T b_sgns[3];
  creal_T dcv1[120];
  double dv1[120];

  /*  function ellipsoid_fit2( mag_x,mag_y, mag_z) */
  /*  ellipsoid_fit2(x(1:60),y(1:60),z(1:60)) */
  /*  narginchk( 1, 3 ) ;  % check input arguments */
  /*  if nargin == 1 */
  /*      equals = ''; % no constraints by default */
  /*  end */
  /*       */
  /*  if size( X, 2 ) ~= 3 */
  /*      error( 'Input data must have three columns!' ); */
  /*  else */
  /*      x = X( :, 1 ); */
  /*      y = X( :, 2 ); */
  /*      z = X( :, 3 ); */
  /*  end */
  /*  % need nine or more data points */
  /*  if length( x ) < 9 && strcmp( equals, '' )  */
  /*     error( 'Must have at least 9 points to fit a unique ellipsoid' ); */
  /*  end */
  /*  if length( x ) < 8 && ( strcmp( equals, 'xy' ) || strcmp( equals, 'xz' ) ) */
  /*     error( 'Must have at least 8 points to fit a unique ellipsoid with two equal radii' ); */
  /*  end */
  /*  if length( x ) < 6 && strcmp( equals, '0' ) */
  /*     error( 'Must have at least 6 points to fit a unique oriented ellipsoid' ); */
  /*  end */
  /*  if length( x ) < 5 && ( strcmp( equals, '0xy' ) || strcmp( equals, '0xz' ) ) */
  /*     error( 'Must have at least 5 points to fit a unique oriented ellipsoid with two equal radii' ); */
  /*  end */
  /*  if length( x ) < 4 && strcmp( equals, 'xyz' ) */
  /*     error( 'Must have at least 4 points to fit a unique sphere' ); */
  /*  end */
  /*  fit ellipsoid in the form Ax^2 + By^2 + Cz^2 + 2Dxy + 2Exz + 2Fyz + 2Gx + */
  /*  2Hy + 2Iz + J = 0 and A + B + C = 3 constraint removing one extra */
  /*  % parameter */
  /*  if strcmp( equals, '' ) */
  /*      D = [ x .* x + y .* y - 2 * z .* z, ... */
  /*          x .* x + z .* z - 2 * y .* y, ... */
  /*          2 * x .* y, ... */
  /*          2 * x .* z, ... */
  /*          2 * y .* z, ... */
  /*          2 * x, ... */
  /*          2 * y, ... */
  /*          2 * z, ... */
  /*          1 + 0 * x ];  % ndatapoints x 9 ellipsoid parameters */
  /*  elseif strcmp( equals, 'xy' ) */
  /*      D = [ x .* x + y .* y - 2 * z .* z, ... */
  /*          2 * x .* y, ... */
  /*          2 * x .* z, ... */
  /*          2 * y .* z, ... */
  /*          2 * x, ... */
  /*          2 * y, ... */
  /*          2 * z, ... */
  /*          1 + 0 * x ];  % ndatapoints x 8 ellipsoid parameters */
  /*  elseif strcmp( equals, 'xz' ) */
  /*      D = [ x .* x + z .* z - 2 * y .* y, ... */
  /*          2 * x .* y, ... */
  /*          2 * x .* z, ... */
  /*          2 * y .* z, ... */
  /*          2 * x, ... */
  /*          2 * y, ... */
  /*          2 * z, ... */
  /*          1 + 0 * x ];  % ndatapoints x 8 ellipsoid parameters */
  /*      % fit ellipsoid in the form Ax^2 + By^2 + Cz^2 + 2Gx + 2Hy + 2Iz = 1 */
  /*  elseif strcmp( equals, '0' ) */
  /*      D = [ x .* x + y .* y - 2 * z .* z, ... */
  /*            x .* x + z .* z - 2 * y .* y, ... */
  /*            2 * x, ... */
  /*            2 * y, ...  */
  /*            2 * z, ...  */
  /*            1 + 0 * x ];  % ndatapoints x 6 ellipsoid parameters */
  /*      % fit ellipsoid in the form Ax^2 + By^2 + Cz^2 + 2Gx + 2Hy + 2Iz = 1, */
  /*      % where A = B or B = C or A = C */
  /*  elseif strcmp( equals, '0xy' ) */
  /*      D = [ x .* x + y .* y - 2 * z .* z, ... */
  /*            2 * x, ... */
  /*            2 * y, ...  */
  /*            2 * z, ...  */
  /*            1 + 0 * x ];  % ndatapoints x 5 ellipsoid parameters */
  /*  elseif strcmp( equals, '0xz' ) */
  /*      D = [ x .* x + z .* z - 2 * y .* y, ... */
  /*            2 * x, ... */
  /*            2 * y, ...  */
  /*            2 * z, ...  */
  /*            1 + 0 * x ];  % ndatapoints x 5 ellipsoid parameters */
  /*       % fit sphere in the form A(x^2 + y^2 + z^2) + 2Gx + 2Hy + 2Iz = 1 */
  /*  elseif strcmp( equals, 'xyz' ) */
  /*      D = [ 2 * x, ... */
  /*            2 * y, ...  */
  /*            2 * z, ...  */
  /*            1 + 0 * x ];  % ndatapoints x 4 ellipsoid parameters */
  /*  else */
  /*      error( [ 'Unknown parameter value ' equals '!' ] ); */
  /*  end */
  /*  ndatapoints x 9 ellipsoid parameters */
  /*  solve the normal system of equations */
  /*  the RHS of the llsq problem (y's) */
  for (i = 0; i < 120; i++) {
    d0 = mag_x[i] * mag_x[i];
    bim = 2.0 * mag_x[i];
    d = 2.0 * mag_y[i];
    brm = 2.0 * mag_z[i];
    ar = d0 + mag_y[i] * mag_y[i];
    ai = mag_z[i] * mag_z[i];
    D[i] = ar - brm * mag_z[i];
    D[120 + i] = (d0 + ai) - d * mag_y[i];
    D[240 + i] = bim * mag_y[i];
    D[360 + i] = bim * mag_z[i];
    D[480 + i] = d * mag_z[i];
    D[600 + i] = bim;
    D[720 + i] = d;
    D[840 + i] = brm;
    D[960 + i] = 1.0 + 0.0 * mag_x[i];
    for (i3 = 0; i3 < 9; i3++) {
      b_a_tmp[i3 + 9 * i] = D[i + 120 * i3];
    }

    ar += ai;
    D_tmp[i] = ar;
  }

  for (i3 = 0; i3 < 9; i3++) {
    d0 = 0.0;
    for (i = 0; i < 120; i++) {
      d0 += b_a_tmp[i3 + 9 * i] * D_tmp[i];
    }

    u[i3] = d0;
    for (i = 0; i < 9; i++) {
      d0 = 0.0;
      for (i4 = 0; i4 < 120; i4++) {
        d0 += b_a_tmp[i3 + 9 * i4] * D[i4 + 120 * i];
      }

      a_tmp[i3 + 9 * i] = d0;
    }
  }

  b_mldivide(a_tmp, u);

  /*  solution to the normal equations */
  /*  find the residual sum of errors */
  /*  chi2 = sum( ( 1 - ( D * u ) ./ d2 ).^2 ); % this chi2 is in the coordinate frame in which the ellipsoid is a unit sphere. */
  /*  find the ellipsoid parameters */
  /*  convert back to the conventional algebraic form */
  memset(&v[0], 0, 10U * sizeof(double));
  v[0] = (u[0] + u[1]) - 1.0;
  v[1] = (u[0] - 2.0 * u[1]) - 1.0;
  v[2] = (u[1] - 2.0 * u[0]) - 1.0;
  for (i = 0; i < 7; i++) {
    v[i + 3] = u[i + 2];
  }

  /*  if strcmp( equals, '' ) */
  /*      v(1) = u(1) +     u(2) - 1; */
  /*      v(2) = u(1) - 2 * u(2) - 1; */
  /*      v(3) = u(2) - 2 * u(1) - 1; */
  /*      v( 4 : 10 ) = u( 3 : 9 ); */
  /*  elseif strcmp( equals, 'xy' ) */
  /*      v(1) = u(1) - 1; */
  /*      v(2) = u(1) - 1; */
  /*      v(3) = -2 * u(1) - 1; */
  /*      v( 4 : 10 ) = u( 2 : 8 ); */
  /*  elseif strcmp( equals, 'xz' ) */
  /*      v(1) = u(1) - 1; */
  /*      v(2) = -2 * u(1) - 1; */
  /*      v(3) = u(1) - 1; */
  /*      v( 4 : 10 ) = u( 2 : 8 ); */
  /*  elseif strcmp( equals, '0' ) */
  /*      v(1) = u(1) +     u(2) - 1; */
  /*      v(2) = u(1) - 2 * u(2) - 1; */
  /*      v(3) = u(2) - 2 * u(1) - 1; */
  /*      v = [ v(1) v(2) v(3) 0 0 0 u( 3 : 6 )' ]; */
  /*  elseif strcmp( equals, '0xy' ) */
  /*      v(1) = u(1) - 1; */
  /*      v(2) = u(1) - 1; */
  /*      v(3) = -2 * u(1) - 1; */
  /*      v = [ v(1) v(2) v(3) 0 0 0 u( 2 : 5 )' ]; */
  /*  elseif strcmp( equals, '0xz' ) */
  /*      v(1) = u(1) - 1; */
  /*      v(2) = -2 * u(1) - 1; */
  /*      v(3) = u(1) - 1; */
  /*      v = [ v(1) v(2) v(3) 0 0 0 u( 2 : 5 )' ]; */
  /*  elseif strcmp( equals, 'xyz' ) */
  /*      v = [ -1 -1 -1 0 0 0 u( 1 : 4 )' ]; */
  /*  end */
  /*  form the algebraic form of the ellipsoid */
  A[0] = v[0];
  A[4] = v[3];
  A[8] = v[4];
  A[12] = v[6];
  A[1] = v[3];
  A[5] = v[1];
  A[9] = v[5];
  A[13] = v[7];
  A[2] = v[4];
  A[6] = v[5];
  A[10] = v[2];
  A[14] = v[8];
  A[3] = v[6];
  A[7] = v[7];
  A[11] = v[8];
  A[15] = v[9];

  /*  find the center of the ellipsoid */
  for (i3 = 0; i3 < 3; i3++) {
    i = i3 << 2;
    u[3 * i3] = -A[i];
    u[1 + 3 * i3] = -A[1 + i];
    u[2 + 3 * i3] = -A[2 + i];
  }

  mldivide(u, *(double (*)[3])&v[6], center);

  /*  form the corresponding translation matrix */
  for (i3 = 0; i3 < 16; i3++) {
    T[i3] = iv0[i3];
  }

  T[3] = center[0];
  T[7] = center[1];
  T[11] = center[2];

  /*  translate to the center */
  for (i3 = 0; i3 < 4; i3++) {
    for (i = 0; i < 4; i++) {
      i4 = i << 2;
      b_T[i3 + i4] = ((T[i3] * A[i4] + T[i3 + 4] * A[1 + i4]) + T[i3 + 8] * A[2
                      + i4]) + T[i3 + 12] * A[3 + i4];
    }
  }

  for (i3 = 0; i3 < 4; i3++) {
    for (i = 0; i < 4; i++) {
      A[i3 + (i << 2)] = ((b_T[i3] * T[i] + b_T[i3 + 4] * T[i + 4]) + b_T[i3 + 8]
                          * T[i + 8]) + b_T[i3 + 12] * T[i + 12];
    }
  }

  /*  solve the eigenproblem */
  for (i3 = 0; i3 < 3; i3++) {
    i = i3 << 2;
    u[3 * i3] = A[i] / -A[15];
    u[1 + 3 * i3] = A[1 + i] / -A[15];
    u[2 + 3 * i3] = A[2 + i] / -A[15];
  }

  eig(u, evecs, evals);
  b_diag(evals, sgns);
  b_sign(sgns);
  b_abs(evals, u);
  diag(u, dv0);
  dv0[0] = 1.0 / dv0[0];
  dv0[1] = 1.0 / dv0[1];
  dv0[2] = 1.0 / dv0[2];
  d_sqrt(dv0);
  radii[0].re = dv0[0] * sgns[0].re;
  radii[0].im = dv0[0] * sgns[0].im;
  radii[1].re = dv0[1] * sgns[1].re;
  radii[1].im = dv0[1] * sgns[1].im;
  radii[2].re = dv0[2] * sgns[2].re;
  radii[2].im = dv0[2] * sgns[2].im;

  /*  calculate difference of the fitted points from the actual data normalized by the conic radii */
  /*  shift data to origin */
  /*  rotate to cardinal axes of the conic; */
  /*  normalize to the conic radii */
  for (i3 = 0; i3 < 120; i3++) {
    b_mag_x[i3].re = mag_x[i3] - center[0];
    b_mag_x[i3].im = 0.0;
    b_mag_x[120 + i3].re = mag_y[i3] - center[1];
    b_mag_x[120 + i3].im = 0.0;
    b_mag_x[240 + i3].re = mag_z[i3] - center[2];
    b_mag_x[240 + i3].im = 0.0;
    for (i = 0; i < 3; i++) {
      i4 = i3 + 120 * i;
      b_d[i4].re = ((b_mag_x[i3].re * evecs[3 * i].re - b_mag_x[i3].im * evecs[3
                     * i].im) + (b_mag_x[i3 + 120].re * evecs[1 + 3 * i].re -
        b_mag_x[i3 + 120].im * evecs[1 + 3 * i].im)) + (b_mag_x[i3 + 240].re *
        evecs[2 + 3 * i].re - b_mag_x[i3 + 240].im * evecs[2 + 3 * i].im);
      b_d[i4].im = ((b_mag_x[i3].re * evecs[3 * i].im + b_mag_x[i3].im * evecs[3
                     * i].re) + (b_mag_x[i3 + 120].re * evecs[1 + 3 * i].im +
        b_mag_x[i3 + 120].im * evecs[1 + 3 * i].re)) + (b_mag_x[i3 + 240].re *
        evecs[2 + 3 * i].im + b_mag_x[i3 + 240].im * evecs[2 + 3 * i].re);
    }

    if (radii[0].im == 0.0) {
      if (b_d[i3].im == 0.0) {
        b_mag_x[i3].re = b_d[i3].re / radii[0].re;
        b_mag_x[i3].im = 0.0;
      } else if (b_d[i3].re == 0.0) {
        b_mag_x[i3].re = 0.0;
        b_mag_x[i3].im = b_d[i3].im / radii[0].re;
      } else {
        b_mag_x[i3].re = b_d[i3].re / radii[0].re;
        b_mag_x[i3].im = b_d[i3].im / radii[0].re;
      }
    } else if (radii[0].re == 0.0) {
      if (b_d[i3].re == 0.0) {
        b_mag_x[i3].re = b_d[i3].im / radii[0].im;
        b_mag_x[i3].im = 0.0;
      } else if (b_d[i3].im == 0.0) {
        b_mag_x[i3].re = 0.0;
        b_mag_x[i3].im = -(b_d[i3].re / radii[0].im);
      } else {
        b_mag_x[i3].re = b_d[i3].im / radii[0].im;
        b_mag_x[i3].im = -(b_d[i3].re / radii[0].im);
      }
    } else {
      brm = fabs(radii[0].re);
      bim = fabs(radii[0].im);
      if (brm > bim) {
        bim = radii[0].im / radii[0].re;
        d = radii[0].re + bim * radii[0].im;
        b_mag_x[i3].re = (b_d[i3].re + bim * b_d[i3].im) / d;
        b_mag_x[i3].im = (b_d[i3].im - bim * b_d[i3].re) / d;
      } else if (bim == brm) {
        if (radii[0].re > 0.0) {
          bim = 0.5;
        } else {
          bim = -0.5;
        }

        if (radii[0].im > 0.0) {
          d = 0.5;
        } else {
          d = -0.5;
        }

        b_mag_x[i3].re = (b_d[i3].re * bim + b_d[i3].im * d) / brm;
        b_mag_x[i3].im = (b_d[i3].im * bim - b_d[i3].re * d) / brm;
      } else {
        bim = radii[0].re / radii[0].im;
        d = radii[0].im + bim * radii[0].re;
        b_mag_x[i3].re = (bim * b_d[i3].re + b_d[i3].im) / d;
        b_mag_x[i3].im = (bim * b_d[i3].im - b_d[i3].re) / d;
      }
    }

    ar = b_d[120 + i3].re;
    ai = b_d[120 + i3].im;
    if (radii[1].im == 0.0) {
      if (ai == 0.0) {
        b_mag_x[120 + i3].re = ar / radii[1].re;
        b_mag_x[120 + i3].im = 0.0;
      } else if (ar == 0.0) {
        b_mag_x[120 + i3].re = 0.0;
        b_mag_x[120 + i3].im = ai / radii[1].re;
      } else {
        b_mag_x[120 + i3].re = ar / radii[1].re;
        b_mag_x[120 + i3].im = ai / radii[1].re;
      }
    } else if (radii[1].re == 0.0) {
      if (ar == 0.0) {
        b_mag_x[120 + i3].re = ai / radii[1].im;
        b_mag_x[120 + i3].im = 0.0;
      } else if (ai == 0.0) {
        b_mag_x[120 + i3].re = 0.0;
        b_mag_x[120 + i3].im = -(ar / radii[1].im);
      } else {
        b_mag_x[120 + i3].re = ai / radii[1].im;
        b_mag_x[120 + i3].im = -(ar / radii[1].im);
      }
    } else {
      brm = fabs(radii[1].re);
      bim = fabs(radii[1].im);
      if (brm > bim) {
        bim = radii[1].im / radii[1].re;
        d = radii[1].re + bim * radii[1].im;
        b_mag_x[120 + i3].re = (ar + bim * ai) / d;
        b_mag_x[120 + i3].im = (ai - bim * ar) / d;
      } else if (bim == brm) {
        if (radii[1].re > 0.0) {
          bim = 0.5;
        } else {
          bim = -0.5;
        }

        if (radii[1].im > 0.0) {
          d = 0.5;
        } else {
          d = -0.5;
        }

        b_mag_x[120 + i3].re = (ar * bim + ai * d) / brm;
        b_mag_x[120 + i3].im = (ai * bim - ar * d) / brm;
      } else {
        bim = radii[1].re / radii[1].im;
        d = radii[1].im + bim * radii[1].re;
        b_mag_x[120 + i3].re = (bim * ar + ai) / d;
        b_mag_x[120 + i3].im = (bim * ai - ar) / d;
      }
    }

    ar = b_d[240 + i3].re;
    ai = b_d[240 + i3].im;
    if (radii[2].im == 0.0) {
      if (ai == 0.0) {
        b_mag_x[240 + i3].re = ar / radii[2].re;
        b_mag_x[240 + i3].im = 0.0;
      } else if (ar == 0.0) {
        b_mag_x[240 + i3].re = 0.0;
        b_mag_x[240 + i3].im = ai / radii[2].re;
      } else {
        b_mag_x[240 + i3].re = ar / radii[2].re;
        b_mag_x[240 + i3].im = ai / radii[2].re;
      }
    } else if (radii[2].re == 0.0) {
      if (ar == 0.0) {
        b_mag_x[240 + i3].re = ai / radii[2].im;
        b_mag_x[240 + i3].im = 0.0;
      } else if (ai == 0.0) {
        b_mag_x[240 + i3].re = 0.0;
        b_mag_x[240 + i3].im = -(ar / radii[2].im);
      } else {
        b_mag_x[240 + i3].re = ai / radii[2].im;
        b_mag_x[240 + i3].im = -(ar / radii[2].im);
      }
    } else {
      brm = fabs(radii[2].re);
      bim = fabs(radii[2].im);
      if (brm > bim) {
        bim = radii[2].im / radii[2].re;
        d = radii[2].re + bim * radii[2].im;
        b_mag_x[240 + i3].re = (ar + bim * ai) / d;
        b_mag_x[240 + i3].im = (ai - bim * ar) / d;
      } else if (bim == brm) {
        if (radii[2].re > 0.0) {
          bim = 0.5;
        } else {
          bim = -0.5;
        }

        if (radii[2].im > 0.0) {
          d = 0.5;
        } else {
          d = -0.5;
        }

        b_mag_x[240 + i3].re = (ar * bim + ai * d) / brm;
        b_mag_x[240 + i3].im = (ai * bim - ar * d) / brm;
      } else {
        bim = radii[2].re / radii[2].im;
        d = radii[2].im + bim * radii[2].re;
        b_mag_x[240 + i3].re = (bim * ar + ai) / d;
        b_mag_x[240 + i3].im = (bim * ai - ar) / d;
      }
    }
  }

  power(b_mag_x, b_d);
  b_sgns[0].re = sgns[0].re;
  b_sgns[0].im = -sgns[0].im;
  b_sgns[1].re = sgns[1].re;
  b_sgns[1].im = -sgns[1].im;
  b_sgns[2].re = sgns[2].re;
  b_sgns[2].im = -sgns[2].im;
  repmat(b_sgns, b_mag_x);
  for (i3 = 0; i3 < 360; i3++) {
    bim = b_d[i3].re;
    b_d[i3].re = b_d[i3].re * b_mag_x[i3].re - b_d[i3].im * b_mag_x[i3].im;
    b_d[i3].im = bim * b_mag_x[i3].im + b_d[i3].im * b_mag_x[i3].re;
  }

  sum(b_d, dcv1);
  for (i3 = 0; i3 < 120; i3++) {
    dcv1[i3].re = 1.0 - dcv1[i3].re;
    dcv1[i3].im = 0.0 - dcv1[i3].im;
  }

  c_abs(dcv1, dv1);
  *chi2 = b_sum(dv1);
  if (fabs(v[9]) > 1.0E-6) {
    d0 = v[9];
    for (i3 = 0; i3 < 10; i3++) {
      v[i3] = -v[i3] / d0;
    }

    /*  normalize to the more conventional form with constant term = -1 */
  } else {
    d0 = v[9];
    c_sign(&d0);
    for (i3 = 0; i3 < 10; i3++) {
      v[i3] *= -d0;
    }
  }
}

/*
 * File trailer for ellipsoid_fit2.c
 *
 * [EOF]
 */
