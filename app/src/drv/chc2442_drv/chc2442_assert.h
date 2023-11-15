/*
 * Copyright (c) 2022, Autel.
 * All rights reserved.
 * @brief   CHC2442 driver module
 * @file    chc2442_assert.h
 * @author  X22012
 * @date    2022.11.01
 *
 * -History:
 *      -# author : X22012  
 *         date   : 2022.11.01
 *         Version: V1.0
 *         details: Created
 */

#ifndef CHC2442_ASSERT_H_
#define CHC2442_ASSERT_H_


/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdbool.h>

#if defined CHC2442_ASSERT

    #define FS_ASSERT(x) FsDevAssert(x)
#else
    /* Assert macro does nothing */
    #define FS_ASSERT(x) ((void)0)
#endif


#endif /* CHC2442_ASSERT_H_ */
