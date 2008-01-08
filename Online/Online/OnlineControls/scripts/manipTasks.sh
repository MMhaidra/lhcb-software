#!/bin/bash
EXECS=/opt/FMC/sbin:/opt/FMC/bin:${PATH};
export LD_LIBRARY_PATH=/opt/FMC/lib;
cd `dirname ${0}`
cmd=`/usr/bin/python -c "import dataflowBoot; dataflowBoot.${1}()"`
eval $cmd
