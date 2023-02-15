#ifndef CBASICTOOLBAR_H
#define CBASICTOOLBAR_H
#include <wx/wx.h>
#include <wx/menu.h>
#include <wx/Toolbar.h>
#include <wx/listctrl.h>
#include "basicglpane.h"

class CBasicToolbar : public wxFrame
{
public:
    CBasicToolbar(wxFrame* parent,const wxString& title);

    void OnQuit(wxCommandEvent& event);
    wxToolBar *toolbar1;
    wxToolBar *toolbar2;
};

#endif // CBASICTOOLBAR_H

