CC=g++
CFLAGS=-Wall
OPT_CFLAGS=-D imageSuite
LDFLAGS=-L/usr/local/lib
OPT_LDFLAGS=`Magick++-config --cppflags --cxxflags --ldflags --libs`
LINKS=-lgsl -lgslcblas -lm
OPT_LINKS=`Magick++-config --ldflags --libs`
SOURCES =IsingModel.cpp IsingMatrix.hpp IsingCell.hpp parser.hpp simulator.hpp extra.hpp
OPT_SOURCES=imageProcessing.hpp
EXECUTABLE =IsingModel

complete: $(SOURCES) $(OPT_SOURCES)
	$(CC) $(CFLAGS) $(OPT_CFLAGS) $(LDFLAGS) $(OPT_LDFLAGS) $(SOURCES) $(OPT_SOURCES) -o $(EXECUTABLE)-$@ $(LINKS) $(OPT_LINKS)
	
core: $(SOURCES)
	$(CC) $(CFLAGS) $(LDFLAGS) $(SOURCES) -o $(EXECUTABLE)-$@ $(LINKS)