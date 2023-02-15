#include "Cbasictoolbar.h"

CBasicToolbar::CBasicToolbar(wxFrame* parent,const wxString& title) : wxFrame(parent, wxID_ANY, title, wxDefaultPosition, wxSize(280, 50))
{
//    wxImage::AddHandler( new wxPNGHandler );
//
//    wxBitmap exit(wxT("E:/CSpace_shrinkwrap/MarchingCubes/wxglcanvas/images/btn_columns.png"), wxBITMAP_TYPE_PNG);
//    wxBitmap newb(wxT("E:/CSpace_shrinkwrap/MarchingCubes/wxglcanvas/images/btn_restart.png"), wxBITMAP_TYPE_PNG);
//    wxBitmap open(wxT("E:/CSpace_shrinkwrap/MarchingCubes/wxglcanvas/images/c_exe_obj.png"), wxBITMAP_TYPE_PNG);
//    wxBitmap save(wxT("E:/CSpace_shrinkwrap/MarchingCubes/wxglcanvas/images/git-favicon.png"), wxBITMAP_TYPE_PNG);
//
//    wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);
//
//    toolbar1 = new wxToolBar(this, wxID_ANY);
//    toolbar1->AddTool(wxID_ANY, newb, wxT(""));
//    toolbar1->AddTool(wxID_ANY, open, wxT(""));
//    toolbar1->AddTool(wxID_ANY, save, wxT(""));
//    toolbar1->Realize();
//
//    toolbar2 = new wxToolBar(this, wxID_ANY);
//    toolbar2->AddTool(wxID_EXIT, exit, wxT("Exit application"));
//    toolbar2->Realize();
//
//    vbox->Add(toolbar1, 0, wxEXPAND);
//    vbox->Add(toolbar2, 0, wxEXPAND);
//
//    SetSizer(vbox);
//
//    Connect(wxID_EXIT, wxEVT_COMMAND_TOOL_CLICKED,
//            wxCommandEventHandler(CBasicToolbar::OnQuit));
//
//    Centre();
}

void CBasicToolbar::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

