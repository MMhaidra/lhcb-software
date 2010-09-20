#ifndef ROMON_ROMONDEF_H
#define ROMON_ROMONDEF_H
#include <iostream>
#include <string>
#include <cstdarg>

/*
 *   ROMon namespace declaration
 */
namespace ROMon {
  std::ostream& log();
  void print_startup(const char* msg);
  void ro_gettime(int* seconds, unsigned int* milli_seconds);
  std::string strupper(const std::string& n);
  std::string strlower(const std::string& n);
  void ro_trl_set_print_level(int lvl);
  size_t ro_rtl_print(void*,int,const char* fmt,va_list args);
}
#endif // ROMON_ROMONDEF_H
