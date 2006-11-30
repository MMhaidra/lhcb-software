
//-----------------------------------------------------------------------------
/** @file RichMCCKPhotonYieldAlg.h
 *
 * Header file for monitor algorithm RichMCCKPhotonYieldAlg
 *
 * CVS Log :-
 * $Id: RichMCCKPhotonYieldAlg.h,v 1.3 2006-11-30 14:31:19 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 2006-11-03
 */
//-----------------------------------------------------------------------------

#ifndef RICHMCMONITORS_RichMCCKPhotonYieldAlg_H
#define RICHMCMONITORS_RichMCCKPhotonYieldAlg_H 1

// base class
#include "RichKernel/RichHistoAlgBase.h"

// MCEvent
#include "Event/MCRichHit.h"
#include "Event/MCRichTrack.h"

// RichKernel
#include "RichKernel/RichMap.h"
#include "RichKernel/RichStatDivFunctor.h"

// tool Interfaces
#include "RichKernel/IRichMCTruthTool.h"

// boost
#include "boost/assign/list_of.hpp"

// GSL
#include "gsl/gsl_math.h"

/** @namespace Rich
 *
 *  General namespace for RICH specific definitions
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
namespace Rich
{

  //-----------------------------------------------------------------------------
  /** @class RichMCCKPhotonYieldAlg RichMCCKPhotonYieldAlg.h
   *
   *  Monitor algorithm to study the RICH CK photon yield using
   *  only MCRichHit and MCParticle data objects
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   2006-11-03
   */
  //-----------------------------------------------------------------------------

  class RichMCCKPhotonYieldAlg : public RichHistoAlgBase
  {

  public:

    /// Standard constructor
    RichMCCKPhotonYieldAlg( const std::string& name, ISvcLocator* pSvcLocator );

    virtual ~RichMCCKPhotonYieldAlg( ); ///< Destructor

    virtual StatusCode initialize();    ///< Algorithm initialization
    virtual StatusCode execute   ();    ///< Algorithm execution
    virtual StatusCode finalize  ();    ///< Algorithm finalization

  private: // utility classes

    /// Tally class to store the number of photons for a number of tracks
    class TrackPhotonTally
    {
    public:
      /// Constructor with optional number of photons and tracks
      TrackPhotonTally( const unsigned int tracks  = 0,
                        const unsigned int photons = 0 )
        : nTracks(tracks), nPhotons(photons) { }
      long unsigned int nTracks;   ///< tally for the number of tracks
      long unsigned int nPhotons;  ///< tally for the number of photons
    };

  private: // definitions

    /// A vector of pointers to MCRichHits
    typedef std::vector<const LHCb::MCRichHit *> MCRichHitVector;
    /// MCParticle/radiator key
    typedef std::pair<const LHCb::MCParticle *, Rich::RadiatorType> MCPRadKey;
    /// Mapping from MCParticles to MCRichHits
    typedef Rich::Map<MCPRadKey, MCRichHitVector> MCParticle2RichHits;
    /// Tally for number of hits in each radiator
    typedef Rich::Map<Rich::RadiatorType,TrackPhotonTally> RichRadCount;

  private: // methods

    /// Access the MC Truth Tool on-demand
    inline const IRichMCTruthTool * mcTruthTool() const 
    { 
      if ( !m_mcTruth ) { acquireTool( "RichMCTruthTool", m_mcTruth, 0, true ); }
      return m_mcTruth; 
    }

  private: // data

    /// Pointer to MCtruth association tool
    mutable const IRichMCTruthTool * m_mcTruth;

    /// Location of MCRichHits to analyse
    std::string m_mcRichHitsLoc;

    // selection cuts
    std::vector<double> m_minP; ///< Min momentum per radiator
    std::vector<double> m_maxP; ///< Max momentum per radiator

    std::vector<double> m_minEntryR; ///< Minimum radiator entry R ( (x*x+y*y)^0.5 )
    std::vector<double> m_minExitR;  ///< Minimum radiator exit R ( (x*x+y*y)^0.5 )
    std::vector<double> m_minEntryX; ///< Minimum radiator entry X
    std::vector<double> m_minExitX;  ///< Minimum radiator exit X
    std::vector<double> m_minEntryY; ///< Minimum radiator entry X
    std::vector<double> m_minExitY;  ///< Minimum radiator exit X

    std::vector<double> m_maxEntryR; ///< Maximum radiator entry R ( (x*x+y*y)^0.5 )
    std::vector<double> m_maxExitR;  ///< Maximum radiator exit R ( (x*x+y*y)^0.5 )
    std::vector<double> m_maxEntryX; ///< Maximum radiator entry X
    std::vector<double> m_maxExitX;  ///< Maximum radiator exit X
    std::vector<double> m_maxEntryY; ///< Maximum radiator entry X
    std::vector<double> m_maxExitY;  ///< Maximum radiator exit X

    std::vector<double> m_minPathLength; ///< Minimum path length in the radiator volume
    std::vector<double> m_maxPathLength; ///< Maximum path length in the radiator volume

    // Track and photon tally for all events
    RichRadCount m_signalRadHits;

  };

} // RICH namespace

#endif // RICHMCMONITORS_RichMCCKPhotonYieldAlg_H
