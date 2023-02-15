#include "NastranOp2Parser/Utilities/utilities.h"
#include "NastranOp2Parser/Op2/nastranop2.h"
#include "cnode.h"
#include <iostream>
#include <fstream>,Cnode& oNodeData
#include <string>
#include <string.h>
#include <vector>
using namespace std;


void NastranOp2::Scan_OQG_DataBlock(string &strHeaderName,
                                    int iHeaderBlockLength,std::vector<int> &ivecHeaderBlock,
                                    int iTableBlockLength,std::vector<int> &ivecTableBlock,Cnode& oNodeData)
{
    try//Reference manual 2012, pgno 538
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

        int i,iNumberofWords=0;
        string tmpStr,strPrimaryName;
        switch (iTableContent)
        {
        case 3 :
            strPrimaryName="SPC Forces";
            iNumberofWords=8;
            //availability of spc forces in OQG datablock is set to true
            oNodeData.SetSpcForceAvailable(true);
            break ;
        case 39 :
            strPrimaryName="MPC Forces";
            iNumberofWords=8;
            //availability of mpc forces in OQG datablock is set to true
            oNodeData.SetMpcForceAvailable(true);
            break ;
        default :
            cout << "      Skipping TableContent "<< iTableContent << endl ;
            return ;
            break ;
        }

        int iEKey,iPointId,iPointType,iIter;
        float fConstraintForce[3],fConstraintMoment[3],fScalar;
        vector<int> ivecSkippedPointTypes;

        int *ipPointer = &ivecTableBlock[0];//Start of tableblock is assigned to integer pointer
        int *ipPointerFinal = ipPointer + iTableBlockLength;//(Start pos of table block) + (table block size)
        while (ipPointer<ipPointerFinal)//loop for setting Reaction(Force,Moment) data to Corresponding nodes
        {
            iEKey=ipPointer[0];
            iPointId=iEKey/10; //cout<<"Node Id : "<<iPointId<<"\n";
            iPointType=ipPointer[1];
            if (iPointType==1)
            {
                //grid
                for (iIter = 0; iIter < 3; iIter++)
                {
                    fConstraintForce[iIter] = *(float*)(ipPointer+2+iIter); //cout<<fConstraintForce[iIter]<<" ";
                }//cout<<"\n";
                for (iIter = 0; iIter < 3; iIter++)
                {
                    fConstraintMoment[iIter] = *(float*)(ipPointer+5+iIter); //cout<<fConstraintMoment[iIter]<<" ";
                }//cout<<"\n";
                //sets constraint forces(spc,mpc) at nodes
                if(iTableContent == 3)          //spc
                {
                    oNodeData.SetSpcConstraintForcesatNodes(oNodeData.SearchandReturn_CurrentNode(iPointId),miSubCaseId,1,fConstraintForce,fConstraintMoment);
                }
                else if(iTableContent == 39)    //mpc
                {
                    oNodeData.SetMpcConstraintForcesatNodes(oNodeData.SearchandReturn_CurrentNode(iPointId),miSubCaseId,1,fConstraintForce,fConstraintMoment);
                }
            }
            else if (iPointType==2)
            {
                //scalar
                fScalar = *(float*)(ipPointer+2); //cout<<fScalar<<"\n";
                //need to write set method for this scalar data in Cnode class
            }
            else
                ivecSkippedPointTypes.push_back(iPointType);
            ipPointer+=iNumberofWords;
        }

        for (int iIter = 0; iIter < ivecSkippedPointTypes.size(); iIter++)
        {
            cout << "unrecognized Point type "<<ivecSkippedPointTypes[iIter]<<" is skipped\n";
        }
        oNodeData.SetNbrofSubCases(miSubCaseId);//sets total nbr of subcases for all the nodal results
        miSubCaseId++;
    }
    catch(...)
    {
        cout<<"NastranOp2::Scan_OQG_DataBlock, default exception thrown while scanning oqg table\n";
    }
}



