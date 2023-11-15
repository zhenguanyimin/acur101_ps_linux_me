
#ifndef MMWAVE_STRUCT_H_
#define MMWAVE_STRUCT_H_

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#define c_const 3.0e8

/* Complex type is often manipulated in the DSP using special compiler intrinsics
   (e.g _cmpyr) which requires the data of this type 
   to be 32-bit (4 byte) aligned. Using intrinsics on unaligned memory
   can cause unexpected results.
*/   
//typedef struct __attribute__((aligned(4))) cmplx16ReIm_t_ // TODO: debug
typedef struct cmplx16ReIm_t_
{
    int16_t real; /*!< @brief real part */
    int16_t imag; /*!< @brief imaginary part */
} cmplx16ReIm_t;
   
//typedef struct __attribute__((aligned(4))) cmplx16ImRe_t_ //TODO: debug
typedef struct cmplx16ImRe_t_
{
    int16_t imag; /*!< @brief imaginary part */
    int16_t real; /*!< @brief real part */
} cmplx16ImRe_t;

/*! @brief  Complex data type. This type of input, (first real than
 * imaginary part), is required for DSP lib FFT functions */
typedef struct cmplx32ReIm_t_
{
    int32_t real; /*!< @brief real part */
    int32_t imag; /*!< @brief imaginary part */
} cmplx32ReIm_t;

/*! @brief  Complex data type, natural for C674x complex
 * multiplication instructions */
typedef struct cmplx32ImRe_t_
{
    int32_t imag; /*!< @brief imaginary part */
    int32_t real; /*!< @brief real part */
} cmplx32ImRe_t;

/*! @brief  Complex data type. This type of input, (first real than
 * imaginary part), is required our fft functions */
typedef struct cmplxFloatReIm_
{
    float real; /*!< @brief real part */
    float imag; /*!< @brief imaginary part */
} cmplxFloatReIm;

/*!
 *  @brief    Parameters of CFAR detected object during the first round of
 *  CFAR detections
 */
typedef struct mmw_objRaw
{
    uint16_t   rangeIdx;     /*!< @brief Range index */
    uint16_t   dopplerIdx;   /*!< @brief Dopler index */
    float   peakVal;      /*!< @brief Peak value */
	float   x;
	float   y;
	float   z;
	float range;
	float azimuth;
	float doppler;
} mmw_objRaw_t;

/*
 *  @brief    Active Doppler lines, lines (bins) on which the
 *            CFAR detector detected objects during the detections in
 *            Doppler direction
 */
typedef struct mmw_1D_DopplerLines
{
    uint32_t   *dopplerLineMask;      /*!< @brief Doppler line bit mask of active
                                   (CFAR detected) Doppler bins in the first
                                   round of CFAR detections in DOppler direction.
                                   The LSB bit of the first word corresponds to
                                   Doppler bin zero of the range/Doppler
                                   detection matrix*/
    uint16_t   currentIndex;     /*!< @brief starting index for the search
                                   for next active Doppler line */
    uint16_t    dopplerLineMaskLen;   /*!< @brief size of dopplerLineMask array, (number of
                                    32_bit words, for example for Doppler FFT
                                    size of 64 this length is equal to 2)*/

	//// Construct function
	//mmw_1D_DopplerLines() {
	//	this->dopplerLineMaskLen = 128;
	//	this->dopplerLineMask = (uint32_t *)malloc(sizeof(uint32_t)*128);
	//}
} mmw_1D_DopplerLines_t;

typedef struct mmw_compRxChannelBiasCfg {
	float rangeBias;
    cmplxFloatReIm *rxChPhaseComp;

}mmw_compRxChannelBiasCfg_t;

typedef struct mmw_configParams_ {
    mmw_compRxChannelBiasCfg_t compRxChanCfg;
	bool extendedMaxVelocityEnabled;
	bool multiObjBeamFormingEnabled;
	float multiObjBeamForming_multiPeakThrsScal;
	uint16_t peakGroupingCfg_scheme;
}mmw_configParams;



/*  Millimeter Wave Data Struct 
* Note that the domain of ADCdataBuf is (TxAzim+TxElev, Rx, dopplerBins, rangeBins)
*/
typedef struct mmwDataObj_{
	float timestamp; // need to be set from parsing module
    /* Number of receive channels */
    uint8_t numRxAntennas; // need to be set from parsing module
    /* Pointer to ADC buffer, (TxAzim+TxElev, Rx, dopplerBins, rangeBins) */
    cmplxFloatReIm *ADCdataBuf; // need to be filled from parsing module
    /* number of transmit antennas, numTxAntAzim+numTxAntElev */
    uint8_t numTxAntennas; // need to be set from parsing module
    /* number of transmit antennas for elevation */
    uint8_t numTxAntElev; // need to be set from parsing module
    /* number of virtual antennas, numVirtualAntAzim+numVirtualAntElev */
    uint8_t numVirtualAntennas; // need to be set from parsing module
    /* number of virtual azimuth antennas, numTxAntAzim*numRxAntennas */
    uint8_t numVirtualAntAzim; // need to be set from parsing module
    /* number of virtual elevation antennas, numTxAntElev*numRxAntennas */
    uint8_t numVirtualAntElev; // need to be set from parsing module
    /* frequence of ADC sampling */
    int32_t freqAdcSample; // need to set from parsing module
    /* number of ADC samples */
    uint32_t numAdcSamples; // need to set from parsing module
    /* number of chirps per frame, the number of chirps from all the Tx */
    uint32_t numChirpsPerFrame; // need to set from parsing module
	/* The number of chirps per Ant */
	uint32_t numChirpsPerAnt; // need to set from parsing module
	 /* the center frequency of mmw */
	uint64_t centerfrequency;
	/* Actual FM bandwidth of mmw */
	uint32_t actualBandwidth;
     /* lamda of mmw */
    float mmwLamda; // need to set from parsing module
	float idleTime; /* wait time of ADC sampling */
	float startTime; /* start time of ADC sampling */
	float rampendTime; /* ramp end time of ADC sampling */
	float TcTime; /* time required for each chirp */

    /* 1D FFT output */
    cmplxFloatReIm *fftOut1D; // (TxAzim+TxElev, Rx, dopplerBins, rangeBins)
    /* 2D FFT output */
    cmplxFloatReIm *fftOut2D; // (TxAzim+TxElev, Rx, dopplerBins, rangeBins)
    /* log2 absolute computation output buffer */
    float *log2Abs; // (1, numDopplerBins)
    /* accumulated sum of log2 absolute over the antennae */
    float *sumAbs; // (1, numDopplerBins)
    /* input buffer for CFAR processing from the detection matrix */
    float *sumAbsRange; // (numDopplerBins, numRangeBins)
    /* CFAR output objects index buffer */
    uint32_t *cfarDetObjIndexBuf;
    /* input for Azimuth FFT */
    cmplxFloatReIm *azimuthIn;
    /* output of Azimuth FFT */
    cmplxFloatReIm *azimuthOut;
    /* output of Azimuth FFT magnitude squared */
    float   *azimuthMagSqr;
    /* twiddle factors table for Azimuth FFT */
    /* Pointer to single point DFT coefficients used for Azimuth processing */
    cmplxFloatReIm *azimuthModCoefs; // reserve
    /* Half bin needed for doppler correction as part of Azimuth processing */
    cmplxFloatReIm azimuthModCoefsHalfBin; //reserve
    /* Pointer to range/Doppler log2 magnitude detection matrix in L3 RAM */
    float *detMatrixDoppler; // (numRangeBins, numDopplerBins)
    float *detMatrix; // (numDopplerBins, numRangeBins)
    /* Pointer to 2D FFT array in range direction, at doppler index 0,
     * for static azimuth heat map */
    cmplxFloatReIm *azimuthStaticHeatMap; // (numDopplerBins, numRangeBins)
    /* number of range bins */
    uint32_t numRangeBins; // numRangeBins approx numAdcSamples, but it < numAdcSamples
    /* number of angle bins */
    uint32_t numAngleBins; // It equals to numVirtualAntAzim
    /* number of doppler bins */
    uint32_t numDopplerBins; // numChirpsPerFrame/numTx
    /* range resolution in meters */
    float rangeResolution; // c/(2*f_sample)
    /* radial velocity resolution in m/s */
    float dopplerResolution; // lamda*f_sample/(2*numDopplerBins*numRangeBins)
    /* Q format of the output x/y/z coordinates */
    uint8_t xyzOutputQFormat; // reserve
    /* Number of detected objects */
    uint32_t numDetObj;
    /* Number of detected objects */
    uint32_t numDetObjRaw;
    /* Detected Doppler lines */
	mmw_1D_DopplerLines_t detDopplerLines;
    /* Detected objects after second pass in Range direction */
    mmw_objRaw_t *detObj2D;
    /* Detected objects before peak grouping */
    mmw_objRaw_t *detObj2DRaw;
    /* Detected objects azimuth index for debugging */
    uint8_t *detObj2dAzimIdx; // (1, numAngleBins)
    /* chirp counter modulo number of chirps per frame */
    uint32_t chirpCount; // reserve
    /* chirp counter modulo number of tx antennas */
    uint8_t txAntennaCount; // reserve
    /* chirp counter modulo number of Doppler bins */
    uint32_t dopplerBinCount; // reserve
    /* number of bytes per ADC sample in ADC buffer  */
    uint8_t    numBytePerSample; // reserve
    /* Rx channel gain/phase offset compensation coefficients */
    mmw_compRxChannelBiasCfg_t compRxChanCfg; // reserve
	/* config */
	mmw_configParams *commonCfg;
} mmwDataObj;


#endif // MMWAVE_STRUCT_H_
