#ifndef CNastranOp2Element_H
#define CNastranOp2Element_H
#include "cnode.h"
#include "celement.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

class CNastranOp2Element
{
public:
    struct SElementType
    {

        static bool isNastranElement(int type) ;

        // Nastran supported element types :

        static const int NONE =  -1 ;

        static const int NAS_TP_MIN =   1 ;
        static const int NAS_TP_MAX = 199 ;

        static const int CBAR    =   24 ;
        static const int CBEAM   =   54 ;
        static const int CBEND   =   46 ;
        static const int CBUSH1D =   56 ;
        static const int CBUSH   =   26 ;
        static const int CDAMP1  =    2 ;
        static const int CDAMP2  =    3 ;
        static const int CDAMP3  =    4 ;
        static const int CDAMP4  =    5 ;
        static const int CDAMP5  =  106 ;
        static const int CELAS1  =    6 ;
        static const int CELAS2  =    7 ;
        static const int CELAS3  =    8 ;
        static const int CELAS4  =    9 ;
        static const int CFAST   =   98 ;
        static const int CFASTP  =   93 ; // same as CFAST
        static const int CGAP    =   19 ;
        static const int CHEXA   =   73 ;
        static const int CMASS1  =   10 ;
        static const int CMASS2  =   11 ;
        static const int CMASS3  =   12 ;
        static const int CMASS4  =   13 ;
        static const int CONM1   =   14 ;
        static const int CONM2   =   15 ;
        static const int CONROD  =   16 ;
        static const int CPENTA  =   41 ;
        static const int CQUAD   =   91 ;
        static const int CQUAD4  =   51 ;
        static const int CQUAD8  =   47 ;
        static const int CQUADR  =   80 ;
        static const int CROD    =   30 ;
        static const int CSHEAR  =   31 ;
        static const int CTETRA  =   55 ;
        static const int CTRIA3  =   59 ;
        static const int CTRIA6  =   48 ;
        static const int CTRIAR  =   92 ;
        static const int CTUBE   =   37 ;
        static const int CVISC   =   39 ;
        static const int PLOTEL  =   52 ;
        static const int SPOINT  =   49 ;

        static const int UNKNOWN = -999 ;

        static std::string GetNameFromId(int iTypeId);
    } ;
    CNastranOp2Element();
    static unsigned int GetNbrofWordsFromTypeId(int typeId);
    static void ScanOp2Record(std::string hName,int nbr,std::vector<int> &iBlock,CElement& oElementData);
    ~CNastranOp2Element();
protected:
private:
};

#endif // CNastranOp2Element_H
