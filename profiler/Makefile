
# CXX = /g/MinGW/bin/g++
CXX = g++
CXXFLAGS = -DNO_SYMBOL -DNO_HASH_MAP -I../

DEBUG?=YES

ifeq ($(DEBUG),YES)
CXXFLAGS += -DDEBUG_PROFILER 
CXXFLAGS += -g 
LDFLAGS = -g -Wl,-Map,profiler.map
endif

PROFILER_OBJ = profiler.o 
OBJS = GccAddress2Symbol.o ../test/test.o

profiler.exe: $(PROFILER_OBJ) $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^

$(OBJS) : CXXFLAGS += -finstrument-functions

clean:
	rm -rf $(OBJS) $(PROFILER_OBJ) *.exe