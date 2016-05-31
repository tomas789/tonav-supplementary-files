# This makefile supports these operations:
# make release
# make debug
# make clean

# CONFIGURATION #

CXX = g++
# Common flags
CXXFLAGS = -std=c++11 -Wall -Wextra -pedantic -pthread -Wno-missing-field-initializers
# Debug mode flags
CXXDEBUGFLAGS = -O0 -ggdb3 -D_GLIBCXX_DEBUG -DDEBUG
# Release mode flags
CXXRELEASEFLAGS = -O3 -DNDEBUG -pipe -fomit-frame-pointer

VPATH= . filters
OBJECTS = example.o
HEADERS = $(wildcard $(addsuffix /*.hpp, $(VPATH)))
EXECUTABLE = example

# IDE COMPATIBILITY #

all: release

# TARGETS #

RELEASEOBJECTS = $(addprefix release/, $(OBJECTS))
RELEASEEXECUTABLE = $(addprefix release/, $(EXECUTABLE))
DEBUGOBJECTS = $(addprefix debug/, $(OBJECTS))
DEBUGEXECUTABLE = $(addprefix debug/, $(EXECUTABLE))

release: CXXFLAGS += $(CXXRELEASEFLAGS)
release: $(RELEASEEXECUTABLE)

debug: CXXFLAGS += $(CXXDEBUGFLAGS)
debug: $(DEBUGEXECUTABLE)

$(RELEASEEXECUTABLE): $(RELEASEOBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(DEBUGEXECUTABLE): $(DEBUGOBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

debug/%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

release/%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -rvf $(RELEASEOBJECTS) $(RELEASEEXECUTABLE)
	rm -rvf $(DEBUGOBJECTS) $(DEBUGEXECUTABLE)

archives:
	rm -rvf _TrackMe_ARCHIVE_ TrackMe.tar.gz TrackMe.zip
	mkdir _TrackMe_ARCHIVE_
	cp -rv filters *.hpp *.cpp *.dox doc Doxyfile Makefile \
	       _TrackMe_ARCHIVE_
	tar -cvzf TrackMe.tar.gz _TrackMe_ARCHIVE_
	zip -r TrackMe.zip _TrackMe_ARCHIVE_
	rm -rv _TrackMe_ARCHIVE_
