INC := -I /usr/local/include/openbabel-2.0
LIBS := -l openbabel -l boost_system

all:
	g++ test.cpp ColoredMol.cpp -o test $(INC) $(LIBS) -std=c++11
