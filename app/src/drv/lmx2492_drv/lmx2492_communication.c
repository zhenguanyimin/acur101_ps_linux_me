/**
 * Copyright (C), Autoroad Tech. Co., Ltd.
 * @brief   LMX2492 driver module
 * @file    lmx2492_commmunication.c
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
#include <unistd.h>
#include "lmx2492_communication.h"
#include "lmx2492_spi_hal.h"
#include "lmx2492_assert.h"

#include "../../bsp/reg/pl_reg.h"

#define LMX2492_WRITE_REG(addr,data)	PL_REG_WRITE(0xFFFF&addr,data)
#define LMX2492_READ_REG(addr)			PL_REG_READ(0xFFFF&addr)

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define LMX2492_COMM_FRAME_SIZE     0x03U  		/* Length of the communication frame */

/*******************************************************************************
 * Local Variables
 ******************************************************************************/
 #if SRC_RF_PLUS
	 uint8_t lmx2492_register_data_array[142] = {
		0x18, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,		// 1, 3-15 reserved
		0x96, 0x00, 0x3C, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0x01, 0x00, 0x18, 0x04, 0x00, 0x0A, 0x32,
		0x00, 0x20, 0x80, 0x41, 0x5A, 0x10, 0x1f, 0x3A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x70, 0x00,		// 0X30-0X3F
		0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x80, 0x0C, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC8, 0x00, 0x20, 0x00, 0x00, 0x00,		// 0X50-0X5F
		0x00, 0xD0, 0x27, 0x44, 0x00, 0x00, 0x00, 0x00, 0xD0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA0,
		0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA0, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA0, 0x0F, 0x00,		// 0X70-0X7F
		0x00, 0x00, 0x00, 0x00, 0xA0, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA0, 0x0F, 0x00
	};
 #else
	uint8_t lmx2492_register_data_array[142] = {
		0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,		// 1, 3-15 reserved
		0x64, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x08, 0x00, 0x00, 0x0A, 0x32,
		0x00, 0x0F, 0x00, 0x41, 0x08, 0x10, 0x1f, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,		// 0X30-0X3F
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
		0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,		// 0X50-0X5F
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,		// 0X70-0X7F
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};
 #endif

/*******************************************************************************
 * Local Functions - Implementation
 ******************************************************************************/
/* Creates a raw frame for SPI transfer. */
static void LMX2492_SPI_CreateSendFrame(lmx2492_data_frame_t* txData, uint8_t* txFrame)
{
    FS_ASSERT(txData != NULL);
    FS_ASSERT(txFrame != NULL);

    /* check if register is r or w */
    switch (txData->commandType)
    {
        case LMX2492RegRead:
            txData->regAddress |= REGISTER_RW_BIT_MASK;
            /* Creates read command. */
            txFrame[2] = 0x00;
            txFrame[1] = (uint8_t)(txData->regAddress);
            txFrame[0] = (uint8_t)(txData->regAddress >> 8);
            break;

        case LMX2492RegWrite:
            /* Creates write command - MSB first. */
            txFrame[2] = (uint8_t)(txData->writeData);
            txFrame[1] = (uint8_t)(txData->regAddress);
            txFrame[0] = (uint8_t)(txData->regAddress >> 8);
            break;
    }

}

/*******************************************************************************
 * API - Implementation
 ******************************************************************************/
void LMX2492_read_write_interrupt_set(uint16_t data)
{
	LMX2492_WRITE_REG(XPAR_AXI2REG2492_INT_BASEADDR, data);

}

/* Performs SPI transfer of the txData. Received frame is saved into rxData structure. */
lmx2492_status_t LMX2492_write_register(lmx2492_data_frame_t* txData)
{

    FS_ASSERT(txData != NULL);
    FS_ASSERT(rxData != NULL);

    lmx2492_status_t status = LMX2492TransStatusOk;
    uint8_t txFrame[LMX2492_COMM_FRAME_SIZE] = {0};

    LMX2492_SPI_CreateSendFrame(txData, txFrame);
    status = LMX2492_SPI_TransData(txFrame);
    if (status != LMX2492TransStatusOk)
    {
        return status;
    }

    return status;

}

void LMX2492_write_single_register(uint32_t data)
{
	LMX2492_WRITE_REG(XPAR_AXI2REG2492_WRITE_BASEADDR, data);

}

///* LMX2492 register reading. */
//lmx2492_status_t LMX2492_read_register(uint16_t regAddr, uint8_t *rxData, uint8_t mask)
//{
//    lmx2492_status_t status = LMX2492TransStatusOk;
//
//    uint8_t tmp = 0;
//    lmx2492_data_frame_t txData = {
//        .commandType = LMX2492RegRead,
//        .regAddress  = regAddr };
//
//	Xil_Out32(XPAR_AXI_GPIO_2492_RDWR_NUM_BASEADDR, 0x81);
//    /* Readout register value */
//    status = LMX2492_write_register(&txData, &tmp);
//    if (status != LMX2492TransStatusOk)
//    {
//        return LMX2492TransStatusError;
//    }
//
//    *rxData = tmp & mask;
//
//    return status;
//
//}

lmx2492_status_t LMX2492_read_register(uint32_t data, uint8_t *value)
{
    lmx2492_status_t status = LMX2492TransStatusOk;

	LMX2492_WRITE_REG(XPAR_AXI2REG2492_WRITE_BASEADDR, data);
	usleep(100);

	*value = (LMX2492_READ_REG(XPAR_AXI2REG2492_READ0_BASEADDR)) & 0XFF;

    return status;

}


/* individual register setting. */
lmx2492_status_t LMX2492_register_setting(uint16_t regAddr, uint8_t value, uint8_t mask, uint8_t shift)
{

	lmx2492_status_t status = LMX2492TransStatusOk;

	uint8_t tmp = lmx2492_register_data_array[regAddr];

    uint8_t writeData = (value << shift) & mask;

    tmp  = (tmp & (~mask)) | writeData;

    lmx2492_register_data_array[regAddr] = tmp;

    return status;

}



//lmx2492_status_t LMX2492_register_setting(uint16_t regAddr, uint8_t value, uint8_t mask, uint8_t shift)
//{
//    lmx2492_status_t status = LMX2492TransStatusOk;
//
//    lmx2492_data_frame_t txData = {
//        .commandType = LMX2492RegRead,
//        .regAddress  = regAddr };
//    uint8_t rxData = 0;
//    uint8_t tmp = 0;
//
//    /* Readout register value */
//    status = LMX2492_write_register(&txData, &rxData);
//    if (status != LMX2492TransStatusOk)
//    {
//        return LMX2492TransStatusError;
//    }
//
//    /* Write to register after modifying corresponding bit */
//    txData.commandType = LMX2492RegWrite;
//    tmp  = rxData & (~mask);
//    tmp |= (value & mask) << shift;
//
//    txData.writeData = tmp;
//
//    status = LMX2492_write_register(&txData, &rxData);
//    if (status != LMX2492TransStatusOk)
//    {
//        return LMX2492TransStatusError;
//    }
//
//    /* Read the register value again to determine whether the write was successful */
//    txData.commandType = LMX2492RegRead;
//    rxData = 0;
//
//    status = LMX2492_write_register(&txData, &rxData);
//    if (status != LMX2492TransStatusOk)
//    {
//        return LMX2492TransStatusError;
//    }
//
//    if (rxData != ((value & mask) << shift)) {
//        return LMX2492WriteRegisterError;
//    }
//
//    return status;
//}
