#!/usr/bin/env python
# -*- coding: utf-8 -*-
# $Id$
# =============================================================================
# A Python Library to create a Progress Bar.
# Copyright (C) 2008  BJ Dierkes <wdierkes@5dollarwhitebox.org>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# =============================================================================
#
# This class is an improvement from the original found at:
#
#   http://code.activestate.com/recipes/168639/
#
# =============================================================================
#                   $Revision$
# Last modification $Date$
# =============================================================================
"""

from PyPAW.progress_bar import ProgressBar
import sys
 
count = 0
total = 100000
 
prog = ProgressBar(count, total, 77, mode='fixed', char='#')
while count <= total:
    count += 1
    prog.increment_amount()
    print prog, '\r',
    sys.stdout.flush()

"""
# =============================================================================
import warnings
warnings.warn (
    """PyPAW:
    Use 'Ostap.progress_bar' module instead of 'PyPAW.progress_bar'""",
    DeprecationWarning ,
    stacklevel   = 3
    )
# =============================================================================
## the actual import 
from Ostap.progress_bar import *

# =============================================================================
if '__main__' == __name__ :
    main()
    
# ====================================================================---------
# The END 
# ====================================================================---------

