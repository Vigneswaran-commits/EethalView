#include "cnode.h"
#include <vector>
#include <string.h>
#include <iostream>
using namespace std;

Cnode::Cnode()
{
    mpNodeRoot = NULL;   //Set Root to  point Null
    //set to true according to results availability in file
    mbNodeDisplacementPresence = false;
    mbNodalAppliedLoad_Presence = false;
    mbSpcForcePresence = false;
    mbMpcForcePresence = false;//set to true,when the result availability in output file is confirmed
    //nbr of subcases
    miNbrofSubCases = 0;
}

int Cnode::Set_RootKey(unsigned int uiNodecount)
{
    m_uiNo_of_Nodes = 0;   //Set Current No of Nodes to Null
    int iRootKey;
    if((uiNodecount%2) == 0)
    {
        iRootKey = uiNodecount/2;          /*takes the middle id as mpNodeRoot key */
    }
    else if((uiNodecount%2) != 0)
    {
        iRootKey = uiNodecount/2;          /*for odd key values,the nearest middle integer  is the key value */
    }
//    m_uiRoot_Key = iRootKey;
    //set to true,when the element result availability in output file is confirmed
    mbNodeDisplacementPresence = false;
    mbNodalAppliedLoad_Presence = false;
    mbSpcForcePresence = false;
    mbMpcForcePresence = false;
    //sets the member root key
    m_uiRoot_Key = 1;
    return iRootKey;
}

Snode*& Cnode::GetRootNode()
{
    return mpNodeRoot;
}

void Cnode::Insert_RootNode_Attributes()
{
    //sets sentinel(leaf) node data
    Snode*  pLeaf = new Snode();
    pLeaf->uiIdentification = 0;
    pLeaf->bRed = false;
    pLeaf->pNodeleft = NULL;
    pLeaf->pNoderight = NULL;
    pLeaf->pNodeParent = NULL;
    //node info
    pLeaf->NodeInfo.NodePosition.iNodeId = 0;
    pLeaf->NodeInfo.NodePosition.iLocalCoordinateSystemId = 0;
    pLeaf->NodeInfo.NodePosition.iDOF_CoordinateSystemId = 0;
    pLeaf->NodeInfo.NodePosition.iPermanentSinglePointConstraints = 0;
    pLeaf->NodeInfo.NodePosition.iSuperelementId = 0;
    pLeaf->NodeInfo.NodePosition.fXCoordinate = 0.0;
    pLeaf->NodeInfo.NodePosition.fYCoordinate = 0.0;
    pLeaf->NodeInfo.NodePosition.fZCoordinate = 0.0;
//    //output displacement
//    pLeaf->NodeInfo.NodeDisplacement.iResultType = 1;      //by default, result type is vector(=1)
//    pLeaf->NodeInfo.NodeDisplacement.iResultContent = 0;  //by default, result content is displa2cement(=0)
//    pLeaf->NodeInfo.NodeDisplacement.fTranslationX = 0.0;
//    pLeaf->NodeInfo.NodeDisplacement.fTranslationY = 0.0;
//    pLeaf->NodeInfo.NodeDisplacement.fTranslationZ = 0.0;
//    pLeaf->NodeInfo.NodeDisplacement.fRotationX = 0.0;
//    pLeaf->NodeInfo.NodeDisplacement.fRotationY = 0.0;
//    pLeaf->NodeInfo.NodeDisplacement.fRotationZ = 0.0;
//    pLeaf->NodeInfo.NodeDisplacement.fScalarValue = 0.0;
//    pLeaf->NodeInfo.vecNodeDisplacement.resize(2);
//    //applied loads at nodes
//    pLeaf->NodeInfo.NodalAppliedLoad.iResultType = 1;      //by default, result type is vector(=1)
//    pLeaf->NodeInfo.NodalAppliedLoad.fAppliedForce_Direction1 = 0.0;
//    pLeaf->NodeInfo.NodalAppliedLoad.fAppliedForce_Direction2 = 0.0;
//    pLeaf->NodeInfo.NodalAppliedLoad.fAppliedForce_Direction3 = 0.0;
//    pLeaf->NodeInfo.NodalAppliedLoad.fAppliedMoment_Direction1 = 0.0;
//    pLeaf->NodeInfo.NodalAppliedLoad.fAppliedMoment_Direction2 = 0.0;
//    pLeaf->NodeInfo.NodalAppliedLoad.fAppliedMoment_Direction3 = 0.0;
//    pLeaf->NodeInfo.NodalAppliedLoad.fScalarValue = 0.0;
//    //constraint force
//    pLeaf->NodeInfo.ConstraintForce.iResultType = 1;       //by default, result type is vector(=1)
//    pLeaf->NodeInfo.ConstraintForce.fSpcConstraintForce_Direction1 = 0.0;
//    pLeaf->NodeInfo.ConstraintForce.fSpcConstraintForce_Direction2 = 0.0;
//    pLeaf->NodeInfo.ConstraintForce.fSpcConstraintForce_Direction3 = 0.0;
//    pLeaf->NodeInfo.ConstraintForce.fSpcConstraintMoment_Direction1 = 0.0;
//    pLeaf->NodeInfo.ConstraintForce.fSpcConstraintMoment_Direction2 = 0.0;
//    pLeaf->NodeInfo.ConstraintForce.fSpcConstraintMoment_Direction3 = 0.0;
//    pLeaf->NodeInfo.ConstraintForce.fScalarValue = 0.0;

    mpBlackSentinelLeaf = pLeaf;


    //sets mpNodeRoot node data
    Snode* pNodeTmpr = NULL;
    pNodeTmpr = new Snode();
    pNodeTmpr->uiIdentification = m_uiRoot_Key;
    pNodeTmpr->bRed = false;
    pNodeTmpr->pNodeleft = mpBlackSentinelLeaf;
    pNodeTmpr->pNoderight = mpBlackSentinelLeaf;
    pNodeTmpr->pNodeParent = NULL;
    //node info
    pNodeTmpr->NodeInfo.NodePosition.iNodeId = 0;
    pNodeTmpr->NodeInfo.NodePosition.iLocalCoordinateSystemId = 0;
    pNodeTmpr->NodeInfo.NodePosition.iDOF_CoordinateSystemId = 0;
    pNodeTmpr->NodeInfo.NodePosition.iPermanentSinglePointConstraints = 0;
    pNodeTmpr->NodeInfo.NodePosition.iSuperelementId = 0;
    pNodeTmpr->NodeInfo.NodePosition.fXCoordinate = 0.0;
    pNodeTmpr->NodeInfo.NodePosition.fYCoordinate = 0.0;
    pNodeTmpr->NodeInfo.NodePosition.fZCoordinate = 0.0;
//    //output displacement
//    pNodeTmpr->NodeInfo.NodeDisplacement.iResultType = 1;
//    pNodeTmpr->NodeInfo.NodeDisplacement.iResultContent = 0;
//    pNodeTmpr->NodeInfo.NodeDisplacement.fTranslationX = 0.0;
//    pNodeTmpr->NodeInfo.NodeDisplacement.fTranslationY = 0.0;
//    pNodeTmpr->NodeInfo.NodeDisplacement.fTranslationZ = 0.0;
//    pNodeTmpr->NodeInfo.NodeDisplacement.fRotationX = 0.0;
//    pNodeTmpr->NodeInfo.NodeDisplacement.fRotationY = 0.0;
//    pNodeTmpr->NodeInfo.NodeDisplacement.fRotationZ = 0.0;
//    pNodeTmpr->NodeInfo.NodeDisplacement.fScalarValue = 0.0;
    //if 5 is less than actual nbr of subcases, export nodal displacement crashes
    pNodeTmpr->NodeInfo.vecNodeDisplacement.resize(5);
    //applied loads at nodes
//    pNodeTmpr->NodeInfo.NodalAppliedLoad.iResultType =1;
//    pNodeTmpr->NodeInfo.NodalAppliedLoad.fAppliedForce_Direction1 = 0.0;
//    pNodeTmpr->NodeInfo.NodalAppliedLoad.fAppliedForce_Direction2 = 0.0;
//    pNodeTmpr->NodeInfo.NodalAppliedLoad.fAppliedForce_Direction3 = 0.0;
//    pNodeTmpr->NodeInfo.NodalAppliedLoad.fAppliedMoment_Direction1 = 0.0;
//    pNodeTmpr->NodeInfo.NodalAppliedLoad.fAppliedMoment_Direction2 = 0.0;
//    pNodeTmpr->NodeInfo.NodalAppliedLoad.fAppliedMoment_Direction3 = 0.0;
//    pNodeTmpr->NodeInfo.NodalAppliedLoad.fScalarValue = 0.0;
    //even if 5 is less than actual nbr of subcases, resized in ResizeAppliedLoadsatNodes_Inorder() method
    pNodeTmpr->NodeInfo.vecNodalAppliedLoad.resize(5);
//    //constraint forces
//    pNodeTmpr->NodeInfo.ConstraintForce.iResultType = 1;       //by default, result type is vector(=1)
//    pNodeTmpr->NodeInfo.ConstraintForce.fSpcConstraintForce_Direction1 = 0.0;
//    pNodeTmpr->NodeInfo.ConstraintForce.fSpcConstraintForce_Direction2 = 0.0;
//    pNodeTmpr->NodeInfo.ConstraintForce.fSpcConstraintForce_Direction3 = 0.0;
//    pNodeTmpr->NodeInfo.ConstraintForce.fSpcConstraintMoment_Direction1 = 0.0;
//    pNodeTmpr->NodeInfo.ConstraintForce.fSpcConstraintMoment_Direction2 = 0.0;
//    pNodeTmpr->NodeInfo.ConstraintForce.fSpcConstraintMoment_Direction3 = 0.0;
//    pNodeTmpr->NodeInfo.ConstraintForce.fScalarValue = 0.0;
    //even if 5 is less than actual nbr of subcases, resized in ResizeConstraintForces_Inorder() method
    pNodeTmpr->NodeInfo.vecConstraintForce.resize(5);

    mpNodeRoot = pNodeTmpr;

    if(m_uiNo_of_Nodes == 0)
    {
        m_uiNo_of_Nodes++;
    }
}

void Cnode::Insert_Node_Attributes(unsigned int uiIdentity,int iNodeId,int iLocalCoordSysId,float fXCoord,float fYCoord,float fZCoord,int iDofCoordSysId,int iPermanentSinglePointConstraints,int iSuperelementId)
{
    Snode* pNullNode = NULL;
    Insert_Node_Attributes(pNullNode,mpNodeRoot,uiIdentity,"mpNodeRoot",iNodeId,iLocalCoordSysId,fXCoord,fYCoord,fZCoord,iDofCoordSysId,iPermanentSinglePointConstraints,iSuperelementId);                                                                                //
}

void Cnode::Insert_Node_Attributes(Snode* &pCurrentsParentNode,Snode* &pCurrentNode,unsigned int uiIdentity,string strNodePosition,int iNodeId,int iLocalCoordSysId,float fXCoord,float fYCoord,float fZCoord,int iDofCoordSysId,int iPermanentSinglePointConstraints,int iSuperelementId)
{
    if(pCurrentNode->uiIdentification == 0)
    {
        Snode* pNewNode = new Snode();
        pNewNode->uiIdentification = uiIdentity;
        pNewNode->bRed = true;                 //every newly inserted node is painted red
        pNewNode->pNodeParent = pCurrentsParentNode;
        if(strNodePosition == "leftchild")
        {
            pNewNode->pNodeleft = pCurrentNode;//all internal nodes are referenced to a black sentinel leaf.
            pNewNode->pNoderight = mpBlackSentinelLeaf;
        }
        else if(strNodePosition == "rightchild")
        {
            pNewNode->pNoderight = pCurrentNode;//all internal nodes are referenced to a black sentinel leaf.
            pNewNode->pNodeleft = mpBlackSentinelLeaf;
        }
        //node info
        pNewNode->NodeInfo.NodePosition.iNodeId = iNodeId;
        pNewNode->NodeInfo.NodePosition.iLocalCoordinateSystemId = iLocalCoordSysId;
        pNewNode->NodeInfo.NodePosition.fXCoordinate = fXCoord;
        pNewNode->NodeInfo.NodePosition.fYCoordinate = fYCoord;
        pNewNode->NodeInfo.NodePosition.fZCoordinate = fZCoord;
        pNewNode->NodeInfo.NodePosition.iDOF_CoordinateSystemId = iDofCoordSysId;
        pNewNode->NodeInfo.NodePosition.iPermanentSinglePointConstraints = iPermanentSinglePointConstraints;
        pNewNode->NodeInfo.NodePosition.iSuperelementId = iSuperelementId;

        pCurrentNode = pNewNode;

        RedBlackTreeInsertion(pCurrentNode);
        m_uiNo_of_Nodes++;
    }
    else if(uiIdentity < pCurrentNode->uiIdentification)
    {
        strNodePosition = "leftchild";
        Insert_Node_Attributes(pCurrentNode,pCurrentNode->pNodeleft,uiIdentity,strNodePosition,iNodeId,iLocalCoordSysId,fXCoord,fYCoord,fZCoord,iDofCoordSysId,iPermanentSinglePointConstraints,iSuperelementId);//Recursion with Current Left Snode as Argument
    }
    else if(uiIdentity > pCurrentNode->uiIdentification)
    {
        strNodePosition = "rightchild";
        Insert_Node_Attributes(pCurrentNode,pCurrentNode->pNoderight,uiIdentity,strNodePosition,iNodeId,iLocalCoordSysId,fXCoord,fYCoord,fZCoord,iDofCoordSysId,iPermanentSinglePointConstraints,iSuperelementId);//Recursion with Current Right Snode as Argument
    }
    else if(uiIdentity == pCurrentNode->uiIdentification)
    {
//        cout<<"A node with same key value "<<uiIdentity<<" already exists\n";
//        cout<<"Please enter a valid key value\n";
    }
}

void Cnode::RotateLeft(Snode *pCurrentNode)//anticlockwise around current
{
    Snode *ptmpNode = pCurrentNode->pNoderight;
    /* establish pCurrentNode->pNoderight link */
    pCurrentNode->pNoderight = ptmpNode->pNodeleft;
    if (ptmpNode->pNodeleft->uiIdentification != 0) ptmpNode->pNodeleft->pNodeParent = pCurrentNode;
    /* establish ptmpNode->pNodeParent link */
    if (ptmpNode->uiIdentification  != 0) ptmpNode->pNodeParent = pCurrentNode->pNodeParent;
    if (pCurrentNode->pNodeParent)
    {
        if (pCurrentNode == pCurrentNode->pNodeParent->pNodeleft)   //checks whether,parent & child are
            pCurrentNode->pNodeParent->pNodeleft = ptmpNode;    //in same or opposite sides of current
        else
            pCurrentNode->pNodeParent->pNoderight = ptmpNode;
    }
    else
    {
        mpNodeRoot = ptmpNode;
    }
    /* link pCurrentNode and ptmpNode */
    ptmpNode->pNodeleft = pCurrentNode;
    if (pCurrentNode->uiIdentification  != 0) pCurrentNode->pNodeParent = ptmpNode;
}

void Cnode::RotateRight(Snode *pCurrentNode)//clockwise around current
{
    Snode *ptmpNode = pCurrentNode->pNodeleft;
    /* establish pCurrentNode->pNodeleft link */
    pCurrentNode->pNodeleft = ptmpNode->pNoderight;
    if (ptmpNode->pNoderight->uiIdentification  != 0) ptmpNode->pNoderight->pNodeParent = pCurrentNode;
    /* establish ptmpNode->pNodeParent link */
    if (ptmpNode->uiIdentification  != 0) ptmpNode->pNodeParent = pCurrentNode->pNodeParent;
    if (pCurrentNode->pNodeParent)
    {
        if (pCurrentNode == pCurrentNode->pNodeParent->pNoderight)//checks whether,parent & child are
            pCurrentNode->pNodeParent->pNoderight = ptmpNode;    //in same or opposite sides of current
        else
            pCurrentNode->pNodeParent->pNodeleft = ptmpNode;
    }
    else
    {
        mpNodeRoot = ptmpNode;
    }
    /* link pCurrentNode and ptmpNode */
    ptmpNode->pNoderight = pCurrentNode;
    if (pCurrentNode->uiIdentification  != 0) pCurrentNode->pNodeParent = ptmpNode;
}

void Cnode::RedBlackTreeInsertion(Snode *pCurrentNode)
{
    /* check Red-Black properties */
    while (pCurrentNode != mpNodeRoot && pCurrentNode->pNodeParent->bRed == true)
    {
        /* we have a violation */
        if (pCurrentNode->pNodeParent == pCurrentNode->pNodeParent->pNodeParent->pNodeleft)//parent and child are in opposite sides
        {
            Snode *ptmpNode = pCurrentNode->pNodeParent->pNodeParent->pNoderight;
            if (ptmpNode->bRed == true)
            {
                /* uncle is RED */
                pCurrentNode->pNodeParent->bRed = false;
                ptmpNode->bRed = false;
                pCurrentNode->pNodeParent->pNodeParent->bRed = true;
                pCurrentNode = pCurrentNode->pNodeParent->pNodeParent;
            }
            else
            {
                /* uncle is BLACK */
                if (pCurrentNode == pCurrentNode->pNodeParent->pNoderight)//parent and child are in same side
                {
                    /* make pCurrentNode a pNodeleft child */
                    pCurrentNode = pCurrentNode->pNodeParent;
                    RotateLeft(pCurrentNode);
                }
                /* recolor and rotate */
                pCurrentNode->pNodeParent->bRed = false;
                pCurrentNode->pNodeParent->pNodeParent->bRed = true;
                RotateRight(pCurrentNode->pNodeParent->pNodeParent);
            }
        }
        else                                                            //parent and child are in opposite sides
        {
            /* mirror image of above code */
            Snode *ptmpNode = pCurrentNode->pNodeParent->pNodeParent->pNodeleft;
            if (ptmpNode->bRed == true)
            {
                /* uncle is RED */
                pCurrentNode->pNodeParent->bRed = false;
                ptmpNode->bRed = false;
                pCurrentNode->pNodeParent->pNodeParent->bRed = true;
                pCurrentNode = pCurrentNode->pNodeParent->pNodeParent;
            }
            else
            {
                /* uncle is BLACK */
                if (pCurrentNode == pCurrentNode->pNodeParent->pNodeleft)//parent and child are in same side
                {
                    pCurrentNode = pCurrentNode->pNodeParent;
                    RotateRight(pCurrentNode);
                }
                pCurrentNode->pNodeParent->bRed = false;
                pCurrentNode->pNodeParent->pNodeParent->bRed = true;
                RotateLeft(pCurrentNode->pNodeParent->pNodeParent);
            }
        }
    }
    mpNodeRoot->bRed = false;
}

void Cnode::SetNoofNodes(int iNoofNodes)
{
    this->iNoofNodes = iNoofNodes;
}

int Cnode::GetNoofNodes()
{
    return iNoofNodes;
}

void Cnode::SetNbrofSubCases(int iNbrofSubCases)
{
    miNbrofSubCases = iNbrofSubCases;
}

int Cnode::GetNbrofSubCases()
{
    return miNbrofSubCases;
}

bool Cnode::NodeDisplacementPresence()
{
    return mbNodeDisplacementPresence;
}

void Cnode::SetNodeDisplacement_Available(bool bNodeDisplacementPresence)
{
    this->mbNodeDisplacementPresence = bNodeDisplacementPresence;
}

void Cnode::SetNode_OutputDisplacement(Snode* &pCurrentNode,int iSubCaseId,int iResultType,int iResultContent,float frgTranslation[3],float frgRotation[3])
{
    //vecNodeDisplacement is resized according to nbr of subcases
    pCurrentNode->NodeInfo.vecNodeDisplacement.resize(iSubCaseId);

    pCurrentNode->NodeInfo.vecNodeDisplacement[iSubCaseId - 1].iResultType = iResultType;
    pCurrentNode->NodeInfo.vecNodeDisplacement[iSubCaseId - 1].iResultContent = iResultContent;
    pCurrentNode->NodeInfo.vecNodeDisplacement[iSubCaseId - 1].fTranslationX = frgTranslation[0];
    pCurrentNode->NodeInfo.vecNodeDisplacement[iSubCaseId - 1].fTranslationY = frgTranslation[1];
    pCurrentNode->NodeInfo.vecNodeDisplacement[iSubCaseId - 1].fTranslationZ = frgTranslation[2];
    pCurrentNode->NodeInfo.vecNodeDisplacement[iSubCaseId - 1].fRotationX = frgRotation[0];
    pCurrentNode->NodeInfo.vecNodeDisplacement[iSubCaseId - 1].fRotationY = frgRotation[1];
    pCurrentNode->NodeInfo.vecNodeDisplacement[iSubCaseId - 1].fRotationZ = frgRotation[2];
}

void Cnode::SetNode_OutputTemperature(Snode* &pCurrentNode,int iSubCaseId,int iResultType,float fScalar)
{
    pCurrentNode->NodeInfo.vecNodeDisplacement[iSubCaseId - 1].iResultType = iResultType;
    pCurrentNode->NodeInfo.vecNodeDisplacement[iSubCaseId - 1].fScalarValue = fScalar;
}

bool Cnode::NodalAppliedLoadPresence()
{
    return mbNodalAppliedLoad_Presence;
}

void Cnode::SetNodalAppliedLoad_Available(bool bNodalAppliedLoad_Presence)
{
    this->mbNodalAppliedLoad_Presence = bNodalAppliedLoad_Presence;
}

void Cnode::ResizeAppliedLoadsatNodes(Snode* pInorderTraverse)
{
    if(pInorderTraverse->NodeInfo.vecNodalAppliedLoad.size() != miNbrofSubCases)
        ResizeAppliedLoadsatNodes_Inorder(pInorderTraverse);
}

void Cnode::ResizeAppliedLoadsatNodes_Inorder(Snode* pInorderTraverse)
{
    if(pInorderTraverse != NULL)
    {
        ResizeAppliedLoadsatNodes_Inorder(pInorderTraverse->pNodeleft);
        if(pInorderTraverse->uiIdentification != 0)
        {
            pInorderTraverse->NodeInfo.vecNodalAppliedLoad.resize(miNbrofSubCases);
        }
        ResizeAppliedLoadsatNodes_Inorder(pInorderTraverse->pNoderight);
    }
}

void Cnode::SetAppliedLoadsatNodes(Snode* &pCurrentNode,int iSubCaseId,int iResultType,float frgAppliedForce[3],float frgAppliedMoment[3])
{
    //vecNodalAppliedLoad is resized according to nbr of subcases
    pCurrentNode->NodeInfo.vecNodalAppliedLoad.resize(iSubCaseId);

    pCurrentNode->NodeInfo.vecNodalAppliedLoad[iSubCaseId - 1].iResultType = iResultType;
    pCurrentNode->NodeInfo.vecNodalAppliedLoad[iSubCaseId - 1].fAppliedForce_Direction1 = frgAppliedForce[0];
    pCurrentNode->NodeInfo.vecNodalAppliedLoad[iSubCaseId - 1].fAppliedForce_Direction2 = frgAppliedForce[1];
    pCurrentNode->NodeInfo.vecNodalAppliedLoad[iSubCaseId - 1].fAppliedForce_Direction3 = frgAppliedForce[2];
    pCurrentNode->NodeInfo.vecNodalAppliedLoad[iSubCaseId - 1].fAppliedMoment_Direction1 = frgAppliedMoment[0];
    pCurrentNode->NodeInfo.vecNodalAppliedLoad[iSubCaseId - 1].fAppliedMoment_Direction2 = frgAppliedMoment[1];
    pCurrentNode->NodeInfo.vecNodalAppliedLoad[iSubCaseId - 1].fAppliedMoment_Direction3 = frgAppliedMoment[2];
}

void Cnode::SetAppliedScalarLoadsatNodes(Snode* &pCurrentNode,int iSubCaseId,int iResultType,float fScalar)
{
    //yet to be added in parser, see in NastranOp2::Scan_OPG_DataBlock method
}

bool Cnode::SpcForcePresence()
{
    return mbSpcForcePresence;
}

bool Cnode::MpcForcePresence()
{
    return mbMpcForcePresence;
}

void Cnode::SetSpcForceAvailable(bool bSpcForcePresence)
{
    this->mbSpcForcePresence = bSpcForcePresence;
}

void Cnode::SetMpcForceAvailable(bool bMpcForcePresence)
{
    this->mbMpcForcePresence = bMpcForcePresence;
}
void Cnode::ResizeConstraintForces(Snode* pInorderTraverse)
{
    //ResizeConstraintForces_Inorder is called only once
    if(pInorderTraverse->NodeInfo.vecConstraintForce.size() != miNbrofSubCases)
        ResizeConstraintForces_Inorder(pInorderTraverse);
}

void Cnode::ResizeConstraintForces_Inorder(Snode* pInorderTraverse)
{
    if(pInorderTraverse != NULL)
    {
        ResizeConstraintForces_Inorder(pInorderTraverse->pNodeleft);
        if(pInorderTraverse->uiIdentification != 0)
        {
            pInorderTraverse->NodeInfo.vecConstraintForce.resize(miNbrofSubCases);
        }
        ResizeConstraintForces_Inorder(pInorderTraverse->pNoderight);
    }
}

void Cnode::SetSpcConstraintForcesatNodes(Snode* &pCurrentNode,int iSubCaseId,int iResultType,float frgSpcConstraintForce[3],float frgSpcConstraintMoment[3])
{
    //vecConstraintForce is resized according to nbr of subcases
    //vecConstraintForce is resized only in this method for both spc and mpc forces
    pCurrentNode->NodeInfo.vecConstraintForce.resize(iSubCaseId);

    pCurrentNode->NodeInfo.vecConstraintForce[iSubCaseId - 1].iResultType = iResultType;
    pCurrentNode->NodeInfo.vecConstraintForce[iSubCaseId - 1].fSpcConstraintForce_Direction1 = frgSpcConstraintForce[0];
    pCurrentNode->NodeInfo.vecConstraintForce[iSubCaseId - 1].fSpcConstraintForce_Direction2 = frgSpcConstraintForce[1];
    pCurrentNode->NodeInfo.vecConstraintForce[iSubCaseId - 1].fSpcConstraintForce_Direction3 = frgSpcConstraintForce[2];
    pCurrentNode->NodeInfo.vecConstraintForce[iSubCaseId - 1].fSpcConstraintMoment_Direction1 = frgSpcConstraintMoment[0];
    pCurrentNode->NodeInfo.vecConstraintForce[iSubCaseId - 1].fSpcConstraintMoment_Direction2 = frgSpcConstraintMoment[1];
    pCurrentNode->NodeInfo.vecConstraintForce[iSubCaseId - 1].fSpcConstraintMoment_Direction3 = frgSpcConstraintMoment[2];
}

void Cnode::SetMpcConstraintForcesatNodes(Snode* &pCurrentNode,int iSubCaseId,int iResultType,float frgMpcConstraintForce[3],float frgMpcConstraintMoment[3])
{
    pCurrentNode->NodeInfo.vecConstraintForce[iSubCaseId - 1].iResultType = iResultType;
    pCurrentNode->NodeInfo.vecConstraintForce[iSubCaseId - 1].fMpcConstraintForce_Direction1 = frgMpcConstraintForce[0];
    pCurrentNode->NodeInfo.vecConstraintForce[iSubCaseId - 1].fMpcConstraintForce_Direction2 = frgMpcConstraintForce[1];
    pCurrentNode->NodeInfo.vecConstraintForce[iSubCaseId - 1].fMpcConstraintForce_Direction3 = frgMpcConstraintForce[2];
    pCurrentNode->NodeInfo.vecConstraintForce[iSubCaseId - 1].fMpcConstraintMoment_Direction1 = frgMpcConstraintMoment[0];
    pCurrentNode->NodeInfo.vecConstraintForce[iSubCaseId - 1].fMpcConstraintMoment_Direction2 = frgMpcConstraintMoment[1];
    pCurrentNode->NodeInfo.vecConstraintForce[iSubCaseId - 1].fMpcConstraintMoment_Direction3 = frgMpcConstraintMoment[2];
}

Snode*& Cnode::SearchandReturn_CurrentNode(unsigned int uiIdentity)
{
    SearchandReturn_CurrentNode(mpNodeRoot,uiIdentity);
}

Snode*& Cnode::SearchandReturn_CurrentNode(Snode* &pSearchedNode,unsigned int uiIdentity)
{
    if(uiIdentity == pSearchedNode->uiIdentification)
    {
        return pSearchedNode;
    }
    else if((uiIdentity < pSearchedNode->uiIdentification) && (pSearchedNode->pNodeleft == NULL))
    {
//        cout<<"No Matches for the Identification :"<<uiIdentity<<"\n";
        return pSearchedNode->pNodeleft;
    }
    else if((uiIdentity > pSearchedNode->uiIdentification) && (pSearchedNode->pNoderight == NULL))
    {
//        cout<<"No Matches for the Identification :"<<uiIdentity<<"\n";
        return pSearchedNode->pNoderight;
    }
    else if(uiIdentity < pSearchedNode->uiIdentification && pSearchedNode->pNodeleft != NULL)
    {
        SearchandReturn_CurrentNode(pSearchedNode->pNodeleft,uiIdentity);
    }
    else if(uiIdentity > pSearchedNode->uiIdentification && pSearchedNode->pNoderight != NULL)
    {
        SearchandReturn_CurrentNode(pSearchedNode->pNoderight,uiIdentity);
    }
}

Snode*& Cnode::SearchandReturn_Parentnode(unsigned int uiIdentity)
{
    string strChildPos = "mpNodeRoot";
    Snode* pNullNode = NULL;
    SearchandReturn_Parentnode(pNullNode,mpNodeRoot,uiIdentity,strChildPos);
}

Snode*& Cnode::SearchandReturn_Parentnode(Snode* &pParentNode,Snode* &pSearchedNode,unsigned int uiIdentity,std::string &strChildPosition)
{
    if(uiIdentity == pSearchedNode->uiIdentification)
    {
        return pParentNode;
    }
    else if((uiIdentity < pSearchedNode->uiIdentification) && (pSearchedNode->pNodeleft == NULL))
    {
//        cout<<"No Matches for the Identification :"<<uiIdentity<<"\n";
        return pSearchedNode->pNodeleft;
    }
    else if((uiIdentity > pSearchedNode->uiIdentification) && (pSearchedNode->pNoderight == NULL))
    {
//        cout<<"No Matches for the Identification :"<<uiIdentity<<"\n";
        return pSearchedNode->pNoderight;
    }
    else if(uiIdentity < pSearchedNode->uiIdentification && pSearchedNode->pNodeleft != NULL)
    {
        strChildPosition = "leftchild";
        SearchandReturn_Parentnode(pSearchedNode,pSearchedNode->pNodeleft,uiIdentity,strChildPosition);
    }
    else if(uiIdentity > pSearchedNode->uiIdentification && pSearchedNode->pNoderight != NULL)
    {
        strChildPosition = "rightchild";
        SearchandReturn_Parentnode(pSearchedNode,pSearchedNode->pNoderight,uiIdentity,strChildPosition);
    }
}

int Cnode::Count_No_of_Nodes()
{
    int iNode_Count;
    iNode_Count = m_uiNo_of_Nodes;
    return iNode_Count;
}

void Cnode::Display_Depth_First_Search_Preorder()
{
    Snode* pTmp_Node = NULL;
    Snode* pTmp_Node_Left = NULL;
    Snode* pTmp_id_Node = NULL;
    pTmp_Node = mpNodeRoot;
    vector<int> iPreOrder_Node_Key;
    vector<int> iPostOrder_Node_Key;
    int iCount = 0;
    int iIter;
    iPreOrder_Node_Key.push_back(mpNodeRoot->uiIdentification); //Push into PreOrder stack
    iPostOrder_Node_Key.push_back(mpNodeRoot->uiIdentification);//Push into PostOrder stack
    while( (pTmp_Node->pNodeleft != NULL) && (pTmp_Node->uiIdentification != 55) )
    {
        if(iPreOrder_Node_Key.size() != 0)
        {
            cout<<"Pre-Order :"<<iPreOrder_Node_Key[iPreOrder_Node_Key.size() - 1]<<"\n";
            iPreOrder_Node_Key.pop_back();
        }
        pTmp_id_Node =  pTmp_Node->pNoderight;
        if( pTmp_id_Node != NULL )
        {
            iPreOrder_Node_Key.push_back(pTmp_id_Node->uiIdentification);//Push into PreOrder stack
            iPostOrder_Node_Key.push_back(pTmp_id_Node->uiIdentification);//Push into PostOrder stack
        }
        pTmp_id_Node = pTmp_Node->pNodeleft;
        if( pTmp_id_Node != NULL )
        {
            iPreOrder_Node_Key.push_back(pTmp_id_Node->uiIdentification);//Push into PreOrder stack
            iPostOrder_Node_Key.push_back(pTmp_id_Node->uiIdentification);//Push into PostOrder stack
        }

        pTmp_Node = pTmp_Node->pNodeleft;
        pTmp_Node_Left = pTmp_Node->pNodeleft;
        if(pTmp_Node_Left == NULL)
        {
            if(iPreOrder_Node_Key.size() != 0)
            {
                cout<<"Pre-Order :"<<iPreOrder_Node_Key[iPreOrder_Node_Key.size() - 1]<<"\n";
                //cout<<"Post Order:"<<iPreOrder_Node_Key[iPreOrder_Node_Key.size() - 1]<<"\n";
                iPreOrder_Node_Key.pop_back();
            }
            pTmp_id_Node =  pTmp_Node->pNoderight;
            if( pTmp_id_Node != NULL )
            {
                iPreOrder_Node_Key.push_back(pTmp_id_Node->uiIdentification);//Push into PreOrder stack
                iPostOrder_Node_Key.push_back(pTmp_id_Node->uiIdentification);//Push into PostOrder stack
            }
//           pTmp_id_Node = pTmp_Node->pNodeleft;
//           if( pTmp_id_Node != NULL )
//           {
//               iPreOrder_Node_Key.push_back(pTmp_id_Node->uiIdentification);//Push into PreOrder stack
//               iPostOrder_Node_Key.push_back(pTmp_id_Node->uiIdentification);//Push into PostOrder stack
//           }
//           if(iPreOrder_Node_Key.size() != 0)
//           {
//               cout<<"Pre-Order :"<<iPreOrder_Node_Key[iPreOrder_Node_Key.size() - 1]<<"\n";
//               iPreOrder_Node_Key.pop_back();
//           }
            if(pTmp_Node->pNoderight != NULL)
            {
                pTmp_Node = pTmp_Node->pNoderight;
            }
//             if(pTmp_Node->pNodeleft == NULL)
//             {
//                 cout<<"Pre-Order :"<<iPreOrder_Node_Key[iPreOrder_Node_Key.size() - 1]<<"\n";
//             }
        }
    }
    for(iIter = 0; iIter < iPreOrder_Node_Key.size(); iIter++)
    {
        cout<<"Pre-Order :"<<iPreOrder_Node_Key[iPreOrder_Node_Key.size() - (iIter + 1)]<<"\n";
    }
    for(iIter = 0; iIter < iPostOrder_Node_Key.size(); iIter++)
    {
        cout<<"Post-Order :"<<iPostOrder_Node_Key[iPostOrder_Node_Key.size() - (iIter + 1)]<<"\n";
    }
}

void Cnode::Display_Node_Keys_Preorder()                         //PREORDER
{
    Snode* pPreorderTraverse = NULL;
    if(mpNodeRoot == NULL)
    {
//        cout<<"Root Snode Points To NULL\n";
//        cout<<"Preorder Tree Traversal Is Not possible\n\n";
    }
    else
    {
        pPreorderTraverse = mpNodeRoot;
        Display_Node_Keys_Preorder(pPreorderTraverse);
    }
}

void Cnode::Display_Node_Keys_Preorder(Snode* pPreorderTraverse)  //PREORDER
{
    if(pPreorderTraverse != NULL)
    {
        if(pPreorderTraverse->uiIdentification != 0)
        {
            cout<<pPreorderTraverse->uiIdentification<<" ";
            cout<<pPreorderTraverse->bRed<<"\n";
        }
        Display_Node_Keys_Preorder(pPreorderTraverse->pNodeleft);  //Traverse and delete leftside Tree nodes
        Display_Node_Keys_Preorder(pPreorderTraverse->pNoderight); //Traverse and delete Rightside Tree nodes
    }
}

void Cnode::Display_Node_Keys_Inorder()                            //INORDER
{
    Snode* pInorderTraverse = NULL;
    if(mpNodeRoot == NULL)
    {
//        cout<<"Root Snode Points To NULL\n";
//        cout<<"Inorder Tree Traversal Is Not possible\n\n";
    }
    else
    {
        pInorderTraverse = mpNodeRoot;
        Display_Node_Keys_Inorder(pInorderTraverse);
    }
}

void Cnode::Display_Node_Keys_Inorder(Snode* pInorderTraverse)      //INORDER
{
    if(pInorderTraverse != NULL)
    {
        Display_Node_Keys_Inorder(pInorderTraverse->pNodeleft);  //Traverse and display leftside Tree nodes
        if(pInorderTraverse->uiIdentification != 0)
        {
            cout<<pInorderTraverse->uiIdentification<<" ";
            cout<<pInorderTraverse->bRed<<"\n";
        }
        Display_Node_Keys_Inorder(pInorderTraverse->pNoderight); //Traverse and display Rightside Tree nodes
    }
}

void Cnode::Display_Node_Keys_Postorder()                            //POSTORDER
{
    Snode* pPostorderTraverse = NULL;
    if(mpNodeRoot == NULL)
    {
//        cout<<"Root Snode Points To NULL\n";
//        cout<<"Postorder Tree Traversal Is Not possible\n\n";
    }
    else
    {
        pPostorderTraverse = mpNodeRoot;
        Display_Node_Keys_Postorder(pPostorderTraverse);
    }
}

void Cnode::Display_Node_Keys_Postorder(Snode* pPostorderTraverse)   //POSTORDER
{
    if(pPostorderTraverse != NULL)
    {
        Display_Node_Keys_Postorder(pPostorderTraverse->pNodeleft);  //Traverse and delete leftside Tree nodes
        Display_Node_Keys_Postorder(pPostorderTraverse->pNoderight); //Traverse and delete Rightside Tree nodes
        if(pPostorderTraverse->uiIdentification != 0)
        {
            cout<<pPostorderTraverse->uiIdentification<<" ";
            cout<<pPostorderTraverse->bRed<<"\n";
        }
    }
}

void Cnode::Delete_Node_At_ID(unsigned int uiIdentity)
{
    if(SearchandReturn_CurrentNode(uiIdentity) == NULL)
    {
        cout<<"Key Value "<<uiIdentity<<" Points To NULL Snode\n";
        cout<<"NULL Snode Deletion Is InValid\n\n";
    }
    else
    {
        Delete_Node_At_ID(SearchandReturn_CurrentNode(uiIdentity));
        m_uiNo_of_Nodes--;
    }
}

void Cnode::Delete_Node_At_ID(Snode* &pNode_Deletion)
{
    if((pNode_Deletion->pNodeleft == NULL) && (pNode_Deletion->pNoderight == NULL))            //Case 1
    {
        delete pNode_Deletion;
        pNode_Deletion = NULL;

    }
    else if((pNode_Deletion->pNodeleft == NULL) || (pNode_Deletion->pNoderight == NULL))       //Case 2
    {
        Snode* pNode_Deletion_Right = NULL;
        Snode* pNode_Deletion_Left = NULL;
        if(pNode_Deletion->pNodeleft == NULL)
        {
            pNode_Deletion_Right = pNode_Deletion->pNoderight;
            delete pNode_Deletion;
            pNode_Deletion = NULL;
            pNode_Deletion = pNode_Deletion_Right;
        }
        else if(pNode_Deletion->pNoderight == NULL)
        {
            pNode_Deletion_Left = pNode_Deletion->pNodeleft;
            delete pNode_Deletion;
            pNode_Deletion = NULL;
            pNode_Deletion = pNode_Deletion_Left;
        }
    }
    else if((pNode_Deletion->pNodeleft != NULL) && (pNode_Deletion->pNoderight != NULL))       //Case 3(Recursive)
    {
        Snode* pNode_Replace = NULL;
        pNode_Replace = pNode_Deletion->pNodeleft;
        while(pNode_Replace->pNoderight != NULL)
        {
            pNode_Replace = pNode_Replace->pNoderight;
        }
        pNode_Deletion->uiIdentification = pNode_Replace->uiIdentification; //Replaces the deletion node value with maximum value from the deletion node's pNodeleft side
        Delete_Node_At_ID(SearchandReturn_CurrentNode(pNode_Deletion->pNodeleft,pNode_Replace->uiIdentification));//search and deletion of duplicate node
    }
}

void Cnode::Free_Memory()
{
    if(mpNodeRoot == NULL)
    {
//        cout<<"Root Snode Points To NULL\n";
//        cout<<"There Is No Allocated Memory To Free\n\n";
    }
    else
    {
        Free_Memory(mpNodeRoot);
        m_uiNo_of_Nodes = 0;
    }
}

void Cnode::Free_Memory(Snode* &pNodeDelete)
{
    if(pNodeDelete != NULL)
    {
        Free_Memory(pNodeDelete->pNodeleft);  //Traverse and delete leftside Tree nodes
        Free_Memory(pNodeDelete->pNoderight); //Traverse and delete Rightside Tree nodes
        if(pNodeDelete->uiIdentification != 0)
        {
            delete pNodeDelete;
            pNodeDelete = NULL;
        }
        //Inorder and Preorder are not fit for freeing memory
    }

}

Cnode::~Cnode()
{
    //Clears Memory Allocated for Storing Snode Data
    Free_Memory();
    if(mpNodeRoot == NULL)
    {
        cout<<"Memory De-Allocation is Successful\n\n";
    }
    else
    {
        cout<<"Memory De-Allocation is UnSuccessful\n\n";
    }
}




