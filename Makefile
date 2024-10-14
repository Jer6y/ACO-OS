
### use magic number 0x1579 to test if the top Makefile be called first
### if it is first called , we deal it with the start file of the kbuild system
### it will give some global variable and export for other to use
### .eg.  CC means compile-command 
###       ECHO MKDIR -> means tool name
ifneq ("${first_build}","0x1579")
first_build	:= 0x1579
export first_build


############## define project relative global variables #################################

this-makefile	:= $(abspath $(lastword ${MAKEFILE_LIST}))
src_tree	:= $(abspath $(dir ${this-makefile}))
export src_tree  	 # src-tree  -> absolute path for project root dir

############## include  files ###########################################################

include ${src_tree}/scripts/Makefile.include    #global function include

############## user/menuconfig control variables #######################################

###### KBUILD_VERBOSE start ############

ifeq ("$(origin V)", "command line")
  KBUILD_VERBOSE = $(V)
endif   #.eg. make ARCH=riscv V=1

ifeq ("${KBUILD_VERBOSE}","")
  KBUILD_VERBOSE = 0
endif   # control by config/auto.conf file

ifndef KBUILD_VERBOSE
  KBUILD_VERBOSE = 0
endif   # for the case not controled by V or auto.conf file

###### KBUILD_VERBOSE end   ############

############## toolchain  variables ###################################################

ifndef ARCH
  $(error "please set ARCH Variable First!!!")
endif #end check for ARCH

ifndef CROSS_COMPILE
  $(error "please set CROSS_COMPILE Variable First!!!")
endif #end check for CROSS_COMPILE

ARCH_IS_SUPPORTED =
ifeq ("${ARCH}","riscv")
  ARCH_IS_SUPPORTED = 1
endif

ifeq ("${ARCH_IS_SUPPORTED}","")
  $(error "don't support the specific arch")
endif

CC              = ${CROSS_COMPILE}gcc
CXX             = ${CROSS_COMPILE}g++
AR              = ${CROSS_COMPILE}ar
LD              = ${CROSS_COMPILE}ld
OBJDUMP         = ${CROSS_COMPILE}objdump
OBJCOPY         = ${CROSS_COMPILE}objcopy
NM              = ${CROSS_COMPILE}nm
AS              = ${CROSS_COMPILE}gcc
GDB		= ${CROSS_COMPILE}gdb
export CC CXX AR LD OBJDUMP OBJCOPY NM AS GDB
ECHO            = echo
RM              = rm -rf
MKDIR           = mkdir -p
MAKE		= make
DTC		= ${src_tree}/scripts/dtc/dtc
BASH		= bash
CP		= cp -rf 
export ECHO RM MKDIR MAKE DTC BASH CP

############## toolchain  flags     ###################################################

CFLAGS          = -O2 -g -ggdb3 -Wall -Werror -fno-builtin -fno-stack-protector \
                  -ffreestanding -fno-common -nostdlib\
                  -I$(src_tree)/include
LDFLAGS         =
ASFLAGS         = -I$(src_tree)/include -D__ASSEMBLY__
export CFLAGS LDFLAGS ASFLAGS

############## ctrol mode changes   ###################################################

ifeq ($(KBUILD_VERBOSE),1)
  Q 		=
else
  Q 		= @
  MAKE 		= make --no-print-directory --silent
  DTC		= ${src_tree}/scripts/dtc/dtc --quiet
endif
export Q	# Q -> VERBOSE Information Control

PHONY:=
###################################### target-1 compile kernel ##########################################

TARGET          = ${src_tree}/kernel
export TARGET

include ${src_tree}/arch/${ARCH}/Makefile.arch

${TARGET}: ${src_tree}/built-in.a
	$Q${LD} ${LDFLAGS} ${src_tree}/built-in.a -o $@
	@${ECHO} $(call QUIET_LINK,$@)

${src_tree}/built-in.a: compile
	@

PHONY+= compile
compile: kconfig_sync generate_asm
	$Q${MAKE} -C ${src_tree} -f ${src_tree}/scripts/Makefile.build


UAPI_ASM_DIR:= ${src_tree}/include/uapi/asm
export UAPI_ASM_DIR
PHONY+= generate_asm
generate_asm: ${UAPI_ASM_DIR}/bits.h
	@

${UAPI_ASM_DIR}/bits.h:
	$Q${CP} ${src_tree}/include/uapi/${ARCH}/asm ${UAPI_ASM_DIR}

###################################### target-2 clean  output   ##########################################

PHONY+= clean
clean:
	$Q${MAKE} -C ${src_tree} -f ${src_tree}/scripts/Makefile.clean clean

PHONY+= distclean
distclean:
	$Q${MAKE} -C ${src_tree} -f ${src_tree}/scripts/Makefile.clean distclean

###################################### target-3 config kernel ##########################################

PHONY+= menuconfig

menuconfig:${src_tree}/Kconfig
	${src_tree}/scripts/kconfig/mconf $<

KCONFIG_AUTOCONF= ${src_tree}/include/config/auto.conf
export KCONFIG_AUTOCONF

GENERATED_FILES	=
GENERATED_FILES	+= ${src_tree}/include/generated/autoconf.h
GENERATED_FILES	+= ${src_tree}/include/config/auto.conf.cmd
KCONFIG_FILE	= ${src_tree}/.config
export KCONFIG_FILE

GENERATED_DIR	= 
GENERATED_DIR	+= ${src_tree}/include/generated
GENERATED_DIR	+= ${src_tree}/include/config
export GENERATED_DIR

CONFIG_FILES	=
CONFIG_FILES	+= ${src_tree}/.config
CONFIG_FILES	+= ${src_tree}/.config.old
export CONFIG_FILES

PHONY += kconfig_sync
kconfig_sync: ${GENERATED_FILES}
	@

${GENERATED_FILES}: ${KCONFIG_FILE}
	$Q${src_tree}/scripts/kconfig/conf --syncconfig ${src_tree}/Kconfig
	@${ECHO} $(call QUIET_SYNC,KCONFIGS)
${KCONFIG_FILE}:
	@echo >&2 '***'
	@echo >&2 '*** Configuration file "$@" not found!'
	@echo >&2 '***'
	@echo >&2 '*** Please run some configurator (e.g. "make menuconfig")'
	@echo >&2 '***'
	@/bin/false

###################################### target-4 Makefile help ###########################################

PHONY+= help
help:
	@echo  'Cleaning targets:'
	@echo  '  clean           	- Remove most generated files but keep the config and'
	@echo  '                    	  enough build support to'
	@echo  '  distclean       	- mrproper + remove editor backup and patch files'
	@echo  ''
	@echo  'Other generic targets:'
	@echo  '  compile         	- Build the bare kernel'
	@echo  ''
	@echo  '  menuconfig      	- Configure Kernel config'
	@echo  ''
	@echo  '  hosttool_compile	- some tools we need is in the project, we need'
	@echo  '                    	  to compile it on our host mahine'
	@echo  ''

###################################### target-5 host-tool compile #######################################

# be careful! we now have the kbuild system run time !!!
# that means  CC = ${CROSS_COMPILE}gcc and so does CXX , CFLAGS
# these var is often used in the other build system , conflict!!!!
# we want to compile these host-tool on host, use host gnu-toolchain such as gcc
# remember to reset variable
DTC_PRJ_PATH := ${src_tree}/scripts/dtc
DTC_ABS_PATH := ${DTC_PRJ_PATH}/dtc
DTC_PRJ_TYPE := project_makefile
DTC_PRJ_MKFE := ${DTC_PRJ_PATH}/Makefile
DTC_PRJ_TAGT := 

PHONY += hosttool_compile
hosttool_compile: ${DTC_ABS_PATH}
	@

${DTC_ABS_PATH}:
	@${BASH} ${src_tree}/scripts/cancel_kbuild_runtime.sh "${DTC_PRJ_TYPE}" "${DTC_PRJ_PATH}" "${DTC_PRJ_MKFE}" \
		"${MAKE}" "${DTC_PRJ_TAGT}" \
		|| { echo "error for scripts dtc compile: $$?"; exit 1; }

###################################### target-x xxxxxxxx xxxx ###########################################



.PHONY: ${PHONY}

else #### end for first call

### when call this Makefile for the second time
### it is time to start our compile
obj-y += arch/
obj-y += module/
endif #### end for other call
