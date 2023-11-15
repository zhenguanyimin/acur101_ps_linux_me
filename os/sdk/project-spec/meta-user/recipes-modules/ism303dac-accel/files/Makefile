obj-m := st_lsm303ah_accel_i2c.o

st_lsm303ah_accel_i2c-objs := st_ism303dac_accel_buffer.o \
		st_ism303dac_accel_i2c.o \
		st_ism303dac_accel_core.o \
		st_ism303dac_accel_trigger.o 

MY_CFLAGS += -g -DDEBUG
ccflags-y += ${MY_CFLAGS}

SRC := $(shell pwd)

all:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC)

modules_install:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC) modules_install

clean:
	rm -f *.o *~ core .depend .*.cmd *.ko *.mod.c
	rm -f Module.markers Module.symvers modules.order
	rm -rf .tmp_versions Modules.symvers
