# test file. The real file has to be generated by the script generateLogin

if [ ! -e ${HOME}/.oldLHCBLoginscript ]; then
  if [ -n "$LB_BANNER" ]; then
    cat ${LB_BANNER}
    rm -f ${LB_BANNER}
    unset LB_BANNER
  else
    . /afs/cern.ch/lhcb/software/releases/LBSCRIPTS/prod/InstallArea/scripts/LbLogin.sh --quiet
  fi
  export LBLOGIN_DONE=yes
fi

