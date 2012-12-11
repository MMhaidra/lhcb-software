#ifndef TUPLETOOLRECOSTATS_H
#define TUPLETOOLRECOSTATS_H 1

// Include files
// from Gaudi
#include "Kernel/IEventTupleTool.h"            // Interface
#include "DecayTreeTupleBase/TupleToolBase.h"

#include "Event/Track.h"
#include "Event/RecSummary.h"

/** @class TupleToolVELOClusters TupleToolVELOClusters.h
 *
 *  Fills Reco stats, from RecSummary
 *
 *  @author Patrick Koppenburg, Fatima Soomro, Jibo He
 *  @date   2009-02-11
 */
class TupleToolVELOClusters : public TupleToolBase, 
                           virtual public IEventTupleTool
{

public:

  /// Standard constructor
  TupleToolVELOClusters( const std::string& type,
                      const std::string& name,
                      const IInterface* parent);

  virtual ~TupleToolVELOClusters( ); ///< Destructor
  virtual StatusCode fill( Tuples::Tuple& );///< Fill tuple
  int unusedVelo();

};

#endif // TUPLETOOLRECOSTATS_H

