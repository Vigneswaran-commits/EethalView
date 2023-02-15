#include "cnode.h"
#include "celement.h"
#include "NastranOp2Parser/op2/nastranop2.h"
#include "NastranOp2Parser/Node/nastranop2node.h"
#include "NastranOp2Parser/Element/nastranop2element.h"
#include "NastranOp2Parser/Utilities/utilities.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
using namespace std;

bool NastranOp2::mbSwapEndians = true ;

NastranOp2::NastranOp2()
{
    //ctor
    miSubCaseId = 1;
}

bool NastranOp2::Header(ifstream &fstrFileStream,string &strHeaderName,int *iTrailerBits)
{
    try
    {
        //The first three words in all IFP Tables uniquely identify or label the contents of the record and are called
        //"header words"
        //For eg : Record 17 -- GRID(4501,45,1) --->Here the trailer bit fstrFileStream 45
        string strError = "NastranOp2::Header, unexpected key value";
        vector<int> ivecBlock;
        ivecBlock.reserve(100);//reserves a capacity for 100 int's
        int iNumberofWords;
        // Detects end of file, or reads header name :
        Utilities::ReadNastranWords(fstrFileStream,1,ivecBlock,mbSwapEndians);
        if (ivecBlock.at(0)==0)
            return true; // end of file !
        if (ivecBlock.at(0)!=2)
        {
            throw(strError);
        }
        Utilities::ReadNastranWords(fstrFileStream,2,ivecBlock,mbSwapEndians);

        Utilities::Op2Word_to_String(&ivecBlock.at(0),2,strHeaderName,true);//DataBlock name 'hname' fstrFileStream changed here

        Utilities::ReadNastranWords(fstrFileStream,1,ivecBlock,mbSwapEndians);
        if (ivecBlock.at(0)!=-1)
        {
            throw(strError);
        }

        // Reads trailer ://Trailer bit determines the presence of a record type in a datablock
        //ie., if the record fstrFileStream present in the table, then the bit fstrFileStream turned ON in the trailer.

        Utilities::ReadNastranWords(fstrFileStream,1,ivecBlock,mbSwapEndians);
        if (ivecBlock.at(0)!=7)
        {
            throw(strError);
        }
        Utilities::ReadNastranWords(fstrFileStream,7,ivecBlock,mbSwapEndians);
        for (int i=0; i<7; i++)
        {
            iTrailerBits[i]=ivecBlock.at(i);
//            cout<<"Trailer ON/OFF : "<<iTrailerBits[i]<<"\n";
        }
        Utilities::ReadNastranWords(fstrFileStream,1,ivecBlock,mbSwapEndians);
        if (ivecBlock.at(0)!=-2)
        {
            throw(strError);
        }

        // Reads other unknown stuff :

        Utilities::ReadNastranWords(fstrFileStream,1,ivecBlock,mbSwapEndians);
        if (ivecBlock.at(0)!=1)
        {
            throw(strError);
        }
        Utilities::ReadNastranWords(fstrFileStream,1,ivecBlock,mbSwapEndians);
        if (ivecBlock.at(0)!=0)
        {
            throw(strError);
        }
        Utilities::ReadNastranWords(fstrFileStream,1,ivecBlock,mbSwapEndians);
        if (ivecBlock.at(0)<2)
        {
            throw(strError);
        }
        iNumberofWords=ivecBlock.at(0);
        Utilities::ReadNastranWords(fstrFileStream,iNumberofWords,ivecBlock,mbSwapEndians); // IH ?

        Utilities::ReadNastranWords(fstrFileStream,1,ivecBlock,mbSwapEndians);
        if (ivecBlock.at(0)!=-3)
        {
            throw(strError);
        }

        return false;
    }
    catch(string strCaughtError)
    {
        cout<<strCaughtError<<"\n";
    }
    catch(...)//ellipsis cathces any type of exceptions thrown
    {
        cout<<"Utilities::Op2Word_to_String, Default exception fstrFileStream thrown while converting op2 words to string\n";
    }
}

int NastranOp2::Read_DataBlockTable(std::ifstream &fstrFileStream,std::vector<int> &ivecBlock,int &iLastInteger)
{
    //this method is common for all IFP and OFP datablock tables
    try
    {
        int iLength,iNumberofWords;
        string strError = "NastranOp2::readTable, invalid value for iNumberofWords";
        vector<int> ivecTempBlock;
        ivecTempBlock.reserve(25000);

        Utilities::ReadNastranWords(fstrFileStream,1,ivecTempBlock,mbSwapEndians); //tmpblock[0] is usually 1
        //        for(int i = 0;i < ivecTempBlock.size();i++)
        //        cout<<ivecTempBlock[i]<<"\n";
        Utilities::ReadNastranWords(fstrFileStream,1,ivecTempBlock,mbSwapEndians); //tmpblock[0] is usually 0
        //        for(int i = 0;i < ivecTempBlock.size();i++)
        //        cout<<ivecTempBlock[i]<<"\n";
        //above values 00 - none;01 - ATOC;10 - PSDF;11 - RALL (doubt)

        iLength=0;
        ivecBlock.clear();
        do
        {
            Utilities::ReadNastranWords(fstrFileStream,1,ivecTempBlock,mbSwapEndians); // next record size
            //            for(int i = 0;i < ivecTempBlock.size();i++)
            //            cout<<ivecTempBlock[i]<<"\n";
            iNumberofWords=ivecTempBlock.at(0); //determines iLastInteger value in readop2()      // or int <= 0
            if (iNumberofWords>0) //if iNumberofWords is negative, no more words to read
            {
                if (iLength+iNumberofWords>static_cast<int>(ivecBlock.capacity()))
                    ivecBlock.reserve(2*(iLength+iNumberofWords));
                Utilities::ReadNastranWords(fstrFileStream,iNumberofWords,ivecTempBlock,mbSwapEndians);//iNumberofWords x 4bytes are read
                //                for(int i = 0;i < ivecTempBlock.size();i++)
                //                cout<<ivecTempBlock[i]<<"\n";
                if (iNumberofWords>static_cast<int>(ivecTempBlock.size())||iNumberofWords<0)
                {
                    throw(strError);
                }
                for (int i=0; i<iNumberofWords; i++)
                    ivecBlock.push_back(ivecTempBlock[i]);//Table values are pushed into ivecBlock array for scanning records
                iLength+=iNumberofWords;
            }
        }
        while (iNumberofWords>0) ;

        iLastInteger=iNumberofWords;//Updates the integer 'iLastInteger' in NastranOp2::ReadNastranOp2File()

        return iLength;
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

int NastranOp2::GetDataBlockType(string strHeaderName)
{
    try
    {
        if (strHeaderName.compare(0,5,"GEOM1")==0                   // IFP Tables
                ||strHeaderName.compare(0,5,"GEOM2")==0
                ||strHeaderName.compare(0,5,"GEOM3")==0
                ||strHeaderName.compare(0,5,"GEOM4")==0
                ||strHeaderName.compare(0,3,"EPT")==0
                ||strHeaderName.compare(0,3,"MPT")==0
                ||strHeaderName.compare(0,7,"PCOMPTS")==0)
        {
            return 1;
        }
        else if (strHeaderName.compare(0,3,"OAG")==0                // OFP Tables
                 ||strHeaderName.compare(0,3,"OEF")==0
                 ||strHeaderName.compare(0,3,"OES")==0
                 ||strHeaderName.compare(0,4,"OGPF")==0
                 ||strHeaderName.compare(0,4,"ONRG")==0
                 ||strHeaderName.compare(0,3,"OPG")==0
                 ||strHeaderName.compare(0,3,"OQG")==0
                 ||strHeaderName.compare(0,3,"OQM")==0
                 ||strHeaderName.compare(0,4,"OSTR")==0
                 ||strHeaderName.compare(0,3,"OUG")==0
                 ||strHeaderName.compare(0,6,"BOPHIG")==0
                 ||strHeaderName.compare(0,6,"BOUGV1")==0
                 ||strHeaderName.compare(0,3,"OVG")==0)
        {
            return 2;
        }
        else                                                // Unknown Tables
        {
            return 0;
        }
    }
    catch(...)//ellipsis cathces any type of exceptions thrown
    {
        cout<<"NastranOp2::GetDataBlockType, Default exception thrown while checking datablock type\n";
    }
}

void NastranOp2::OpenNastranOp2File(string strFileLocation,ifstream &fstrFileStream)
{
    try
    {
        string strDate,strTitle,strTempLabel,strLabel;
        int iKey;
        vector<int> ivecBlock;
        ivecBlock.reserve(100);
        //strFileLocation.c_str()--Generates a null-terminated sequence of characters(c-string)
        fstrFileStream.open(strFileLocation.c_str(),ios_base::in | ios_base::binary);
        if(!fstrFileStream.is_open())
        {
            throw(strFileLocation);
        }
        mbSwapEndians=Match_Op2_Cpu_Endiannes(fstrFileStream);//checks for endianness match and set the boolean once for all
        // Date :
        Utilities::ReadNastranWords(fstrFileStream,1,ivecBlock,mbSwapEndians);
        iKey=ivecBlock.at(0);
        if (iKey!=-1)
            Utilities::ReadNastranWords(fstrFileStream,iKey,ivecBlock,mbSwapEndians);
        Utilities::Op2Word_to_String(&ivecBlock.at(0),iKey,strDate,true);
        // Title :
        Utilities::ReadNastranWords(fstrFileStream,1,ivecBlock,mbSwapEndians);
        iKey=ivecBlock.at(0);
        if (iKey!=-1)
            Utilities::ReadNastranWords(fstrFileStream,iKey,ivecBlock,mbSwapEndians);
        Utilities::Op2Word_to_String(&ivecBlock.at(0),iKey,strTitle,true);
        // Label :
        Utilities::ReadNastranWords(fstrFileStream,1,ivecBlock,mbSwapEndians);
        Utilities::Op2Word_to_String(&ivecBlock.at(0),1,strTempLabel,true);
        strLabel.append(strTempLabel);
        Utilities::ReadNastranWords(fstrFileStream,2,ivecBlock,mbSwapEndians);
        Utilities::Op2Word_to_String(&ivecBlock.at(0),2,strTempLabel,true);
        strLabel.append(strTempLabel);
        Utilities::ReadNastranWords(fstrFileStream,1,ivecBlock,mbSwapEndians);
        Utilities::Op2Word_to_String(&ivecBlock.at(0),1,strTempLabel,true);
        strLabel.append(strTempLabel);
        // End of reading :
        Utilities::ReadNastranWords(fstrFileStream,1,ivecBlock,mbSwapEndians);
    }
    catch(string strFileName)
    {
        cout<<"NastranOp2Reader::OpenNastranOp2File, failed to open op2 file : "<<strFileName<<"\n";
    }
    catch(...)//ellipsis cathces any type of exceptions thrown
    {
        cout<<"NastranOp2Reader::OpenNastranOp2File, Default exception fstrFileStream thrown while opening op2 file\n";
    }
}

bool NastranOp2::Match_Op2_Cpu_Endiannes(ifstream &FileStream)
{
    bool bOp2Endian,bCpuEndian;
    char iptrCharacter[4];

    //check op2 endian
    FileStream.read(iptrCharacter,4);//The code reads the first word of the op2 binary file
    if(FileStream.bad())
        cout<<"NastranOp2::Match_Op2_Cpu_Endiannes, failed read operation\n";
    if(iptrCharacter[0]==0)
        bOp2Endian = true;
    else
        bOp2Endian = false;
    FileStream.seekg(0);//sets the position of next byte to be extracted at 0

    //check cpu endian
    short int iFourByteWord = 0x0001;
    char* iptrCpuCharacter = (char*) &iFourByteWord;
    if(iptrCpuCharacter[0]==0)
        bCpuEndian = true;
    else
        bCpuEndian = false;


    //if op2 file endians doesn't match with the current cpu endians, returns True or else returns False
    if(bOp2Endian != bCpuEndian)
        return true;
    else
        return false;
}

bool NastranOp2::GetSwapEndians()
{
    return mbSwapEndians;
}

void NastranOp2::ScanIFPTableRecords(string strHeaderName,int *trailer,int iNumber,vector<int> &ivecBlock,Cnode& oNodeData,CElement& oElementData)
{
    try
    {
//        cout<<ivecBlock.at(0)<<" "<<ivecBlock.at(1)<<" "<<ivecBlock.at(2)<<"\n";
        if (strHeaderName.compare(0,5,"GEOM1")==0)
        {
            //matches the first three values(eg:Record 17 -- GRID(4501,45,1)) to find the appropriate records under GEOM1
            if ((ivecBlock.at(0)==1701&&ivecBlock.at(1)==17&&ivecBlock.at(2)==6)
                    ||(ivecBlock.at(0)==1801&&ivecBlock.at(1)==18&&ivecBlock.at(2)==5)
                    ||(ivecBlock.at(0)==1901&&ivecBlock.at(1)==19&&ivecBlock.at(2)==7)
                    ||(ivecBlock.at(0)==2001&&ivecBlock.at(1)==20&&ivecBlock.at(2)==9)
                    ||(ivecBlock.at(0)==2101&&ivecBlock.at(1)==21&&ivecBlock.at(2)==8)
                    ||(ivecBlock.at(0)==2201&&ivecBlock.at(1)==22&&ivecBlock.at(2)==10))
            {
//                coordSys::ScanOp2Record(db,strHeaderName,iNumber,ivecBlock) ;
            }
            else if (ivecBlock.at(0)==4501&&ivecBlock.at(1)==45&&ivecBlock.at(2)==1)
            {
                CNastranOp2Node::ScanOp2Record(strHeaderName,iNumber,ivecBlock,oNodeData) ;//declared in Scope--postlib::nastran::node::ScanOp2Record()
            }
            else if (ivecBlock.at(0)==65535&&ivecBlock.at(1)==65535&&ivecBlock.at(2)==65535)
            {
                // Nothing to do !
            }
            else
            {
                cout << "      Skipping IFP record(" << ivecBlock.at(0)
                     << "," << ivecBlock.at(1)
                     << "," << ivecBlock.at(2) << ")"
                     << " of length " << iNumber
                     << " (strHeaderName=\"" << strHeaderName << "\")\n";
            }
        }
        else if (strHeaderName.compare(0,5,"GEOM2")==0)
        {
            if (ivecBlock.at(0)==65535&&ivecBlock.at(1)==65535&&ivecBlock.at(2)==65535)
            {
                // Nothing to do !
            }
            else
            {
                //call depends on the number of element types--need to confirm
                CNastranOp2Element::ScanOp2Record(strHeaderName,iNumber,ivecBlock,oElementData) ;//declared in Scope--postlib::nastran::element::ScanOp2Record()
            }
        }
        else if (strHeaderName.compare(0,5,"GEOM3")==0)
        {
            // Nothing to do !
        }
        else if (strHeaderName.compare(0,5,"GEOM4")==0)
        {
            if (ivecBlock.at(0)==65535&&ivecBlock.at(1)==65535&&ivecBlock.at(2)==65535)
            {
                // Nothing to do !
            }
            else
            {
                // mpc::ScanOp2Record(db,strHeaderName,iNumber,ivecBlock) ;//declared in Scope--postlib::nastran::mpc::ScanOp2Record()
            }
        }
        else if (strHeaderName.compare(0,3,"EPT")==0)
        {
            if (ivecBlock.at(0)==65535&&ivecBlock.at(1)==65535&&ivecBlock.at(2)==65535)
            {
                // Nothing to do !
            }
            else
            {
//                elemprop::ScanOp2Record(db,strHeaderName,iNumber,ivecBlock) ;//declared in Scope--postlib::nastran::elemprop::ScanOp2Record()
            }
        }
        else if (strHeaderName.compare(0,3,"MPT")==0)
        {
            if (ivecBlock.at(0)==65535&&ivecBlock.at(1)==65535&&ivecBlock.at(2)==65535)
            {
                // Nothing to do !
            }
            else
            {
                // material::ScanOp2Record(db,strHeaderName,iNumber,ivecBlock) ;
            }
        }
        else if (strHeaderName.compare(0,7,"PCOMPTS")==0)
        {
//            elemprop::scanOp2Pcompts(db,strHeaderName,iNumber,ivecBlock) ;//declared in Scope--postlib::nastran::elemprop::ScanOp2Record()
        }
        else
        {
            cout << "      Skipping IFP record(" << ivecBlock.at(0)
                 << "," << ivecBlock.at(1)
                 << "," << ivecBlock.at(2) << ")"
                 << " of length " << iNumber
                 << " (strHeaderName=\"" << strHeaderName << "\")\n";
        }
    }
    catch(...)
    {
        cout<<"NastranOp2::ScanIFPTableRecord, Default exception thrown while scanning IFP table record\n";
    }
}

NastranOp2::~NastranOp2()
{
    //dtor
}

