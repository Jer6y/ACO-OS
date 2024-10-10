#!/bin/bash

#### normal clear the kbuild system runtime ######
unset CC
unset CXX 
unset AR 
unset LD 
unset OBJDUMP
unset OBJCOPY
unset NM
unset AS
unset CFLAGS
unset LDFLAGS
unset ASFLAGS
unset Q
unset TARGET

set -e

PRJ_TYPE=$1
if [ -z "$PRJ_TYPE" ]; then
	echo "null for project type"
	exit 1
fi

if [ "$PRJ_TYPE" == "project_makefile" ]; then
	PRJ_ABS_PATH=$2
	PRJ_ABS_MKFE=$3
	PRJ_WAT_TOOL=$4
	PRJ_REL_TAGT=$5
	if [ -z "$PRJ_ABS_PATH" ] || [ -z "$PRJ_ABS_MKFE" ] || [ -z "$PRJ_WAT_TOOL" ]; then
		echo "null for makefile project path and makfile path and make tool"
	fi
	$PRJ_WAT_TOOL -C $PRJ_ABS_PATH -f $PRJ_ABS_MKFE $PRJ_REL_TAGT || { exit 1; }
else
	echo "error!!!"
	exit 1
fi

exit 0
