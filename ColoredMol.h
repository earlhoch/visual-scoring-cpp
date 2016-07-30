#include <openbabel/mol.h>

class ColoredMol
{
    public:
    ColoredMol(std::string inLigName, std::string inRecName, std::string inModel, std::string inWeights, float inSize, std::string inOutRec = "", std::string inOutLig = "", bool inNo_frag = false, bool inVerbose = false);
    void color();
    void print();

    private:
    std::string ligName, recName, hLig, hRec, model, weights, outRec, outLig; 
    OpenBabel::OBMol ligMol, recMol;
    float size;
    bool no_frag, verbose;

    void addHydrogens();
    void removeAndScore(int ia[]);
    float score();
    void writeScores();
    bool inRange();
    int transform();
    void removeResidues();
};
