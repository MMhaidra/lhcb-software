// $Id: ISecondaryVertexTool.h,v 1.3 2005-07-04 08:20:05 pkoppenb Exp $
#ifndef SECONDARYVERTEXTOOL_ISECONDARYVERTEXTOOL_H 
#define SECONDARYVERTEXTOOL_ISECONDARYVERTEXTOOL_H 1

// Include files
// from STL
#include <string>
// from Gaudi
#include "Kernel/DVAlgorithm.h"
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_ISecondaryVertexTool("ISecondaryVertexTool", 1 , 0); 

/** @class ISecondaryVertexTool ISecondaryVertexTool.h 
 *  
 *  v1.2
 *  @author Marco Musy (Milano)
 *  @date   2004-12-14
 */
class ISecondaryVertexTool : virtual public IAlgTool {
public:
  
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_ISecondaryVertexTool; };
 
  virtual std::vector<Vertex> 
    buildVertex( const Vertex, const ParticleVector ) = 0; 
};
#endif // SECONDARYVERTEXTOOL_ISECONDARYVERTEXTOOL_H
