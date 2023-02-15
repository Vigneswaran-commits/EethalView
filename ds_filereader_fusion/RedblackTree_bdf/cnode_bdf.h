//C++ class for storing the node values in tree structure
#ifndef NODE_BDF_H_INCLUDED
#define NODE_BDF_H_INCLUDED
#include <string.h>
#include <iostream>
#include <vector>
//mesh node coordinates
struct SNodeCoord_bdf
{
    int iNodeId;
    int iLocalCoordinateSystemId;
    float fXCoordinate,fYCoordinate,fZCoordinate;
    int iDOF_CoordinateSystemId;
    int iPermanentSinglePointConstraints;
    int iSuperelementId;
};
//applied loads at nodes
struct SNodalAppliedLoad_bdf
{
    int iResultType;    //=1 for vector,=2 for scalar values
    float fAppliedForce_Direction1,fAppliedForce_Direction2,fAppliedForce_Direction3;
    float fAppliedMoment_Direction1,fAppliedMoment_Direction2,fAppliedMoment_Direction3;
    float fScalarValue; //can be pressure
};
//constraint forces(spc,mpc forces)
struct SConstraintForce_bdf
{
    int iResultType;    //=1 for vector,=2 for scalar values
    //spc constraint forces
    float fSpcConstraintForce_Direction1,fSpcConstraintForce_Direction2,fSpcConstraintForce_Direction3;
    float fSpcConstraintMoment_Direction1,fSpcConstraintMoment_Direction2,fSpcConstraintMoment_Direction3;
    //mpc constraint forces
    float fMpcConstraintForce_Direction1,fMpcConstraintForce_Direction2,fMpcConstraintForce_Direction3;
    float fMpcConstraintMoment_Direction1,fMpcConstraintMoment_Direction2,fMpcConstraintMoment_Direction3;
    float fScalarValue; //??
};
//struct wraps both mesh node coordinates and their results
struct SNodeInfo_bdf
{
    //node coordinates
    SNodeCoord_bdf NodePosition;
    //nodal results
    std::vector<SNodalAppliedLoad_bdf> vecNodalAppliedLoad;
    std::vector<SConstraintForce_bdf> vecConstraintForce;
};
//struct wraps Tree node values and its structure
struct Snode_bdf
{
    unsigned int uiIdentification;//mesh node id is the key
    bool bRed;
    Snode_bdf* pNodeleft;         //since root node in pointed to null in constructor
    Snode_bdf* pNoderight;
    Snode_bdf* pNodeParent;
    //mesh and node result information
    SNodeInfo_bdf NodeInfo;
};
class Cnode_bdf
{
private:
    unsigned int m_uiRoot_Key;
    unsigned int m_uiNo_of_Nodes;
    Snode_bdf* mpNodeRoot;
    Snode_bdf* mpBlackSentinelLeaf;
    //total number of nodes in fe mesh
    int iNoofNodes;
    //set to true according to results availability in file
    bool mbNodeDisplacementPresence;
    bool mbNodalAppliedLoad_Presence;
    bool mbSpcForcePresence,mbMpcForcePresence;
    //nbr of subcases
    int miNbrofSubCases;
public:
    Cnode_bdf();
    int Set_RootKey(unsigned int uiNodecount);
    Snode_bdf*& GetRootNode();
    void Insert_RootNode_Attributes();
    void Insert_Node_Attributes(unsigned int uiIdentity,int iNodeId,int iLocalCoordSysId,float fXCoord,float fYCoord,float fZCoord,int iDofCoordSysId,int iPermanentSinglePointConstraints,int iSuperelementId); //overloaded function
    void Insert_Node_Attributes(Snode_bdf* &pCurrentsParentNode,Snode_bdf* &pCurrentNode,unsigned int uiIdentity,std::string strNodePosition,int iNodeId,int iLocalCoordSysId,float fXCoord,float fYCoord,float fZCoord,int iDofCoordSysId,int iPermanentSinglePointConstraints,int iSuperelementId);
    Snode_bdf*& SearchandReturn_CurrentNode(unsigned int uiIdentity);
    Snode_bdf*& SearchandReturn_CurrentNode(Snode_bdf* &pSearchedNode,unsigned int uiIdentity);
    Snode_bdf*& SearchandReturn_Parentnode(unsigned int uiIdentity);
    Snode_bdf*& SearchandReturn_Parentnode(Snode_bdf* &pParentNode,Snode_bdf* &pSearchedNode,unsigned int uiIdentity,std::string &strChildPosition);
    int Count_No_of_Nodes();
    void Display_Depth_First_Search_Preorder();
    //root-left-right. used for completely duplicating binary trees
    void Display_Node_Keys_Preorder();
    void Display_Node_Keys_Preorder(Snode_bdf* pNodeTraverse);
    //left-root-right.used in binary search trees
    void Display_Node_Keys_Inorder();
    void Display_Node_Keys_Inorder(Snode_bdf* pInorderTraverse);
    //left-right-root. used for deleting all nodes
    void Display_Node_Keys_Postorder();
    void Display_Node_Keys_Postorder(Snode_bdf* pNodeTraverse);
    void Delete_Node_At_ID(unsigned int uiIdentity);
    void Delete_Node_At_ID(Snode_bdf* &pNode_Deletion);
    void Free_Memory();
    void Free_Memory(Snode_bdf* &pNodeDelete);

    void RotateLeft(Snode_bdf *pCurrentNode);
    void RotateRight(Snode_bdf *pCurrentNode);
    void RedBlackTreeInsertion(Snode_bdf *pCurrentNode);

    //total number of nodes in the fe mesh
    void SetNoofNodes(int iNoofNodes);
    int GetNoofNodes();
    //number of subcases
    void SetNbrofSubCases(int iNbrofSubCases);
    int GetNbrofSubCases();
    //set applied loads at nodes
    bool NodalAppliedLoadPresence();
    void SetNodalAppliedLoad_Available(bool bNodalAppliedLoad_Presence);
    void ResizeAppliedLoadsatNodes(Snode_bdf* pInorderTraverse);
    void ResizeAppliedLoadsatNodes_Inorder(Snode_bdf* pInorderTraverse);
    void SetAppliedLoadsatNodes(Snode_bdf* &pCurrentNode,int iSubCaseId,int iResultType,float frgAppliedForce[3],float frgAppliedMoment[3]);
    void SetAppliedScalarLoadsatNodes(Snode_bdf* &pCurrentNode,int iSubCaseId,int iResultType,float fScalar);
    //set constraint(spc,mpc) forces
    bool SpcForcePresence();
    bool MpcForcePresence();
    void SetSpcForceAvailable(bool bSpcForcePresence);
    void SetMpcForceAvailable(bool bMpcForcePresence);
    void ResizeConstraintForces(Snode_bdf* pInorderTraverse);
    void ResizeConstraintForces_Inorder(Snode_bdf* pInorderTraverse);
    void SetSpcConstraintForcesatNodes(Snode_bdf* &pCurrentNode,int iSubCaseId,int iResultType,float frgConstraintForce[3],float frgConstraintMoment[3]);
    void SetMpcConstraintForcesatNodes(Snode_bdf* &pCurrentNode,int iSubCaseId,int iResultType,float frgMpcConstraintForce[3],float frgMpcConstraintMoment[3]);
    //add method to set scalar data from oqg(dont know the scalar data name)
    ~Cnode_bdf();
};
#endif
