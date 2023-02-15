#include <wx/wx.h>
#include <wx/combobox.h>
#include <wx/wxchar.h>
#include "graphframe.h"
#include "graph/RenderNode.h"
#include "cbasicmenubar.h"
#define ID_PLOT	          1000
CRenderNode *CGraphFrame::pGraphGL;

CGraphFrame::CGraphFrame(const wxString& title) : wxFrame(NULL, wxID_ANY, title,  wxPoint(-1,-1), wxSize(750,450), wxDEFAULT_FRAME_STYLE ^ wxRESIZE_BORDER)
{
    //sizer arrangement
    wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);//sizer enables us to put several widgets into a row or a column.
    wxBoxSizer* hbox1 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* hbox2 = new wxBoxSizer(wxHORIZONTAL);

    const wxString Xaxischoices[13] = {"Strs-Strn_X", "Strs-Strn_Y", "Strs-Strn_Z", "Strs-Strn_XY",
                                       "Strs-Strn_YZ", "Strs-Strn_XZ","Strn-Strs_ShearAngle",
                                       "Strn-Strs_1stPrincipal", "Strn-Strs_2ndPrincipal", "Strn-Strs_3rdPrincipal",
                                       "Strn-Strs_MajorPrincipal", "Strn-Strs_MinorPrincipal", "Strn-Strs_Vonmises"};
    pComboBox1 = new wxComboBox(this, wxID_ANY, wxT("Choices"), wxDefaultPosition, wxSize(140,20), 13, Xaxischoices, wxCB_DROPDOWN, wxDefaultValidator, wxT("Graph Input"));
    pComboBox1->CanApplyThemeBorder();
    pComboBox1->ShouldInheritColours();
    wxString XaxisSelectedText = pComboBox1->GetStringSelection();

    //wxImage::AddHandler( new wxPNGHandler );
    wxBitmap bitmapPlot(wxT("images/pen5_new_grey.png"), wxBITMAP_TYPE_PNG);

    //button
    wxBitmapButton *button = new wxBitmapButton(this, wxID_APPLY, bitmapPlot, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, wxT("Plotter"));

    hbox1->Add(pComboBox1, 0, wxEXPAND | wxALL, 0);//options 1-window/sizer,2-proportion with other windows,3-flags,4-border
    hbox1->AddSpacer(2);
    hbox1->Add(button,     0, wxEXPAND | wxALL, 0);

    //initialise and load values in graph data structure, CRenderNode
    int args[] = {WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16, 0};//alternate for glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH)
    pGraphGL = new CRenderNode(this, args);
    hbox2->Add(pGraphGL, 0, wxEXPAND | wxALIGN_CENTER | wxALL, 0);

    vbox->Add(hbox1, 0, wxEXPAND | wxALL, 5);
    vbox->Add(hbox2, 0, wxEXPAND | wxALL, 5);

     SetSizer(vbox);

     Centre();
     bGraphWindowPresent = true;
     CBasicMenubar::GraphFramePresent(true);
}

void CGraphFrame::setstate(wxIdleEvent& event)
{
//    if(this->IsActive())
//        pGraphGL->CanSetFocus(true);
//    else
//        pGraphGL->CanSetFocus(false);
}

void CGraphFrame::Plot(wxCommandEvent& event)
{
    bool bSelected = true;
    switch(pComboBox1->GetSelection())
    {
    case 0:
        CBasicMenubar::glPane->ExportGraph_StrsStrnXX();
        break;
    case 1:
        CBasicMenubar::glPane->ExportGraph_StrsStrnYY();
        break;
    case 2:
        CBasicMenubar::glPane->ExportGraph_StrsStrnZZ();
        break;
    case 3:
        CBasicMenubar::glPane->ExportGraph_StrsStrnXY();
        break;
    case 4:
        CBasicMenubar::glPane->ExportGraph_StrsStrnYZ();
        break;
    case 5:
        CBasicMenubar::glPane->ExportGraph_StrsStrnXZ();
        break;
    case 6:
        CBasicMenubar::glPane->ExportGraph_StrsStrnShearAngle();
        break;
    case 7:
        CBasicMenubar::glPane->ExportGraph_StrsStrn1stPrincipal();
        break;
    case 8:
        CBasicMenubar::glPane->ExportGraph_StrsStrn2ndPrincipal();
        break;
    case 9:
        CBasicMenubar::glPane->ExportGraph_StrsStrn3rdPrincipal();
        break;
    case 10:
        CBasicMenubar::glPane->ExportGraph_StrsStrnMajorPrincipal();
        break;
    case 11:
        CBasicMenubar::glPane->ExportGraph_StrsStrnMinorPrincipal();
        break;
    case 12:
        CBasicMenubar::glPane->ExportGraph_StrsStrnVmises();
        break;
    case -1:
        bSelected = false;;
        break;
    }
    if(bSelected){
      pGraphGL->ClearoRenderOrgNode();
      pGraphGL->Set_Array_Values();
      pGraphGL->FitView();
    }
}

bool CGraphFrame::isGraphWindowPresent()
{
    if(bGraphWindowPresent)
        return true;
    else
        return false;
}

CGraphFrame::~CGraphFrame()
{
    //dtor
    delete pGraphGL;
    pGraphGL = NULL;
    bGraphWindowPresent = false;
    CBasicMenubar::GraphFramePresent(false);
}
