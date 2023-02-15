#ifndef CTEXTCONTROL_H
#define CTEXTCONTROL_H
#include <wx/wx.h>
#include <wx/textctrl.h>
#include <iostream>

class CTextControl : public wxFrame
{
public:
    CTextControl(wxFrame* parent,const wxString& title);
    bool isOutputConsolePresent();
    void Active();
    ~CTextControl();
protected:
private:
    bool bOutputConsolePresent;
    wxTextCtrl *pTextControl;
};

#endif // CTEXTCONTROL_H


