#ifndef CSCCFEBTimeSlice_h
#define CSCCFEBTimeSlice_h


/**
 CFEB Data Stream 
The ordering of the words in the data stream from a single CFEB is described by the following nested loops: 

do (N_ts  time samples){ 
  do (Gray code loop over 16 CSC Strips; S=0,1,3,2,6,7,5,4,12,13,15,14,10,11,9,8){ 
    do (loop over 6 CSC Layers; L=3,1,5,6,4,2){ 
    }
  } 
  CRC word 
  CFEB Info word 98 
  CFEB Info word 99 
  Dummy word (0x7FFF)
}
*/

struct CSCCFEBDataWord {
  unsigned short adcCounts   : 12;
  unsigned short adcOverflow : 1;
  /// combined from all 16 strips to make a word
  unsigned short controllerData : 1;
  /// Overlapped sample flag (normally HIGH;
  /// set LOW when two separate LCTs share a time sample).
  unsigned short overlappedSampleFlag : 1;
  /// Always LOW for data words.  HIGH for DDU Code word
  /// (maybe End of Event or Error)
  unsigned short errorstat : 1;
};

#include <iostream>
#include <string.h> //for bzero

struct CSCCFEBSCAControllerWord {
  /**
TRIG_TIME indicates which of the eight time samples in the 400ns SCA block (lowest bit is the first sample, highest bit the eighth sample) corresponds to the arrival of the LCT; it should be at some fixed phase relative to the peak of the CSC pulse.  SCA_BLK is the SCA Capacitor block used for this time sample. L1A_PHASE and LCT_PHASE show the phase of the 50ns CFEB digitization clock at the time the trigger was received (1=clock high, 0=clock low).  SCA_FULL indicates lost SCA data due to SCA full condition.  The TS_FLAG bit indicates the number of time samples to digitize per event; high=16 time samples, low=8 time samples. 

  */
  CSCCFEBSCAControllerWord() {bzero(this, 2);}

  unsigned short trig_time : 8;
  unsigned short sca_blk   : 4;
  unsigned short l1a_phase : 1;
  unsigned short lct_phase : 1;
  unsigned short sca_full  : 1;
  unsigned short ts_flag   : 1;
};




class CSCCFEBTimeSlice {
 public:
  CSCCFEBTimeSlice() : dummy(0x7fff) {
    bzero(this, 99*2);
  }
    // 
  /// input from 0 to 95
  CSCCFEBDataWord * timeSample(int index) const {
    return (CSCCFEBDataWord *)(theSamples+index);
  }
  /// layer and element count from one
  CSCCFEBDataWord * timeSample(int layer, int channel) const;

  /// whether we keep 8 or 16 time samples
  bool sixteenSamples() {/*return scaControllerWord(1).ts_flag;i*/
    return timeSample(95)->controllerData;}
  unsigned sizeInWords() const {return 100;}

  /// unpacked from the controller words for each channel in the layer
  CSCCFEBSCAControllerWord scaControllerWord(int layer);
  
  void setControllerWord(const CSCCFEBSCAControllerWord & controllerWord);

  bool check() const {return dummy == 0x7FFF;}

  friend std::ostream & operator<<(std::ostream & os, const CSCCFEBTimeSlice &);

 private:
  unsigned short theSamples[96];

  /// WORD 97
  unsigned crc : 16;

  /// WORD 98
  unsigned n_free_sca_blocks : 4;
  unsigned lctpipe_count : 4;
  unsigned lctpipe_full  : 1;
  unsigned l1pipe_full   : 1;
  unsigned lctpipe_empty : 1;
  unsigned l1pipe_empty  : 1;
  unsigned blank_space_1 : 4;

  /// WORD 99
  unsigned l1pipe_cnt : 8;
  /// indicates that a used SCA block is now available
  unsigned lct_pop    : 1;
  /// CFEB_PUSH indicates that data is being sent to the DMB
  unsigned cfeb_push  : 1;
  unsigned sca_full   : 1;
  unsigned busy       : 1;
  unsigned blank_space_2 : 4;

  /// word 100 is a dummy: 0x7FFF
  unsigned dummy : 16;
};

#endif
