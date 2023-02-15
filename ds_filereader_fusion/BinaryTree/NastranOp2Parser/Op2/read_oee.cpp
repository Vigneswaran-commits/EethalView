#include "NastranOp2Parser/Utilities/utilities.h"
#include "NastranOp2Parser/Op2/nastranop2.h"
#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
using namespace std;


void NastranOp2::Scan_OEE_DataBlock(string &strHeaderName,
                                    int iHeaderBlockLength,std::vector<int> &ivecHeaderBlock,
                                    int iTableBlockLength,std::vector<int> &ivecTableBlock,CElement& oElementData)
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

        //miSubCaseId value is obtained from strLabel
        char cSubCaseId;
        cSubCaseId = strLabel[strLabel.size() - 1];
        // A char is actually just a number interpreted through the ASCII table. By looking at the
        //table, you can find that '0' is 48, '1' is 49 and so on until '9' as 57.
        miSubCaseId = cSubCaseId - 48;

        int iNumberofWords=0;
        string tmpStr,strPrimaryName;

        switch (iTableContent)
        {
        case 18 : // Element Strain Energy
            strPrimaryName="Element Strain Energy";
            iNumberofWords=4;
            oElementData.SetStrainEnergyData_Available(true);
            //cout<<"Element Strain Energy\n";
            break ;
        case 36 : // Element Kinetic Energy
            strPrimaryName="Element Kinetic Energy";
            iNumberofWords=4;
            oElementData.SetKineticEnergyData_Available(true);
            //cout<<"Element Kinetic Energy\n";
            break ;
        case 37 : // Element Energy Loss
            strPrimaryName="Element Energy Loss";
            iNumberofWords=4;
            oElementData.SetEnergyLossData_Available(true);
            //cout<<"Element Energy Loss\n";
            break ;
        default :
            cout << "      Skipping TableContent "<< iTableContent << endl ;
            return ;
            break ;
        }

        int iEKey,iElemId;
        float fElementEnergy,fPercentofTotalEnergy,fElementEnergyDensity;

        int *ipPointer=&ivecTableBlock[0];
        int *ipPointerFinal=ipPointer+iTableBlockLength-4; // One skips the sum on elements !
        while (ipPointer<ipPointerFinal)
        {
            iEKey=ipPointer[0];
            iElemId=iEKey/10;
            //vecElementEnergy is resized according to nbr of subcases
            oElementData.SearchandReturn_CurrentNode(iElemId)->ElementInfo.vecElementEnergy.resize(miSubCaseId);
            //cout<<"elem id : "<<iElemId<<"\n";
            //Element Energy or Subtotal after all elements
            fElementEnergy=*(float*)(ipPointer+1);
            //cout<<"ElemEnergy : "<<fElementEnergy<<"\n";
            //Percent of Total Energy
            fPercentofTotalEnergy=*(float*)(ipPointer+2);
            //cout<<"PercentofTotalEnergy : "<<fPercentofTotalEnergy<<"\n";
            //Element Energy Density
            fElementEnergyDensity=*(float*)(ipPointer+3);
            //cout<<"ElemEnergyDensity : "<<fElementEnergyDensity<<"\n\n";

            //sets element energies(strain,kinetic,loss)
            if(iTableContent == 18)
            {
                oElementData.SetStrainEnergyValues(oElementData.SearchandReturn_CurrentNode(iElemId),miSubCaseId,fElementEnergy,fPercentofTotalEnergy,fElementEnergyDensity);
            }
            else if(iTableContent == 36)
            {
                oElementData.SetKineticEnergyValues(oElementData.SearchandReturn_CurrentNode(iElemId),miSubCaseId,fElementEnergy,fPercentofTotalEnergy,fElementEnergyDensity);
            }
            else if(iTableContent == 37)
            {
                oElementData.SetEnergyLossValues(oElementData.SearchandReturn_CurrentNode(iElemId),miSubCaseId,fElementEnergy,fPercentofTotalEnergy,fElementEnergyDensity);
            }

            ipPointer+=iNumberofWords;
        }
        //miSubCaseId value is obtained from strLabel
        oElementData.SetNbrofSubCases(miSubCaseId);//sets total nbr of subcases for all the nodal results
    }
    catch(...)
    {
        cout<<"NastranOp2::Scan_OEE_DataBlock, default exception thrown while scanning oee table\n";
    }
}



