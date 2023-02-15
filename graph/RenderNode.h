#include <wx/wx.h>
#include <wx/wxchar.h>
#include "cnode.h"
#include "celement.h"
#include "node.h"
#include "basicglpane.h"
#include<vector>
#include <wx/wx.h>
#include <wx/menu.h>
#include <wx/glcanvas.h>
#include <wx/string.h>
#include <wx/progdlg.h>
#include <list>
using namespace std;
#ifndef CRENDERNODE_H
#define CRENDERNODE_H
struct Snodecoord
{
    int iIdentification;
    float iXcord;
    float iYcord;
    float iZcord;
};
class CRenderNode : public wxGLCanvas
{
private:
    wxGLContext* m_glContext;
    vector<Snodecoord*> m_oRenderOrgNode;
    int m_RootKey, iCounter;
    int m_iMouse_XPos,m_iMouse_YPos,m_iMouse_click_value,m_iScalar_Button_Click_Count;
    float m_fX_Min_Value,m_fX_Max_Value;
    float m_fY_Min_Value,m_fY_Max_Value;
    int m_iResized_Window_Width,m_iResized_Window_Height;
    float m_fMouseX,m_fMouseY;
    bool bIsGraphFramePresent, bFixinSpaceOnce;
    wxComboBox* pComboBox1, *pComboBox2;
    wxProgressDialog *pProgressBar;

    static double mdrgAutorotationMatrix[16];
    static double mdrgTranslationMatrix[16];
    static GLdouble mdrgScaleViewMatrix[16];
    static double mdrgProductMatrix[16];

    void Set_Data_Min_Max();
    void Draw_Scalar_Graph_Axis_Method();   //Callback Render Method
    void Draw_Scalar_Graph_Method();        //Callback Render Method
public:
    //CRenderNode(const wxString& title);
    CRenderNode(wxFrame* parent, int* args);

    void MouseMoved(wxMouseEvent& event);
    void PanGraphwithMouse(wxMouseEvent& event);
    void Keyboard_Method(unsigned char iKey,int iXcord,int iYcord);
    void ZoomIn();
    void ZoomOut();
    void mouseWheelMoved(wxMouseEvent& event);
    void FitView();
    void rightClick(wxMouseEvent& event);

    void Set_Array_Values();
    bool Set_Tree_Values();
    void ClearoRenderOrgNode();

    //math
    void MatrixMultiplication_4x4(double frgA[16],double frgB[16]);

    void Reshape_method(wxSizeEvent& evt);
    void Update();
    void Render(wxPaintEvent& evt);
    bool IsGraphFramePresent();
    virtual ~CRenderNode();

    DECLARE_EVENT_TABLE()
};
#endif


