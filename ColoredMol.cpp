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
    std::vector<float> test = {-58.3, 1.23, 2.34, 3.45};
    std::list<int> test2 = {400, 1000};
    transform(test);
    std::cout << "color called" << '\n';
    OBConversion conv;
    conv.SetInFormat("PDB");
    conv.ReadFile(&ligMol, ligName);
    conv.ReadFile(&recMol, recName);
    int x [] = {2, 24, 25};

    std::cout << "Number of lig atoms: " << ligMol.NumAtoms() << '\n';
    std::cout << "Number of rec atoms: " << recMol.NumAtoms() << '\n';

    addHydrogens();
    ligCenter();
    std::cout << "inRange: " << inRange(test2) << '\n';
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
bool ColoredMol::inRange(std::list<int> atomList)
{
    float x = cenCoords[0];
    float y = cenCoords[1];
    float z = cenCoords[2];

    float allowedDist = size / 2;
    int numAtoms = recMol.NumAtoms();

    OBAtom* atom;
    for( auto i = atomList.begin(); i != atomList.end(); ++i)
    {
        if (*i >= numAtoms)
        {
            return false;
        }

        atom = recMol.GetAtom(*i);
        if(atom->GetX() < x + allowedDist)
            if (atom->GetY() < y + allowedDist)
                if (atom->GetZ() < z + allowedDist)
                    if (atom->GetX() > x - allowedDist)
                        if (atom->GetY() > y - allowedDist)
                            if (atom->GetZ() > z - allowedDist)
                                return true;
    }

        return false;
}



void ColoredMol::ligCenter()
{
    vector3 cen = ligMol.Center(0);
    cenCoords[0] = cen.GetX();
    cenCoords[1] = cen.GetY();
    cenCoords[2] = cen.GetZ();
    std::cout << cenCoords[0] << "|" << cenCoords[1] << "|" << cenCoords[2] << '\n';
}

std::vector<float> ColoredMol::transform(std::vector<float> inList)
{
    std::vector<float> outList (inList.size());
    float tempVal;
    for (int i = 0; i < inList.size(); ++i)
    {
        tempVal = inList[i];
        if(tempVal < 0)
        {
            tempVal = 0 - std::sqrt(std::abs(tempVal));
        }
        else
        {
            tempVal = std::sqrt(tempVal);
        }

        tempVal = tempVal * 100;
        std::cout << inList[i] << " : " << tempVal << '\n';

        outList[i] = tempVal;
    }

    for (int i = 0; i < outList.size(); ++i)
    {
        std::cout << outList[i] << '\n';
    }

    return outList;
}
void ColoredMol::removeResidues(){}
