#!/bin/bash
# =========================================================================
#
#  Farm worker node controller startup script
#
#  Author   M.Frank
#  Version: 1.0
#  Date:    20/05/2013
#
# =========================================================================
#
# Output:   -print=${OUTPUT_LEVEL} \
# Debug:    -sleep=20 \
#
#
HH=`hostname -s | tr a-z A-Z`;
if test "${MOORESTARTUP_MODE}" = "RESTORE";
then
    mkdir -p /tmp/Commands; # Needed by the tasks
    if test "${HH}" = "`echo ${HH} | grep -i -e hlt[a-f][0-1][0-9][0-3][0-9]`"; then
	if test "${PARTITION_NAME}" = "LHCb"; then
	    ##echo `python ${FARMCONFIGROOT}/job/ConfigureCheckpoint.py -r ${RUNINFO} -c -l` | tr ";" ";\n";
	    eval `python ${FARMCONFIGROOT}/job/ConfigureCheckpoint.py -r ${RUNINFO} -c -l`;
	elif test "${PARTITION_NAME}" = "LHCb2"; then
	    eval `python ${FARMCONFIGROOT}/job/ConfigureCheckpoint.py -r ${RUNINFO} -c -l -t Moore2`;
	elif test "${PARTITION_NAME}" = "FEST"; then
	    eval `python ${FARMCONFIGROOT}/job/ConfigureCheckpoint.py -r ${RUNINFO} -c -l`;
	fi;
    fi;
fi;
#
#
#
ctrl_args="-count=${NBOFSLAVES} -bindcpu=0";
PRINT_LEVEL=4;
#
if test "${PARTITION_NAME}" = "LHCb"; then
    #PRINT_LEVEL=3;
    ctrl_args="${ctrl_args} -service=${UTGID}/NTasks"
fi;
if test "${PARTITION_NAME}" = "LHCb2"; then
    # This will make the controller wait until on PAUSE all slaves are paused.
    #PRINT_LEVEL=3;
    PRINT_LEVEL=4;
    #ctrl_args="-fsm=DAQPause";
    ctrl_args="${ctrl_args} -service=${UTGID}/NTasks"
    echo "[INFO] ${UTGID} Partition:${PARTITION_NAME} FSM protocol: ${ctrl_args} Print-Level:${PRINT_LEVEL}";
#    if test "${HH}" = "HLTC0411"; then
#	ctrl_args="-count=22 -bindcpu=0";
#    fi;
fi;
#if test "${HH}" = "HLTA0328"; then
#  if test "${PARTITION_NAME}" = "LHCb"; then
#    cd /home/frankm/cmtuser/OnlineDev_v5r24;
#    . setup.x86_64-slc6-gcc48-dbg.vars;
#    ctrl_args="$ctrl_args -service=/${HH}/num_tasks";
#    #PRINT_LEVEL=2;
#    echo "[INFO] ${UTGID} `pwd`";
#  fi;
#fi;
#
#
#
exec -a ${UTGID} fsm_ctrl.exe \
    -print=${PRINT_LEVEL} -sleep=0 \
    -partition=${PARTITION_NAME} \
    -runinfo=${RUNINFO} \
    -mode=${MOORESTARTUP_MODE} \
    -taskconfig=${ARCH_FILE} \
    -maxcount=40 \
    ${ctrl_args}
