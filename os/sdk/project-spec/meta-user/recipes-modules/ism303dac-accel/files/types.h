/* SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note */
/* industrial I/O data types needed both in and out of kernel
 *
 * Copyright (c) 2008 Jonathan Cameron
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 */

#ifndef _ISM303DAC_TYPES_H_
#define _ISM303DAC_TYPES_H_

/* see also: https://github.com/STMicroelectronics/STMems_Linux_IIO_drivers */

enum iio_chan_type_ISM303DAC {
	IIO_SIGN_MOTION = 100, //MUST bigger than enum iio_chan_type
    IIO_STEP_DETECTOR,
    IIO_STEP_COUNTER,
    IIO_TILT,
    IIO_TAP,
    IIO_TAP_TAP,
    IIO_WRIST_TILT_GESTURE,
    IIO_GESTURE,
    IIO_WKUP,
    IIO_FREE_FALL,
};

enum iio_event_type_ISM303DAC {
	IIO_EV_TYPE_FIFO_FLUSH = 100, //MUST bigger than enum iio_event_type
};

enum iio_event_direction_ISM303DAC {
	IIO_EV_DIR_FIFO_EMPTY = 100, //MUST bigger than enum iio_event_direction
	IIO_EV_DIR_FIFO_DATA,
};

#endif /* _ISM303DAC_TYPES_H_ */

