#include "node.h"
#include <iostream>
using namespace std;

Cnodegraph::Cnodegraph()
{
}

int Cnodegraph::Set_RootKey(int iNodecount)
{
    m_pNodeRoot = NULL;   //Set Root to  point NUlL
    m_iNo_of_Nodes = 0;   //Set Current No of Nodes to Null

    int iRootKey;
    if((iNodecount%2) == 0)
    {
        iRootKey = iNodecount/2;          /*takes the middle id as root key value*/
    }
    else if((iNodecount%2) != 0)
    {
        iRootKey = iNodecount/2;          /*for odd key values,the nearest middle integer  is the key value */
    }
    m_iRoot_Key = iRootKey;
    return iRootKey;
}

void Cnodegraph::Set_Rootnode(Snodegraph* pNodeRoot)
{
    m_pNodeRoot = pNodeRoot;
}

void Cnodegraph::Insert_Node_Attributes(int iIdentity,float iXcoord,float iYcoord,float iZcoord)
{
    Snodegraph* pNodeTmpr = NULL;
    pNodeTmpr = m_pNodeRoot;
    Insert_Node_Attributes(pNodeTmpr,iIdentity,iXcoord,iYcoord,iZcoord);
}

void Cnodegraph::Insert_RootNode_Attributes(float iXcoord,float iYcoord,float iZcoord)
{
    Snodegraph* pNodeTmpr = NULL;
    Snodegraph* pNodeCheck_Root = NULL;
    pNodeTmpr = new Snodegraph();
    pNodeTmpr->iIdentification = m_iRoot_Key;
    pNodeTmpr->iXcord = iXcoord;
    pNodeTmpr->iYcord = iYcoord;
    pNodeTmpr->iZcord = iZcoord;
    pNodeTmpr->pNodeleft = NULL;
    pNodeTmpr->pNoderight = NULL;
    m_pNodeRoot = pNodeTmpr;
    pNodeCheck_Root = m_pNodeRoot;
    if(m_iNo_of_Nodes == 0)
    {
        m_iNo_of_Nodes++;
    }
}


void Cnodegraph::Insert_Node_Attributes(Snodegraph* &pNodeTmpr,int iIdentity,float iXcoord,float iYcoord,float iZcoord)
{
    if(pNodeTmpr == NULL)
    {
        pNodeTmpr = new Snodegraph();
        pNodeTmpr->iIdentification = iIdentity;
        pNodeTmpr->iXcord = iXcoord;
        pNodeTmpr->iYcord = iYcoord;
        pNodeTmpr->iZcord = iZcoord;
        pNodeTmpr->pNodeleft = NULL;
        pNodeTmpr->pNoderight = NULL;
        m_iNo_of_Nodes++;
    }
    else if(iIdentity < pNodeTmpr->iIdentification)
    {
        Insert_Node_Attributes(pNodeTmpr->pNodeleft,iIdentity,iXcoord,iYcoord,iZcoord);
    }
    else if(iIdentity > pNodeTmpr->iIdentification)
    {
        Insert_Node_Attributes(pNodeTmpr->pNoderight,iIdentity,iXcoord,iYcoord,iZcoord);
    }
    else if(iIdentity == pNodeTmpr->iIdentification)
    {
        cout<<"A node with same key value "<<iIdentity<<" already exists\n";
        cout<<"Please enter a valid key value\n";
    }
}

Snodegraph* Cnodegraph::Search_Node_Return_Attributes(int iIdentity)
{
    Snodegraph* pNodeTmpr = NULL;
    pNodeTmpr = m_pNodeRoot;
    Search_Node_Return_Attributes(pNodeTmpr,iIdentity);
}

Snodegraph* Cnodegraph::Search_Node_Return_Attributes(Snodegraph* pNodeTmpr,int iIdentity)
{
    if(iIdentity == pNodeTmpr->iIdentification)
    {
        return pNodeTmpr;
    }
    else if((iIdentity < pNodeTmpr->iIdentification) && (pNodeTmpr->pNodeleft == NULL))
    {
        cout<<"No Matches for the Identification :"<<iIdentity<<"\n";
        return NULL;
    }
    else if((iIdentity > pNodeTmpr->iIdentification) && (pNodeTmpr->pNoderight == NULL))
    {
        cout<<"No Matches for the Identification :"<<iIdentity<<"\n";
        return NULL;
    }
    else if(iIdentity < pNodeTmpr->iIdentification && pNodeTmpr->pNodeleft != NULL)
    {
        Search_Node_Return_Attributes(pNodeTmpr->pNodeleft,iIdentity);
    }
    else if(iIdentity > pNodeTmpr->iIdentification && pNodeTmpr->pNoderight != NULL)
    {
        Search_Node_Return_Attributes(pNodeTmpr->pNoderight,iIdentity);
    }
}

void Cnodegraph::Delete_Node_At_ID(int iIdentity)
{
    Snodegraph* pNode_Deletion = NULL;
    pNode_Deletion = Search_Node_Return_Attributes(iIdentity);
    if((pNode_Deletion->pNodeleft == NULL) && (pNode_Deletion->pNoderight == NULL))
    {
        delete pNode_Deletion;
        pNode_Deletion = NULL;
    }
    else if((pNode_Deletion->pNodeleft == NULL) || (pNode_Deletion->pNoderight == NULL))
    {
        Snodegraph* pNode_Deletion_Right = NULL;
        Snodegraph* pNode_Deletion_Left = NULL;
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
}

int Cnodegraph::Count_No_of_Nodes()
{
    int iNode_Count;
    iNode_Count = m_iNo_of_Nodes;
    return iNode_Count;
}

void Cnodegraph::Display_Node_Keys_Inorder()
{
    Snodegraph* pNodeTraverse = NULL;
    pNodeTraverse = m_pNodeRoot;
    Display_Node_Keys_Inorder(pNodeTraverse);
}

void Cnodegraph::Display_Node_Keys_Inorder(Snodegraph* pNodeTraverse)
{
    if(pNodeTraverse != NULL)
    {
        cout<<pNodeTraverse->iIdentification<<"\n";
        Free_Memory(pNodeTraverse->pNodeleft);  //Traverse and delete leftside Tree nodes

        Free_Memory(pNodeTraverse->pNoderight); //Traverse and delete Rightside Tree nodes
        //Inorder and Preorder are not fit for freeing memory
    }
}

void Cnodegraph::Free_Memory()
{
    Snodegraph* pNodeTmpr = NULL;
    pNodeTmpr = m_pNodeRoot;
    if(pNodeTmpr)
      Free_Memory(pNodeTmpr);
}

void Cnodegraph::Free_Memory(Snodegraph* pNodeDelete)
{
    if(pNodeDelete != NULL)
    {
        Free_Memory(pNodeDelete->pNodeleft);  //Traverse and delete leftside Tree nodes
        Free_Memory(pNodeDelete->pNoderight); //Traverse and delete Rightside Tree nodes
        delete pNodeDelete;
        pNodeDelete=NULL;
        //Inorder and Preorder are not fit for freeing memory
    }
}

Cnodegraph::~Cnodegraph()
{
    Free_Memory();
}




