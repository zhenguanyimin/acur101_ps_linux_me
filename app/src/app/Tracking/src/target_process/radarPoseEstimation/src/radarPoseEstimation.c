#include "../../tracking/include/tracking_int.h"
#include "../include/radarPoseEstimation.h"
#include "../../magCalibration4/magCalibration4.h"

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%测试用%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#if defined (DEBUG_LOG_GYRO)
sRadarPoseEstimation radarPostEstimation_malloc[1] = { 0 };
sMagCaliParam magCaliParam_malloc[1] = { 0 };
sMagCaliData magCaliData_malloc[1] = { 0 };
sGyroCaliParam gyroCaliParam_malloc[1] = {0};
sAccelCaliParam accelCaliParam_malloc[1] = {0};
//float gyroDataX[300] = { 0.21,-0.07,-0.07,0.07,0.14,-0.21,-0.14,0.07,0,-0.07,0.07,0.14,0.21,0,0,-0.07,0.07,-0.14,-0.07,0.21,-0.21,-0.07,-0.14,-0.42,-0.07,-0.07,0.21,-0.07,-0.21,-0.07,0.14,0.14,-0.21,-0.07,0,-0.28,0.07,0,-0.21,0.42,-0.07,0.35,-0.14,0.07,-0.07,-0.21,0.21,-0.35,-0.84,-1.26,-1.47,-2.73,-3.85,-4.69,-5.6,-7.21,-9.03,-10.36,-12.74,-14.14,-15.75,-17.5,-19.110001,-21.280001,-23.52,-25.059999,-27.09,-29.889999,-31.15,-31.92,-35,-36.68,-37.66,-40.040001,-42.490002,-44.66,-48.23,-56.98,-59.220001,-63,-66.989998,-68.18,-69.440002,-73.150002,-76.510002,-77.349998,-76.580002,-76.860001,-75.389999,-75.25,-74.830002,-74.269997,-71.889999,-69.860001,-68.949997,-68.599998,-66.639999,-66.010002,-65.940002,-65.660004,-63.419998,-61.110001,-60.619999,-61.740002,-62.509998,-60.970001,-61.18,-62.299999,-63.630001,-64.82,-66.080002,-70.839996,-74.690002,-74.690002,-73.57,-73.989998,-74.690002,-75.669998,-73.57,-74.059998,-77.419998,-79.309998,-79.309998,-80.360001,-77.489998,-72.589996,-69.720001,-65.870003,-61.599998,-56.279999,-52.150002,-50.119999,-48.580002,-48.23,-46.48,-42.77,-40.529999,-39.830002,-41.580002,-42,-38.43,-35.419998,-35.07,-35,-30.870001,-26.110001,-23.940001,-22.540001,-19.879999,-16.1,-13.23,-10.92,-9.1,-7.63,-8.96,-10.29,-7.98,-5.25,-5.81,-6.79,-5.46,-3.99,-2.66,-1.19,-0.42,0.07,0.77,0.35,0.21,0.28,-0.07,0.49,-0.56,-0.28,0.42,0.49,0,0.56,0.42,0.7,0.98,0.21,-0.56,0.63,-0.14,-0.42,0.07,0.7,0,-0.07,0.49,0.14,-0.42,0.28,0,0.14,0.07,0.35,0.14,-0.07,0.28,0,-0.35,0.21,0.07,-0.07,0.14,-0.21,0.07,0.28,0.07,0.21,0.21,0.7,0.56,0.28,0.63,1.82,1.82,1.89,1.4,1.33,1.68,2.03,1.26,0.14,0.07,-0.14,-0.14,-0.28,-0.49,-0.21,0.28,-0.14,-0.42,0.07,-0.21,-0.28,0.14,-0.21,0.21,0.14,-0.28,0.14,0,0.49,-0.21,0.07,0.28,0.28,0,0,-0.35,0.14,0.21,-0.14,0.07,0.14,-0.21,0.28,-0.07,-0.28,0.14,-0.14,-0.14,-0.28,-0.07,-0.35,-0.49,0,0.28,-0.21,-0.28,0.35,-0.35,0.07,-0.84,-0.07,0.07,-0.56,-0.56,0.49,0.28,-0.35,-0.42,0.49,-0.07,-0.7,0.14,0.56,-0.56,-0.07,0.56,-0.07,0.63,-1.12,0.56,0.49,-1.05,0.56 };
//float gyroDataY[300] = { -0.14,0.07,0.07,-0.35,0,-0.07,-0.21,0,0.14,0,-0.14,-0.07,0,0,-0.07,-0.21,-0.21,-0.07,-0.14,-0.07,0.21,-0.07,-0.35,-0.21,-0.28,-0.07,0.14,-0.14,-0.14,0,0.07,0.14,0.07,0.14,-0.21,0,-0.07,-0.49,-0.77,-0.91,-2.24,-2.87,-5.88,-8.33,-9.8,-13.02,-17.290001,-23.52,-29.469999,-40.669998,-49,-55.509998,-59.919998,-64.540001,-69.650002,-75.739998,-80.080002,-82.739998,-87.220001,-89.18,-91.349998,-93.449997,-96.32,-97.440002,-98.910004,-100.309998,-102.760002,-102.900002,-100.660004,-100.239998,-101.080002,-100.379997,-100.239998,-100.589996,-100.870003,-101.43,-106.540001,-114.099998,-115.849998,-117.949997,-118.230003,-116.059998,-116.480003,-117.529999,-118.440002,-117.459999,-113.959999,-108.360001,-105.279999,-102.339996,-99.470001,-95.690002,-91.559998,-87.57,-84.349998,-81.760002,-77.489998,-75.529999,-73.639999,-71.470001,-68.529999,-66.150002,-65.029999,-64.260002,-63.209999,-62.02,-61.110001,-60.830002,-60.900002,-61.32,-62.93,-64.75,-66.290001,-65.449997,-64.540001,-62.93,-62.860001,-61.529999,-60.689999,-60.689999,-61.040001,-61.950001,-61.950001,-60.41,-57.959999,-53.900002,-50.68,-48.16,-44.869999,-40.529999,-37.869999,-35.209999,-34.720001,-33.529999,-31.15,-28.77,-27.23,-27.58,-27.65,-26.530001,-24.709999,-23.52,-22.959999,-21.77,-18.690001,-16.870001,-15.12,-13.79,-11.83,-9.8,-8.19,-6.72,-5.32,-5.6,-5.6,-4.9,-4.9,-3.99,-3.64,-3.64,-3.36,-2.03,-1.19,-0.56,0.14,0.56,0.49,0.28,-0.14,0.14,0.42,0,-0.56,-0.14,0.14,0.14,0.21,0.35,0.14,0.35,0.07,0,-0.21,-0.28,-0.14,0.14,0.21,0,-0.21,-0.07,0.14,-0.28,-0.28,-0.21,-0.07,0.28,0.21,-0.07,-0.14,0.21,0.14,-0.14,-0.14,-0.35,-0.35,-0.07,-0.07,-0.14,0,0.21,-0.14,0.49,0.28,0.21,0,0.21,0.98,1.12,0.91,1.05,0.7,0.91,1.54,0.84,0.42,-0.35,-0.14,-0.42,0,-0.35,-0.14,-0.07,-0.21,-0.07,-0.14,0.07,0,-0.14,-0.21,-0.14,-0.42,-0.42,-0.35,-0.42,0,-0.28,0.14,-0.21,-0.14,-0.42,-0.21,0,-0.21,-0.28,-0.07,-0.28,-0.21,-0.21,-0.28,-0.28,-0.42,-0.49,-0.21,-0.21,-0.63,-0.14,-0.42,-0.21,-0.07,-0.14,-0.49,0.07,-0.28,-0.42,-0.07,-0.35,-0.21,-0.14,-0.21,-0.07,0.07,-0.07,-0.63,-0.35,0.07,-0.28,-0.21,-0.21,0.14,-0.28,-0.35,-0.35,-0.07,-0.28,-0.21,-0.14,-0.14,-0.28,0,-0.21 };
//float gyroDataZ[300] = { 0.07,0.7,-0.14,-0.07,0.42,0.49,0.21,-0.14,0.35,0.35,0.28,0.28,0.21,0.35,0.14,0.21,0,0.21,0.28,0.14,0.35,0.28,0.14,0.35,0.14,0.28,0,0.14,0,0.49,0.14,0.49,0.35,-0.07,0,0.28,0.28,0.49,1.33,1.68,1.89,1.89,3.57,5.67,7.56,10.01,12.32,16.52,21.559999,29.120001,33.459999,36.75,38.290001,38.990002,40.529999,43.049999,43.259998,43.889999,45.150002,45.009998,45.849998,46.619999,48.439999,49.98,50.68,51.029999,51.59,51.869999,51.099998,51.169998,51.380001,52.360001,52.709999,53.619999,55.09,55.23,55.299999,55.509998,55.509998,54.459999,53.900002,52.5,49.490002,47.459999,44.450001,41.580002,40.32,38.5,36.119999,34.580002,34.509998,33.740002,32.759998,31.01,31.639999,31.219999,29.33,28.77,27.790001,25.83,24.639999,23.799999,23.450001,24.01,23.309999,23.59,23.870001,24.639999,23.799999,25.200001,27.23,27.65,27.02,26.110001,25.690001,23.73,23.870001,21.49,21,20.370001,20.719999,21.700001,21.98,19.32,16.379999,14.98,13.58,13.23,11.9,12.18,11.9,12.32,13.51,14.91,13.93,12.74,10.85,12.04,13.37,11.41,10.36,9.94,9.73,9.24,7.07,6.58,5.88,6.02,5.04,4.97,4.69,4.76,3.22,4.06,4.9,3.92,3.43,2.73,3.43,4.13,3.29,2.73,3.01,3.64,3.22,2.66,3.64,4.2,3.92,3.71,3.22,3.08,2.45,1.61,1.47,2.1,2.31,2.45,2.66,2.45,1.4,0.7,0.35,0.21,0,0.56,0.7,0.84,1.47,1.19,0.77,0.28,0.21,-0.21,0.35,0.07,0.14,0.28,0.07,-0.07,0.07,0,-0.35,-0.28,-0.14,0.28,0.07,-0.28,0.14,0.07,-0.07,0.21,0.07,0.28,0.21,0.35,0.07,-0.21,0.28,0.28,0,-0.21,-0.42,0.14,0.42,0.21,0.28,0.21,0.14,0.21,0.28,0.07,0.35,0.28,0,0.28,0.14,0.42,0.28,0,0.56,0.42,0.07,0.42,0,0.35,0.42,0.28,0.42,0.56,0.56,-0.07,0.28,0.35,0,0.35,0.07,0.21,0.14,0.56,0.28,0.28,0.42,0.49,-0.14,0.14,0.14,-0.07,0.49,0.49,0.49,0.28,0.28,0.42,0.21,0.14,0.35,0.14,0.07,0.21,0.07,0.07,0,0.63,0.42,0.28,0.21,-0.07,0.49,0.14,0.14,0.21,0.28,0,0.56,0,0.42,0.07,-0.14,0.28 };
float gyroDataX[300] = { 0.07,-0.21,0.07,0.21,-0.28,0.21,-0.35,-0.21,0.35,-0.35,-0.14,0.07,0.14,-0.14,0.07,0,-0.21,-0.21,0.21,-0.28,-0.07,0,-0.14,0.07,0.14,-0.49,-0.42,0.21,-0.35,0.07,-0.14,-0.28,0,-0.35,-0.14,-0.07,0,0,-0.35,0,0.07,0.28,0.28,-0.14,-0.07,0.42,0.14,0.28,0.14,0.28,0.7,0.07,0.07,0.07,-0.49,-0.77,-0.7,-1.12,-0.56,-0.35,-0.84,-0.14,-0.42,-1.4,-0.7,-0.14,-1.26,-0.63,-0.56,-1.33,-0.42,-0.35,-1.26,-0.84,-0.49,-1.19,-0.91,-0.49,-1.12,-0.63,-0.49,-0.91,-0.42,-0.14,-0.42,-1.05,-0.91,-1.19,-2.73,-2.94,-6.79,-13.44,-21.629999,-32.689999,-50.470001,-53.759998,-55.650002,-59.779999,-61.25,-60.900002,-63.560001,-70.910004,-79.730003,-87.010002,-94.43,-98.489998,-99.470001,-100.379997,-103.879997,-104.720001,-102.129997,-100.519997,-97.019997,-96.25,-98.839996,-95.339996,-94.220001,-94.360001,-91.349998,-90.790001,-91.839996,-89.18,-87.220001,-85.330002,-83.93,-80.919998,-78.190002,-78.400002,-77.349998,-73.709999,-70.349998,-68.669998,-65.449997,-62.720001,-61.040001,-59.43,-56.349998,-54.18,-53.900002,-53.130001,-50.959999,-48.299999,-45.5,-42.279999,-38.849998,-34.02,-29.68,-28.42,-27.23,-22.959999,-20.16,-20.440001,-21.280001,-23.66,-25.27,-26.530001,-27.02,-28.98,-32.200001,-33.040001,-32.48,-30.030001,-26.879999,-26.879999,-25.969999,-22.540001,-22.68,-22.4,-19.6,-20.860001,-19.389999,-14.91,-13.3,-10.71,-7.14,-6.09,-5.32,-4.9,-4.69,-3.15,-2.8,-2.66,-3.71,-5.32,-5.53,-4.97,-4.13,-3.29,-3.29,-1.82,-2.31,-2.31,-0.56,0.28,-0.14,-0.63,-0.42,0,0.14,-0.07,-0.21,0.7,0.14,-0.35,-0.14,0.14,0.07,-0.14,0.07,0.14,-0.14,-0.14,0.42,0.21,0.21,0.42,0.49,0.77,-0.07,0.42,0.56,0.42,0.28,0.07,0.63,0.35,-0.14,0.84,0.07,-0.21,0.42,0.35,0.49,0.21,-0.07,0,0.21,-0.14,0.35,0.21,-0.7,-0.28,0.84,-0.07,-0.42,0.07,0.28,0.35,-0.56,-0.7,0.77,0.56,-0.49,-0.35,0.28,0.21,-0.07,-0.07,-0.07,-0.14,0.42,0,-0.35,-0.35,-0.42,0.35,0.21,-0.7,0.14,0.98,-0.63,-0.56,0.49,0.14,-0.14,-0.14,0,0.07,-0.42,0.14,0.14,-0.56,-0.14,0.14,0,-0.28,-0.07,0,0,-0.21,-0.07,-0.07,0.07,-0.35,-0.14,0.35,-0.14,-0.28,0.28,0.21 };
float gyroDataY[300] = { -0.14,-0.21,0.28,0,0.14,0.07,-0.14,-0.49,-0.35,-0.35,-0.07,-0.28,-0.14,-0.14,0.07,-0.07,0.28,-0.07,-0.21,-0.56,-0.28,0,0,-0.21,-0.42,0.07,-0.35,-0.35,0,-0.21,-0.07,-0.07,-0.07,-0.14,-0.14,-0.21,0.14,0,-0.21,0,0.28,0.14,-0.07,0.56,1.47,1.05,0.7,1.05,2.52,3.43,2.94,1.75,2.38,1.89,0.28,-0.98,-1.61,-1.47,-0.49,0.28,0.7,0.98,0.63,0.35,0.63,0.63,0.28,0.07,0.14,0.07,0.28,0.91,0.21,-0.42,0.28,-0.14,-0.49,-0.42,-0.28,0.49,0.21,0.42,0.63,0.35,-0.28,-0.98,-1.68,-2.38,-4.13,-7.28,-13.51,-24.15,-38.080002,-55.860001,-79.660004,-87.709999,-93.309998,-97.650002,-98.699997,-101.43,-106.260002,-116.760002,-131.179993,-142.100006,-156.589996,-160.300003,-162.259995,-167.580002,-171.149994,-171.080002,-169.119995,-165.130005,-161.139999,-160.649994,-160.300003,-159.25,-157.850006,-153.929993,-152.740005,-151.199997,-150.080002,-147.910004,-144.619995,-142.169998,-138.25,-134.470001,-131.880005,-129.990005,-127.470001,-122.5,-117.669998,-113.610001,-108.150002,-104.510002,-101.290001,-97.720001,-93.379997,-90.440002,-89.459999,-87.360001,-84.419998,-80.849998,-75.879997,-70.139999,-63.91,-56.349998,-50.540001,-47.740002,-43.400002,-38.779999,-35.349998,-34.16,-36.049999,-38.849998,-42,-43.540001,-45.290001,-49.209999,-52.919998,-54.110001,-53.830002,-49.07,-46.48,-44.73,-41.720001,-39.549999,-37.52,-35.98,-35.139999,-34.580002,-30.66,-26.18,-21,-16.799999,-12.46,-10.29,-8.82,-8.05,-7.56,-5.6,-3.78,-5.53,-7.28,-8.19,-8.75,-7.98,-6.93,-5.67,-4.06,-4.2,-3.43,-2.38,-0.63,-0.28,-0.84,-1.12,-0.77,0.21,0.28,0.42,0.28,0,-0.07,-0.28,-0.07,-0.14,0,0.14,-0.07,-0.21,0.42,0.42,0.21,0.49,0.14,0.63,0.84,0.77,0.49,0.49,0.28,0.42,0.28,0.42,0.35,0.56,0.21,0.14,0.49,0.49,0.14,0.35,0.14,0.49,0.28,0.35,-0.14,0.14,0.35,0.07,0,-0.07,0.14,0,-0.14,0.42,-0.35,0.14,0.14,-0.28,0.14,-0.28,0,0,0,0.14,0.49,-0.07,-0.07,0.14,-0.14,0.07,-0.56,-0.28,0.07,-0.21,0.21,0.14,0.28,-0.07,-0.21,-0.28,-0.07,-0.35,-0.28,-0.21,-0.07,-0.28,0.21,-0.49,0,-0.35,-0.07,0,-0.28,-0.21,-0.28,0.07,-0.21,-0.07,-0.28,0,-0.21,-0.14,0.07,-0.28,-0.07,-0.28,0,0 };
float gyroDataZ[300] = { -0.35,0.28,0.21,0.56,0.56,0.49,0.14,0.21,0,0.07,0.14,0.21,0.21,0.14,0.28,0.14,0.07,0.07,0.35,0.21,0.14,0.35,0.07,0.14,0.28,0.07,0.21,0,0.21,0.21,0.14,0.21,-0.21,0.49,0.21,0,-0.07,0.35,0.35,0.14,0.56,0.42,0.7,0.42,0.56,0.77,1.19,2.45,3.15,4.34,6.86,14.07,25.059999,32.34,37.450001,43.259998,46.619999,48.580002,49.07,49.560001,51.869999,54.950001,56.209999,57.610001,57.82,56.209999,55.93,55.299999,54.459999,52.360001,50.330002,48.720001,46.48,45.080002,43.75,42.630001,42.84,41.93,40.740002,40.740002,39.619999,37.380001,33.459999,31.639999,31.43,26.950001,21.559999,14.63,8.82,3.71,0.28,-1.75,-1.26,0.28,3.43,2.8,2.94,3.64,2.8,2.38,1.61,1.05,1.68,2.24,1.96,2.31,0.91,1.26,1.12,1.4,1.26,1.96,2.24,2.03,1.75,2.03,2.31,1.96,2.38,1.89,1.54,2.24,0.98,1.68,2.24,1.82,2.17,1.89,1.54,1.68,1.82,2.52,1.89,1.82,1.61,1.75,1.75,1.26,1.68,0.77,0.7,0.84,0.42,0.84,0.28,0.77,0.42,1.47,1.33,0.28,0.35,0.49,1.4,0.98,1.19,0.98,0.21,1.26,1.33,1.05,-0.21,-0.91,-0.28,-0.07,-0.21,0.42,0.56,0.14,1.26,1.47,0.56,-0.35,-0.84,-0.21,-0.07,0.56,0.14,0.35,0.56,-0.14,0.14,0.63,0.84,0.35,0.35,-0.28,-0.14,0,0,0.49,0.14,0.07,0.21,0.14,0.42,0.07,0,-0.21,0.28,0,0.42,0.35,-0.07,0.07,0.07,0.14,0.42,0.14,0.42,0.14,0.49,0.42,0.14,0.42,0,0.49,0.84,0.84,0.49,0.56,0.49,0.7,0.35,0.77,0.35,0.21,0.49,0.14,0.42,0.07,0.35,0.49,0.21,0.35,0.56,0.07,0.56,0.07,0.49,0.63,-0.07,0.35,0.7,0.14,0.35,-0.35,0.63,0.42,-0.07,0.14,0,0.56,0.63,0.07,0.14,0.21,0.28,0.21,-0.21,0.42,0.14,0.42,0.63,0.14,-0.14,0.35,0.21,-0.35,0.14,0.28,0,0.21,0.42,0.35,0.28,0.35,0.21,0.28,0.28,0.42,0.07,0.35,0.14,0.07,0.21,0,0.49,0.35,0.14,0.35,0.35,0.21,-0.14,0.42,0,0.14,-0.14,0.21,0.07,-0.14};
int data_idx = 0;
int initialFrame = 1;
#endif
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
static float accelObs[60] = { 0 };
static float accelObsX[60] = { 0 };
static float accelObsY[60] = { 0 };
static float accelObsZ[60] = { 0 };
static float gyroReadingLast[3] = { 0 };
static float accelReadingLast[3] = { 0 };
static float magReadingLast[3] = { 0 };
static int accelObsIdx = 0;

//%%%%%%%%%%%%%%%%%%%%%%%%%INTERFACE%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void calcMagCaliParam(sRadarPoseEstimation* radarPoseEstimation_inst)
{
	double mag_x[120] = { 0 }; //为double计算开辟数组
	double mag_y[120] = { 0 };
	double mag_z[120] = { 0 };
	double GT_mag = 0;

	int* magx_int = radarPoseEstimation_inst->magCaliData->mag_x;
	int* magy_int = radarPoseEstimation_inst->magCaliData->mag_y;
	int* magz_int = radarPoseEstimation_inst->magCaliData->mag_z;

	float* softIron = radarPoseEstimation_inst->magCaliParam->softIron;
	float* hardIron = radarPoseEstimation_inst->magCaliParam->hardIron;

	double softIron_d[9] = { 0 }; //校准函数输出为double型
	double hardIron_d[3] = { 0 };
	creal_T C[9] = { 0 };
	int ii = 0;

	GT_mag = radarPoseEstimation_inst->magCaliData->GT_mag / MAGDATASCALE;
	for (ii = 0; ii < 120; ii++)
	{
		mag_x[ii] = magx_int[ii] / MAGDATASCALE;
		mag_y[ii] = magy_int[ii] / MAGDATASCALE;
		mag_z[ii] = magz_int[ii] / MAGDATASCALE;
	}
	magCalibration4(mag_x, mag_y, mag_z, GT_mag, C, hardIron_d, &(radarPoseEstimation_inst->magCaliParam->chi2));
	for (ii = 0; ii < 9; ii++)
	{
		softIron_d[ii] = C[ii].re;
		softIron[ii] = (float)softIron_d[ii];
	}
	for (ii = 0; ii < 3; ii++)
	{
		hardIron[ii] = (float)hardIron_d[ii];
	}
}


void calcRadarPose(sRadarPoseEstimation* radarPoseEstimation_inst, float g, sAlgObjData* algObj, float dt)
{
	//float g = -980.f;
	float pitch = 0.f;
	float roll = 0.f;
	float yaw = 0.f;
	float pitch_mat[9] = { 0 };
	float roll_mat[9] = { 0 };
	float R_mag[9] = { 0 };
	float mag_hori[3] = { 0 };
	float mag_calibrated[3] = { 0 };
	float ax = (float)(radarPoseEstimation_inst->a[0] / ACCELDATASCALE), ay = (float)(radarPoseEstimation_inst->a[1] / ACCELDATASCALE), az = (float)(radarPoseEstimation_inst->a[2] / ACCELDATASCALE);
	float mag_x = (float)(radarPoseEstimation_inst->mag[0] / MAGDATASCALE), mag_y = (float)(radarPoseEstimation_inst->mag[1] / MAGDATASCALE), mag_z = (float)(radarPoseEstimation_inst->mag[2] / MAGDATASCALE);
	float wx = (float)radarPoseEstimation_inst->gyro[0], wy = (float)radarPoseEstimation_inst->gyro[1], wz = (float)radarPoseEstimation_inst->gyro[2];
	float mag_raw[3] = { 0 }; // hardiron消除后的磁场强度

	/*sPlatformInfo* platformInfo = algObj->platformInfo;
	sPlatformData* platFormData = &platformInfo->platformData;*/
	//float pitching = (float)platFormData->pitching * INV_ONE15FORMAT_SCALE;//经测试，这个变量每帧都会清零
	//float rolling = (float)platFormData->rolling * INV_ONE15FORMAT_SCALE;//经测试，这个变量每帧都会清零
	//float heading = (float)platFormData->heading * INV_ONE15FORMAT_SCALE;//经测试，这个变量每帧都会清零
	float pitching = radarPoseEstimation_inst->pitch;
	float rolling = radarPoseEstimation_inst->roll;
	float heading = radarPoseEstimation_inst->yaw;
	float cosPitch = cosf(pitching);
	float sinPitch = sinf(pitching);
	float cosYaw = cosf(heading);
	float sinYaw = sinf(heading);
	float cosRoll = cosf(rolling);
	float sinRoll = sinf(rolling);
	float tanPitch = tanf(pitching);
	int ii = 0;
	float rotMatrix[TRACKING_DIMENSION * TRACKING_DIMENSION] = { 0.f };
	float gyroReading[3] = { 0 };
	float gyroReadingGlobal[3] = { 0 };
	float rollRate = 0, pitchRate = 0, yawRate = 0;
	float magnitude_a = 0.f;
	float axMean = 0.f, ayMean = 0.f, azMean = 0.f;
	float axVar = 0.f, ayVar = 0.f, azVar = 0.f;

	////计算功能验证%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	//ax = 4.9f;
	//ay = -7.35f;
	//az = 4.2435f;
	///*mag_x = -2.6762f;
	//mag_y = 3.4172f;
	//mag_z = 9.0089f;*/
	//mag_x = -2.6762f+3;//加入hard-iron error
	//mag_y = 3.4172f+6;
	//mag_z = 9.0089f+10;

	//magCaliParam->hardIron[0] = 3;
	//magCaliParam->hardIron[1] = 6;
	//magCaliParam->hardIron[2] = 10;
	//mag_x -= magCaliParam->hardIron[0];
	//mag_y -= magCaliParam->hardIron[1];
	//mag_z -= magCaliParam->hardIron[2];
	////%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	//计算功能验证%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	//ax = 4.9f;
	//ay = -7.35f;
	//az = 4.2435f;
	//mag_x = 382.f;
	//mag_y = 831.f;
	//mag_z = 540.f;

	//ax = 31.f;
	//ay = -93.f;
	//az = -937.f;
	//mag_x = 369.f;
	//mag_y = 115.f;
	//mag_z = 345.f;
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	//磁力计数据平滑

	mag_x = 0.5 * mag_x + 0.5 * magReadingLast[0];
	mag_y = 0.5 * mag_y + 0.5 * magReadingLast[1];
	mag_z = 0.5 * mag_z + 0.5 * magReadingLast[2];
	magReadingLast[0] = mag_x;
	magReadingLast[1] = mag_y;
	magReadingLast[2] = mag_z;

	ax = radarPoseEstimation_inst->accelCaliParam->scale_x * (ax - radarPoseEstimation_inst->accelCaliParam->bias_x);//scalex的值初始化为1
	ay = radarPoseEstimation_inst->accelCaliParam->scale_y * (ay - radarPoseEstimation_inst->accelCaliParam->bias_y);
	az = radarPoseEstimation_inst->accelCaliParam->scale_z * (az - radarPoseEstimation_inst->accelCaliParam->bias_z);
	ax = 0.5 * ax + 0.5 * accelReadingLast[0];
	ay = 0.5 * ay + 0.5 * accelReadingLast[1];
	az = 0.5 * az + 0.5 * accelReadingLast[2];
	accelReadingLast[0] = ax;
	accelReadingLast[1] = ay;
	accelReadingLast[2] = az;
	magnitude_a = sqrtf(ax * ax + ay * ay + az * az);

	radarPoseEstimation_inst->movingFlag = 0;
	accelObs[accelObsIdx] = magnitude_a;
	accelObsX[accelObsIdx] = ax;
	accelObsY[accelObsIdx] = ay;
	accelObsZ[accelObsIdx] = az;
	accelObsIdx++;
	if (accelObsIdx > 59)
		accelObsIdx = 0;
	for (ii = 0; ii < 60; ii++)
	{
		if (accelObs[ii] > g+30.f || accelObs[ii] < g-30.f)
		{
			radarPoseEstimation_inst->movingFlag = 1;
			break;
		}
	}

	//计算加速度3轴方差
	for (ii = 0; ii < 60; ii++)
	{
		axMean += accelObsX[ii];
		ayMean += accelObsY[ii];
		azMean += accelObsZ[ii];
	}
	axMean /= 60.f;
	ayMean /= 60.f;
	azMean /= 60.f;
	for (ii = 0; ii < 60; ii++)
	{
		axVar += (accelObsX[ii] - axMean) * (accelObsX[ii] - axMean);
		ayVar += (accelObsY[ii] - ayMean) * (accelObsY[ii] - ayMean);
		azVar += (accelObsZ[ii] - azMean) * (accelObsZ[ii] - azMean);
	}
	axVar /= 60.f;
	ayVar /= 60.f;
	azVar /= 60.f;
	if (axVar > 200.f || ayVar > 200.f || azVar > 200.f)
		radarPoseEstimation_inst->movingFlag = 1;

	if (fabsf(wx)>3000.f || fabsf(wy) > 3000.f || fabsf(wz) > 3000.f) //陀螺仪判断运动状态
		radarPoseEstimation_inst->movingFlag = 1;

	if (radarPoseEstimation_inst->movingFlag == 0)
	{
		//用校准参数校正磁力计数据
		mag_raw[0] = mag_x - radarPoseEstimation_inst->magCaliParam->hardIron[0];//消除hardiron
		mag_raw[1] = mag_y - radarPoseEstimation_inst->magCaliParam->hardIron[1];
		mag_raw[2] = mag_z - radarPoseEstimation_inst->magCaliParam->hardIron[2];

		tracking_matrixMultiply(3, 3, 1, radarPoseEstimation_inst->magCaliParam->softIron, mag_raw, mag_calibrated);//消除softiron

		if (ax > g)
			ax = g - 1;
		else if(ax < -g)
			ax = -g + 1;

		pitch = asinf(-1 * ax / g); // g = +9.8
		if (fabsf(cosf(pitch)) > 0.05)
			roll = atan2f(ay / cosf(pitch) / g, az / cosf(pitch) / g); //需要把左手系传感器的y轴数据加上负号，原始数据就加
		else
			roll = 0.f;

		//pitch_mat = [cosf(pitch), 0, sinf(pitch), 0, 1, 0, -1*sin(pitch), 0, cos(pitch)];
		pitch_mat[0] = cosf(pitch);
		pitch_mat[2] = sinf(pitch);
		pitch_mat[4] = 1;
		pitch_mat[6] = -1 * sinf(pitch);
		pitch_mat[8] = cosf(pitch);

		//roll_mat = [1, 0, 0, 0, cos(roll), -1 * sin(roll), 0, sin(roll), cos(roll)];
		roll_mat[0] = 1;
		roll_mat[4] = cosf(roll);
		roll_mat[5] = -1 * sinf(roll);
		roll_mat[7] = sinf(roll);
		roll_mat[8] = cosf(roll);


		tracking_matrixMultiply(3, 3, 3, pitch_mat, roll_mat, R_mag);
		tracking_matrixMultiply(3, 3, 1, R_mag, mag_calibrated, mag_hori);
		yaw = -1 * atan2f(mag_hori[1], mag_hori[0]); //需要把左手系传感器的y轴数据加上负号，原始数据就加

		/*printf("magCaliParam->hardIron: %.3f %.3f %.3f\n", magCaliParam->hardIron[0], magCaliParam->hardIron[1], magCaliParam->hardIron[2]);
		printf("mag_hori: %.3f %.3f %.3f\n", mag_hori[0], mag_hori[1], mag_hori[2]);
		printf("yaw: %.3f pitch: %.3f roll: %.3f\n", yaw, pitch, roll);
		printf("mag calibrated: %.3f, %.3f, %.3f", mag_calibrated[0], mag_calibrated[1], mag_calibrated[2]);*/

		radarPoseEstimation_inst->yaw -= radarPoseEstimation_inst->magnetic_declination;
		radarPoseEstimation_inst->yaw = yaw;
		radarPoseEstimation_inst->pitch = pitch;
		radarPoseEstimation_inst->roll = roll;
	}
	else
	{
		rotMatrix[0] = cosPitch * cosYaw;
		rotMatrix[1] = -sinYaw * cosRoll + cosYaw * sinPitch * sinRoll;
		rotMatrix[2] = sinYaw * sinRoll + cosYaw * sinPitch * cosRoll;
		rotMatrix[3] = sinYaw * cosPitch;
		rotMatrix[4] = cosYaw * cosRoll + sinYaw * sinPitch * sinRoll;
		rotMatrix[5] = -cosYaw * sinRoll + sinYaw * sinPitch * cosRoll;
		rotMatrix[6] = -sinPitch;
		rotMatrix[7] = cosPitch * sinRoll;
		rotMatrix[8] = cosPitch * cosRoll;

		gyroReading[0] = (radarPoseEstimation_inst->gyro[0] - radarPoseEstimation_inst->gyroCaliParam->bias_x) / 1000.f * DEG2RAD;
		gyroReading[1] = (radarPoseEstimation_inst->gyro[1] - radarPoseEstimation_inst->gyroCaliParam->bias_y) / 1000.f * DEG2RAD;
		gyroReading[2] = (radarPoseEstimation_inst->gyro[2] - radarPoseEstimation_inst->gyroCaliParam->bias_z) / 1000.f * DEG2RAD;
#if defined (DEBUG_LOG_GYRO)
		gyroReading[0] = gyroDataX[data_idx] * DEG2RAD;
		gyroReading[1] = gyroDataY[data_idx] * DEG2RAD;
		gyroReading[2] = gyroDataZ[data_idx] * DEG2RAD;
		data_idx++;
#endif

		tracking_matrixMultiply(TRACKING_DIMENSION, TRACKING_DIMENSION, 1, rotMatrix, gyroReading, gyroReadingGlobal);

		//gyroReadingGlobal[0] = 0.5 * gyroReadingGlobal[0] + 0.5 * gyroReadingLast[0]; //改为在地球坐标系下低通滤波
		//gyroReadingGlobal[1] = 0.5 * gyroReadingGlobal[1] + 0.5 * gyroReadingLast[1];
		//gyroReadingGlobal[2] = 0.5 * gyroReadingGlobal[2] + 0.5 * gyroReadingLast[2];
		//gyroReadingLast[0] = gyroReadingGlobal[0];
		//gyroReadingLast[1] = gyroReadingGlobal[1];
		//gyroReadingLast[2] = gyroReadingGlobal[2];

		//radarPoseEstimation_inst->roll += gyroReadingGlobal[0] * dt;
		//radarPoseEstimation_inst->pitch += gyroReadingGlobal[1] * dt;
		//radarPoseEstimation_inst->yaw += gyroReadingGlobal[2] * dt;
		if (fabsf(cosf(pitch)) > 0.05)
		{
			radarPoseEstimation_inst->roll += (gyroReadingGlobal[0] * cosYaw + gyroReadingGlobal[1] * sinYaw) / cosPitch * dt;
			radarPoseEstimation_inst->pitch += (-gyroReadingGlobal[0] * sinYaw + gyroReadingGlobal[1] * cosYaw) * dt;
			radarPoseEstimation_inst->yaw += (gyroReadingGlobal[2] + (gyroReadingGlobal[0] * cosYaw + gyroReadingGlobal[1] * sinYaw) * tanPitch) * dt;
		}
		else
		{
			radarPoseEstimation_inst->pitch += -gyroReadingGlobal[0] * sinYaw + gyroReadingGlobal[1] * cosYaw;
			radarPoseEstimation_inst->yaw += gyroReadingGlobal[2];
		}
	}

	if (radarPoseEstimation_inst->roll > 180.f * DEG2RAD)
		radarPoseEstimation_inst->roll -= 360.f * DEG2RAD;
	else if (radarPoseEstimation_inst->roll < -180.f * DEG2RAD)
		radarPoseEstimation_inst->roll += 360.f * DEG2RAD;
	if (radarPoseEstimation_inst->pitch > 180.f * DEG2RAD)
		radarPoseEstimation_inst->pitch -= 360.f * DEG2RAD;
	else if (radarPoseEstimation_inst->pitch < -180.f * DEG2RAD)
		radarPoseEstimation_inst->pitch += 360.f * DEG2RAD;
	if (radarPoseEstimation_inst->yaw > 180.f * DEG2RAD)
		radarPoseEstimation_inst->yaw -= 360.f * DEG2RAD;
	else if (radarPoseEstimation_inst->yaw < -180.f * DEG2RAD)
		radarPoseEstimation_inst->yaw += 360.f * DEG2RAD;
}

void passYPR2algo(sAlgObjData* algObj, sRadarPoseEstimation* radarPoseEstimation_inst)
{
	//sPlatformData* platformData = &(algObj->platformInfo-s>platformData);
	algObj->platformInfo->platformData.heading = radarPoseEstimation_inst->yaw * ONE15FORMAT_SCALE * RAD2DEG;
	algObj->platformInfo->platformData.pitching = radarPoseEstimation_inst->pitch * ONE15FORMAT_SCALE * RAD2DEG;
	algObj->platformInfo->platformData.rolling = radarPoseEstimation_inst->roll * ONE15FORMAT_SCALE * RAD2DEG;
}
#if defined (DEBUG_LOG_GYRO)
void testRadarPoseEstimation(sAlgObjData* algObj)
{
	sRadarPoseEstimation* radarPoseEstimation_inst = radarPostEstimation_malloc;
	int ii = 0;
	int magx_exam[120] = { 478,456,480,490,487,423,456,447,442,451,481,441,432,442,457,481,472,399,220,313,376,198,-30,-12,163,160,216,43,-90,145,325,226,87,193,403,453,411,304,253,256,198,7,25,157,123,183,309,375,361,378,507,496,507,502,454,315,138,103,142,106,49,57,153,103,180,310,348,417,448,390,417,514,487,475,492,390,453,462,354,208,-61,-30,19,42,303,276,324,282,301,489,462,486,502,441,240,285,430,430,214,-39,-90,132,238,271,33,-139,-142,76,267,63,-141,-112,24,231,178,-49,19,346,450,355 };
	int magy_exam[120] = { 658,729,637,486,588,754,709,499,568,630,625,435,571,742,715,508,505,759,882,819,658,691,709,684,514,346,300,265,309,286,334,241,237,234,352,448,393,309,240,279,325,297,277,580,864,766,556,667,867,837,592,472,507,538,466,268,234,226,591,541,361,349,586,852,826,654,448,511,756,844,796,612,528,490,514,711,726,646,432,601,760,489,264,217,348,568,345,274,345,459,654,615,549,781,918,780,597,633,846,865,642,480,442,487,598,480,472,448,411,612,688,532,445,450,616,742,759,597,663,871 };
	int magz_exam[120] = { 513,492,408,459,531,393,444,624,544,340,415,592,666,475,417,439,585,642,669,690,726,819,801,820,841,766,714,663,666,697,658,565,400,426,447,424,382,346,427,645,732,693,688,826,730,790,798,736,514,433,481,480,505,498,540,448,348,577,852,849,783,777,852,741,760,787,757,717,579,433,537,490,364,343,514,690,615,634,745,834,768,802,664,612,711,802,634,358,268,471,616,555,403,514,561,747,699,699,720,660,784,837,808,808,844,757,744,829,786,828,745,778,823,814,841,771,781,771,666,519 };
#if defined (DEBUG_LOG_GYRO)
	FILE* fp1 = NULL;
#endif
	if (initialFrame)
	{
		/*radarPoseEstimation_inst->roll = 91.f*DEG2RAD;
		radarPoseEstimation_inst->pitch = 1.41f * DEG2RAD;
		radarPoseEstimation_inst->yaw = 1.41f * DEG2RAD;*/
		radarPoseEstimation_inst->roll = 91.2f*DEG2RAD;
		radarPoseEstimation_inst->pitch = 0.53f * DEG2RAD;
		radarPoseEstimation_inst->yaw = 3.72f * DEG2RAD;
		initialFrame = 0;
	}
	radarPoseEstimation_inst->magCaliParam = magCaliParam_malloc;
	radarPoseEstimation_inst->magCaliData = magCaliData_malloc;
	radarPoseEstimation_inst->accelCaliParam = accelCaliParam_malloc;
	radarPoseEstimation_inst->gyroCaliParam = gyroCaliParam_malloc;

	for (ii = 0; ii < 120; ii++)
	{
		radarPoseEstimation_inst->magCaliData->mag_x[ii] = magx_exam[ii];
		radarPoseEstimation_inst->magCaliData->mag_y[ii] = magy_exam[ii];
		radarPoseEstimation_inst->magCaliData->mag_z[ii] = magz_exam[ii];
	}
	radarPoseEstimation_inst->magCaliData->GT_mag = 430;
	radarPoseEstimation_inst->a[0] = 490;
	radarPoseEstimation_inst->a[1] = -735;
	radarPoseEstimation_inst->a[2] = 424;
	radarPoseEstimation_inst->mag[0] = 382;
	radarPoseEstimation_inst->mag[1] = 831;
	radarPoseEstimation_inst->mag[2] = 540;

	//验证陀螺仪
	radarPoseEstimation_inst->a[0] = 1000;
	radarPoseEstimation_inst->a[1] = 500;
	radarPoseEstimation_inst->a[2] = 0;

	calcMagCaliParam(radarPoseEstimation_inst);
	calcRadarPose(radarPoseEstimation_inst, 980.f, algObj, 0.02f);
	passYPR2algo(algObj, radarPoseEstimation_inst);
	
	
#if defined (DEBUG_LOG_GYRO)
	fopen_s(&fp1, "E:/record/20230222/record2.txt", "a+");
	fprintf(fp1, "%.3f	%.3f	%.3f	%d\n", radarPoseEstimation_inst->roll * RAD2DEG, radarPoseEstimation_inst->pitch * RAD2DEG, radarPoseEstimation_inst->yaw * RAD2DEG, data_idx);
	fclose(fp1);
#endif
}
#endif

void calcGyroCaliParam(sGyroCaliData* gyroCaliData, sRadarPoseEstimation* radarPoseEstimation_inst)
{
	int ii = 0;
	float bias_x = 0;
	float bias_y = 0;
	float bias_z = 0;
	for (ii = 0; ii < 120; ii++)
	{
		bias_x = bias_x + (gyroCaliData->angVel_x[ii] - bias_x) / (ii + 1);
		bias_y = bias_y + (gyroCaliData->angVel_y[ii] - bias_y) / (ii + 1);
		bias_z = bias_z + (gyroCaliData->angVel_z[ii] - bias_z) / (ii + 1);
	}
	radarPoseEstimation_inst->gyroCaliParam->bias_x = bias_x;
	radarPoseEstimation_inst->gyroCaliParam->bias_y = bias_y;
	radarPoseEstimation_inst->gyroCaliParam->bias_z = bias_z;
}

void calcAccelCaliParam(sAccelCaliData* accelCaliData, sRadarPoseEstimation* radarPoseEstimation_inst, float g)
{
	int ii = 0;
	float bias_x = 0;
	float bias_y = 0;
	float bias_z = 0;
	float scale_x = 0;
	float scale_y = 0;
	float scale_z = 0;
	float unbiasedAbsSum_x = 0.f;
	float unbiasedAbsSum_y = 0.f;
	float unbiasedAbsSum_z = 0.f;
	for (ii = 0; ii < 120; ii++)
	{
		bias_x = bias_x + (accelCaliData->accel_x[ii] - bias_x) / (ii + 1);
		bias_y = bias_y + (accelCaliData->accel_y[ii] - bias_y) / (ii + 1);
		bias_z = bias_z + (accelCaliData->accel_z[ii] - bias_z) / (ii + 1);
	}
	for (ii = 0; ii < 60; ii++)
	{
		unbiasedAbsSum_x += (float)accelCaliData->accel_x[ii] - accelCaliData->accel_x[ii + 60];
		unbiasedAbsSum_y += (float)accelCaliData->accel_y[ii] - accelCaliData->accel_y[ii + 60];
		unbiasedAbsSum_z += (float)accelCaliData->accel_z[ii] - accelCaliData->accel_z[ii + 60];
	}
	scale_x = g/(unbiasedAbsSum_x / 120.f);
	scale_y = g/(unbiasedAbsSum_y / 120.f);
	scale_z = g/(unbiasedAbsSum_z / 120.f);

	radarPoseEstimation_inst->accelCaliParam->bias_x = bias_x;
	radarPoseEstimation_inst->accelCaliParam->bias_y = bias_y;
	radarPoseEstimation_inst->accelCaliParam->bias_z = bias_z;
	radarPoseEstimation_inst->accelCaliParam->scale_x = scale_x; // 校准数据 = scale * （实测 - bias）
	radarPoseEstimation_inst->accelCaliParam->scale_y = scale_y;
	radarPoseEstimation_inst->accelCaliParam->scale_z = scale_z;
}
