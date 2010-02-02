// $Id: STDQSummaryAlg.h,v 1.1 2010-02-02 16:40:59 nchiapol Exp $
#ifndef STDQSummaryAlg_H
#define STDQSummaryAlg_H 1

#include "Kernel/STTupleAlgBase.h"

/** @class STDQSummaryAlg STDQSummaryAlg.h
 *
 *  Class for writing STDQ summary
 *
 *  @author M.Needham, N.Chiapolini
 *  @date   2/2/2010
 */

#include <fstream>
#include <string>

#include "Event/STCluster.h"
#include "Event/STSummary.h"
#include "STDQCounters.h"

#include <boost/accumulators/accumulators.hpp>


class STDQSummaryAlg : public ST::TupleAlgBase {

public:
 
  /// constructer
  STDQSummaryAlg(const std::string& name, ISvcLocator *svcloc );

  /// destructor
  virtual ~STDQSummaryAlg();

  /// initialize
  virtual StatusCode initialize();

  /// execute
  StatusCode execute();

  /// finalize
  StatusCode finalize();

private:
  /// write run summary into storage vector (m_dataStorage)
  void outputInfo();

  /// reset all counters (delets and recreates Counters)
  void resetCounters();

  /// get data from event and add to Counters
  void processEvent(const LHCb::STClusters* clusters, 
                    const LHCb::STSummary* summary) const;
  
  /// calculate the processed efficiency
  double calculateProcEff(const unsigned int corrupted, 
			  const LHCb::STSummary::RecoveredInfo& recovered) const;

  /// count events with signal (S/N > m_threshold)
  unsigned int countHigh(const LHCb::STClusters* clusters) const;

  /// write content of m_dataStorage to text file
  void writeTxtFile();     
  
  /// write content of m_dataStorage to tuple
  void writeTuple();
  
  /// write data-value to text stream (used by writeTxtFile())
  template <typename TYPE>
    void writeTxtEntry(std::ofstream& str, TYPE data, int width);

  
  int m_lastRunNumber;            ///< RunNumber of last event
  std::string m_summaryLocation;  ///< data location
  std::string m_clusterLocation;  ///< data location 
 
  /// Struct containing the data collected for one run
  struct DataRow {
    int          run;             ///< run number
    unsigned int event;           ///< number of events
    double       clus;            ///< mean number of clusters
    double       noise;           ///< mean number of noise clusters
    double       procEff;         ///< mean processing efficiency
    int          corrupted;       ///< total number of corrupted banks
    int          sumMissing;      ///< total number of missing banks
  };
  /// temporary storage vector contains data of all events
  std::vector<DataRow> m_dataStorage;
  
  /// Class with all boost::accumulators.
  STDQCounters *Counters;

  bool m_writeTxtFile;
  bool m_writeTuple;              
  std::string m_outputFileName;   ///< filename for the text file
  std::string m_separator;        ///< column separator in text file


  double m_threshold;             ///< threshold for signal (S/N)

  typedef std::vector<std::string> Strings;
  Strings m_txtColumns;           ///< text file column headers

};

template <typename TYPE>
inline void STDQSummaryAlg::writeTxtEntry(std::ofstream& str,  TYPE data, int width){
  str << m_separator << " " << std::setw(width) << data << " " ; 
}

#endif 
