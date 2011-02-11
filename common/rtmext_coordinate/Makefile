CXXFLAGS = `rtm-config --cflags` -I. 
LDFLAGS  = `rtm-config --libs` -lm
SHFLAGS  = -shared

IDLC     = `rtm-config --idlc`
IDLFLAGS = `rtm-config --idlflags` -I`rtm-config --prefix`/include/rtm/idl
WRAPPER  = rtm-skelwrapper
WRAPPER_FLAGS = --include-dir="" --skel-suffix=Skel --stub-suffix=Stub

STUB_OBJ = TimedCoordinateStub.o
OBJS     = Coordinate.o $(SKEL_OBJ) $(STUB_OBJ)

.SUFFIXES : .so

all: Coordinate.so

.cpp.o:
	rm -f $@
	$(CXX) $(CXXFLAGS) -c -o $@ $<

.o.so:
	rm -f $@
	$(CXX) $(SHFLAGS) -o $@ $(OBJS) $(LDFLAGS)

Coordinate.so: $(OBJS)
Coordinate.o: Coordinate.h TimedCoordinateStub.h

TimedCoordinateStub.o: TimedCoordinateStub.cpp TimedCoordinateStub.h
TimedCoordinateStub.cpp : TimedCoordinateStub.h
TimedCoordinateStub.h : TimedCoordinate.idl
	$(IDLC) $(IDLFLAGS) $^
	$(WRAPPER) $(WRAPPER_FLAGS) --idl-file=$^

clean: clean_skelstub
	rm -f $(OBJS) Coordinate.so

clean_skelstub:
	rm -f *Skel.h *Skel.cpp
	rm -f *Stub.h *Stub.cpp
	rm -f TimedCoordinateDynSK.cc TimedCoordinateSK.cc TimedCoordinate.hh
