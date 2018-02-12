#!/bin/sh
# author:      Erik Sohns <eriksohns@123mail.org>
# this script starts the debugger after setting debugee capabilities
# arguments:   debugger executable {/usr/bin/gdb}
# return value: - 0 success, 1 failure

# sanity checks
#command -v sudo >/dev/null 2>&1 || { echo "sudo is not installed, aborting" >&2; exit 1; }
#command -v gksudo >/dev/null 2>&1 || { echo "gksudo is not installed, aborting" >&2; exit 1; }
command -v gdb >/dev/null 2>&1 || { echo "gdb is not installed, aborting" >&2; exit 1; }

# *NOTE*: regular users may not have the CAP_SETFCAP capability needed to modify
#         (executable) file capabilities --> run as root
# *TODO*: verify this programmatically
#HAS_GKSUDO=0
#if [ -x gksudo ]; then
# HAS_GKSUDO=1
#fi

#if [ "${USER}" != "root" ]; then
# SUDO=sudo
# CMDLINE_ARGS="$@"
# if [ ${HAS_GKSUDO} -eq 1 ]; then
#  SUDO=gksudo
#  CMDLINE_ARGS="--disable-grab $0 $@"
# fi
# echo "invoking sudo $0 \"${CMDLINE_ARGS}\"..."
# exec ${SUDO} $0 ${CMDLINE_ARGS}
#fi
#echo "starting..."

# sanity checks
command -v dirname >/dev/null 2>&1 || { echo "dirname is not installed, aborting" >&2; exit 1; }
command -v readlink >/dev/null 2>&1 || { echo "readlink is not installed, aborting" >&2; exit 1; }
command -v echo >/dev/null 2>&1 || { echo "echo is not supported, aborting" >&2; exit 1; }
command -v cd >/dev/null 2>&1 || { echo "cd is not supported, aborting" >&2; exit 1; }
command -v popd >/dev/null 2>&1 || { echo "popd is not supported, aborting" >&2; exit 1; }
command -v pushd >/dev/null 2>&1 || { echo "pushd is not supported, aborting" >&2; exit 1; }

# sanity checks
DEFAULT_GDB_CMD=/usr/bin/gdb
GDB_CMD="${DEFAULT_GDB_CMD}"
if [ $# -lt 1 ]; then
 echo "INFO: using default debugger: \"${DEFAULT_GDB_CMD}\""
else
 # parse any arguments
 if [ $# -ge 1 ]; then
  GDB_CMD="$1"
  shift
 fi
fi

DEFAULT_SCRIPTS_DIR="$(dirname $(readlink -f $0))"
SCRIPTS_DIR=${DEFAULT_SCRIPTS_DIR}
# sanity check(s)
[ ! -d ${SCRIPTS_DIR} ] && echo "ERROR: invalid scripts directory (was: \"${SCRIPTS_DIR}\"), aborting" && exit 1
CAPABILITIES_SCRIPT="${SCRIPTS_DIR}/set_capabilities.sh"
[ ! -x ${CAPABILITIES_SCRIPT} ] && echo "ERROR: invalid capabilities script (was: \"${CAPABILITIES_SCRIPT}\"), aborting" && exit 1
CAPABILITY_WRAPPER="${SCRIPTS_DIR}/../../libCommon/cmake/test_i/capabilities/capability_wrapper"
[ ! -x ${CAPABILITY_WRAPPER} ] && echo "ERROR: invalid capability wrapper (was: \"${CAPABILITY_WRAPPER}\"), aborting" && exit 1
SHELL_WRAPPER="${SCRIPTS_DIR}/../../libCommon/scripts/shell_wrapper.sh"
[ ! -x ${SHELL_WRAPPER} ] && echo "ERROR: invalid shell wrapper (was: \"${SHELL_WRAPPER}\"), aborting" && exit 1
DEBUGEE="${SCRIPTS_DIR}/../cmake/src/test_i/ardrone"
[ ! -x ${DEBUGEE} ] && echo "ERROR: invalid debugee (was: \"${DEBUGEE}\"), aborting" && exit 1
ARGUMENTS="-l -t"
WORKING_DIRECTORY="${SCRIPTS_DIR}/../src/test_i"
[ ! -d ${WORKING_DIRECTORY} ] && echo "ERROR: invalid working directory (was: \"${WORKING_DIRECTORY}\"), aborting" && exit 1

# *TODO*: modify this script so it can run from kdevelop directly
#if [ ! -x ${GDB_CMD} ]; then
# echo "Usage $(basename $0) [debugger executable]"
# exit 1
#fi
#[ ! -x ${GDB_CMD} ] && echo "ERROR: invalid debugger executable (was: \"${GDB_CMD}\"), aborting" && exit 1
#echo "using debugger \"${GDB_CMD}\" with arguments: \"$@\""

${CAPABILITIES_SCRIPT} >/dev/null 2>&1
[ $? -ne 0 ] && echo "ERROR: failed to run ${CAPABILITIES_SCRIPT}: $?, aborting" && exit 1
echo "set debugee capabilities"

# remember current dir...
pushd . >/dev/null 2>&1

cd $WORKING_DIRECTORY
[ $? -ne 0 ] && echo "ERROR: failed to cd (wd was: \"${WORKING_DIRECTORY}\"): $?, aborting" && exit 1

${CAPABILITY_WRAPPER} 12 ${SHELL_WRAPPER} ${DEBUGEE} ${ARGUMENTS}
[ $? -ne 0 ] && echo "ERROR: failed to run ${CAPABILITY_WRAPPER}: $?, aborting" && exit 1

#$(${GDB_CMD}) $@
#[ $? -ne 0 ] && echo "ERROR: failed to debugger ${GDB_CMD}: $?, aborting" && exit 1

