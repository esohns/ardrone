#!/bin/sh
# author:      Erik Sohns <eriksohns@123mail.org>
# this script sets up the required system configuration
# arguments:   [install|uninstall] [-d]
# return value: - 0 success, 1 failure

# sanity checks
command -v sudo >/dev/null 2>&1 || { echo "sudo is not installed, aborting" >&2; exit 1; }
#command -v gksudo >/dev/null 2>&1 || { echo "gksudo is not installed, aborting" >&2; exit 1; }
command -v exec >/dev/null 2>&1 || { echo "exec is not installed, aborting" >&2; exit 1; }

# *NOTE*: regular users may not have the CAP_SETFCAP capability needed to modify
#         (executable) file capabilities --> run as root
# *TODO*: verify this programmatically
HAS_GKSUDO=0
if [ -x gksudo ]; then
 HAS_GKSUDO=1
fi

if [ "${USER}" != "root" ]; then
 SUDO=sudo
 CMDLINE_ARGS="$@"
 if [ ${HAS_GKSUDO} -eq 1 ]; then
  SUDO=gksudo
  CMDLINE_ARGS="--disable-grab $0 $@"
 fi
# echo "invoking sudo $0 \"${CMDLINE_ARGS}\"..."
 exec ${SUDO} $0 ${CMDLINE_ARGS}
fi
#echo "starting..."

# sanity checks
#command -v chgrp >/dev/null 2>&1 || { echo "chgrp is not installed, aborting" >&2; exit 1; }
#command -v chmod >/dev/null 2>&1 || { echo "chmod is not installed, aborting" >&2; exit 1; }
#command -v chown >/dev/null 2>&1 || { echo "chown is not installed, aborting" >&2; exit 1; }
command -v dirname >/dev/null 2>&1 || { echo "dirname is not installed, aborting" >&2; exit 1; }
command -v echo >/dev/null 2>&1 || { echo "echo is not supported, aborting" >&2; exit 1; }
command -v getopts >/dev/null 2>&1 || { echo "getopts is not installed, aborting" >&2; exit 1; }
command -v groups >/dev/null 2>&1 || { echo "groups is not installed, aborting" >&2; exit 1; }
command -v logname >/dev/null 2>&1 || { echo "logname is not installed, aborting" >&2; exit 1; }
#command -v popd >/dev/null 2>&1 || { echo "popd is not installed, aborting" >&2; exit 1; }
#command -v pushd >/dev/null 2>&1 || { echo "pushd is not installed, aborting" >&2; exit 1; }
command -v readlink >/dev/null 2>&1 || { echo "readlink is not installed, aborting" >&2; exit 1; }
command -v tr >/dev/null 2>&1 || { echo "tr is not installed, aborting" >&2; exit 1; }
command -v setcap >/dev/null 2>&1 || { echo "setcap is not installed, aborting" >&2; exit 1; }
command -v getcap >/dev/null 2>&1 || { echo "getcap is not installed, aborting" >&2; exit 1; }

MODE=""
if [ $# -ge 1 ]; then
 case $1 in
  install | uninstall)
   MODE=$1
   shift
   ;;
  *)
   echo "ERROR: invalid/unknown mode (was: \"$1\"), aborting" >&2 && exit 1
   ;;
 esac
 echo "INFO: mode \"${MODE}\"..."
fi

DEBUG=0
while getopts ":d" option; do
 case ${option} in
  d)
   DEBUG=1
   echo "INFO: debugging..."
   ;;
  \?)
   echo "ERROR: Invalid option: -$OPTARG" >&2 && exit 1
   ;;
  :)
   echo "ERROR: Option -$OPTARG requires an argument" >&2 && exit 1
   ;;
 esac
done

DEFAULT_PROJECT_DIR="$(dirname $(readlink -f $0))/.."
PROJECT_DIR=${DEFAULT_PROJECT_DIR}
# sanity check(s)
[ ! -d ${PROJECT_DIR} ] && echo "ERROR: invalid project dir (was: \"${PROJECT_DIR}\"), aborting" >&2 && exit 1

#DEFAULT_BUILD="Debug"
#BUILD=${DEFAULT_BUILD}
#if [ $# -lt 1 ]; then
# echo "INFO: using default build: \"${BUILD}\""
#else
 # parse any arguments
# if [ $# -ge 1 ]; then
#  BUILD="$1"
# fi
#fi
DEFAULT_TMP_DIR="${TEMP}"
TMP_DIR="$DEFAULT_TMP_DIR"
if [ -z "${TMP_DIR}" ]; then
 TMP_DIR="/tmp"
fi
[ ! -d ${TMP_DIR} ] && echo "ERROR: invalid tmp dir (was: \"${TMP_DIR}\"), aborting" >&2 && exit 1
#echo "using tmp directory \"$TMP_DIR\"..."

# sanity check(s)
#[ ${BUILD} != "Debug" -a ${BUILD} != "debug_tracing" -a ${BUILD} != "release" ] && echo "WARNING: invalid/unknown build (was: \"${BUILD}\"), continuing"
BUILD_DIR="${PROJECT_DIR}/cmake"
[ ! -d "${BUILD_DIR}" ] && echo "ERROR: invalid build dir (was: \"${BUILD_DIR}\"), aborting" >&2 && exit 1

SRC_DIR="${PROJECT_DIR}/src"
[ ! -d "${SRC_DIR}" ] && echo "ERROR: invalid source dir (was: \"${SRC_DIR}\"), aborting" >&2 && exit 1

# step 1: set executable file capabilities

#SUB_DIRS="ardrone"
#declare -a LIBS=("libACE.so")
BINS="ardrone"
i=0
#for DIR in $SUB_DIRS
#do
# LIB="${MODULES_DIR}/${DIR}/${LIB_DIR}/${LIBS[$i]}"
 BIN="${BUILD_DIR}/src/${BINS}"
 [ ! -r "${BIN}" ] && echo "ERROR: invalid binary file (was: \"${BIN}\"), aborting" >&2 && exit 1

# echo "processing \"$BIN\"..."

# cp -f ${BIN} ${TMP_DIR}
# [ $? -ne 0 ] && echo "ERROR: failed to cp ${BIN}: \"$?\", aborting" && exit 1
# echo "copied \"$BIN\"..."

# BIN_TMP="${TMP_DIR}/${BINS}"
# [ ! -r "${BIN_TMP}" ] && echo "ERROR: invalid binary file (was: \"${BIN_TMP}\"), aborting" && exit 1

# *NOTE*: test_i/ardrone needs 'cap_net_admin' to scan the WLAN, but gtk does
#         not support set(g/u)id programs ATM (see:
#         https://www.gtk.org/setuid.html)
#         --> use a (setuid) capabilities wrapper (see:
#             libCommon/test_i/capabilities/capability_wrapper) and file
#             capabilities (see: scripts/set_capabilities.sh) to grant
#             'cap_net_admin' to test_i/ardrone
# chown --quiet root ${BIN_TMP}
# chown --quiet root:root ${BIN}
# [ $? -ne 0 ] && echo "ERROR: failed to chown ${BIN}: \"$?\", aborting" && exit 1
# echo "modified \"$BINS\": user:group"
# chgrp --quiet root ${BIN}
# [ $? -ne 0 ] && echo "ERROR: failed to chgrp ${BIN}: \"$?\", aborting" && exit 1
# chmod --quiet +s ${BIN_TMP}
# chmod --quiet ug+s ${BIN}
# [ $? -ne 0 ] && echo "ERROR: failed to chmod u+s ${BIN}: \"$?\", aborting" && exit 1
# echo "modified \"$BINS\": uid gid suid sgid"

 setcap 'cap_dac_override,cap_fowner,cap_net_admin+eip' ${BIN}
 [ $? -ne 0 ] && echo "ERROR: failed to setcap ${BIN}: \"$?\", aborting" >&2 && exit 1

 CMD_OUTPUT=$(getcap ${BIN})
# echo "CMD_OUTPUT \"$CMD_OUTPUT\""
 j=0
 for k in $(echo $CMD_OUTPUT | tr " " "\n")
 do
#  echo "$j: \"$k\""
  if [ $j -eq 2 ]
  then
   CAPABILITIES=$k
  fi
  j=$(($j+1))
 done
 echo "modified \"$BINS\": ${CAPABILITIES}..."
# i=$(($i+1))
#done

# step2: add user to 'netdev' group, if necessary

REAL_USER=$(logname)
#echo "$REAL_USER: \"$REAL_USER\""
GROUPS=$(groups $REAL_USER)
#echo "GROUPS: \"$GROUPS\""
is_netdev_member=0
for k in $(echo $GROUPS | tr " " "\n")
do
# echo "$k: \"$k\""
 if [ "$k" = "netdev" ]
 then
  is_netdev_member=1
 fi
done
if [ ${is_netdev_member} -eq 0 ]
then
 usermod -a -G netdev $REAL_USER
 [ $? -ne 0 ] && echo "ERROR: failed to usermod: \"$?\", aborting" >&2 && exit 1
# *NOTE*: (ubuntu) gnome users may have to restart the system to apply this
#         change; logging back in apparently does not work
# *TODO*: find out why
 echo "added $REAL_USER to \"netdev\"; log out and back in to effectuate..."
fi

if [ "${MODE}" = "install" ] || [ "${MODE}" = "uninstall" ]
then
 ARGUMENTS=""
 case $MODE in
  install)
   ARGUMENTS="-y"
   echo "installing \"$BINS\"..."
   ;;
  uninstall)
   ARGUMENTS="-x"
   echo "uninstalling \"$BINS\"..."
   ;;
  *)
   echo "ERROR: invalid/unknown mode (was: \"$MODE\"), aborting" >&2 && exit 1
   ;;
 esac
 if [ $DEBUG -gt 0 ]; then
  ARGUMENTS="$ARGUMENTS -l -t"
 fi
# echo "ARGUMENTS: \"$ARGUMENTS\"..."

# remember current dir...
 pushd . >/dev/null 2>&1

 cd ${SRC_DIR} >/dev/null 2>&1
 [ $? -ne 0 ] && echo "ERROR: failed to cd: \"$?\", aborting" >&2 && exit 1

 ${BIN} ${ARGUMENTS}
 echo "...DONE"

 popd >/dev/null 2>&1
fi
