#ifndef CABOUTWINDOW_H
#define CABOUTWINDOW_H
#include <wx/wx.h>
#include <wx/textctrl.h>
#include <wx/treectrl.h>
#include <wx/stattext.h>
#include <wx/statbmp.h>
class CAboutWindow : public wxFrame
{
public:
    /** Default constructor */
    CAboutWindow(wxWindow *parent,const wxString& title);
    /** Default destructor */
    ~CAboutWindow();
protected:
private:
    wxStaticBitmap* pKallanaiImage;
    wxStaticText* pKallanaiImageText;
    wxStaticBitmap* pEethalLogo;
    wxTextCtrl* pAboutEethalText;
};

#endif // CABOUTWINDOW_H
