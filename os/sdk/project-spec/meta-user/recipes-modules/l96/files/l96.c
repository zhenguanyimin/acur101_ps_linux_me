// SPDX-License-Identifier: GPL-2.0
/*
 * l96 GNSS receiver driver
 *
 * Copyright (C) 2023 chenyili <chenyili@autel.com>
 */

#include <linux/errno.h>
#include <linux/gnss.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/regulator/consumer.h>
#include <linux/serdev.h>
#include <linux/gpio.h>

#include "serial.h"

struct quectel_data {
	struct gpio_desc *on;
	struct gpio_desc *reset;
};

static int quectel_set_active(struct gnss_serial *gserial)
{
	struct quectel_data *data = gnss_serial_get_drvdata(gserial);

	if (data->on)
		gpiod_direction_output(data->on, 1);
	if (data->reset)
		gpiod_direction_output(data->reset, 0);

	return 0;
}

static int quectel_set_standby(struct gnss_serial *gserial)
{
	struct quectel_data *data = gnss_serial_get_drvdata(gserial);
	
	if (data->reset)
		gpiod_direction_output(data->reset, 1);

	if (data->on)
		gpiod_direction_output(data->on, 0);

	return 0;
}

static int quectel_set_power(struct gnss_serial *gserial,
				enum gnss_serial_pm_state state)
{
	switch (state) {
	case GNSS_SERIAL_ACTIVE:
		return quectel_set_active(gserial);
	case GNSS_SERIAL_OFF:
	case GNSS_SERIAL_STANDBY:
		return quectel_set_standby(gserial);
	}

	return -EINVAL;
}

static const struct gnss_serial_ops quectel_gserial_ops = {
	.set_power = quectel_set_power,
};

static int quectel_probe(struct serdev_device *serdev)
{
	struct gnss_serial *gserial;
	struct quectel_data *data;
	int ret;

	gserial = gnss_serial_allocate(serdev, sizeof(*data));
	if (IS_ERR(gserial)) {
		ret = PTR_ERR(gserial);
		return ret;
	}

	gserial->ops = &quectel_gserial_ops;

	gserial->gdev->type = GNSS_TYPE_NMEA;

	data = gnss_serial_get_drvdata(gserial);

	data->on = devm_gpiod_get(&serdev->dev, "on", GPIOD_OUT_LOW);
	if (IS_ERR(data->on)) {
		ret = PTR_ERR(data->on);
		if (ret == -ENODEV)
			data->on = NULL;
		else 
			goto err_free_gserial;
	}

	data->reset = devm_gpiod_get(&serdev->dev, "reset", GPIOD_OUT_HIGH);
	if (IS_ERR(data->reset)) {
		ret = PTR_ERR(data->reset);
		if (ret == -ENODEV)
			data->reset = NULL;
		else
			goto err_free_gserial;
	}

	ret = gnss_serial_register(gserial);
	if (ret)
		goto err_free_gserial;

	return 0;

err_free_gserial:
	gnss_serial_free(gserial);

	return ret;
}

static void quectel_remove(struct serdev_device *serdev)
{
	struct gnss_serial *gserial = serdev_device_get_drvdata(serdev);
	struct quectel_data *data = gnss_serial_get_drvdata(gserial);

	gnss_serial_deregister(gserial);
	gnss_serial_free(gserial);
};

#ifdef CONFIG_OF
static const struct of_device_id quectel_of_match[] = {
	{ .compatible = "quectel,l96" },
	{},
};
MODULE_DEVICE_TABLE(of, quectel_of_match);
#endif

static struct serdev_device_driver quectel_gnss_driver = {
	.driver	= {
		.name		= "gnss-quectel",
		.of_match_table	= of_match_ptr(quectel_of_match),
		//.pm		= &gnss_serial_pm_ops,
	},
	.probe	= quectel_probe,
	.remove	= quectel_remove,
};
module_serdev_device_driver(quectel_gnss_driver);

MODULE_AUTHOR("chenyili <chenyili@autel.com>");
MODULE_DESCRIPTION("quectel GNSS receiver driver");
MODULE_LICENSE("GPL v2");

