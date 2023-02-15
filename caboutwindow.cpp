#include "caboutwindow.h"
#include <wx/wx.h>
#include <wx/textctrl.h>
#include <fstream>
using namespace std;
#define ID_KALLANAIIMAGE  10001
#define ID_KALLANAITEXT   10002
#define ID_EETHALLOGO     10003
#define ID_EETHALTEXT     10004



CAboutWindow::CAboutWindow(wxWindow *parent,const wxString& title) : wxFrame(parent, wxID_ANY, title, wxPoint(450,200), wxSize(400, 300),(wxDEFAULT_FRAME_STYLE | wxFRAME_FLOAT_ON_PARENT) & ~ (wxRESIZE_BORDER | wxMAXIMIZE_BOX | wxMINIMIZE_BOX),wxT("") )
{
    //kallanai dam image
    wxImage::AddHandler( new wxPNGHandler );
    wxBitmap bitmapKallanai(wxT("images/about/kallanai.png"), wxBITMAP_TYPE_PNG);
    pKallanaiImage = new wxStaticBitmap(this,ID_KALLANAIIMAGE,bitmapKallanai,wxDefaultPosition,wxDefaultSize,0,wxT(""));
    //kallanai dam text
    pKallanaiImageText = new wxStaticText(this,ID_KALLANAITEXT,wxT("Kallanai,Tamilnadu"),wxPoint(0,0),wxDefaultSize,wxALIGN_CENTRE,wxT(""));
    pKallanaiImageText->Centre();
    wxFont TextFont = pKallanaiImageText->GetFont();
    TextFont.SetWeight(wxBOLD);
    pKallanaiImageText->SetFont(TextFont);
    //eethal logo
    wxImage::AddHandler( new wxPNGHandler );
    wxBitmap bitmapEthalLogo(wxT("images/logo/aboutlogo/aboutimage.png"), wxBITMAP_TYPE_PNG);
    pEethalLogo = new wxStaticBitmap(this,ID_EETHALLOGO,bitmapEthalLogo,wxDefaultPosition,wxDefaultSize,0,wxT(""));
    //eethal text
    pAboutEethalText = new wxTextCtrl(this,ID_EETHALTEXT,wxT(""), wxDefaultPosition,wxDefaultSize,wxTE_MULTILINE | wxTE_RICH | wxTE_AUTO_URL | wxTE_READONLY | wxHSCROLL | wxTE_CENTER,wxDefaultValidator,wxT(""));
    streambuf *sbStdOutBuffer = cout.rdbuf();
    cout.rdbuf(pAboutEethalText);
    cout<<"Vigneswaran Ramamoorthy\n\n";
    cout<<"vikipr@gmail.com\n\n";
    cout.rdbuf(sbStdOutBuffer);

    wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);//sizer enables us to put several widgets into a row or a column.
    wxBoxSizer* vbox1 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* vbox2 = new wxBoxSizer(wxVERTICAL);

    vbox1->Add(pKallanaiImage, 80, wxEXPAND | wxALL, 1);//options 1-window/sizeer,2-proportion with other windows,3-flags,4-border
    vbox1->Add(pKallanaiImageText, 10, wxEXPAND | wxALL, 1);

    vbox2->Add(pEethalLogo, 50, wxEXPAND | wxALL, 1);
    vbox2->Add(pAboutEethalText, 50, wxEXPAND | wxALL, 1);

    hbox->Add(vbox1, 50, wxEXPAND | wxALL, 2);
    hbox->Add(vbox2, 50, wxEXPAND | wxRIGHT | wxBOTTOM, 4);
    SetSizer(hbox);
    Centre();
}

CAboutWindow::~CAboutWindow()
{
    //dtor
    close(true);
}
