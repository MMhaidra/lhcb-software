#include "oci.h"
#include <iostream>
#include <math.h>
#include <ctime>
#include "DBparam.h"

#define MINSTRINGLEN 50
#define ERROR_BUFFER 200
#define MAXROWS 10000
//#if defined( _TEST33_) || defined( __INITIALIZE_STRUCTURE_)
//#include "test33.h"
//#endif




int NormalizeVector(char* VectorList, int nbofElement,int max_sourceLength,char* VectorDestination);
int AppendInt(int source_copy,int* &VectorList, int &Vector_currentsize, int first_time);
int AppendString(char* source_copy,char* &VectorList, int &Vector_currentsize,int &nbofElement,int &max_sourceLength, int first_time);
int ShowErrors (sword status, OCIError *errhp, const char* log);
int null_charptr(char* ptr_name);
void Format_output(int output_oraclecode, char* attribute_value, char* logmessage, char separator);
void MinStringLength(int &actual_len);
void OCIReportError(OCIError *errhp,const char* ApplicName, char* errMessg, int errnum);
void GetErrorMess(const char* ApplicName, const char* logMess,char* errMessg, int errnum); 
//int GetSubsystemList(char* subList,int &lenarray, char* subList1,int &lenarray1);
int GetSubsystemID(char * sysnameList);
int RetrieveSysIDList(char * sysnameList,char* sysIDlist);
int DecomposeSysID(int sysID,char* sysIDlist);
int GetDeviceStatus(int devstatus,char* device_status);
int GetDeviceStatusID(char* device_status);
int DecomposeSysID_bis(int sysID,char* sysIDlist);