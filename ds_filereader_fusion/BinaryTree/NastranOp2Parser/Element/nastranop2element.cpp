#include "NastranOp2Parser/Element/NastranOp2Element.h"
#include "NastranOp2Parser/Utilities/utilities.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
using namespace std;

CNastranOp2Element::CNastranOp2Element()
{
    //ctor
}

unsigned int CNastranOp2Element::GetNbrofWordsFromTypeId(int typeId)
{
    try
    {
        int nbrWords;

        switch (typeId)//Values are assigned in postlib::nastran::element:elemtype
        {
        case CNastranOp2Element::SElementType::CBAR :
            nbrWords=16;
            break;
        case CNastranOp2Element::SElementType::CBEAM :
            nbrWords=18;
            break;
        case CNastranOp2Element::SElementType::CBEND :
            nbrWords=13;
            break;
        case CNastranOp2Element::SElementType::CBUSH :
            nbrWords=14;
            break;
        case CNastranOp2Element::SElementType::CBUSH1D :
            nbrWords=8;
            break;
        case CNastranOp2Element::SElementType::CDAMP1 :
            nbrWords=6;
            break;
        case CNastranOp2Element::SElementType::CDAMP2 :
            nbrWords=6;
            break;
        case CNastranOp2Element::SElementType::CDAMP3 :
            nbrWords=4;
            break;
        case CNastranOp2Element::SElementType::CDAMP4 :
            nbrWords=4;
            break;
        case CNastranOp2Element::SElementType::CDAMP5 :
            nbrWords=4;
            break;
        case CNastranOp2Element::SElementType::CELAS1 :
            nbrWords=6;
            break;
        case CNastranOp2Element::SElementType::CELAS2 :
            nbrWords=8;
            break;
        case CNastranOp2Element::SElementType::CELAS3 :
            nbrWords=4;
            break;
        case CNastranOp2Element::SElementType::CELAS4 :
            nbrWords=4;
            break;
        case CNastranOp2Element::SElementType::CFAST :
            nbrWords=28;
            break;
        case CNastranOp2Element::SElementType::CFASTP :
            nbrWords=82;
            break;
        case CNastranOp2Element::SElementType::CGAP :
            nbrWords=9;
            break;
        case CNastranOp2Element::SElementType::CHEXA :
            nbrWords=22;
            break;
        case CNastranOp2Element::SElementType::CMASS1 :
            nbrWords=6;
            break;
        case CNastranOp2Element::SElementType::CMASS2 :
            nbrWords=6;
            break;
        case CNastranOp2Element::SElementType::CMASS3 :
            nbrWords=4;
            break;
        case CNastranOp2Element::SElementType::CMASS4 :
            nbrWords=4;
            break;
        case CNastranOp2Element::SElementType::CONM1 :
            nbrWords=24;
            break;
        case CNastranOp2Element::SElementType::CONM2 :
            nbrWords=13;
            break;
        case CNastranOp2Element::SElementType::CONROD :
            nbrWords=8;
            break;
        case CNastranOp2Element::SElementType::CPENTA :
            nbrWords=17;
            break;
        case CNastranOp2Element::SElementType::CQUAD :
            nbrWords=11;
            break;
        case CNastranOp2Element::SElementType::CQUAD4 :
            nbrWords=14;
            break;
        case CNastranOp2Element::SElementType::CQUAD8 :
            nbrWords=17;
            break;
        case CNastranOp2Element::SElementType::CQUADR :
            nbrWords=14;
            break;
        case CNastranOp2Element::SElementType::CROD :
            nbrWords=4;
            break;
        case CNastranOp2Element::SElementType::CSHEAR :
            nbrWords=6;
            break;
        case CNastranOp2Element::SElementType::CTETRA :
            nbrWords=12;
            break;
        case CNastranOp2Element::SElementType::CTRIA3 :
            nbrWords=13;
            break;
        case CNastranOp2Element::SElementType::CTRIA6 :
            nbrWords=14;
            break;
        case CNastranOp2Element::SElementType::CTRIAR :
            nbrWords=13;
            break;
        case CNastranOp2Element::SElementType::CTUBE :
            nbrWords=4;
            break;
        case CNastranOp2Element::SElementType::CVISC :
            nbrWords=4;
            break;
        case CNastranOp2Element::SElementType::PLOTEL :
            nbrWords=3;
            break;
        case CNastranOp2Element::SElementType::SPOINT :
            nbrWords=1;
            break;
        default :
            nbrWords=-1;
            break;
        } // switch (typeId) ...
        return nbrWords;
    }
    catch(...)
    {
        cout<<"CNastranOp2Element::GetNbrofWordsFromTypeId, Default exception thrown while getting number of words from type id\n";
    }
}

string CNastranOp2Element::SElementType::GetNameFromId(int iTypeId)
{
    try
    {
        switch (iTypeId)
        {
        case NONE :
            return "NONE";
            break;
        case CBAR :
            return "CBAR";
            break;
        case CBEAM :
            return "CBEAM";
            break;
        case CBEND :
            return "CBEND";
            break;
        case CBUSH :
            return "CBUSH";
            break;
        case CBUSH1D :
            return "CBUSH1D";
            break;
        case CDAMP1 :
            return "CDAMP1";
            break;
        case CDAMP2 :
            return "CDAMP2";
            break;
        case CDAMP3 :
            return "CDAMP3";
            break;
        case CDAMP4 :
            return "CDAMP4";
            break;
        case CDAMP5 :
            return "CDAMP5";
            break;
        case CELAS1 :
            return "CELAS1";
            break;
        case CELAS2 :
            return "CELAS2";
            break;
        case CELAS3 :
            return "CELAS3";
            break;
        case CELAS4 :
            return "CELAS4";
            break;
        case CFAST :
            return "CFAST";
            break;
        case CFASTP :
            return "CFASTP";
            break;
        case CGAP :
            return "CGAP";
            break;
        case CHEXA :
            return "CHEXA";
            break;
        case CMASS1 :
            return "CMASS1";
            break;
        case CMASS2 :
            return "CMASS2";
            break;
        case CMASS3 :
            return "CMASS3";
            break;
        case CMASS4 :
            return "CMASS4";
            break;
        case CONM2 :
            return "CONM2";
            break;
        case CONROD :
            return "CONROD";
            break;
        case CPENTA :
            return "CPENTA";
            break;
        case CQUAD :
            return "CQUAD";
            break;
        case CQUAD4 :
            return "CQUAD4";
            break;
        case CQUAD8 :
            return "CQUAD8";
            break;
        case CQUADR :
            return "CQUADR";
            break;
        case CROD :
            return "CROD";
            break;
        case CSHEAR :
            return "CSHEAR";
            break;
        case CTETRA :
            return "CTETRA";
            break;
        case CTRIA3 :
            return "CTRIA3";
            break;
        case CTRIA6 :
            return "CTRIA6";
            break;
        case CTRIAR :
            return "CTRIAR";
            break;
        case CTUBE :
            return "CTUBE";
            break;
        case CVISC :
            return "CVISC";
            break;
        case PLOTEL :
            return "PLOTEL";
            break;
        case SPOINT :
            return "SPOINT";
            break;
            //
            // Unknown element types :
            //
        default :
            return "UNKNOWN";
            break;
        }
    }
    catch(...)
    {
        cout<<"CNastranOp2Element::SElementType::GetNameFromId, Default exception thrown while getting element name from type id\n";
    }
}


CNastranOp2Element::~CNastranOp2Element()
{
    //dtor
}
