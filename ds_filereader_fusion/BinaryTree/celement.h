//C++ class for storing the node values in tree structure
#ifndef ELEMENT_H_INCLUDED
#define ELEMENT_H_INCLUDED
#include <string.h>
#include <iostream>
#include <vector>
#include <string>
#include "cnode.h"
using namespace std;
//contains element's connectiivity node coordinates
struct SNodeCoordinates
{
    int iNodeId;
    float fXNormal,fYNormal,fZNormal;
    float fXCoordinate,fYCoordinate,fZCoordinate;
};
//element stresses and strains
struct SElement_StressStrain
{
    int iElementType; //eg 1d,2d,3d element
    //stress
    float fStressX,fStressY,fStressZ,fStressXY,fStressYZ,fStressXZ; //stress components
    float fStressX_rbg[3],fStressY_rbg[3],fStressZ_rbg[3],fStressXY_rbg[3],fStressYZ_rbg[3],fStressXZ_rbg[3]; // for legend
    //2-dimension
    float fStress_ShearAngle;
    float fMajorPrincipalStress;
    float fMinorPrincipalStress;
    float fVonmisesStress;                                   //or(maximum shear stress)
    //3-dimension
    float fFirstPrincipalStress,fSecondPrincipalStress,fThirdPrincipalStress;
    //strain
    float fStrainX,fStrainY,fStrainZ,fStrainXY,fStrainYZ,fStrainXZ; //strain components
    //2-dimension
    float fStrain_ShearAngle;
    float fMajorPrincipalStrain;
    float fMinorPrincipalStrain;
    float fVonmisesStrain;                                   //or(maximum shear strain)
    //3-dimension
    float fFirstPrincipalStrain,fSecondPrincipalStrain,fThirdPrincipalStrain;
};
//output element energy
struct SOutput_ElementEnergy
{
    float fStrainEnergy,fPercent_of_TotalStrainEnergy,fStrainEnergyDensity;
    float fKineticEnergy,fPercent_of_TotalKineticEnergy,fKineticEnergyDensity;
    float fEnergyLoss,fPercent_of_TotalEnergyLoss,fEnergyLossDensity;
};
struct SElementInfo
{
    //element type,name,connectivity
    int iElementType;                   //eg scalar,axisymmetric,rigid element
    const char* pcElementName;                //eg cbar element
    std::vector<SNodeCoordinates> ivecElementConnectivity;
    //elemental results
    std::vector<SElement_StressStrain> vecStressStrainData; //vector for subcases
    std::vector<SOutput_ElementEnergy> vecElementEnergy;   //vector for subcases
};
struct SBinaryTreeNode         //struct wraps Tree node values and its structure
{
    unsigned int uiIdentification;//element id is the root key
    bool bRed;
    SBinaryTreeNode* pNodeleft;         //since root node in pointed to null in constructor
    SBinaryTreeNode* pNoderight;
    SBinaryTreeNode* pNodeParent;
    //elemental model and result info
    SElementInfo ElementInfo;
};
class CElement
{
private:
    unsigned int m_uiRoot_Key;
    unsigned int m_uiNo_of_Nodes;
    SBinaryTreeNode* mpNodeRoot;
    SBinaryTreeNode* mpBlackSentinelLeaf;

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
    CElement();
    CElement(const CElement &ElemObj);  //copy constructor: it's safe now to pass and return CElement object as value
    int Set_RootKey(unsigned int uiNodecount);
    void Insert_RootNode_Attributes();
    void Insert_Node_Attributes(unsigned int uiIdentity,int iElemType,char* pcElemName,int iElemStrength,std::vector<int> iElemConnectNodes); //overloaded function
    void Insert_Node_Attributes(SBinaryTreeNode* &pCurrentsParentNode,SBinaryTreeNode* &pCurrentNode,unsigned int uiIdentity,int iElemType,char* pcElemName,std::string strNodePosition,std::vector<int> iElemConnectNodes);
//    void RedBlackInsertion(SBinaryTreeNode* &pCurrentNode);
    SBinaryTreeNode*& SearchandReturn_CurrentNode(unsigned int uiIdentity);
    SBinaryTreeNode*& SearchandReturn_CurrentNode(SBinaryTreeNode* &pSearchedNode,unsigned int uiIdentity);
    SBinaryTreeNode*& SearchandReturn_Parentnode(unsigned int uiIdentity);
    SBinaryTreeNode*& SearchandReturn_Parentnode(SBinaryTreeNode* &pParentNode,SBinaryTreeNode* &pSearchedNode,unsigned int uiIdentity,std::string &strChildPosition);
    SBinaryTreeNode*& GetRootNode();
    int Count_No_of_Nodes();
    void Display_Depth_First_Search_Preorder();
    //root-left-right. used for completely duplicating binary trees
    void Display_Node_Keys_Preorder();
    void Display_Node_Keys_Preorder(SBinaryTreeNode* pNodeTraverse);
    //left-root-right.used in binary search trees
    void Display_Node_Keys_Inorder();
    void Display_Node_Keys_Inorder(SBinaryTreeNode* pInorderTraverse);
    //left-right-root. used for deleting all nodes
    void Display_Node_Keys_Postorder();
    void Display_Node_Keys_Postorder(SBinaryTreeNode* pNodeTraverse);
    void Delete_Node_At_ID(unsigned int uiIdentity);
    void Delete_Node_At_ID(SBinaryTreeNode* &pNode_Deletion);
    void Free_Memory();
    void Free_Memory(SBinaryTreeNode* &pNodeDelete);

    void RotateLeft(SBinaryTreeNode *pCurrentNode);
    void RotateRight(SBinaryTreeNode *pCurrentNode);
    void RedBlackTreeInsertion(SBinaryTreeNode *pCurrentNode);

    void SetCoordinates2ConnectingNodesInorder(Cnode& NodeInfo);
    void SetCoordinates2ConnectingNodesInorder(SBinaryTreeNode* &pInorderTraverse,Cnode& NodeInfo);
    void SetNodeCoord2ConnectingNodes(Cnode& NodeInfo);
    //display methods
    void SetNormalInorder();
    void SetNormalInorder(SBinaryTreeNode* &pInorderTraverse);
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
    //set element stresses and strains
    bool ElemStressPresence();
    bool ElemStrainPresence();
    void SetElemStress_Available(bool mbElemStress_Presence);
    void SetElemStrain_Available(bool mbElemStrain_Presence);
    void SetElementStresses(SBinaryTreeNode* &pCurrentNode,int iSubCaseId,int iElemType,float fStressComponents[6],float fStress_ShearAngle,
                            float fMajorPrincipalStress,float fMinorPrincipalStress,float fVonmisesStress,
                            float fFirstPrincipalStress,float fSecondPrincipalStress,float fThirdPrincipalStress);
    void SetElementStrains(SBinaryTreeNode* &pCurrentNode,int iSubCaseId,int iElemType,float fStrainComponents[6],float fStrain_ShearAngle,
                           float fMajorPrincipalStrain,float fMinorPrincipalStrain,float fVonmisesStrain,
                           float fFirstPrincipalStrain,float fSecondPrincipalStrain,float fThirdPrincipalStrain);
    //set element energies
    bool StrainEnergyPresence();
    bool KineticEnergyPresence();
    bool EnergyLossPresence();
    void SetStrainEnergyData_Available(bool bStrainEnergyPresence);
    void SetKineticEnergyData_Available(bool bKineticEnergyPresence);
    void SetEnergyLossData_Available(bool bEnergyLossPresence);
    void SetStrainEnergyValues(SBinaryTreeNode* &pCurrentNode,int iSubCaseId,float fStrainEnergy,float fPercent_of_TotalStrainEnergy,float fStrainEnergyDensity);
    void SetKineticEnergyValues(SBinaryTreeNode* &pCurrentNode,int iSubCaseId,float fKineticEnergy,float fPercent_of_TotalKineticEnergy,float fKineticEnergyDensity);
    void SetEnergyLossValues(SBinaryTreeNode* &pCurrentNode,int iSubCaseId,float fEnergyLoss,float fPercent_of_TotalEnergyLoss,float fEnergyLossDensity);
    ~CElement();
};
#endif
