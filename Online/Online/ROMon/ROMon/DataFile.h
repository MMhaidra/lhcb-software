// $Id: DataFile.h,v 1.1 2010-09-20 19:00:10 frankb Exp $
//====================================================================
//  ROMon
//--------------------------------------------------------------------
//
//  Package    : ROMon
//
//  Author     : M.Frank
//  Created    : 20/09/2010
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/DataFile.h,v 1.1 2010-09-20 19:00:10 frankb Exp $
#ifndef ONLINE_ROMON_DATAFILE_H
#define ONLINE_ROMON_DATAFILE_H

#include <string>
#include <sys/types.h>
#include <sys/stat.h>

/*
 *   ROMon namespace declaration
 */
namespace ROMon {

  /**@class DataFile BootMon.h ROMon/BootMon.h
   *
   * Class used to read the messages file from the boot logger node
   *
   * @author M.Frank
   */
  class DataFile {
  protected:
    /// The file descriptor for unbuffered I/O
    int           m_fd;
    /// The file name
    std::string   m_name;
    /// The stat information for the file
    struct stat   m_stbuff;
    /// Current read pointer within the file
    size_t        m_pointer;

  public:
    /// @class DataProcessor BootMon.h ROMon/BootMon.h
    struct DataProcessor {
      /// Worker callback signature
      virtual bool operator()(const char* line) const = 0;
    };

  public:
    /// Initializing constructor
    DataFile(const std::string& n);
    /// Default destructor
    virtual ~DataFile();
    /// Access file name
    const std::string& name() const { return m_name; }
    /// Open the data file
    bool open();
    /// Close the data file
    void close();
    /// Check if file migrated
    bool migrated()  const;
    /// Process file netries line by line using functor object
    int scan(const DataProcessor& functor);
  };
}      // End namespace ROMon

#endif // ONLINE_ROMON_DATAFILE_H
