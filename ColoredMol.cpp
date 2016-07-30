#include <iostream>
#include <openbabel/obconversion.h>
#include <openbabel/mol.h>
#include "ColoredMol.h"

using namespace OpenBabel;

ColoredMol::ColoredMol (std::string inLigName, std::string inRecName, std::string inModel, std::string inWeights, float inSize, std::string inOutRec, std::string inOutLig, bool inNo_frag, bool inVerbose)
    {
        std::cout << "constructor called" << '\n';
        ligName = inLigName;
        recName = inRecName;
        model = inModel;
        weights = inWeights;
        size = inSize;
        outRec = inOutRec;
        outLig = inOutLig;
        no_frag = inNo_frag;
        verbose = inVerbose;
    }

void ColoredMol::color()
{
    std::cout << "color called" << '\n';
}

void ColoredMol::print()
{
    std::cout << "ligName: " << ligName << '\n';
    std::cout << "recName: " << recName << '\n';
    std::cout << "model: " << model << '\n';
    std::cout << "size: " << size << '\n';
    std::cout << "outRec: " << outRec << '\n';
    std::cout << "outLig: " << outLig << '\n';
    std::cout << "no_frag: " << no_frag << '\n';
    std::cout << "verbose: " << verbose << '\n';
}


void ColoredMol::removeAndScore(int ia[])
{
    std::cout << ia[0];
    std::ifstream file;
    file.open("3gvu_lig.pdb", std::ios::in);

    std::string line;
    while(std::getline(file, line))
    {
        std::vector<int> conNums;
        if(line.find("CONECT") < std::string::npos)
        {
            std::string newLine = line.substr(11,std::string::npos);
            std::string first = line.substr(6,5);
            int firstNum = std::stoi(first);

            std::istringstream iss(newLine);
            int d;

            while (iss >> d)
            {
                conNums.push_back(d);
            }

            bool valid = true;

            for(auto j = 0; j != sizeof(&ia); ++j) //list to remove
            {
                if (firstNum == ia[j]) //first CONECT record is in list, remove whole line
                {
                    valid = false;
                    break;
                }
            }

            if(valid)
            {
                std::list<int> outNums;
                bool hit;
                for(auto i = conNums.begin(); i != conNums.end() ; ++i) //list of nums in line
                {
                        hit = false;
                        for(auto j = 0; j != sizeof(&ia); ++j) //list to remove
                        {
                            std::cout << *i << "|" << ia[j] << '\n';
                            if(*i == ia[j]) //number in removal list
                            {
                                hit = true;
                            }
                        }

                        if (!(hit))
                        {
                            outNums.push_back(*i);
                        }
                }

                std::cout << hit << '\n';
                std::cout << line.substr(0,11) << " ";
                outNums.sort();
                outNums.unique();
                for(auto k = outNums.begin(); k != outNums.end() ; ++k)
                {
                    std::cout << "|" << *k << "|";
                }
                std::cout << '\n';
            }
        }
    }
}
float ColoredMol::score(){}
void ColoredMol::writeScores(){}
bool ColoredMol::inRange(){}
int ColoredMol::transform(){}
void ColoredMol::removeResidues(){}

