#!/bin/bash
echo with extra arguments $*
. ./setupOnline.sh
export MSGSVC=MessageSvc
${gaudi_exe} ${gaudi_exe_args} -opt=../options/MEPRxSvc.opts\
    -msgsvc=${MSGSVC}  &
