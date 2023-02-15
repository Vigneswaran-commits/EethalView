#include <wx/button.h>
#include "ai.h"
#include "cbasicmenubar.h"
#include "ctextcontrol.h"
#include "ds_filereader_fusion/RedblackTree_bdf/NastranBdfParser/nastranbdf.h"

#define ID_BUTTON1	      1
#define ID_BUTTON2	      2
#define ID_RESEARCH       3

wxString AI::m_wxstrBDFFilePath;
wxString AI::m_wxstrBDFFileName;

AI::AI(const wxString& title) : wxFrame(NULL, wxID_ANY, title,  wxPoint(-1,-1), wxSize(200,170), wxDEFAULT_FRAME_STYLE ^ wxRESIZE_BORDER)
{
    //ctor
    wxButton *button1 = new wxButton(this, ID_BUTTON1, wxT("Open BDF"), wxPoint(50, 20));
    wxButton *button2 = new wxButton(this, ID_BUTTON2, wxT("Open OP2"), wxPoint(50, 60));
    wxButton *button3 = new wxButton(this, ID_RESEARCH, wxT("Research"), wxPoint(50, 100));

    Centre();

    Connect(ID_BUTTON1, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(AI::Open_BDF_File));
    Connect(ID_BUTTON2, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(AI::Open_OP2_File));
    Connect(ID_RESEARCH, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(AI::ShowResearchResults));

    bAiWindowPresent = true;
    CBasicMenubar::AiFramePresent(true);
}

void AI::Open_BDF_File(wxCommandEvent& event)
{
    wxFileDialog openFileDialog(this, _("Open CAE Input file"), "", "",
                                "bdf File (*.bdf)|*.bdf", wxFD_OPEN|wxFD_FILE_MUST_EXIST);

    if (openFileDialog.ShowModal() == wxID_CANCEL)
        return;  // the user changed idea...

    //load the file into memory; can also be done with e.g. wxWidgets input streams
    SetSTLFilePath(openFileDialog.GetDirectory());
    SetSTLFileName(openFileDialog.GetFilename());
    FreeDataStructureMemory();          //free already created memory
    SetDataStructureRootKeys();        //set roots to null
    Read_and_Store_BDF();             //read file and store in datastructure
}

void AI::Open_OP2_File(wxCommandEvent& event)
{
    wxFileDialog openFileDialog(this, _("Open CAE Result file"), "", "",
                                "op2 File (*.op2)|*.op2", wxFD_OPEN|wxFD_FILE_MUST_EXIST);

    if (openFileDialog.ShowModal() == wxID_CANCEL)
        return; // the user changed idea...

    //load the file into memory; can also be done with e.g. wxWidgets input streams
    CBasicMenubar::glPane->SetSTLFilePath(openFileDialog.GetDirectory());
    CBasicMenubar::glPane->SetSTLFileName(openFileDialog.GetFilename());
    CBasicMenubar::glPane->FreeDataStructureMemory();          //free already created memory
    CBasicMenubar::glPane->SetDataStructureRootKeys();        //set roots to null
    CBasicMenubar::glPane->Read_and_Store_Files();           //read file and store in datastructure
}

void AI::SetSTLFileName(wxString wxstrFileName)
{
    m_wxstrBDFFileName = wxstrFileName;
}

void AI::SetSTLFilePath(wxString wxstrFilePath)
{
    m_wxstrBDFFilePath = wxstrFilePath;
}

void AI::SetDataStructureRootKeys()
{
    //sets the roots of redblacktree to point null
    oNodeData_bdf.Set_RootKey(55);
    oElementData_bdf.Set_RootKey(55);
}

void AI::Read_and_Store_BDF()
{
    string strFilePathName;
    strFilePathName.append(m_wxstrBDFFilePath);
    strFilePathName.append("\\");
    strFilePathName.append(m_wxstrBDFFileName);
    if(strFilePathName.length() != 0)
    {
        pProgressBar = new wxProgressDialog(wxT("Loading BDF.."),wxT("please wait.."),100,NULL,wxPD_AUTO_HIDE | wxPD_APP_MODAL);
        oNodeData_bdf.Insert_RootNode_Attributes();
        oElementData_bdf.Insert_RootNode_Attributes();
        if(m_wxstrBDFFileName.AfterFirst(wxT('.')) == "bdf")
        {
            mbDataStructurePopulated = true;
            NastranBDF oInstance;
            oInstance.ReadNastranBDFFile(strFilePathName,oNodeData_bdf,oElementData_bdf);
        }
        //oElementData.SetNodeCoord2ConnectingNodes(oNodeData);//maps node coords to elements connectivity
        pProgressBar->Update(100,wxT(""),NULL);
    }
}

void AI::Learning()
{
    //CBasicMenubar::glPane->CanSetFocus(true);
}

void AI::Sleeping() {}
void AI::Researching() {}
void AI::Playing() {}

void AI::ShowResearchResults(wxCommandEvent& event)
{
    CBasicMenubar::pOutputConsole->SetFocus();
    cout<<"AI Output\n";
}

bool AI::isAiWindowPresent()
{
    if(bAiWindowPresent)
        return true;
    else
        return false;
}

void AI::FreeDataStructureMemory()
{
    oNodeData_bdf.Free_Memory();
    oElementData_bdf.ClearEelementNamesStrength();
    oElementData_bdf.Free_Memory();
}

AI::~AI()
{
    //dtor
    bAiWindowPresent = false;
    CBasicMenubar::AiFramePresent(false);
}
