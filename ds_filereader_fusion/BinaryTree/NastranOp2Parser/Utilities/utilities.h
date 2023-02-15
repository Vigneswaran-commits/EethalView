#ifndef UTILITIES_H
#define UTILITIES_H
#include <iostream>
#include <vector>
#include <string>
class Utilities
{
public:
    /** Default constructor */
    Utilities();
    static void swapEndiannes4(int *iBlock);
    static void swapEndiannes8(int *iBlock);
    static void swapEndiannes(int &iBlock);
    static void swapEndiannes(int nbr,int *iBlock);
    static void Op2Word_to_String(const int *iptrOp2Words,int iNumberofWords,std::string &strName,bool bcancelBlanks);
    static float Op2Word_to_Float(const int *iblock);
    static int Op2Word_to_Int(const int *iblock);
    static void ReadNastranWords(std::ifstream &FileStream,int NbrofWords,std::vector<int> &ivecBlock,bool bSwapEndiannes);
    /** Default destructor */
    ~Utilities();
protected:
private:
};

#endif // UTILITIES_H
