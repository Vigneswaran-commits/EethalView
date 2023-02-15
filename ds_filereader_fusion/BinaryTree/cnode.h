//C++ class for storing the node values in tree structure
#ifndef NODE_H_INCLUDED
#define NODE_H_INCLUDED
#include <string.h>
#include <iostream>
#include <vector>
//mesh node coordinates
struct SNodeCoord
{
    int iNodeId;
    int iLocalCoordinateSystemId;
    float fXCoordinate,fYCoordinate,fZCoordinate;
    int iDOF_CoordinateSystemId;
    int iPermanentSinglePointConstraints;
    int iSuperelementId;
};
//output node displacements
struct SOutputDisplacement //may contain displacement/velocity/acceleration depending upon analysis type
{
    int iResultType;    //=1 for vector,=2 for scalar values
    int iResultContent;//=0 for displacment,=1 for velocity,=2 for acceleration
    float fTranslationX,fTranslationY,fTranslationZ;
    float fRotationX,fRotationY,fRotationZ;
    float fScalarValue; //can be temperature
};
//applied loads at nodes
struct SNodalAppliedLoad
{
    int iResultType;    //=1 for vector,=2 for scalar values
    float fAppliedForce_Direction1,fAppliedForce_Direction2,fAppliedForce_Direction3;
    float fAppliedMoment_Direction1,fAppliedMoment_Direction2,fAppliedMoment_Direction3;
    float fScalarValue; //can be pressure
};
//constraint forces(spc,mpc forces)
struct SConstraintForce
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
struct SNodeInfo
{
    //node coordinates
    SNodeCoord NodePosition;
    //nodal results
    std::vector<SOutputDisplacement> vecNodeDisplacement;
    std::vector<SNodalAppliedLoad> vecNodalAppliedLoad;
    std::vector<SConstraintForce> vecConstraintForce;
};
//struct wraps Tree node values and its structure
struct Snode
{
    unsigned int uiIdentification;//mesh node id is the key
    bool bRed;
    Snode* pNodeleft;         //since root node in pointed to null in constructor
    Snode* pNoderight;
    Snode* pNodeParent;
    //mesh and node result information
    SNodeInfo NodeInfo;
};
class Cnode
{
private:
    unsigned int m_uiRoot_Key;
    unsigned int m_uiNo_of_Nodes;
    Snode* mpNodeRoot;
    Snode* mpBlackSentinelLeaf;
    //total number of nodes in fe mesh
    int iNoofNodes;
    //set to true according to results availability in file
    bool mbNodeDisplacementPresence;
    bool mbNodalAppliedLoad_Presence;
    bool mbSpcForcePresence,mbMpcForcePresence;
    //nbr of subcases
    int miNbrofSubCases;
public:
    Cnode();
    int Set_RootKey(unsigned int uiNodecount);
    Snode*& GetRootNode();
    void Insert_RootNode_Attributes();
    void Insert_Node_Attributes(unsigned int uiIdentity,int iNodeId,int iLocalCoordSysId,float fXCoord,float fYCoord,float fZCoord,int iDofCoordSysId,int iPermanentSinglePointConstraints,int iSuperelementId); //overloaded function
    void Insert_Node_Attributes(Snode* &pCurrentsParentNode,Snode* &pCurrentNode,unsigned int uiIdentity,std::string strNodePosition,int iNodeId,int iLocalCoordSysId,float fXCoord,float fYCoord,float fZCoord,int iDofCoordSysId,int iPermanentSinglePointConstraints,int iSuperelementId);
    Snode*& SearchandReturn_CurrentNode(unsigned int uiIdentity);
    Snode*& SearchandReturn_CurrentNode(Snode* &pSearchedNode,unsigned int uiIdentity);
    Snode*& SearchandReturn_Parentnode(unsigned int uiIdentity);
    Snode*& SearchandReturn_Parentnode(Snode* &pParentNode,Snode* &pSearchedNode,unsigned int uiIdentity,std::string &strChildPosition);
    int Count_No_of_Nodes();
    void Display_Depth_First_Search_Preorder();
    //root-left-right. used for completely duplicating binary trees
    void Display_Node_Keys_Preorder();
    void Display_Node_Keys_Preorder(Snode* pNodeTraverse);
    //left-root-right.used in binary search trees
    void Display_Node_Keys_Inorder();
    void Display_Node_Keys_Inorder(Snode* pInorderTraverse);
    //left-right-root. used for deleting all nodes
    void Display_Node_Keys_Postorder();
    void Display_Node_Keys_Postorder(Snode* pNodeTraverse);
    void Delete_Node_At_ID(unsigned int uiIdentity);
    void Delete_Node_At_ID(Snode* &pNode_Deletion);
    void Free_Memory();
    void Free_Memory(Snode* &pNodeDelete);

    void RotateLeft(Snode *pCurrentNode);
    void RotateRight(Snode *pCurrentNode);
    void RedBlackTreeInsertion(Snode *pCurrentNode);

    //total number of nodes in the fe mesh
    void SetNoofNodes(int iNoofNodes);
    int GetNoofNodes();
    //number of subcases
    void SetNbrofSubCases(int iNbrofSubCases);
    int GetNbrofSubCases();
    //set node output displacement
    bool NodeDisplacementPresence();
    void SetNodeDisplacement_Available(bool bNodeDisplacementPresence);
    void SetNode_OutputDisplacement(Snode* &pCurrentNode,int iSubCaseId,int iResultType,int iResultContent,float frgTranslation[3],float frgRotation[3]);
    void SetNode_OutputTemperature(Snode* &pCurrentNode,int iSubCaseId,int iResultType,float fScalar);
    //set applied loads at nodes
    bool NodalAppliedLoadPresence();
    void SetNodalAppliedLoad_Available(bool bNodalAppliedLoad_Presence);
    void ResizeAppliedLoadsatNodes(Snode* pInorderTraverse);
    void ResizeAppliedLoadsatNodes_Inorder(Snode* pInorderTraverse);
    void SetAppliedLoadsatNodes(Snode* &pCurrentNode,int iSubCaseId,int iResultType,float frgAppliedForce[3],float frgAppliedMoment[3]);
    void SetAppliedScalarLoadsatNodes(Snode* &pCurrentNode,int iSubCaseId,int iResultType,float fScalar);
    //set constraint(spc,mpc) forces
    bool SpcForcePresence();
    bool MpcForcePresence();
    void SetSpcForceAvailable(bool bSpcForcePresence);
    void SetMpcForceAvailable(bool bMpcForcePresence);
    void ResizeConstraintForces(Snode* pInorderTraverse);
    void ResizeConstraintForces_Inorder(Snode* pInorderTraverse);
    void SetSpcConstraintForcesatNodes(Snode* &pCurrentNode,int iSubCaseId,int iResultType,float frgConstraintForce[3],float frgConstraintMoment[3]);
    void SetMpcConstraintForcesatNodes(Snode* &pCurrentNode,int iSubCaseId,int iResultType,float frgMpcConstraintForce[3],float frgMpcConstraintMoment[3]);
    //add method to set scalar data from oqg(dont know the scalar data name)
    ~Cnode();
};
#endif
