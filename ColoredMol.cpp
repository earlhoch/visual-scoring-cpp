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
    OBConversion conv;
    conv.SetInFormat("PDB");
    conv.ReadFile(&ligMol, ligName);
    conv.ReadFile(&recMol, recName);
    int x [] = {2, 24, 25};

    std::cout << "Number of lig atoms: " << ligMol.NumAtoms() << '\n';
    std::cout << "Number of rec atoms: " << recMol.NumAtoms() << '\n';

    addHydrogens();
    removeAndScore(x);
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
    std::stringstream ss;
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
                            //std::cout << *i << "|" << ia[j] << '\n';
                            if(*i == ia[j]) //number in removal list
                            {
                                hit = true;
                                break;
                            }
                        }

                        if (!(hit))
                        {
                            outNums.push_back(*i);
                        }
                }

                outNums.sort();
                outNums.unique();

                if (outNums.size() > 0) //don't print line if no atoms to connect
                {

                    ss << "CONECT";
                    ss.width(5);
                    ss << std::right << firstNum;
                    for ( auto i = outNums.begin(); i != outNums.end(); ++i)
                    {
                        ss.width(5);
                        ss << std::right << *i;
                    }
                    ss << '\n';
                }

                outNums.clear();

            }
        }
        if(line.find("HETATM") < std::string::npos)
        {
            bool keepLine = true;
            std::string indexString = line.substr(6,5);
            int index = std::stoi(indexString);

            for( int i = 0; i != sizeof(&ia); ++i)
            {
                if (ia[i] == index)
                {
                    keepLine = false;
                    break;
                }
            }
            if ( keepLine )
            {
                ss << line << '\n';
            }

        }
    }
    std::cout << "Score: " << score() << '\n';

    OBConversion conv;
    OBMol temp;
    conv.SetInFormat("PDB");
    conv.ReadString(&temp, ss.str());
    std::cout << "temp atoms: " << temp.NumAtoms() << '\n';
}
void ColoredMol::addHydrogens()
{
    std::cout << "adding hydrogens\n";
    recMol.AddHydrogens();
    ligMol.AddHydrogens();

    OBConversion conv;
    conv.SetOutFormat("PDB");

    hRec = conv.WriteString(&recMol);
    hLig = conv.WriteString(&ligMol);
}

float ColoredMol::score(){return 1.11;}
void ColoredMol::writeScores(){}
bool ColoredMol::inRange(){}

void ColoredMol::ligCenter()
{
    vector3 cen = ligMol.Center(0);
    cenCoords[0] = cen.GetX();
    cenCoords[1] = cen.GetY();
    cenCoords[2] = cen.GetZ();
}

int ColoredMol::transform(){}
void ColoredMol::removeResidues(){}
