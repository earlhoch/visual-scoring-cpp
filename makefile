INC := -I /usr/include/openbabel-2.0
LIBS := -l openbabel

all:
	g++ test.cpp ColoredMol.cpp -o test $(INC) $(LIBS)
