//C++ class for storing the node values in tree structure
#ifndef CNODE_H_INCLUDED
#define CNODE_H_INCLUDED
struct Snodegraph
{
    int iIdentification;
    float iXcord;
    float iYcord;
    float iZcord;
    Snodegraph* pNodeleft;         //since root node in pointed to null in constructor,
    Snodegraph* pNoderight;       //no need of making it to point null
};
class Cnodegraph
{
private:

    int m_iRoot_Key;
    int m_iNo_of_Nodes;
    Snodegraph* m_pNodeRoot;

public:
    void Insert_Node_Attributes(Snodegraph* &pNodeTmpr,int iIdentity,float iXcoord,float iYcoord,float iZcoord);
    Snodegraph* Search_Node_Return_Attributes(Snodegraph* pNodeTmpr,int iIdentity);
    void Delete_Node_At_ID(int iIdentity);
    void Display_Node_Keys_Inorder();
    void Display_Node_Keys_Inorder(Snodegraph* pNodeTraverse);
    void Free_Memory();
    void Free_Memory(Snodegraph* pNodeDelete);

//public:
    Cnodegraph();
    void Set_Rootnode(Snodegraph* pNodeRoot);
    int Set_RootKey(int iNodecount);
    void Insert_RootNode_Attributes(float iXcoord,float iYcoord,float iZcoord);
    void Insert_Node_Attributes(int iIdentity,float iXcoord,float iYcoord,float iZcoord); //overloading function
    Snodegraph* Search_Node_Return_Attributes(int iIdentity);
    int Count_No_of_Nodes();
    ~Cnodegraph();
};
#endif





