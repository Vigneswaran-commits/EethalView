#include "cnode.h"
#include "NastranOp2Parser/Node/nastranop2node.h"
#include "NastranOp2Parser/Utilities/utilities.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
using namespace std;

void CNastranOp2Node::ScanOp2Record(string strHeaderName,int iTableLength,vector<int> &ivecBlock,Cnode& oNodeData)
{
    try
    {
        string strError = "Nastran::Op2, expected Record17 --Grid(4501,45,1), scanning stopped";
        int iNodeId,iLocationCoordSystemId;
        float fXCoordinate,fYCoordinate,fZCoordinate;
        int iDofCoordSystemId;
        float fPermanentSPC;
        int iSuperelementId;
        int iNumber1,iNumber2,iNumber3;
        int iNumberofWords, iNumberofNodes;
        int iNodeId_Iter,iNodeCord_Iter;
        vector<int>::const_iterator ivecit_Pointer;//ivecit_Pointer vector iterator is made to point at the ivecBlock begining
        //ivecit_Pointer is pointed to the begining of ivecBlock vector
        ivecit_Pointer=ivecBlock.begin();//ivecit_Pointer tracks the current location in the file till the reading is completed
        iNumber1=*ivecit_Pointer;
        ivecit_Pointer++;       //++ increments to the next element in the vector
        iNumber2=*ivecit_Pointer;
        ivecit_Pointer++;
        iNumber3=*ivecit_Pointer;
        ivecit_Pointer++;

        //GEOM1 : Record 17 -- GRID(4501,45,1)
        //Reference Manual - pgno 238 in 2006//202 in 2012 manual
        if (iNumber1==4501&&iNumber2==45&&iNumber3==1)
        {
            iNumberofWords=8;//No of Words is 8 (ID,CP,X1,X2,X3,CD,PS,SEID)
        }
        else
        {
            throw(strError);
        }

        iNumberofNodes=(iTableLength-3)/iNumberofWords;//Nof_nodes = (length from readTable() - 3)/No of Words
        //cout<<"No of Nodes : "<<iNumberofNodes<<"\n";
        oNodeData.SetNoofNodes(iNumberofNodes);         //sets node strength in fe mesh
        for (iNodeId_Iter = 0; iNodeId_Iter < iNumberofNodes; iNodeId_Iter++)
        {
            //grid point identification number
            iNodeId=*ivecit_Pointer;//cout<<"Node ID : "<<iNodeId<<"\n";
            ivecit_Pointer++;

            //location coordinate system identification number
            iLocationCoordSystemId=*ivecit_Pointer;//cout<<"Location Cord sys ID : "<<iLocationCoordSystemId<<"\n";
            ivecit_Pointer++;

            //x coordinate
            fXCoordinate=Utilities::Op2Word_to_Float(&ivecit_Pointer[0]);//cout<<fXCoordinate<<"    ";
            ivecit_Pointer++;

            //y coordinate
            fYCoordinate=Utilities::Op2Word_to_Float(&ivecit_Pointer[0]);//cout<<fYCoordinate<<"    ";
            ivecit_Pointer++;

            //z coordinate
            fZCoordinate=Utilities::Op2Word_to_Float(&ivecit_Pointer[0]);//cout<<fZCoordinate<<"    ";
            ivecit_Pointer++;
            //cout<<"\n";

            //Dof coordinate system identification number
            iDofCoordSystemId=*ivecit_Pointer;//cout<<"DOF cord sys IDno : "<<iDofCoordSystemId<<"\n";
            ivecit_Pointer++;

            //permanent single point constraints
            fPermanentSPC = *ivecit_Pointer;//cout<<"Single Point Constraint : "<<fPermanentSPC<<"\n";
            ivecit_Pointer++;

            //superelement identification number
            iSuperelementId = *ivecit_Pointer;//cout<<"Superelement_ID : "<<iSuperelementId<<"\n\n";
            ivecit_Pointer++;
            //inserts node info
            //tree leaves identity=0,tree root identity=1,so node with id 1 is omitted while insertion(need to check)
            oNodeData.Insert_Node_Attributes(iNodeId,iNodeId,iLocationCoordSystemId,fXCoordinate,fYCoordinate,fZCoordinate,iDofCoordSystemId,fPermanentSPC,iSuperelementId);
        }
    }
    catch(string strCaughtError)
    {
        cout<<strCaughtError<<"\n";
    }
}
