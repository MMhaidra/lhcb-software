C $Id: gcoloct.F,v 1.1.1.1 2006-04-24 21:45:50 robbep Exp $
      INTEGER*4 FUNCTION COLOCT_ADDRESS() 
      IMPLICIT NONE

      INTEGER IOCTET
      COMMON/COLOCT/IOCTET
 
      INTEGER*4 GETADDR

      COLOCT_ADDRESS = GETADDR(IOCTET)
      RETURN
      END
