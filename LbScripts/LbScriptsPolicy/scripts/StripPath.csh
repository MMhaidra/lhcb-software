if (! ${?LB_NO_STRIP_PATH} ) then
  set tmpfile = `runpy LbScriptsPolicy.PathStripper --shell=csh --mktemp -e PATH -e LD_LIBRARY_PATH -e PYTHONPATH -e HPATH `
  source $tmpfile
  rm -f $tmpfile
  unset tmpfile
  unsetenv LB_NO_STRIP_PATH
endif
