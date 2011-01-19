CXXFLAGS = `rtm-config --cflags` -I. `pkg-config --cflags opencv`
LDFLAGS  = `rtm-config --libs` `pkg-config --libs opencv`
SHFLAGS  = -shared

OBJS     = ImageTranslater.o

.SUFFIXES : .so

all: ImageTranslater.so

.cpp.o:
	rm -f $@
	$(CXX) $(CXXFLAGS) -c -o $@ $<

.o.so:
	rm -f $@
	$(CXX) $(SHFLAGS) -o $@ $(OBJS) $(LDFLAGS)

ImageTranslater.so: $(OBJS)
ImageTranslater.o: ImageTranslater.h

clean:
	rm -f $(OBJS) ImageTranslater.so
