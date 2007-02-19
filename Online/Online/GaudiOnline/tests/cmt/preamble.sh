#/bin/sh
killall test.exe
killall Gaudi.exe
killall gentest.exe
rm /dev/shm/bm_* /dev/shm/sem.bm_* /dev/shm/TAN* /dev/shm/sem.TAN*
#
export TAN_PORT=YES
export TAN_NODE=$HOSTNAME
export DIM_DNS_NODE=$HOSTNAME
#
export OPTS=$GAUDIONLINEROOT/options
export msg_svc=MessageSvc
export msg_svc=LHCb::DimMessageSvc

export test_exe="$ONLINEKERNELROOT/$CMTCONFIG/test.exe "
export gaudi_run="$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe libGaudiOnline.so OnlineStart "
export gaudi_exe="$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe libGaudiOnline.so OnlineTask -msgsvc=$msg_svc -auto "
export gaudi_exe2="$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe libGaudiOnline.so OnlineTask -msgsvc=MessageSvc -auto "
export MINITERM='xterm  -ls -132 -geometry 132x12 -title '
export BIGTERM='xterm  -ls -132 -geometry 132x45 -title '
export WIDETERM='xterm  -ls -132 -geometry 160x50 -title '
#export MINITERM=echo
#
