#include "exportdata.h"
#include "cnode.h"
#include "celement.h"
#include "basicglpane.h"
#include <iostream>
#include <fstream>
#include <wx/progdlg.h>
using namespace std;

wxProgressDialog* ExportData::pProgressBar = NULL;
ExportData::ExportData()
{
    //ctor
}

void ExportData::WriteNodeCoordinatesInorder(Snode* pInorderTraverse,ofstream &fstrWriteNodeCoordinates)
{
    if(pInorderTraverse != NULL)
    {
        WriteNodeCoordinatesInorder(pInorderTraverse->pNodeleft,fstrWriteNodeCoordinates);  //Traverse and display leftside Tree nodes
        if(pInorderTraverse->uiIdentification != 0)
        {
            fstrWriteNodeCoordinates<<pInorderTraverse->NodeInfo.NodePosition.iNodeId<<"          "<<
                                    pInorderTraverse->NodeInfo.NodePosition.fXCoordinate<<"          "<<
                                    pInorderTraverse->NodeInfo.NodePosition.fYCoordinate<<"          "<<
                                    pInorderTraverse->NodeInfo.NodePosition.fZCoordinate<<"\n";
        }
        WriteNodeCoordinatesInorder(pInorderTraverse->pNoderight,fstrWriteNodeCoordinates); //Traverse and display Rightside Tree nodes
    }
}

void ExportData::ExportNodeCoordinates(Cnode &oNodeInfo)
{
    ofstream fstrWriteNodeCoordinates("Node Coordinates.text",ios::out);
    if(fstrWriteNodeCoordinates.is_open())
    {
        if(!BasicGLPane::GetAutoRotation())
            pProgressBar = new wxProgressDialog(wxT("Exporting Node Coordinates.."),wxT("please wait.."),100,NULL,wxPD_AUTO_HIDE | wxPD_APP_MODAL);
        fstrWriteNodeCoordinates<<"Node Coordinates :\n";
        fstrWriteNodeCoordinates<<"No of Nodes : "<<oNodeInfo.GetNoofNodes()<<"\n";
        fstrWriteNodeCoordinates<<"NodeId          XCoordinate          YCoordinate          ZCoordinate\n";
        WriteNodeCoordinatesInorder(oNodeInfo.GetRootNode(),fstrWriteNodeCoordinates);
        fstrWriteNodeCoordinates.close();
        if(!BasicGLPane::GetAutoRotation())
            pProgressBar->Update(100,wxT(""),NULL);
    }
    else
    {
        cout<<"unable to open file for writing node coordinates\n";
    }
}

void ExportData::WriteElementConnectivityInorder(SBinaryTreeNode* pInorderTraverse,std::ofstream &fstrWriteElementConnectivity)
{
    if(pInorderTraverse != NULL)
    {
        WriteElementConnectivityInorder(pInorderTraverse->pNodeleft,fstrWriteElementConnectivity);  //Traverse and display leftside Tree nodes
        if(pInorderTraverse->uiIdentification != 0)
        {
            fstrWriteElementConnectivity<<pInorderTraverse->uiIdentification<<"          "<<
                                        pInorderTraverse->ElementInfo.iElementType<<"          ";
            for(int iIter = 0; iIter < pInorderTraverse->ElementInfo.ivecElementConnectivity.size(); iIter++)
                fstrWriteElementConnectivity<<pInorderTraverse->ElementInfo.ivecElementConnectivity[iIter].iNodeId<<"          ";
            fstrWriteElementConnectivity<<"\n";
        }
        WriteElementConnectivityInorder(pInorderTraverse->pNoderight,fstrWriteElementConnectivity); //Traverse and display Rightside Tree nodes
    }
}

void ExportData::ExportElementConnectivity(CElement &oElementInfo)
{
    ofstream fstrWriteElementConnectivity("Element Connectivity.text",ios::out);
    if(fstrWriteElementConnectivity.is_open())
    {
        if(!BasicGLPane::GetAutoRotation())
            pProgressBar = new wxProgressDialog(wxT("Exporting Element Connectivity"),wxT("please wait.."),100,NULL,wxPD_AUTO_HIDE | wxPD_APP_MODAL);
        fstrWriteElementConnectivity<<"Element Connectivity Nodes :\n";
        for(int iIter = 0; iIter < oElementInfo.NumberofElementTypes(); iIter++)
        {
            if(oElementInfo.ElementName(iIter) != NULL)
                fstrWriteElementConnectivity<<"No of "<<oElementInfo.ElementName(iIter)<<" Elements : "<<oElementInfo.ElementStrength(iIter)<<"\n";
            else
                fstrWriteElementConnectivity<<"No of Unknown Elements : "<<oElementInfo.ElementStrength(iIter)<<"\n";
        }
        fstrWriteElementConnectivity<<"ElemId          ElemTypeId          Element Connectivity Nodes\n";
        WriteElementConnectivityInorder(oElementInfo.GetRootNode(),fstrWriteElementConnectivity);
        fstrWriteElementConnectivity.close();
        if(!BasicGLPane::GetAutoRotation())
            pProgressBar->Update(100,wxT(""),NULL);
    }
    else
    {
        cout<<"unable to open file for writing node coordinates\n";
    }
}

void ExportData::WriteNodalAppliedLoadsInorder(Snode* pInorderTraverse,int iSubCaseId,std::ofstream &fstrWriteNodalAppliedLoads)
{
    if(pInorderTraverse != NULL)
    {
        WriteNodalAppliedLoadsInorder(pInorderTraverse->pNodeleft,iSubCaseId,fstrWriteNodalAppliedLoads);  //Traverse and display leftside Tree nodes
        if(pInorderTraverse->uiIdentification != 0)
        {
            fstrWriteNodalAppliedLoads<<pInorderTraverse->NodeInfo.NodePosition.iNodeId                        <<"          "<<
                                      pInorderTraverse->NodeInfo.vecNodalAppliedLoad[iSubCaseId].iResultType                  <<"          "<<
                                      pInorderTraverse->NodeInfo.vecNodalAppliedLoad[iSubCaseId].fAppliedForce_Direction1     <<"          "<<
                                      pInorderTraverse->NodeInfo.vecNodalAppliedLoad[iSubCaseId].fAppliedForce_Direction2     <<"          "<<
                                      pInorderTraverse->NodeInfo.vecNodalAppliedLoad[iSubCaseId].fAppliedForce_Direction3     <<"          "<<
                                      pInorderTraverse->NodeInfo.vecNodalAppliedLoad[iSubCaseId].fAppliedMoment_Direction1    <<"          "<<
                                      pInorderTraverse->NodeInfo.vecNodalAppliedLoad[iSubCaseId].fAppliedMoment_Direction2    <<"          "<<
                                      pInorderTraverse->NodeInfo.vecNodalAppliedLoad[iSubCaseId].fAppliedMoment_Direction3    <<"\n";
        }
        WriteNodalAppliedLoadsInorder(pInorderTraverse->pNoderight,iSubCaseId,fstrWriteNodalAppliedLoads); //Traverse and display Rightside Tree nodes
    }
}

void ExportData::ExportNodalAppliedLoads(Cnode &oNodeInfo)
{
    ofstream fstrWriteNodalAppliedLoads("Applied Loads in Nodes.text",ios::out);
    if(fstrWriteNodalAppliedLoads.is_open())
    {
        if(!BasicGLPane::GetAutoRotation())
            pProgressBar = new wxProgressDialog(wxT("Exporting Applied Loads in Nodes"),wxT("please wait.."),100,NULL,wxPD_AUTO_HIDE | wxPD_APP_MODAL);
        fstrWriteNodalAppliedLoads<<"Applied Loads in Nodes :\n";
        if(oNodeInfo.NodalAppliedLoadPresence() == true)
        {
            for(int iIter = 0; iIter < oNodeInfo.GetNbrofSubCases(); iIter++)
            {
                fstrWriteNodalAppliedLoads<<"SUBCASE = "<<iIter + 1<<":\n";
                fstrWriteNodalAppliedLoads<<"NodeId          PointType          ForceinDirection1          ForceinDirection2          ForceinDirection3          MomentinDirection1          MomentinDirection2          MomentinDirection3\n";
                WriteNodalAppliedLoadsInorder(oNodeInfo.GetRootNode(),iIter,fstrWriteNodalAppliedLoads);
            }
        }
        else
        {
            fstrWriteNodalAppliedLoads<<"\nNodal Applied Loads Data is not available.\n";
        }
        fstrWriteNodalAppliedLoads.close();
        if(!BasicGLPane::GetAutoRotation())
            pProgressBar->Update(100,wxT(""),NULL);
    }
    else
    {
        cout<<"unable to open file for writing applied Loads in Nodes\n";
    }
}

void ExportData::ExportConstraintForces(Cnode &oNodeInfo)
{
    ExportSpcConstraintForce(oNodeInfo);
    ExportMpcConstraintForce(oNodeInfo);
}

void ExportData::WriteSpcConstraintForceInorder(Snode* pInorderTraverse,int iSubCaseId,std::ofstream &fstrWriteSpcConstraintForce)
{
    if(pInorderTraverse != NULL)
    {
        WriteSpcConstraintForceInorder(pInorderTraverse->pNodeleft,iSubCaseId,fstrWriteSpcConstraintForce);  //Traverse and display leftside Tree nodes
        if(pInorderTraverse->uiIdentification != 0)
        {
            fstrWriteSpcConstraintForce<<pInorderTraverse->NodeInfo.NodePosition.iNodeId                             <<"          "<<
                                       pInorderTraverse->NodeInfo.vecConstraintForce[iSubCaseId].iResultType                        <<"          "<<
                                       pInorderTraverse->NodeInfo.vecConstraintForce[iSubCaseId].fSpcConstraintForce_Direction1     <<"          "<<
                                       pInorderTraverse->NodeInfo.vecConstraintForce[iSubCaseId].fSpcConstraintForce_Direction2     <<"          "<<
                                       pInorderTraverse->NodeInfo.vecConstraintForce[iSubCaseId].fSpcConstraintForce_Direction3     <<"          "<<
                                       pInorderTraverse->NodeInfo.vecConstraintForce[iSubCaseId].fSpcConstraintMoment_Direction1    <<"          "<<
                                       pInorderTraverse->NodeInfo.vecConstraintForce[iSubCaseId].fSpcConstraintMoment_Direction2    <<"          "<<
                                       pInorderTraverse->NodeInfo.vecConstraintForce[iSubCaseId].fSpcConstraintMoment_Direction3    <<"\n";
        }
        WriteSpcConstraintForceInorder(pInorderTraverse->pNoderight,iSubCaseId,fstrWriteSpcConstraintForce); //Traverse and display Rightside Tree nodes
    }
}

void ExportData::ExportSpcConstraintForce(Cnode &oNodeInfo)
{
    ofstream fstrWriteSpcConstraintForce("Single-Point Constraint Forces.text",ios::out);
    if(fstrWriteSpcConstraintForce.is_open())
    {
        if(!BasicGLPane::GetAutoRotation())
            pProgressBar = new wxProgressDialog(wxT("Exporting Single-Point Constraint Forces"),wxT("please wait.."),100,NULL,wxPD_AUTO_HIDE | wxPD_APP_MODAL);
        fstrWriteSpcConstraintForce<<"Single-Point Constraint Forces in Nodes :\n";
        if(oNodeInfo.SpcForcePresence() == true)
        {
            for(int iIter = 0; iIter < oNodeInfo.GetNbrofSubCases(); iIter++)
            {
                fstrWriteSpcConstraintForce<<"SUBCASE = "<<iIter + 1<<":\n";
                fstrWriteSpcConstraintForce<<"NodeId          PointType          ConstraintForce_Direction1          ConstraintForce_Direction2          ConstraintForce_Direction3          ConstraintMoment_Direction1          ConstraintMoment_Direction2          ConstraintMoment_Direction3\n";
                WriteSpcConstraintForceInorder(oNodeInfo.GetRootNode(),iIter,fstrWriteSpcConstraintForce);
            }
        }
        else
        {
            fstrWriteSpcConstraintForce<<"\nSingle-Point Constraint Force Data is not available.\n";
        }
        fstrWriteSpcConstraintForce.close();
        if(!BasicGLPane::GetAutoRotation())
            pProgressBar->Update(100,wxT(""),NULL);
    }
    else
    {
        cout<<"unable to open file for writing Single-Point Constraint Forces\n";
    }
}

void ExportData::WriteMpcConstraintForceInorder(Snode* pInorderTraverse,int iSubCaseId,std::ofstream &fstrWriteMpcConstraintForce)
{
    if(pInorderTraverse != NULL)
    {
        WriteMpcConstraintForceInorder(pInorderTraverse->pNodeleft,iSubCaseId,fstrWriteMpcConstraintForce);  //Traverse and display leftside Tree nodes
        if(pInorderTraverse->uiIdentification != 0)
        {
            fstrWriteMpcConstraintForce<<pInorderTraverse->NodeInfo.NodePosition.iNodeId                                                    <<"          "<<
                                       pInorderTraverse->NodeInfo.vecConstraintForce[iSubCaseId].iResultType                        <<"          "<<
                                       pInorderTraverse->NodeInfo.vecConstraintForce[iSubCaseId].fMpcConstraintForce_Direction1     <<"          "<<
                                       pInorderTraverse->NodeInfo.vecConstraintForce[iSubCaseId].fMpcConstraintForce_Direction2     <<"          "<<
                                       pInorderTraverse->NodeInfo.vecConstraintForce[iSubCaseId].fMpcConstraintForce_Direction3     <<"          "<<
                                       pInorderTraverse->NodeInfo.vecConstraintForce[iSubCaseId].fMpcConstraintMoment_Direction1    <<"          "<<
                                       pInorderTraverse->NodeInfo.vecConstraintForce[iSubCaseId].fMpcConstraintMoment_Direction2    <<"          "<<
                                       pInorderTraverse->NodeInfo.vecConstraintForce[iSubCaseId].fMpcConstraintMoment_Direction3    <<"\n";
        }
        WriteMpcConstraintForceInorder(pInorderTraverse->pNoderight,iSubCaseId,fstrWriteMpcConstraintForce); //Traverse and display Rightside Tree nodes
    }
}

void ExportData::ExportMpcConstraintForce(Cnode &oNodeInfo)
{
    ofstream fstrWriteMpcConstraintForce("Multi-Point Constraint Forces.text",ios::out);
    if(fstrWriteMpcConstraintForce.is_open())
    {
        if(!BasicGLPane::GetAutoRotation())
            pProgressBar = new wxProgressDialog(wxT("Exporting Multi-Point Constraint Forces"),wxT("please wait.."),100,NULL,wxPD_AUTO_HIDE | wxPD_APP_MODAL);
        fstrWriteMpcConstraintForce<<"Multi-Point Constraint Forces in Nodes :\n";

        if(oNodeInfo.MpcForcePresence() == true)
        {
            for(int iIter = 0; iIter < oNodeInfo.GetNbrofSubCases(); iIter++)
            {
                fstrWriteMpcConstraintForce<<"SUBCASE = "<<iIter + 1<<":\n";
                fstrWriteMpcConstraintForce<<"NodeId          PointType          ConstraintForce_Direction1          ConstraintForce_Direction2          ConstraintForce_Direction3          ConstraintMoment_Direction1          ConstraintMoment_Direction2          ConstraintMoment_Direction3\n";
                WriteMpcConstraintForceInorder(oNodeInfo.GetRootNode(),iIter,fstrWriteMpcConstraintForce);
            }
        }
        else
        {
            fstrWriteMpcConstraintForce<<"\nMulti-Point Constraint Force Data is not available.\n";
        }
        fstrWriteMpcConstraintForce.close();
        if(!BasicGLPane::GetAutoRotation())
            pProgressBar->Update(100,wxT(""),NULL);
    }
    else
    {
        cout<<"unable to open file for writing Multi-Point Constraint Forces\n";
    }
}

void ExportData::WriteNodalOutputDisplacementInorder(Snode* pInorderTraverse,int iSubCaseId,std::ofstream &fstrWriteNodalOutputDisplacement)
{
    if(pInorderTraverse != NULL)
    {
        WriteNodalOutputDisplacementInorder(pInorderTraverse->pNodeleft,iSubCaseId,fstrWriteNodalOutputDisplacement);  //Traverse and display leftside Tree nodes
        if(pInorderTraverse->uiIdentification != 0)
        {
            fstrWriteNodalOutputDisplacement<<  pInorderTraverse->NodeInfo.NodePosition.iNodeId     <<"          "<<
                                            pInorderTraverse->NodeInfo.vecNodeDisplacement[iSubCaseId].iResultType      <<"          "<<
                                            pInorderTraverse->NodeInfo.vecNodeDisplacement[iSubCaseId].fTranslationX    <<"          "<<
                                            pInorderTraverse->NodeInfo.vecNodeDisplacement[iSubCaseId].fTranslationY    <<"          "<<
                                            pInorderTraverse->NodeInfo.vecNodeDisplacement[iSubCaseId].fTranslationZ    <<"          "<<
                                            pInorderTraverse->NodeInfo.vecNodeDisplacement[iSubCaseId].fRotationX       <<"          "<<
                                            pInorderTraverse->NodeInfo.vecNodeDisplacement[iSubCaseId].fRotationY       <<"          "<<
                                            pInorderTraverse->NodeInfo.vecNodeDisplacement[iSubCaseId].fRotationZ       <<"\n";
        }
        WriteNodalOutputDisplacementInorder(pInorderTraverse->pNoderight,iSubCaseId,fstrWriteNodalOutputDisplacement); //Traverse and display Rightside Tree nodes
    }
}

void ExportData::ExportNodalOutputDisplacement(Cnode &oNodeInfo)
{
    ofstream fstrWriteNodalOutputDisplacement("Nodal Displacement.text",ios::out);
    if(fstrWriteNodalOutputDisplacement.is_open())
    {
        if(!BasicGLPane::GetAutoRotation())
            pProgressBar = new wxProgressDialog(wxT("Exporting Nodal Displacement Results"),wxT("please wait.."),100,NULL,wxPD_AUTO_HIDE | wxPD_APP_MODAL);
        fstrWriteNodalOutputDisplacement<<"Nodal Displacement Results :\n";
        int value = oNodeInfo.GetRootNode()->NodeInfo.vecNodeDisplacement.size();
        if(oNodeInfo.NodeDisplacementPresence() == true)
        {
            for(int iIter = 0; iIter < oNodeInfo.GetNbrofSubCases(); iIter++)
            {
                fstrWriteNodalOutputDisplacement<<"SUBCASE = "<<iIter + 1<<":\n";
                fstrWriteNodalOutputDisplacement<<"NodeId          PointType          TransX          TransY          TransZ          RotX          RotY          RotZ\n";
                WriteNodalOutputDisplacementInorder(oNodeInfo.GetRootNode(),iIter,fstrWriteNodalOutputDisplacement);
            }
        }
        else
        {
            fstrWriteNodalOutputDisplacement<<"\nNode Output Displacement Data is not available.\n";
        }
        fstrWriteNodalOutputDisplacement.close();
        if(!BasicGLPane::GetAutoRotation())
            pProgressBar->Update(100,wxT(""),NULL);
    }
    else
    {
        cout<<"unable to open file for writing nodal displacement results\n";
    }
}

void ExportData::ExportElementStressStrainResults(CElement &oElementInfo)
{
    ExportElementStrain(oElementInfo);
    ExportElementStress(oElementInfo);
    int iValue;
}

void ExportData::WriteElementStrainInorder(SBinaryTreeNode* pInorderTraverse,int iSubCaseId,std::ofstream &fstrWriteElementStrain)
{
    if(pInorderTraverse != NULL)
    {
        WriteElementStrainInorder(pInorderTraverse->pNodeleft,iSubCaseId,fstrWriteElementStrain);  //Traverse and display leftside Tree nodes
        if(pInorderTraverse->uiIdentification != 0)
        {
            if(pInorderTraverse->ElementInfo.vecStressStrainData.size() != 0)
            {
                if(pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].iElementType == 2)
                    fstrWriteElementStrain<<  pInorderTraverse->uiIdentification<<"          "<<
                                          pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fStrainX<<"          "<<
                                          pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fStrainY<<"          "<<
                                          pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fStrainZ<<"          "<<
                                          pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fStrainXY<<"          "<<
                                          pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fStrainYZ<<"          "<<
                                          pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fStrainXZ<<"          "<<
                                          pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fStrain_ShearAngle<<"          "<<"NA          "<<"NA          "<<"NA          "<<
                                          pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fMajorPrincipalStrain<<"          "<<
                                          pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fMinorPrincipalStrain<<"          "<<
                                          pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fVonmisesStrain<<"\n";
                else if(pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].iElementType == 3)
                    fstrWriteElementStrain<<  pInorderTraverse->uiIdentification<<"          "<<
                                          pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fStrainX<<"          "<<
                                          pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fStrainY<<"          "<<
                                          pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fStrainZ<<"          "<<
                                          pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fStrainXY<<"          "<<
                                          pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fStrainYZ<<"          "<<
                                          pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fStrainXZ<<"          "<<
                                          "NA          "<<
                                          pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fFirstPrincipalStrain<<"          "<<
                                          pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fSecondPrincipalStrain<<"          "<<
                                          pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fThirdPrincipalStrain<<"          "<<
                                          "NA          "<<
                                          "NA          "<<
                                          "NA          "<<"\n";

            }
        }
        WriteElementStrainInorder(pInorderTraverse->pNoderight,iSubCaseId,fstrWriteElementStrain); //Traverse and display Rightside Tree nodes
    }
}

void ExportData::ExportElementStrain(CElement &oElementInfo)
{
    ofstream fstrWriteElementStrain("Element Strain.text",ios::out);
    if(fstrWriteElementStrain.is_open())
    {
        if(!BasicGLPane::GetAutoRotation())
            pProgressBar = new wxProgressDialog(wxT("Exporting Element Strain Results"),wxT("please wait.."),100,NULL,wxPD_AUTO_HIDE | wxPD_APP_MODAL);
        fstrWriteElementStrain<<"Element Strains :\n";
        if(oElementInfo.ElemStrainPresence() == true)
        {
            for(int iIter = 0; iIter < oElementInfo.GetNbrofSubCases(); iIter++)
            {
                fstrWriteElementStrain<<"SUBCASE = "<<iIter + 1<<":\n";
                fstrWriteElementStrain<<"ElemId          XX          YY          ZZ          XY          YZ          XZ          ShearAngle          1stPrincipal          2ndPrincipal          3rdPrincipal          MajorPrincipal          MinorPrincipal          Vonmises\n";
                WriteElementStrainInorder(oElementInfo.GetRootNode(),iIter,fstrWriteElementStrain);
            }
        }
        else
        {
            fstrWriteElementStrain<<"\nElement Strain Data is not available.\n";
        }

        fstrWriteElementStrain.close();
        if(!BasicGLPane::GetAutoRotation())
            pProgressBar->Update(100,wxT(""),NULL);
    }
    else
    {
        cout<<"unable to open file for writing element strain results\n";
    }
}

void ExportData::WriteElementStressInorder(SBinaryTreeNode* pInorderTraverse,int iSubCaseId,std::ofstream &fstrWriteElementStress)
{
    if(pInorderTraverse != NULL)
    {
        WriteElementStressInorder(pInorderTraverse->pNodeleft,iSubCaseId,fstrWriteElementStress);  //Traverse and display leftside Tree nodes
        if(pInorderTraverse->uiIdentification != 0)
        {
            if(pInorderTraverse->ElementInfo.vecStressStrainData.size() != 0)
            {
                if(pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].iElementType == 2)
                    fstrWriteElementStress<<  pInorderTraverse->uiIdentification<<"          "<<
                                          pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fStressX<<"          "<<
                                          pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fStressY<<"          "<<
                                          pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fStressZ<<"          "<<
                                          pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fStressXY<<"          "<<
                                          pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fStressYZ<<"          "<<
                                          pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fStressXZ<<"          "<<
                                          pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fStress_ShearAngle<<"          "<<"NA          "<<"NA          "<<"NA          "<<
                                          pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fMajorPrincipalStress<<"          "<<
                                          pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fMinorPrincipalStress<<"          "<<
                                          pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fVonmisesStress<<"\n";
                else if(pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].iElementType == 3)
                    fstrWriteElementStress<<  pInorderTraverse->uiIdentification<<"          "<<
                                          pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fStressX<<"          "<<
                                          pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fStressY<<"          "<<
                                          pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fStressZ<<"          "<<
                                          pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fStressXY<<"          "<<
                                          pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fStressYZ<<"          "<<
                                          pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fStressXZ<<"          "<<
                                          "NA          "<<
                                          pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fFirstPrincipalStress<<"          "<<
                                          pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fSecondPrincipalStress<<"          "<<
                                          pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fThirdPrincipalStress<<"          "<<
                                          "NA          "<<
                                          "NA          "<<
                                          "NA          "<<"\n";

            }
        }
        WriteElementStressInorder(pInorderTraverse->pNoderight,iSubCaseId,fstrWriteElementStress); //Traverse and display Rightside Tree nodes
    }
}

void ExportData::ExportElementStress(CElement &oElementInfo)
{
    ofstream fstrWriteElementStress("Element Stress.text",ios::out);
    if(fstrWriteElementStress.is_open())
    {
        if(!BasicGLPane::GetAutoRotation())
            pProgressBar = new wxProgressDialog(wxT("Exporting Element Stress Results"),wxT("please wait.."),100,NULL,wxPD_AUTO_HIDE | wxPD_APP_MODAL);
        fstrWriteElementStress<<"Element Stresses :\n";
        if(oElementInfo.ElemStressPresence() == true)
        {
            for(int iIter = 0; iIter < oElementInfo.GetNbrofSubCases(); iIter++)
            {
                fstrWriteElementStress<<"SUBCASE = "<<iIter + 1<<":\n";
                fstrWriteElementStress<<"ElemId          XX          YY          ZZ          XY          YZ          XZ          ShearAngle          1stPrincipal          2ndPrincipal          3rdPrincipal          MajorPrincipal          MinorPrincipal          Vonmises\n";
                WriteElementStressInorder(oElementInfo.GetRootNode(),iIter,fstrWriteElementStress);
            }
        }
        else
        {
            fstrWriteElementStress<<"\nElement Stress Data is not available.\n";
        }
        fstrWriteElementStress.close();
        if(!BasicGLPane::GetAutoRotation())
            pProgressBar->Update(100,wxT(""),NULL);
    }
    else
    {
        cout<<"unable to open file for writing element strain results\n";
    }
}

void ExportData::WriteOutputStrainEnergyInorder(SBinaryTreeNode* pInorderTraverse,int iSubCaseId,std::ofstream &fstrWriteOutputStrainEnergy)
{
    if(pInorderTraverse != NULL)
    {
        WriteOutputStrainEnergyInorder(pInorderTraverse->pNodeleft,iSubCaseId,fstrWriteOutputStrainEnergy);  //Traverse and display leftside Tree nodes
        if(pInorderTraverse->uiIdentification != 0)
        {
            if(pInorderTraverse->ElementInfo.vecElementEnergy.size() != 0)
            {
                fstrWriteOutputStrainEnergy<<  pInorderTraverse->uiIdentification                                                                              <<"          "<<
                                           pInorderTraverse->ElementInfo.vecElementEnergy[iSubCaseId].fStrainEnergy                 <<"          "<<
                                           pInorderTraverse->ElementInfo.vecElementEnergy[iSubCaseId].fPercent_of_TotalStrainEnergy <<"          "<<
                                           pInorderTraverse->ElementInfo.vecElementEnergy[iSubCaseId].fStrainEnergyDensity          <<"\n";
            }
        }
        WriteOutputStrainEnergyInorder(pInorderTraverse->pNoderight,iSubCaseId,fstrWriteOutputStrainEnergy); //Traverse and display Rightside Tree nodes
    }
}

void ExportData::ExportOutputStrainEnergy(CElement &oElementInfo)
{
    ofstream fstrWriteOutputStrainEnergy("Element Strain Energy.text",ios::out);
    if(fstrWriteOutputStrainEnergy.is_open())
    {
        if(!BasicGLPane::GetAutoRotation())
            pProgressBar = new wxProgressDialog(wxT("Exporting Element Strain Energy"),wxT("please wait.."),100,NULL,wxPD_AUTO_HIDE | wxPD_APP_MODAL);
        fstrWriteOutputStrainEnergy<<"Element Strain Energy :\n";
        if(oElementInfo.StrainEnergyPresence() == true)
        {
            for(int iIter = 0; iIter < oElementInfo.GetNbrofSubCases(); iIter++)
            {
                fstrWriteOutputStrainEnergy<<"SUBCASE = "<<iIter + 1<<":\n";
                fstrWriteOutputStrainEnergy<<"ElemId          Strain Energy          Percent of Total Strain Energy          Strain Energy Density\n";
                WriteOutputStrainEnergyInorder(oElementInfo.GetRootNode(),iIter,fstrWriteOutputStrainEnergy);
            }
        }
        else
        {
            fstrWriteOutputStrainEnergy<<"\nStrain Energy Data is not available.\n";
        }
        fstrWriteOutputStrainEnergy.close();
        if(!BasicGLPane::GetAutoRotation())
            pProgressBar->Update(100,wxT(""),NULL);
    }
    else
    {
        cout<<"unable to open file for writing element strain results\n";
    }
}

void ExportData::WriteOutputKineticEnergyInorder(SBinaryTreeNode* pInorderTraverse,int iSubCaseId,std::ofstream &fstrWriteOutputKineticEnergy)
{
    if(pInorderTraverse != NULL)
    {
        WriteOutputKineticEnergyInorder(pInorderTraverse->pNodeleft,iSubCaseId,fstrWriteOutputKineticEnergy);  //Traverse and display leftside Tree nodes
        if(pInorderTraverse->uiIdentification != 0)
        {
            if(pInorderTraverse->ElementInfo.vecElementEnergy.size() != 0)
            {
                fstrWriteOutputKineticEnergy<<  pInorderTraverse->uiIdentification                                     <<"          "<<
                                            pInorderTraverse->ElementInfo.vecElementEnergy[iSubCaseId].fKineticEnergy                 <<"          "<<
                                            pInorderTraverse->ElementInfo.vecElementEnergy[iSubCaseId].fPercent_of_TotalKineticEnergy <<"          "<<
                                            pInorderTraverse->ElementInfo.vecElementEnergy[iSubCaseId].fKineticEnergyDensity          <<"\n";
            }
        }
        WriteOutputKineticEnergyInorder(pInorderTraverse->pNoderight,iSubCaseId,fstrWriteOutputKineticEnergy); //Traverse and display Rightside Tree nodes
    }
}

void ExportData::ExportOutputKineticEnergy(CElement &oElementInfo)
{
    ofstream fstrWriteOutputKineticEnergy("Element Kinetic Energy.text",ios::out);
    if(fstrWriteOutputKineticEnergy.is_open())
    {
        if(!BasicGLPane::GetAutoRotation())
            pProgressBar = new wxProgressDialog(wxT("Exporting Element Kinetic Energy"),wxT("please wait.."),100,NULL,wxPD_AUTO_HIDE | wxPD_APP_MODAL);
        fstrWriteOutputKineticEnergy<<"Element Kinetic Energy :\n";
        if(oElementInfo.KineticEnergyPresence() == true)
        {
            for(int iIter = 0; iIter < oElementInfo.GetNbrofSubCases(); iIter++)
            {
                fstrWriteOutputKineticEnergy<<"SUBCASE = "<<iIter + 1<<":\n";
                fstrWriteOutputKineticEnergy<<"ElemId          Kinetic Energy          Percent of Total Kinetic Energy          Kinetic Energy Density\n";
                WriteOutputKineticEnergyInorder(oElementInfo.GetRootNode(),iIter,fstrWriteOutputKineticEnergy);
            }
        }
        else
        {
            fstrWriteOutputKineticEnergy<<"\nKinetic Energy Data is not available.\n";
        }
        fstrWriteOutputKineticEnergy.close();
        if(!BasicGLPane::GetAutoRotation())
            pProgressBar->Update(100,wxT(""),NULL);
    }
    else
    {
        cout<<"unable to open file for writing element kinetic results\n";
    }
}

void ExportData::WriteOutputEnergyLossInorder(SBinaryTreeNode* pInorderTraverse,int iSubCaseId,std::ofstream &fstrWriteOutputEnergyLoss)
{
    if(pInorderTraverse != NULL)
    {
        WriteOutputEnergyLossInorder(pInorderTraverse->pNodeleft,iSubCaseId,fstrWriteOutputEnergyLoss);  //Traverse and display leftside Tree nodes
        if(pInorderTraverse->uiIdentification != 0)
        {
            if(pInorderTraverse->ElementInfo.vecElementEnergy.size() != 0)
            {
                fstrWriteOutputEnergyLoss<<  pInorderTraverse->uiIdentification                                     <<"          "<<
                                         pInorderTraverse->ElementInfo.vecElementEnergy[iSubCaseId].fEnergyLoss                 <<"          "<<
                                         pInorderTraverse->ElementInfo.vecElementEnergy[iSubCaseId].fPercent_of_TotalEnergyLoss <<"          "<<
                                         pInorderTraverse->ElementInfo.vecElementEnergy[iSubCaseId].fEnergyLossDensity          <<"\n";
            }
        }
        WriteOutputEnergyLossInorder(pInorderTraverse->pNoderight,iSubCaseId,fstrWriteOutputEnergyLoss); //Traverse and display Rightside Tree nodes
    }
}

void ExportData::ExportOutputEnergyLoss(CElement &oElementInfo)
{
    ofstream fstrWriteOutputEnergyLoss("Element Energy Loss.text",ios::out);
    if(fstrWriteOutputEnergyLoss.is_open())
    {
        if(!BasicGLPane::GetAutoRotation())
            pProgressBar = new wxProgressDialog(wxT("Exporting Element Energy Loss"),wxT("please wait.."),100,NULL,wxPD_AUTO_HIDE | wxPD_APP_MODAL);
        fstrWriteOutputEnergyLoss<<"Element Energy Loss:\n";
        if(oElementInfo.EnergyLossPresence() == true)
        {
            for(int iIter = 0; iIter < oElementInfo.GetNbrofSubCases(); iIter++)
            {
                fstrWriteOutputEnergyLoss<<"SUBCASE = "<<iIter + 1<<":\n";
                fstrWriteOutputEnergyLoss<<"ElemId          Energy Loss          Percent of Total Energy Loss          Energy Loss Density\n";
                WriteOutputEnergyLossInorder(oElementInfo.GetRootNode(),iIter,fstrWriteOutputEnergyLoss);
            }
        }
        else
        {
            fstrWriteOutputEnergyLoss<<"\nEnergy Loss Data is not available.\n";
        }
        fstrWriteOutputEnergyLoss.close();
        if(!BasicGLPane::GetAutoRotation())
            pProgressBar->Update(100,wxT(""),NULL);
    }
    else
    {
        cout<<"unable to open file for writing element energy loss results\n";
    }
}

void ExportData::ExportAllDatatoTextFile(Cnode &oNodeInfo,CElement &oElementInfo)
{
    /***writes input mesh information in a text file***/
    ExportNodeCoordinates(oNodeInfo);
    ExportElementConnectivity(oElementInfo);
    ExportNodalAppliedLoads(oNodeInfo);
    ExportConstraintForces(oNodeInfo);
    /***writes results information in a text file***/
    ExportNodalOutputDisplacement(oNodeInfo);
    ExportElementStressStrainResults(oElementInfo);
    ExportOutputStrainEnergy(oElementInfo);
    ExportOutputKineticEnergy(oElementInfo);
    ExportOutputEnergyLoss(oElementInfo);
}

void ExportData::ExportGraph_StrsStrnXXInorder(SBinaryTreeNode* pInorderTraverse,int iSubCaseId,std::ofstream &fstrWriteStrsStrnXX)
{
    if(pInorderTraverse != NULL)
    {
        ExportGraph_StrsStrnXXInorder(pInorderTraverse->pNodeleft,iSubCaseId,fstrWriteStrsStrnXX);  //Traverse and display leftside Tree nodes
        if(pInorderTraverse->uiIdentification != 0)
        {
            if(pInorderTraverse->ElementInfo.vecStressStrainData.size() != 0)
            {
                if(pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].iElementType == 2)
                    fstrWriteStrsStrnXX<<pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fStrainX<<"          "<<
                                       pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fStressX<<"\n";
                else if(pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].iElementType == 3)
                    fstrWriteStrsStrnXX<<pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fStrainX<<"          "<<
                                       pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fStressX<<"\n";

            }
        }
        ExportGraph_StrsStrnXXInorder(pInorderTraverse->pNoderight,iSubCaseId,fstrWriteStrsStrnXX); //Traverse and display Rightside Tree nodes
    }
}


void ExportData::ExportGraph_StrsStrnXX(CElement &oElementInfo)
{
    if(oElementInfo.ElemStressPresence() && oElementInfo.ElemStrainPresence())
    {
        ofstream fstrWriteStrsStrnXX("Stress_Strain_Plot.text",ios::out);
        if(fstrWriteStrsStrnXX.is_open())
        {
            if(!BasicGLPane::GetAutoRotation())
                pProgressBar = new wxProgressDialog(wxT("Exporting Stress Strain XX"),wxT("please wait.."),100,NULL,wxPD_AUTO_HIDE | wxPD_APP_MODAL);
            for(int iIter = 0; iIter < oElementInfo.GetNbrofSubCases(); iIter++)
                ExportGraph_StrsStrnXXInorder(oElementInfo.GetRootNode(),iIter,fstrWriteStrsStrnXX);
            fstrWriteStrsStrnXX.close();
            if(!BasicGLPane::GetAutoRotation())
                pProgressBar->Update(100,wxT(""),NULL);
        }
        else
            cout<<"unable to open file for writing element stress strain XX results\n";
    }
    else
    {
        cout<<"Element Stress Strain XX Data is not available.\n";
        //delete the file, if it exists
        remove("Stress_Strain_Plot.text");
    }
}

void ExportData::ExportGraph_StrsStrnYYInorder(SBinaryTreeNode* pInorderTraverse,int iSubCaseId,std::ofstream &fstrWriteStrsStrnYY)
{
    if(pInorderTraverse != NULL)
    {
        ExportGraph_StrsStrnYYInorder(pInorderTraverse->pNodeleft,iSubCaseId,fstrWriteStrsStrnYY);  //Traverse and display leftside Tree nodes
        if(pInorderTraverse->uiIdentification != 0)
        {
            if(pInorderTraverse->ElementInfo.vecStressStrainData.size() != 0)
            {
                if(pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].iElementType == 2)
                    fstrWriteStrsStrnYY<<pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fStrainY<<"          "<<
                                       pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fStressY<<"\n";
                else if(pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].iElementType == 3)
                    fstrWriteStrsStrnYY<<pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fStrainY<<"          "<<
                                       pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fStressY<<"\n";

            }
        }
        ExportGraph_StrsStrnYYInorder(pInorderTraverse->pNoderight,iSubCaseId,fstrWriteStrsStrnYY); //Traverse and display Rightside Tree nodes
    }
}


void ExportData::ExportGraph_StrsStrnYY(CElement &oElementInfo)
{
    if(oElementInfo.ElemStressPresence() && oElementInfo.ElemStrainPresence())
    {
        ofstream fstrWriteStrsStrnYY("Stress_Strain_Plot.text",ios::out);
        if(fstrWriteStrsStrnYY.is_open())
        {
            if(!BasicGLPane::GetAutoRotation())
                pProgressBar = new wxProgressDialog(wxT("Exporting Stress Strain YY"),wxT("please wait.."),100,NULL,wxPD_AUTO_HIDE | wxPD_APP_MODAL);
            for(int iIter = 0; iIter < oElementInfo.GetNbrofSubCases(); iIter++)
                ExportGraph_StrsStrnYYInorder(oElementInfo.GetRootNode(),iIter,fstrWriteStrsStrnYY);
            fstrWriteStrsStrnYY.close();
            if(!BasicGLPane::GetAutoRotation())
                pProgressBar->Update(100,wxT(""),NULL);
        }
        else
            cout<<"unable to open file for writing element stress strain YY results\n";
    }
    else
    {
        cout<<"Element Stress Strain YY Data is not available.\n";
        //delete the file, if it exists
        remove("Stress_Strain_Plot.text");
    }
}

void ExportData::ExportGraph_StrsStrnZZInorder(SBinaryTreeNode* pInorderTraverse,int iSubCaseId,std::ofstream &fstrWriteStrsStrnZZ)
{
    if(pInorderTraverse != NULL)
    {
        ExportGraph_StrsStrnZZInorder(pInorderTraverse->pNodeleft,iSubCaseId,fstrWriteStrsStrnZZ);  //Traverse and display leftside Tree nodes
        if(pInorderTraverse->uiIdentification != 0)
        {
            if(pInorderTraverse->ElementInfo.vecStressStrainData.size() != 0)
            {
                if(pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].iElementType == 2)
                    fstrWriteStrsStrnZZ<<pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fStrainZ<<"          "<<
                                       pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fStressZ<<"\n";
                else if(pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].iElementType == 3)
                    fstrWriteStrsStrnZZ<<pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fStrainZ<<"          "<<
                                       pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fStressZ<<"\n";

            }
        }
        ExportGraph_StrsStrnZZInorder(pInorderTraverse->pNoderight,iSubCaseId,fstrWriteStrsStrnZZ); //Traverse and display Rightside Tree nodes
    }
}


void ExportData::ExportGraph_StrsStrnZZ(CElement &oElementInfo)
{
    if(oElementInfo.ElemStressPresence() && oElementInfo.ElemStrainPresence())
    {
        ofstream fstrWriteStrsStrnZZ("Stress_Strain_Plot.text",ios::out);
        if(fstrWriteStrsStrnZZ.is_open())
        {
            if(!BasicGLPane::GetAutoRotation())
                pProgressBar = new wxProgressDialog(wxT("Exporting Stress Strain ZZ"),wxT("please wait.."),100,NULL,wxPD_AUTO_HIDE | wxPD_APP_MODAL);
            for(int iIter = 0; iIter < oElementInfo.GetNbrofSubCases(); iIter++)
                ExportGraph_StrsStrnZZInorder(oElementInfo.GetRootNode(),iIter,fstrWriteStrsStrnZZ);
            fstrWriteStrsStrnZZ.close();
            if(!BasicGLPane::GetAutoRotation())
                pProgressBar->Update(100,wxT(""),NULL);
        }
        else
            cout<<"unable to open file for writing element stress strain ZZ results\n";
    }
    else
    {
        cout<<"Element Stress Strain ZZ Data is not available.\n";
        //delete the file, if it exists
        remove("Stress_Strain_Plot.text");
    }
}

void ExportData::ExportGraph_StrsStrnXYInorder(SBinaryTreeNode* pInorderTraverse,int iSubCaseId,std::ofstream &fstrWriteStrsStrnXY)
{
    if(pInorderTraverse != NULL)
    {
        ExportGraph_StrsStrnXYInorder(pInorderTraverse->pNodeleft,iSubCaseId,fstrWriteStrsStrnXY);  //Traverse and display leftside Tree nodes
        if(pInorderTraverse->uiIdentification != 0)
        {
            if(pInorderTraverse->ElementInfo.vecStressStrainData.size() != 0)
            {
                if(pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].iElementType == 2)
                    fstrWriteStrsStrnXY<<pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fStrainXY<<"          "<<
                                       pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fStressXY<<"\n";
                else if(pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].iElementType == 3)
                    fstrWriteStrsStrnXY<<pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fStrainXY<<"          "<<
                                       pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fStressXY<<"\n";

            }
        }
        ExportGraph_StrsStrnXYInorder(pInorderTraverse->pNoderight,iSubCaseId,fstrWriteStrsStrnXY); //Traverse and display Rightside Tree nodes
    }
}


void ExportData::ExportGraph_StrsStrnXY(CElement &oElementInfo)
{
    if(oElementInfo.ElemStressPresence() && oElementInfo.ElemStrainPresence())
    {
        ofstream fstrWriteStrsStrnXY("Stress_Strain_Plot.text",ios::out);
        if(fstrWriteStrsStrnXY.is_open())
        {
            if(!BasicGLPane::GetAutoRotation())
                pProgressBar = new wxProgressDialog(wxT("Exporting Stress Strain XY"),wxT("please wait.."),100,NULL,wxPD_AUTO_HIDE | wxPD_APP_MODAL);
            for(int iIter = 0; iIter < oElementInfo.GetNbrofSubCases(); iIter++)
                ExportGraph_StrsStrnXYInorder(oElementInfo.GetRootNode(),iIter,fstrWriteStrsStrnXY);
            fstrWriteStrsStrnXY.close();
            if(!BasicGLPane::GetAutoRotation())
                pProgressBar->Update(100,wxT(""),NULL);
        }
        else
            cout<<"unable to open file for writing element stress strain XY results\n";
    }
    else
    {
        cout<<"Element Stress Strain XY Data is not available.\n";
        //delete the file, if it exists
        remove("Stress_Strain_Plot.text");
    }
}

void ExportData::ExportGraph_StrsStrnYZInorder(SBinaryTreeNode* pInorderTraverse,int iSubCaseId,std::ofstream &fstrWriteStrsStrnYZ)
{
    if(pInorderTraverse != NULL)
    {
        ExportGraph_StrsStrnYZInorder(pInorderTraverse->pNodeleft,iSubCaseId,fstrWriteStrsStrnYZ);  //Traverse and display leftside Tree nodes
        if(pInorderTraverse->uiIdentification != 0)
        {
            if(pInorderTraverse->ElementInfo.vecStressStrainData.size() != 0)
            {
                if(pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].iElementType == 2)
                    fstrWriteStrsStrnYZ<<pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fStrainYZ<<"          "<<
                                       pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fStressYZ<<"\n";
                else if(pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].iElementType == 3)
                    fstrWriteStrsStrnYZ<<pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fStrainYZ<<"          "<<
                                       pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fStressYZ<<"\n";

            }
        }
        ExportGraph_StrsStrnYZInorder(pInorderTraverse->pNoderight,iSubCaseId,fstrWriteStrsStrnYZ); //Traverse and display Rightside Tree nodes
    }
}


void ExportData::ExportGraph_StrsStrnYZ(CElement &oElementInfo)
{
    if(oElementInfo.ElemStressPresence() && oElementInfo.ElemStrainPresence())
    {
        ofstream fstrWriteStrsStrnYZ("Stress_Strain_Plot.text",ios::out);
        if(fstrWriteStrsStrnYZ.is_open())
        {
            if(!BasicGLPane::GetAutoRotation())
                pProgressBar = new wxProgressDialog(wxT("Exporting Stress Strain YZ"),wxT("please wait.."),100,NULL,wxPD_AUTO_HIDE | wxPD_APP_MODAL);
            for(int iIter = 0; iIter < oElementInfo.GetNbrofSubCases(); iIter++)
                ExportGraph_StrsStrnYZInorder(oElementInfo.GetRootNode(),iIter,fstrWriteStrsStrnYZ);
            fstrWriteStrsStrnYZ.close();
            if(!BasicGLPane::GetAutoRotation())
                pProgressBar->Update(100,wxT(""),NULL);
        }
        else
            cout<<"unable to open file for writing element stress strain YZ results\n";
    }
    else
    {
        cout<<"Element Stress Strain YZ Data is not available.\n";
        //delete the file, if it exists
        remove("Stress_Strain_Plot.text");
    }
}

void ExportData::ExportGraph_StrsStrnXZInorder(SBinaryTreeNode* pInorderTraverse,int iSubCaseId,std::ofstream &fstrWriteStrsStrnXZ)
{
    if(pInorderTraverse != NULL)
    {
        ExportGraph_StrsStrnXZInorder(pInorderTraverse->pNodeleft,iSubCaseId,fstrWriteStrsStrnXZ);  //Traverse and display leftside Tree nodes
        if(pInorderTraverse->uiIdentification != 0)
        {
            if(pInorderTraverse->ElementInfo.vecStressStrainData.size() != 0)
            {
                if(pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].iElementType == 2)
                    fstrWriteStrsStrnXZ<<pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fStrainXZ<<"          "<<
                                       pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fStressXZ<<"\n";
                else if(pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].iElementType == 3)
                    fstrWriteStrsStrnXZ<<pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fStrainXZ<<"          "<<
                                       pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fStressXZ<<"\n";

            }
        }
        ExportGraph_StrsStrnXZInorder(pInorderTraverse->pNoderight,iSubCaseId,fstrWriteStrsStrnXZ); //Traverse and display Rightside Tree nodes
    }
}


void ExportData::ExportGraph_StrsStrnXZ(CElement &oElementInfo)
{
    if(oElementInfo.ElemStressPresence() && oElementInfo.ElemStrainPresence())
    {
        ofstream fstrWriteStrsStrnXZ("Stress_Strain_Plot.text",ios::out);
        if(fstrWriteStrsStrnXZ.is_open())
        {
            if(!BasicGLPane::GetAutoRotation())
                pProgressBar = new wxProgressDialog(wxT("Exporting Stress Strain XZ"),wxT("please wait.."),100,NULL,wxPD_AUTO_HIDE | wxPD_APP_MODAL);
            for(int iIter = 0; iIter < oElementInfo.GetNbrofSubCases(); iIter++)
                ExportGraph_StrsStrnXZInorder(oElementInfo.GetRootNode(),iIter,fstrWriteStrsStrnXZ);
            fstrWriteStrsStrnXZ.close();
            if(!BasicGLPane::GetAutoRotation())
                pProgressBar->Update(100,wxT(""),NULL);
        }
        else
            cout<<"unable to open file for writing element stress strain XZ results\n";
    }
    else
    {
        cout<<"Element Stress Strain XZ Data is not available.\n";
        //delete the file, if it exists
        remove("Stress_Strain_Plot.text");
    }
}

void ExportData::ExportGraph_StrsStrnShearAngleInorder(SBinaryTreeNode* pInorderTraverse,int iSubCaseId,std::ofstream &fstrWriteStrsStrnShearAngle)
{
    if(pInorderTraverse != NULL)
    {
        ExportGraph_StrsStrnShearAngleInorder(pInorderTraverse->pNodeleft,iSubCaseId,fstrWriteStrsStrnShearAngle);  //Traverse and display leftside Tree nodes
        if(pInorderTraverse->uiIdentification != 0)
        {
            if(pInorderTraverse->ElementInfo.vecStressStrainData.size() != 0)
            {
                if(pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].iElementType == 2)
                    fstrWriteStrsStrnShearAngle<<pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fStrain_ShearAngle<<"          "<<
                                       pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fStress_ShearAngle<<"\n";
                else if(pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].iElementType == 3)
                    fstrWriteStrsStrnShearAngle<<pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fStrain_ShearAngle<<"          "<<
                                       pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fStress_ShearAngle<<"\n";

            }
        }
        ExportGraph_StrsStrnShearAngleInorder(pInorderTraverse->pNoderight,iSubCaseId,fstrWriteStrsStrnShearAngle); //Traverse and display Rightside Tree nodes
    }
}


void ExportData::ExportGraph_StrsStrnShearAngle(CElement &oElementInfo)
{
    if(oElementInfo.ElemStressPresence() && oElementInfo.ElemStrainPresence())
    {
        ofstream fstrWriteStrsStrnShearAngle("Stress_Strain_Plot.text",ios::out);
        if(fstrWriteStrsStrnShearAngle.is_open())
        {
            if(!BasicGLPane::GetAutoRotation())
                pProgressBar = new wxProgressDialog(wxT("Exporting Stress Strain ShearAngle"),wxT("please wait.."),100,NULL,wxPD_AUTO_HIDE | wxPD_APP_MODAL);
            for(int iIter = 0; iIter < oElementInfo.GetNbrofSubCases(); iIter++)
                ExportGraph_StrsStrnShearAngleInorder(oElementInfo.GetRootNode(),iIter,fstrWriteStrsStrnShearAngle);
            fstrWriteStrsStrnShearAngle.close();
            if(!BasicGLPane::GetAutoRotation())
                pProgressBar->Update(100,wxT(""),NULL);
        }
        else
            cout<<"unable to open file for writing element stress strain Shear Angle results\n";
    }
    else
    {
        cout<<"Element Stress Strain Shear Angle Data is not available.\n";
        //delete the file, if it exists
        remove("Stress_Strain_Plot.text");
    }
}

void ExportData::ExportGraph_StrsStrn1stPrincipalInorder(SBinaryTreeNode* pInorderTraverse,int iSubCaseId,std::ofstream &fstrWriteStrsStrn1stPrincipal)
{
    if(pInorderTraverse != NULL)
    {
        ExportGraph_StrsStrn1stPrincipalInorder(pInorderTraverse->pNodeleft,iSubCaseId,fstrWriteStrsStrn1stPrincipal);  //Traverse and display leftside Tree nodes
        if(pInorderTraverse->uiIdentification != 0)
        {
            if(pInorderTraverse->ElementInfo.vecStressStrainData.size() != 0)
            {
                if(pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].iElementType == 2)
                    fstrWriteStrsStrn1stPrincipal<<pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fFirstPrincipalStrain<<"          "<<
                                       pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fFirstPrincipalStress<<"\n";
                else if(pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].iElementType == 3)
                    fstrWriteStrsStrn1stPrincipal<<pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fFirstPrincipalStrain<<"          "<<
                                       pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fFirstPrincipalStress<<"\n";

            }
        }
        ExportGraph_StrsStrn1stPrincipalInorder(pInorderTraverse->pNoderight,iSubCaseId,fstrWriteStrsStrn1stPrincipal); //Traverse and display Rightside Tree nodes
    }
}

void ExportData::ExportGraph_StrsStrn1stPrincipal(CElement &oElementInfo)
{
    if(oElementInfo.ElemStressPresence() && oElementInfo.ElemStrainPresence())
    {
        ofstream fstrWriteStrsStrn1stPrincipal("Stress_Strain_Plot.text",ios::out);
        if(fstrWriteStrsStrn1stPrincipal.is_open())
        {
            if(!BasicGLPane::GetAutoRotation())
                pProgressBar = new wxProgressDialog(wxT("Exporting Stress Strain 1stPrincipal"),wxT("please wait.."),100,NULL,wxPD_AUTO_HIDE | wxPD_APP_MODAL);
            for(int iIter = 0; iIter < oElementInfo.GetNbrofSubCases(); iIter++)
                ExportGraph_StrsStrn1stPrincipalInorder(oElementInfo.GetRootNode(),iIter,fstrWriteStrsStrn1stPrincipal);
            fstrWriteStrsStrn1stPrincipal.close();
            if(!BasicGLPane::GetAutoRotation())
                pProgressBar->Update(100,wxT(""),NULL);
        }
        else
            cout<<"unable to open file for writing element stress strain 1stPrincipal results\n";
    }
    else
    {
        cout<<"Element Stress Strain 1stPrincipal Data is not available.\n";
        //delete the file, if it exists
        remove("Stress_Strain_Plot.text");
    }
}

void ExportData::ExportGraph_StrsStrn2ndPrincipalInorder(SBinaryTreeNode* pInorderTraverse,int iSubCaseId,std::ofstream &fstrWriteStrsStrn2ndPrincipal)
{
    if(pInorderTraverse != NULL)
    {
        ExportGraph_StrsStrn2ndPrincipalInorder(pInorderTraverse->pNodeleft,iSubCaseId,fstrWriteStrsStrn2ndPrincipal);  //Traverse and display leftside Tree nodes
        if(pInorderTraverse->uiIdentification != 0)
        {
            if(pInorderTraverse->ElementInfo.vecStressStrainData.size() != 0)
            {
                if(pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].iElementType == 2)
                    fstrWriteStrsStrn2ndPrincipal<<pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fSecondPrincipalStrain<<"          "<<
                                       pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fSecondPrincipalStress<<"\n";
                else if(pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].iElementType == 3)
                    fstrWriteStrsStrn2ndPrincipal<<pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fSecondPrincipalStrain<<"          "<<
                                       pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fSecondPrincipalStress<<"\n";

            }
        }
        ExportGraph_StrsStrn2ndPrincipalInorder(pInorderTraverse->pNoderight,iSubCaseId,fstrWriteStrsStrn2ndPrincipal); //Traverse and display Rightside Tree nodes
    }
}


void ExportData::ExportGraph_StrsStrn2ndPrincipal(CElement &oElementInfo)
{
    if(oElementInfo.ElemStressPresence() && oElementInfo.ElemStrainPresence())
    {
        ofstream fstrWriteStrsStrn2ndPrincipal("Stress_Strain_Plot.text",ios::out);
        if(fstrWriteStrsStrn2ndPrincipal.is_open())
        {
            if(!BasicGLPane::GetAutoRotation())
                pProgressBar = new wxProgressDialog(wxT("Exporting Stress Strain 1stPrincipal"),wxT("please wait.."),100,NULL,wxPD_AUTO_HIDE | wxPD_APP_MODAL);
            for(int iIter = 0; iIter < oElementInfo.GetNbrofSubCases(); iIter++)
                ExportGraph_StrsStrn2ndPrincipalInorder(oElementInfo.GetRootNode(),iIter,fstrWriteStrsStrn2ndPrincipal);
            fstrWriteStrsStrn2ndPrincipal.close();
            if(!BasicGLPane::GetAutoRotation())
                pProgressBar->Update(100,wxT(""),NULL);
        }
        else
            cout<<"unable to open file for writing element stress strain 2nd Principal results\n";
    }
    else
    {
        cout<<"Element Stress Strain 2nd Principal Data is not available.\n";
        //delete the file, if it exists
        remove("Stress_Strain_Plot.text");
    }
}

void ExportData::ExportGraph_StrsStrn3rdPrincipalInorder(SBinaryTreeNode* pInorderTraverse,int iSubCaseId,std::ofstream &fstrWriteStrsStrn3rdPrincipal)
{
    if(pInorderTraverse != NULL)
    {
        ExportGraph_StrsStrn3rdPrincipalInorder(pInorderTraverse->pNodeleft,iSubCaseId,fstrWriteStrsStrn3rdPrincipal);  //Traverse and display leftside Tree nodes
        if(pInorderTraverse->uiIdentification != 0)
        {
            if(pInorderTraverse->ElementInfo.vecStressStrainData.size() != 0)
            {
                if(pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].iElementType == 2)
                    fstrWriteStrsStrn3rdPrincipal<<pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fThirdPrincipalStrain<<"          "<<
                                       pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fThirdPrincipalStress<<"\n";
                else if(pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].iElementType == 3)
                    fstrWriteStrsStrn3rdPrincipal<<pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fThirdPrincipalStrain<<"          "<<
                                       pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fThirdPrincipalStress<<"\n";

            }
        }
        ExportGraph_StrsStrn3rdPrincipalInorder(pInorderTraverse->pNoderight,iSubCaseId,fstrWriteStrsStrn3rdPrincipal); //Traverse and display Rightside Tree nodes
    }
}


void ExportData::ExportGraph_StrsStrn3rdPrincipal(CElement &oElementInfo)
{
    if(oElementInfo.ElemStressPresence() && oElementInfo.ElemStrainPresence())
    {
        ofstream fstrWriteStrsStrn3rdPrincipal("Stress_Strain_Plot.text",ios::out);
        if(fstrWriteStrsStrn3rdPrincipal.is_open())
        {
            if(!BasicGLPane::GetAutoRotation())
                pProgressBar = new wxProgressDialog(wxT("Exporting Stress Strain 3rd Principal"),wxT("please wait.."),100,NULL,wxPD_AUTO_HIDE | wxPD_APP_MODAL);
            for(int iIter = 0; iIter < oElementInfo.GetNbrofSubCases(); iIter++)
                ExportGraph_StrsStrn3rdPrincipalInorder(oElementInfo.GetRootNode(),iIter,fstrWriteStrsStrn3rdPrincipal);
            fstrWriteStrsStrn3rdPrincipal.close();
            if(!BasicGLPane::GetAutoRotation())
                pProgressBar->Update(100,wxT(""),NULL);
        }
        else
            cout<<"unable to open file for writing element stress strain 3rd Principal results\n";
    }
    else
    {
        cout<<"Element Stress Strain 3rd Principal Data is not available.\n";
        //delete the file, if it exists
        remove("Stress_Strain_Plot.text");
    }
}

void ExportData::ExportGraph_StrsStrnMajorPrincipalInorder(SBinaryTreeNode* pInorderTraverse,int iSubCaseId,std::ofstream &fstrWriteStrsStrnMajorPrincipal)
{
    if(pInorderTraverse != NULL)
    {
        ExportGraph_StrsStrnMajorPrincipalInorder(pInorderTraverse->pNodeleft,iSubCaseId,fstrWriteStrsStrnMajorPrincipal);  //Traverse and display leftside Tree nodes
        if(pInorderTraverse->uiIdentification != 0)
        {
            if(pInorderTraverse->ElementInfo.vecStressStrainData.size() != 0)
            {
                if(pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].iElementType == 2)
                    fstrWriteStrsStrnMajorPrincipal<<pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fMajorPrincipalStrain<<"          "<<
                                       pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fMajorPrincipalStress<<"\n";
                else if(pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].iElementType == 3)
                    fstrWriteStrsStrnMajorPrincipal<<pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fMajorPrincipalStrain<<"          "<<
                                       pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fMajorPrincipalStress<<"\n";

            }
        }
        ExportGraph_StrsStrnMajorPrincipalInorder(pInorderTraverse->pNoderight,iSubCaseId,fstrWriteStrsStrnMajorPrincipal); //Traverse and display Rightside Tree nodes
    }
}


void ExportData::ExportGraph_StrsStrnMajorPrincipal(CElement &oElementInfo)
{
    if(oElementInfo.ElemStressPresence() && oElementInfo.ElemStrainPresence())
    {
        ofstream fstrWriteStrsStrnMajorPrincipal("Stress_Strain_Plot.text",ios::out);
        if(fstrWriteStrsStrnMajorPrincipal.is_open())
        {
            if(!BasicGLPane::GetAutoRotation())
                pProgressBar = new wxProgressDialog(wxT("Exporting Stress Strain Major Principal"),wxT("please wait.."),100,NULL,wxPD_AUTO_HIDE | wxPD_APP_MODAL);
            for(int iIter = 0; iIter < oElementInfo.GetNbrofSubCases(); iIter++)
                ExportGraph_StrsStrnMajorPrincipalInorder(oElementInfo.GetRootNode(),iIter,fstrWriteStrsStrnMajorPrincipal);
            fstrWriteStrsStrnMajorPrincipal.close();
            if(!BasicGLPane::GetAutoRotation())
                pProgressBar->Update(100,wxT(""),NULL);
        }
        else
            cout<<"unable to open file for writing element stress strain Major Principal results\n";
    }
    else
    {
        cout<<"Element Stress Strain Major Principal Data is not available.\n";
        //delete the file, if it exists
        remove("Stress_Strain_Plot.text");
    }
}

void ExportData::ExportGraph_StrsStrnMinorPrincipalInorder(SBinaryTreeNode* pInorderTraverse,int iSubCaseId,std::ofstream &fstrWriteStrsStrnMinorPrincipal)
{
    if(pInorderTraverse != NULL)
    {
        ExportGraph_StrsStrnMinorPrincipalInorder(pInorderTraverse->pNodeleft,iSubCaseId,fstrWriteStrsStrnMinorPrincipal);  //Traverse and display leftside Tree nodes
        if(pInorderTraverse->uiIdentification != 0)
        {
            if(pInorderTraverse->ElementInfo.vecStressStrainData.size() != 0)
            {
                if(pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].iElementType == 2)
                    fstrWriteStrsStrnMinorPrincipal<<pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fMinorPrincipalStrain<<"          "<<
                                       pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fMinorPrincipalStress<<"\n";
                else if(pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].iElementType == 3)
                    fstrWriteStrsStrnMinorPrincipal<<pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fMinorPrincipalStrain<<"          "<<
                                       pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fMinorPrincipalStress<<"\n";

            }
        }
        ExportGraph_StrsStrnMinorPrincipalInorder(pInorderTraverse->pNoderight,iSubCaseId,fstrWriteStrsStrnMinorPrincipal); //Traverse and display Rightside Tree nodes
    }
}


void ExportData::ExportGraph_StrsStrnMinorPrincipal(CElement &oElementInfo)
{
    if(oElementInfo.ElemStressPresence() && oElementInfo.ElemStrainPresence())
    {
        ofstream fstrWriteStrsStrnMinorPrincipal("Stress_Strain_Plot.text",ios::out);
        if(fstrWriteStrsStrnMinorPrincipal.is_open())
        {
            if(!BasicGLPane::GetAutoRotation())
                pProgressBar = new wxProgressDialog(wxT("Exporting Stress Strain Minor Principal"),wxT("please wait.."),100,NULL,wxPD_AUTO_HIDE | wxPD_APP_MODAL);
            for(int iIter = 0; iIter < oElementInfo.GetNbrofSubCases(); iIter++)
                ExportGraph_StrsStrnMinorPrincipalInorder(oElementInfo.GetRootNode(),iIter,fstrWriteStrsStrnMinorPrincipal);
            fstrWriteStrsStrnMinorPrincipal.close();
            if(!BasicGLPane::GetAutoRotation())
                pProgressBar->Update(100,wxT(""),NULL);
        }
        else
            cout<<"unable to open file for writing element stress strain Minor Principal results\n";
    }
    else
    {
        cout<<"Element Stress Strain Minor Principal Data is not available.\n";
        //delete the file, if it exists
        remove("Stress_Strain_Plot.text");
    }
}

void ExportData::ExportGraph_StrsStrnVmisesInorder(SBinaryTreeNode* pInorderTraverse,int iSubCaseId,std::ofstream &fstrWriteStrsStrnvmises)
{
    if(pInorderTraverse != NULL)
    {
        ExportGraph_StrsStrnVmisesInorder(pInorderTraverse->pNodeleft,iSubCaseId,fstrWriteStrsStrnvmises);  //Traverse and display leftside Tree nodes
        if(pInorderTraverse->uiIdentification != 0)
        {
            if(pInorderTraverse->ElementInfo.vecStressStrainData.size() != 0)
            {
                if(pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].iElementType == 2)
                    fstrWriteStrsStrnvmises<<pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fVonmisesStrain<<"          "<<
                                       pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fVonmisesStress<<"\n";
                else if(pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].iElementType == 3)
                    fstrWriteStrsStrnvmises<<pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fVonmisesStrain<<"          "<<
                                       pInorderTraverse->ElementInfo.vecStressStrainData[iSubCaseId].fVonmisesStress<<"\n";

            }
        }
        ExportGraph_StrsStrnVmisesInorder(pInorderTraverse->pNoderight,iSubCaseId,fstrWriteStrsStrnvmises); //Traverse and display Rightside Tree nodes
    }
}

void ExportData::ExportGraph_StrsStrnVmises(CElement &oElementInfo)
{
    if(oElementInfo.ElemStressPresence() && oElementInfo.ElemStrainPresence())
    {
        ofstream fstrWriteStrsStrnvmises("Stress_Strain_Plot.text",ios::out);
        if(fstrWriteStrsStrnvmises.is_open())
        {
            if(!BasicGLPane::GetAutoRotation())
                pProgressBar = new wxProgressDialog(wxT("Exporting Stress Strain XY"),wxT("please wait.."),100,NULL,wxPD_AUTO_HIDE | wxPD_APP_MODAL);
            for(int iIter = 0; iIter < oElementInfo.GetNbrofSubCases(); iIter++)
                ExportGraph_StrsStrnVmisesInorder(oElementInfo.GetRootNode(),iIter,fstrWriteStrsStrnvmises);
            fstrWriteStrsStrnvmises.close();
            if(!BasicGLPane::GetAutoRotation())
                pProgressBar->Update(100,wxT(""),NULL);
        }
        else
            cout<<"unable to open file for writing element stress strain XY results\n";
    }
    else
    {
        cout<<"Element Stress Strain Vonmises Data is not available.\n";
        //delete the file, if it exists
        remove("Stress_Strain_Plot.text");
    }
}


ExportData::~ExportData()
{
    //dtor
    delete pProgressBar;
}
