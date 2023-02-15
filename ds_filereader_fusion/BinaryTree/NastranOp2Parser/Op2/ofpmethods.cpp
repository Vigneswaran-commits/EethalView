#include "NastranOp2Parser/op2/nastranop2.h"
#include "NastranOp2Parser/Utilities/utilities.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
using namespace std;

//results
#define INVALID_TABLECONTENT     0
#define  OUG     1
#define  OPG     2
#define  OQG     3
#define  OEF     4
#define  OES     5
#define  OGF     6
#define  OEE     7
#define  LAMA    8
#define  OEIGS   9
#define  OGPWG   10
#define  OELOF1  11
#define  OELOP1  12
#define  OEP     13
#define  OGS     14
#define  OMM     15
#define  OGPKE   16

int NastranOp2::GetOFPDataBlockType(vector<int> &ivecHeaderBlock)
{
//    refer pgno 65 in 2012 manual
    try
    {
        //table content = TCODE%1000, e.g., displacements(oug), stresses(oes), etc.
        switch(ivecHeaderBlock.at(1)%1000)
        {
        case 1  : //displacement vector
        case 7  : //eigenvector
        case 10 : //velocity vector
        case 11 : //acceleration vector
        case 14 : //eigenvector(solution set)
        case 15 : //displacement vector(solution set)
        case 16 : //velocity vector(solution set)
        case 17 : //acceleration vector(solution set)
            return OUG;
            break;
        case 2  : //load vector
        case 12 : //nonlinear force vector
            return OPG;
            break;
        case 3  : //spcforce vector
        case 39 : //mpc forces
            return OQG;
            break;
        case 4  : //element force/flux
        case 25 : //composite failure indices
            return OEF;
            break;
        case 5  : //element stress/strain
        case 20 : //stresses at grid point for the CURV module
        case 21 : //strain/curvature at grid points
            return OES;
            break;
        case 19 : //grid point force balance
            return OGF;
            break;
        case 18 : //element strain energy
        case 36 : //element kinetic energy
        case 37 : //element energy loss
            return OEE;
            break;
        case 6  : //eigenvalue summary
            return LAMA;
            break;
        case 9  : //eigenvalue analysis summary
            return OEIGS;
            break;
        case 13 : //grid point weight generator
            return OGPWG;
            break;
        case 22 : //element internal forces and moments
            return OELOF1;
            break;
        case 23 : //summation of element oriented forces on adjacent elements
            return OELOP1;
            break;
        case 24 : //element pressures
            return OEP;
            break;
        case 26 : //grid point stresses(surface)
        case 27 : //grid point stresses(volume--direct)
        case 28 : //grid point stresses(volume--principal)
        case 29 : //element stress discontinuities(surface)
        case 30 : //element stress discontinuities(volume--direct)
        case 31 : //element stress discontinuities(volume--principal)
        case 32 : //grid point stress discontinuities(surface)
        case 33 : //grid point stress discontinuities(volume--direct)
        case 34 : //grid point stress discontinuities(volume--principal)
        case 35 : //grid point stresses(plain strain)
            return OGS;
            break;
        case 38 : //maxmin summary
            return OMM;
            break;
        case 40 : //grid point kinetic energy
            return OGPKE;
            break;
        default  :
            cout<<"Invalid table content value : "<<ivecHeaderBlock.at(1)%1000<<"\n";
            return INVALID_TABLECONTENT;
            break;
        }
    }
    catch(...)
    {
        cout<<"NastranOp2::GetOFPDataBlockType, default exception is thrown while getting ofp data block type\n";
    }
}

void NastranOp2::ScanOp2Record(string &strHeaderName,int iHeaderLength,vector<int> &ivecHeaderBlock,int iTableLength,vector<int> &ivecTableBlock,Cnode& oNodeData,CElement& oElementData)
{
    try
    {
        int iOfpDataBlockType = GetOFPDataBlockType(ivecHeaderBlock);
        switch(iOfpDataBlockType)
        {
        case OUG :
            Scan_OUG_DataBlock(strHeaderName,iHeaderLength,ivecHeaderBlock,iTableLength,ivecTableBlock,oNodeData);//Table of displacements, velocities, accelerations
            break;
        case OES :
            Scan_OES_DataBlock(strHeaderName,iHeaderLength,ivecHeaderBlock,iTableLength,ivecTableBlock,oElementData);//Table of element stresses or strains
            break;
        case OPG :
            Scan_OPG_DataBlock(strHeaderName,iHeaderLength,ivecHeaderBlock,iTableLength,ivecTableBlock,oNodeData);//Table of applied loads at nodes
            break;
        case OEE :
            Scan_OEE_DataBlock(strHeaderName,iHeaderLength,ivecHeaderBlock,iTableLength,ivecTableBlock,oElementData);//Output element energy (strain, kinetic, loss)
            break;
        case OQG :
            Scan_OQG_DataBlock(strHeaderName,iHeaderLength,ivecHeaderBlock,iTableLength,ivecTableBlock,oNodeData);//Table of single or multipoint constraint forces
            break;
        case OGF :
            Scan_OGF_DataBlock(strHeaderName,iHeaderLength,ivecHeaderBlock,iTableLength,ivecTableBlock);//Table of grid point forces
            break;
        default  :
            //cout<<"Invalid Ofp datablock type\n";
            break;
        }
    }
    catch(...)
    {
        cout<<"NastranOp2::ScanOp2Record, default exception is thrown\n";
    }
}

bool NastranOp2::ScanHeaderInformation(vector<int> &ivecHeaderBlock,
                                       string &strTitle,
                                       string &strSubTitle,
                                       string &strLabel,
                                       int &iApproachCode,int &iAnalysisType,int &iDeviceType,
                                       int &iTableCode,int &iTableContent,bool &bSort2_Flag,
                                       bool &bComplex_Flag,bool &bRandom_Flag,
                                       int &iFormatCode,
                                       int &iStressCode,bool &bStrain_Flag,bool &bStrainorCurvature_Flag,
                                       bool &bMaterialCoordSystemId_Flag,
                                       int &iThermal,
                                       string &strSubCaseName,int intIds[2],
                                       float realIds[2],
                                       int &iELTYPE,int &iNUMWDE)
{
    //refer pgno 62 in reference manual 2012
    try
    {
        Utilities::Op2Word_to_String(&ivecHeaderBlock.at(50),32,strTitle,true);
        Utilities::Op2Word_to_String(&ivecHeaderBlock.at(82),32,strSubTitle,true);
        Utilities::Op2Word_to_String(&ivecHeaderBlock.at(114),32,strLabel,true);
        //1. approach code
        iApproachCode = ivecHeaderBlock.at(0);
        iAnalysisType = iApproachCode/10;

        switch(iAnalysisType)
        {
        case 1 : //statics (SOl 101)
//        intIds[0]=ivecHeaderBlock.at(3);
//        intIds[1]=ivecHeaderBlock.at(4);
            strSubCaseName = "Statics\n";
            break;
        case 2 : //modal(SOL 103)
            strSubCaseName = "Normal Modes or Buckling(real eigenvalues)\n";
            break;
        case 6 : //transient(SOL 159)
            strSubCaseName = "Transient\n";
            break;
        case 8 : //post buckling(SOL 105)
            strSubCaseName = "Post Buckling\n";
            break;
        case 10: //nonlinear statics(SOL 106)
            strSubCaseName = "Nonlinear Statics\n";
            break;
        default:
            cout<<"Unknown analysis type, stopped scanning record\n";
            return false;
            break;
        }
        iDeviceType = iApproachCode%10;
        //2. table code
        iTableCode = ivecHeaderBlock.at(1);
        iTableContent = iTableCode%1000;
        if( (iTableCode/1000)&2 != 0 ) bComplex_Flag = true;
        else bComplex_Flag = false;
        if( (iTableCode/1000)&1 != 0 ) bSort2_Flag = true;
        else bSort2_Flag = false;
        if( (iTableCode/1000)&4 != 0 ) bRandom_Flag = true;
        else bRandom_Flag = false;


        if(bComplex_Flag || bRandom_Flag)
        {
            cout<<"Skipping complex or random results\n";
            return false;
        }

        // Special case for grid point forces
        if (iTableContent==19 && iAnalysisType==2)
        {
            intIds[0]=ivecHeaderBlock.at(3);
            intIds[1]=ivecHeaderBlock.at(4);
            realIds[0]==*(float*)(&ivecHeaderBlock.at(15));
            cout<<"subCaseName is sol103"<<intIds[1]<<" "<<realIds[0]<<"\n";
        }

        iELTYPE = ivecHeaderBlock.at(2); //element type id
        iNUMWDE = ivecHeaderBlock.at(9); //number of words per entry in datacard

        //3. format code
        iFormatCode = ivecHeaderBlock.at(8);
        if(iFormatCode != 1)
        {
            cout<<"Skipping complex results\n";
            return false;
        }
        //4. stress code
        iStressCode = ivecHeaderBlock.at(10);
        if( (iStressCode & 2) != 0 ) bStrain_Flag = true;
        else bStrain_Flag = false;
        if( (iStressCode & 4) != 0 ) bStrainorCurvature_Flag = true;
        else bStrainorCurvature_Flag = false;
        if( (iStressCode & 16) != 0 )bMaterialCoordSystemId_Flag = true;
        else bMaterialCoordSystemId_Flag = false;

        iThermal = ivecHeaderBlock.at(22);

        return true;
    }
    catch(...)
    {
        cout<<"NastranOp2::ScanHeaderInformation, default exception thrown while scanning OFP header information\n";
    }
}
