#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED
#include <windows.h>
#include <wx/wx.h>
#include "basicglpane.h"
class MyApp: public wxApp
{
private:
    CViewerIntroImage* pIntroImage;
    CBasicMenubar *pViewerFrame;
public:
    MyApp() {};
    ~MyApp() {};
    virtual bool OnInit();
    virtual int OnExit();
    virtual void CleanUp();
};

#endif // MAIN_H_INCLUDED
