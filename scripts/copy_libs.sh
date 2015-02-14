#!/bin/sh
# author:      Erik Sohns <eriksohns@123mail.org>
# this script collects the libraries in one place for in-source-tree
# debugging
# *NOTE*: this is neither portable nor particularly stable !
# parameters:   - $1 [BUILD] {"debug" || "debug_tracing" || "release" || ...}
# return value: - 0 success, 1 failure

DEFAULT_PROJECT_DIR="$(dirname $(readlink -f $0))/.."
PROJECT_DIR=${DEFAULT_PROJECT_DIR}
# sanity check(s)
[ ! -d ${PROJECT_DIR} ] && echo "ERROR: invalid project dir (was: \"${PROJECT_DIR}\"), aborting" && exit 1

DEFAULT_BUILD="debug"
BUILD=${DEFAULT_BUILD}
if [ $# -lt 1 ]; then
 echo "INFO: using default build: \"${BUILD}\""
else
 # parse any arguments
 if [ $# -ge 1 ]; then
  BUILD="$1"
 fi
fi

# sanity check(s)
[ ${BUILD} != "debug" -a ${BUILD} != "debug_tracing" -a ${BUILD} != "release" ] && echo "WARNING: invalid/unknown build (was: \"${BUILD}\"), continuing"
BUILD_DIR="${PROJECT_DIR}/build/${BUILD}"
[ ! -d "${BUILD_DIR}" ] && echo "ERROR: invalid build dir (was: \"${BUILD_DIR}\"), aborting" && exit 1

LIB_DIR="lib"
TARGET_DIR="${BUILD_DIR}/${LIB_DIR}"
if [ ! -d "${TARGET_DIR}" ]; then
 mkdir ${TARGET_DIR}
 [ $? -ne 0 ] && echo "ERROR: failed to mkdir \"${TARGET_DIR}\", aborting" && exit 1
 echo "INFO: created directory \"${TARGET_DIR}\", continuing"
fi

LIB_DIR=".libs"
SUB_DIRS="libCommon/src
libCommon/src/ui
libACEStream/src
libACENetwork/src
libACENetwork/src/client"
declare -a LIBS=("libCommon.so"
"libCommon_UI.so"
"libACEStream.so"
"libACENetwork.so"
"libACENetwork_Client.so")
i=0
for DIR in $SUB_DIRS
do
 LIB="${BUILD_DIR}/modules/${DIR}/${LIB_DIR}/${LIBS[$i]}"
 [ ! -r "${LIB}" ] && echo "ERROR: invalid library file (was: \"${LIB}\"), aborting" && exit 1
 cp ${LIB} "${TARGET_DIR}/${LIBS[$i]}.0"
 [ $? -ne 0 ] && echo "ERROR: failed to copy \"${LIB}\" to \"${TARGET_DIR}\": $?, aborting" && exit 1
 echo "copied \"$LIB\"..."

 i=$i+1
done

