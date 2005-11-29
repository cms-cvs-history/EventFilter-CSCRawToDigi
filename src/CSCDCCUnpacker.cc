#include "EventFilter/CSCRawToDigi/interface/CSCDCCUnpacker.h"

//Framework stuff
#include "FWCore/Framework/interface/Handle.h"
#include "FWCore/Framework/interface/Event.h"

//FEDRawData 
#include "DataFormats/FEDRawData/interface/FEDRawData.h"
#include "DataFormats/FEDRawData/interface/FEDNumbering.h"
#include "DataFormats/FEDRawData/interface/FEDRawDataCollection.h"

//Digi stuff
#include "DataFormats/CSCDigi/interface/CSCStripDigi.h"
#include "DataFormats/CSCDigi/interface/CSCWireDigi.h"
#include "DataFormats/CSCDigi/interface/CSCComparatorDigi.h"
//#include "DataFormats/CSCDigi/interface/CSCALCTDigi.h"
//#include "DataFormats/CSCDigi/interface/CSCCLCTDigi.h"
//Include LCT digis later

#include "DataFormats/CSCDigi/interface/CSCRPCDigi.h"

#include "DataFormats/CSCDigi/interface/CSCStripDigiCollection.h"
#include "DataFormats/CSCDigi/interface/CSCWireDigiCollection.h"
#include "DataFormats/CSCDigi/interface/CSCComparatorDigiCollection.h"
//#include "DataFormats/CSCDigi/interface/CSCALCTDigiCollection.h"
//#include "DataFormats/CSCDigi/interface/CSCCLCTDigiCollection.h"
#include "DataFormats/CSCDigi/interface/CSCRPCDigiCollection.h"

#include "DataFormats/MuonDetId/interface/CSCDetId.h"

#include "EventFilter/CSCRawToDigi/interface/CSCDCCEventData.h"
#include "EventFilter/CSCRawToDigi/interface/CSCEventData.h"


#include <iostream>


CSCDCCUnpacker::CSCDCCUnpacker(const edm::ParameterSet & pset){
  //fill constructor here
  dccData = 0;
  produces<CSCWireDigiCollection>();
  produces<CSCStripDigiCollection>();


}

CSCDCCUnpacker::~CSCDCCUnpacker(){

  //fill destructor here
  delete dccData;   

}


void CSCDCCUnpacker::produce(edm::Event & e, const edm::EventSetup& c){

  // Get a handle to the FED data collection
  edm::Handle<FEDRawDataCollection> rawdata;
  e.getByLabel("DaqRawData", rawdata);

  // create the collection of CSC wire and strip Digis
  std::auto_ptr<CSCWireDigiCollection> wireProduct(new CSCWireDigiCollection);
  std::auto_ptr<CSCStripDigiCollection> stripProduct(new CSCStripDigiCollection);
  //std::auto_ptr<CSCALCTDigiCollection> alctProduct(new CSCALCTDigiCollection);
  //std::auto_ptr<CSCCLCTpDigiCollection> clctProduct(new CSCCLCTDigiCollection);
  //std::auto_ptr<CSCComparatorDigiCollection> ComparatorProduct(new CSCComparatorDigiCollection);
  //std::auto_ptr<CSCRPCDigiCollection> RPCProduct(new CSCRPCDigiCollection);

  for (int id=FEDNumbering::getCSCFEDIds().first;
       id<=FEDNumbering::getCSCFEDIds().second; ++id){ //for each of our DCCs
    
    // Take a reference to this FED's data
    const FEDRawData& fedData = rawdata->FEDData(id);

    if (fedData.size()){ //unpack data 
     
      //get a pointer to dcc data and pass it to constructor for unpacking
      dccData=new CSCDCCEventData((short unsigned int *) fedData.data()); 


      //get a reference to dduData
      const std::vector<CSCDDUEventData> & dduData = dccData->dduData(); 

      for (int iDDU=0; iDDU<dduData.size(); ++iDDU) {  //loop over DDUs
	
	//get a reference to chamber data
	const std::vector<CSCEventData> & cscData = dduData[iDDU].cscData();
	
	for (int iCSC=0; iCSC<cscData.size(); ++iCSC) { //loop over CSCs

	  ///Digis for each chamber must be obtained here	
	  ///below is an example for  wire digis 
	  ///it must be repeated for all 6 types!
	  
	  for (int ilayer = 1; ilayer <= 6; ilayer++) { 

            #warning Fake mapping of the endcap, station, ring and chamber!!!
	    CSCDetId layer(1, //endcap
			   1, //station
			   1, //ring
			   1, //chamber
			   ilayer); //layer


	    std::vector <CSCWireDigi> wireDigis =  cscData[iCSC].wireDigis(ilayer);
	    for (int i=0; i<wireDigis.size() ; i++) {
	      wireProduct->insertDigi(layer, wireDigis[i]);
	    }

	    std::vector <CSCStripDigi> stripDigis =  cscData[iCSC].stripDigis(ilayer);
            for (int i=0; i<stripDigis.size() ; i++) {
              stripProduct->insertDigi(layer, stripDigis[i]);
            }

	  }
	}
      }     
    }
  }
  // commit to the event
  e.put(wireProduct);
  e.put(stripProduct);
  //e.put(ALCTProduct);
  //e.put(CLCTProduct);
  //e.put(ComparatorProduct);
  //e.put(RPCProduct);

}




