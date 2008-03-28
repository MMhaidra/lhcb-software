//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/Gaucho/Gaucho/MonObject.h,v 1.2 2008-03-28 10:59:12 evh Exp $
#ifndef GAUCHO_MONOBJECT_H
#define GAUCHO_MONOBJECT_H 1

// Include files
#include <string>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <cmath>
#include <TStopwatch.h>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include <pthread.h>

/** @class ClassName MonObject.h OnlineKernel/MonObject.h
 *
 *  MonObject base class
 *
 *  @author Claus Buszello
 *  @author Eric van Herwijnen
 *  @author Juan Otalora
 *  @date 08/01/2008
 */

  static const std::string s_monObject("MonObject");
  static const std::string s_monH1F("MonH1F");
  static const std::string s_monH1D("MonH1D");
  static const std::string s_monH2F("MonH2F");
  static const std::string s_monH2D("MonH2D");
  static const std::string s_monProfile("MonProfile");
  static const std::string s_monInt("MonInt");
  static const std::string s_monDouble("MonDouble");
  static const std::string s_monFloat("MonFloat");
  static const std::string s_monString("MonString");
  static const std::string s_monLong("MonLong");
  static const std::string s_monBool("MonBool");
  static const std::string s_monPairDD("MonPairDD");
  static const std::string s_monPairII("MonPairII");
  static const std::string s_monPairDI("MonPairDI");
  static const std::string s_monHitMap2D("MonHitMap2D");
  static const std::string s_monStatEntity("MonStatEntity");
  static const std::string s_monVectorI("MonVectorI");
  static const std::string s_monVectorD("MonVectorD");

class MonObject{

protected:
  std::string m_typeName;
  std::string m_comments;
  std::string m_dimPrefix;
  unsigned int m_version;
  bool m_endOfRun;

public:
  MonObject(IMessageSvc* msgSvc, const std::string& source, int version=0);
  virtual ~MonObject();

  template<class Archive>
  void serialize(Archive & ar, const unsigned int version){
    if (version != m_version) m_version = version;
    ar & m_typeName; 
    ar & m_version;
  }  
  virtual void save(boost::archive::binary_oarchive & ar, const unsigned int version);
  virtual void load(boost::archive::binary_iarchive & ar, const unsigned int version);

  void lock();
  void unlock();

  std::string comments(){return m_comments;}
  void setComments(std::string comm){m_comments = comm;}

  bool endOfRun(){return m_endOfRun;}
  void setEndOfRun(bool endofrun){m_endOfRun = endofrun;}

  virtual std::string dimPrefix() const {return m_dimPrefix;}
  void setDimPrefix(std::string dimPrefix){m_dimPrefix = dimPrefix;}

  virtual std::string typeName() const {return m_typeName;} 
  virtual int version(){return m_version;}

  virtual void combine(MonObject* monObject);
  virtual void copyFrom(MonObject* monObject);
  virtual void reset();
  virtual void subtract();
  virtual void print();
  virtual void write(){};

  // bool islocked();
   TStopwatch watchOut;
   TStopwatch watch;

protected:
  IMessageSvc*  m_msgSvc;
  std::string   m_source;
  std::string   m_name;
  MsgStream createMsgStream();
  void doOutputMsgStream(MsgStream &msg);
private:
  pthread_mutex_t  m_mtx;

  friend class boost::serialization::access;
};
#endif // GAUCHO_MONOBJECT_H

