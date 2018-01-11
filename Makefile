
ifdef REQUIRE_VERSION
FLAGS += -DREQUIRE_VERSION=$(REQUIRE_VERSION)
endif

ifdef EXPERIMENTAL
FLAGS += -DEXPERIMENTAL=1
endif

SOURCES = $(wildcard src/*.cpp src/dsp/*cpp)
CXXFLAGS += -Isrc -Isrc/dsp

include ../../plugin.mk

BENCHMARK_SOURCES = $(wildcard benchmarks/*.cpp src/dsp/*cpp)
BENCHMARK_OBJECTS = $(patsubst %, build/%.o, $(BENCHMARK_SOURCES))
BENCHMARK_DEPS = $(patsubst %, build/%.d, $(BENCHMARK_SOURCES))
-include $(BENCHMARK_DEPS)
benchmark: $(BENCHMARK_OBJECTS)
	$(CXX) -o $@ $^ ../../build/src/util.cpp.o -lbenchmark -lpthread
benchmark_clean:
	rm -f benchmark $(BENCHMARK_OBJECTS)
clean: benchmark_clean

distprep:
	rm -f build/src/bogaudio.cpp*

SLUG=Bogaudio
dist: distprep
	@$(MAKE) all REQUIRE_VERSION=1
	mkdir -p dist/$(SLUG)
	cp README* dist/$(SLUG)/
	cp LICENSE* dist/$(SLUG)/
	cp plugin.* dist/$(SLUG)/
	cp -R res dist/$(SLUG)/
	(cd dist && zip -r $(SLUG)-$(VERSION)-$(ARCH).zip $(SLUG))
