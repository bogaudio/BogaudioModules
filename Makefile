
ifdef EXPERIMENTAL
FLAGS += -DEXPERIMENTAL=1
endif

ifdef TEST
FLAGS += -DTEST=1
endif

ifndef NO_RACK_SIMD
FLAGS += -DRACK_SIMD=1
endif

DSP_SOURCES = $(wildcard src/dsp/*cpp src/dsp/filters/*cpp)
SOURCES = $(wildcard src/*.cpp) $(DSP_SOURCES)

DISTRIBUTABLES += $(wildcard LICENSE*) res

RACK_DIR ?= ../..
include $(RACK_DIR)/plugin.mk

CXXFLAGS += -Isrc -Isrc/dsp -Ilib

BENCHMARK_SOURCES = $(wildcard benchmarks/*.cpp) $(DSP_SOURCES)
BENCHMARK_OBJECTS = $(patsubst %, build/%.o, $(BENCHMARK_SOURCES))
BENCHMARK_DEPS = $(patsubst %, build/%.d, $(BENCHMARK_SOURCES))
-include $(BENCHMARK_DEPS)
benchmark: $(BENCHMARK_OBJECTS)
	$(CXX) -o $@ $^ -lbenchmark -lpthread
benchmark_clean:
	rm -f benchmark $(BENCHMARK_OBJECTS)

TESTMAIN_SOURCES = test/testmain.cpp $(DSP_SOURCES)
TESTMAIN_OBJECTS = $(patsubst %, build/%.o, $(TESTMAIN_SOURCES))
TESTMAIN_DEPS = $(patsubst %, build/%.d, $(TESTMAIN_SOURCES))
-include $(TESTMAIN_DEPS)
testmain: $(TESTMAIN_OBJECTS)
	# $(CXX) -o $@ $^ ../../build/src/util.cpp.o
	$(CXX) -o $@ $^
testmain_clean:
	rm -f testmain $(TESTMAIN_OBJECTS)

PLOT_SOURCES = test/plot.cpp $(DSP_SOURCES)
PLOT_OBJECTS = $(patsubst %, build/%.o, $(PLOT_SOURCES))
PLOT_DEPS = $(patsubst %, build/%.d, $(PLOT_SOURCES))
-include $(PLOT_DEPS)
plot: $(PLOT_OBJECTS)
	$(CXX) -o $@ $^
plotrun: plot
	./plot
plotrungp: plot
	./plot > plot.tmp && gnuplot -p test/plot.gp
plot_clean:
	rm -f plot plot.tmp $(PLOT_OBJECTS)

SCATTER_SOURCES = test/scatter.cpp $(DSP_SOURCES)
SCATTER_OBJECTS = $(patsubst %, build/%.o, $(SCATTER_SOURCES))
SCATTER_DEPS = $(patsubst %, build/%.d, $(SCATTER_SOURCES))
-include $(SCATTER_DEPS)
scatter: $(SCATTER_OBJECTS)
	$(CXX) -o $@ $^
scatterrun: scatter
	./scatter
scatterrungp: scatter
	./scatter > scatter.tmp && gnuplot -p test/scatter.gp
scatter_clean:
	rm -f scatter scatter.tmp $(SCATTER_OBJECTS)

clean: benchmark_clean testmain_clean plot_clean scatter_clean
