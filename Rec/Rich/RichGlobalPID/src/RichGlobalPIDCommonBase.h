
//--------------------------------------------------------------------------
/** @file RichGlobalPIDCommonBase.h
 *
 *  Header file for RICH global PID algorithm base class : Rich::Rec::GlobalPID::CommonBase
 *
 *  CVS Log :-
 *  $Id: RichGlobalPIDCommonBase.h,v 1.1 2008-03-25 16:26:44 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2002-11-30
 */
//--------------------------------------------------------------------------

#ifndef RICHGLOBALPID_RichGlobalPIDCommonBase_H
#define RICHGLOBALPID_RichGlobalPIDCommonBase_H 1

// Event
#include "Event/RichRecStatus.h"
#include "Event/RichGlobalPIDTrack.h"
#include "Event/RichGlobalPID.h"
#include "Event/ProcStatus.h"

// interfaces
#include "RichKernel/IRichParticleProperties.h"

// Definitions
#include "RichGlobalPID/RichGlobalPIDTkQuality.h"
#include "RichRecBase/RichTrackID.h"

namespace Rich
{
  namespace Rec
  {
    namespace GlobalPID
    {

      /** @class CommonBase RichGlobalPIDCommonBase.h
       *
       *  Abstract base class for GlobalPID algorithms and tools
       *
       *  @author Chris Jones
       *  @date   2008-02-29
       */
      template <class PBASE>
      class CommonBase : public PBASE
      {

      public:

        /// Standard algorithm-like constructor
        CommonBase( const std::string& name,
                    ISvcLocator* pSvcLocator );

        /// Standard tool-like constructor
        CommonBase( const std::string& type,
                    const std::string& name,
                    const IInterface* parent );

        virtual ~CommonBase( ); ///< Destructor

        // Initialization of the algorithm after creation
        virtual StatusCode initialize();

        // Finalization of the algorithm before deletion
        virtual StatusCode finalize();

      protected:

        LHCb::RichGlobalPIDTracks  * gpidTracks() const;  ///< Access RichGlobalPIDTracks
        LHCb::RichGlobalPIDs       * gpidPIDs() const;    ///< Access RichGlobalPIDPIDs
        LHCb::ProcStatus           * procStatus() const;  ///< Access the ProcStatus object

        /// The GPID sequence name
        inline const std::string& gpidName() const { return m_richGPIDName; }

        /// The PID types to consider
        inline const Rich::Particles& pidTypes() const { return m_pidTypes; }

        /// Clean up GPID event TES objects
        void deleteGPIDEvent();

      private:

        /// Initialise constructor
        void initConstructor();

      private:

        /// Pointer to parent RichGlobalPIDTrack container
        mutable LHCb::RichGlobalPIDTracks * m_GPIDtracks;

        /// Pointer to parent RichGlobalPID container
        mutable LHCb::RichGlobalPIDs * m_GPIDs;

        /// Pointer to the ProcStatus object
        mutable LHCb::ProcStatus * m_procStat;

        /// Location of working RICH Global PID tracks in TES
        std::string m_richGPIDTrackLocation;

        /// Location of Global PID results in TES
        std::string m_richGPIDLocation;

        /// Global algorithm name. Common to all sub-algorithms
        std::string m_richGPIDName;

        /// Location of processing status object in TES
        std::string m_procStatLocation;

        /// Pointer to RichParticleProperties interface
        const IParticleProperties * m_richPartProp;

        /// Particle ID types to consider
        Rich::Particles m_pidTypes;

      };

      template <class PBASE>
      inline LHCb::RichGlobalPIDTracks * CommonBase<PBASE>::gpidTracks() const
      {
        if ( !m_GPIDtracks )
        {
          m_GPIDtracks = 
            this -> template getOrCreate<LHCb::RichGlobalPIDTracks,LHCb::RichGlobalPIDTracks>( this->evtSvc(), 
                                                                                               m_richGPIDTrackLocation );
        }
        return m_GPIDtracks;
      }

      template <class PBASE>
      inline LHCb::RichGlobalPIDs * CommonBase<PBASE>::gpidPIDs() const
      {
        if ( !m_GPIDs )
        {
          m_GPIDs = 
            this -> template getOrCreate<LHCb::RichGlobalPIDs,LHCb::RichGlobalPIDs>( this->evtSvc(), 
                                                                                     m_richGPIDLocation );
        }
        return m_GPIDs;
      }

      template <class PBASE>
      inline LHCb::ProcStatus * CommonBase<PBASE>::procStatus() const
      {
        if ( !m_procStat )
        {
          m_procStat = 
            this -> template getOrCreate<LHCb::ProcStatus,LHCb::ProcStatus>( this->evtSvc(), 
                                                                             m_procStatLocation );
        }
        return m_procStat;
      }

    }
  }
}

#endif // RICHGLOBALPID_RichGlobalPIDCommonBase_H
