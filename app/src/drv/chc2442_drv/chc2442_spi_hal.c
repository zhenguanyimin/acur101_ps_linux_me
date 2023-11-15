/**
 * Copyright (C), Autoroad Tech. Co., Ltd.
 * @brief   CHC2442 driver module
 * @file    CHC2442_spi_hal.c
 * @author  X22012
 * @date    2022.11.01
 *
 * -History:
 *      -# author : X22012  
 *         date   : 2022.11.01
 *         Version: V1.0
 *         details: Created
 */


/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "chc2442_common.h"
#include "chc2442_spi_hal.h"
//#include "xparameters.h"
/*******************************************************************************
 * 2442 driver extern Functions Implementation
 ******************************************************************************/

/*******************************************************************************
 * Local Functions - Implementation
 ******************************************************************************/

#include "../../bsp/reg/pl_reg.h"

#define CHC2442_WRITE_REG(addr,data)	PL_REG_WRITE(0xFFFF&addr,data)
#define CHC2442_READ_REG(addr)			PL_REG_READ(0xFFFF&addr)

/*******************************************************************************
 * API - Implementation
 ******************************************************************************/
chc2442_status_t CHC2442_SPI_TransData(uint32_t txData)
{
	/* Call of blocking driver function. */
	CHC2442_WRITE_REG(XPAR_AXI2REG2442_WRITE_BASEADDR, txData);

	return chc2442TransStatusOk;
}

