
//-----------------------------------------------------------------------------
/** @file RichSeedTrackSelector.h
 *
 *  Header file for RICH reconstruction tool : Rich::Rec::SeedTrackSelector
 *
 *  CVS Log :-
 *  $Id: RichSeedTrackSelector.h,v 1.3 2007-02-02 10:10:41 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/08/2006
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RichSeedTrackSelector_H
#define RICHRECTOOLS_RichSeedTrackSelector_H 1

// base class
#include "RichBaseTrackSelector.h"

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
  /** @namespace Rec
   *
   *  General namespace for RICH reconstruction software
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   08/07/2004
   */
  //-----------------------------------------------------------------------------
  namespace Rec
  {

    //-----------------------------------------------------------------------------
    /** @class SeedTrackSelector RichSeedTrackSelector.h
     *
     *  Extension to the RichBaseTrackSelector for Seed tracks
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   2006-08-12
     */
    //-----------------------------------------------------------------------------

    class SeedTrackSelector : public BaseTrackSelector
    {

    public: // Gaudi methods

      /// Standard constructor
      SeedTrackSelector( const std::string& type,
                         const std::string& name,
                         const IInterface* parent );

      /// Destructor
      virtual ~SeedTrackSelector( );

    public: // interface methods

      /// Test if the given Track is selected under the current criteria
      virtual bool trackSelected( const LHCb::Track * track ) const;

      /// Test it the given RichRecTrack is selected
      virtual bool trackSelected( const LHCb::RichRecTrack * track ) const;

    protected: // methods

      /// Print the track selection
      virtual MsgStream & printSel( MsgStream & os ) const;

    private: // data

      double m_minSeedLL; ///< minimum seed track likelihood cut
      double m_maxSeedLL; ///< maximum seed track likelihood cut

    };

  }
} // RICH namespace

#endif // RICHRECTOOLS_RichSeedTrackSelector_H
