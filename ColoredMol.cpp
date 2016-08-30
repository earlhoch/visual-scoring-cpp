#include <iostream>
#include <iomanip>
#include <set>
#include <openbabel/obconversion.h>
#include <openbabel/obiter.h>
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
    std::cout << "color called" << '\n'; std::vector<float> test = {-58.3, 1.23, 2.34, 3.45};
    std::set<int> test3 = {400, 1000};
    transform(test);
    OBConversion conv;
    conv.SetInFormat("PDB");
    conv.ReadFile(&ligMol, ligName);
    conv.ReadFile(&recMol, recName);
    int x [] = {2, 24, 25};


    std::cout << "Number of lig atoms: " << ligMol.NumAtoms() << '\n';
    std::cout << "Number of rec atoms: " << recMol.NumAtoms() << '\n';

    addHydrogens();
    ligCenter();
    std::cout << "inRange: " << inRange(test3) << '\n';

    std::vector<float> writeTest (hRecMol.NumAtoms(), 0.00);
    writeTest[2] = 3.45;
    writeTest[488] = 818.93;
    writeTest[3] = 0.0323343;
    writeTest[4] = 999999999;
    writeTest[5] = 48.45555555;
    writeScores(writeTest, true);
    
    /*
    std::cout << "before\n";
    removeResidues();
    std::cout << "after\n";
    */

    //float garbage = removeAndScore(test3, true);

    //removeResidues();
    removeEachAtom();
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

float ColoredMol::removeAndScore(std::vector<bool> removeList, bool isRec)
{
    std::string molString;
    OBMol mol;
    if(isRec)
    {
        molString = hRec;
        mol = hRecMol;
    }
    else
    {
        molString = hLig;
        mol = hLigMol;
    }

    if(!(isRec)) //if ligand
    {
        OBAtom* atom;
        for(int i = 0;i < removeList.size(); ++i)
        {
            if (removeList[i]) //index is in removeList
            {
            atom = mol.GetAtom(i);
            FOR_NBORS_OF_ATOM(neighbor, atom)
            {
                if(neighbor->GetAtomicNum() == 1)
                {
                    std::cout << "adding: " << neighbor->GetIdx() << '\n';
                    removeList[neighbor->GetIdx()] = true;
                }
            }
            }
        }
    }
    else //if receptor
    {
        //make set for inRange test
        std::set<int> removeSet;
        for (int i = 0; i < removeList.size(); ++i)
        {
            if (removeList[i])
            {
                removeSet.insert(i);
            }
        }

       if (!(inRange(removeSet)))
        {
            return 0.00;
        }
    }





    std::stringstream ss;
    std::stringstream molStream(molString);

    std::string line;
    while(std::getline(molStream, line))
    {
        std::vector<int> conNums;
        if(line.find("CONECT") < std::string::npos)
        {
            std::string newLine = line.substr(11,std::string::npos);
            std::string first = line.substr(6,5);
            int firstNum = std::stoi(first);

            std::istringstream iss(newLine);
            int d;

            while (iss >> d) // add all CONECT fields to list
            {
                conNums.push_back(d);
            }

            bool valid;

            if (removeList[firstNum] == true) //first CONECT record is in list, remove whole line
            {
                valid = false;
            }
            else
            {
                valid = true;
            }
           

            if(valid)
            {
                std::set<int> outNums;
                bool hit;
                for(auto i = conNums.begin(); i != conNums.end() ; ++i) //list of nums in line
                {
                        if(removeList[*i] == false) //number in removal list
                        {
                            outNums.insert(*i);
                        }
                }

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
        else if((line.find("HETATM") < std::string::npos) ||
            (line.find("ATOM") < std::string::npos))
        {
            bool keepLine = true;
            std::string firstNumString = line.substr(6,5);
            int firstNum = std::stoi(firstNumString);

            if (!(removeList[firstNum])) //remove line if in list
            {
                ss << line << '\n';
            }


        }
    }
    float scoreVal = score();

    std::cout << ss.str();

    static int count = 0;
    std::ofstream pdbOut;
    pdbOut.open("out" + std::to_string(count) + ".pdb");
    pdbOut << ss.str();
    pdbOut.close();
    count++;
    return scoreVal;
}
void ColoredMol::addHydrogens()
{
    std::cout << "adding hydrogens\n";
    recMol.AddHydrogens();
    ligMol.AddHydrogens();

    OBConversion conv;
    conv.SetOutFormat("PDB");
    conv.SetInFormat("PDB");

    hRec = conv.WriteString(&recMol);
    hLig = conv.WriteString(&ligMol);

    conv.ReadString(&hRecMol, hRec);
    conv.ReadString(&hLigMol, hLig);

    std::cout << "finished adding hydrogens\n";
}

float ColoredMol::score(){return (float)1.11;}
void ColoredMol::writeScores(std::vector<float> scoreList, bool isRec)
{
    std::string filename;
    std::string molString;
    if(isRec)
    {
        filename = outRec;
        molString = hRec;
    }
    else
    {
        filename = outLig;
        molString = hLig;
    }

    std::ofstream outFile;
    outFile.open(filename);

    outFile << "CNN MODEL: " << model << '\n';
    outFile << "CNN WEIGHTS: " << model << '\n';

    std::stringstream molStream(molString);
    std::string line;
    std::string indexString;
    int index;
    std::stringstream scoreStream;
    std::string scoreString;
    while(std::getline(molStream, line))
    {
        if ((line.find("ATOM") < std::string::npos) || 
            (line.find("HETATM") < std::string::npos))
        {
            scoreStream.str(""); //clear stream for next score
            indexString = line.substr(6,5);
            index = std::stoi(indexString);

            if ((scoreList[index] > 0.001) || (scoreList[index] < -0.001)) //ignore very small scores
            {
                scoreStream << std::fixed << std::setprecision(5) << scoreList[index];
                outFile << line.substr(0,61);
                scoreString = scoreStream.str();
                scoreString.resize(5);
                outFile.width(5);
                outFile.fill('.');
                outFile << std::right << scoreString;
                outFile << line.substr(66) << '\n';

            }
            else
            {
                outFile << line << '\n';
            }
        }
        else
        {
            outFile << line << '\n';
        }

    }
}


bool ColoredMol::inRange(std::set<int> atomList)
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
void ColoredMol::removeResidues()
{
    std::cout << "help";
    std::vector<float> scoreDict(hRecMol.NumAtoms() + 1, 0.00);
    std::string lastRes = "";
    std::string currRes;
    std::vector<bool> atomList (hRecMol.NumAtoms() + 1, false);
    std::set<std::string> resList;


    std::cout << '\n' << hLig.back() << '\n';

    std::string molString = hRec;
    std::stringstream molStream(molString);
    std::string line;
    while(std::getline(molStream, line))
    {
        if((line.find("ATOM") < std::string::npos) ||
           (line.find("HETATM") < std::string::npos))
        {
            currRes = line.substr(23,4);
            std::cout << currRes << '\n';
            if(line.substr(23,4) != lastRes)
            {
                resList.insert(currRes);
                std::cout << "Adding ^\n";
                lastRes = currRes;
            }
        }
    }

    for( auto i = resList.begin(); i != resList.end(); ++i)
    {
        molStream.clear();
        molStream.str(molString);
        std::cout << "RES: " << *i << '\n';
        while(std::getline(molStream, line))
        {
            if((line.find("ATOM") < std::string::npos) ||
               (line.find("HETATM") < std::string::npos))
            {
                if(line.substr(23,4) == *i)
                {
                    std::string indexString = line.substr(6,5);
                    int index = std::stoi(indexString);
                    //std::cout << index << '\n';
                    atomList[index] = true;
                }
            }
        }
        float scoreVal = removeAndScore(atomList, true);
        std::cout << "res score: " << scoreVal << '\n';

        std::cout << atomList.size() << '\n';
        
        
        for ( auto f : atomList)
        {
            if(f)
            {
            scoreDict[f] = scoreVal;
            }
        }
        

        atomList.clear();

        

    }

    writeScores(scoreDict, true);
}

void ColoredMol::removeEachAtom()
{
    std::vector<float> scoreDict(hLigMol.NumAtoms());
    std::stringstream ss (hLig);
    std::string line;

    std::string indexString;
    int index;
    std::vector<bool> removeList (hLigMol.NumAtoms() + 1);
    float scoreVal;

    while(std::getline(ss, line))
    {
        if ((line.find("ATOM") < std::string::npos) ||
            (line.find("HETATM") < std::string::npos))
        {
            indexString = line.substr(6, 5);
            index = std::stoi(indexString);
            if (hLigMol.GetAtom(index)->GetAtomicNum() != 1)
            {
                removeList[index] = true;

                scoreVal = removeAndScore(removeList, false);
                removeList[index] = false;

                scoreDict[index] = scoreVal;
            }
        }

    }


}



