#include "NastranOp2Parser/Utilities/utilities.h"
#include "NastranOp2Parser/Op2/nastranop2.h"
#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
using namespace std;


void NastranOp2::Scan_OGF_DataBlock(string &strHeaderName,
                                    int iHeaderBlockLength,std::vector<int> &ivecHeaderBlock,
                                    int iTableBlockLength,std::vector<int> &ivecTableBlock)
{
    try//Reference manual 2012, pgno 340
    {
        string strTitle,strSubTitle,strLabel;
        int iApproachCode,iAnalysisType,iDeviceType;
        int iTableCode,iTableContent;
        bool bSort2_Flag,bComplex_Flag,bRandom_Flag;
        int iFormatCode;
        int iStressCode;
        bool bStrain_Flag,bStrainorCurvature_Flag,bMaterialCoordSystemId_Flag;
        int iThermal;
        string strSubCaseName;
        int intIds[2];
        float realIds[2];
        int iELTYPE,iNUMWDE;
        if (!ScanHeaderInformation(ivecHeaderBlock,
                                   strTitle,strSubTitle,strLabel,
                                   iApproachCode,iAnalysisType,iDeviceType,
                                   iTableCode,iTableContent,bSort2_Flag,bComplex_Flag,bRandom_Flag,
                                   iFormatCode,
                                   iStressCode,bStrain_Flag,bStrainorCurvature_Flag,bMaterialCoordSystemId_Flag,
                                   iThermal,
                                   strSubCaseName,intIds,realIds,
                                   iELTYPE,iNUMWDE))
            return;

        if (iThermal)
            return;

        switch (iTableContent)
        {
        case 19 : // Grid point force balance
            // Nothing is done !
            break ;
        default :
            cout << "      Skipping iTableContent "<< iTableContent << endl ;
            return ;
            break ;
        }

        int iIter;
        string tmpStr;
        string strElemType;
        // various results
//        tmpStr="Grid Point Forces, Internal Forces";
//        tmpStr="Grid Point Forces, Internal Moments";
//        tmpStr="Grid Point Forces, MPC Forces";
//        tmpStr="Grid Point Forces, MPC Moments";
//        tmpStr="Grid Point Forces, SPC Forces";
//        tmpStr="Grid Point Forces, SPC Moments";
//        tmpStr="Grid Point Forces, Reaction Forces";
//        tmpStr="Grid Point Forces, Reaction Moments";
//        tmpStr="Grid Point Forces, Applied Forces";
//        tmpStr="Grid Point Forces, Applied Moments";
//        tmpStr="Grid Point Forces, Total Forces";
//        tmpStr="Grid Point Forces, Total Moments";

        int ekey,pointId,elemId;
        float fData[3];
        bool bcancelBlanks = true;

        int *ipPointer = &ivecTableBlock[0];
        int *ipPointerFinal = ipPointer + iTableBlockLength;
        int iNumberofWords = iNUMWDE;
        bool bReac;
        while (ipPointer<ipPointerFinal)
        {
            Utilities::Op2Word_to_String(&ipPointer[2],2,strElemType,bcancelBlanks);

            bReac=false;
            if (strElemType=="APP-LOAD")
            {
            }
            else if (strElemType=="*TOTALS*")
            {
            }
            else if (strElemType=="F-OF-MPC")
            {
                bReac=true;
            }
            else if (strElemType=="F-OF-SPC")
            {
                bReac=true;
            }
            else
            {
            }
            ekey=ipPointer[0];
            pointId=ekey/10;
            elemId=ipPointer[1];
            //Force in displacement coordinate system direction x,y,z
            for (iIter = 0; iIter < 3; iIter++)
                fData[iIter] = *(float*)(ipPointer+4+iIter);
            //Moment in displacement coordinate system direction x,y,z
            for (iIter = 0; iIter < 3; iIter++)
                fData[iIter] = *(float*)(ipPointer+7+iIter);
            ipPointer+=iNumberofWords;
        }
    }
    catch(...)
    {
        cout<<"NastranOp2::Scan_OGF_DataBlock, default exception thrown while scanning ogf table\n";
    }
}

