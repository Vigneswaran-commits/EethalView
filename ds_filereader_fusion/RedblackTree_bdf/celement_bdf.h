//C++ class for storing the node values in tree structure
#ifndef ELEMENT_BDF_H_INCLUDED
#define ELEMENT_BDF_H_INCLUDED
#include <string.h>
#include <iostream>
#include <vector>
#include <string>
#include "cnode.h"
using namespace std;
//contains element's connectiivity node coordinates
struct SNodeCoordinates_bdf
{
    int iNodeId;
    float fXNormal,fYNormal,fZNormal;
    float fXCoordinate,fYCoordinate,fZCoordinate;
};

struct SElementInfo_bdf
{
    //element type,name,connectivity
    int iElementType;                   //eg scalar,axisymmetric,rigid element
    const char* pcElementName;                //eg cbar element
    std::vector<SNodeCoordinates_bdf> ivecElementConnectivity;
    //elemental results
};
struct SBinaryTreeNode_bdf         //struct wraps Tree node values and its structure
{
    unsigned int uiIdentification;//element id is the root key
    bool bRed;
    SBinaryTreeNode_bdf* pNodeleft;         //since root node in pointed to null in constructor
    SBinaryTreeNode_bdf* pNoderight;
    SBinaryTreeNode_bdf* pNodeParent;
    //elemental model and result info
    SElementInfo_bdf ElementInfo_bdf;
};
class CElement_bdf
{
private:
    unsigned int m_uiRoot_Key;
    unsigned int m_uiNo_of_Nodes;
    SBinaryTreeNode_bdf* mpNodeRoot;
    SBinaryTreeNode_bdf* mpBlackSentinelLeaf;

    //available element names and their counts in finite element mesh
    std::vector<char*> cpvecEelementNames;  //stores all the available elem type names(cbar,crod etc)
    std::vector<int> ivecElementStrength;   //stores the total number of each type of elements(cbar=120,crod=200 etc)

    //set to true according to results availability in file
    bool mbElemStress_Presence,mbElemStrain_Presence;
    bool mbStrainEnergyPresence,mbKineticEnergyPresence,mbEnergyLossPresence;//set to true if strain,kinetic,loss is present

    //nbr of subcases
    int miNbrofSubCases;

public:
    //min values for stress components
    vector<float> fStressX_min; vector<float> fStressY_min; vector<float> fStressZ_min;
    vector<float> fStressXY_min;vector<float> fStressYZ_min;vector<float> fStressXZ_min;
    //max values for stress components
    vector<float> fStressX_max; vector<float> fStressY_max; vector<float> fStressZ_max;
    vector<float> fStressXY_max;vector<float> fStressYZ_max;vector<float> fStressXZ_max;
public:
    CElement_bdf();
    CElement_bdf(const CElement_bdf &ElemObj);  //copy constructor: it's safe now to pass and return CElement object as value
    int Set_RootKey(unsigned int uiNodecount);
    void Insert_RootNode_Attributes();
    void Insert_Node_Attributes(unsigned int uiIdentity,int iElemType,char* pcElemName,int iElemStrength,std::vector<int> iElemConnectNodes); //overloaded function
    void Insert_Node_Attributes(SBinaryTreeNode_bdf* &pCurrentsParentNode,SBinaryTreeNode_bdf* &pCurrentNode,unsigned int uiIdentity,int iElemType,char* pcElemName,std::string strNodePosition,std::vector<int> iElemConnectNodes);
//    void RedBlackInsertion(SBinaryTreeNode_bdf* &pCurrentNode);
    SBinaryTreeNode_bdf*& SearchandReturn_CurrentNode(unsigned int uiIdentity);
    SBinaryTreeNode_bdf*& SearchandReturn_CurrentNode(SBinaryTreeNode_bdf* &pSearchedNode,unsigned int uiIdentity);
    SBinaryTreeNode_bdf*& SearchandReturn_Parentnode(unsigned int uiIdentity);
    SBinaryTreeNode_bdf*& SearchandReturn_Parentnode(SBinaryTreeNode_bdf* &pParentNode,SBinaryTreeNode_bdf* &pSearchedNode,unsigned int uiIdentity,std::string &strChildPosition);
    SBinaryTreeNode_bdf*& GetRootNode();
    int Count_No_of_Nodes();
    void Display_Depth_First_Search_Preorder();
    //root-left-right. used for completely duplicating binary trees
    void Display_Node_Keys_Preorder();
    void Display_Node_Keys_Preorder(SBinaryTreeNode_bdf* pNodeTraverse);
    //left-root-right.used in binary search trees
    void Display_Node_Keys_Inorder();
    void Display_Node_Keys_Inorder(SBinaryTreeNode_bdf* pInorderTraverse);
    //left-right-root. used for deleting all nodes
    void Display_Node_Keys_Postorder();
    void Display_Node_Keys_Postorder(SBinaryTreeNode_bdf* pNodeTraverse);
    void Delete_Node_At_ID(unsigned int uiIdentity);
    void Delete_Node_At_ID(SBinaryTreeNode_bdf* &pNode_Deletion);
    void Free_Memory();
    void Free_Memory(SBinaryTreeNode_bdf* &pNodeDelete);

    void RotateLeft(SBinaryTreeNode_bdf *pCurrentNode);
    void RotateRight(SBinaryTreeNode_bdf *pCurrentNode);
    void RedBlackTreeInsertion(SBinaryTreeNode_bdf *pCurrentNode);

    void SetCoordinates2ConnectingNodesInorder(Cnode& NodeInfo);
    void SetCoordinates2ConnectingNodesInorder(SBinaryTreeNode_bdf* &pInorderTraverse,Cnode& NodeInfo);
    void SetNodeCoord2ConnectingNodes(Cnode& NodeInfo);
    //display methods
    void SetNormalInorder();
    void SetNormalInorder(SBinaryTreeNode_bdf* &pInorderTraverse);
    void SetNormal();

    //returns available element names and their counts in fe mesh
    void SetElemStrength(int iElemCount);//for stl
    int NumberofElementTypes();
    char* ElementName(int iIter);
    int ElementStrength(int iIter);
    int TotalElementStrength();
    void ClearEelementNamesStrength();
    //number of subcases
    void SetNbrofSubCases(int iNbrofSubCases);
    int GetNbrofSubCases();
    ~CElement_bdf();
};
#endif
