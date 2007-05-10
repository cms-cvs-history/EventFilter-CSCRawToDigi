#include "EventFilter/CSCRawToDigi/interface/CSCDCCUnpacker.h"

//Framework stuff
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

//FEDRawData
#include "DataFormats/FEDRawData/interface/FEDRawData.h"
#include "DataFormats/FEDRawData/interface/FEDNumbering.h"
#include "DataFormats/FEDRawData/interface/FEDRawDataCollection.h"

//Digi stuff
#include "DataFormats/CSCDigi/interface/CSCStripDigi.h"
#include "DataFormats/CSCDigi/interface/CSCCFEBStatusDigi.h"
#include "DataFormats/CSCDigi/interface/CSCWireDigi.h"
#include "DataFormats/CSCDigi/interface/CSCComparatorDigi.h"
#include "DataFormats/CSCDigi/interface/CSCALCTDigi.h"
#include "DataFormats/CSCDigi/interface/CSCCLCTDigi.h"
#include "DataFormats/CSCDigi/interface/CSCRPCDigi.h"
#include "DataFormats/CSCDigi/interface/CSCCorrelatedLCTDigi.h"

#include "DataFormats/CSCDigi/interface/CSCStripDigiCollection.h"
#include "DataFormats/CSCDigi/interface/CSCCFEBStatusDigiCollection.h"
#include "DataFormats/CSCDigi/interface/CSCWireDigiCollection.h"
#include "DataFormats/CSCDigi/interface/CSCComparatorDigiCollection.h"
#include "DataFormats/CSCDigi/interface/CSCALCTDigiCollection.h"
#include "DataFormats/CSCDigi/interface/CSCCLCTDigiCollection.h"
#include "DataFormats/CSCDigi/interface/CSCRPCDigiCollection.h"
#include "DataFormats/CSCDigi/interface/CSCCorrelatedLCTDigiCollection.h"

#include "DataFormats/MuonDetId/interface/CSCDetId.h"

#include "EventFilter/CSCRawToDigi/interface/CSCDCCEventData.h"
#include "EventFilter/CSCRawToDigi/interface/CSCEventData.h"
#include "EventFilter/CSCRawToDigi/interface/CSCCFEBData.h"
#include "EventFilter/CSCRawToDigi/interface/CSCALCTHeader.h"
#include "EventFilter/CSCRawToDigi/interface/CSCAnodeData.h"
#include "EventFilter/CSCRawToDigi/interface/CSCCLCTData.h"
#include "EventFilter/CSCRawToDigi/interface/CSCDDUEventData.h"
#include "EventFilter/CSCRawToDigi/interface/CSCTMBData.h"
#include "EventFilter/CSCRawToDigi/interface/CSCTMBHeader.h"
#include "EventFilter/CSCRawToDigi/interface/CSCRPCData.h"
#include "EventFilter/CSCRawToDigi/interface/CSCDCCExaminer.h"


#include <EventFilter/CSCRawToDigi/interface/CSCMonitorInterface.h>
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "CondFormats/CSCObjects/interface/CSCReadoutMappingFromFile.h"

#include <iostream>


CSCDCCUnpacker::CSCDCCUnpacker(const edm::ParameterSet & pset) :
  numOfEvents(0){

  PrintEventNumber = pset.getUntrackedParameter<bool>("PrintEventNumber", true);
  debug = pset.getUntrackedParameter<bool>("Debug", false);
  useExaminer = pset.getUntrackedParameter<bool>("UseExaminer", true);
  examinerMask = pset.getUntrackedParameter<unsigned int>("ExaminerMask",0x7FB7BF6);
  instatiateDQM = pset.getUntrackedParameter<bool>("runDQM", false);
  errorMask = pset.getUntrackedParameter<unsigned int>("ErrorMask",0xDFCFEFFF);
  unpackStatusDigis = pset.getUntrackedParameter<bool>("UnpackStatusDigis", false);
  inputObjectsTag = pset.getParameter<edm::InputTag>("InputObjects");

  if(instatiateDQM)
    {
      monitor = edm::Service<CSCMonitorInterface>().operator->();
    }

  produces<CSCWireDigiCollection>("MuonCSCWireDigi");
  produces<CSCStripDigiCollection>("MuonCSCStripDigi");
  produces<CSCComparatorDigiCollection>("MuonCSCComparatorDigi");
  produces<CSCALCTDigiCollection>("MuonCSCALCTDigi");
  produces<CSCCLCTDigiCollection>("MuonCSCCLCTDigi");
  produces<CSCRPCDigiCollection>("MuonCSCRPCDigi");
  produces<CSCCorrelatedLCTDigiCollection>("MuonCSCCorrelatedLCTDigi");
  if (unpackStatusDigis) produces<CSCCFEBStatusDigiCollection>("MuonCSCCFEBStatusDigi");

  CSCAnodeData::setDebug(debug);
  CSCALCTHeader::setDebug(debug);
  CSCCLCTData::setDebug(debug);
  CSCEventData::setDebug(debug);
  CSCTMBData::setDebug(debug);
  CSCDCCEventData::setDebug(debug);
  CSCDDUEventData::setDebug(debug);
  CSCTMBHeader::setDebug(debug);
  CSCRPCData::setDebug(debug);

  CSCDDUEventData::setErrorMask(errorMask);

  theMapping  = CSCReadoutMappingFromFile(pset);

}

CSCDCCUnpacker::~CSCDCCUnpacker()
{
  
  //fill destructor here

}


void CSCDCCUnpacker::produce(edm::Event & e, const edm::EventSetup& c)
{

  //++numOfEvents;
  /// Get a handle to the FED data collection
  edm::Handle<FEDRawDataCollection> rawdata;
  e.getByLabel(inputObjectsTag, rawdata);
  //e.getByLabel("source", rawdata);

  /// create the collection of CSC wire and strip Digis
  std::auto_ptr<CSCWireDigiCollection> wireProduct(new CSCWireDigiCollection);
  std::auto_ptr<CSCStripDigiCollection> stripProduct(new CSCStripDigiCollection);
  std::auto_ptr<CSCALCTDigiCollection> alctProduct(new CSCALCTDigiCollection);
  std::auto_ptr<CSCCLCTDigiCollection> clctProduct(new CSCCLCTDigiCollection);
  std::auto_ptr<CSCComparatorDigiCollection> comparatorProduct(new CSCComparatorDigiCollection);
  std::auto_ptr<CSCRPCDigiCollection> rpcProduct(new CSCRPCDigiCollection);
  std::auto_ptr<CSCCorrelatedLCTDigiCollection> corrlctProduct(new CSCCorrelatedLCTDigiCollection);
  std::auto_ptr<CSCCFEBStatusDigiCollection> cfebStatusProduct(new CSCCFEBStatusDigiCollection);

  for (int id=FEDNumbering::getCSCFEDIds().first;
       id<=FEDNumbering::getCSCFEDIds().second; ++id)
    { //for each of our DCCs
      /// uncomment this for regional unpacking
      /// if (id!=SOME_ID) continue;

      /// Take a reference to this FED's data
      const FEDRawData& fedData = rawdata->FEDData(id);
      unsigned short int length =  fedData.size();

      if (length)
	{ ///if fed has data then unpack it

	  goodEvent = true;
	  if (useExaminer) 
	    {///examine event for integrity
	      CSCDCCExaminer examiner;
	      if( examinerMask&0x40000 ) examiner.crcCFEB(1);
	      if( examinerMask&0x8000  ) examiner.crcTMB (1);
	      if( examinerMask&0x0400  ) examiner.crcALCT(1);
	      examiner.output1().hide();
	      examiner.output2().hide();
	      const short unsigned int *data = (short unsigned int *)fedData.data();
	      if( examiner.check(data,long(fedData.size()/2)) != 0 )
		{
		  goodEvent=false;
		} 
	      else 
		{
		  goodEvent=!(examiner.errors()&examinerMask);
		}
	    }

      
	  if (goodEvent) 
	    {
	      ///get a pointer to data and pass it to constructor for unpacking
	      CSCDCCEventData dccData((short unsigned int *) fedData.data());

	      if(instatiateDQM) monitor->process(dccData);

	      ///get a reference to dduData
	      const std::vector<CSCDDUEventData> & dduData = dccData.dduData();

	      for (unsigned int iDDU=0; iDDU<dduData.size(); ++iDDU) 
		{  ///loop over DDUs

		  ///get a reference to chamber data
		  const std::vector<CSCEventData> & cscData = dduData[iDDU].cscData();
		  
		  ///skip the DDU if its data has serious errors
		  /// define a mask for serious errors  (currently DFCFEFFF)
		  if (dduData[iDDU].trailer().errorstat()&errorMask) 
		    {
		      edm::LogError("CSCDCCUnpacker") << "DDU has errors - Digis are not stored! " <<
			std::hex << dduData[iDDU].trailer().errorstat();
		      continue;
		    }
		  for (unsigned int iCSC=0; iCSC<cscData.size(); ++iCSC) 
		    { //loop over CSCs

		      ///first process chamber-wide digis such as LCT
		      int endcap = 1;
		      int station = 1;
		      int tmb = 1;
		      int vmecrate = cscData[iCSC].dmbHeader().crateID();
		      int dmb = cscData[iCSC].dmbHeader().dmbID();
		      int icfeb = 0; ///default value for all digis not related to cfebs
		      int ilayer = 0; /// zeroth layer indicates whole chamber

		      if (debug)
			edm::LogInfo ("CSCDCCUnpacker") << "crate = " << vmecrate << "; dmb = " << dmb;

		      /// this is some default value of CSCDetID
		      CSCDetId layer(1, //endcap
				     1, //station
				     1, //ring
				     1, //chamber
				     1); //layer

		      if ( (vmecrate>=0)&&(vmecrate<=200) && (dmb>=0)&&(dmb<=10) ) 
			{
			  layer = theMapping.detId( endcap, station, vmecrate, dmb, tmb,icfeb,ilayer );
			} 
		      else 
			{
			  edm::LogError ("CSCDCCUnpacker") << " detID input out of range!!! ";
			  edm::LogError ("CSCDCCUnpacker") << " skipping this chamber! ";
			  continue;
			}


		      /// fill alct digi
		      int nalct = cscData[iCSC].dmbHeader().nalct();
		      if (nalct) 
			{
			  if (cscData[iCSC].alctHeader().check()) 
			    {
			      std::vector <CSCALCTDigi>  alctDigis =
				cscData[iCSC].alctHeader().ALCTDigis();
			      alctProduct->put(std::make_pair(alctDigis.begin(), alctDigis.end()),layer);
			    }
			  else  edm::LogError ("CSCDCCUnpacker") << 
				  "ALCT check failed! not storing ALCT digi ";
			}

		      ///fill correlatedlct and clct digi
		      int nclct = cscData[iCSC].dmbHeader().nclct();
		      bool goodTMB=false;
		      if (nclct) 
			{
			  if (cscData[iCSC].tmbHeader().check())
			    {
			      if (cscData[iCSC].clctData().check()) goodTMB=true; 
			    }
			  else 
			    {
			      edm::LogError ("CSCDCCUnpacker") <<
				"one of TMB checks failed! not storing TMB digis ";
			    }
			}

		      if (goodTMB) 
			{ 
			  std::vector <CSCCorrelatedLCTDigi>  correlatedlctDigis =
			    cscData[iCSC].tmbHeader().CorrelatedLCTDigis();
			  corrlctProduct->put(std::make_pair(correlatedlctDigis.begin(),
							     correlatedlctDigis.end()),layer);
			  std::vector <CSCCLCTDigi>  clctDigis =
			    cscData[iCSC].tmbHeader().CLCTDigis();
			  clctProduct->put(std::make_pair(clctDigis.begin(), clctDigis.end()),layer);
			
			  /// fill cscRpc digi
			  if (cscData[iCSC].tmbData().checkSize()) 
			    {
			      if (cscData[iCSC].tmbData().hasRPC()) 
				{
				  std::vector <CSCRPCDigi>  rpcDigis =
				    cscData[iCSC].tmbData().rpcData().digis();
				  rpcProduct->put(std::make_pair(rpcDigis.begin(), rpcDigis.end()),layer);
				}
			    } 
			  else edm::LogError("CSCDCCUnpacker") <<" TMBData check size failed!";
			  
			}
		      
		      /// fill CFEBStatusDigi
		      if (unpackStatusDigis) 
			{
			  for ( icfeb = 0; icfeb < 5; ++icfeb ) 
			    {///loop over status digis
			      if ( cscData[iCSC].cfebData(icfeb) != NULL )
				cfebStatusProduct->
				  insertDigi(layer, cscData[iCSC].cfebData(icfeb)->statusDigi());
			    }
			}


		      ///this loop stores wire, strip and comparator digis:
		      for (int ilayer = 1; ilayer <= 6; ++ilayer) 
			{
			  /// set layer (dmb and vme are valid because already checked in line 205
			  layer = theMapping.detId( endcap, station, vmecrate, dmb, tmb,icfeb,ilayer );

			  std::vector <CSCWireDigi> wireDigis =  cscData[iCSC].wireDigis(ilayer);
			  wireProduct->put(std::make_pair(wireDigis.begin(), wireDigis.end()),layer);
			  
			  for ( icfeb = 0; icfeb < 5; ++icfeb )
			    {
			      layer = theMapping.detId( endcap, station, vmecrate, dmb, tmb,icfeb,ilayer );
			      if (cscData[iCSC].cfebData(icfeb)) 
				{
				  std::vector<CSCStripDigi> stripDigis;
				  cscData[iCSC].cfebData(icfeb)->digis(layer.rawId(),stripDigis);
				  stripProduct->put(std::make_pair(stripDigis.begin(), 
								   stripDigis.end()),layer);
				}

			      if (goodTMB) 
				{
				  std::vector <CSCComparatorDigi>  comparatorDigis =
				    cscData[iCSC].clctData().comparatorDigis(ilayer, icfeb);
				  comparatorProduct->put(std::make_pair(comparatorDigis.begin(), 
									    comparatorDigis.end()),layer);
				}
			    }///end of loop over cfebs
			}///end of loop over layers
		    }///end of loop over chambers
		}///endof loop over DDUs
	    }///end of good event
	  else 
	    {
	      edm::LogError("CSCDCCUnpacker") <<" Examiner deemed the event bad!";
	    }
	}///end of if fed has data
    }///end of loop over DCCs

  // commit to the event
  e.put(wireProduct,          "MuonCSCWireDigi");
  e.put(stripProduct,         "MuonCSCStripDigi");
  e.put(alctProduct,          "MuonCSCALCTDigi");
  e.put(clctProduct,          "MuonCSCCLCTDigi");
  e.put(comparatorProduct,    "MuonCSCComparatorDigi");
  e.put(rpcProduct,           "MuonCSCRPCDigi");
  e.put(corrlctProduct,       "MuonCSCCorrelatedLCTDigi");
  if (unpackStatusDigis) e.put(cfebStatusProduct,    "MuonCSCCFEBStatusDigi");

  //if (PrintEventNumber) edm::LogInfo("CSCDCCUnpacker") 
  //  <<"**************[DCCUnpackingModule]:" << numOfEvents<<" events analyzed ";
}




