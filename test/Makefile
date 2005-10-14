CXX     = g++

CXXFLAGS  = -pedantic -Wno-long-long -Wall 

DEFINES   = -DUNPCK_ONLY -DL1CSC_STANDALONE $(INPDEF) $(OUTDEF)

#------------------------------------------------------
INCLUDES += -I../../ -I../../../ 

SRCPATH   = ../src
SRCS      = $(SRCPATH)/CSCCFEBData.cc $(SRCPATH)/CSCDCCHeader.cc\
            $(SRCPATH)/CSCDDUEventData.cc $(SRCPATH)/CSCDCCEventData.cc\
            $(SRCPATH)/CSCDDUHeader.cc \
            $(SRCPATH)/CSCDMBHeader.cc $(SRCPATH)/CSCEventData.cc\
            $(SRCPATH)/CSCCFEBTimeSlice.cc $(SRCPATH)/CSCDCCUnpacker.cc\
            $(SRCPATH)/CSCTMBHeader.cc $(SRCPATH)/CSCALCTHeader.cc \
            $(SRCPATH)/CSCCLCTData.cc $(SRCPATH)/CSCAnodeData.cc \
            $(SRCPATH)/CSCTMBScope.cc $(SRCPATH)/CSCTMBData.cc \
            $(SRCPATH)/CSCBadCFEBWord.cc $(SRCPATH)/CSCBadCFEBTimeSlice.cc \
            $(SRCPATH)/CSCRPCData.cc $(SRCPATH)/CSCTMBTrailer.cc

OBJS      = $(notdir $(SRCS:.cc=.o))

%.o: ../src/%.cc ../interface/%.h
	$(CXX) -c $(CXXFLAGS) $(DEFINES) $(INCLUDES) $<

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $(DEFINES) $(INCLUDES) $<


all: lib fastAnal 

lib: libCSCRawToDigi.a libCSCRawToDigi.so

fastAnal: fastAnal.o libCSCRawToDigi.a
	$(CXX) $(CXXFLAGS) $< libCSCRawToDigi.a $(ORCALIB) $(ROOTLIBS) -o $@

libCSCRawToDigi.so: $(OBJS)
	$(CXX) $(CXXFLAGS) $(DEFINES) $(INCLUDES) -shared $(OBJS) $(ORCALIB) -o $@

libCSCRawToDigi.a: $(OBJS)
	ar cr $@ $(OBJS)

clean:
	@rm -f libCSCRawToDigi.* *.o program RunNum* core*
