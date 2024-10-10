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

RV_CFLAGS := 
RV_ASFLAGS:=
RV_LDFLAGS:= -static -T ${CONFIG_DTS_PATH}/link.ld
export RV_CFLAGS RV_ASFLAGS RV_LDFLAGS

${BOARD_TARGET}: ${src_tree}/arch/${ARCH}/early_boot/built-in.a ${CONFIG_DTS_PATH}/device.dtb ${CONFIG_DTS_PATH}/link.ld
	$Q${LD} ${RV_LDFLAGS} $< -o $@

${CONFIG_DTS_PATH}/link.ld:${CONFIG_DTS_PATH}/link.h
	$Q${CC} -E -I${src_tree}/include $< -o $@

${CONFIG_DTS_PATH}/device.dtb: ${CONFIG_DTS_PATH}/device.dts
	$Q${DTC} -I dts -O dtb $< -o $@

${src_tree}/arch/${ARCH}/early_boot/built-in.a:
	$Q${MAKE} -C ${src_tree}/arch/${ARCH}/early_boot -f ${src_tree}/arch/${ARCH}/scripts/Makefile.earlybuild

clean:
	$Q${MAKE} -C ${src_tree}/arch/${ARCH} -f ${src_tree}/arch/${ARCH}/scripts/Makefile.clean clean

distclean:
	$Q${MAKE} -C ${src_tree}/arch/${ARCH} -f ${src_tree}/arch/${ARCH}/scripts/Makefile.clean distclean

.PHONY: clean distclean compile
