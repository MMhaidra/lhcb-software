// $Id: NNetTool.h,v 1.6 2006-05-16 10:20:04 musy Exp $
#ifndef NNETTOOL_NNETTOOL_H 
#define NNETTOOL_NNETTOOL_H 1
// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/AlgTool.h"
#include "INNetTool.h"            // Interface

/** @class NNetTool NNetTool.h NNetTool.h
 *  
 * Neural Net returns wrong tag fraction for a given tagger.
 * Inputs are typically: event-multiplicity,
 * tampering flag, tagger p/GeV, pt/GeV, IP-significance,
 * IP significance wrt secondary Vtx, Delta-eta between B and tagger,
 * Delta-phi, Delta-Q (= mass of B+tagger system - mass of B).
 *  v1.3
 *  @author Marco Musy
 *  @date   2004-12-14
 */
class NNetTool : public GaudiTool, 
	virtual public INNetTool {
public:
  /// Standard constructor
  NNetTool( const std::string& type, 
	    const std::string& name,
	    const IInterface* parent );

  virtual ~NNetTool( ); ///< Destructor

  /// Initialize
  StatusCode initialize();

  /// Finalize
  StatusCode finalize();
  
  virtual double MLPm(std::vector<double>&);
  virtual double MLPe(std::vector<double>&);
  virtual double MLPk(std::vector<double>&);
  virtual double MLPkS(std::vector<double>&);
  virtual double MLPpS(std::vector<double>&);

private:
  double SIGMOID(double);
  void normaliseOS(std::vector<double>& );
  void normaliseSS(std::vector<double>& );

  double pol2(double x, double a0, double a1);
  double pol3(double x, double a0, double a1, double a2);
  double pol4(double x, double a0, double a1, double a2, double a3);

};
#endif // NNETTOOL_NNETTOOL_H
