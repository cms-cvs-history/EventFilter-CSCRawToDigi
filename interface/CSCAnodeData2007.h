#ifndef CSCAnodeData2007_h
#define CSCAnodeData2007_h

#include "EventFilter/CSCRawToDigi/interface/CSCAnodeDataFormat.h"
class CSCALCTHeader;

class CSCAnodeDataFrame2007 {
 public:
  CSCAnodeDataFrame2007() {}

  /// given a wiregroup between 0 and 11, it tells whether this bit was on
  bool isHit(unsigned wireGroup) const {
    assert(wireGroup < 12);
    return ( (data_>>wireGroup) & 0x1 );
  }

  unsigned short data() const {return data_;}
  
 private:
  unsigned short data_     : 12;
  unsigned short reserved_ : 3;
  unsigned short flag_     : 1;
};


class CSCAnodeData2007 : public CSCAnodeDataFormat {

public:
  /// a blank one, for Monte Carlo
  explicit CSCAnodeData2007(const CSCALCTHeader &);
  /// fill from a real datastream
  CSCAnodeData2007(const CSCALCTHeader &, const unsigned short *buf);

  virtual unsigned short * data() {return theDataFrames;}
  /// the amount of the input binary buffer read, in 16-bit words
  virtual unsigned short int sizeInWords() const {return sizeInWords2007_;}

  /// input layer is from 1 to 6
  virtual std::vector<CSCWireDigi> wireDigis(int layer) const;
  
  virtual void add(const CSCWireDigi &, int layer);


private:
  void init(const CSCALCTHeader &);
  int index(int tbin, int layer, int layerPart) const;
  const CSCAnodeDataFrame2007 & findFrame(int tbin, int layer, int layerPart) const;
  CSCAnodeDataFrame2007 & findFrame(int tbin, int layer, int layerPart);

  /// we don't know the size at first.  Max should be 7 boards * 32 bins * 6 layers * 2
  unsigned short theDataFrames[2700];
  unsigned short int sizeInWords2007_;
  unsigned short int nAFEBs_;
  unsigned short int  nTimeBins_;

  unsigned short int layerParts_;///number of layer parts in the ALCT
  unsigned short int maxWireGroups_;///number of wiregroups in the ALCT
};

#endif

