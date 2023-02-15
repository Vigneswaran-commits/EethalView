#include "NastranOp2Parser/Utilities/utilities.h"
#include "NastranOp2Parser/Op2/nastranop2.h"
#include "cnode.h"
#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
using namespace std;


void NastranOp2::Scan_OPG_DataBlock(string &strHeaderName,
                                    int iHeaderBlockLength,std::vector<int> &ivecHeaderBlock,
                                    int iTableBlockLength,std::vector<int> &ivecTableBlock,Cnode& oNodeData)
{
    try//Reference manual 2012, pgno 531
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

        //availability of OPG(nodal applied load) datablock is set to true
        oNodeData.SetNodalAppliedLoad_Available(true);
        switch (iTableContent)
        {
        case 2 : // Load vector
            // Nothing is done!
            break ;
        default :
            cout << "   Skipping table_content "<< iTableContent << endl ;
            return ;
            break ;
        }

        int iIter,iPointType;
        int iEKey,pointId;
        float frgAppliedForce[3],frgAppliedMoment[3];

        int *ipPointer=&ivecTableBlock[0];
        int *ipPointerFinal=ipPointer+iTableBlockLength;
        int iNumberofWords=iNUMWDE;
        while (ipPointer<ipPointerFinal)
        {
            iEKey=ipPointer[0];
            pointId=iEKey/10;     //cout<<"node id : "<<pointId<<"\n";
            iPointType=ipPointer[1];
            if(iPointType == 1)
            {
                //applied force in x,y,z
                for (iIter = 0; iIter < 3; iIter++)
                {
                    frgAppliedForce[iIter] = *(float*)(ipPointer+2+iIter);//cout<<frgAppliedForce[0]<<"\n";
                }//cout<<"\n";
                //applied moment about x,y,z
                for (iIter = 0; iIter < 3; iIter++)
                {
                    frgAppliedMoment[iIter] = *(float*)(ipPointer+5+iIter);//cout<<frgAppliedMoment[0]<<"\n";
                }//cout<<"\n";
                //sets applied loads at nodes
                oNodeData.SetAppliedLoadsatNodes(oNodeData.SearchandReturn_CurrentNode(pointId),miSubCaseId,1,frgAppliedForce,frgAppliedMoment);
            }
            else if(iPointType == 2)
            {
                //for scalar applied loads
            }
            ipPointer+=iNumberofWords;
        }
        oNodeData.SetNbrofSubCases(miSubCaseId);//sets total nbr of subcases for all the nodal results
        miSubCaseId++;
    }
    catch(...)
    {
        cout<<"NastranOp2::Scan_OPG_DataBlock, default exception thrown while scanning opg table\n";
    }
}



