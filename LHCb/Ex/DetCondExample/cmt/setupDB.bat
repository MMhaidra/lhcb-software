@echo off
rem $Id: setupDB.bat,v 1.5 2002-12-03 17:33:37 andreav Exp $
if "%CONDDB_implementation%." == "." ( 
  echo ERROR: the environment variable CONDDB_implementation is not set
  echo Please "set CONDDB_implementation=<value>" and try again
  echo Valid values are {CondDBOracle, CondDBMySQL}
) ELSE if "%CONDDB_implementation%" == "CondDBOracle" (
  call %DETCONDEXAMPLEROOT%\cmt\setupDB_Oracle.bat
) ELSE if "%CONDDB_implementation%" == "CondDBMySQL" (
  call %DETCONDEXAMPLEROOT%\cmt\setupDB_MySQL.bat
) ELSE (
  echo CONDDB_implementation is set to %CONDDB_implementation%
  echo ERROR: valid values are {CondDBOracle, CondDBMySQL}
)

