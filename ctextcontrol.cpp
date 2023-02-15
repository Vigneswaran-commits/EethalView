#include "ctextcontrol.h"
#include <wx/textctrl.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
using namespace std;

#define ID_TEXTCONTROL                  2017

CTextControl::CTextControl(wxFrame* parent,const wxString& title) : wxFrame(parent, ID_TEXTCONTROL, title, wxDefaultPosition, wxSize(800, 400),wxDEFAULT_FRAME_STYLE)
{
    bOutputConsolePresent = true;
    //ctor
    pTextControl = new wxTextCtrl(this, -1, wxT(""), wxPoint(-1, -1),wxDefaultSize,wxTE_MULTILINE | wxTE_RICH | wxTE_AUTO_URL | wxTE_READONLY | wxHSCROLL | wxTE_LEFT,wxDefaultValidator,wxT(""));
    streambuf *sbStdOutBuffer = cout.rdbuf();
    cout.rdbuf(pTextControl);
}

bool CTextControl::isOutputConsolePresent()
{
    if(bOutputConsolePresent)
        return true;
    else
        return false;
}

void CTextControl::Active()
{
    int x;
}

CTextControl::~CTextControl()
{
    //dtor
    bOutputConsolePresent = false;
}
