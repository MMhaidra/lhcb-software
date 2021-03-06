#include "ANNSvc.h"
#include <string>
#include <boost/functional/hash.hpp>


class HltANNSvc : public ANNSvc {
public:
    HltANNSvc( const std::string& name, ISvcLocator *svcLocator )
    :ANNSvc(name, svcLocator, {{"Hlt1SelectionID"},
                               {"Hlt2SelectionID"},
                               {"InfoID"},
                               {"RoutingBits"},
                               {"RelInfoLocations"},
                               {"PackedObjectLocations"}})
    {
        declareProperty("allowUndefined",m_allowUndefined=true,"do we allow undefined, on-demand generated, key/value pairs?");
    }
    using ANNSvc::handleUndefined;
    boost::optional<minor_value_type> handleUndefined(const major_key_type& major, const std::string& minor) const override;
private:
    bool              m_allowUndefined;
};

DECLARE_COMPONENT( HltANNSvc )

boost::optional<IANNSvc::minor_value_type> 
HltANNSvc::handleUndefined(const major_key_type& major, const std::string& minor) const
{
   if (!m_allowUndefined)  return boost::none;

   static boost::hash<std::string> hasher; 
   log() << MSG::DEBUG << "handleUndefined called for " << major << " : " << minor
                       << " -->  " << hasher(minor) << endmsg;
   return minor_value_type(minor,hasher(minor));
}
