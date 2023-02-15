#include "celement_bdf.h"
#include <vector>
#include <string.h>
#include <iostream>
using namespace std;

CElement_bdf::CElement_bdf()
{
    mpNodeRoot = NULL;   //Set Root to  point Null
    mbElemStress_Presence = false;
    mbElemStrain_Presence = false;
    mbStrainEnergyPresence = false;
    mbKineticEnergyPresence = false;
    mbEnergyLossPresence = false;//set to true,when the result availability in output file is confirmed
    //nbr of subcases
    miNbrofSubCases = 0;
}

//copy constructor (for deep copy)
CElement_bdf::CElement_bdf(const CElement_bdf &ElemObj)
{
    //LHS are the new object members to which the data from argument is deep copied to
    m_uiRoot_Key = ElemObj.m_uiRoot_Key;
    m_uiNo_of_Nodes = ElemObj.m_uiNo_of_Nodes;
    if(ElemObj.mpNodeRoot != NULL) //if root is null, pointers and vectors are not assigned
    {
        mpNodeRoot = new SBinaryTreeNode_bdf();                         //ptr
        *mpNodeRoot = *ElemObj.mpNodeRoot;                          //ptr
        mpBlackSentinelLeaf = new SBinaryTreeNode_bdf();                //ptr
        *mpBlackSentinelLeaf = *ElemObj.mpBlackSentinelLeaf;        //ptr
        cpvecEelementNames.resize(ElemObj.cpvecEelementNames.size());
        for(int iter = 0; iter < ElemObj.cpvecEelementNames.size(); iter++)
            cpvecEelementNames[iter] = ElemObj.cpvecEelementNames[iter];
        ivecElementStrength.resize(ElemObj.ivecElementStrength.size());
        for(int iter = 0; iter < ElemObj.ivecElementStrength.size(); iter++)
            ivecElementStrength[iter] = ElemObj.ivecElementStrength[iter];
    }
    else
    {
        //only ptrs are assigned null, other member data will have arbitrary values
        mpNodeRoot = NULL;                  //ptr
        mpBlackSentinelLeaf = NULL;         //ptr
    }
    mbElemStress_Presence = ElemObj.mbElemStress_Presence;
    mbElemStrain_Presence = ElemObj.mbElemStrain_Presence;
    mbStrainEnergyPresence = ElemObj.mbStrainEnergyPresence;
    mbKineticEnergyPresence = ElemObj.mbKineticEnergyPresence;
    mbEnergyLossPresence = ElemObj.mbEnergyLossPresence;
    miNbrofSubCases = ElemObj.miNbrofSubCases;
}

int CElement_bdf::Set_RootKey(unsigned int uiNodecount)
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
    mbElemStress_Presence = false;
    mbElemStrain_Presence = false;
    mbStrainEnergyPresence = false;
    mbKineticEnergyPresence = false;
    mbEnergyLossPresence = false;
    //sets the member root key
    m_uiRoot_Key = 1;
    return iRootKey;
}

void CElement_bdf::Insert_RootNode_Attributes()
{
    //sets sentinel(leaf) node data                     (element values set for sentinel node are not going to be used)
    SBinaryTreeNode_bdf*  pLeaf = new SBinaryTreeNode_bdf();
    pLeaf->uiIdentification = 0;
    pLeaf->bRed = false;
    pLeaf->pNodeleft = NULL;
    pLeaf->pNoderight = NULL;
    pLeaf->pNodeParent = NULL;
    //element info
    SNodeCoordinates_bdf TmpNodeCoord;
    TmpNodeCoord.iNodeId = 0;
    TmpNodeCoord.fXNormal = 0.0;
    TmpNodeCoord.fYNormal = 0.0;
    TmpNodeCoord.fZNormal = 0.0;
    TmpNodeCoord.fXCoordinate = 0.0;
    TmpNodeCoord.fXCoordinate = 0.0;
    TmpNodeCoord.fXCoordinate = 0.0;
    pLeaf->ElementInfo_bdf.ivecElementConnectivity.push_back(TmpNodeCoord);

    mpBlackSentinelLeaf = pLeaf;

    //sets mpNodeRoot node data
    SBinaryTreeNode_bdf* pNodeTmpr = NULL;
    pNodeTmpr = new SBinaryTreeNode_bdf();
    pNodeTmpr->uiIdentification = m_uiRoot_Key;
    pNodeTmpr->bRed = false;
    pNodeTmpr->pNodeleft = mpBlackSentinelLeaf;
    pNodeTmpr->pNoderight = mpBlackSentinelLeaf;
    pNodeTmpr->pNodeParent = NULL;
    //element info
    pNodeTmpr->ElementInfo_bdf.ivecElementConnectivity.push_back(TmpNodeCoord);

    mpNodeRoot = pNodeTmpr;

    if(m_uiNo_of_Nodes == 0)
    {
        m_uiNo_of_Nodes++;
    }
}

void CElement_bdf::Insert_Node_Attributes(unsigned int uiIdentity,int iElemType,char* pcElemName,int iElemStrength,std::vector<int> iElemConnectNodes)
{
    //this code inserts all element type names in a vector
    bool bInsert = true;
    for(int iIter = 0; iIter < cpvecEelementNames.size(); iIter++)
    {
        if(cpvecEelementNames[iIter] == pcElemName)
            bInsert = false;
    }
    if(bInsert == true)
    {
        cpvecEelementNames.push_back(pcElemName);
        ivecElementStrength.push_back(iElemStrength);
    }

    //************
    SBinaryTreeNode_bdf* pNullNode = NULL;
    Insert_Node_Attributes(pNullNode,mpNodeRoot,uiIdentity,iElemType,pcElemName,"mpNodeRoot",iElemConnectNodes);                                                                                                              //
}

void CElement_bdf::Insert_Node_Attributes(SBinaryTreeNode_bdf* &pCurrentsParentNode,SBinaryTreeNode_bdf* &pCurrentNode,unsigned int uiIdentity,int iElemType,char* pcElemName,std::string strNodePosition,std::vector<int> iElemConnectNodes)
{
    if(pCurrentNode->uiIdentification == 0)
    {
        SBinaryTreeNode_bdf* pNewNode = new SBinaryTreeNode_bdf();
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
        //element info
        for(int iIter = 0; iIter < iElemConnectNodes.size(); iIter++)
        {
            SNodeCoordinates_bdf TmpNodeCoord;
            TmpNodeCoord.iNodeId = iElemConnectNodes[iIter];
            TmpNodeCoord.fXNormal = 0.0;
            TmpNodeCoord.fYNormal = 0.0;
            TmpNodeCoord.fZNormal = 0.0;
            TmpNodeCoord.fXCoordinate = 0.0;
            TmpNodeCoord.fYCoordinate = 0.0;
            TmpNodeCoord.fZCoordinate = 0.0;
            pNewNode->ElementInfo_bdf.ivecElementConnectivity.push_back(TmpNodeCoord);
        }
        pNewNode->ElementInfo_bdf.iElementType = iElemType;
        pNewNode->ElementInfo_bdf.pcElementName = pcElemName;

        pCurrentNode = pNewNode;

        RedBlackTreeInsertion(pCurrentNode);
        m_uiNo_of_Nodes++;
    }
    else if(uiIdentity < pCurrentNode->uiIdentification)
    {
        strNodePosition = "leftchild";
        Insert_Node_Attributes(pCurrentNode,pCurrentNode->pNodeleft,uiIdentity,iElemType,pcElemName,strNodePosition,iElemConnectNodes);//Recursion with Current Left SBinaryTreeNode_bdf as Argument
    }
    else if(uiIdentity > pCurrentNode->uiIdentification)
    {
        strNodePosition = "rightchild";
        Insert_Node_Attributes(pCurrentNode,pCurrentNode->pNoderight,uiIdentity,iElemType,pcElemName,strNodePosition,iElemConnectNodes);//Recursion with Current Right SBinaryTreeNode_bdf as Argument
    }
    else if(uiIdentity == pCurrentNode->uiIdentification)
    {
//        cout<<"A node with same key value "<<uiIdentity<<" already exists\n";
//        cout<<"Please enter a valid key value\n";
    }
}

//void CElement_bdf::RedBlackInsertion(SBinaryTreeNode_bdf* &pCurrentNode)
//{
//    string strChildPosition = "mpNodeRoot";
//    SBinaryTreeNode_bdf* pCurrentsParent = NULL;
//    if(pCurrentNode->bRed == false)
//    {
//        //if new node becomes child of a black node,no problem
//    }
//    else//current pNodeParent is black and current is red
//    {
//        //if new node becomes child of red node,double red violation begins at leaf
//        //now current node has a red child which is the location of double red violation.this current will always have a black pNodeParent.
//        //step : 1
//        if(pCurrentNode->pNodeParent->pNodeleft->bRed == true && pCurrentNode->pNodeParent->pNoderight->bRed == true)
//        {
//            pCurrentNode->pNodeParent->pNodeleft->bRed = false;
//            pCurrentNode->pNodeParent->pNoderight->bRed = false;
//            pCurrentNode->pNodeParent->bRed = true;
//            //set current to pNodeParent of the pNodeParent(step1's last step)
//            pCurrentNode = pCurrentNode->pNodeParent->pNodeParent;
//        }
//
//        if(pCurrentNode != NULL)
//        {
//            if(pCurrentNode->uiIdentification != mpNodeRoot->uiIdentification)
//            {
//                if( (pCurrentNode->pNodeParent->pNodeleft->uiIdentification == pCurrentNode->uiIdentification && pCurrentNode->pNoderight->bRed == true) ||
//                        (pCurrentNode->pNodeParent->pNoderight->uiIdentification == pCurrentNode->uiIdentification && pCurrentNode->pNodeleft->bRed == true) )
//                {
//                    //step : 2 --pNodeParent and child of current are in same side
//                    pCurrentsParent = pCurrentNode->pNodeParent;
//                    if(pCurrentsParent->pNodeleft->uiIdentification == pCurrentNode->uiIdentification && pCurrentNode->pNoderight->bRed == true) strChildPosition = "leftnode";
//                    else if(pCurrentsParent->pNoderight->uiIdentification == pCurrentNode->uiIdentification && pCurrentNode->pNodeleft->bRed == true) strChildPosition = "rightnode";
//                    if(strChildPosition == "rightnode")//if current is pNoderight of its pNodeParent,rotate clockwise around current node
//                    {
//                        pCurrentsParent->pNoderight = pCurrentNode->pNodeleft;
//                        pCurrentNode->pNodeleft = pCurrentsParent->pNoderight->pNoderight;
//                        pCurrentsParent->pNoderight->pNoderight = pCurrentNode;
//
//                        pCurrentsParent->pNoderight->pNoderight->pNodeParent = pCurrentsParent->pNoderight;//set current's pNodeParent
//
//                        pCurrentNode = pCurrentsParent->pNoderight;//set current to new pNodeParent of the current
//                        pCurrentNode->pNodeParent = pCurrentsParent;
//                    }
//                    else if(strChildPosition == "leftnode")//if current is pNodeleft of its pNodeParent,rotate anticlockwise around current node
//                    {
//                        pCurrentsParent->pNodeleft = pCurrentNode->pNoderight;
//                        pCurrentNode->pNoderight = pCurrentsParent->pNodeleft->pNodeleft;
//                        pCurrentsParent->pNodeleft->pNodeleft = pCurrentNode;
//
//                        pCurrentsParent->pNodeleft->pNodeleft->pNodeParent = pCurrentsParent->pNodeleft;
//
//                        pCurrentNode = pCurrentsParent->pNodeleft;
//                        pCurrentNode->pNodeParent = pCurrentsParent;
//                    }
//                }
//            }
//            else
//            {
//                mpNodeRoot->bRed = false;
//            }
//
//            //step : 3.1 --pNodeParent and child of current are in opposite sides
//            strChildPosition = "mpNodeRoot";
//            pCurrentsParent = pCurrentNode->pNodeParent;
//            if( (pCurrentNode->pNodeParent->pNodeleft->uiIdentification == pCurrentNode->uiIdentification && pCurrentNode->pNodeleft->bRed == true) ||
//                    (pCurrentNode->pNodeParent->pNoderight->uiIdentification == pCurrentNode->uiIdentification && pCurrentNode->pNoderight->bRed == true) )
//            {
//                if(pCurrentsParent->pNodeleft->uiIdentification == pCurrentNode->uiIdentification && pCurrentNode->pNodeleft->bRed == true) strChildPosition = "leftnode";
//                else if(pCurrentsParent->pNoderight->uiIdentification == pCurrentNode->uiIdentification&& pCurrentNode->pNoderight->bRed == true) strChildPosition = "rightnode";
//                if(strChildPosition == "rightnode")//if current is pNoderight of its pNodeParent,rotate anticlockwise around current node
//                {
//                    pCurrentsParent->pNoderight = pCurrentNode->pNodeleft;
//                    pCurrentNode->pNodeleft = pCurrentsParent;
//                    pCurrentNode->pNodeParent = pCurrentsParent->pNodeParent;//set the pNodeParent for current and currentparent
//                    pCurrentsParent->pNodeParent = pCurrentNode;
////                    cout<<pCurrentNode->pNoderight->uiIdentification<<"\n";
////                    cout<<pCurrentNode->pNoderight->pNoderight->uiIdentification<<"\n";
//                }
//                else if(strChildPosition == "leftnode")//if current is pNodeleft of its pNodeParent,rotate clockwise around current node
//                {
//                    pCurrentsParent->pNodeleft = pCurrentNode->pNoderight;
//                    pCurrentNode->pNoderight = pCurrentsParent;
//                    pCurrentNode->pNodeParent = pCurrentsParent->pNodeParent;//set the pNodeParent for current and currentparent
//                    pCurrentsParent->pNodeParent = pCurrentNode;
//                }
//
//                //step : 3.2
//                bool bRed= pCurrentNode->bRed;
//                pCurrentNode->bRed = pCurrentsParent->bRed;
//                pCurrentsParent->bRed = bRed;
//            }
//
//            //step : 4
//            //if current is the mpNodeRoot or the mpNodeRoot sentinel,set mpNodeRoot to black and exit.otherwise repeat.
//            if(pCurrentNode->pNodeleft->uiIdentification == mpNodeRoot->uiIdentification ||
//                    pCurrentNode->pNoderight->uiIdentification == mpNodeRoot->uiIdentification ) //current is mpNodeRoot
//            {
//                //set mpNodeRoot to point new mpNodeRoot
//                mpNodeRoot = pCurrentNode;
//                mpNodeRoot->bRed = false;
//            }
//            else if(pCurrentNode->pNodeleft == NULL)                                //current is mpNodeRoot sentinel
//            {
//                pCurrentNode->bRed = false;
//                mpNodeRoot->bRed = false;
//            }
//            else                                                                    //repeat the above steps
//            {
//                RedBlackInsertion(pCurrentNode);
//            }
//        }
//        else
//        {
//            mpNodeRoot->bRed = false;
//        }
//    }
//}

void CElement_bdf::RotateLeft(SBinaryTreeNode_bdf *pCurrentNode)//anticlockwise around current
{
    SBinaryTreeNode_bdf *ptmpNode = pCurrentNode->pNoderight;
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

void CElement_bdf::RotateRight(SBinaryTreeNode_bdf *pCurrentNode)//clockwise around current
{
    SBinaryTreeNode_bdf *ptmpNode = pCurrentNode->pNodeleft;
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

void CElement_bdf::RedBlackTreeInsertion(SBinaryTreeNode_bdf *pCurrentNode)
{
    /* check Red-Black properties */
    while (pCurrentNode != mpNodeRoot && pCurrentNode->pNodeParent->bRed == true)
    {
        /* we have a violation */
        if (pCurrentNode->pNodeParent == pCurrentNode->pNodeParent->pNodeParent->pNodeleft)//parent and child are in opposite sides
        {
            SBinaryTreeNode_bdf *ptmpNode = pCurrentNode->pNodeParent->pNodeParent->pNoderight;
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
            SBinaryTreeNode_bdf *ptmpNode = pCurrentNode->pNodeParent->pNodeParent->pNodeleft;
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


SBinaryTreeNode_bdf*& CElement_bdf::SearchandReturn_CurrentNode(unsigned int uiIdentity)
{
    SearchandReturn_CurrentNode(mpNodeRoot,uiIdentity);
}

SBinaryTreeNode_bdf*& CElement_bdf::SearchandReturn_CurrentNode(SBinaryTreeNode_bdf* &pSearchedNode,unsigned int uiIdentity)
{
    if(uiIdentity == pSearchedNode->uiIdentification)
    {
        return pSearchedNode;
    }
    else if((uiIdentity < pSearchedNode->uiIdentification) && (pSearchedNode->pNodeleft == NULL))
    {
        cout<<"No Matches for the Identification :"<<uiIdentity<<"\n";
        return pSearchedNode->pNodeleft;
    }
    else if((uiIdentity > pSearchedNode->uiIdentification) && (pSearchedNode->pNoderight == NULL))
    {
        cout<<"No Matches for the Identification :"<<uiIdentity<<"\n";
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

SBinaryTreeNode_bdf*& CElement_bdf::SearchandReturn_Parentnode(unsigned int uiIdentity)
{
    string strChildPos = "mpNodeRoot";
    SBinaryTreeNode_bdf* pNullNode = NULL;
    SearchandReturn_Parentnode(pNullNode,mpNodeRoot,uiIdentity,strChildPos);
}

SBinaryTreeNode_bdf*& CElement_bdf::SearchandReturn_Parentnode(SBinaryTreeNode_bdf* &pParentNode,SBinaryTreeNode_bdf* &pSearchedNode,unsigned int uiIdentity,std::string &strChildPosition)
{
    if(uiIdentity == pSearchedNode->uiIdentification)
    {
        return pParentNode;
    }
    else if((uiIdentity < pSearchedNode->uiIdentification) && (pSearchedNode->pNodeleft == NULL))
    {
        cout<<"No Matches for the Identification :"<<uiIdentity<<"\n";
        return pSearchedNode->pNodeleft;
    }
    else if((uiIdentity > pSearchedNode->uiIdentification) && (pSearchedNode->pNoderight == NULL))
    {
        cout<<"No Matches for the Identification :"<<uiIdentity<<"\n";
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

SBinaryTreeNode_bdf*& CElement_bdf::GetRootNode()
{
    return mpNodeRoot;
}

int CElement_bdf::Count_No_of_Nodes()
{
    int iNode_Count;
    iNode_Count = m_uiNo_of_Nodes;
    return iNode_Count;
}

void CElement_bdf::Display_Depth_First_Search_Preorder()
{
    SBinaryTreeNode_bdf* pTmp_Node = NULL;
    SBinaryTreeNode_bdf* pTmp_Node_Left = NULL;
    SBinaryTreeNode_bdf* pTmp_id_Node = NULL;
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

void CElement_bdf::Display_Node_Keys_Preorder()                         //PREORDER
{
    SBinaryTreeNode_bdf* pPreorderTraverse = NULL;
    if(mpNodeRoot == NULL)
    {
//        cout<<"Root SBinaryTreeNode_bdf Points To NULL\n";
//        cout<<"Preorder Tree Traversal Is Not possible\n\n";
    }
    else
    {
        pPreorderTraverse = mpNodeRoot;
        Display_Node_Keys_Preorder(pPreorderTraverse);
    }
}

void CElement_bdf::Display_Node_Keys_Preorder(SBinaryTreeNode_bdf* pPreorderTraverse)  //PREORDER
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

void CElement_bdf::Display_Node_Keys_Inorder()                            //INORDER
{
    SBinaryTreeNode_bdf* pInorderTraverse = NULL;
    if(mpNodeRoot == NULL)
    {
//        cout<<"Root SBinaryTreeNode_bdf Points To NULL\n";
//        cout<<"Inorder Tree Traversal Is Not possible\n\n";
    }
    else
    {
        pInorderTraverse = mpNodeRoot;
        Display_Node_Keys_Inorder(pInorderTraverse);
    }
}

void CElement_bdf::Display_Node_Keys_Inorder(SBinaryTreeNode_bdf* pInorderTraverse)      //INORDER
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

void CElement_bdf::Display_Node_Keys_Postorder()                            //POSTORDER
{
    SBinaryTreeNode_bdf* pPostorderTraverse = NULL;
    if(mpNodeRoot == NULL)
    {
//        cout<<"Root SBinaryTreeNode_bdf Points To NULL\n";
//        cout<<"Postorder Tree Traversal Is Not possible\n\n";
    }
    else
    {
        pPostorderTraverse = mpNodeRoot;
        Display_Node_Keys_Postorder(pPostorderTraverse);
    }
}

void CElement_bdf::Display_Node_Keys_Postorder(SBinaryTreeNode_bdf* pPostorderTraverse)   //POSTORDER
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

void CElement_bdf::Delete_Node_At_ID(unsigned int uiIdentity)
{
    if(SearchandReturn_CurrentNode(uiIdentity) == NULL)
    {
        cout<<"Key Value "<<uiIdentity<<" Points To NULL SBinaryTreeNode_bdf\n";
        cout<<"NULL SBinaryTreeNode_bdf Deletion Is InValid\n\n";
    }
    else
    {
        Delete_Node_At_ID(SearchandReturn_CurrentNode(uiIdentity));
        m_uiNo_of_Nodes--;
    }
}

void CElement_bdf::Delete_Node_At_ID(SBinaryTreeNode_bdf* &pNode_Deletion)
{
    if((pNode_Deletion->pNodeleft == NULL) && (pNode_Deletion->pNoderight == NULL))            //Case 1
    {
        delete pNode_Deletion;
        pNode_Deletion = NULL;

    }
    else if((pNode_Deletion->pNodeleft == NULL) || (pNode_Deletion->pNoderight == NULL))       //Case 2
    {
        SBinaryTreeNode_bdf* pNode_Deletion_Right = NULL;
        SBinaryTreeNode_bdf* pNode_Deletion_Left = NULL;
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
        SBinaryTreeNode_bdf* pNode_Replace = NULL;
        pNode_Replace = pNode_Deletion->pNodeleft;
        while(pNode_Replace->pNoderight != NULL)
        {
            pNode_Replace = pNode_Replace->pNoderight;
        }
        pNode_Deletion->uiIdentification = pNode_Replace->uiIdentification; //Replaces the deletion node value with maximum value from the deletion node's pNodeleft side
        Delete_Node_At_ID(SearchandReturn_CurrentNode(pNode_Deletion->pNodeleft,pNode_Replace->uiIdentification));//search and deletion of duplicate node
    }
}

void CElement_bdf::Free_Memory()
{
    if(mpNodeRoot == NULL)
    {
//        cout<<"Root SBinaryTreeNode_bdf Points To NULL\n";
//        cout<<"There Is No Allocated Memory To Free\n\n";
    }
    else
    {
        Free_Memory(mpNodeRoot);
        m_uiNo_of_Nodes = 0;
    }
}

void CElement_bdf::Free_Memory(SBinaryTreeNode_bdf* &pNodeDelete)
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

void CElement_bdf::SetCoordinates2ConnectingNodesInorder(Cnode& NodeInfo)                            //INORDER
{
    SBinaryTreeNode_bdf* pInorderTraverse = NULL;
    if(mpNodeRoot == NULL)
    {
//        cout<<"Root SBinaryTreeNode_bdf Points To NULL\n";
//        cout<<"Inorder Tree Traversal Is Not possible\n\n";
    }
    else
    {
        pInorderTraverse = mpNodeRoot;
        SetCoordinates2ConnectingNodesInorder(pInorderTraverse,NodeInfo);
    }
}

void CElement_bdf::SetCoordinates2ConnectingNodesInorder(SBinaryTreeNode_bdf* &pInorderTraverse,Cnode& NodeInfo)      //INORDER
{
    if(pInorderTraverse != NULL)
    {
        SetCoordinates2ConnectingNodesInorder(pInorderTraverse->pNodeleft,NodeInfo);  //Traverse and display leftside Tree nodes
        if(pInorderTraverse->uiIdentification != 0)
        {
            for(int iIter = 0; iIter < pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity.size(); iIter++)
            {
                SNodeCoordinates_bdf TmpNodeCoord = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[iIter];
                pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[iIter].fXCoordinate = NodeInfo.SearchandReturn_CurrentNode(TmpNodeCoord.iNodeId)->NodeInfo.NodePosition.fXCoordinate;
                pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[iIter].fYCoordinate = NodeInfo.SearchandReturn_CurrentNode(TmpNodeCoord.iNodeId)->NodeInfo.NodePosition.fYCoordinate;
                pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[iIter].fZCoordinate = NodeInfo.SearchandReturn_CurrentNode(TmpNodeCoord.iNodeId)->NodeInfo.NodePosition.fZCoordinate;
            }
        }
        SetCoordinates2ConnectingNodesInorder(pInorderTraverse->pNoderight,NodeInfo); //Traverse and display Rightside Tree nodes
    }
}


void CElement_bdf::SetNodeCoord2ConnectingNodes(Cnode& NodeInfo)
{
    SetCoordinates2ConnectingNodesInorder(NodeInfo);
}

void CElement_bdf::SetNormalInorder()
{
    SBinaryTreeNode_bdf* pInorderTraverse = NULL;
    if(mpNodeRoot == NULL)
    {
//        cout<<"Root SBinaryTreeNode_bdf Points To NULL\n";
//        cout<<"Inorder Tree Traversal Is Not possible\n\n";
    }
    else
    {
        pInorderTraverse = mpNodeRoot;
        SetNormalInorder(pInorderTraverse);
    }
}

void CElement_bdf::SetNormalInorder(SBinaryTreeNode_bdf* &pInorderTraverse)
{
    if(pInorderTraverse != NULL)
    {
        SetNormalInorder(pInorderTraverse->pNodeleft);  //Traverse and display leftside Tree nodes
        if(pInorderTraverse->uiIdentification != 0)
        {
            float fVertex1[3],fVertex2[3],fVertex3[3];
            float fVector1[3],fVector2[3];
            float fNormal[] = {0.0,0.0,1.0};
            if(pInorderTraverse->ElementInfo_bdf.iElementType == 4)//2D element
            {
                if(pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity.size() == 3)
                {
                    fVertex1[0] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[0].fXCoordinate;
                    fVertex1[1] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[0].fYCoordinate;
                    fVertex1[2] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[0].fZCoordinate;
                    fVertex2[0] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[1].fXCoordinate;
                    fVertex2[1] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[1].fYCoordinate;
                    fVertex2[2] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[1].fZCoordinate;
                    fVertex3[0] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[2].fXCoordinate;
                    fVertex3[1] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[2].fYCoordinate;
                    fVertex3[2] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[2].fZCoordinate;
                    //finds cross product of the vectors
                    fVector1[0]=fVertex2[0]-fVertex1[0];
                    fVector1[1]=fVertex2[1]-fVertex1[1];
                    fVector1[2]=fVertex2[2]-fVertex1[2];
                    fVector2[0]=fVertex3[0]-fVertex1[0];
                    fVector2[1]=fVertex3[1]-fVertex1[1];
                    fVector2[2]=fVertex3[2]-fVertex1[2];
                    fNormal[0]=fVector1[1]*fVector2[2] - fVector1[2]*fVector2[1];
                    fNormal[1]=fVector1[2]*fVector2[0] - fVector1[0]*fVector2[2];
                    fNormal[2]=fVector1[0]*fVector2[1] - fVector1[1]*fVector2[0];
                    //push triangle coordinate normals into tree
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[0].fXNormal = fNormal[0];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[0].fYNormal = fNormal[1];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[0].fZNormal = fNormal[2];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[1].fXNormal = fNormal[0];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[1].fYNormal = fNormal[1];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[1].fZNormal = fNormal[2];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[2].fXNormal = fNormal[0];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[2].fYNormal = fNormal[1];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[2].fZNormal = fNormal[2];
                }
                else if(pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity.size() == 4)
                {
                    fVertex1[0] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[0].fXCoordinate;
                    fVertex1[1] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[0].fYCoordinate;
                    fVertex1[2] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[0].fZCoordinate;
                    fVertex2[0] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[1].fXCoordinate;
                    fVertex2[1] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[1].fYCoordinate;
                    fVertex2[2] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[1].fZCoordinate;
                    fVertex3[0] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[3].fXCoordinate;
                    fVertex3[1] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[3].fYCoordinate;
                    fVertex3[2] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[3].fZCoordinate;
                    //finds cross product of the vectors
                    fVector1[0]=fVertex2[0]-fVertex1[0];
                    fVector1[1]=fVertex2[1]-fVertex1[1];
                    fVector1[2]=fVertex2[2]-fVertex1[2];
                    fVector2[0]=fVertex3[0]-fVertex1[0];
                    fVector2[1]=fVertex3[1]-fVertex1[1];
                    fVector2[2]=fVertex3[2]-fVertex1[2];
                    fNormal[0]=fVector1[1]*fVector2[2] - fVector1[2]*fVector2[1];
                    fNormal[1]=fVector1[2]*fVector2[0] - fVector1[0]*fVector2[2];
                    fNormal[2]=fVector1[0]*fVector2[1] - fVector1[1]*fVector2[0];
                    //push triangle coordinate normals into tree
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[0].fXNormal = fNormal[0];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[0].fYNormal = fNormal[1];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[0].fZNormal = fNormal[2];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[1].fXNormal = fNormal[0];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[1].fYNormal = fNormal[1];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[1].fZNormal = fNormal[2];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[2].fXNormal = fNormal[0];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[2].fYNormal = fNormal[1];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[2].fZNormal = fNormal[2];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[3].fXNormal = fNormal[0];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[3].fYNormal = fNormal[1];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[3].fZNormal = fNormal[2];
                }
            }
            else if(pInorderTraverse->ElementInfo_bdf.iElementType == 5)//3D element
            {
                if(pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity.size() == 8)
                {
                    //0,1,2,3
                    fVertex1[0] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[0].fXCoordinate;
                    fVertex1[1] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[0].fYCoordinate;
                    fVertex1[2] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[0].fZCoordinate;
                    fVertex2[0] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[1].fXCoordinate;
                    fVertex2[1] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[1].fYCoordinate;
                    fVertex2[2] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[1].fZCoordinate;
                    fVertex3[0] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[2].fXCoordinate;
                    fVertex3[1] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[2].fYCoordinate;
                    fVertex3[2] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[2].fZCoordinate;
                    //finds cross product of the vectors
                    fVector1[0]=fVertex2[0]-fVertex1[0];
                    fVector1[1]=fVertex2[1]-fVertex1[1];
                    fVector1[2]=fVertex2[2]-fVertex1[2];
                    fVector2[0]=fVertex3[0]-fVertex1[0];
                    fVector2[1]=fVertex3[1]-fVertex1[1];
                    fVector2[2]=fVertex3[2]-fVertex1[2];
                    fNormal[0]=fVector1[1]*fVector2[2] - fVector1[2]*fVector2[1];
                    fNormal[1]=fVector1[2]*fVector2[0] - fVector1[0]*fVector2[2];
                    fNormal[2]=fVector1[0]*fVector2[1] - fVector1[1]*fVector2[0];
                    //push triangle coordinate normals into tree
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[0].fXNormal = fNormal[0];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[0].fYNormal = fNormal[1];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[0].fZNormal = fNormal[2];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[1].fXNormal = fNormal[0];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[1].fYNormal = fNormal[1];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[1].fZNormal = fNormal[2];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[2].fXNormal = fNormal[0];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[2].fYNormal = fNormal[1];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[2].fZNormal = fNormal[2];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[3].fXNormal = fNormal[0];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[3].fYNormal = fNormal[1];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[3].fZNormal = fNormal[2];
                    //4,5,6,7
                    fVertex1[0] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[4].fXCoordinate;
                    fVertex1[1] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[4].fYCoordinate;
                    fVertex1[2] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[4].fZCoordinate;
                    fVertex2[0] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[5].fXCoordinate;
                    fVertex2[1] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[5].fYCoordinate;
                    fVertex2[2] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[5].fZCoordinate;
                    fVertex3[0] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[6].fXCoordinate;
                    fVertex3[1] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[6].fYCoordinate;
                    fVertex3[2] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[6].fZCoordinate;
                    //finds cross product of the vectors
                    fVector1[0]=fVertex2[0]-fVertex1[0];
                    fVector1[1]=fVertex2[1]-fVertex1[1];
                    fVector1[2]=fVertex2[2]-fVertex1[2];
                    fVector2[0]=fVertex3[0]-fVertex1[0];
                    fVector2[1]=fVertex3[1]-fVertex1[1];
                    fVector2[2]=fVertex3[2]-fVertex1[2];
                    fNormal[0]=fVector1[1]*fVector2[2] - fVector1[2]*fVector2[1];
                    fNormal[1]=fVector1[2]*fVector2[0] - fVector1[0]*fVector2[2];
                    fNormal[2]=fVector1[0]*fVector2[1] - fVector1[1]*fVector2[0];
                    //push triangle coordinate normals into tree
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[4].fXNormal = fNormal[0];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[4].fYNormal = fNormal[1];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[4].fZNormal = fNormal[2];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[5].fXNormal = fNormal[0];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[5].fYNormal = fNormal[1];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[5].fZNormal = fNormal[2];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[6].fXNormal = fNormal[0];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[6].fYNormal = fNormal[1];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[6].fZNormal = fNormal[2];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[7].fXNormal = fNormal[0];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[7].fYNormal = fNormal[1];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[7].fZNormal = fNormal[2];
                    //1,4,5,2
                    fVertex1[0] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[1].fXCoordinate;
                    fVertex1[1] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[1].fYCoordinate;
                    fVertex1[2] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[1].fZCoordinate;
                    fVertex2[0] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[4].fXCoordinate;
                    fVertex2[1] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[4].fYCoordinate;
                    fVertex2[2] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[4].fZCoordinate;
                    fVertex3[0] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[5].fXCoordinate;
                    fVertex3[1] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[5].fYCoordinate;
                    fVertex3[2] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[5].fZCoordinate;
                    //finds cross product of the vectors
                    fVector1[0]=fVertex2[0]-fVertex1[0];
                    fVector1[1]=fVertex2[1]-fVertex1[1];
                    fVector1[2]=fVertex2[2]-fVertex1[2];
                    fVector2[0]=fVertex3[0]-fVertex1[0];
                    fVector2[1]=fVertex3[1]-fVertex1[1];
                    fVector2[2]=fVertex3[2]-fVertex1[2];
                    fNormal[0]=fVector1[1]*fVector2[2] - fVector1[2]*fVector2[1];
                    fNormal[1]=fVector1[2]*fVector2[0] - fVector1[0]*fVector2[2];
                    fNormal[2]=fVector1[0]*fVector2[1] - fVector1[1]*fVector2[0];
                    //push triangle coordinate normals into tree
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[1].fXNormal = fNormal[0];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[1].fYNormal = fNormal[1];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[1].fZNormal = fNormal[2];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[4].fXNormal = fNormal[0];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[4].fYNormal = fNormal[1];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[4].fZNormal = fNormal[2];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[5].fXNormal = fNormal[0];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[5].fYNormal = fNormal[1];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[5].fZNormal = fNormal[2];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[2].fXNormal = fNormal[0];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[2].fYNormal = fNormal[1];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[2].fZNormal = fNormal[2];
                    //2,5,6,3
                    fVertex1[0] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[2].fXCoordinate;
                    fVertex1[1] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[2].fYCoordinate;
                    fVertex1[2] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[2].fZCoordinate;
                    fVertex2[0] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[5].fXCoordinate;
                    fVertex2[1] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[5].fYCoordinate;
                    fVertex2[2] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[5].fZCoordinate;
                    fVertex3[0] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[6].fXCoordinate;
                    fVertex3[1] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[6].fYCoordinate;
                    fVertex3[2] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[6].fZCoordinate;
                    //finds cross product of the vectors
                    fVector1[0]=fVertex2[0]-fVertex1[0];
                    fVector1[1]=fVertex2[1]-fVertex1[1];
                    fVector1[2]=fVertex2[2]-fVertex1[2];
                    fVector2[0]=fVertex3[0]-fVertex1[0];
                    fVector2[1]=fVertex3[1]-fVertex1[1];
                    fVector2[2]=fVertex3[2]-fVertex1[2];
                    fNormal[0]=fVector1[1]*fVector2[2] - fVector1[2]*fVector2[1];
                    fNormal[1]=fVector1[2]*fVector2[0] - fVector1[0]*fVector2[2];
                    fNormal[2]=fVector1[0]*fVector2[1] - fVector1[1]*fVector2[0];
                    //push triangle coordinate normals into tree
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[2].fXNormal = fNormal[0];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[2].fYNormal = fNormal[1];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[2].fZNormal = fNormal[2];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[5].fXNormal = fNormal[0];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[5].fYNormal = fNormal[1];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[5].fZNormal = fNormal[2];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[6].fXNormal = fNormal[0];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[6].fYNormal = fNormal[1];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[6].fZNormal = fNormal[2];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[3].fXNormal = fNormal[0];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[3].fYNormal = fNormal[1];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[3].fZNormal = fNormal[2];
                    //3,6,7,0
                    fVertex1[0] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[3].fXCoordinate;
                    fVertex1[1] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[3].fYCoordinate;
                    fVertex1[2] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[3].fZCoordinate;
                    fVertex2[0] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[7].fXCoordinate;
                    fVertex2[1] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[7].fYCoordinate;
                    fVertex2[2] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[7].fZCoordinate;
                    fVertex3[0] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[6].fXCoordinate;
                    fVertex3[1] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[6].fYCoordinate;
                    fVertex3[2] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[6].fZCoordinate;
                    //finds cross product of the vectors
                    fVector1[0]=fVertex2[0]-fVertex1[0];
                    fVector1[1]=fVertex2[1]-fVertex1[1];
                    fVector1[2]=fVertex2[2]-fVertex1[2];
                    fVector2[0]=fVertex3[0]-fVertex1[0];
                    fVector2[1]=fVertex3[1]-fVertex1[1];
                    fVector2[2]=fVertex3[2]-fVertex1[2];
                    fNormal[0]=fVector1[1]*fVector2[2] - fVector1[2]*fVector2[1];
                    fNormal[1]=fVector1[2]*fVector2[0] - fVector1[0]*fVector2[2];
                    fNormal[2]=fVector1[0]*fVector2[1] - fVector1[1]*fVector2[0];
                    //push triangle coordinate normals into tree
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[3].fXNormal = fNormal[0];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[3].fYNormal = fNormal[1];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[3].fZNormal = fNormal[2];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[7].fXNormal = fNormal[0];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[7].fYNormal = fNormal[1];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[7].fZNormal = fNormal[2];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[6].fXNormal = fNormal[0];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[6].fYNormal = fNormal[1];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[6].fZNormal = fNormal[2];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[0].fXNormal = fNormal[0];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[0].fYNormal = fNormal[1];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[0].fZNormal = fNormal[2];
                    //0,7,4,1
                    fVertex1[0] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[0].fXCoordinate;
                    fVertex1[1] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[0].fYCoordinate;
                    fVertex1[2] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[0].fZCoordinate;
                    fVertex2[0] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[7].fXCoordinate;
                    fVertex2[1] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[7].fYCoordinate;
                    fVertex2[2] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[7].fZCoordinate;
                    fVertex3[0] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[4].fXCoordinate;
                    fVertex3[1] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[4].fYCoordinate;
                    fVertex3[2] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[4].fZCoordinate;
                    //finds cross product of the vectors
                    fVector1[0]=fVertex2[0]-fVertex1[0];
                    fVector1[1]=fVertex2[1]-fVertex1[1];
                    fVector1[2]=fVertex2[2]-fVertex1[2];
                    fVector2[0]=fVertex3[0]-fVertex1[0];
                    fVector2[1]=fVertex3[1]-fVertex1[1];
                    fVector2[2]=fVertex3[2]-fVertex1[2];
                    fNormal[0]=fVector1[1]*fVector2[2] - fVector1[2]*fVector2[1];
                    fNormal[1]=fVector1[2]*fVector2[0] - fVector1[0]*fVector2[2];
                    fNormal[2]=fVector1[0]*fVector2[1] - fVector1[1]*fVector2[0];
                    //push triangle coordinate normals into tree
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[0].fXNormal = fNormal[0];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[0].fYNormal = fNormal[1];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[0].fZNormal = fNormal[2];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[7].fXNormal = fNormal[0];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[7].fYNormal = fNormal[1];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[7].fZNormal = fNormal[2];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[4].fXNormal = fNormal[0];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[4].fYNormal = fNormal[1];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[4].fZNormal = fNormal[2];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[1].fXNormal = fNormal[0];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[1].fYNormal = fNormal[1];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[1].fZNormal = fNormal[2];
                }
                else if(pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity.size() == 6)
                {
                    //0,1,2
                    fVertex1[0] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[0].fXCoordinate;
                    fVertex1[1] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[0].fYCoordinate;
                    fVertex1[2] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[0].fZCoordinate;
                    fVertex2[0] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[1].fXCoordinate;
                    fVertex2[1] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[1].fYCoordinate;
                    fVertex2[2] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[1].fZCoordinate;
                    fVertex3[0] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[2].fXCoordinate;
                    fVertex3[1] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[2].fYCoordinate;
                    fVertex3[2] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[2].fZCoordinate;
                    //finds cross product of the vectors
                    fVector1[0]=fVertex2[0]-fVertex1[0];
                    fVector1[1]=fVertex2[1]-fVertex1[1];
                    fVector1[2]=fVertex2[2]-fVertex1[2];
                    fVector2[0]=fVertex3[0]-fVertex1[0];
                    fVector2[1]=fVertex3[1]-fVertex1[1];
                    fVector2[2]=fVertex3[2]-fVertex1[2];
                    fNormal[0]=fVector1[1]*fVector2[2] - fVector1[2]*fVector2[1];
                    fNormal[1]=fVector1[2]*fVector2[0] - fVector1[0]*fVector2[2];
                    fNormal[2]=fVector1[0]*fVector2[1] - fVector1[1]*fVector2[0];
                    //push triangle coordinate normals into tree
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[0].fXNormal = fNormal[0];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[0].fYNormal = fNormal[1];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[0].fZNormal = fNormal[2];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[1].fXNormal = fNormal[0];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[1].fYNormal = fNormal[1];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[1].fZNormal = fNormal[2];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[2].fXNormal = fNormal[0];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[2].fYNormal = fNormal[1];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[2].fZNormal = fNormal[2];
                    //3,4,5
                    fVertex1[0] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[3].fXCoordinate;
                    fVertex1[1] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[3].fYCoordinate;
                    fVertex1[2] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[3].fZCoordinate;
                    fVertex2[0] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[4].fXCoordinate;
                    fVertex2[1] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[4].fYCoordinate;
                    fVertex2[2] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[4].fZCoordinate;
                    fVertex3[0] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[5].fXCoordinate;
                    fVertex3[1] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[5].fYCoordinate;
                    fVertex3[2] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[5].fZCoordinate;
                    //finds cross product of the vectors
                    fVector1[0]=fVertex2[0]-fVertex1[0];
                    fVector1[1]=fVertex2[1]-fVertex1[1];
                    fVector1[2]=fVertex2[2]-fVertex1[2];
                    fVector2[0]=fVertex3[0]-fVertex1[0];
                    fVector2[1]=fVertex3[1]-fVertex1[1];
                    fVector2[2]=fVertex3[2]-fVertex1[2];
                    fNormal[0]=fVector1[1]*fVector2[2] - fVector1[2]*fVector2[1];
                    fNormal[1]=fVector1[2]*fVector2[0] - fVector1[0]*fVector2[2];
                    fNormal[2]=fVector1[0]*fVector2[1] - fVector1[1]*fVector2[0];
                    //push triangle coordinate normals into tree
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[3].fXNormal = fNormal[0];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[3].fYNormal = fNormal[1];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[3].fZNormal = fNormal[2];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[4].fXNormal = fNormal[0];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[4].fYNormal = fNormal[1];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[4].fZNormal = fNormal[2];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[5].fXNormal = fNormal[0];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[5].fYNormal = fNormal[1];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[5].fZNormal = fNormal[2];
                    //0,2,5,3
                    fVertex1[0] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[0].fXCoordinate;
                    fVertex1[1] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[0].fYCoordinate;
                    fVertex1[2] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[0].fZCoordinate;
                    fVertex2[0] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[2].fXCoordinate;
                    fVertex2[1] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[2].fYCoordinate;
                    fVertex2[2] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[2].fZCoordinate;
                    fVertex3[0] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[5].fXCoordinate;
                    fVertex3[1] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[5].fYCoordinate;
                    fVertex3[2] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[5].fZCoordinate;
                    //finds cross product of the vectors
                    fVector1[0]=fVertex2[0]-fVertex1[0];
                    fVector1[1]=fVertex2[1]-fVertex1[1];
                    fVector1[2]=fVertex2[2]-fVertex1[2];
                    fVector2[0]=fVertex3[0]-fVertex1[0];
                    fVector2[1]=fVertex3[1]-fVertex1[1];
                    fVector2[2]=fVertex3[2]-fVertex1[2];
                    fNormal[0]=fVector1[1]*fVector2[2] - fVector1[2]*fVector2[1];
                    fNormal[1]=fVector1[2]*fVector2[0] - fVector1[0]*fVector2[2];
                    fNormal[2]=fVector1[0]*fVector2[1] - fVector1[1]*fVector2[0];
                    //push triangle coordinate normals into tree
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[0].fXNormal = fNormal[0];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[0].fYNormal = fNormal[1];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[0].fZNormal = fNormal[2];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[2].fXNormal = fNormal[0];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[2].fYNormal = fNormal[1];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[2].fZNormal = fNormal[2];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[5].fXNormal = fNormal[0];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[5].fYNormal = fNormal[1];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[5].fZNormal = fNormal[2];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[3].fXNormal = fNormal[0];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[3].fYNormal = fNormal[1];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[3].fZNormal = fNormal[2];
                    //1,2,5,4
                    fVertex1[0] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[1].fXCoordinate;
                    fVertex1[1] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[1].fYCoordinate;
                    fVertex1[2] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[1].fZCoordinate;
                    fVertex2[0] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[2].fXCoordinate;
                    fVertex2[1] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[2].fYCoordinate;
                    fVertex2[2] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[2].fZCoordinate;
                    fVertex3[0] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[5].fXCoordinate;
                    fVertex3[1] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[5].fYCoordinate;
                    fVertex3[2] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[5].fZCoordinate;
                    //finds cross product of the vectors
                    fVector1[0]=fVertex2[0]-fVertex1[0];
                    fVector1[1]=fVertex2[1]-fVertex1[1];
                    fVector1[2]=fVertex2[2]-fVertex1[2];
                    fVector2[0]=fVertex3[0]-fVertex1[0];
                    fVector2[1]=fVertex3[1]-fVertex1[1];
                    fVector2[2]=fVertex3[2]-fVertex1[2];
                    fNormal[0]=fVector1[1]*fVector2[2] - fVector1[2]*fVector2[1];
                    fNormal[1]=fVector1[2]*fVector2[0] - fVector1[0]*fVector2[2];
                    fNormal[2]=fVector1[0]*fVector2[1] - fVector1[1]*fVector2[0];
                    //push triangle coordinate normals into tree
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[1].fXNormal = fNormal[0];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[1].fYNormal = fNormal[1];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[1].fZNormal = fNormal[2];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[2].fXNormal = fNormal[0];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[2].fYNormal = fNormal[1];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[2].fZNormal = fNormal[2];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[5].fXNormal = fNormal[0];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[5].fYNormal = fNormal[1];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[5].fZNormal = fNormal[2];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[4].fXNormal = fNormal[0];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[4].fYNormal = fNormal[1];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[4].fZNormal = fNormal[2];
                    //0,1,4,3
                    fVertex1[0] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[0].fXCoordinate;
                    fVertex1[1] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[0].fYCoordinate;
                    fVertex1[2] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[0].fZCoordinate;
                    fVertex2[0] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[1].fXCoordinate;
                    fVertex2[1] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[1].fYCoordinate;
                    fVertex2[2] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[1].fZCoordinate;
                    fVertex3[0] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[4].fXCoordinate;
                    fVertex3[1] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[4].fYCoordinate;
                    fVertex3[2] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[4].fZCoordinate;
                    //finds cross product of the vectors
                    fVector1[0]=fVertex2[0]-fVertex1[0];
                    fVector1[1]=fVertex2[1]-fVertex1[1];
                    fVector1[2]=fVertex2[2]-fVertex1[2];
                    fVector2[0]=fVertex3[0]-fVertex1[0];
                    fVector2[1]=fVertex3[1]-fVertex1[1];
                    fVector2[2]=fVertex3[2]-fVertex1[2];
                    fNormal[0]=fVector1[1]*fVector2[2] - fVector1[2]*fVector2[1];
                    fNormal[1]=fVector1[2]*fVector2[0] - fVector1[0]*fVector2[2];
                    fNormal[2]=fVector1[0]*fVector2[1] - fVector1[1]*fVector2[0];
                    //push triangle coordinate normals into tree
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[0].fXNormal = fNormal[0];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[0].fYNormal = fNormal[1];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[0].fZNormal = fNormal[2];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[1].fXNormal = fNormal[0];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[1].fYNormal = fNormal[1];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[1].fZNormal = fNormal[2];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[4].fXNormal = fNormal[0];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[4].fYNormal = fNormal[1];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[4].fZNormal = fNormal[2];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[3].fXNormal = fNormal[0];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[3].fYNormal = fNormal[1];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[3].fZNormal = fNormal[2];
                }
                else if(pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity.size() == 4)
                {
                    //0,1,3
                    fVertex1[0] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[0].fXCoordinate;
                    fVertex1[1] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[0].fYCoordinate;
                    fVertex1[2] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[0].fZCoordinate;
                    fVertex2[0] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[1].fXCoordinate;
                    fVertex2[1] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[1].fYCoordinate;
                    fVertex2[2] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[1].fZCoordinate;
                    fVertex3[0] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[3].fXCoordinate;
                    fVertex3[1] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[3].fYCoordinate;
                    fVertex3[2] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[3].fZCoordinate;
                    //finds cross product of the vectors
                    fVector1[0]=fVertex2[0]-fVertex1[0];
                    fVector1[1]=fVertex2[1]-fVertex1[1];
                    fVector1[2]=fVertex2[2]-fVertex1[2];
                    fVector2[0]=fVertex3[0]-fVertex1[0];
                    fVector2[1]=fVertex3[1]-fVertex1[1];
                    fVector2[2]=fVertex3[2]-fVertex1[2];
                    fNormal[0]=fVector1[1]*fVector2[2] - fVector1[2]*fVector2[1];
                    fNormal[1]=fVector1[2]*fVector2[0] - fVector1[0]*fVector2[2];
                    fNormal[2]=fVector1[0]*fVector2[1] - fVector1[1]*fVector2[0];
                    //push triangle coordinate normals into tree
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[0].fXNormal = fNormal[0];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[0].fYNormal = fNormal[1];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[0].fZNormal = fNormal[2];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[1].fXNormal = fNormal[0];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[1].fYNormal = fNormal[1];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[1].fZNormal = fNormal[2];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[3].fXNormal = fNormal[0];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[3].fYNormal = fNormal[1];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[3].fZNormal = fNormal[2];
                    //1,2,3
                    fVertex1[0] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[1].fXCoordinate;
                    fVertex1[1] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[1].fYCoordinate;
                    fVertex1[2] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[1].fZCoordinate;
                    fVertex2[0] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[2].fXCoordinate;
                    fVertex2[1] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[2].fYCoordinate;
                    fVertex2[2] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[2].fZCoordinate;
                    fVertex3[0] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[3].fXCoordinate;
                    fVertex3[1] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[3].fYCoordinate;
                    fVertex3[2] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[3].fZCoordinate;
                    //finds cross product of the vectors
                    fVector1[0]=fVertex2[0]-fVertex1[0];
                    fVector1[1]=fVertex2[1]-fVertex1[1];
                    fVector1[2]=fVertex2[2]-fVertex1[2];
                    fVector2[0]=fVertex3[0]-fVertex1[0];
                    fVector2[1]=fVertex3[1]-fVertex1[1];
                    fVector2[2]=fVertex3[2]-fVertex1[2];
                    fNormal[0]=fVector1[1]*fVector2[2] - fVector1[2]*fVector2[1];
                    fNormal[1]=fVector1[2]*fVector2[0] - fVector1[0]*fVector2[2];
                    fNormal[2]=fVector1[0]*fVector2[1] - fVector1[1]*fVector2[0];
                    //push triangle coordinate normals into tree
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[1].fXNormal = fNormal[0];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[1].fYNormal = fNormal[1];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[1].fZNormal = fNormal[2];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[2].fXNormal = fNormal[0];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[2].fYNormal = fNormal[1];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[2].fZNormal = fNormal[2];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[3].fXNormal = fNormal[0];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[3].fYNormal = fNormal[1];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[3].fZNormal = fNormal[2];
                    //2,0,3
                    fVertex1[0] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[2].fXCoordinate;
                    fVertex1[1] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[2].fYCoordinate;
                    fVertex1[2] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[2].fZCoordinate;
                    fVertex2[0] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[0].fXCoordinate;
                    fVertex2[1] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[0].fYCoordinate;
                    fVertex2[2] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[0].fZCoordinate;
                    fVertex3[0] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[3].fXCoordinate;
                    fVertex3[1] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[3].fYCoordinate;
                    fVertex3[2] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[3].fZCoordinate;
                    //finds cross product of the vectors
                    fVector1[0]=fVertex2[0]-fVertex1[0];
                    fVector1[1]=fVertex2[1]-fVertex1[1];
                    fVector1[2]=fVertex2[2]-fVertex1[2];
                    fVector2[0]=fVertex3[0]-fVertex1[0];
                    fVector2[1]=fVertex3[1]-fVertex1[1];
                    fVector2[2]=fVertex3[2]-fVertex1[2];
                    fNormal[0]=fVector1[1]*fVector2[2] - fVector1[2]*fVector2[1];
                    fNormal[1]=fVector1[2]*fVector2[0] - fVector1[0]*fVector2[2];
                    fNormal[2]=fVector1[0]*fVector2[1] - fVector1[1]*fVector2[0];
                    //push triangle coordinate normals into tree
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[2].fXNormal = fNormal[0];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[2].fYNormal = fNormal[1];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[2].fZNormal = fNormal[2];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[0].fXNormal = fNormal[0];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[0].fYNormal = fNormal[1];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[0].fZNormal = fNormal[2];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[3].fXNormal = fNormal[0];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[3].fYNormal = fNormal[1];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[3].fZNormal = fNormal[2];
                    //0,1,2
                    fVertex1[0] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[0].fXCoordinate;
                    fVertex1[1] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[0].fYCoordinate;
                    fVertex1[2] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[0].fZCoordinate;
                    fVertex2[0] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[1].fXCoordinate;
                    fVertex2[1] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[1].fYCoordinate;
                    fVertex2[2] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[1].fZCoordinate;
                    fVertex3[0] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[2].fXCoordinate;
                    fVertex3[1] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[2].fYCoordinate;
                    fVertex3[2] = pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[2].fZCoordinate;
                    //finds cross product of the vectors
                    fVector1[0]=fVertex2[0]-fVertex1[0];
                    fVector1[1]=fVertex2[1]-fVertex1[1];
                    fVector1[2]=fVertex2[2]-fVertex1[2];
                    fVector2[0]=fVertex3[0]-fVertex1[0];
                    fVector2[1]=fVertex3[1]-fVertex1[1];
                    fVector2[2]=fVertex3[2]-fVertex1[2];
                    fNormal[0]=fVector1[1]*fVector2[2] - fVector1[2]*fVector2[1];
                    fNormal[1]=fVector1[2]*fVector2[0] - fVector1[0]*fVector2[2];
                    fNormal[2]=fVector1[0]*fVector2[1] - fVector1[1]*fVector2[0];
                    //push triangle coordinate normals into tree
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[0].fXNormal = fNormal[0];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[0].fYNormal = fNormal[1];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[0].fZNormal = fNormal[2];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[1].fXNormal = fNormal[0];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[1].fYNormal = fNormal[1];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[1].fZNormal = fNormal[2];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[2].fXNormal = fNormal[0];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[2].fYNormal = fNormal[1];
                    pInorderTraverse->ElementInfo_bdf.ivecElementConnectivity[2].fZNormal = fNormal[2];
                }
            }
        }
        SetNormalInorder(pInorderTraverse->pNoderight); //Traverse and display Rightside Tree nodes
    }
}

void CElement_bdf::SetNormal()
{
    SetNormalInorder();
}

char* CElement_bdf::ElementName(int iIter)
{
    return this->cpvecEelementNames[iIter];
}

int CElement_bdf::ElementStrength(int iIter)
{
    return this->ivecElementStrength[iIter];
}

int CElement_bdf::TotalElementStrength()
{
    int iElemStrength = 0;
    for(int i = 0; i < ivecElementStrength.size(); i++)
    {
        iElemStrength += ivecElementStrength[i];
    }
    return iElemStrength;
}

void CElement_bdf::ClearEelementNamesStrength()
{
    cpvecEelementNames.clear();
    ivecElementStrength.clear();
}

void CElement_bdf::SetNbrofSubCases(int iNbrofSubCases)
{
    miNbrofSubCases = iNbrofSubCases;
}

int CElement_bdf::GetNbrofSubCases()
{
    return miNbrofSubCases;
}

void CElement_bdf::SetElemStrength(int iElemCount)
{   ivecElementStrength.reserve(1);
    ivecElementStrength[0] = iElemCount;
}

int CElement_bdf::NumberofElementTypes()
{
    return this->cpvecEelementNames.size();
}

CElement_bdf::~CElement_bdf()
{
    //Clears Memory Allocated for Storing SBinaryTreeNode_bdf Data
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




