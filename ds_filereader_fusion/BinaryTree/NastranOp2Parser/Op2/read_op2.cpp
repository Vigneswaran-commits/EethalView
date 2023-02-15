#include "cnode.h"
#include "celement.h"
#include "NastranOp2Parser/op2/nastranop2.h"
#include "NastranOp2Parser/Utilities/utilities.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
using namespace std;

#define NONE 0
#define  OUG 1
#define  OPG 2
#define  OQG 3
#define  OEF 4
#define  OES 5
#define  OGF 6



void NastranOp2::ReadNastranOp2File(std::string strOp2FileName,Cnode& oNodeData,CElement& oElementData)
{
    try
    {
        int eof;
        ifstream is;

        string strHeaderName;
        int irgTrailer[7],iTableBlockType,iLastInt,iLength1,iLength2;
        vector<int> iBlock,shortBlock;
        //shortBlock is Vector for storing the header block
        //iBlock is Vector for storing the table block
        iBlock.reserve(1000000);
        shortBlock.reserve(1000);

        NastranOp2::OpenNastranOp2File(strOp2FileName,is);
        cout << "\n";
        cout << "Loading file " << strOp2FileName << "..." << "\n";
        eof=false;

        while (!eof)//***loop for reading different data blocks till the end of file***
        {
            // Reading header :
            eof = NastranOp2::Header(is,strHeaderName,irgTrailer);//DataBlock name 'strHeaderName' is changed here & eof is set to
            if(eof)
            {
                //cout<<"eof is true\n";
                return;
            }
            iTableBlockType=NastranOp2::GetDataBlockType(strHeaderName);//Determines 'strHeaderName' is Model or Result datablock
            if (iTableBlockType == 1)         // IFP Tables(Reads Model data)
            {
                //cout << "   Reading IFP Block \"" << strHeaderName << "\"" << endl ;
                do
                {
                    iLength1=NastranOp2::Read_DataBlockTable(is,iBlock,iLastInt);//Table(Record) values are stored in iBlock vector & later
                    //the words are read seperately
                    if (iLastInt != 0)
                        NastranOp2::ScanIFPTableRecords(strHeaderName,irgTrailer,iLength1,iBlock,oNodeData,oElementData);
                }
                while (iLastInt != 0);
            }
            else if (iTableBlockType == 2)   // OFP Tables(Reads Results data)
            {
                miSubCaseId = 1;
                //cout << "   Reading OFP Block \"" << strHeaderName << "\"" << endl ;
                iLength1 = iLength2 = -1;
                do
                {
                    static vector<int> lastHeader;//Vector for storing the header block
                    //iBlock is Vector for storing the table block
                    iLength1 = NastranOp2::Read_DataBlockTable(is,shortBlock,iLastInt);//Returns length of header block
                    if (static_cast<int>(shortBlock.size()) != iLength1)
                    {
                        string strError = "NastranOp2::ReadNastranOp2File, shortBlock.size()!=iLength1";
                        throw(strError);
                    }
                    else if (shortBlock.size() != 0)//Mostly Executed
                        lastHeader = shortBlock;
                    else
                        iLength1 = lastHeader.size();

                    if (iLastInt != 0)
                    {
                        iLength2 = NastranOp2::Read_DataBlockTable(is,iBlock,iLastInt);//Returns length of table block
                        NastranOp2::ScanOp2Record(strHeaderName,iLength1,lastHeader,iLength2,iBlock,oNodeData,oElementData);
                    }
                }
                while (iLastInt != 0) ;
            }
            else                            //Unknown Tables
            {
                //cout << "   Skipping Unknown Block \"" << strHeaderName << "\"" << endl ;
                do
                {
                    NastranOp2::Read_DataBlockTable(is,iBlock,iLastInt);
                }
                while (iLastInt != 0) ;
            }
        }
    }
    catch(string strCaughtError)
    {
        cout<<strCaughtError<<"\n";
    }
    catch(...)//ellipsis cathces any type of exceptions thrown
    {
        cout<<"NastranOp2::readTable, Default exception thrown while reading datablock table\n";
    }
}

