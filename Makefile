CC=g++
CFLAGS=-Wall 
LDFLAGS=-L/usr/local/lib
OPT_LDFLAGS=`Magick++-config --cppflags --cxxflags --ldflags --libs`
LINKS=-lgsl -lgslcblas -lm
OPT_LINKS=`Magick++-config --ldflags --libs`
SOURCES =IsingModel.cc
OPT_SOURCES=imageProcessing.cc
HEADERS =IsingMatrix.h IsingCell.h parser.h simulator.h
EXECUTABLE =IsingModel

complete: $(SOURCES) $(OPT_SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OPT_LDFLAGS) $(SOURCES) $(OPT_SOURCES) -o $(EXECUTABLE)-$@ $(LINKS) $(OPT_LINKS)
	
concise: $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(SOURCES) -o $(EXECUTABLE)-$@ $(LINKS)