#include <iostream>
#include "ColoredMol.h"

int main()
{
    int x [5] = {1, 2, 3, 4, 5};
    std::string ligName = "3gvu_lig.pdb";
    std::string recName = "3gvu_rec.pdb";
    std::string model = "model";
    std::string weights = "weights";
    float size = 23.5;
    std::string outRec = "colored_3gvu_rec.pdb";
    std::string outLig = "colored_3gvu_lig.pdb";
    bool no_frag = false;
    bool verbose = true;

    ColoredMol cMol = ColoredMol(ligName, recName, model, weights, size, outRec, outLig, no_frag, verbose);
    cMol.color();
    //cMol.print();

    return 0;
}
