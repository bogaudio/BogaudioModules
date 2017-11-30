
SOURCES = $(wildcard src/*.cpp)

include ../../plugin.mk

CXXFLAGS += -Isrc/dsp

dist: all
	mkdir -p dist/BogaudioModules
	cp LICENSE* dist/BogaudioModules/
	cp plugin.* dist/BogaudioModules/
	cp -R res dist/BogaudioModules/
	rm dist/BogaudioModules/res/*-src.svg
