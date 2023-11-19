#include "basicglpane.h"
#include "cbasicmenubar.h"
#include "cbasictoolbar.h"
#include "border.h"
#include "ctextcontrol.h"
#include "caboutwindow.h"
#include "cviewerintroimage.h"
#include "graph/RenderNode.h"
#include <wx/glcanvas.h>
#include "main.h"
#include <wx/wx.h>
#include <wx/sizer.h>
#include <wx/msw/button.h>
#include <wx/icon.h>

#include <wx/event.h>
#include <wx/app.h>


// include OpenGL
#ifdef __WXMAC__
#include "OpenGL/glu.h"
#include "OpenGL/gl.h"
#else
#include "GL/glut.h"
#include "GL/glu.h"
#include "GL/gl.h"
#endif
#define ID_MYFRAME	      1
IMPLEMENT_APP(MyApp)
/*equivalent to main function*/
bool MyApp::OnInit()
{
    //***********menubar and menu icons
    pIntroImage = new CViewerIntroImage(NULL,wxT("IntroductionImage"));
    pIntroImage->Show(true);
    pIntroImage->SetFocus();

    pViewerFrame = new CBasicMenubar(wxT("Eethal Viewer"));
    pViewerFrame->SetAutoLayout(true);
    pViewerFrame->SetIcons(wxIconBundle("images/logo/mainframe.png",wxBITMAP_TYPE_PNG));
    pViewerFrame->Show(true);

    pIntroImage->Close(true);
    if(pIntroImage)
    {
        delete pIntroImage;
        pIntroImage = NULL;
    }
    return true;
}

int MyApp::OnExit()
{
    // Eethalview application specific cleanup is done here
    if(pViewerFrame)
    {
        delete pViewerFrame;
        pViewerFrame = NULL;
    }

    // This allows the base wxApp class to perform its cleanup.
    // By returning wxApp::OnExit(), default cleanup provided by wxApp is done here
    return wxApp::OnExit(); // Zero for success, non-zero for error
}

void MyApp::CleanUp()
{

}

//main application frame
BEGIN_EVENT_TABLE(CBasicMenubar, wxFrame)
    EVT_IDLE(CBasicMenubar::setstate)
    EVT_CLOSE(CBasicMenubar::OnClose)
END_EVENT_TABLE()

//macros which act as glut callbacks for BasicGLPane display methods.ogle
BEGIN_EVENT_TABLE(BasicGLPane, wxGLCanvas)
    EVT_MOTION(BasicGLPane::mouseMoved)
    EVT_LEFT_DOWN(BasicGLPane::leftClick)
    EVT_LEFT_UP(BasicGLPane::mouseReleased)
    EVT_RIGHT_DOWN(BasicGLPane::rightClick)
    EVT_LEAVE_WINDOW(BasicGLPane::MouseLeaveWindow)
    EVT_ENTER_WINDOW(BasicGLPane::MouseEnteredWindow)
    EVT_SIZE(BasicGLPane::resized)
    EVT_KEY_DOWN(BasicGLPane::keyPressed)
    EVT_KEY_UP(BasicGLPane::keyReleased)
    EVT_MOUSEWHEEL(BasicGLPane::mouseWheelMoved)
    EVT_PAINT(BasicGLPane::Render)
END_EVENT_TABLE()

//graph frame
BEGIN_EVENT_TABLE(CGraphFrame, wxFrame)
    EVT_BUTTON(wxID_APPLY, CGraphFrame::Plot)
    EVT_IDLE(CGraphFrame::setstate)
END_EVENT_TABLE()

//ai frame
BEGIN_EVENT_TABLE(AI, wxFrame)
    EVT_BUTTON(wxID_APPLY, AI::Open_BDF_File)
    EVT_BUTTON(wxID_APPLY, AI::Open_OP2_File)
END_EVENT_TABLE()

//macros which act as glut callbacks for RenderNode display methods.
BEGIN_EVENT_TABLE(CRenderNode, wxGLCanvas)
    EVT_MOTION(CRenderNode::MouseMoved)
    EVT_RIGHT_DOWN(CRenderNode::rightClick)
    EVT_MOUSEWHEEL(CRenderNode::mouseWheelMoved)
    EVT_SIZE(CRenderNode::Reshape_method)
    EVT_PAINT(CRenderNode::Render)
END_EVENT_TABLE()

//-------------------------------------------------------------------
/*finds the cross product of two vectors
    float fVector1[3],fVector2[3];
float fVertex1[3],fVertex2[3],fVertex3[3];
if(iIncrement == 3)
{
    fVertex1[0] = fXCoordinate;
    fVertex1[1] = fYCoordinate;
    fVertex1[2] = fZCoordinate;
}
else if(iIncrement == 4)
{
    fVertex2[0] = fXCoordinate;
    fVertex2[1] = fYCoordinate;
    fVertex2[2] = fZCoordinate;
}
else if(iIncrement == 5)
{
    fVertex3[0] = fXCoordinate;
    fVertex3[1] = fYCoordinate;
    fVertex3[2] = fZCoordinate;
    //finds cross product of the vectors
    fVector1[0]=fVertex2[0]-fVertex1[0];
    fVector1[1]=fVertex2[1]-fVertex1[1];
    fVector1[2]=fVertex2[2]-fVertex1[2];

    fVector2[0]=fVertex3[0]-fVertex1[0];
    fVector2[1]=fVertex3[1]-fVertex1[1];
    fVector2[2]=fVertex3[2]-fVertex1[2];

    fNormal[0]=fVector1[1]*fVector2[2] - fVector1[2]*fVector2[1];
    fNormal[1]=fVector1[2]*fVector2[0] - fVector1[0]*fVector2[2];
    fNormal[2]=fVector1[0]*fVector2[1] - fVector1[1]*fVector2[0];
*/
//-------------------------------------------------------------------
