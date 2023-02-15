#include "celement.h"
#include "NastranOp2Parser/Element/NastranOp2Element.h"
#include "NastranOp2Parser/Utilities/utilities.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
using namespace std;

void CNastranOp2Element::ScanOp2Record(string strHeaderName,int iTableLength,vector<int> &ivecBlock,CElement& oElementData)
{
    try
    {
        int iElementId,iElementConnectNodes,iNbrofElements;
        int iElementType = 0;
        char* cpElementName = NULL;
        int iNumber1,iNumber2,iNumber3;
        int iNumberofWords,iAllocateNumberofWords;
        int iElemId_Iter,iIter;
        int *ip_Pointer;//Integer pointer 'ip_Pointer' is made to point at the ivecBlock begining

        //Integer pointer 'ip_Pointer' is made to point at the ivecBlock begining
        ip_Pointer=&ivecBlock[0];//ip_Pointer tracks the current location in the file till the reading is completed

        iNumber1=Utilities::Op2Word_to_Int(ip_Pointer);
        ip_Pointer++;                              //++ increments to the next element in the vector
        iNumber2=Utilities::Op2Word_to_Int(ip_Pointer);//element type id
        ip_Pointer++;
        iNumber3=Utilities::Op2Word_to_Int(ip_Pointer);
        ip_Pointer++;

        //GEOM2--Record 69 -- CQUAD4(2958,51,177) & CTRIA3
        //Reference pgno 277 & 286 in 2006//237 & 245 in 2012
        iNumberofWords=CNastranOp2Element::GetNbrofWordsFromTypeId(iNumber2);
        iAllocateNumberofWords=iNumberofWords;
//            if (iNumber2==SElementType::CQUAD8&&op2::getPostParam()==-2)
//                iNumberofWords=16;
        if (iNumber2==SElementType::CFAST||iNumber2==SElementType::CFASTP)
            iAllocateNumberofWords=11;

        if ((iTableLength-3)%iNumberofWords)
        {
            string StrTemp = SElementType::GetNameFromId(iTableLength);
            cout << "      Skipping Problematic " << StrTemp
                 << " IFP record(" << ivecBlock[0]
                 << "," << ivecBlock[1]
                 << "," << ivecBlock[2] << ")"
                 << " of length " << iTableLength << endl ;
            return;
        }

        iNbrofElements=(iTableLength-3)/iNumberofWords;
        //cout<<"No of Elements : "<<iNbrofElements<<"\n";
        if (iNumberofWords>0)
        {
            for (iElemId_Iter = 0; iElemId_Iter < iNbrofElements; iElemId_Iter++)
            {
                vector<int> ivecConnectivityNodes;
                iElementId = Utilities::Op2Word_to_Int(ip_Pointer+0);//  cout<<"Element Id : "<<iElementId<<"\n";
                //*************************************
                switch (iNumber2)
                {
                case SElementType::CFAST :
                {
//                    int formatFlag=ip_Pointer[3];
//                    tmpElement.words[0]=ip_Pointer[0];
//                    tmpElement.words[1]=ip_Pointer[1];
//                    tmpElement.words[2]=2;
//                    tmpElement.words[3]=0;
//                    tmpElement.words[4]=0;
//                    tmpElement.words[5]=ip_Pointer[2];
//                    tmpElement.words[6]=ip_Pointer[4];
//                    tmpElement.words[7]=ip_Pointer[5];
//                    if (formatFlag==9||formatFlag==10)
//                    {
//                        tmpElement.words[8]=ip_Pointer[16];
//                        tmpElement.words[9]=ip_Pointer[17];
//                        tmpElement.words[10]=ip_Pointer[18];
//                    }
//                    else
//                    {
//                        ((float*)tmpElement.words)[8]=0.0;
//                        ((float*)tmpElement.words)[9]=0.0;
//                        ((float*)tmpElement.words)[10]=0.0;
//                    }
                }
                break;
                case SElementType::CFASTP :
                {
//                    tmpElement.words[0]=ip_Pointer[0];
//                    tmpElement.words[1]=ip_Pointer[1];
//                    tmpElement.words[2]=1;
//                    tmpElement.words[3]=0;
//                    tmpElement.words[4]=0;
//                    tmpElement.words[5]=ip_Pointer[2];
//                    tmpElement.words[6]=ip_Pointer[4];
//                    tmpElement.words[7]=ip_Pointer[5];
//                    tmpElement.words[8]=ip_Pointer[42];
//                    tmpElement.words[9]=ip_Pointer[43];
//                    tmpElement.words[10]=ip_Pointer[44];
                }
                break;
                //1D elements
                //inertia2.op2 has cbeam elements
                //no example op2 found
                case SElementType::CBEAM ://2012 doc,pgno217//Eid,Pid,GA,GB,...//Geom2 Record 11--CBEAM(5408,54,261)
                    for (iIter=0; iIter<iNumberofWords; iIter++)//
                    {
                        iElementConnectNodes=ip_Pointer[iIter]; //cout<<"Element Connectivity : "<<iElementConnectNodes<<"\n";
                        if(iIter==2 || iIter==3)
                            ivecConnectivityNodes.push_back(iElementConnectNodes);
                    }
                    {
                        iElementType = 3;
                        cpElementName = "CBEAM";
                    }
                    break;
                    //nastran_static_solution_1.op2 has cbar elements
                case SElementType::CBAR ://2012 doc,pgno215//Eid,Pid,GA,GB,...//Geom2 Record 9--CBAR(2408,24,180)
                    for (iIter=0; iIter<iNumberofWords; iIter++)//
                    {
                        iElementConnectNodes=ip_Pointer[iIter]; //cout<<"Element Connectivity : "<<iElementConnectNodes<<"\n";
                        if(iIter==2 || iIter==3)
                            ivecConnectivityNodes.push_back(iElementConnectNodes);
                    }
                    {
                        iElementType = 3;
                        cpElementName = "CBAR";
                    }
                    break;
                case SElementType::CROD ://2012 doc,pgno241//Eid,Pid,G(2)//Geom2 Record 81--CROD(3001,30,48)
                    for (iIter=0; iIter<iNumberofWords; iIter++)//
                    {
                        iElementConnectNodes=ip_Pointer[iIter]; //cout<<"Element Connectivity : "<<iElementConnectNodes<<"\n";
                        if(iIter==2 || iIter==3)
                            ivecConnectivityNodes.push_back(iElementConnectNodes);
                    }
//                        cout<<"\n";
                    {
                        iElementType = 3;
                        cpElementName = "CROD";
                    }
                    break;
                    //2D elements
                case SElementType::CTRIA3 ://EID,PID,G(3),THETA,ZOFFS,UNDEF(2),TFLAG,T(3)
                    for (iIter=0; iIter<iNumberofWords; iIter++)//
                    {
                        iElementConnectNodes=ip_Pointer[iIter];
                        // cout<<"Element Connectivity : "<<iElementConnectNodes<<"\n";
                        if(iIter == 2 || iIter == 3 || iIter == 4)
                            ivecConnectivityNodes.push_back(iElementConnectNodes);
                    }
                    {
                        iElementType = 4;
                        cpElementName = "CTRIA3";
                    }
                    break;
                    //boot1.op2 contains both cquad4 and ctria3 elements
                case SElementType::CQUAD4 ://EID,PID,G(4),THETA,ZOFFS,UNDEF,TFLAG,T(4) //Record 69 -- CQUAD4(2958,51,177)
                    for (iIter=0; iIter<iNumberofWords; iIter++)//
                    {
                        iElementConnectNodes=ip_Pointer[iIter];
                        if(iIter==2 || iIter==3 || iIter==4 || iIter==5)
                            ivecConnectivityNodes.push_back(iElementConnectNodes);
                    }
                    {
                        iElementType = 4;
                        cpElementName = "CQUAD4";
                    }
                    break;
                    //3D elements
                    //solidBending.op2 contains ctetra elements.
                    //ctetra is either 4 or 10(including mid-side nodes) nodes.
                    //if it is 10, then read the first 4 values alone.
                case SElementType::CTETRA ://2012 doc,pgno244//Eid,Pid,G(10)//Geom2 Record 88--CTETRA(5508,55,217)
                    for (iIter=0; iIter<iNumberofWords; iIter++)//
                    {
                        iElementConnectNodes=ip_Pointer[iIter]; //cout<<"Element Connectivity : "<<iElementConnectNodes<<"\n";
                        if(iIter==2 || iIter==3 || iIter==4 || iIter==5)
                            ivecConnectivityNodes.push_back(iElementConnectNodes);
                    }
                    {
                        iElementType = 5;
                        cpElementName = "CTETRA";
                    }
                    //bullet_local.op2 contains chexa elements.
                case SElementType::CHEXA ://2012 doc,pgno232//Eid,Pid,G(20)//Geom2 Record 46--CHEXA(7308,73,253)
                    for (iIter=0; iIter<iNumberofWords; iIter++)//
                    {
                        iElementConnectNodes=ip_Pointer[iIter]; //cout<<"Element Connectivity : "<<iElementConnectNodes<<"\n";
                        if(iIter==2 || iIter==3 || iIter==4 || iIter==5 || iIter==6 || iIter==7 || iIter==8|| iIter==9)
                            ivecConnectivityNodes.push_back(iElementConnectNodes);
                    }
                    {
                        iElementType = 5;
                        cpElementName = "CHEXA";
                    }
                    break;
                case SElementType::CPENTA ://2012 doc,pgno236//Eid,Pid,G(15)//Geom2 Record 63--CHEXA(4108,41,280)
                    for (iIter=0; iIter<iNumberofWords; iIter++)//
                    {
                        iElementConnectNodes=ip_Pointer[iIter]; //cout<<"Element Connectivity : "<<iElementConnectNodes<<"\n";
                        if(iIter==2 || iIter==3 || iIter==4 || iIter==5 || iIter==6 || iIter==7)
                            ivecConnectivityNodes.push_back(iElementConnectNodes);
                    }
                    {
                        iElementType = 5;
                        cpElementName = "CPENTA";
                    }
                    break;
                default :
                    for (iIter=0; iIter<iNumberofWords; iIter++)
                        iElementConnectNodes=ip_Pointer[iIter];
                    break;
                }
                //************end of switch*************
                ip_Pointer+=iNumberofWords;
                //inserts element connectivity nodes
                //tree leaves identity=0,tree root identity=1,so element with id 1 is omitted while insertion(need to check)
                oElementData.Insert_Node_Attributes(iElementId,iElementType,cpElementName,iNbrofElements,ivecConnectivityNodes);//sets elem id,connectivity nodes
            }
        }
        else
        {
            cout << "         Skipping IFP record(" << ivecBlock[0]
                 << "," << ivecBlock[1]
                 << "," << ivecBlock[2] << ")"
                 << " of length " << iTableLength << endl ;
            return;
        }
    }
    catch(...)
    {
        cout << "         Problematic IFP record(" << ivecBlock[0]
             << "," << ivecBlock[1]
             << "," << ivecBlock[2] << ")"
             << " of length " << iTableLength << endl ;
    }
}
