#include "cviewerintroimage.h"
#include <wx/statbmp.h>
CViewerIntroImage::CViewerIntroImage(wxWindow *parent,const wxString& title) : wxFrame(parent, wxID_ANY, title, wxPoint(450,200), wxSize(400, 200),wxSTAY_ON_TOP,wxT("") )
{
    //ctor
    //kallanai dam image
    wxImage::AddHandler( new wxPNGHandler );
    wxBitmap bitmapKallanai(wxT("images/logo/intrologo/intrologo.png"), wxBITMAP_TYPE_PNG);
    pViewerIntroductoryImage = new wxStaticBitmap(this,-1,bitmapKallanai,wxDefaultPosition,wxDefaultSize,0,wxT(""));
}

CViewerIntroImage::~CViewerIntroImage()
{
    //dtor
}
