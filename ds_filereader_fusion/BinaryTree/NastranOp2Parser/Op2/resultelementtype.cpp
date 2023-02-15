#include "NastranOp2Parser/op2/nastranop2.h"
#include <iomanip>
using namespace std ;

map<string,int> NastranOp2::SResultElementType::strimapElementNamesId ;
vector<bool> NastranOp2::SResultElementType::bvecEnabledCards(250,true) ;

string NastranOp2::SResultElementType::GetNameFromId(int iTypeId)
{
    try
    {
        switch (iTypeId)
        {
        case ROD :
            return "ROD" ;
            break;
        case BEAM :
            return "BEAM" ;
            break;
        case TUBE :
            return "TUBE" ;
            break;
        case SHEAR :
            return "SHEAR" ;
            break;
        case CONROD :
            return "CONROD" ;
            break;
        case ELAS1 :
            return "ELAS1" ;
            break;
        case ELAS2 :
            return "ELAS2" ;
            break;
        case ELAS3 :
            return "ELAS3" ;
            break;
        case ELAS4 :
            return "ELAS4" ;
            break;
        case DAMP1 :
            return "DAMP1" ;
            break;
        case DAMP2 :
            return "DAMP2" ;
            break;
        case DAMP3 :
            return "DAMP3" ;
            break;
        case DAMP4 :
            return "DAMP4" ;
            break;
        case VISC :
            return "VISC" ;
            break;
        case MASS1 :
            return "MASS1" ;
            break;
        case MASS2 :
            return "MASS2" ;
            break;
        case MASS3 :
            return "MASS3" ;
            break;
        case MASS4 :
            return "MASS4" ;
            break;
        case CONM1 :
            return "CONM1" ;
            break;
        case CONM2 :
            return "CONM2" ;
            break;
        case PLOTEL :
            return "PLOTEL" ;
            break;
        case QUAD4 :
            return "QUAD4" ;
            break;
        case BAR :
            return "BAR" ;
            break;
        case GAP :
            return "GAP" ;
            break;
        case TETRA :
            return "TETRA" ;
            break;
        case BUSH1D :
            return "BUSH1D" ;
            break;
        case QUAD8 :
            return "QUAD8" ;
            break;
        case HEXA :
            return "HEXA" ;
            break;
        case PENTA :
            return "PENTA" ;
            break;
        case BEND :
            return "BEND" ;
            break;
        case TRIAR :
            return "TRIAR" ;
            break;
        case TRIA3 :
            return "TRIA3" ;
            break;
        case TRIA6 :
            return "TRIA6" ;
            break;
        case QUADR :
            return "QUADR" ;
            break;
        case TETRANL :
            return "TETRANL" ;
            break;
        case GAPNL :
            return "GAPNL" ;
            break;
        case TUBENL :
            return "TUBENL" ;
            break;
        case TRIA3NL :
            return "TRIA3NL" ;
            break;
        case RODNL :
            return "RODNL" ;
            break;
        case QUAD4NL :
            return "QUAD4NL" ;
            break;
        case PENTANL :
            return "PENTANL" ;
            break;
        case CONRODNL :
            return "CONRODNL" ;
            break;
        case HEXANL :
            return "HEXANL" ;
            break;
        case BEAMNL :
            return "BEAMNL" ;
            break;
        case QUAD4LC :
            return "QUAD4LC" ;
            break;
        case QUAD8LC :
            return "QUAD8LC" ;
            break;
        case TRIA3LC :
            return "TRIA3LC" ;
            break;
        case TRIA6LC :
            return "TRIA6LC" ;
            break;
        case BUSH :
            return "BUSH" ;
            break;
        case QUAD144 :
            return "QUAD144" ;
            break;
        case QUADRNL :
            return "QUADRNL" ;
            break;
        case TRIARNL :
            return "TRIARNL" ;
            break;
        case ELAS1NL :
            return "ELAS1NL" ;
            break;
        case ELAS3NL :
            return "ELAS3NL" ;
            break;
        case BUSHNL :
            return "BUSHNL" ;
            break;
        case QUADRLC :
            return "QUADRLC" ;
            break;
        case TRIARLC :
            return "TRIARLC" ;
            break;
        default :
            return "";
            break;
        }
    }
    catch(...)
    {
        cout<<"NastranOp2::SResultElementType::GetNameFromId, default exception is thrown\n";
    }
}



int NastranOp2::SResultElementType::GetIdFromName(const string &iTypeName)
{
    try
    {
        static bool firstHere=true;
        if (firstHere)
        {
            strimapElementNamesId["ROD"]=ROD;
            strimapElementNamesId["BEAM"]=BEAM;
            strimapElementNamesId["TUBE"]=TUBE;
            strimapElementNamesId["SHEAR"]=SHEAR;
            strimapElementNamesId["CONROD"]=CONROD;
            strimapElementNamesId["ELAS1"]=ELAS1;
            strimapElementNamesId["ELAS2"]=ELAS2;
            strimapElementNamesId["ELAS3"]=ELAS3;
            strimapElementNamesId["ELAS4"]=ELAS4;
            strimapElementNamesId["DAMP1"]=DAMP1;
            strimapElementNamesId["DAMP2"]=DAMP2;
            strimapElementNamesId["DAMP3"]=DAMP3;
            strimapElementNamesId["DAMP4"]=DAMP4;
            strimapElementNamesId["VISC"]=VISC;
            strimapElementNamesId["MASS1"]=MASS1;
            strimapElementNamesId["MASS2"]=MASS2;
            strimapElementNamesId["MASS3"]=MASS3;
            strimapElementNamesId["MASS4"]=MASS4;
            strimapElementNamesId["CONM1"]=CONM1;
            strimapElementNamesId["CONM2"]=CONM2;
            strimapElementNamesId["PLOTEL"]=PLOTEL;
            strimapElementNamesId["QUAD4"]=QUAD4;
            strimapElementNamesId["BAR"]=BAR;
            strimapElementNamesId["GAP"]=GAP;
            strimapElementNamesId["TETRA"]=TETRA;
            strimapElementNamesId["BUSH1D"]=BUSH1D;
            strimapElementNamesId["QUAD8"]=QUAD8;
            strimapElementNamesId["HEXA"]=HEXA;
            strimapElementNamesId["PENTA"]=PENTA;
            strimapElementNamesId["BEND"]=BEND;
            strimapElementNamesId["TRIAR"]=TRIAR;
            strimapElementNamesId["TRIA3"]=TRIA3;
            strimapElementNamesId["TRIA6"]=TRIA6;
            strimapElementNamesId["QUADR"]=QUADR;
            strimapElementNamesId["TETRANL"]=TETRANL;
            strimapElementNamesId["GAPNL"]=GAPNL;
            strimapElementNamesId["TUBENL"]=TUBENL;
            strimapElementNamesId["TRIA3NL"]=TRIA3NL;
            strimapElementNamesId["RODNL"]=RODNL;
            strimapElementNamesId["QUAD4NL"]=QUAD4NL;
            strimapElementNamesId["PENTANL"]=PENTANL;
            strimapElementNamesId["CONRODNL"]=CONRODNL;
            strimapElementNamesId["HEXANL"]=HEXANL;
            strimapElementNamesId["BEAMNL"]=BEAMNL;
            strimapElementNamesId["QUAD4LC"]=QUAD4LC;
            strimapElementNamesId["QUAD8LC"]=QUAD8LC;
            strimapElementNamesId["TRIA3LC"]=TRIA3LC;
            strimapElementNamesId["TRIA6LC"]=TRIA6LC;
            strimapElementNamesId["BUSH"]=BUSH;
            strimapElementNamesId["QUAD144"]=QUAD144;
            strimapElementNamesId["QUADRNL"]=QUADRNL;
            strimapElementNamesId["TRIARNL"]=TRIARNL;
            strimapElementNamesId["ELAS1NL"]=ELAS1NL;
            strimapElementNamesId["ELAS3NL"]=ELAS3NL;
            strimapElementNamesId["BUSHNL"]=BUSHNL;
            strimapElementNamesId["QUADRLC"]=QUADRLC;
            strimapElementNamesId["TRIARLC"]=TRIARLC;
            firstHere=false;
        }

        static map<string,int>::iterator mSIit;
        mSIit=strimapElementNamesId.find(iTypeName);
        if (mSIit==strimapElementNamesId.end())
            return -1;
        else
            return mSIit->second;
    }
    catch(...)
    {
        cout<<"NastranOp2::SResultElementType::GetIdFromName, default exception is thrown\n";
    }
}



bool NastranOp2::SResultElementType::isEnabled(int iTypeId)
{
    try
    {
        if (iTypeId<0||iTypeId>=static_cast<int>(bvecEnabledCards.size()))
            return false;
        else
            return bvecEnabledCards.at(iTypeId);
    }
    catch(...)
    {
        cout<<"NastranOp2::SResultElementType::isEnabled, default exception is thrown\n";
    }
}



bool NastranOp2::SResultElementType::isEnabled(const string &strTypeName)
{
    try
    {
        static int pos;
        pos=GetIdFromName(strTypeName);
        if (pos<0||pos>=static_cast<int>(bvecEnabledCards.size()))
            return false;
        else
            return bvecEnabledCards.at(pos);
    }
    catch(...)
    {
        cout<<"NastranOp2::SResultElementType::isEnabled, default exception is thrown\n";
    }
}
