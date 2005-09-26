#define RTL_IMPLEMENTATION
#include <map>
#include <cstdio>
#include <cstdlib>
#include <string>
#include "RTL/rtl.h"
#include "fcntl.h"

#ifdef VMS
#define lock_prio  14
typedef struct  {
  short len,code;
  int addr,retaddr;
}  TABNAM_LIST;
static int def_prio;
#include <secdef.h>
#include <psldef.h>

#elif linux
#include "unistd.h"
#include <sys/mman.h>

#elif _WIN32
#include <windows.h>
#endif

namespace {
  struct GblLock  {
    GblLock() {}
    int status() const { return 1; }
    operator bool()  const { return true; }
  };
  struct GblDesc  {
    int         size;
    int         fd;
    void*       address[2];
    std::string name;
    GblDesc() : size(0), fd(0) { address[0] = address[1] = 0; }
    GblDesc(const GblDesc& c) : size(c.size), fd(c.fd), name(c.name) {
      address[0] = c.address[0];
      address[1] = c.address[1];
    }
  };
  typedef std::map<std::string, GblDesc> GblDescMap;
  struct NamedGblHandlers : public GblDescMap  {
  };
  GblDescMap& namedGblHandlers() {
    static NamedGblHandlers s_Handlers;
    return s_Handlers;
  }
}


/// Create named global section
int lib_rtl_create_section(const char* sec_name, int size, void* address) {
  GblDescMap::const_iterator i = namedGblHandlers().find(sec_name);
  if ( i == namedGblHandlers().end() )  {
    int* add = (int*)address;
#ifdef VMS
    int inadd[2]={0,0};
    DESCRIPTOR(name,sec_name);
    int size  = (size + 511)>>9;
    int flags   = SEC$M_GBL+SEC$M_EXPREG+SEC$M_WRT+SEC$M_PERM+SEC$M_SYSGBL+SEC$M_PAGFIL;
    str$upcase (&name, &name);
    int sc = sys$crmpsc (inadd, add, PSL$C_USER, flags, &name, 0,0,0,size,0,0,0); 
    if ( sc & 1 )  {
      return sc;
    }
#elif linux
    int fd = 0;
    int sysprot  = PROT_READ+PROT_WRITE;
    int sysflags = MAP_SHARED;
    std::string n = "/";
    n += sec_name;
    fd = shm_open(n.c_str(),O_RDWR|O_CREAT,0644);
    if ( fd ) {
      ::ftruncate(ft, size);
    }
    add[1] = fd;
    add[0] = (int)mmap (0, size, sysprot, sysflags, fd, 0);
    if ( add[0] != 0 )  {
      GblDesc dsc;
      dsc.fd = fd;
      dsc.name = sec_name;
      dsc.address[0] = add[0];
      namedGblHandlers().insert(dsc); 
      return 1;
    }
    ::shm_unlink(n.c_str());
    return 0;
#elif _WIN32
    // Setup inherited security attributes (FIXME: merge somewhere else)
    SECURITY_ATTRIBUTES	sa = {sizeof(SECURITY_ATTRIBUTES), NULL, true};
    DWORD  siz  = (size/4096)*4096 + (((size%4096)==0) ? 0 : 4096);  //  multiple of page size
    HANDLE hMap = ::CreateFileMapping(INVALID_HANDLE_VALUE,&sa,PAGE_READWRITE,0,siz,sec_name);
    if (hMap != NULL && ::GetLastError() == ERROR_ALREADY_EXISTS)   { 
      ::CloseHandle(hMap); 
      hMap = ::OpenFileMapping(FILE_MAP_ALL_ACCESS,FALSE,sec_name);
    }
    if ( hMap )  {
      add[0] = (int)::MapViewOfFile(hMap,FILE_MAP_ALL_ACCESS,0,0,0);
      if ( add[0] != 0 )  {
        add[1] = (int)hMap;
        return 1;
      }
    }
    return 0;
#endif
  }
  return 0;
}

/// Delete named global section
int lib_rtl_delete_section(const char *sec_name)    {
  GblDescMap::const_iterator i = namedGblHandlers().find(sec_name);
  if ( i != namedGblHandlers().end() )  {
#ifdef linux
    std::string n = "/";
    n += sec_name;
    return shm_unlink(n.c_str()) ==0 ? 1 : 0;
#else
    return 1;
#endif
  }
  return 0;
}

/// Map global section a a specific address
int lib_rtl_map_section(const char* sec_name, int size, void* address)   {
  int* add = (int*)address;
#ifdef VMS
  DESCRIPTOR(name,sec_name);
  int inadd[2]={0,0};
  int flags   = SEC$M_EXPREG+SEC$M_WRT+SEC$M_SYSGBL;
  str$upcase (&name, &name);
  int status = sys$mgblsc (inadd,add,PSL$C_USER,flags,&name,0,0);
  return status;
#elif linux
  return lib_rtl_create_section(sec_name, size, address);
#elif _WIN32
  HANDLE hMap = ::OpenFileMapping(FILE_MAP_ALL_ACCESS,FALSE,sec_name);
  if ( hMap )  {
    add[0] = (int)::MapViewOfFile(hMap,FILE_MAP_ALL_ACCESS,0,0,0);
    if ( add[0] != 0 )  {
      add[1] = (int)hMap;
      return 1;
    }
  }
  return 0;
#endif
}

/// Unmap global section: address is quadword: void*[2]
int lib_rtl_unmap_section(void* address)   {
#ifdef VMS
  int radd[2];
  int* inadd = (int*)address;
  int status = sys$deltva (inadd, radd, PSL$C_USER);
  return sys$purgws (inadd);
#elif linux
  void** inadd = (void**)address;
  int sc = ::munmap(inadd[0],1024*1024*1024);
  return sc==0 ? 1 : 1;  
#elif _WIN32
  int* inadd = (int*)address;
  return (::UnmapViewOfFile((void*)inadd[0]) == 0) ? 0 : 1;
#endif
}

/// Flush global section to disk file
int lib_rtl_flush_section(void* address, int len)   {
#if _WIN32
  int* inadd = (int*)address;
  DWORD sc = ::FlushViewOfFile((void*)inadd[0],0);
#elif linux
  void** inadd = (void**)address;
  ::msync(inadd[0],len,MS_INVALIDATE|MS_SYNC);
#endif
  return 1;
}
