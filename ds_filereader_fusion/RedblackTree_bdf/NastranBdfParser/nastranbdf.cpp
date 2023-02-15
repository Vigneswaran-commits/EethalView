#include "ds_filereader_fusion/RedblackTree_bdf/NastranBdfParser/nastranbdf.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
using namespace std;

#define ID_GRID   0
#define ID_CQUAD4 1

NastranBDF::NastranBDF()
{
    //ctor
}

void NastranBDF::ReadNastranBDFFile(wxString strFilePathName,Cnode_bdf& oNodeData_bdf,CElement_bdf& oElementData_bdf)
{
    int iLineLimit = 100, iCardLimit = 10;
    char str[iLineLimit], cCard[iCardLimit];
    int iNodeId = 0;
    float fNodeCord[3];
    ifstream fstrReadBDF(strFilePathName, ios::in);
    if(fstrReadBDF.is_open())
    {
        int iScanStatus = -1; //EOF = -1
        while(fstrReadBDF.getline(str, iLineLimit)) //break if error or eof
        {
            iScanStatus = sscanf(str, "%9s", &cCard); //9 + nullterminator = iCardLimit
            if(iScanStatus == -1 || iScanStatus == 0 || cCard[0] == '$') //sscanf input failure || type mismatch || commnets
                continue;
            vector<int> vecElementConnectivity;
            switch (ReturnCardId(cCard))
            {
                //variable common to many cases are declared here
            case ID_GRID:
                iScanStatus = sscanf(str, "%*s %i %f %f %f", &iNodeId, &fNodeCord[0], &fNodeCord[1], &fNodeCord[2]);
                if(iScanStatus == -1 || iScanStatus < 4) //sscanf input failure || type mismatch
                    continue;
                oNodeData_bdf.Insert_Node_Attributes(iNodeId,iNodeId,1,fNodeCord[0],fNodeCord[1],fNodeCord[2],1,1,1);
                break;
            case ID_CQUAD4:
                int iElemId, iElemType;
                int iElemNodes[4];
                iScanStatus = sscanf(str, "%*s %i %i %i %i %i %i", &iElemId, &iElemType ,&iElemNodes[0], &iElemNodes[1], &iElemNodes[2], &iElemNodes[3]);
                if(iScanStatus == -1 || iScanStatus < 4) //sscanf input failure || type mismatch
                    continue;
                vecElementConnectivity.push_back(iElemNodes[0]);
                vecElementConnectivity.push_back(iElemNodes[1]);
                vecElementConnectivity.push_back(iElemNodes[2]);
                vecElementConnectivity.push_back(iElemNodes[3]);
                oElementData_bdf.Insert_Node_Attributes(iElemId,iElemType,"CQUAD4",0,vecElementConnectivity);//elem strength is entered as zero
                break;
            default:
                break;
            }
        }
        fstrReadBDF.close();
    }
}

int NastranBDF::ReturnCardId(char *cCard)
{
    if(strcmp(cCard, "GRID") == 0)
     return ID_GRID;
    else if(strcmp(cCard, "CQUAD4") == 0)
     return ID_CQUAD4;
}

NastranBDF::~NastranBDF()
{
    //dtor
}

