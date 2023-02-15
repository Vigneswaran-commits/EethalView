#include "NastranOp2Parser/Utilities/utilities.h"
#include "NastranOp2Parser/op2/nastranop2.h"
#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
using namespace std;

Utilities::Utilities()
{
    //ctor
}

void Utilities::swapEndiannes4(int *iBlock)
{
    static unsigned int nbrA,nbrB;
    nbrA=(unsigned int)iBlock[0];
    nbrB=(((nbrA&0x000000FF)<<24)+
          ((nbrA&0x0000FF00)<<8 )+
          ((nbrA&0x00FF0000)>>8 )+
          ((nbrA&0xFF000000)>>24));
    iBlock[0]=(int)nbrB;
}

void Utilities::swapEndiannes8(int *iBlock)
{
    static unsigned int nbrA1,nbrA2,nbrB1,nbrB2;
    nbrA1=(unsigned int)iBlock[0];
    nbrA2=(unsigned int)iBlock[1];
    nbrB1=(((nbrA2&0x000000FF)<<24)+
           ((nbrA2&0x0000FF00)<<8 )+
           ((nbrA2&0x00FF0000)>>8 )+
           ((nbrA2&0xFF000000)>>24));
    nbrB2=(((nbrA1&0x000000FF)<<24)+
           ((nbrA1&0x0000FF00)<<8 )+
           ((nbrA1&0x00FF0000)>>8 )+
           ((nbrA1&0xFF000000)>>24));
    iBlock[0]=(int)nbrB1;
    iBlock[1]=(int)nbrB2;
}

void Utilities::swapEndiannes(int &iBlock)
{
    swapEndiannes4(&iBlock);
}

void Utilities::swapEndiannes(int nbr,int *iBlock)
{
    static int i;
    for (i=0; i<nbr; i++)
        swapEndiannes(iBlock[i]);
}

void Utilities::Op2Word_to_String(const int *iptrOp2Words,int iNumberofWords,string &strName,bool bcancelBlanks)
{
    try
    {
        static char crgBuffer[257];
        int i,iNumberofCharacters;

        if (iNumberofWords>64)
        {
            string strError = "Utilities::Op2Word_to_String, Too many Op2 Words";
            throw(strError);
        }
        iNumberofCharacters=4*iNumberofWords;

        strncpy(crgBuffer,(char*)iptrOp2Words,iNumberofCharacters);
        if (NastranOp2::GetSwapEndians())
            swapEndiannes(iNumberofWords,(int*)crgBuffer);
        if (bcancelBlanks)
        {
            for (i=iNumberofCharacters-1; i>=0&&crgBuffer[i]==' '; i--)
                crgBuffer[i]=0;
        }
        crgBuffer[iNumberofCharacters]=0;
        strName=crgBuffer;//DataBlock name 'hname' is changed here
    }
    catch(string strCaughtError)
    {
        cout<<strCaughtError<<"\n";
    }
    catch(...)//ellipsis cathces any type of exceptions thrown
    {
        cout<<"Utilities::Op2Word_to_String, Default exception is thrown while converting op2 words to string\n";
    }
}

float Utilities::Op2Word_to_Float(const int *iblock)
{
    try
    {
        return *(float*)iblock;
    }
    catch(...)
    {
        cout<<"Utilities::Op2Word_to_Float, default exception is thrown while converting op2 word to float\n";
    }
}



int Utilities::Op2Word_to_Int(const int *iblock)
{
    try
    {
        return *iblock;
    }
    catch(...)
    {
        cout<<"Utilities::Op2Word_to_Int, default exception is thrown while converting op2 word to int\n";
    }
}

void Utilities::ReadNastranWords(ifstream &FileStream,int NbrofWords,vector<int> &ivecBlock,bool bSwapEndiannes)
{
    int iFirstFourBytes,iLastFourBytes,iCheck;
    char* iptrCharacter;
    string new_char;
    ivecBlock.clear();//removes all elements from vector leaving a size of 0
    if (NbrofWords > ivecBlock.capacity())
        ivecBlock.reserve(2*(NbrofWords)+10);//the records atmost 2*buffersize words long

    //read first 4 bytes
    iptrCharacter = (char*) &iFirstFourBytes;
    new_char=(char*)&iFirstFourBytes;
    FileStream.read(iptrCharacter,4);
    if(FileStream.bad() || FileStream.fail())
        cout<<"Utilities::ReadNastranWords, failed reading first four bytes\n";
    if (NastranOp2::GetSwapEndians())
        Utilities::swapEndiannes(iFirstFourBytes);
    if(iFirstFourBytes != NbrofWords*4)
    {
        //cout<<"Utilities::ReadNastranWords, FORTRAN record length first mismatch\n";
    }

    //read FirstBytes value bytes
    ivecBlock.resize(NbrofWords); //resized to nbr bytes
    iptrCharacter = (char*) &iCheck;
    iptrCharacter = (char*) &ivecBlock.at(0);
    new_char=(char*)&ivecBlock.at(0);
    FileStream.read(iptrCharacter,iFirstFourBytes);//reads iFirstFourBytes value number of words
//    if(FileStream.bad() || FileStream.fail())
//        cout<<"Utilities::ReadNastranWords, failed reading value bytes\n";
    if (NastranOp2::GetSwapEndians())
        for (int i=0; i<NbrofWords; i++)
            Utilities::swapEndiannes(ivecBlock[i]);

    //read last 4 bytes
    iptrCharacter = (char*) &iLastFourBytes;
    new_char=(char*)&iLastFourBytes;
    FileStream.read(iptrCharacter,4);
//    if(FileStream.bad() || FileStream.fail())
//        cout<<"Utilities::ReadNastranWords, failed reading last four bytes\n";
    if (NastranOp2::GetSwapEndians())
        Utilities::swapEndiannes(iLastFourBytes);
    if(iLastFourBytes != NbrofWords*4)
    {
        //cout<<"Utilities::ReadNastranWords, FORTRAN record length last mismatch\n";
    }
}

Utilities::~Utilities()
{
    //dtor
}
