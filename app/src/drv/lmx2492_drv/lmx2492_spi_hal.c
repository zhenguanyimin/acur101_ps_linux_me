/**
 * Copyright (C), Autoroad Tech. Co., Ltd.
 * @brief   LMX2492 driver module
 * @file    lmx2492_spi_hal.c
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
#include "lmx2492_spi_hal.h"

/*******************************************************************************
 * 2492 driver extern Functions Implementation
 ******************************************************************************/

/*******************************************************************************
 * Local Functions - Implementation
 ******************************************************************************/

#include "../../bsp/reg/pl_reg.h"

#define LMX2492_WRITE_REG(addr,data)	PL_REG_WRITE(0xFFFF&addr,data)
#define LMX2492_READ_REG(addr)			PL_REG_READ(0xFFFF&addr)

/*******************************************************************************
 * API - Implementation
 ******************************************************************************/
lmx2492_status_t LMX2492_SPI_TransData(uint8_t* txData)
{
	// status_t status;
	uint32_t data = 0;

	data = (uint32_t)((data | txData[0]) << 16 | (data | txData[1]) << 8 | txData[2]);

	LMX2492_WRITE_REG(XPAR_AXI2REG2492_WRITE_BASEADDR, data);

	return LMX2492TransStatusOk;

}

