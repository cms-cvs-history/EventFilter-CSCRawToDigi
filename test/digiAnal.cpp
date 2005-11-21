#include "TriDAS/emu/emuDAQ/DDUReadout/include/FileReaderDDU.h"
#include "EventFilter/CSCRawToDigi/interface/CSCDDUEventData.h"
#include "DataFormats/CSCDigi/interface/CSCStripDigi.h"
#include "EventFilter/CSCRawToDigi/interface/CSCEventData.h"
#include "EventFilter/CSCRawToDigi/interface/CSCAnodeData.h"
#include "EventFilter/CSCRawToDigi/interface/CSCALCTHeader.h"
#include "EventFilter/CSCRawToDigi/interface/CSCCLCTData.h"
#include "EventFilter/CSCRawToDigi/interface/CSCTMBData.h"
#include "EventFilter/CSCRawToDigi/interface/CSCDDUTrailer.h"
#include "EventFilter/CSCRawToDigi/interface/CSCDMBHeader.h"
#include <iostream>


int deepAnal(const CSCEventData & data, int & nalct, int & nclct, int & ncfeb, int & nrpc) {
  if (data.nclct()){ 
    //std::cout << "NCLCT = " << data.nclct() << std::endl;
    nclct++;
  }
  if (data.nalct()) {
    nalct++;
    //std::cout << "NALCT = "<< data.nalct() << std::endl;

  }
  if ((data.cfebData(0))||
      (data.cfebData(1))||
      (data.cfebData(2))||
      (data.cfebData(3))||
      (data.cfebData(4))) ncfeb++;
  for (int ilayer = 1; ilayer <= 6; ilayer++) { 
    std::vector <CSCStripDigi> digis =  data.stripDigis(ilayer);
    for (int i=0; i<digis.size() ; i++) {
      std::cout << "digis" << i << std::endl; 
    }
  }
  return 0;  
}

int printStats(int k, int NreadEvents, int dmb, int nalct, int nclct, int ncfeb, int nrpc) {
  std::cout << "Fraction of chambers in dmb " << k << " = " << (float)dmb/NreadEvents << std::endl;
  std::cout << "   nalct = " <<(float) nalct/NreadEvents << std::endl;
  std::cout << "   nclct = " <<(float) nclct/NreadEvents << std::endl;
  std::cout << "   ncfeb = " <<(float) ncfeb/NreadEvents << std::endl;
  std::cout << "   nprc  = " <<(float) nrpc/NreadEvents << std::endl << std::endl;
}

int main(int argc, char **argv) {
   
  
  //CSCAnodeData::setDebug(true);
  //CSCALCTHeader::setDebug(true);
  //CSCCLCTData::setDebug(true); 
  //CSCEventData::setDebug(true);  
  //CSCTMBData::setDebug(true);
  //DDUReader::setDebug(true);
  //CSCDDUEventData::setDebug(true);

  int maxEvents = 500000;
  int dduConnect = 0;
  int mismatch = 0;
  int errorsDetected = 0;
  int reportedChambers =0;
  int unpackedChambers = 0;
  std::string datafile=argv[1];
  int NreadEvents=0;
  if (argv[2]) maxEvents = (int) atof(argv[2]);
  FileReaderDDU ddu;
  ddu.openFile(datafile);
  int const maxCham = 6;
  int nch[maxCham+1] = {0};
  int dmb[maxCham]   = {0};
  unsigned dmbID[maxCham]   = {0};
  int nalct[maxCham] = {0};
  int nclct[maxCham] = {0};
  int ncfeb[maxCham] = {0};
  int nrpc[maxCham]  = {0};

  for (int i = 0; ddu.readNextEvent() && (i < maxEvents); ++i){
    
    if (i>-1) {
    if (ddu.errorFlag) {std::cout << "error flag = " << std::hex 
			     << ddu.errorFlag << std::dec <<
			  std::endl; errorsDetected++; }  
    
    CSCDDUEventData dduEvent((short unsigned int *)ddu.data());
    std::cout << "checking dduEvent " << dduEvent.check() << std::endl;
    if (ddu.errorStat) {
      std::cout << "error stat = " << std::hex 
	   << ddu.errorStat << std::dec << std::endl;
      dduEvent.decodeStatus(ddu.errorStat);
    }
    const std::vector<CSCEventData> & cscData = dduEvent.cscData();  
    reportedChambers += dduEvent.header().ncsc();
    unpackedChambers += cscData.size();
    int NChambers = cscData.size();
    int repChambers = dduEvent.header().ncsc();
    std::cout << " Unpacked Chambers = " << NChambers << std::endl;
    std::cout << " Reported Chambers = " << repChambers << std::endl;
    if (NChambers!=repChambers) { std::cout<< "mismatched size!!!" << std::endl; mismatch++;}

    

    int dmb_dav = dduEvent.header().dmb_dav();     
    std::cout << "dmbdav = " << std::hex << dmb_dav << std::dec << std::endl;
  
    //dduConnect |= dmb_dav;    
    //std::cout << "dduConnect = " << hex << dduConnect << dec << std::endl;
   
    int dmb_full = dduEvent.trailer().dmb_full();     
    std::cout << "dmbfull = " << std::hex << dmb_full << std::dec << std::endl;
    int dmb_warn = dduEvent.trailer().dmb_warn();     
    std::cout << "dmbwarn = " << std::hex << dmb_warn << std::dec << std::endl;
  
  
    for (int j = 0; j<maxCham+1; j++) {
      if (NChambers==j) nch[j]++;
    }
    
    int g=0;
    
    int DDUmask = 0x3223; // 0011 0010 0010 0011
    bool dmbCondition[maxCham] = 
     {dmb_dav&0x1, dmb_dav&0x2, dmb_dav&0x20, dmb_dav&0x200, 
      dmb_dav&0x1000, dmb_dav&0x2000};
  
    //int DDUmask = 0x4800; // 0100 1000 0000 0000
    //bool dmbCondition[maxCham] = 
    //  {dmb_dav&0x1, dmb_dav&0x2, dmb_dav&0x20, dmb_dav&0x800, dmb_dav&0x4000};
  

    for (int k=0; k<maxCham; k++) {
      if (dmbCondition[k]){
        //dmbID[k]=cscData[g].dmbHeader.dmbID();
	deepAnal(cscData[g], nalct[k], nclct[k], ncfeb[k], nrpc[k]);
	dmb[k]++;
	g++;
      }
    }

    }
    NreadEvents = i+1;
    std::cout << "***************** End of event " << i+1 << std::endl;
    
  }
  
  std::cout << "Number of chambers reported by DDU during run = " << reportedChambers << std::endl;
  std::cout << "Number of chambers unpacked during run = " << unpackedChambers << std::endl;
  std::cout << "Number of read events during run = " << NreadEvents << std::endl;
  std::cout << "Number of events where mismatch occurs = " << mismatch << std::endl; 
  std::cout << "Number of events where errors detected = " << errorsDetected << std::endl; 
  for (int k=0; k<maxCham+1; k++) {
    std::cout << "Number of Events with " << k <<" chambers unpacked = " << nch[k] << std::endl;
  }
  for (int k=0; k<maxCham; k++) {
    printStats(k, NreadEvents, dmb[k], nalct[k], nclct[k], ncfeb[k], nrpc[k]);
  }

  ddu.closeFile();

}
