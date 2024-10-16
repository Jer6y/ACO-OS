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

quiet_cmd_objcopy 	= OBJCP $@
cmd_objcopy		= ${OBJCOPY} -O binary ${TARGET} $@
${BOARD_TARGET}: ${CONFIG_DTS_PATH}/device.dtb ${TARGET}
	$(call cmd,objcopy)

quiet_cmd_dtc	= DTC   $@
cmd_dtc		= ${DTC} -I dts -O dtb $< -o $@
${CONFIG_DTS_PATH}/device.dtb: ${CONFIG_DTS_PATH}/device.dts
	$(call cmd,dtc)

clean:
	$Q${MAKE} -C ${src_tree}/arch/${ARCH} -f ${src_tree}/arch/${ARCH}/scripts/Makefile.clean clean

distclean:
	$Q${MAKE} -C ${src_tree}/arch/${ARCH} -f ${src_tree}/arch/${ARCH}/scripts/Makefile.clean distclean

.PHONY: clean distclean
