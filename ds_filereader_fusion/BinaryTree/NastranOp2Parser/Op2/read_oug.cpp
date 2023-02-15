#include "NastranOp2Parser/Utilities/utilities.h"
#include "NastranOp2Parser/op2/nastranop2.h"
#include "cnode.h"
#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
using namespace std;

void NastranOp2::Scan_OUG_DataBlock(string &strHeaderName,
                                    int iHeaderBlockLength,std::vector<int> &ivecHeaderBlock,
                                    int iTableBlockLength,std::vector<int> &ivecTableBlock,Cnode& oNodeData)
{
    try//Reference manual pgno 631 in 2006//545 in 2012
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


//        cout<<"Title : "<<strTitle<<"\n";
//        cout<<"SubTitle : "<<strSubTitle<<"\n";
//        cout<<"Load Cases : "<<strLabel<<"\n";
        //cout<<"Solution Type : "<<strSubCaseName;
        //cout<<"SubCase="<<miSubCaseId<<"\n";

        int iIter,iNumberofWords=0;
        string strPrimaryName;
        int iResultContent = 0;
        //availability of OUG(node output displacement) datablock is set to true
        oNodeData.SetNodeDisplacement_Available(true);
//        *********Type of result to be read available***************
        switch (iTableContent)
        {
        case 1 : // Displacements
        case 7 : // Eigenvectors
        case 14 : // Displacements
        case 15 : // Eigenvectors
            if (iThermal)   //For thermal problem results
            {
                strPrimaryName="Temperature";
            }
            else            //For structural problem results
            {
                strPrimaryName="Displacements";
                iResultContent = 0;
            }
            iNumberofWords=8;
            break ;
        case 10 : // Velocities
        case 16 : // Velocities
            if (iThermal)
            {
                strPrimaryName="Temperature Variation Rate";
            }
            else
            {
                strPrimaryName="Velocities";
                iResultContent = 1;
            }
            iNumberofWords=8;
            break ;
        case 11 : // Accelerations
        case 17 : // Accelerations
            strPrimaryName="Accelerations";
            iResultContent = 2;
            iNumberofWords=8;
            break ;
        default :
            cout << "      Skipping iTableContent "<< iTableContent << endl ;
            return ;
            break ;
        }
//        ******************

        int ekey,pointId,pointType;
        float fTranslation[3],fRotation[3],fScalarValue;
        int *ipPointer=&ivecTableBlock[0];//assigns first element address of ivecTableBlock to integer pointer
        int *ptrFinal=ipPointer+iTableBlockLength;//(Start pos of table block) + (table block size)
        while (ipPointer<ptrFinal)//loop for setting Displacement(Trans+Rot) data to Corresponding nodes
        {
            ekey=ipPointer[0];
            pointId=ekey/10;//cout<<"Node Id : "<<pointId<<"\n";
            pointType=ipPointer[1]; //cout<<"pointType : "<<pointType<<"\n";
            //Point type: G for grid(1) and S for scalar(0)
            if (pointType==1)
            {
                // GRID :
                for (iIter=0; iIter<3; iIter++)
                {
                    fTranslation[iIter]=*(float*)(ipPointer+2+iIter); //cout<<fTranslation[iIter]<<" ";
                }//cout<<"\n";
                for (iIter=0; iIter<3; iIter++)
                {
                    fRotation[iIter]=*(float*)(ipPointer+5+iIter); //cout<<fRotation[iIter]<<" ";
                }//cout<<"\n";
                //sets node output displacement
                oNodeData.SetNode_OutputDisplacement(oNodeData.SearchandReturn_CurrentNode(pointId),miSubCaseId,1,iResultContent,fTranslation,fRotation);
            }
            else if (pointType==2)
            {
                // SPOINT :
                fScalarValue=*(float*)(ipPointer+2);
                oNodeData.SetNode_OutputTemperature(oNodeData.SearchandReturn_CurrentNode(pointId),miSubCaseId,2,fScalarValue);
            }
            else
            {
                cout<<"Unexpected OUG point type\n";
            }
            //ipPointer is not incremented anywhere else in loop, so it is incremented for iNumberofWords
            ipPointer+=iNumberofWords;
        }
        oNodeData.SetNbrofSubCases(miSubCaseId);//sets total nbr of subcases for all the nodal results
        miSubCaseId++;
    }
    catch(...)
    {
        cout<<"NastranOp2::scanOp2_finalize_OUG, default exception thrown while scanning oug table\n";
    }
}
