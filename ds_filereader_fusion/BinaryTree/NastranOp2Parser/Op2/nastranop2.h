#ifndef NASTRANOP2_H
#define NASTRANOP2_H
#include "cnode.h"
#include "celement.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
class NastranOp2
{
public:
    struct SResultElementType
    {

        // Supported element types for Results op2 output :

        static const int      ROD =   1 ;
        static const int     BEAM =   2 ;
        static const int     TUBE =   3 ;
        static const int    SHEAR =   4 ;
        static const int   CONROD =  10 ;
        static const int    ELAS1 =  11 ;
        static const int    ELAS2 =  12 ;
        static const int    ELAS3 =  13 ;
        static const int    ELAS4 =  14 ;
        static const int    DAMP1 =  20 ;
        static const int    DAMP2 =  21 ;
        static const int    DAMP3 =  22 ;
        static const int    DAMP4 =  23 ;
        static const int     VISC =  24 ;
        static const int    MASS1 =  25 ;
        static const int    MASS2 =  26 ;
        static const int    MASS3 =  27 ;
        static const int    MASS4 =  28 ;
        static const int    CONM1 =  29 ;
        static const int    CONM2 =  30 ;
        static const int   PLOTEL =  31 ;
        static const int    QUAD4 =  33 ;
        static const int      BAR =  34 ;
        static const int      GAP =  38 ;
        static const int    TETRA =  39 ;
        static const int   BUSH1D =  40 ;
        static const int    QUAD8 =  64 ;
        static const int     HEXA =  67 ;
        static const int    PENTA =  68 ;
        static const int     BEND =  69 ;
        static const int    TRIAR =  70 ;
        static const int    TRIA3 =  74 ;
        static const int    TRIA6 =  75 ;
        static const int    QUADR =  82 ;

        static const int  TETRANL =  85 ;
        static const int    GAPNL =  86 ;
        static const int   TUBENL =  87 ;
        static const int  TRIA3NL =  88 ;
        static const int    RODNL =  89 ;
        static const int  QUAD4NL =  90 ;
        static const int  PENTANL =  91 ;
        static const int CONRODNL =  92 ;
        static const int   HEXANL =  93 ;
        static const int   BEAMNL =  94 ;

        static const int  QUAD4LC =  95 ;
        static const int  QUAD8LC =  96 ;
        static const int  TRIA3LC =  97 ;
        static const int  TRIA6LC =  98 ;

        static const int     BUSH = 102 ;
        static const int    CFAST = 126 ;
        static const int  QUAD144 = 144 ;
        static const int  QUADRNL = 172 ;
        static const int  TRIARNL = 173 ;
        static const int  ELAS1NL = 224 ;
        static const int  ELAS3NL = 225 ;
        static const int   BUSHNL = 226 ;
        static const int  QUADRLC = 232 ;
        static const int  TRIARLC = 233 ;
        //struct methods
        static std::map<std::string,int> strimapElementNamesId ;

        static std::string GetNameFromId(int iTypeId) ;
        static int GetIdFromName(const std::string &iTypeName) ;

        static std::vector<bool> bvecEnabledCards ;//sizeof(bvecEnabledCards)==250

        static bool isEnabled(int iElementType) ;
        static bool isEnabled(const std::string &strTypeName) ;
    } ;

    NastranOp2();
    bool Header(std::ifstream &fstrFileStream,std::string &strHeaderName,int *iTrailerBits);
    int Read_DataBlockTable(std::ifstream &fstrFileStream,std::vector<int> &ivecBlock,int &lastInt);
    int GetDataBlockType(std::string strHeaderName);
    void OpenNastranOp2File(std::string strFileLocation,std::ifstream &fstrFileStream);
    void ReadNastranOp2File(std::string strOp2FileName,Cnode& oNodeData,CElement& oElementData);
    bool Match_Op2_Cpu_Endiannes(std::ifstream &FileStream);
    static bool GetSwapEndians();
    //methods for reading IFP table
    void ScanIFPTableRecords(std::string strHeaderName,int *trailer,int iNumber,std::vector<int> &ivecBlock,Cnode& oNodeData,CElement& oElementData);
    //methods for reading OFP table
    int GetOFPDataBlockType(std::vector<int> &ivecHeaderBlock);
    void ScanOp2Record(std::string &strHeaderName,int iHeaderLength,std::vector<int> &ivecHeaderBlock,int iTableLength,std::vector<int> &ivecTableBlock,Cnode& oNodeData,CElement& oElementData);
    bool ScanHeaderInformation(std::vector<int> &ivecHeaderBlock,
                               std::string &strTitle,
                               std::string &strSubTitle,
                               std::string &strLabel,
                               int &iApproachCode,int &iAnalysisType,int &iDeviceType,
                               int &iTableCode,int &iTableContent,bool &bSort2_Flag,
                               bool &bComplex_Flag,bool &bRandom_Flag,
                               int &iFormatCode,
                               int &iStressCode,bool &bStrain_Flag,bool &bStrainorCurvature_Flag,
                               bool &bMaterialCoordSystemId_Flag,
                               int &iThermal,
                               std::string &strSubCaseName,int intIds[2],
                               float realIds[2],
                               int &iELTYPE,int &iNUMWDE);
    //scans displacements, velocities, accelerations
    void Scan_OUG_DataBlock(std::string &strHeaderName,
                            int iHeaderBlockLength,std::vector<int> &ivecHeaderBlock,
                            int iTableBlockLength,std::vector<int> &ivecTableBlock,Cnode& oNodeData);
    //scans element stresses or strains
    void Scan_OES_DataBlock(std::string &strHeaderName,
                            int iHeaderBlockLength,std::vector<int> &ivecHeaderBlock,
                            int iTableBlockLength,std::vector<int> &ivecTableBlock,CElement& oElementData);
    //scans applied loads at nodes
    void Scan_OPG_DataBlock(std::string &strHeaderName,
                            int iHeaderBlockLength,std::vector<int> &ivecHeaderBlock,
                            int iTableBlockLength,std::vector<int> &ivecTableBlock,Cnode& oNodeData);
    //scans Output element energy (strain, kinetic, loss)
    void Scan_OEE_DataBlock(std::string &strHeaderName,
                            int iHeaderBlockLength,std::vector<int> &ivecHeaderBlock,
                            int iTableBlockLength,std::vector<int> &ivecTableBlock,CElement& oElementData);
    //scans single or multipoint constraint forces
    void Scan_OQG_DataBlock(std::string &strHeaderName,
                            int iHeaderBlockLength,std::vector<int> &ivecHeaderBlock,
                            int iTableBlockLength,std::vector<int> &ivecTableBlock,Cnode& oNodeData);
    //scans grid point forces
    void Scan_OGF_DataBlock(std::string &strHeaderName,
                            int iHeaderBlockLength,std::vector<int> &ivecHeaderBlock,
                            int iTableBlockLength,std::vector<int> &ivecTableBlock);
    ~NastranOp2();
protected:
private:
    int miSubCaseId;
    static bool mbSwapEndians;
};

#endif // NASTRANOP2_H
