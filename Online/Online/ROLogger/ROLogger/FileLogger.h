// $Id: FileLogger.h,v 1.2 2008-05-27 06:52:49 frankb Exp $
//====================================================================
//  ROLogger
//--------------------------------------------------------------------
//
//  Package    : ROLogger
//
//  Description: Readout monitoring in the LHCb experiment
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROLogger/ROLogger/FileLogger.h,v 1.2 2008-05-27 06:52:49 frankb Exp $
#ifndef ROLOGGER_FILELOGGER_H
#define ROLOGGER_FILELOGGER_H

// Framework include files
#include "ROLogger/MessageLogger.h"

// Forward declarations
namespace UPI { class ReallyClose; }

/*
 *   ROLogger namespace declaration
 */
namespace ROLogger {
  // Forward declarations
  class PartitionListener;

  /**@class FileLogger FileLogger.h ROLogger/FileLogger.h
   *
   *   DIM error logger
   *
   *   @author M.Frank
   */
  class FileLogger : public MessageLogger  {
    typedef std::vector<std::string> Farms;

  protected:
    /// Menu id
    int                m_id;
    /// Reference to partition listener
    PartitionListener* m_listener;
    /// Output directory
    std::string        m_outdir;
    /// Parameter buffer
    char               m_msgSeverity[16];
    /// Buffer to store HT farm content
    Farms              m_farms;
    /// Prefix for date check of messages
    std::string        m_date;
    /// Menu to shutdown application
    UPI::ReallyClose*  m_quit;

    /// Handle DIM message
    virtual void handleMessage(const char* msg);
    /// Open new output file
    FILE* openOutput();

  public:
    /// Standard constructor with object setup through parameters
    FileLogger(int argc, char** argv);
    /// Standard destructor
    virtual ~FileLogger();
    /// Interactor callback handler
    virtual void handle(const Event& ev);
  };
}
#endif /* ROLOGGER_FILELOGGER_H */
