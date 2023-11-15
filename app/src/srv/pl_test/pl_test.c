
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>

#include "pl_test.h"

#include "../../bsp/reg/pl_reg.h"

#define PL_TEST_WRITE_REG(addr,data)	PL_REG_WRITE(0xFFFF&addr,data)
#define PL_TEST_READ_REG(addr)			PL_REG_READ(0xFFFF&addr)


void  pl_reg_read(uint32_t reg_addr, uint32_t *value)
{
	*value = PL_TEST_READ_REG(reg_addr);
	printf( "[%s %d] %x %x\r\n", __func__, __LINE__, reg_addr, *value );
}

void  pl_reg_write(uint32_t reg_addr, uint32_t value)
{
	printf( "[%s %d] %x %x\r\n", __func__, __LINE__, reg_addr, value );
    PL_TEST_WRITE_REG(reg_addr, value);
}
