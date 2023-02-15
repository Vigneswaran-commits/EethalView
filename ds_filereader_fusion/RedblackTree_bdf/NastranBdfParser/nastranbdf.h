#ifndef NASTRANOP2_H
#define NASTRANOP2_H
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <wx/wx.h>
#include <wx/string.h>

#include "ds_filereader_fusion/RedblackTree_bdf/cnode_bdf.h"
#include "ds_filereader_fusion/RedblackTree_bdf/celement_bdf.h"
class NastranBDF
{
public:
    struct SResultElementType
    {
    };

    NastranBDF();
    ~NastranBDF();
    int ReturnCardId(char *cCard);
    void ReadNastranBDFFile(wxString strFilePathName,Cnode_bdf& oNodeData_bdf,CElement_bdf& oElementData_bdf);
private:
};

#endif // NASTRANOP2_H
