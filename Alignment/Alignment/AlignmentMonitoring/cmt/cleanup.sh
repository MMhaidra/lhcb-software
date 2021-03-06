if test "${CMTROOT}" = ""; then
  CMTROOT=/afs/cern.ch/sw/contrib/CMT/v1r20p20090520; export CMTROOT
fi
. ${CMTROOT}/mgr/setup.sh
tempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if test ! $? = 0 ; then tempfile=/tmp/cmt.$$; fi
${CMTROOT}/mgr/cmt cleanup -sh -pack=AlignmentMonitoring -version=v0r1 -path=/afs/cern.ch/user/m/mamartin/cmtuser/Alignment_v10r4/Alignment $* >${tempfile}; . ${tempfile}
/bin/rm -f ${tempfile}

