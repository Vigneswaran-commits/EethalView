#ifndef CLOADINGWINDOW_H
#define CLOADINGWINDOW_H
#include <wx/wx.h>
#include <wx/stattext.h>

class CLoadingWindow : public wxFrame
{
public:
    /** Default constructor */
    CLoadingWindow(wxWindow *parent,const wxString& title);
//        void OnInternalIdle();
    /** Default destructor */
    ~CLoadingWindow();
protected:
private:
    wxStaticText* pLoadingMessage;
};

#endif // CLOADINGWINDOW_H
