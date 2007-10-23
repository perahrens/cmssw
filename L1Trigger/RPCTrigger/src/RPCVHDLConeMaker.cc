// -*- C++ -*-
//
// Package:    RPCVHDLConeMaker
// Class:      RPCVHDLConeMaker
// 
/**\class RPCVHDLConeMaker RPCVHDLConeMaker.cc src/RPCVHDLConeMaker/src/RPCVHDLConeMaker.cc

 Description: <one line class summary>

 Implementation:
     <Notes on implementation>
*/
//
// Original Author:  Artur Kalinowski
//         Created:  Tue Aug  1 13:54:56 CEST 2006
// $Id: RPCVHDLConeMaker.cc,v 1.1 2007/04/05 06:46:32 fruboes Exp $
//
//


// system include files
#include <memory>
#include <fstream>
#include <iomanip>
#include <ctime>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
//
// class decleration
//
#include "L1Trigger/RPCTrigger/interface/RPCVHDLConeMaker.h"
#include "L1Trigger/RPCTrigger/interface/RPCRingFromRolls.h"

#include "CondFormats/RPCObjects/interface/RPCReadOutMapping.h"
#include "CondFormats/DataRecord/interface/RPCReadOutMappingRcd.h"

#include "DataFormats/MuonDetId/interface/RPCDetId.h"

//
// constructors and destructor
//
RPCVHDLConeMaker::RPCVHDLConeMaker(const edm::ParameterSet& iConfig){

  ::putenv("CORAL_AUTH_USER konec");
  ::putenv("CORAL_AUTH_PASSWORD konecPass");

  RPCLinksDone  = false;

}


RPCVHDLConeMaker::~RPCVHDLConeMaker(){}


//
// member functions
//


void RPCVHDLConeMaker::initRPCLinks(const edm::EventSetup& iSetup){

  using namespace edm;
  using namespace std;

  if(!RPCLinksDone) RPCLinksDone  = true;
  else return;
 //Open the cabling database
  edm::ESHandle<RPCReadOutMapping> map;
  iSetup.get<RPCReadOutMappingRcd>().get(map);
  LogInfo("") << "version: " << map->version() << endl;

 // Build the trigger linksystem geometry;
  if (!theLinksystem.isGeometryBuilt()){
    edm::LogInfo("RPC") << "Building RPC links map for a RPCTrigger";
    edm::ESHandle<RPCGeometry> rpcGeom;
    iSetup.get<MuonGeometryRecord>().get( rpcGeom );     
    theLinksystem.buildGeometry(rpcGeom);
    edm::LogInfo("RPC") << "RPC links map for a RPCTrigger built";
  } 
 aLinks=theLinksystem.getLinks();
}

// ------------ method called to produce the data  ------------
void
RPCVHDLConeMaker::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup){

   using namespace edm;
   using namespace std;

   //initRPCLinks(iSetup);

   //writeLogCones(0,12,0,0,iSetup);
     writeLogCones(0,0,0,0,iSetup);

   //writeLogConesForTB(12,iSetup);
   return;
}

void RPCVHDLConeMaker::writeLogCones(int towMin, int towMax, int secMin, int secMax,
				     const edm::EventSetup& iSetup){

  using namespace edm;
  using namespace std;


  for(int iTow=towMin;iTow<=towMax;iTow++){
    for(int iSec=secMin;iSec<=secMax;iSec++){

      string fileName="cones/pac_t";
      char text[100];
      sprintf(text,"%d",iTow);
      fileName.append(text);
      sprintf(text,"_sec%d",iSec);
      fileName.append(text);
      fileName+=".vhd"; 
      string fileName1="patterns/";
      //string fileName1="patterns/testPatterns/pacPat_";
      //sprintf(text,"t%d.vhd",iTow);
      sprintf(text,"t%dsc0.vhdl",abs(iTow));
      fileName1.append(text);
      std::ofstream out(fileName.c_str());
      writeHeader(iTow,iSec,out);
      writeConesDef(iTow,iSec,out,iSetup);
      //writeQualityDef(out);  
      //writePatternsDef(out);
      out.close();
      //      
      string command = "cat "+ fileName +  " " + fileName1;
      command+=" > tmp.out";
      system(command.c_str());
      command = "mv tmp.out ";
      command+=fileName;
      system(command.c_str());
      //
            
      std::ofstream out1(fileName.c_str(),ios::app);
      out1<<std::endl<<"end RPC_PAC_patt;"<<std::endl;
      //writeSorterDef(out1);
      out1.close();
      
    }
  }
}

void RPCVHDLConeMaker::writeHeader(int aTower, int aSector, std::ofstream & out){

  using namespace edm;
  using namespace std;

  
  int maxPAC = 12;
  //int maxPAC = 2;

  bool begin = true;

  //Get current time
  time_t rawtime;
  struct tm * timeinfo;
  time ( &rawtime );
  timeinfo = localtime ( &rawtime );
  asctime (timeinfo);
  //
  out<<"-- version "<<asctime(timeinfo);
  out<<"-- AK file generated by RPCVHDLConeMaker"<<endl;
  out<<"library ieee;"<<endl;
  out<<"use ieee.std_logic_1164.all;"<<endl;
  out<<"use work.std_logic_1164_ktp.all;"<<endl;
  out<<"use work.RPC_PAC_def.all;"<<endl;
  out<<""<<endl;
  out<<"package RPC_PAC_patt is"<<endl;
  out<<""<<endl;
  out<<"constant TT_EPACS_COUNT         :natural := ";
  //out<<0;
  out<<maxPAC;
  out<<";"<<endl;
  out<<"constant TT_TPACS_COUNT         :natural := ";
  out<<0;
  //out<<maxPAC;
  out<<";"<<endl;
  out<<"constant TT_REF_GROUP_NUMBERS   :natural := 1;"<<endl;
  out<<"constant TT_GBSORT_INPUTS       :natural := "
     <<maxPAC<<";"<<endl;
  out<<""<<endl;
  out<<"constant PACLogPlainsDecl       :TPACLogPlainsDecl := ("<<endl;
  out<<"  --PAC_INDEX"<<endl;
  out<<"  --|   PAC_MODEL"<<endl;
  out<<"  --|   |      logplane 1 size .........logplane 6 size"<<endl;
  //
    for(int iPAC=0;iPAC<maxPAC;iPAC++){
	if(!begin) out<<",";
	else begin = false;
	out<<"   ("
	   <<iPAC
	   <<",  E, (  "
	   <<RPCRingFromRolls::m_LOGPLANE_SIZE[abs(aTower)][0]<<", "
	   <<RPCRingFromRolls::m_LOGPLANE_SIZE[abs(aTower)][1]<<", "
	   <<RPCRingFromRolls::m_LOGPLANE_SIZE[abs(aTower)][2]<<", "
	   <<RPCRingFromRolls::m_LOGPLANE_SIZE[abs(aTower)][3]<<", "
	   <<RPCRingFromRolls::m_LOGPLANE_SIZE[abs(aTower)][4]<<", "
	   <<RPCRingFromRolls::m_LOGPLANE_SIZE[abs(aTower)][5]
	   <<"))";		     
	out<<"-- "<<endl;
    }
//
    out<<");"<<endl<<endl;
  out<<"--PAC_INDEX PAC_MODEL Tower LogSector LogSegment"<<endl;
    for(int iPAC=0;iPAC<maxPAC;iPAC++){
       out<<"--    "
	 <<iPAC
	 <<"        E"<<"\t"
	 <<aTower<<"\t"
	 <<aSector<<"\t"
	 <<iPAC<<"\t"<<endl;
    }

  out<<""<<endl;
  out<<""<<endl;
  out<<"constant LogPlainConn           :TLogPlainConn := ("<<endl;
  out<<"  --PAC_INDEX   Logplane        LinkChannel     LinkLeftBit"<<endl;
  out<<"  --| PAC_MODEL |       Link    |       LogPlaneLeftBit"<<endl;
  out<<"  --|      |    |       |       |       |       |       LinkBitsCount"<<endl;
  out<<"  --------------------------------------------------------------"<<endl;


}


void RPCVHDLConeMaker::writeConesDef(int iTower, int iSec, std::ofstream & out, const edm::EventSetup& iSetup){


  //int dccInputChannel = 12;
  int dccInputChannel = 4;

  if(iTower<2) dccInputChannel = 4;
  if(iTower>1 && iTower<5) dccInputChannel = 5;
 if(iTower>4 && iTower<9) dccInputChannel = 6;
 if(iTower>8 && iTower<13) dccInputChannel = 7;
 if(iTower>12) dccInputChannel = 8;


  using namespace edm;
  using namespace std;

 
  int minPAC = iSec*12;
  int maxPAC = minPAC+11;
  //int maxPAC = minPAC+1;

  //Open the cabling database
  edm::ESHandle<RPCReadOutMapping> map;
  iSetup.get<RPCReadOutMappingRcd>().get(map);
  LogInfo("") << "version: " << map->version() << endl;

  // Build the trigger linksystem geometry;
  if (!theLinksystem.isGeometryBuilt()){
    edm::LogInfo("RPC") << "Building RPC links map for a RPCTrigger";
    edm::ESHandle<RPCGeometry> rpcGeom;
    iSetup.get<MuonGeometryRecord>().get( rpcGeom );     
    theLinksystem.buildGeometry(rpcGeom);
    edm::LogInfo("RPC") << "RPC links map for a RPCTrigger built";
  } 
  RPCRingFromRolls::RPCLinks aLinks=theLinksystem.getLinks();
  
  bool beg = true;

  LogInfo("")<<aLinks.size()<<endl;
  for(int iCone=minPAC;iCone<=maxPAC;iCone++){
    for(int iPlane=1;iPlane<7;iPlane++){
      RPCRingFromRolls::RPCLinks::const_iterator CI= aLinks.begin();
      for(;CI!=aLinks.end();CI++){
	RPCRingFromRolls::stripCords aCoords = CI->first;
	RPCRingFromRolls::RPCConnectionsVec aConnVec = CI->second;
	RPCRingFromRolls::RPCConnectionsVec::const_iterator aConnCI = aConnVec.begin();
	RPCDetId aId(aCoords.m_detRawId);
	for(;aConnCI!=aConnVec.end();aConnCI++){
	  if(aConnCI->m_tower==iTower && 
	     aConnCI->m_PAC==iCone &&
	     aConnCI->m_logplane==iPlane){
	    ////////////////////
	    //if(aCoords.isVirtual) continue;
	    //std::cout<<"raw id: "<<aCoords.m_detRawId<<" strip: "<<aCoords.m_stripNo<<std::endl;	
	    std::pair<LinkBoardElectronicIndex, int>  
	      beggining = map->getRAWSpecForCMSChamberSrip(aCoords.m_detRawId,aCoords.m_stripNo,dccInputChannel);
	    if(beggining.second==-99) continue;	  
	    if(beggining.second==-99){
	      if(!beg)out<<",";
	      else beg = false;
	      cout<<"("<<iCone<<",\t E, \t"<<iPlane-1<<",\t"
			 <<1<<",\t"
			 <<0<<",\t"
			 <<aConnCI->m_posInCone-1<<",\t "
			 <<95<<", \t"
			 <<1<<") --"<<aId<<endl;	      	      	      
	    }
	    else{
	      if(iPlane==3)
		LogInfo("")<<"("<<iCone<<",\t E, \t"<<iPlane<<"\t"
		    <<beggining.first.tbLinkInputNum<<"\t"
		    <<beggining.first.lbNumInLink<<"\t"
		    <<aConnCI->m_posInCone<<"\t "
		    <<beggining.second<<" \t"
		    <<1<<") --"<<aId<<endl;	      	      	      
	      if(!beg)out<<",";
	      else beg = false;
	      out<<"("<<iCone-minPAC<<",\t E, \t"<<iPlane-1<<",\t"
		 <<beggining.first.tbLinkInputNum<<",\t"
		 <<beggining.first.lbNumInLink<<",\t"
		 <<aConnCI->m_posInCone<<",\t "
		 <<beggining.second<<", \t";
	      out<<1<<") --"<<aId<<endl;	      	      	      
	    }
	  }
	}
      }
    }
  }
  out<<");"<<endl;
}

void RPCVHDLConeMaker::writeQualityDef(std::ofstream & out){

  using namespace std;

  out<<endl;
  
  out<<"constant PACCellQuality :TPACCellQuality := ("<<endl;
  //out<<"(0,\"010000\",7),"<<endl;
  //out<<"(0,\"001111\",0)"<<endl;
  out<<"(0,\"111111\",3),"<<endl;
  out<<"(0,\"111110\",2),"<<endl;
  out<<"(0,\"111101\",2),"<<endl;
  out<<"(0,\"111011\",2),"<<endl;
  out<<"(0,\"110111\",2),"<<endl;
  out<<"(0,\"101111\",2),"<<endl;
  out<<"(0,\"011111\",2),"<<endl;
  out<<"(0,\"111100\",1),"<<endl;
  out<<"(0,\"111010\",1),"<<endl;
  out<<"(0,\"110110\",1),"<<endl;
  out<<"(0,\"101110\",1),"<<endl;
  out<<"(0,\"011110\",1),"<<endl;
  out<<"(0,\"111001\",1),"<<endl;
  out<<"(0,\"110101\",1),"<<endl;
  out<<"(0,\"101101\",1),"<<endl;
  out<<"(0,\"011101\",1),"<<endl;
  out<<"(0,\"110011\",1),"<<endl;
  out<<"(0,\"101011\",1),"<<endl;
  out<<"(0,\"011011\",1),"<<endl;
  out<<"(0,\"100111\",1),"<<endl;
  out<<"(0,\"010111\",1),"<<endl;
  out<<"(0,\"001111\",1)"<<endl;
  out<<");"<<endl<<endl;
  
}


void RPCVHDLConeMaker::writePatternsDef(std::ofstream & out){

  using namespace edm;
  using namespace std;


  ///
  int aTower = 4;
  int minPAC = 0;
  int maxPAC = 1;
  ///

  int iTower = aTower;

  out<<"constant PACPattTable		:TPACPattTable := ("<<endl;
  out<<"-- PAC_INDEX"<<endl;
  out<<"-- | PAC_MODEL"<<endl;
  out<<"-- | |   Ref Group Index"<<endl;
  out<<"-- | |   |  Qualit Tab index"<<endl;
  out<<"-- | |   |  |  Plane1    Plane2   Plane3    Plane4     Plane5     Plane6  sign code  pat number"<<endl;
  for(int iPAC=minPAC;iPAC<=maxPAC;iPAC++){
    for(int i=0;i<8;i++){
      out<<"("<<iPAC<<", T, 0, 0, (";
      for(int iLogPlane=0;iLogPlane<6;iLogPlane++){
	int strip= (RPCRingFromRolls::m_LOGPLANE_SIZE[iTower][iLogPlane]-8)/2+i;
	out<<"( "<<setw(2)<<strip<<", "<<strip<<")";
	if(iLogPlane<5) out<<", ";
      }
      out<<"),";
      out<<"  1,  "<<i+1<<")";
      if(i!=7 || iPAC<maxPAC-minPAC) out<<", ";
      out<<"--0"<<endl;
    }
  }
  out<<");"<<endl<<endl;
}


void RPCVHDLConeMaker::writeXMLPatternsDef(std::ofstream & out){

  using namespace edm;
  using namespace std;


  ///
  int aTower = 0;
  int minPAC = 0;
  int maxPAC = 0;
  ///

  int iTower = aTower;

  //Get current time
  time_t rawtime;
  struct tm * timeinfo;
  time ( &rawtime );
  timeinfo = localtime ( &rawtime );
  asctime (timeinfo);
  //
  out<<"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>"<<endl;
  out<<"<?xml-stylesheet type=\"text/xsl\" href=\"default.xsl\"?>"<<endl;
  out<<"<pacdef>"<<endl;
  out<<"<date>"<<asctime(timeinfo);
  out<<"</date>"<<endl;
  out<<""<<endl;
  out<<"<descr>-- EfficiencyCut 0.9"<<endl;
  out<<"-- Simple patterns for the MTCC tests."<<endl;
  out<<"</descr>"<<endl;
  out<<endl;
  out<<"<qualitTable>"<<endl;
  out<<"<quality id=\"0\" planes=\"001111\" val=\"1\"/>"<<endl;
  out<<"<quality id=\"0\" planes=\"010111\" val=\"1\"/>"<<endl;
  out<<"<quality id=\"0\" planes=\"011011\" val=\"1\"/>"<<endl;
  out<<"<quality id=\"0\" planes=\"011101\" val=\"1\"/>"<<endl;
  out<<"<quality id=\"0\" planes=\"011110\" val=\"1\"/>"<<endl;
  out<<"<quality id=\"0\" planes=\"011111\" val=\"2\"/>"<<endl;
  out<<"<quality id=\"0\" planes=\"100111\" val=\"1\"/>"<<endl;
  out<<"<quality id=\"0\" planes=\"101011\" val=\"1\"/>"<<endl;
  out<<"<quality id=\"0\" planes=\"101101\" val=\"1\"/>"<<endl;
  out<<"<quality id=\"0\" planes=\"101110\" val=\"1\"/>"<<endl;
  out<<"<quality id=\"0\" planes=\"101111\" val=\"2\"/>"<<endl;
  out<<"<quality id=\"0\" planes=\"110011\" val=\"1\"/>"<<endl;
  out<<"<quality id=\"0\" planes=\"110101\" val=\"1\"/>"<<endl;
  out<<"<quality id=\"0\" planes=\"110110\" val=\"1\"/>"<<endl;
  out<<"<quality id=\"0\" planes=\"110111\" val=\"2\"/>"<<endl;
  out<<"<quality id=\"0\" planes=\"111001\" val=\"1\"/>"<<endl;
  out<<"<quality id=\"0\" planes=\"111010\" val=\"1\"/>"<<endl;
  out<<"<quality id=\"0\" planes=\"111011\" val=\"2\"/>"<<endl;
  out<<"<quality id=\"0\" planes=\"111100\" val=\"1\"/>"<<endl;
  out<<"<quality id=\"0\" planes=\"111101\" val=\"2\"/>"<<endl;
  out<<"<quality id=\"0\" planes=\"111110\" val=\"2\"/>"<<endl;
  out<<"<quality id=\"0\" planes=\"111111\" val=\"3\"/>"<<endl;
  out<<"<quality id=\"1\" planes=\"000111\" val=\"0\"/>"<<endl;
  out<<"<quality id=\"1\" planes=\"001011\" val=\"0\"/>"<<endl;
  out<<"<quality id=\"1\" planes=\"001101\" val=\"0\"/>"<<endl;
  out<<"<quality id=\"1\" planes=\"001110\" val=\"0\"/>"<<endl;
  out<<"<quality id=\"1\" planes=\"001111\" val=\"1\"/>"<<endl;
  out<<"</qualitTable>"<<endl;
  out<<endl;
  out<<"<pac logSector=\"0\" logSegment=\"0\" tower=\""<<iTower<<"\">"<<endl;

    for(int i=0;i<8;i++){
      out<<"<pat code=\""<<i+1<<"\" grp=\"0\" num=\"0\" qual=\"0\" sign=\"1\" type=\"T\">"<<endl;
      for(int iLogPlane=0;iLogPlane<6;iLogPlane++){
	int strip= (RPCRingFromRolls::m_LOGPLANE_SIZE[iTower][iLogPlane]-8)/2+i;
	out<<"<str Pl=\""<<iLogPlane
	   <<"\" f=\""<<strip
	   <<"\" t=\""<<strip<<"\"/>"<<endl;
      }
      out<<"</pat>"<<endl;
    }
    out<<"</pac>"<<endl;
    out<<endl;
    out<<"</pacdef>"<<endl;
    
}


void RPCVHDLConeMaker::writeSorterDef(std::ofstream & out){

  using namespace edm;
  using namespace std;


  ///
  int aTower = 0;
  int minPAC = 0;
  int maxPAC = 2;
  //int maxPAC = 12;
  ///

  //out<<" "<<endl;
  out<<"\n \n constant GBSortDecl		:TGBSortDecl := ("<<endl;  
  out<<"--PAC_INDEX"<<endl;  
  out<<"--|   PAC_MODEL"<<endl;  
  out<<"--|	 |   GBSORT_INPUT_INDEX"<<endl;  
  for(int i=0;i<(maxPAC-minPAC);i++){
    out<<" ("<<i<<",\t T,\t"<<i<<")";
    if(i<(maxPAC-minPAC)-1) out<<",";
    out<<endl;
    //out<<",(2,   T,  2)"<<endl;  
  }
  out<<");"<<endl;  
  out<<""<<endl;  
  out<<"end RPC_PAC_patt;"<<endl;  
}
