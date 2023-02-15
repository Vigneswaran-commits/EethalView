#include "cloadingwindow.h"
#include <wx/wx.h>
#include <wx/stattext.h>
#define ID_LOADMESSAGE 10005
CLoadingWindow::CLoadingWindow(wxWindow *parent,const wxString& title) : wxFrame(parent, wxID_ANY, title, wxPoint(600,300), wxSize(200, 50),wxSTAY_ON_TOP,wxT("") )
{
    //load message
    pLoadingMessage = new wxStaticText(this,ID_LOADMESSAGE,wxT("loading.please wait...."),wxPoint(40,15),wxDefaultSize,wxALIGN_CENTRE,wxT(""));
}

CLoadingWindow::~CLoadingWindow()
{
    //dtor
}
