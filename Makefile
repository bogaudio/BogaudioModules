
ifdef REQUIRE_VERSION
FLAGS += -DREQUIRE_VERSION=$(REQUIRE_VERSION)
endif

SOURCES = $(wildcard src/*.cpp src/dsp/*cpp)
CXXFLAGS += -Isrc/dsp

include ../../plugin.mk

BENCHMARK_SOURCES = $(wildcard benchmarks/*.cpp src/dsp/*cpp)
BENCHMARK_OBJECTS = $(patsubst %, build/%.o, $(BENCHMARK_SOURCES))
BENCHMARK_DEPS = $(patsubst %, build/%.d, $(BENCHMARK_SOURCES))
-include $(BENCHMARK_DEPS)
benchmark: $(BENCHMARK_OBJECTS)
	$(CXX) -o $@ $^ ../../build/src/util.cpp.o -lbenchmark
benchmark_clean:
	rm -f benchmark
clean: benchmark_clean

distprep:
	rm -f build/src/BogaudioModules.cpp*

SLUG=Bogaudio
dist: distprep
	@$(MAKE) all REQUIRE_VERSION=1
	mkdir -p dist/$(SLUG)
	cp README* dist/$(SLUG)/
	cp LICENSE* dist/$(SLUG)/
	cp plugin.* dist/$(SLUG)/
	cp -R res dist/$(SLUG)/
	rm dist/$(SLUG)/res/*-src.svg
	(cd dist && zip -r $(SLUG)-$(VERSION)-$(ARCH).zip $(SLUG))
