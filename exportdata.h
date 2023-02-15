#ifndef EXPORTDATA_H
#define EXPORTDATA_H
#include "cnode.h"
#include "celement.h"
#include <fstream>
#include <wx/progdlg.h>
class ExportData
{
public:
    ExportData();
    /***writes input mesh information in a text file***/
    //node coordinates
    static void WriteNodeCoordinatesInorder(Snode* pInorderTraverse,std::ofstream &fstrWriteNodeCoordinates);
    static void ExportNodeCoordinates(Cnode &oNodeInfo);
    //element connectivity
    static void WriteElementConnectivityInorder(SBinaryTreeNode* pInorderTraverse,std::ofstream &fstrWriteElementConnectivity);
    static void ExportElementConnectivity(CElement &oElementInfo);
    //applied loads at nodes
    static void WriteNodalAppliedLoadsInorder(Snode* pInorderTraverse,int iSubCaseId,std::ofstream &fstrWriteNodalAppliedLoads);
    static void ExportNodalAppliedLoads(Cnode &oNodeInfo);
    //constraint forces
    static void ExportConstraintForces(Cnode &oNodeInfo);
    static void WriteSpcConstraintForceInorder(Snode* pInorderTraverse,int iSubCaseId,std::ofstream &fstrWriteSpcConstraintForce);
    static void ExportSpcConstraintForce(Cnode &oNodeInfo);
    static void WriteMpcConstraintForceInorder(Snode* pInorderTraverse,int iSubCaseId,std::ofstream &fstrWriteSpcConstraintForce);
    static void ExportMpcConstraintForce(Cnode &oNodeInfo);

    /***writes output results information in a text file***/
    //output nodal displacement
    static void WriteNodalOutputDisplacementInorder(Snode* pInorderTraverse,int iSubCaseId,std::ofstream &fstrWriteNodalOutputDisplacement);
    static void ExportNodalOutputDisplacement(Cnode &oNodeInfo);
    //stress strain
    static void ExportElementStressStrainResults(CElement &oElementInfo);
    static void WriteElementStrainInorder(SBinaryTreeNode* pInorderTraverse,int iSubCaseId,std::ofstream &fstrWriteElementStrain);
    static void ExportElementStrain(CElement &oElementInfo);
    static void WriteElementStressInorder(SBinaryTreeNode* pInorderTraverse,int iSubCaseId,std::ofstream &fstrWriteElementStress);
    static void ExportElementStress(CElement &oElementInfo);
    //output strain energy
    static void WriteOutputStrainEnergyInorder(SBinaryTreeNode* pInorderTraverse,int iSubCaseId,std::ofstream &fstrWriteOutputStrainEnergy);
    static void ExportOutputStrainEnergy(CElement &oElementInfo);
    //output kinetic energy
    static void WriteOutputKineticEnergyInorder(SBinaryTreeNode* pInorderTraverse,int iSubCaseId,std::ofstream &fstrWriteOutputKineticEnergy);
    static void ExportOutputKineticEnergy(CElement &oElementInfo);
    //output energy loss
    static void WriteOutputEnergyLossInorder(SBinaryTreeNode* pInorderTraverse,int iSubCaseId,std::ofstream &fstrWriteOutputEnergyLoss);
    static void ExportOutputEnergyLoss(CElement &oElementInfo);
    //write all data to text file
    static void ExportAllDatatoTextFile(Cnode &oNodeInfo,CElement &oElementInfo);
    static void ExportGraph_StrsStrnXXInorder(SBinaryTreeNode* pInorderTraverse,int iSubCaseId,std::ofstream &fstrWriteStressXX);
    static void ExportGraph_StrsStrnXX(CElement &oElementInfo);
    static void ExportGraph_StrsStrnYYInorder(SBinaryTreeNode* pInorderTraverse,int iSubCaseId,std::ofstream &fstrWriteStressYY);
    static void ExportGraph_StrsStrnYY(CElement &oElementInfo);
    static void ExportGraph_StrsStrnZZInorder(SBinaryTreeNode* pInorderTraverse,int iSubCaseId,std::ofstream &fstrWriteStressZZ);
    static void ExportGraph_StrsStrnZZ(CElement &oElementInfo);
    static void ExportGraph_StrsStrnXYInorder(SBinaryTreeNode* pInorderTraverse,int iSubCaseId,std::ofstream &fstrWriteStressXY);
    static void ExportGraph_StrsStrnXY(CElement &oElementInfo);
    static void ExportGraph_StrsStrnYZInorder(SBinaryTreeNode* pInorderTraverse,int iSubCaseId,std::ofstream &fstrWriteStressYZ);
    static void ExportGraph_StrsStrnYZ(CElement &oElementInfo);
    static void ExportGraph_StrsStrnXZInorder(SBinaryTreeNode* pInorderTraverse,int iSubCaseId,std::ofstream &fstrWriteStressXZ);
    static void ExportGraph_StrsStrnXZ(CElement &oElementInfo);

    static void ExportGraph_StrsStrnShearAngleInorder(SBinaryTreeNode* pInorderTraverse,int iSubCaseId,std::ofstream &fstrWriteStressXX);
    static void ExportGraph_StrsStrnShearAngle(CElement &oElementInfo);
    static void ExportGraph_StrsStrn1stPrincipalInorder(SBinaryTreeNode* pInorderTraverse,int iSubCaseId,std::ofstream &fstrWriteStressYY);
    static void ExportGraph_StrsStrn1stPrincipal(CElement &oElementInfo);
    static void ExportGraph_StrsStrn2ndPrincipalInorder(SBinaryTreeNode* pInorderTraverse,int iSubCaseId,std::ofstream &fstrWriteStressZZ);
    static void ExportGraph_StrsStrn2ndPrincipal(CElement &oElementInfo);
    static void ExportGraph_StrsStrn3rdPrincipalInorder(SBinaryTreeNode* pInorderTraverse,int iSubCaseId,std::ofstream &fstrWriteStressXY);
    static void ExportGraph_StrsStrn3rdPrincipal(CElement &oElementInfo);
    static void ExportGraph_StrsStrnMajorPrincipalInorder(SBinaryTreeNode* pInorderTraverse,int iSubCaseId,std::ofstream &fstrWriteStressYZ);
    static void ExportGraph_StrsStrnMajorPrincipal(CElement &oElementInfo);
    static void ExportGraph_StrsStrnMinorPrincipalInorder(SBinaryTreeNode* pInorderTraverse,int iSubCaseId,std::ofstream &fstrWriteStressXZ);
    static void ExportGraph_StrsStrnMinorPrincipal(CElement &oElementInfo);

    static void ExportGraph_StrsStrnVmisesInorder(SBinaryTreeNode* pInorderTraverse,int iSubCaseId,std::ofstream &fstrWriteStrsStrnvmises);
    static void ExportGraph_StrsStrnVmises(CElement &oElementInfo);
    ~ExportData();
protected:
private:
    static wxProgressDialog* pProgressBar;
};

#endif // EXPORTDATA_H
