#ifndef MUONDET_MUONFRONTENDIDPACK_H
#define MUONDET_MUONFRONTENDIDPACK_H 1


namespace MuonFrontEndIDPack{

static const unsigned int bitChamber=11;
static const unsigned int bitReadout=1;
static const unsigned int bitLayer=1;
static const unsigned int bitFEGridX=6;
static const unsigned int bitFEGridY=3;
static const unsigned int bitFEIDX=6;
static const unsigned int bitFEIDY=3;



static const unsigned int shiftChamber  = 0; 
static const unsigned int shiftReadout  = shiftChamber + bitReadout ;
static const unsigned int shiftLayer    = shiftReadout  + bitLayer ; 
static const unsigned int shiftFEGridX  = shiftLayer + bitFEGridX ;
static const unsigned int shiftFEGridY  = shiftFEGridX  + bitFEGridY ;
static const unsigned int shiftFEIDX    = shiftFEGridX  + bitFEIDX ;
static const unsigned int shiftFEIDY    = shiftFEIDX  + bitFEIDY ;


 
static const unsigned int maskChamber  =  ( ( ( (unsigned int) 1 ) << bitChamber  ) - 1  ) << shiftChamber ;     
static const unsigned int maskReadout  =  ( ( ( (unsigned int) 1 ) << bitReadout ) - 1  ) << shiftReadout ;     
static const unsigned int maskLayer    =  ( ( ( (unsigned int) 1 ) << bitLayer  ) - 1  ) << shiftLayer ;     
static const unsigned int maskFEGridX  =  ( ( ( (unsigned int) 1 ) << bitFEGridX  ) - 1  ) << shiftFEGridX;     
static const unsigned int maskFEGridY  =  ( ( ( (unsigned int) 1 ) << bitFEGridY  ) - 1  ) << shiftFEGridY ;     
static const unsigned int maskFEIDX    =  ( ( ( (unsigned int) 1 ) << bitFEIDX  ) - 1  ) << shiftFEIDX ;     
static const unsigned int maskFEIDY    =  ( ( ( (unsigned int) 1 ) << bitFEIDY  ) - 1  ) << shiftFEIDY ;     


inline unsigned int getBit(unsigned int  packedValue, unsigned int shift,
unsigned int mask){
 return ((packedValue&mask)>>shift);
}

inline void  setBit(unsigned int &origValue,
                     unsigned int shift, unsigned int mask,
                     unsigned int bitValue){
 unsigned int                           store   = origValue;
 unsigned int       result=(((bitValue<<shift)&mask)| (store&~mask));
  origValue=result;
} 

};
#endif 
