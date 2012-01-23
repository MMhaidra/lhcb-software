// $Id: $
#ifndef PACKDECREPORT_H 
#define PACKDECREPORT_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class PackDecReport PackDecReport.h
 *  
 *
 *  @author Olivier Callot
 *  @date   2012-01-20
 */
class PackDecReport : public GaudiAlgorithm {
public: 
  /// Standard constructor
  PackDecReport( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PackDecReport( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  std::string m_inputName;
  std::string m_outputName;
  bool m_alwaysOutput;      ///< Flag to turn on the creation of output, even when input is missing
  bool m_filter;
};
#endif // PACKDECREPORT_H
