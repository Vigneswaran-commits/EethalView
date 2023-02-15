#include "NastranOp2Parser/op2/nastranop2.h"
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

using namespace std ;

void NastranOp2::Scan_OES_DataBlock(string &strHeaderName,
                                    int iHeaderBlockLength,std::vector<int> &ivecHeaderBlock,
                                    int iTableBlockLength,std::vector<int> &ivecTableBlock,CElement& oElementData)
{
    try
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

        char cSubCaseId;
        //miSubCaseId value is obtained from strLabel
        cSubCaseId = strLabel[strLabel.size() - 1];
        // A char is actually just a number interpreted through the ASCII table. By looking at the
        //table, you can find that '0' is 48, '1' is 49 and so on until '9' as 57.
        miSubCaseId = cSubCaseId - 48;

        if (iThermal)
            return;

        switch (iTableContent)
        {
        case 5 : // Element stress (or strain)
            // Nothing is done!
            break ;
        default :
            cout << "      Skipping iTableContent "<< iTableContent << endl ;
            return ;
            break ;
        }

        if (!NastranOp2::SResultElementType::isEnabled(iELTYPE))
        {
            cout << "      Skipping element type \""
                 << NastranOp2::SResultElementType::GetNameFromId(iELTYPE) << "\"" << endl ;
            return ;
        }

        int nbrPoints=0,nbrWords=0,unitLength=0;

        switch (iELTYPE)
        {
        case NastranOp2::SResultElementType::ROD :
        case NastranOp2::SResultElementType::CONROD :
        case NastranOp2::SResultElementType::TUBE :
            nbrWords=5;
            break;
        case NastranOp2::SResultElementType::SHEAR :
            nbrWords=4;
            break;
        case NastranOp2::SResultElementType::BEAM :
            nbrPoints=11;
            unitLength=10;
            nbrWords=1+nbrPoints*unitLength;
            break;
        case NastranOp2::SResultElementType::ELAS1 :
        case NastranOp2::SResultElementType::ELAS2 :
        case NastranOp2::SResultElementType::ELAS3 :
            nbrWords=2;
            break;
        case NastranOp2::SResultElementType::QUAD4 :
            nbrWords=17;
            break;
        case NastranOp2::SResultElementType::BAR :
            nbrWords=16;
            break;
        case NastranOp2::SResultElementType::GAP :
            nbrWords=9;
            break;
        case NastranOp2::SResultElementType::TETRA :
            nbrPoints=5;
            unitLength=21;
            nbrWords=4+nbrPoints*unitLength;
            break;
        case NastranOp2::SResultElementType::BUSH1D :
            nbrWords=8;
            break;
        case NastranOp2::SResultElementType::QUAD8 :
            nbrPoints=5;
            unitLength=17;
            nbrWords=2+nbrPoints*unitLength;
            break;
        case NastranOp2::SResultElementType::HEXA :
            nbrPoints=9;
            unitLength=21;
            nbrWords=4+nbrPoints*unitLength;
            break;
        case NastranOp2::SResultElementType::PENTA :
            nbrPoints=7;
            unitLength=21;
            nbrWords=4+nbrPoints*unitLength;
            break;
        case NastranOp2::SResultElementType::BEND :
            nbrPoints=2;
            unitLength=10;
            nbrWords=1+nbrPoints*unitLength;
            break;
        case NastranOp2::SResultElementType::TRIAR :
            nbrPoints=4;
            unitLength=17;
            nbrWords=2+nbrPoints*unitLength;
            break;
        case NastranOp2::SResultElementType::TRIA3 :
            nbrWords=17;
            break;
        case NastranOp2::SResultElementType::TRIA6 :
            nbrPoints=4;
            unitLength=17;
            nbrWords=2+nbrPoints*unitLength;
            break;
        case NastranOp2::SResultElementType::QUADR :
            nbrPoints=5;
            unitLength=17;
            nbrWords=2+nbrPoints*unitLength;
            break;
        case NastranOp2::SResultElementType::TETRANL :
            nbrPoints=5;
            unitLength=16;
            nbrWords=2+nbrPoints*unitLength;
            break;
        case NastranOp2::SResultElementType::GAPNL :
            nbrWords=11;
            break;
        case NastranOp2::SResultElementType::TUBENL :
        case NastranOp2::SResultElementType::RODNL :
        case NastranOp2::SResultElementType::CONRODNL :
            nbrWords=7;
            break;
        case NastranOp2::SResultElementType::TRIA3NL :
        case NastranOp2::SResultElementType::QUAD4NL :
        case NastranOp2::SResultElementType::QUADRNL :
        case NastranOp2::SResultElementType::TRIARNL :
            nbrWords=iNUMWDE;
            break;
        case NastranOp2::SResultElementType::PENTANL :
            nbrPoints=7;
            unitLength=16;
            nbrWords=2+nbrPoints*unitLength;
            break;
        case NastranOp2::SResultElementType::HEXANL :
            nbrPoints=9;
            unitLength=16;
            nbrWords=2+nbrPoints*unitLength;
            break;
        case NastranOp2::SResultElementType::BEAMNL :
            nbrWords=51;
            break;
        case NastranOp2::SResultElementType::QUAD4LC :
        case NastranOp2::SResultElementType::QUAD8LC :
        case NastranOp2::SResultElementType::TRIA3LC :
        case NastranOp2::SResultElementType::TRIA6LC :
        case NastranOp2::SResultElementType::QUADRLC :
        case NastranOp2::SResultElementType::TRIARLC :
            nbrWords=11;
            break;
        case NastranOp2::SResultElementType::BUSH :
        case NastranOp2::SResultElementType::CFAST :
            nbrWords=7;
            break;
        case NastranOp2::SResultElementType::QUAD144 :
            nbrPoints=5;
            unitLength=17;
            nbrWords=2+nbrPoints*unitLength;
            break;
        case NastranOp2::SResultElementType::ELAS1NL :
        case NastranOp2::SResultElementType::ELAS3NL :
            nbrWords=3;
            break;
        case NastranOp2::SResultElementType::BUSHNL :
            nbrWords=19;
            break;
        default :
            cout << "      Skipping iELTYPE " << iELTYPE << endl ;
            return;
            break ;
        } ;

        int i,j,k,shiftJ,shiftK,mCsId;
        string tmpStr;
        int ekey,elemId,pointId,layerId;
        string layerName,cType;

        float *V;
//        switch (iELTYPE)
//        {
//        case NastranOp2::SResultElementType::ROD :
//        case NastranOp2::SResultElementType::CONROD :
//        case NastranOp2::SResultElementType::TUBE :
//        case NastranOp2::SResultElementType::BEND :
//            if (f_strain)
//                tmpStr="Beam Axial Strain for Axial Loads";
//            else
//                tmpStr="Beam Axial Stress for Axial Loads";
//            dbKey.setResId(tmpStr);
//            BeamStressA=DB.getResult(dbKey,true);
//            if (BeamStressA)
//            {
//                BeamStressA->setName(tmpStr);
//                BeamStressA->tensorOrder=0;
//                BeamStressA->format=1;
//                BeamStressA->setIntId(0,intIds[0]);
//                BeamStressA->setIntId(1,intIds[1]);
//                BeamStressA->setRealId(0,realIds[0]);
//                BeamStressA->setRealId(1,realIds[1]);
//            }
//            if (f_strain)
//                tmpStr="Beam Shear Strain for Torsion Loads";
//            else
//                tmpStr="Beam Shear Stress for Torsion Loads";
//            dbKey.setResId(tmpStr);
//            BeamStressT=DB.getResult(dbKey,true);
//            if (BeamStressT)
//            {
//                BeamStressT->setName(tmpStr);
//                BeamStressT->tensorOrder=0;
//                BeamStressT->format=1;
//                BeamStressT->setIntId(0,intIds[0]);
//                BeamStressT->setIntId(1,intIds[1]);
//                BeamStressT->setRealId(0,realIds[0]);
//                BeamStressT->setRealId(1,realIds[1]);
//            }
//            break ;
//        case NastranOp2::SResultElementType::SHEAR :
//            if (f_strain)
//                tmpStr="Shear Panel Strain, Max";
//            else
//                tmpStr="Shear Panel Stress, Max";
//            dbKey.setResId(tmpStr);
//            shearPanelMax=DB.getResult(dbKey,true);
//            if (shearPanelMax)
//            {
//                shearPanelMax->setName(tmpStr);
//                shearPanelMax->tensorOrder=0;
//                shearPanelMax->format=1;
//                shearPanelMax->setIntId(0,intIds[0]);
//                shearPanelMax->setIntId(1,intIds[1]);
//                shearPanelMax->setRealId(0,realIds[0]);
//                shearPanelMax->setRealId(1,realIds[1]);
//            }
//            if (f_strain)
//                tmpStr="Shear Panel Strain, Average";
//            else
//                tmpStr="Shear Panel Stress, Average";
//            dbKey.setResId(tmpStr);
//            shearPanelAverage=DB.getResult(dbKey,true);
//            if (shearPanelAverage)
//            {
//                shearPanelAverage->setName(tmpStr);
//                shearPanelAverage->tensorOrder=0;
//                shearPanelAverage->format=1;
//                shearPanelAverage->setIntId(0,intIds[0]);
//                shearPanelAverage->setIntId(1,intIds[1]);
//                shearPanelAverage->setRealId(0,realIds[0]);
//                shearPanelAverage->setRealId(1,realIds[1]);
//            }
//            break;
//        case NastranOp2::SResultElementType::BEAM :
//            if (f_strain)
//                tmpStr="Beam Axial Strain for Total Loads";
//            else
//                tmpStr="Beam Axial Stress for Total Loads";
//            dbKey.setResId(tmpStr);
//            BeamStressB=DB.getResult(dbKey,true);
//            if (BeamStressB)
//            {
//                BeamStressB->setName(tmpStr);
//                BeamStressB->tensorOrder=0;
//                BeamStressB->format=1;
//                BeamStressB->setIntId(0,intIds[0]);
//                BeamStressB->setIntId(1,intIds[1]);
//                BeamStressB->setRealId(0,realIds[0]);
//                BeamStressB->setRealId(1,realIds[1]);
//            }
//            break ;
//        case NastranOp2::SResultElementType::ELAS1 :
//        case NastranOp2::SResultElementType::ELAS2 :
//        case NastranOp2::SResultElementType::ELAS3 :
//        case NastranOp2::SResultElementType::ELAS1NL :
//        case NastranOp2::SResultElementType::ELAS3NL :
//            if (f_strain)
//                tmpStr="Spring Scalar Strain";
//            else
//                tmpStr="Spring Scalar Stress";
//            dbKey.setResId(tmpStr);
//            BeamStressA=DB.getResult(dbKey,true);
//            if (BeamStressA)
//            {
//                BeamStressA->setName(tmpStr);
//                BeamStressA->tensorOrder=0;
//                BeamStressA->format=1;
//                BeamStressA->setIntId(0,intIds[0]);
//                BeamStressA->setIntId(1,intIds[1]);
//                BeamStressA->setRealId(0,realIds[0]);
//                BeamStressA->setRealId(1,realIds[1]);
//            }
//            break ;
//        case NastranOp2::SResultElementType::BAR :
//            if (f_strain)
//                tmpStr="Beam Axial Strain for Axial Loads";
//            else
//                tmpStr="Beam Axial Stress for Axial Loads";
//            dbKey.setResId(tmpStr);
//            BeamStressA=DB.getResult(dbKey,true);
//            if (BeamStressA)
//            {
//                BeamStressA->setName(tmpStr);
//                BeamStressA->tensorOrder=0;
//                BeamStressA->format=1;
//                BeamStressA->setIntId(0,intIds[0]);
//                BeamStressA->setIntId(1,intIds[1]);
//                BeamStressA->setRealId(0,realIds[0]);
//                BeamStressA->setRealId(1,realIds[1]);
//            }
//            if (f_strain)
//                tmpStr="Beam Axial Strain for Bending Loads";
//            else
//                tmpStr="Beam Axial Stress for Bending Loads";
//            dbKey.setResId(tmpStr);
//            BeamStressB=DB.getResult(dbKey,true);
//            if (BeamStressB)
//            {
//                BeamStressB->setName(tmpStr);
//                BeamStressB->tensorOrder=0;
//                BeamStressB->format=1;
//                BeamStressB->setIntId(0,intIds[0]);
//                BeamStressB->setIntId(1,intIds[1]);
//                BeamStressB->setRealId(0,realIds[0]);
//                BeamStressB->setRealId(1,realIds[1]);
//            }
//            if (f_strain)
//                tmpStr="Beam Axial Strain for Total Loads";
//            else
//                tmpStr="Beam Axial Stress for Total Loads";
//            dbKey.setResId(tmpStr);
//            BeamStressT=DB.getResult(dbKey,true);
//            if (BeamStressT)
//            {
//                BeamStressT->setName(tmpStr);
//                BeamStressT->tensorOrder=0;
//                BeamStressT->format=1;
//                BeamStressT->setIntId(0,intIds[0]);
//                BeamStressT->setIntId(1,intIds[1]);
//                BeamStressT->setRealId(0,realIds[0]);
//                BeamStressT->setRealId(1,realIds[1]);
//            }
//            break ;
//        case NastranOp2::SResultElementType::BUSH1D :
//            tmpStr="Beam Forces";
//            dbKey.setResId(tmpStr);
//            BeamForces=DB.getResult(dbKey,true);
//            if (BeamForces)
//            {
//                BeamForces->setName(tmpStr);
//                BeamForces->tensorOrder=2;
//                BeamForces->format=1;
//                BeamForces->setIntId(0,intIds[0]);
//                BeamForces->setIntId(1,intIds[1]);
//                BeamForces->setRealId(0,realIds[0]);
//                BeamForces->setRealId(1,realIds[1]);
//            }
//            tmpStr="Beam Deformations";
//            dbKey.setResId(tmpStr);
//            BeamDeformations=DB.getResult(dbKey,true);
//            if (BeamDeformations)
//            {
//                BeamDeformations->setName(tmpStr);
//                BeamDeformations->tensorOrder=2;
//                BeamDeformations->format=1;
//                BeamDeformations->setIntId(0,intIds[0]);
//                BeamDeformations->setIntId(1,intIds[1]);
//                BeamDeformations->setRealId(0,realIds[0]);
//                BeamDeformations->setRealId(1,realIds[1]);
//            }
//            tmpStr="Beam Velocities";
//            dbKey.setResId(tmpStr);
//            BeamVelocities=DB.getResult(dbKey,true);
//            if (BeamVelocities)
//            {
//                BeamVelocities->setName(tmpStr);
//                BeamVelocities->tensorOrder=2;
//                BeamVelocities->format=1;
//                BeamVelocities->setIntId(0,intIds[0]);
//                BeamVelocities->setIntId(1,intIds[1]);
//                BeamVelocities->setRealId(0,realIds[0]);
//                BeamVelocities->setRealId(1,realIds[1]);
//            }
//            tmpStr="Beam Axial Stress for Axial Loads";
//            dbKey.setResId(tmpStr);
//            BeamStress=DB.getResult(dbKey,true);
//            if (BeamStress)
//            {
//                BeamStress->setName(tmpStr);
//                BeamStress->tensorOrder=0;
//                BeamStress->format=1;
//                BeamStress->setIntId(0,intIds[0]);
//                BeamStress->setIntId(1,intIds[1]);
//                BeamStress->setRealId(0,realIds[0]);
//                BeamStress->setRealId(1,realIds[1]);
//            }
//            tmpStr="Beam Axial Strain for Axial Loads";
//            dbKey.setResId(tmpStr);
//            BeamStrain=DB.getResult(dbKey,true);
//            if (BeamStrain)
//            {
//                BeamStrain->setName(tmpStr);
//                BeamStrain->tensorOrder=0;
//                BeamStrain->format=1;
//                BeamStrain->setIntId(0,intIds[0]);
//                BeamStrain->setIntId(1,intIds[1]);
//                BeamStrain->setRealId(0,realIds[0]);
//                BeamStrain->setRealId(1,realIds[1]);
//            }
//            tmpStr="Bush Plastic Strain";
//            dbKey.setResId(tmpStr);
//            bushPlasticStrain=DB.getResult(dbKey,true);
//            if (bushPlasticStrain)
//            {
//                bushPlasticStrain->setName(tmpStr);
//                bushPlasticStrain->tensorOrder=0;
//                bushPlasticStrain->format=1;
//                bushPlasticStrain->setIntId(0,intIds[0]);
//                bushPlasticStrain->setIntId(1,intIds[1]);
//                bushPlasticStrain->setRealId(0,realIds[0]);
//                bushPlasticStrain->setRealId(1,realIds[1]);
//            }
//            break ;
//        case NastranOp2::SResultElementType::BUSH :
//        case NastranOp2::SResultElementType::CFAST :
//            if (f_strain)
//                tmpStr="Bush Forces Strain Tensor";
//            else
//                tmpStr="Bush Forces Stress Tensor";
//            dbKey.setResId(tmpStr);
//            BeamForces=DB.getResult(dbKey,true);
//            if (BeamForces)
//            {
//                BeamForces->setName(tmpStr);
//                BeamForces->tensorOrder=2;
//                BeamForces->format=1;
//                BeamForces->setIntId(0,intIds[0]);
//                BeamForces->setIntId(1,intIds[1]);
//                BeamForces->setRealId(0,realIds[0]);
//                BeamForces->setRealId(1,realIds[1]);
//            }
//            if (f_strain)
//                tmpStr="Bush Moments Strain Tensor";
//            else
//                tmpStr="Bush Moments Stress Tensor";
//            dbKey.setResId(tmpStr);
//            BeamMoments=DB.getResult(dbKey,true);
//            if (BeamMoments)
//            {
//                BeamMoments->setName(tmpStr);
//                BeamMoments->tensorOrder=2;
//                BeamMoments->format=1;
//                BeamMoments->setIntId(0,intIds[0]);
//                BeamMoments->setIntId(1,intIds[1]);
//                BeamMoments->setRealId(0,realIds[0]);
//                BeamMoments->setRealId(1,realIds[1]);
//            }
//            break ;
//        case NastranOp2::SResultElementType::TETRANL :
//        case NastranOp2::SResultElementType::PENTANL :
//        case NastranOp2::SResultElementType::HEXANL :
//        case NastranOp2::SResultElementType::TRIA3NL :
//        case NastranOp2::SResultElementType::QUAD4NL :
//        case NastranOp2::SResultElementType::QUADRNL :
//        case NastranOp2::SResultElementType::TRIARNL :
//            tmpStr="Stress Tensor";
//            dbKey.setResId(tmpStr);
//            stressNL=DB.getResult(dbKey,true);
//            if (stressNL)
//            {
//                stressNL->setName(tmpStr);
//                stressNL->tensorOrder=2;
//                stressNL->format=1;
//                stressNL->setIntId(0,intIds[0]);
//                stressNL->setIntId(1,intIds[1]);
//                stressNL->setRealId(0,realIds[0]);
//                stressNL->setRealId(1,realIds[1]);
//            }
//            tmpStr="Strain Tensor";
//            dbKey.setResId(tmpStr);
//            strainNL=DB.getResult(dbKey,true);
//            if (strainNL)
//            {
//                strainNL->setName(tmpStr);
//                strainNL->tensorOrder=2;
//                strainNL->format=1;
//                strainNL->setIntId(0,intIds[0]);
//                strainNL->setIntId(1,intIds[1]);
//                strainNL->setRealId(0,realIds[0]);
//                strainNL->setRealId(1,realIds[1]);
//            }
//            tmpStr="Effective Plastic Strain";
//            dbKey.setResId(tmpStr);
//            strainNLplastic=DB.getResult(dbKey,true);
//            if (strainNLplastic)
//            {
//                strainNLplastic->setName(tmpStr);
//                strainNLplastic->tensorOrder=0;
//                strainNLplastic->format=1;
//                strainNLplastic->setIntId(0,intIds[0]);
//                strainNLplastic->setIntId(1,intIds[1]);
//                strainNLplastic->setRealId(0,realIds[0]);
//                strainNLplastic->setRealId(1,realIds[1]);
//            }
//            tmpStr="Effective Creep Strain";
//            dbKey.setResId(tmpStr);
//            strainNLcreep=DB.getResult(dbKey,true);
//            if (strainNLcreep)
//            {
//                strainNLcreep->setName(tmpStr);
//                strainNLcreep->tensorOrder=0;
//                strainNLcreep->format=1;
//                strainNLcreep->setIntId(0,intIds[0]);
//                strainNLcreep->setIntId(1,intIds[1]);
//                strainNLcreep->setRealId(0,realIds[0]);
//                strainNLcreep->setRealId(1,realIds[1]);
//            }
//            break;
//        case NastranOp2::SResultElementType::GAP :
//        case NastranOp2::SResultElementType::GAPNL :
//            tmpStr="Beam Forces";
//            dbKey.setResId(tmpStr);
//            BeamForces=DB.getResult(dbKey,true);
//            if (BeamForces)
//            {
//                BeamForces->setName(tmpStr);
//                BeamForces->tensorOrder=2;
//                BeamForces->format=1;
//                BeamForces->setIntId(0,intIds[0]);
//                BeamForces->setIntId(1,intIds[1]);
//                BeamForces->setRealId(0,realIds[0]);
//                BeamForces->setRealId(1,realIds[1]);
//            }
//            tmpStr="Beam Deformations";
//            dbKey.setResId(tmpStr);
//            BeamDeformations=DB.getResult(dbKey,true);
//            if (BeamDeformations)
//            {
//                BeamDeformations->setName(tmpStr);
//                BeamDeformations->tensorOrder=2;
//                BeamDeformations->format=1;
//                BeamDeformations->setIntId(0,intIds[0]);
//                BeamDeformations->setIntId(1,intIds[1]);
//                BeamDeformations->setRealId(0,realIds[0]);
//                BeamDeformations->setRealId(1,realIds[1]);
//            }
//            tmpStr="Gap Slips";
//            dbKey.setResId(tmpStr);
//            GapSlips=DB.getResult(dbKey,true);
//            if (GapSlips)
//            {
//                GapSlips->setName(tmpStr);
//                GapSlips->tensorOrder=2;
//                GapSlips->format=1;
//                GapSlips->setIntId(0,intIds[0]);
//                GapSlips->setIntId(1,intIds[1]);
//                GapSlips->setRealId(0,realIds[0]);
//                GapSlips->setRealId(1,realIds[1]);
//            }
//            break;
//        case NastranOp2::SResultElementType::TUBENL :
//        case NastranOp2::SResultElementType::RODNL :
//        case NastranOp2::SResultElementType::CONRODNL :
//            tmpStr="Beam Axial Stress for Axial Loads";
//            dbKey.setResId(tmpStr);
//            BeamStressNL=DB.getResult(dbKey,true);
//            if (BeamStressNL)
//            {
//                BeamStressNL->setName(tmpStr);
//                BeamStressNL->tensorOrder=0;
//                BeamStressNL->format=1;
//                BeamStressNL->setIntId(0,intIds[0]);
//                BeamStressNL->setIntId(1,intIds[1]);
//                BeamStressNL->setRealId(0,realIds[0]);
//                BeamStressNL->setRealId(1,realIds[1]);
//            }
//            tmpStr="Beam Axial Strain for Axial Loads";
//            dbKey.setResId(tmpStr);
//            BeamStrainNL=DB.getResult(dbKey,true);
//            if (BeamStrainNL)
//            {
//                BeamStrainNL->setName(tmpStr);
//                BeamStrainNL->tensorOrder=0;
//                BeamStrainNL->format=1;
//                BeamStrainNL->setIntId(0,intIds[0]);
//                BeamStrainNL->setIntId(1,intIds[1]);
//                BeamStrainNL->setRealId(0,realIds[0]);
//                BeamStrainNL->setRealId(1,realIds[1]);
//            }
//            tmpStr="Effective Plastic Strain";
//            dbKey.setResId(tmpStr);
//            strainNLplastic=DB.getResult(dbKey,true);
//            if (strainNLplastic)
//            {
//                strainNLplastic->setName(tmpStr);
//                strainNLplastic->tensorOrder=0;
//                strainNLplastic->format=1;
//                strainNLplastic->setIntId(0,intIds[0]);
//                strainNLplastic->setIntId(1,intIds[1]);
//                strainNLplastic->setRealId(0,realIds[0]);
//                strainNLplastic->setRealId(1,realIds[1]);
//            }
//            tmpStr="Effective Creep Strain";
//            dbKey.setResId(tmpStr);
//            strainNLcreep=DB.getResult(dbKey,true);
//            if (strainNLcreep)
//            {
//                strainNLcreep->setName(tmpStr);
//                strainNLcreep->tensorOrder=0;
//                strainNLcreep->format=1;
//                strainNLcreep->setIntId(0,intIds[0]);
//                strainNLcreep->setIntId(1,intIds[1]);
//                strainNLcreep->setRealId(0,realIds[0]);
//                strainNLcreep->setRealId(1,realIds[1]);
//            }
//            break;
//        case NastranOp2::SResultElementType::BEAMNL :
//            tmpStr="Beam Axial Stress for Total Loads";
//            dbKey.setResId(tmpStr);
//            BeamStressNL=DB.getResult(dbKey,true);
//            if (BeamStressNL)
//            {
//                BeamStressNL->setName(tmpStr);
//                BeamStressNL->tensorOrder=0;
//                BeamStressNL->format=1;
//                BeamStressNL->setIntId(0,intIds[0]);
//                BeamStressNL->setIntId(1,intIds[1]);
//                BeamStressNL->setRealId(0,realIds[0]);
//                BeamStressNL->setRealId(1,realIds[1]);
//            }
//            tmpStr="Beam Axial Strain for Total Loads";
//            dbKey.setResId(tmpStr);
//            BeamStrainNL=DB.getResult(dbKey,true);
//            if (BeamStrainNL)
//            {
//                BeamStrainNL->setName(tmpStr);
//                BeamStrainNL->tensorOrder=0;
//                BeamStrainNL->format=1;
//                BeamStrainNL->setIntId(0,intIds[0]);
//                BeamStrainNL->setIntId(1,intIds[1]);
//                BeamStrainNL->setRealId(0,realIds[0]);
//                BeamStrainNL->setRealId(1,realIds[1]);
//            }
//            tmpStr="Effective Plastic Strain";
//            dbKey.setResId(tmpStr);
//            strainNLplastic=DB.getResult(dbKey,true);
//            if (strainNLplastic)
//            {
//                strainNLplastic->setName(tmpStr);
//                strainNLplastic->tensorOrder=0;
//                strainNLplastic->format=1;
//                strainNLplastic->setIntId(0,intIds[0]);
//                strainNLplastic->setIntId(1,intIds[1]);
//                strainNLplastic->setRealId(0,realIds[0]);
//                strainNLplastic->setRealId(1,realIds[1]);
//            }
//            tmpStr="Effective Creep Strain";
//            dbKey.setResId(tmpStr);
//            strainNLcreep=DB.getResult(dbKey,true);
//            if (strainNLcreep)
//            {
//                strainNLcreep->setName(tmpStr);
//                strainNLcreep->tensorOrder=0;
//                strainNLcreep->format=1;
//                strainNLcreep->setIntId(0,intIds[0]);
//                strainNLcreep->setIntId(1,intIds[1]);
//                strainNLcreep->setRealId(0,realIds[0]);
//                strainNLcreep->setRealId(1,realIds[1]);
//            }
//            break;
//        case NastranOp2::SResultElementType::BUSHNL :
//            tmpStr="Beam Forces";
//            dbKey.setResId(tmpStr);
//            BeamForces=DB.getResult(dbKey,true);
//            if (BeamForces)
//            {
//                BeamForces->setName(tmpStr);
//                BeamForces->tensorOrder=2;
//                BeamForces->format=1;
//                BeamForces->setIntId(0,intIds[0]);
//                BeamForces->setIntId(1,intIds[1]);
//                BeamForces->setRealId(0,realIds[0]);
//                BeamForces->setRealId(1,realIds[1]);
//            }
//            tmpStr="Beam Moments";
//            dbKey.setResId(tmpStr);
//            BeamMoments=DB.getResult(dbKey,true);
//            if (BeamMoments)
//            {
//                BeamMoments->setName(tmpStr);
//                BeamMoments->tensorOrder=2;
//                BeamMoments->format=1;
//                BeamMoments->setIntId(0,intIds[0]);
//                BeamMoments->setIntId(1,intIds[1]);
//                BeamMoments->setRealId(0,realIds[0]);
//                BeamMoments->setRealId(1,realIds[1]);
//            }
//            break ;
//        default :
//            if (f_strain)
//            {
//                tmpStr="Strain Tensor";
//                dbKey.setResId(tmpStr);
//                tmpStress=DB.getResult(dbKey,true);
//                if (tmpStress)
//                {
//                    tmpStress->setName(tmpStr);
//                    tmpStress->tensorOrder=2;
//                    tmpStress->format=1;
//                    tmpStress->setIntId(0,intIds[0]);
//                    tmpStress->setIntId(1,intIds[1]);
//                    tmpStress->setRealId(0,realIds[0]);
//                    tmpStress->setRealId(1,realIds[1]);
//                }
//                tmpStr="Curvature Tensor";
//                dbKey.setResId(tmpStr);
//                tmpCurvature=DB.getResult(dbKey,true);
//                if (tmpCurvature)
//                {
//                    tmpCurvature->setName(tmpStr);
//                    tmpCurvature->tensorOrder=2;
//                    tmpCurvature->format=1;
//                    tmpCurvature->setIntId(0,intIds[0]);
//                    tmpCurvature->setIntId(1,intIds[1]);
//                    tmpCurvature->setRealId(0,realIds[0]);
//                    tmpCurvature->setRealId(1,realIds[1]);
//                }
//            }
//            else
//            {
//                tmpStr="Stress Tensor";
//                dbKey.setResId(tmpStr);
//                tmpStress=DB.getResult(dbKey,true);
//                if (tmpStress)
//                {
//                    tmpStress->setName(tmpStr);
//                    tmpStress->tensorOrder=2;
//                    tmpStress->format=1;
//                    tmpStress->setIntId(0,intIds[0]);
//                    tmpStress->setIntId(1,intIds[1]);
//                    tmpStress->setRealId(0,realIds[0]);
//                    tmpStress->setRealId(1,realIds[1]);
//                }
//            }
//            break ;
//        }

//        if (!(tmpStress||tmpCurvature||BeamStressA||BeamStressB||BeamStressT
//                ||stressNL||strainNL||strainNLplastic||strainNLcreep
//                ||BeamStressNL||BeamStrainNL||BeamForces||BeamMoments
//                ||BeamDeformations||GapSlips||BeamVelocities
//                ||shearPanelMax||shearPanelAverage
//                ||BeamStress||BeamStrain||bushPlasticStrain))
//            return;

        bool bStation;
        int statCounter; // Stations in CBEAM element
        float frgOesData[nbrWords];
        int *ptr=&ivecTableBlock[0];
        int *ptrFinal=ptr+iTableBlockLength;
        while (ptr<ptrFinal)
        {
            float fStressStrainComponents[6] = {0.0,0.0,0.0,0.0,0.0,0.0};
            ekey=ptr[0];
            elemId=ekey/10; //cout<<"element id : "<<elemId<<"\n";
            //vecStressStrainData is resized according to nbr of subcases
            oElementData.SearchandReturn_CurrentNode(elemId)->ElementInfo.vecStressStrainData.resize(miSubCaseId);
            switch (iELTYPE)
            {
            case NastranOp2::SResultElementType::ROD :  //tested
            case NastranOp2::SResultElementType::CONROD :
            case NastranOp2::SResultElementType::TUBE :
                if(bStrain_Flag)
                {
                    frgOesData[0] = *(float*)(ptr+1);   //axial strain
//                    frgOesData[1] = *(float*)(ptr+2); //axial safety margin
                    frgOesData[2] = *(float*)(ptr+3);   //torsional strain
//                    frgOesData[3] = *(float*)(ptr+4); //torsional safety margin
                }
                break;
            case NastranOp2::SResultElementType::SHEAR ://not tested
                if(bStrain_Flag)
                {
                    frgOesData[0] = *(float*)(ptr+1); //maximum shear
                    frgOesData[1] = *(float*)(ptr+2); //average shear
//                    frgOesData[2] = *(float*)(ptr+3); //skipping safety margin
                }
                break;
//            case NastranOp2::SResultElementType::BEAM :
//                if (BeamStressB)
//                {
//                    V=tmpValues1.getVals();
//                    statCounter=0;
//                    for (i=0; i<nbrPoints; i++)
//                    {
//                        pointId=ptr[1+unitLength*i];
//                        bStation=false;
//                        if (pointId>0)
//                        {
//                            tmpKey.setNodeId(pointId);
//                            bStation=true;
//                        }
//                        else
//                        {
//                            tmpKey.setNodeId(node::nodeIds::CbeamSt1-statCounter);
//                            statCounter++;
//                            bStation=(((float*)ptr)[unitLength*i+2]>1.0e-16);
//                        }
//                        if (bStation)
//                        {
//                            tmpKey.setLayerId(result::key::layerType::PointC);
//                            V[0]=*(float*)(ptr+3+unitLength*i);
//                            BeamStressB->data[tmpKey]=tmpValues1;
//                            tmpKey.setLayerId(result::key::layerType::PointD);
//                            V[0]=*(float*)(ptr+4+unitLength*i);
//                            BeamStressB->data[tmpKey]=tmpValues1;
//                            tmpKey.setLayerId(result::key::layerType::PointE);
//                            V[0]=*(float*)(ptr+5+unitLength*i);
//                            BeamStressB->data[tmpKey]=tmpValues1;
//                            tmpKey.setLayerId(result::key::layerType::PointF);
//                            V[0]=*(float*)(ptr+6+unitLength*i);
//                            BeamStressB->data[tmpKey]=tmpValues1;
//                        }  // if (bStation) ...
//                    }
//                }
//                break;
            case NastranOp2::SResultElementType::ELAS1 :
            case NastranOp2::SResultElementType::ELAS2 :
            case NastranOp2::SResultElementType::ELAS3 ://not tested
                if(bStrain_Flag)
                {
                    frgOesData[0]=*(float*)(ptr+1); //e
                }
                break;
            case NastranOp2::SResultElementType::BAR :  //tested
                if(bStrain_Flag)
                {
                    frgOesData[0] = *(float*)(ptr+ 1);  //sa1
                    frgOesData[1] = *(float*)(ptr+ 2);  //sa2
                    frgOesData[2] = *(float*)(ptr+ 3);  //sa3
                    frgOesData[3] = *(float*)(ptr+ 4);  //sa4
                    frgOesData[4] = *(float*)(ptr+ 5);  //axial
                    frgOesData[5] = *(float*)(ptr+ 6);  //sa maximum
                    frgOesData[6] = *(float*)(ptr+ 7);  //sa minimum
                    //frgOesData[7] = *(float*)(ptr+ 8);//skipping safety margin in tension
                    frgOesData[8] = *(float*)(ptr+ 9);  //sb1
                    frgOesData[9] = *(float*)(ptr+ 10); //sb2
                    frgOesData[10] = *(float*)(ptr+ 11);//sb3
                    frgOesData[11] = *(float*)(ptr+ 12);//sb4
                    frgOesData[12] = *(float*)(ptr+ 13);//sb maximum
                    frgOesData[13] = *(float*)(ptr+ 14);//sb minimum
                    //frgOesData[14] = *(float*)(ptr+ 15);//skipping safety margin in compression
                }
                break;
            case NastranOp2::SResultElementType::BUSH1D ://not tested
                frgOesData[0] = *(float*)(ptr+ 1);  //element force
                frgOesData[1] = *(float*)(ptr+ 2);  //axial displacement
                frgOesData[2] = *(float*)(ptr+ 3);  //axial velocity
                frgOesData[3] = *(float*)(ptr+ 4);  //axial stress
                frgOesData[4] = *(float*)(ptr+ 5);  //axial strain
                frgOesData[5] = *(float*)(ptr+ 6);  //plastic strain
                frgOesData[6] = *(float*)(ptr+ 7);  //failed element flag
                break;
            case NastranOp2::SResultElementType::TETRA :
            case NastranOp2::SResultElementType::HEXA :
            case NastranOp2::SResultElementType::PENTA :
                if (bStrain_Flag)
                {
//                    for (i=0; i<nbrPoints; i++)
//                    {
                    pointId=ptr[4+unitLength*0];
                    frgOesData[0]=*(float*)(ptr+5+unitLength*0);  //normal in x
                    frgOesData[1]=*(float*)(ptr+6+unitLength*0);  //shear in xy
                    frgOesData[2]=*(float*)(ptr+13+unitLength*0); //normal in y
                    frgOesData[3]=*(float*)(ptr+14+unitLength*0); //shear in yz
                    frgOesData[4]=*(float*)(ptr+19+unitLength*0); //normal in z
                    frgOesData[5]=*(float*)(ptr+20+unitLength*0); //shear in zx
                    frgOesData[6]=*(float*)(ptr+7+unitLength*0);  //1st principal strain
                    frgOesData[7]=*(float*)(ptr+15+unitLength*0); //2nd principal strain
                    frgOesData[8]=*(float*)(ptr+21+unitLength*0); //3rd principal strain
//                    }
                    //insert strain data in treestructure
                    fStressStrainComponents[0] = frgOesData[0];//sigma x
                    fStressStrainComponents[1] = frgOesData[2];//sigma y
                    fStressStrainComponents[2] = frgOesData[4];//sigma z
                    fStressStrainComponents[3] = frgOesData[1];//tow xy
                    fStressStrainComponents[4] = frgOesData[3];//tow yz
                    fStressStrainComponents[5] = frgOesData[5];//tow xz
                    oElementData.SetElementStrains(oElementData.SearchandReturn_CurrentNode(elemId),miSubCaseId,3,fStressStrainComponents,0.0,0.0,0.0,0.0,frgOesData[6],frgOesData[7],frgOesData[8]);
                }
                else
                {
//                    for (i=0; i<nbrPoints; i++)
//                    {
                    pointId=ptr[4+unitLength*0];
                    frgOesData[0]=*(float*)(ptr+5+unitLength*0);  //normal in x
                    frgOesData[1]=*(float*)(ptr+6+unitLength*0);  //shear in xy
                    frgOesData[2]=*(float*)(ptr+13+unitLength*0); //normal in y
                    frgOesData[3]=*(float*)(ptr+14+unitLength*0); //shear in yz
                    frgOesData[4]=*(float*)(ptr+19+unitLength*0); //normal in z
                    frgOesData[5]=*(float*)(ptr+20+unitLength*0); //shear in zx
                    frgOesData[6]=*(float*)(ptr+7+unitLength*0);  //1st principal strain
                    frgOesData[7]=*(float*)(ptr+15+unitLength*0); //2nd principal strain
                    frgOesData[8]=*(float*)(ptr+21+unitLength*0); //3rd principal strain
//                    }
                    //insert strain data in treestructure
                    fStressStrainComponents[0] = frgOesData[0];//sigma x
                    fStressStrainComponents[1] = frgOesData[2];//sigma y
                    fStressStrainComponents[2] = frgOesData[4];//sigma z
                    fStressStrainComponents[3] = frgOesData[1];//tow xy
                    fStressStrainComponents[4] = frgOesData[3];//tow yz
                    fStressStrainComponents[5] = frgOesData[5];//tow xz
                    oElementData.SetElementStresses(oElementData.SearchandReturn_CurrentNode(elemId),miSubCaseId,3,fStressStrainComponents,0.0,0.0,0.0,0.0,frgOesData[6],frgOesData[7],frgOesData[8]);
                }
                break;
//            case NastranOp2::SResultElementType::BEND :
//                if (BeamStressA)
//                {
//                    tmpElement=dynamic_cast<const element*>(DB.getElement(elemId));
//                    if (tmpElement==NULL)
//                    {
//                        cout << "No Element with ID = " << elemId << " found in the DataBase." << endl ;
////                        DB.writeSummary(cout);
//                        errThrow("op2::scanOp2_finalize_OES","cannot read CBEND element forces");
//                    }
//                    V=tmpValues1.getVals();
//                    for (i=0; i<nbrPoints; i++)
//                    {
//                        tmpKey.setNodeId(ptr[1+unitLength*i]);
//                        tmpKey.setLayerId(result::key::layerType::PointC);
//                        V[0]=*(float*)(ptr+3+unitLength*i);
//                        BeamStressA->data[tmpKey]=tmpValues1;
//                        tmpKey.setLayerId(result::key::layerType::PointD);
//                        V[0]=*(float*)(ptr+4+unitLength*i);
//                        BeamStressA->data[tmpKey]=tmpValues1;
//                        tmpKey.setLayerId(result::key::layerType::PointE);
//                        V[0]=*(float*)(ptr+5+unitLength*i);
//                        BeamStressA->data[tmpKey]=tmpValues1;
//                        tmpKey.setLayerId(result::key::layerType::PointF);
//                        V[0]=*(float*)(ptr+6+unitLength*i);
//                        BeamStressA->data[tmpKey]=tmpValues1;
//                    }
//                }
//                break;
            case NastranOp2::SResultElementType::QUAD4 :
            case NastranOp2::SResultElementType::TRIA3 ://tested
                if(bStrain_Flag)
                {
                    frgOesData[0] = *(float*)(ptr+ 1); //z1=fibre distance
                    frgOesData[1] = *(float*)(ptr+ 2); //Normal in x at Z1
                    frgOesData[2] = *(float*)(ptr+ 3); //Normal in y at Z1
                    frgOesData[3] = *(float*)(ptr+ 4); //shear in xy at z1
                    frgOesData[4] = *(float*)(ptr+ 5); //theta(shear angle) at z1
                    frgOesData[5] = *(float*)(ptr+ 6); //major principal at z1
                    frgOesData[6] = *(float*)(ptr+ 7); //minor principal at z1
                    frgOesData[7] = *(float*)(ptr+ 8); //maximum shear at z1
                    frgOesData[8] = *(float*)(ptr+ 9); //z2=fibre distance
                    frgOesData[9] = *(float*)(ptr+10); //Normal in x at Z2
                    frgOesData[10] = *(float*)(ptr+11); //Normal in y at Z2
                    frgOesData[11] = *(float*)(ptr+12); //shear in xy at z2
                    frgOesData[12] = *(float*)(ptr+13); //theta(shear angle) at z2
                    frgOesData[13] = *(float*)(ptr+14); //major principal at z2
                    frgOesData[14] = *(float*)(ptr+15); //minor principal at z2
                    frgOesData[15] = *(float*)(ptr+16); //maximum shear at z2

                    fStressStrainComponents[0] = frgOesData[1];//sigma x
                    fStressStrainComponents[1] = frgOesData[2];//sigma y
                    fStressStrainComponents[3] = frgOesData[3];//tow xy
                    oElementData.SetElementStrains(oElementData.SearchandReturn_CurrentNode(elemId),miSubCaseId,2,fStressStrainComponents,frgOesData[4],frgOesData[5],frgOesData[6],frgOesData[7],0.0,0.0,0.0);
                }
                else
                {
                    frgOesData[0] = *(float*)(ptr+ 1); //z1=fibre distance
                    frgOesData[1] = *(float*)(ptr+ 2); //Normal in x at Z1
                    frgOesData[2] = *(float*)(ptr+ 3); //Normal in y at Z1
                    frgOesData[3] = *(float*)(ptr+ 4); //shear in xy at z1
                    frgOesData[4] = *(float*)(ptr+ 5); //theta(shear angle) at z1
                    frgOesData[5] = *(float*)(ptr+ 6); //major principal at z1
                    frgOesData[6] = *(float*)(ptr+ 7); //minor principal at z1
                    frgOesData[7] = *(float*)(ptr+ 8); //maximum shear at z1
                    frgOesData[8] = *(float*)(ptr+ 9); //z2=fibre distance
                    frgOesData[9] = *(float*)(ptr+10); //Normal in x at Z2
                    frgOesData[10] = *(float*)(ptr+11); //Normal in y at Z2
                    frgOesData[11] = *(float*)(ptr+12); //shear in xy at z2
                    frgOesData[12] = *(float*)(ptr+13); //theta(shear angle) at z2
                    frgOesData[13] = *(float*)(ptr+14); //major principal at z2
                    frgOesData[14] = *(float*)(ptr+15); //minor principal at z2
                    frgOesData[15] = *(float*)(ptr+16); //maximum shear at z2

                    fStressStrainComponents[0] = frgOesData[1];//sigma x
                    fStressStrainComponents[1] = frgOesData[2];//sigma y
                    fStressStrainComponents[3] = frgOesData[3];//tow xy
                    oElementData.SetElementStresses(oElementData.SearchandReturn_CurrentNode(elemId),miSubCaseId,2,fStressStrainComponents,frgOesData[4],frgOesData[5],frgOesData[6],frgOesData[7],0.0,0.0,0.0);
                }
                break;
            case NastranOp2::SResultElementType::QUAD4LC :
            case NastranOp2::SResultElementType::QUAD8LC :
            case NastranOp2::SResultElementType::TRIA3LC :
            case NastranOp2::SResultElementType::TRIA6LC :
            case NastranOp2::SResultElementType::QUADRLC :
            case NastranOp2::SResultElementType::TRIARLC ://not tested
                if(bStrain_Flag)
                {
                    frgOesData[0] = *(float*)(ptr+ 1); //lamina number
                    frgOesData[1] = *(float*)(ptr+ 2); //normal-1
                    frgOesData[2] = *(float*)(ptr+ 3); //normal-2
                    frgOesData[3] = *(float*)(ptr+ 4); //shear-12
                    frgOesData[4] = *(float*)(ptr+ 5); //shear-1z
                    frgOesData[5] = *(float*)(ptr+ 6); //shear-2z
                    frgOesData[6] = *(float*)(ptr+ 7); //shear angle
                    frgOesData[7] = *(float*)(ptr+ 8); //major principal
                    frgOesData[8] = *(float*)(ptr+ 9); //minor principal
                    frgOesData[9] = *(float*)(ptr+10); //vonmises or maximum shear
                }
                break;
            case NastranOp2::SResultElementType::BUSH :
            case NastranOp2::SResultElementType::CFAST :
                frgOesData[0] = *(float*)(ptr+ 1); //translation x
                frgOesData[1] = *(float*)(ptr+ 2); //translation y
                frgOesData[2] = *(float*)(ptr+ 3); //translation z
                frgOesData[3] = *(float*)(ptr+ 4); //rotation x
                frgOesData[4] = *(float*)(ptr+ 5); //rotation y
                frgOesData[5] = *(float*)(ptr+ 6); //rotation z
                break;
//            case NastranOp2::SResultElementType::QUAD8 :
//            case NastranOp2::SResultElementType::TRIAR :
//            case NastranOp2::SResultElementType::TRIA6 :
//            case NastranOp2::SResultElementType::QUADR :
//            case NastranOp2::SResultElementType::QUAD144 :
//                for (i=0; i<nbrPoints; i++)
//                {
//                    pointId=ptr[2+unitLength*i];
//                    if (i==0)
//                        tmpKey.setNodeId(common::node::nodeIds::NONE);
//                    else
//                        tmpKey.setNodeId(pointId);
//                    if (f_mcid)
//                    {
//                        if (DB.elementIsLaminated(elemId))
//                            tmpValues6.setCid(result::values::cidType::LamCS);
//                        else
//                            tmpValues6.setCid(result::values::cidType::MatCS);
//                    }
//                    else
//                        tmpValues6.setCid(result::values::cidType::ElemCS);
//                    V=tmpValues6.getVals();
//                    V[2]=0.0;
//                    V[4]=0.0;
//                    V[5]=0.0;
//
//                    if (tmpStress)
//                    {
//                        V[0]=*(float*)(ptr+ 4+unitLength*i);
//                        V[1]=*(float*)(ptr+ 5+unitLength*i);
//                        V[3]=*(float*)(ptr+ 6+unitLength*i);
//                        if (f_strain)
//                        {
//                            V[3]*=0.5;
//                            if (SCODE==11)
//                            {
//                                tmpKey.setLayerId(result::key::layerType::NONE);
//                                tmpStress->data[tmpKey]=tmpValues6;
//                            }
//                            else
//                            {
//                                tmpKey.setLayerId(result::key::layerType::Z1);
//                                tmpStress->data[tmpKey]=tmpValues6;
//                            }
//                        }
//                        else
//                        {
//                            tmpKey.setLayerId(result::key::layerType::Z1);
//                            tmpStress->data[tmpKey]=tmpValues6;
//                        }
//                    }
//
//                    V[0]=*(float*)(ptr+12+unitLength*i);
//                    V[1]=*(float*)(ptr+13+unitLength*i);
//                    V[3]=*(float*)(ptr+14+unitLength*i);
//                    if (f_strain)
//                    {
//                        V[3]*=0.5;
//                        if (SCODE==11)
//                        {
//                            tmpKey.setLayerId(result::key::layerType::NONE);
//                            V[0]*=-1.0;
//                            V[1]*=-1.0;
//                            V[3]*=-1.0;
//                            if (tmpCurvature)
//                                tmpCurvature->data[tmpKey]=tmpValues6;
//                        }
//                        else
//                        {
//                            tmpKey.setLayerId(result::key::layerType::Z2);
//                            if (tmpStress)
//                                tmpStress->data[tmpKey]=tmpValues6;
//                        }
//                    }
//                    else
//                    {
//                        tmpKey.setLayerId(result::key::layerType::Z2);
//                        if (tmpStress)
//                            tmpStress->data[tmpKey]=tmpValues6;
//                    }
//                }
//                break;
//            case NastranOp2::SResultElementType::TETRANL :
//            case NastranOp2::SResultElementType::PENTANL :
//            case NastranOp2::SResultElementType::HEXANL :
//                tmpKey.setLayerId(result::key::layerType::NONE);
//                if (f_mcid)
//                    tmpValues6.setCid(result::values::cidType::MatCS);
//                else
//                    tmpValues6.setCid(result::values::cidType::ElemCS);
//                util::convertOp2WordsToString(&ptr[1],1,cType);
//                for (i=0; i<nbrPoints; i++)
//                {
//                    pointId=ptr[2+unitLength*i];
//                    if (cType=="GRID")
//                    {
//                        if (pointId==0)
//                            tmpKey.setNodeId(common::node::nodeIds::NONE);
//                        else
//                            tmpKey.setNodeId(pointId);
//                    }
//                    else
//                    {
//                        if (pointId<=0||pointId>=100)
//                        {
//                            errThrow("op2::scanOp2_finalize_OES",
//                                     "invalid Gauss point ID.");
//                        }
//                        else
//                            tmpKey.setNodeId(-100-pointId);
//                    }
//
//                    if (stressNL)
//                    {
//                        V=tmpValues6.getVals();
//                        V[0]=*(float*)(ptr+ 3+unitLength*i);
//                        V[1]=*(float*)(ptr+ 4+unitLength*i);
//                        V[2]=*(float*)(ptr+ 5+unitLength*i);
//                        V[3]=*(float*)(ptr+ 6+unitLength*i);
//                        V[4]=*(float*)(ptr+ 7+unitLength*i);
//                        V[5]=*(float*)(ptr+ 8+unitLength*i);
//                        stressNL->data[tmpKey]=tmpValues6;
//                    }
//
//                    if (strainNL)
//                    {
//                        V=tmpValues6.getVals();
//                        V[0]=*(float*)(ptr+12+unitLength*i);
//                        V[1]=*(float*)(ptr+13+unitLength*i);
//                        V[2]=*(float*)(ptr+14+unitLength*i);
//                        V[3]=0.5f*(*(float*)(ptr+15+unitLength*i));
//                        V[4]=0.5f*(*(float*)(ptr+16+unitLength*i));
//                        V[5]=0.5f*(*(float*)(ptr+17+unitLength*i));
//                        strainNL->data[tmpKey]=tmpValues6;
//                    }
//
//                    if (strainNLplastic)
//                    {
//                        V=tmpValues1.getVals();
//                        V[0]=*(float*)(ptr+10+unitLength*i);
//                        strainNLplastic->data[tmpKey]=tmpValues1;
//                    }
//
//                    if (strainNLcreep)
//                    {
//                        V=tmpValues1.getVals();
//                        V[0]=*(float*)(ptr+11+unitLength*i);
//                        strainNLcreep->data[tmpKey]=tmpValues1;
//                    }
//                }
//                break;
            case NastranOp2::SResultElementType::GAP :
                frgOesData[0] = *(float*)(ptr+ 1); //fx
                frgOesData[1] = *(float*)(ptr+ 2); //sfy
                frgOesData[2] = *(float*)(ptr+ 3); //sfz
                frgOesData[3] = *(float*)(ptr+ 4); //u
                frgOesData[4] = *(float*)(ptr+ 5); //v
                frgOesData[5] = *(float*)(ptr+ 6); //w
                frgOesData[6] = *(float*)(ptr+ 7); //sv
                frgOesData[7] = *(float*)(ptr+ 8); //sw
                break;
            case NastranOp2::SResultElementType::GAPNL :
                frgOesData[0] = *(float*)(ptr+ 1); //comp x
                frgOesData[1] = *(float*)(ptr+ 2); //shear in y
                frgOesData[2] = *(float*)(ptr+ 3); //shear in z
                frgOesData[3] = *(float*)(ptr+ 4); //axial in u
                frgOesData[4] = *(float*)(ptr+ 5); //shear in v
                frgOesData[5] = *(float*)(ptr+ 6); //shear in w
                frgOesData[6] = *(float*)(ptr+ 7); //slip in v
                frgOesData[7] = *(float*)(ptr+ 8); //slip in w
                frgOesData[8] = *(float*)(ptr+ 9); //form1
                frgOesData[9] = *(float*)(ptr+10); //form2
                break;
            case NastranOp2::SResultElementType::TUBENL :
            case NastranOp2::SResultElementType::RODNL :
            case NastranOp2::SResultElementType::CONRODNL :
                frgOesData[0] = *(float*)(ptr+ 1); //axial stress
                frgOesData[1] = *(float*)(ptr+ 2); //equivalent stress
                frgOesData[2] = *(float*)(ptr+ 3); //total strain
                frgOesData[3] = *(float*)(ptr+ 4); //effective plastic strain
                frgOesData[4] = *(float*)(ptr+ 5); //effective creep strain
                frgOesData[5] = *(float*)(ptr+ 6); //linear torional stress
                break;
//            case NastranOp2::SResultElementType::TRIA3NL :
//            case NastranOp2::SResultElementType::QUAD4NL :
//            case NastranOp2::SResultElementType::QUADRNL :
//            case NastranOp2::SResultElementType::TRIARNL :
//                tmpKey.setNodeId(common::node::nodeIds::NONE);
//                if (f_mcid)
//                    tmpValues6.setCid(result::values::cidType::MatCS);
//                else
//                    tmpValues6.setCid(result::values::cidType::ElemCS);
//                if (iNUMWDE==13)
//                {
//                    tmpKey.setLayerId(result::key::layerType::NONE);
//                    if (stressNL)
//                    {
//                        V=tmpValues6.getVals();
//                        V[0]=*(float*)(ptr+ 2);
//                        V[1]=*(float*)(ptr+ 3);
//                        V[2]=*(float*)(ptr+ 4);
//                        V[3]=*(float*)(ptr+ 5);
//                        V[4]=0.0;
//                        V[5]=0.0;
//                        stressNL->data[tmpKey]=tmpValues6;
//                    }
//                    if (strainNL)
//                    {
//                        V=tmpValues6.getVals();
//                        V[0]=*(float*)(ptr+ 9);
//                        V[1]=*(float*)(ptr+10);
//                        V[2]=*(float*)(ptr+11);
//                        V[3]=0.5f*(*(float*)(ptr+12));
//                        V[4]=0.0;
//                        V[5]=0.0;
//                        strainNL->data[tmpKey]=tmpValues6;
//                    }
//                    if (strainNLplastic)
//                    {
//                        V=tmpValues1.getVals();
//                        V[0]=*(float*)(ptr+ 7);
//                        strainNLplastic->data[tmpKey]=tmpValues1;
//                    }
//                    if (strainNLcreep)
//                    {
//                        V=tmpValues1.getVals();
//                        V[0]=*(float*)(ptr+ 8);
//                        strainNLcreep->data[tmpKey]=tmpValues1;
//                    }
//                }
//                else if (iNUMWDE==25)
//                {
//                    for (j=0; j<2; j++)
//                    {
//                        if (j==0)
//                        {
//                            tmpKey.setLayerId(result::key::layerType::Z1);
//                            shiftJ=0;
//                        }
//                        else
//                        {
//                            tmpKey.setLayerId(result::key::layerType::Z2);
//                            shiftJ=12;
//                        }
//
//                        if (stressNL)
//                        {
//                            V=tmpValues6.getVals();
//                            V[0]=*(float*)(ptr+shiftJ+ 2);
//                            V[1]=*(float*)(ptr+shiftJ+ 3);
//                            V[2]=*(float*)(ptr+shiftJ+ 4);
//                            V[3]=*(float*)(ptr+shiftJ+ 5);
//                            V[4]=0.0;
//                            V[5]=0.0;
//                            stressNL->data[tmpKey]=tmpValues6;
//                        }
//                        if (strainNL)
//                        {
//                            V=tmpValues6.getVals();
//                            V[0]=*(float*)(ptr+shiftJ+ 9);
//                            V[1]=*(float*)(ptr+shiftJ+10);
//                            V[2]=*(float*)(ptr+shiftJ+11);
//                            V[3]=0.5f*(*(float*)(ptr+shiftJ+12));
//                            V[4]=0.0;
//                            V[5]=0.0;
//                            strainNL->data[tmpKey]=tmpValues6;
//                        }
//                        if (strainNLplastic)
//                        {
//                            V=tmpValues1.getVals();
//                            V[0]=*(float*)(ptr+shiftJ+ 7);
//                            strainNLplastic->data[tmpKey]=tmpValues1;
//                        }
//                        if (strainNLcreep)
//                        {
//                            V=tmpValues1.getVals();
//                            V[0]=*(float*)(ptr+shiftJ+ 8);
//                            strainNLcreep->data[tmpKey]=tmpValues1;
//                        }
//                    }
//                }
//                else
//                {
//                    errThrow("op2::scanOp2_finalize_OES","invalid value of iNUMWDE");
//                }
//                break;
//            case NastranOp2::SResultElementType::BEAMNL :
//                tmpKey.setNodeId(common::node::nodeIds::NONE);
//                tmpKey.setLayerId(result::key::layerType::NONE);
//                V=tmpValues1.getVals();
//                for (j=0; j<2; j++)
//                {
//                    shiftJ=25*j;
//                    tmpKey.setNodeId(ptr[shiftJ+1]);
//                    for (k=0; k<4; k++)
//                    {
//                        shiftK=6*k;
//                        switch (k)
//                        {
//                        case 0 :
//                            tmpKey.setLayerId(result::key::layerType::PointC);
//                            break;
//                        case 1 :
//                            tmpKey.setLayerId(result::key::layerType::PointD);
//                            break;
//                        case 2 :
//                            tmpKey.setLayerId(result::key::layerType::PointE);
//                            break;
//                        case 3 :
//                            tmpKey.setLayerId(result::key::layerType::PointF);
//                            break;
//                        } ;
//                        if (BeamStressNL)
//                        {
//                            V[0]=*(float*)(ptr+ 3);
//                            BeamStressNL->data[tmpKey]=tmpValues1;
//                        }
//                        if (BeamStrainNL)
//                        {
//                            V[0]=*(float*)(ptr+ 5);
//                            BeamStrainNL->data[tmpKey]=tmpValues1;
//                        }
//                        if (strainNLplastic)
//                        {
//                            V[0]=*(float*)(ptr+ 6);
//                            strainNLplastic->data[tmpKey]=tmpValues1;
//                        }
//                        if (strainNLcreep)
//                        {
//                            V[0]=*(float*)(ptr+ 7);
//                            strainNLcreep->data[tmpKey]=tmpValues1;
//                        }
//                    }
//                }
//                break;
            case NastranOp2::SResultElementType::ELAS1NL :
            case NastranOp2::SResultElementType::ELAS3NL :
                frgOesData[0] = *(float*)(ptr+ 1); //force
                frgOesData[1] = *(float*)(ptr+ 2); //stress
                break;
            case NastranOp2::SResultElementType::BUSHNL :
                frgOesData[0] = *(float*)(ptr+ 1); //Force X
                frgOesData[1] = *(float*)(ptr+ 2); //Force Y
                frgOesData[2] = *(float*)(ptr+ 3); //Force Z
                frgOesData[3] = *(float*)(ptr+ 4); //Stress translational X
                frgOesData[4] = *(float*)(ptr+ 5); //Stress translational Y
                frgOesData[5] = *(float*)(ptr+ 6); //Stress translational Z
                frgOesData[6] = *(float*)(ptr+ 7); //Strain rotational X
                frgOesData[7] = *(float*)(ptr+ 8); //Strain rotational Y
                frgOesData[8] = *(float*)(ptr+ 9); //Strain rotational Z
                frgOesData[9] = *(float*)(ptr+10); //Moment X
                frgOesData[10] = *(float*)(ptr+11); //Moment Y
                frgOesData[11] = *(float*)(ptr+12); //Moment Z
                frgOesData[12] = *(float*)(ptr+13); //Stress rotational X
                frgOesData[13] = *(float*)(ptr+14); //Stress rotational Y
                frgOesData[14] = *(float*)(ptr+15); //Stress rotational Z
                frgOesData[15] = *(float*)(ptr+16); //Strain rotational X
                frgOesData[16] = *(float*)(ptr+17); //Strain rotational Y
                frgOesData[17] = *(float*)(ptr+18); //Strain rotational Z
                break;
            } ;
            ptr+=nbrWords;
        }
        //availability of OES(Element stress/strain) datablock is set to true
        oElementData.SetElemStress_Available(true);
        oElementData.SetElemStrain_Available(true);
        //miSubCaseId value is obtained from strLabel
        oElementData.SetNbrofSubCases(miSubCaseId);//sets total nbr of subcases for all the nodal results
    }
    catch(...)
    {
        //cout<<"NastranOp2::Scan_OES_DataBlock, default exception thrown while scanning oes table\n";
    }
}
