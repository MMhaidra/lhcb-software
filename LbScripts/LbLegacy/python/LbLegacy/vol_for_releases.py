""" Script create a new afs volume for a new project version
 060616 - first version
 060623 - prepare the script to be called in mkproject.py
 060925 - set python_version and day
 061031 - fix size
 070306 - add ANALYSIS
"""
#-----------------------------------------------------------------------------

import logging
import os

from  getpass import getuser

from LbConfiguration.Project import getProject
from LbUtils.afs.volume import createVolume

#-----------------------------------------------------------------------------------
def create_volume(pname, pversion, size=0):
    log = logging.getLogger()

    projconf = getProject(pname, svn_fallback=True)

    if not size :
        size = projconf.FullSize()

    # get user name
    MYNAME = getuser()

    # project directory, afs group, volume root
    PNAME = projconf.NAME()
    P_DIR = projconf.ReleaseArea()
    group = projconf.AFSLibrarianGroup()

    # set the volume name
    vol_name = projconf.AFSReleaseVolumeName(pversion)

    PPATH = os.path.join(P_DIR, PNAME, PNAME + '_' + pversion)
    if not os.path.exists(PPATH):
        if os.path.exists(os.path.dirname(PPATH)):
            createVolume(PPATH, vol_name, size, MYNAME, group)
        else:
            log.error('The project %s does not exist on %s', PNAME, P_DIR)
            return False
    else:
        log.warning('The version %s of the project %s already exists',
                    pversion, projconf.Name())

    return True
