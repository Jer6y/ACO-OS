include ${src_tree}/scripts/Makefile.include
-include ${KCONFIG_AUTOCONF}

ifeq ("${CONFIG_DTS_PATH}","")
is_build := 1
  ifeq ("${MAKECMDGOALS}","clean")
    is_build := 0
  endif
  
  ifeq ("${MAKECMDGOALS}","distclean")
    is_build := 0
  endif

  ifeq ("${is_build}","1")
    $(error please set CONFIG_DTS_PATH First, modify your Kconfig system)
  endif
endif

RV_CFLAGS := -Wall -Werror -Wno-unused-variable -Wno-implicit-function-declaration -Wno-pointer-sign -Wno-unused-but-set-variable\
	     -ffreestanding -fno-common -nostdlib\
	     -I${src_tree}/arch/${ARCH}/include -I${src_tree}/include -mcmodel=medany -g	\
	     -I${src_tree}/arch/${ARCH}/early_boot/include
RV_ASFLAGS:= -g	-I${src_tree}/arch/${ARCH}/include -I${src_tree}/include -I${src_tree}/arch/${ARCH}/early_boot/include -D__ASSEMBLY__
RV_LDFLAGS:= -static -T ${CONFIG_DTS_PATH}/link.ld
export RV_CFLAGS RV_ASFLAGS RV_LDFLAGS

${BOARD_TARGET}: ${src_tree}/arch/${ARCH}/early_boot/built-in.a ${CONFIG_DTS_PATH}/device.dtb ${CONFIG_DTS_PATH}/link.ld
	$Q${LD} ${RV_LDFLAGS} $< -o $@

${CONFIG_DTS_PATH}/link.ld:${CONFIG_DTS_PATH}/link.h
	$Q${CC} -E -I${src_tree}/include -I${src_tree}/arch/${ARCH}/include $< -o $@

${CONFIG_DTS_PATH}/device.dtb: ${CONFIG_DTS_PATH}/device.dts
	$Q${DTC} -I dts -O dtb $< -o $@

${src_tree}/arch/${ARCH}/early_boot/built-in.a: compile
	@

compile:
	$Q${MAKE} -C ${src_tree}/arch/${ARCH}/early_boot -f ${src_tree}/arch/${ARCH}/scripts/Makefile.earlybuild

clean:
	$Q${MAKE} -C ${src_tree}/arch/${ARCH} -f ${src_tree}/arch/${ARCH}/scripts/Makefile.clean clean

distclean:
	$Q${MAKE} -C ${src_tree}/arch/${ARCH} -f ${src_tree}/arch/${ARCH}/scripts/Makefile.clean distclean

.PHONY: clean distclean compile
