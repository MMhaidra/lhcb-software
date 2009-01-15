// $Id: SiAmplifierResponseBase.cpp,v 1.1 2009-01-15 10:02:19 mneedham Exp $

// Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

// local
#include "SiAmplifierResponseBase.h"

SiAmplifierResponseBase::SiAmplifierResponseBase( const std::string& type, 
                                          const std::string& name, 
                                          const IInterface* parent ): 
  GaudiTool( type, name, parent ),
  m_responseSpline(0)
{
  // constructer
  declareProperty("splineType",  m_splineType  = "Cspline"                   );
  declareProperty("type",        m_type        = "signal"                    );
  declareProperty("vfs",         m_vfs         = 400                         );
  declareProperty("capacitance", m_capacitance = 18 * Gaudi::Units::picofarad);
  declareProperty("dt", m_printDt = 2.0 * Gaudi::Units::ns) ;
  declareProperty("printToScreen", m_printToScreen = false);
  declareProperty("printForRoot", m_printForRoot = false); 
  // need a line here to get the interface
  declareInterface<ISiAmplifierResponse>(this); 
}

SiAmplifierResponseBase::~SiAmplifierResponseBase()
{
  // destructer
  if (0 != m_responseSpline) delete m_responseSpline;
}

StatusCode SiAmplifierResponseBase::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);

  // Fill the ISiAmplifierResponse::Info object. Used to find out for which 
  // parameters the spline curve is valid.
  m_info.capacitance = m_capacitance;
  m_info.vfs = m_vfs;
  m_info.type = m_type;
  return sc;
}

/// calculate amplifier response
double SiAmplifierResponseBase::response(const double time) const
{
  return ((time>m_tMin)&&(time<m_tMax) ? m_responseSpline->eval(time) : 0.0);
}

/// information on the validity
ISiAmplifierResponse::Info SiAmplifierResponseBase::validity() const
{
  return m_info;
}

GaudiMath::Interpolation::Type SiAmplifierResponseBase::typeFromString() const
{  
  GaudiMath::Interpolation::Type aType;
  using namespace GaudiMath::Interpolation;
  if      (m_splineType == "Cspline")          aType = Cspline;
  else if (m_splineType == "Linear")           aType = Linear;
  else if (m_splineType == "Polynomial")       aType = Polynomial;
  else if (m_splineType == "Akima")            aType = Akima;
  else if (m_splineType == "Akima_Periodic")   aType = Akima_Periodic;
  else if (m_splineType == "Cspline_Periodic") aType = Cspline_Periodic;
  else aType = Cspline; // default to cubic

  return aType;
}

void SiAmplifierResponseBase::sample(std::vector<double>& times, std::vector<double>& values) const{

  double t = m_tMin;
  while(t < m_tMax) {
    times.push_back(t);
    values.push_back(response(t));
    t += m_printDt; 
  } // loop times
}

void SiAmplifierResponseBase::printToScreen() const {

  std::vector<double> times; std::vector<double> values;
  sample(times,values); 
  info() << "Printing response function" << endmsg;
  info() << "Type: " << m_type << endmsg; 
  info() << "Vfs: " << m_vfs << endmsg;
  info() << "Assumed Capacitance: " 
         << m_capacitance/Gaudi::Units::picofarad << " pF "<< endmsg;
  for (unsigned int i = 0 ; i < times.size() ; ++i){
    info() << "Time " << times[i] << " value " << values[i] << endmsg;
  } // loop i

} 

void SiAmplifierResponseBase::printForRoot() const {

  std::vector<double> times; std::vector<double> values;
  sample(times,values);
  std::cout << "Printing TGraph for " << name() << std::endl;
  std::cout << "void plotGraph(){" << std::endl;
  printArray(times, "times");
  printArray(values, "values");
  std::cout << "TGraph* aGraph = new TGraph(" << values.size() 
            << ",times,values);" << std::endl;
  std::cout << "aGraph->Draw(\"APL\");" << std::endl;;
  std::cout <<"}" << std::endl;; 
}

void SiAmplifierResponseBase::printArray(const std::vector<double>& values, 
                                  const std::string& name) const{
  
  std::cout << "float " << name << "[" << values.size() << "] = {" ; 
  for (unsigned int i = 0; i < values.size(); ++i ){
    std::cout << values[i];
    if (i != values.size()-1 ) std::cout << "," ; 
  } // i          
  std::cout << "};" << std::endl;    
}



