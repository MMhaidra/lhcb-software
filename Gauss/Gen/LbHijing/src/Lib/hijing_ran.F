C $Id: ran.F,v 1.1 2009-03-06 13:24:00 robbep Exp $
c use Gaudi RndmNumberSvc as random generator
      
      FUNCTION HIJING_RAN(DUMMY)
      IMPLICIT NONE
      DOUBLE PRECISION DUMMY, CPYR
      DOUBLE PRECISION HIJING_RAN
      INTEGER IDUMMY
      HIJING_RAN=CPYR(IDUMMY)
c      write(*,*) 'MYHIJING: ran.F: hijing_RAN=',hijing_RAN
      RETURN
      END
      
