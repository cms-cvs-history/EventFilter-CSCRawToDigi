/** \file
 *
 *  $Date: 2008/05/02 20:16:35 $
 *  $Revision: 1.7 $
 *  \author A. Tumanov - Rice
 */

#include <EventFilter/CSCRawToDigi/src/CSCDigiToRawModule.h>
#include <EventFilter/CSCRawToDigi/src/CSCDigiToRaw.h>
#include <DataFormats/FEDRawData/interface/FEDRawDataCollection.h>
#include <DataFormats/CSCDigi/interface/CSCStripDigiCollection.h>
#include <DataFormats/CSCDigi/interface/CSCWireDigiCollection.h>
#include "DataFormats/Common/interface/Handle.h"
#include <FWCore/Framework/interface/Event.h>
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "CondFormats/DataRecord/interface/CSCChamberMapRcd.h"



using namespace edm;
using namespace std;

CSCDigiToRawModule::CSCDigiToRawModule(const edm::ParameterSet & pset): 
  packer(new CSCDigiToRaw),
  theStripDigiTag(pset.getParameter<edm::InputTag>("stripDigiTag")),
  theWireDigiTag(pset.getParameter<edm::InputTag>("wireDigiTag")),
  theComparatorDigiTag(pset.getParameter<edm::InputTag>("comparatorDigiTag")),
  theALCTDigiTag(pset.getParameter<edm::InputTag>("alctDigiTag")),
  theCLCTDigiTag(pset.getParameter<edm::InputTag>("clctDigiTag")),
  theCorrelatedLCTDigiTag(pset.getParameter<edm::InputTag>("correlatedLCTDigiTag"))
{
  //theStrip = pset.getUntrackedParameter<string>("DigiCreator", "cscunpacker");
  produces<FEDRawDataCollection>("CSCRawData"); 
}


CSCDigiToRawModule::~CSCDigiToRawModule(){
  delete packer;
}


void CSCDigiToRawModule::produce(Event & e, const EventSetup& c){
  ///reverse mapping for packer
  edm::ESHandle<CSCChamberMap> hcham;
  c.get<CSCChamberMapRcd>().get(hcham); 
  const CSCChamberMap* theMapping = hcham.product();


  auto_ptr<FEDRawDataCollection> fed_buffers(new FEDRawDataCollection);
  // Take digis from the event
  Handle<CSCStripDigiCollection> stripDigis;
  e.getByLabel(theStripDigiTag, stripDigis);
  Handle<CSCWireDigiCollection> wireDigis;
  e.getByLabel(theWireDigiTag, wireDigis);
  Handle<CSCComparatorDigiCollection> comparatorDigis;
  e.getByLabel(theComparatorDigiTag, comparatorDigis);
  Handle<CSCALCTDigiCollection> alctDigis;
  e.getByLabel(theALCTDigiTag, alctDigis);
  Handle<CSCCLCTDigiCollection> clctDigis;
  e.getByLabel(theCLCTDigiTag, clctDigis);
  Handle<CSCCorrelatedLCTDigiCollection> correlatedLCTDigis;
  e.getByLabel(theComparatorDigiTag, correlatedLCTDigis);



  // Create the packed data
  packer->createFedBuffers(*stripDigis, *wireDigis, *(fed_buffers.get()), theMapping, e);


  
  // put the raw data to the event
  e.put(fed_buffers, "CSCRawData");
}


