#include "ProcessorKernel/TileRegister.h"
#include <cmath> // For definition of pow on windows

L0Muon::TileRegister::TileRegister() {

}

L0Muon::TileRegister::TileRegister(int bits) 
  : L0Muon::Register(bits){
  m_duplicatedTiles = false; 
}

L0Muon::TileRegister::TileRegister(int bits, unsigned long pattern) 
  : L0Muon::Register(bits,pattern) {
  m_duplicatedTiles = false; 
} 
    
L0Muon::TileRegister::TileRegister(int bits, 
                                   unsigned long pattern,
                                   std::vector<LHCb::MuonTileID> ids) 
  : L0Muon::Register(bits,pattern), m_ids(ids){
  m_duplicatedTiles = false; 
}    
    
L0Muon::TileRegister::~TileRegister() {}    

bool L0Muon::TileRegister::checkDuplicatedTiles() {
  for (unsigned int ind1 =0; ind1 < m_ids.size(); ++ind1){
    if (!tile(ind1).isDefined()) continue;
    for (unsigned int ind2 =ind1+1; ind2 < m_ids.size(); ++ind2){
      //       if ( m_ids[ind1] == m_ids[ind2]) return true;
      if ( tile(ind1) == tile(ind2)) {
        std::cout << tile(ind1).toString()<<" "<<tile(ind2).toString()<<std::endl;
        return true;
      }
    }    
  }  
  return false ;
}


std::vector<LHCb::MuonTileID> L0Muon::TileRegister::firedTiles() {
 
 
  std::vector<LHCb::MuonTileID> result;
  unsigned int ind;
  for ( ind = 0; ind<m_bitset.size(); ind++) {
    if (m_bitset.test(ind)) {
      result.push_back(tile(ind));    
      LHCb::MuonTileID mid = tile(ind);
    }  
  }
  return result;
}

void L0Muon::TileRegister::setTileVector(const std::vector<LHCb::MuonTileID>& ids) {
  m_ids = ids;
}


void L0Muon::TileRegister::setTile(LHCb::MuonTileID & id){
  for (unsigned int ind =0; ind < m_ids.size(); ind++){
    //LHCb::MuonTileID mid = m_ids[ind];
    if ( m_ids[ind]== id){
      m_bitset.set(ind);
      m_set = true; 
      if (!m_duplicatedTiles)
        break;
    }    
  }  
}


void L0Muon::TileRegister::setTilesTagVector(boost::dynamic_bitset<> & tilestag){
  m_tilestag = tilestag;
}

void L0Muon::TileRegister::setStripsTagVector(boost::dynamic_bitset<> & stripstag){
  m_stripstag = stripstag; 
}


/*
  Fill the vector m_pads
*/
void L0Muon::TileRegister::makePads(){
 
  if (m_pads.size() != 0){
    m_pads.clear();
  }  
  std::vector<LHCb::MuonTileID> tmp = firedTiles();
  
  unsigned int i,j;
  if ( ! tmp.empty() ) {
    for ( i=0; i< m_bitset.size(); i++){
      LHCb::MuonTileID mid = tile(i);
      if (mid.isValid()==0) continue;

      if (m_bitset.test(i)) {
        if ( m_tilestag[i] == 1){
          m_pads.push_back(mid);
        }
        if ( m_tilestag[i] ==0){
          if ( i< m_bitset.size()-1) {
            if (m_bitset.test(i)) {
              for ( j = i+1; j < m_bitset.size(); j++){
		
                if (m_bitset.test(j)) {

                  if ( m_stripstag[i] != m_stripstag[j]) {
                    LHCb::MuonTileID ip = tile(i);
                    LHCb::MuonTileID jp = tile(j);
                    LHCb::MuonTileID ijp = ip.intercept(jp);
                    if (ijp.isValid()){
		      m_pads.push_back(ijp);
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
}

void L0Muon::TileRegister::print_bits(unsigned int event, FILE *file){

  if (file==0) file= stdout;
  // Loop over the bits of the register 
  for (boost::dynamic_bitset<>::size_type i =0; i < m_bitset.size();i++){
    int val=m_bitset[i] ;
    LHCb::MuonTileID  mid= tile(i);
    if (val) {
      
      if (mid.isValid()){
        fprintf(file,"%5u %3u ( %2d,%2d ) Q%1d M%1d R%1d %2d %2d  %d\n"
                ,event ,(unsigned int)i 
                ,mid.layout().xGrid()
                ,mid.layout().yGrid()
                ,mid.quarter()+1 
                ,mid.station()+1 
                ,mid.region()+1 
                ,mid.nX()
                ,mid.nY()
                ,val
                ); 
      } else {
        fprintf(file,"%5u %3u                           %d\n",event,(unsigned int)i,val);
      }
    }
    

  }
}



void L0Muon::TileRegister::print_tiles(FILE *file,int ntiles_per_line){
  if (file==0) file= stdout;  
  int ic=0;
  for (std::vector<LHCb::MuonTileID> ::iterator im = m_ids.begin();im!=m_ids.end();im++) {
    if ((ic % ntiles_per_line)==0)  {
      if (ic>0) fprintf(file,"\n");
      fprintf(file,"\t%3d: ",ic);
    }
    if (im->isValid())
      fprintf(file," M%1d(%2d,%2d)Q%1dR%1d %2d-%2d ;",
              im->station()+1,im->layout().xGrid(),im->layout().yGrid(),
              im->quarter()+1,im->region()+1,im->nX(),im->nY());
    else 
      fprintf(file," M.(..,..)Q.R. ..-.. ;");
      
    ic++;
  }
  fprintf(file,"\n");
}

std::string L0Muon::TileRegister::toXML(std::string tab){

  std::string xmlString=tab;
  
  char buf[8];
  std::string str;

  xmlString +="<"+XMLTileRegister+" ";
  xmlString +=" name = \""+name()+"\" ";
  xmlString +=" type = \""+type()+"\" ";
  sprintf(buf,"%d",size());
  str = buf;
  xmlString +=" size = \""+str+"\" ";
  std::string  duplicatedTiles = "0";
  if (checkDuplicatedTiles()) duplicatedTiles = "1";
  xmlString +=" duplicatedTiles = \""+duplicatedTiles+"\" ";
  xmlString +=" >\n";
  std::vector<LHCb::MuonTileID>::iterator  i_ids = m_ids.begin();
  for (int i = 0; i<size();i++) {
    xmlString +=tab+"    ";
    xmlString +="<"+XMLTile+" ";
    xmlString +=" id = \""+(*i_ids).toString()+"\" ";
    int tilestag = m_tilestag[i];
    sprintf(buf,"%d",tilestag);
    str = buf;
    xmlString +=" tilestag  = \""+str+"\" ";
    int stripstag = m_stripstag[i];
    sprintf(buf,"%d",stripstag);
    str = buf;
    xmlString +=" stripstag = \""+str+"\" ";
    xmlString +=" />\n";
    i_ids++;
  }

  xmlString += tab;
  xmlString +="</"+XMLTileRegister+">\n";  

  return xmlString;  
}
