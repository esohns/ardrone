#!/bin/sh
#//%%%FILE%%%////////////////////////////////////////////////////////////////////
#// File Name: scanner.sh
#//
#// History:
#//   Date   |Name | Description of modification
#// ---------|-----|-------------------------------------------------------------
#// 20/02/06 | soh | Creation.
#//%%%FILE%%%////////////////////////////////////////////////////////////////////

# sanity check(s)
command -v dirname >/dev/null 2>&1 || { echo "dirname is not installed, aborting" >&2; exit 1; }
command -v flex >/dev/null 2>&1 || { echo "flex is not installed, aborting" >&2; exit 1; }
command -v tee >/dev/null 2>&1 || { echo "tee is not installed, aborting" >&2; exit 1; }

PROJECT_ROOT=$(dirname $0)/..
#[ ! -d ./${PROJECT_ROOT} ] && echo "ERROR: invalid directory (was: ${PROJECT_ROOT}), aborting" && exit 1
SCRIPTS_DIR=${PROJECT_ROOT}/scripts
[ ! -d ${SCRIPTS_DIR} ] && echo "ERROR: invalid directory (was: ${SCRIPTS_DIR}), aborting" && exit 1
FILES="control_scanner.l mavlink_scanner.l navdata_scanner.l"
for FILE in $FILES
do
 [ ! -f ${SCRIPTS_DIR}/${FILE} ] && echo "ERROR: invalid file (was: ${SCRIPTS_DIR}/${FILE}), aborting" && exit 1
done

# -------------------------------------------------------------------

# generate flex scanners
for FILE in $FILES
do
 flex --noline ${SCRIPTS_DIR}/${FILE} 2>&1 | tee ${SCRIPTS_DIR}/scanner_report.txt
 [ $? -ne 0 ] && echo "ERROR: failed to flex \"${FILE}\", aborting" && exit 1
done

FILES="ardrone_control_scanner.cpp ardrone_control_scanner.h ardrone_mavlink_scanner.cpp ardrone_mavlink_scanner.h ardrone_navdata_scanner.cpp ardrone_navdata_scanner.h"

# move the files into the project directory
for FILE in $FILES
do
mv -f $FILE ${PROJECT_ROOT}
if [ $? -ne 0 ]; then
 echo "ERROR: failed to mv \"$FILE\", aborting"
 exit 1
fi
echo "moved \"$FILE\"..."
done

