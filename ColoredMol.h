#include <openbabel/mol.h>
#include <set>

class ColoredMol
{
    public:
    ColoredMol(std::string inLigName, std::string inRecName, std::string inModel, std::string inWeights, float inSize, std::string inOutRec = "", std::string inOutLig = "", bool inNo_frag = false, bool inVerbose = false);
    void color();
    void print();

    private:
    std::string ligName, recName, hRec, hLig, model, weights, outRec, outLig; 
    OpenBabel::OBMol ligMol, recMol, hLigMol, hRecMol;
    float size;
    float cenCoords [3];
    bool no_frag, verbose;

    void addHydrogens();
    float removeAndScore(std::set<int> removeList, bool isRec);
    void ligCenter();
    float score();
    void writeScores(std::vector<float> scoreList, bool isRec);
    bool inRange(std::list<int> atomList);
    std::vector<float> transform(std::vector<float> inList);
    void removeResidues();
};
