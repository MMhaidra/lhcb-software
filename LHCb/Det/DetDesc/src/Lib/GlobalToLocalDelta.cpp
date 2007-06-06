// $Id: GlobalToLocalDelta.cpp,v 1.5 2007-06-06 17:43:12 jpalac Exp $
// Include files 
#include "DetDesc/IDetectorElement.h"
#include "DetDesc/IGeometryInfo.h"
// local
#include "DetDesc/GlobalToLocalDelta.h"
#include "RotationZYX.h"
#include "3DConversions.h"
namespace DetDesc {

const Gaudi::Transform3D localDeltaMatrix(const IDetectorElement* DE,
                                          const Gaudi::Transform3D& globalDelta) {
  return localDeltaMatrix(DE->geometry(), globalDelta);
}

const Gaudi::Transform3D localDeltaMatrix(const IGeometryInfo* GI,
                                          const Gaudi::Transform3D& toMisaligned_global) {

  const Gaudi::Transform3D toLocal_total       = GI->matrix();
  const Gaudi::Transform3D toGlobal_ideal       = GI->idealMatrixInv();
  const Gaudi::Transform3D toMisalignedOld_local = GI->localDeltaMatrix().Inverse();
  return toMisalignedOld_local*toLocal_total*toGlobal_ideal*toMisaligned_global;
  
}
// @todo move these to a general Transform3D tools suite.  
const Gaudi::Transform3D localToGlobalTransformation(std::vector<double>& pivotParams,
                                                     std::vector<double>& translationParams,
                                                     std::vector<double>& rotationParams) 
{
  const Gaudi::Transform3D pivot    = DetDesc::XYZTranslation(pivotParams);
  const Gaudi::Transform3D rotation = DetDesc::ZYXRotation(rotationParams);
  const Gaudi::Transform3D translation = DetDesc::XYZTranslation(translationParams);

  return translation*pivot*rotation*(pivot.Inverse());
  
}

const Gaudi::Transform3D XYZTranslation(const std::vector<double>& params) 
{
  return (params.size()==3) ? Gaudi::Transform3D( Gaudi::TranslationXYZ(params[0], 
                                                                        params[1], 
                                                                        params[2]) )
    : Gaudi::Transform3D();
  
}

const Gaudi::Transform3D ZYXRotation(const std::vector<double>& params)
{
  if (params.size() != 3) return Gaudi::Transform3D();
  
  Gaudi::RotationZYX rotZYX(params[2], params[1], params[0]);
  Gaudi::Rotation3D tmp;
  Gaudi::Math::convert(rotZYX, tmp);

  return Gaudi::Transform3D(tmp);
}

void getZYXTransformParameters(const Gaudi::Transform3D& CDM,
                               std::vector<double>& rotationParams,
                               std::vector<double>& translationParams,
                               const std::vector<double>& pivotParams) 
{

  Gaudi::Rotation3D newRot;
  Gaudi::TranslationXYZ newTrans;
  CDM.GetDecomposition(newRot, newTrans);
  const Gaudi::Transform3D pivotTrans = DetDesc::XYZTranslation(pivotParams);
  
  // Take the pivot out of the rotation.
  const Gaudi::Transform3D newRotPart = 
    pivotTrans.Inverse() * Gaudi::Transform3D(newRot) * pivotTrans;

  Gaudi::TranslationXYZ tmp;
  newRotPart.GetDecomposition(newRot, tmp);

  getZYXRotationParameters(newRot, rotationParams);

  getTranslationParameters(newTrans, translationParams);
  
}

void getZYXRotationParameters(const Gaudi::Rotation3D& rot,
                              std::vector<double>& rotParams)
{

  const Gaudi::RotationZYX rotation(rot);

  rotation.GetComponents(rotParams[2], rotParams[1], rotParams[0]);

  return;
}


void getTranslationParameters(const Gaudi::TranslationXYZ& trans,
                              std::vector<double>& transParams)
{

  transParams[0]=trans.X();
  transParams[1]=trans.Y();
  transParams[2]=trans.Z();
  
  return;
}
  
} // namespace DetDesc
//=============================================================================
