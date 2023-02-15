#ifndef CVIEWERINTROIMAGE_H
#define CVIEWERINTROIMAGE_H
#include <wx/wx.h>
#include <wx/statbmp.h>

class CViewerIntroImage : public wxFrame
{
public:
    /** Default constructor */
    CViewerIntroImage(wxWindow *parent,const wxString& title);
    /** Default destructor */
    ~CViewerIntroImage();
protected:
private:
    wxStaticBitmap* pViewerIntroductoryImage;
};

#endif // CVIEWERINTROIMAGE_H
