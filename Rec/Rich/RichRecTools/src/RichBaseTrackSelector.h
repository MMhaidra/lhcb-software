
//-----------------------------------------------------------------------------
/** @file RichBaseTrackSelector.h
 *
 *  Header file for RICH reconstruction tool : RichBaseTrackSelector
 *
 *  CVS Log :-
 *  $Id: RichBaseTrackSelector.h,v 1.2 2006-08-13 19:07:30 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/08/2006
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RichBaseTrackSelector_H
#define RICHRECTOOLS_RichBaseTrackSelector_H 1

// base class
#include "RichRecBase/RichRecToolBase.h"

// interfaces
#include "IRichBaseTrackSelector.h"

// Event model
#include "Event/Track.h"
#include "Event/RichRecTrack.h"

// boost
#include "boost/numeric/conversion/bounds.hpp"
#include "boost/limits.hpp"
#include "boost/format.hpp"

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
  /** @class RichBaseTrackSelector RichBaseTrackSelector.h
   *
   *  Utility tool used by RichBaseTrackSelector which implements core Track
   *  selection criteria.
   * 
   *  Users should not use directly but instead use a RichTrackSelector
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   2006-08-12
   */
  //-----------------------------------------------------------------------------

  class RichBaseTrackSelector : public RichRecToolBase,
                                virtual public IRichBaseTrackSelector
  {

  public: // Gaudi methods

    /// Standard constructor
    RichBaseTrackSelector( const std::string& type,
                           const std::string& name,
                           const IInterface* parent );

    /// Destructor
    virtual ~RichBaseTrackSelector( );

    /// Initialize method
    StatusCode initialize();

    /// Finalize method
    StatusCode finalize();

  public: // interface methods

    /// Test if the given Track is selected under the current criteria
    virtual bool trackSelected( const LHCb::Track * track ) const;

    /// Test it the given RichRecTrack is selected
    virtual bool trackSelected( const LHCb::RichRecTrack * track ) const;

    /// Returns the overall minimum momentum cut value
    virtual double minPCut() const;

    /// Returns the overall maximum momentum cut value
    virtual double maxPCut() const;

    /// Returns the overall minimum pt cut value
    virtual double minPtCut() const;

    /// Returns the overall maximum pt cut value
    virtual double maxPtCut() const;

    /// Returns the overall minimum chi^2 cut
    virtual double minChi2Cut() const;

    /// Returns the overall maximum chi^2 cut
    virtual double maxChi2Cut() const;

  protected: // methods

    /// Print the track selection
    virtual MsgStream & printSel( MsgStream & os ) const;

  protected: // data

    double m_minChi2Cut; ///< Min chi^2 cut
    double m_maxChi2Cut; ///< Max chi^2 cut

    double m_minPCut;    ///< Min p cut
    double m_maxPCut;    ///< Max p cut

    double m_minPtCut;   ///< Min pt cut
    double m_maxPtCut;   ///< Max pt cut

    int m_chargeSel;     ///< Charge selection (-1=negative,+1=positive,0=all)

    bool m_acceptClones; ///< Accept or reject clone tracks

  };

} // RICH namespace

#endif // RICHRECTOOLS_RichBaseTrackSelector_H
