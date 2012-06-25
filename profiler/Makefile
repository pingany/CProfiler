# Makefile

# CXXFLAGS += -DNO_SYMBOL
# CXXFLAGS += -DNO_HASH_MAP

DEBUG ?= YES

TARGET ?= program_with_profiler

ifeq ($(DEBUG),YES)
CXXFLAGS += -DDEBUG_PROFILER 
CXXFLAGS += -g 
LDFLAGS += -g
endif

PROFILER_OBJS = $(PROFILER_DIR)/profiler.o $(PROFILER_DIR)/CommonAddress2Symbol.o

$(OBJS) : CXXFLAGS += -finstrument-functions

$(TARGET): $(PROFILER_OBJS) $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^

clean:
	rm -rf $(OBJS) $(PROFILER_OBJS) $(TARGET)