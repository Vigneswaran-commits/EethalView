#ifndef AI_H
#define AI_H

#include <wx/wx.h>
#include <wx/button.h>
#include <wx/string.h>
#include "basicglpane.h"
#include "ds_filereader_fusion/RedblackTree_bdf/cnode_bdf.h"
#include "ds_filereader_fusion/RedblackTree_bdf/celement_bdf.h"


//add code to find potential changes in the input file, using the results op2. These changes inturn should give inputs to improve the
//next analysis by some research
class AI : public wxFrame
{
public:
        /** Default constructor */
        AI(const wxString& title);
        /** Default destructor */
        ~AI();

        void Open_BDF_File(wxCommandEvent& event);
        void Open_OP2_File(wxCommandEvent& event);
        void SetSTLFileName(wxString wxstrFileName);
        void SetSTLFilePath(wxString wxstrFilePath);
        void SetDataStructureRootKeys();
        void Read_and_Store_BDF();

        void Learning();
        void Sleeping();
        void Researching();
        void Playing();
        void ShowResearchResults(wxCommandEvent& event);

        bool isAiWindowPresent();
        void FreeDataStructureMemory();

        DECLARE_EVENT_TABLE()

private:
    //data structure for nodes and elements
    Cnode_bdf oNodeData_bdf;
    CElement_bdf oElementData_bdf;

    bool bAiWindowPresent;
    static wxString m_wxstrBDFFilePath;
    static wxString m_wxstrBDFFileName;
    //progress bar displayed while loading model from file
    wxProgressDialog* pProgressBar;
    bool mbDataStructurePopulated;
};

#endif // AI_H
