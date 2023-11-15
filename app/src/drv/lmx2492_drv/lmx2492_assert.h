/*
 * Copyright (c) 2022, Autel.
 * All rights reserved.
 * @brief   LMX2492 driver module
 * @file    lmx2492_assert.h
 * @author  X22012
 * @date    2022.11.01
 *
 * -History:
 *      -# author : X22012  
 *         date   : 2022.11.01
 *         Version: V1.0
 *         details: Created
 */

#ifndef LMX2492_ASSERT_H_
#define LMX2492_ASSERT_H_


/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdbool.h>

#if defined LMX2492_ASSERT

    #define FS_ASSERT(x) FsDevAssert(x)
#else
    /* Assert macro does nothing */
    #define FS_ASSERT(x) ((void)0)
#endif


#endif /* LMX2492_ASSERT_H_ */
