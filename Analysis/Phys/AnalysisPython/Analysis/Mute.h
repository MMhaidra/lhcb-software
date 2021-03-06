// $Id$ 
// ============================================================================
#ifndef ANALYSIS_MUTE_H 
#define ANALYSIS_MUTE_H 1
// ============================================================================
// Include files
// ============================================================================
// STD& STL 
// ============================================================================
#include <fstream>
#include <streambuf>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
// ============================================================================
namespace Gaudi
{
  // ==========================================================================
  namespace Utils 
  {
    // ========================================================================
    /** @class Mute Mute.h Analysis/Mute.h
     *
     *  Helper utility to mute output (or redirect it to the file)
     *
     *  @see Gaudi::Utils::Tee
     *  @author Vanya Belyaev
     *  @date   2013-02-19
     */
    class GAUDI_API Mute 
    {
    public:
      // ======================================================================
      /// constructor with file name 
      Mute  ( const std::string& fname        , 
              const bool         out   = true ) ; 
      /// default consructor ( redirect output to "dev/null") 
      Mute  ( const bool         out   = true ) ; // default consructor
      ///  destructor 
      ~Mute ( ) ;            //  destructor 
      // ======================================================================
    private: 
      // ======================================================================
      /// copy     constructor is disabled 
      Mute ( const Mute& ) ;                 // copy    constructor is disabled 
      /// assignement operator is disabled 
      Mute & operator=( const Mute& ) ;     // assignement operator is disabled 
      // ======================================================================
    public: // helper functions for implementation of ContextManager in python
      // ======================================================================
      /** helper (empty) function to implement __enter__   
       *  the action is performed in constructor
       */
      void enter () ;
      /// helper         function to implement __exit__   
      void exit  () ;
      // ======================================================================
    private: 
      // ======================================================================
      bool            m_cout   ;
      std::ofstream   m_output ;
      std::streambuf* m_buffer ;
      // ======================================================================
    };
    // ========================================================================
  } //                                            end of namespace Gaudi::Utils 
  // ==========================================================================
} //                                                     end of namespace Gaudi
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // ANALYSIS_MUTE_H
// ============================================================================
