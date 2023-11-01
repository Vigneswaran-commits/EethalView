#include<iostream>
#include<windows.h>
#include "RenderNode.h"
#include "cbasicmenubar.h"
#include "exportdata.h"
#include "node.h"
#include"vector"
#include "GL/glut.h"
#include "GL/GLU.h"
#include "GL/GL.h"
#include<math.h>
#include<stdio.h>
#include <wx/wx.h>
#include <wx/combobox.h>
#include <wx/wxchar.h>

#include<fstream>
#include<string>
using namespace std;

double CRenderNode::mdrgTranslationMatrix[] = {1.0,0.0,0.0,0.0,0.0,1.0,0.0,0.0,0.0,0.0,1.0,0.0,0.0,0.0,0.0,1.0};
double CRenderNode::mdrgScaleViewMatrix[] = {1.0,0.0,0.0,0.0,0.0,1.0,0.0,0.0,0.0,0.0,1.0,0.0,0.0,0.0,0.0,1.0};
double CRenderNode::mdrgProductMatrix[] = {1.0,0.0,0.0,0.0,0.0,1.0,0.0,0.0,0.0,0.0,1.0,0.0,0.0,0.0,0.0,1.0};
void  g_Timer(int iValue);//Forward Declaration of Global Timer Function

CRenderNode::CRenderNode(wxFrame* parent, int* args) :
    wxGLCanvas(parent, wxID_ANY, args, wxDefaultPosition, wxSize(750,450), wxFULL_REPAINT_ON_RESIZE)
//CRenderNode::CRenderNode(const wxString& title) : wxFrame(NULL, wxID_ANY, title,  wxPoint(-1,-1), wxSize(800,500), wxDEFAULT_FRAME_STYLE | wxFRAME_EX_METAL)
{
    iCounter = 1;
    m_iScalar_Button_Click_Count = 0;
    bIsGraphFramePresent = true;
    bFixinSpaceOnce = true;
    this->SetFocus();//???
    // To avoid flashing on MSW
    SetBackgroundStyle(wxBG_STYLE_CUSTOM);
    m_glContext = new wxGLContext(this);
    pProgressBar = NULL;
}

bool CRenderNode::Set_Tree_Values()
{
/*NODE ID      STRAIN(X-Axis)   STRESS(Y-AXIS)  DEFAULT VALUE*/
    int iIter = 0;
    string line;
    float x, y;
    ifstream myfile("Stress_Strain_Plot.text");

    if(myfile.is_open()) {
        while(getline(myfile, line)) {
            if(sscanf(line.c_str(), "%f %f", &x, &y) == 2) {
              Snodecoord *NodeCord = new Snodecoord();
              NodeCord->iIdentification = iIter+1;
              NodeCord->iXcord = x;
              NodeCord->iYcord = y;
              NodeCord->iZcord = 0.0;
              m_oRenderOrgNode.push_back(NodeCord);
            }
            iIter++;
        }
        if(myfile.eof())
          cout<<"Flag Reached the End of File\n";
        myfile.close();
    }
    else {
        //cout<<"File is Not Reay For Input or Output Operations\n";
    }
}

void CRenderNode::Set_Array_Values()//Node here refers to data structure
{
    if(!BasicGLPane::GetAutoRotation())
        pProgressBar = new wxProgressDialog(wxT("Importing the same"),wxT("please wait.."),100,NULL,wxPD_AUTO_HIDE | wxPD_APP_MODAL);
    Set_Tree_Values();
    Set_Data_Min_Max();
    if(!BasicGLPane::GetAutoRotation())
        pProgressBar->Update(100,wxT(""),NULL);
    Update();
}

void CRenderNode::ClearoRenderOrgNode()
{
    m_oRenderOrgNode.clear();
}

void CRenderNode::Set_Data_Min_Max()//swapping to find the X-Y Min,Max Values
{
    if(m_oRenderOrgNode.size() < 1)
        return;
    int iIter;
    float fXMin_DataSet_Vaue,fXMax_DataSet_Value;
    float fYMin_DataSet_Vaue,fYMax_DataSet_Value;


    //Set Minimum Values
    fXMin_DataSet_Vaue = m_oRenderOrgNode[0]->iXcord;
    for(iIter = 1; iIter < m_oRenderOrgNode.size(); iIter++) //Set X_Min
    {
        if(fXMin_DataSet_Vaue > m_oRenderOrgNode[iIter]->iXcord)
        {
            fXMin_DataSet_Vaue = m_oRenderOrgNode[iIter]->iXcord;
        }
    }
    fYMin_DataSet_Vaue =  m_oRenderOrgNode[0]->iYcord;
    for(iIter = 1; iIter < m_oRenderOrgNode.size(); iIter++) //Set Y-Min
    {
        if(fYMin_DataSet_Vaue > m_oRenderOrgNode[iIter]->iYcord)
        {
            fYMin_DataSet_Vaue = m_oRenderOrgNode[iIter]->iYcord;
        }
    }
    //Set Maximum Values
    fXMax_DataSet_Value = m_oRenderOrgNode[0]->iXcord;
    for(iIter = 1; iIter < m_oRenderOrgNode.size(); iIter++) //Set X_Max
    {
        if(fXMax_DataSet_Value < m_oRenderOrgNode[iIter]->iXcord)
        {
            fXMax_DataSet_Value = m_oRenderOrgNode[iIter]->iXcord;
        }
    }
    fYMax_DataSet_Value =  m_oRenderOrgNode[0]->iYcord;
    for(iIter = 1; iIter < m_oRenderOrgNode.size(); iIter++) //Set Y-Max
    {
        if(fYMax_DataSet_Value < m_oRenderOrgNode[iIter]->iYcord)
        {
            fYMax_DataSet_Value = m_oRenderOrgNode[iIter]->iYcord;
        }
    }
    m_fX_Min_Value = fXMin_DataSet_Vaue;
    m_fX_Max_Value = fXMax_DataSet_Value;
    m_fY_Min_Value = fYMin_DataSet_Vaue;
    m_fY_Max_Value = fYMax_DataSet_Value;
}

void CRenderNode::MouseMoved(wxMouseEvent& event)
{
    this->SetFocus();
    if(event.LeftIsDown())
        PanGraphwithMouse(event);
    else
    {
        m_fMouseX = event.GetX();
        m_fMouseY = event.GetY();
    }
}

void CRenderNode::PanGraphwithMouse(wxMouseEvent& event)
{
    float fCurrentXLoc = 0.0,fCurrentYLoc = 0.0,fMouseXPan_Distance = 0.0,fMouseYPan_Distance = 0.0;
    fCurrentXLoc = event.GetX();
    fCurrentYLoc = event.GetY();
    fMouseXPan_Distance = (fCurrentXLoc - m_fMouseX)/200;
    fMouseYPan_Distance = (fCurrentYLoc - m_fMouseY)/200;
    glTranslatef(fMouseXPan_Distance,-fMouseYPan_Distance,0.0f);
    Refresh();
    m_fMouseX = fCurrentXLoc;
    m_fMouseY = fCurrentYLoc;
}

void CRenderNode::Keyboard_Method(unsigned char iKey,int iXcord,int iYcord)
{
}

void CRenderNode::ZoomIn()
{
    wxClientDC dc(this); // only to be used in paint events. use wxClientDC to paint outside the paint event
    SetCurrent(*m_glContext);
    glScalef(1.1,1.1,1.1);
    Update();
}

void CRenderNode::ZoomOut()
{
    wxClientDC dc(this); // only to be used in paint events. use wxClientDC to paint outside the paint event
    SetCurrent(*m_glContext);
    glScalef(0.83,0.83,0.83);
    Update();
}

void CRenderNode::mouseWheelMoved(wxMouseEvent& event)
{
    wxClientDC dc(this); // only to be used in paint events. use wxClientDC to paint outside the paint event
    SetCurrent(*m_glContext);
    float fWheelRotationValue;
    fWheelRotationValue = event.GetWheelRotation();//returns the +/- mouse scroll float value
    if(fWheelRotationValue > 0)
        ZoomOut();
    else
        ZoomIn();
}
void CRenderNode::FitView()
{
    glLoadIdentity();
    glScalef(1.7,2.4,1.5);
    glTranslatef(-0.075,-0.08,0.0);
    Refresh();
}

void CRenderNode::rightClick(wxMouseEvent& event)
{
    wxClientDC dc(this); // only to be used in paint events. use wxClientDC to paint outside the paint event
    SetCurrent(*m_glContext);
    this->FitView();
}

void CRenderNode::MatrixMultiplication_4x4(double frgA[16],double frgB[16])
{
//    double frgA[16] = {1.0,0.0,0.0,0.0,0.0,1.0,0.0,0.0,0.0,0.0,1.0,0.0,0.0,0.0,0.0,1.0};
//    double frgB[16] = {1.0,2.0,3.0,4.0,5.0,6.0,7.0,8.0,9.0,10.0,11.0,12.0,13.0,14.0,15.0,16.0};
//    double frgA[16],frgB[16],mdrgProductMatrix[16];
//    for(int i = 0;i < 16;i++)
//    frgA[i] = frgA1[i];
//    for(int j = 0;j < 16;j++)
//    frgB[j] = frgB1[j];
    //multiplication of two column major matrices
//    mdrgProductMatrix[11] = frgA[11]*frgB[11]+frgA[12]*frgB[21]+frgA[13]*frgB[31]+frgA[14]*frgB[41];   mdrgProductMatrix[12] = frgA[11]*frgB[12]+frgA[12]*frgB[22]+frgA[13]*frgB[32]+frgA[14]*frgB[42];   mdrgProductMatrix[13] = frgA[11]*frgB[13]+frgA[12]*frgB[23]+frgA[13]*frgB[33]+frgA[14]*frgB[43];   mdrgProductMatrix[14] = frgA[11]*frgB[14]+frgA[12]*frgB[24]+frgA[13]*frgB[34]+frgA[14]*frgB[44];
//    mdrgProductMatrix[21] = frgA[21]*frgB[11]+frgA[22]*frgB[21]+frgA[23]*frgB[31]+frgA[24]*frgB[41];   mdrgProductMatrix[22] = frgA[21]*frgB[12]+frgA[22]*frgB[22]+frgA[23]*frgB[32]+frgA[24]*frgB[42];   mdrgProductMatrix[23] = frgA[21]*frgB[13]+frgA[22]*frgB[23]+frgA[23]*frgB[33]+frgA[24]*frgB[43];   mdrgProductMatrix[24] = frgA[21]*frgB[14]+frgA[22]*frgB[24]+frgA[23]*frgB[34]+frgA[24]*frgB[44];
//    mdrgProductMatrix[31] = frgA[31]*frgB[11]+frgA[32]*frgB[21]+frgA[33]*frgB[31]+frgA[34]*frgB[41];   mdrgProductMatrix[32] = frgA[31]*frgB[12]+frgA[32]*frgB[22]+frgA[33]*frgB[32]+frgA[34]*frgB[42];   mdrgProductMatrix[33] = frgA[31]*frgB[13]+frgA[32]*frgB[23]+frgA[33]*frgB[33]+frgA[34]*frgB[43];   mdrgProductMatrix[34] = frgA[31]*frgB[14]+frgA[32]*frgB[24]+frgA[33]*frgB[34]+frgA[34]*frgB[44];
//    mdrgProductMatrix[41] = frgA[41]*frgB[11]+frgA[42]*frgB[21]+frgA[43]*frgB[31]+frgA[44]*frgB[41];   mdrgProductMatrix[42] = frgA[41]*frgB[12]+frgA[42]*frgB[22]+frgA[43]*frgB[32]+frgA[44]*frgB[42];   mdrgProductMatrix[43] = frgA[41]*frgB[13]+frgA[42]*frgB[23]+frgA[43]*frgB[33]+frgA[44]*frgB[43];   mdrgProductMatrix[44] = frgA[41]*frgB[14]+frgA[42]*frgB[24]+frgA[43]*frgB[34]+frgA[44]*frgB[44];
//row wise assignment
    mdrgProductMatrix[0] = frgA[0]*frgB[0]  + frgA[4]*frgB[1] + frgA[8]*frgB[2]  + frgA[12]*frgB[3];//a11
    mdrgProductMatrix[1] = frgA[1]*frgB[0]  + frgA[5]*frgB[1] + frgA[9]*frgB[2]  + frgA[13]*frgB[3];//a21
    mdrgProductMatrix[2] = frgA[2]*frgB[0]  + frgA[6]*frgB[1] + frgA[10]*frgB[2] + frgA[14]*frgB[3];//a31
    mdrgProductMatrix[3] = frgA[3]*frgB[0]  + frgA[7]*frgB[1] + frgA[11]*frgB[2] + frgA[15]*frgB[3];//a41
    mdrgProductMatrix[4] = frgA[0]*frgB[4]  + frgA[4]*frgB[5] + frgA[8]*frgB[6]  + frgA[12]*frgB[7];//a12
    mdrgProductMatrix[5] = frgA[1]*frgB[4]  + frgA[5]*frgB[5] + frgA[9]*frgB[6]  + frgA[13]*frgB[7];//a22
    mdrgProductMatrix[6] = frgA[2]*frgB[4]  + frgA[6]*frgB[5] + frgA[10]*frgB[6] + frgA[14]*frgB[7];//a32
    mdrgProductMatrix[7] = frgA[3]*frgB[4]  + frgA[7]*frgB[5] + frgA[11]*frgB[6] + frgA[15]*frgB[7];//a42
    mdrgProductMatrix[8] = frgA[0]*frgB[8]  + frgA[4]*frgB[9] + frgA[8]*frgB[10] + frgA[12]*frgB[11];//a13
    mdrgProductMatrix[9] = frgA[1]*frgB[8]  + frgA[5]*frgB[9] + frgA[9]*frgB[10] + frgA[13]*frgB[11];//a23
    mdrgProductMatrix[10] = frgA[2]*frgB[8] + frgA[6]*frgB[9] + frgA[10]*frgB[10]+ frgA[14]*frgB[11];//a33
    mdrgProductMatrix[11] = frgA[3]*frgB[8] + frgA[7]*frgB[9] + frgA[11]*frgB[10]+ frgA[15]*frgB[11];//a43
    mdrgProductMatrix[12] = frgA[0]*frgB[12]+ frgA[4]*frgB[13]+ frgA[8]*frgB[14] + frgA[12]*frgB[15];//a14
    mdrgProductMatrix[13] = frgA[1]*frgB[12]+ frgA[5]*frgB[13]+ frgA[9]*frgB[14] + frgA[13]*frgB[15];//a24
    mdrgProductMatrix[14] = frgA[2]*frgB[12]+ frgA[6]*frgB[13]+ frgA[10]*frgB[14]+ frgA[14]*frgB[15];//a34
    mdrgProductMatrix[15] = frgA[3]*frgB[12]+ frgA[7]*frgB[13]+ frgA[11]*frgB[14]+ frgA[15]*frgB[15];//a44
}

void CRenderNode::Reshape_method(wxSizeEvent& evt)
{
//    m_iResized_Window_Width = GetSize().x;
//    m_iResized_Window_Height = GetSize().y;
//    wxGLCanvas::OnSize(evt);
//
//    glViewport(0,0,(GLsizei)GetSize().x,(GLsizei)GetSize().y); //sets the viewport to window size
//    glMatrixMode(GL_PROJECTION);//projects on screen
//
//    glLoadIdentity(); //Loads the identity matrix
//    gluPerspective(60,(GLfloat)GetSize().x/(GLfloat)GetSize().y,0.5,100.0);//sets the view to perspective mode
//    glMatrixMode(GL_MODELVIEW);//sets the matrix mode for transformations i the screen

    wxClientDC dc(this); // only to be used in paint events. use wxClientDC to paint outside the paint event
    SetCurrent(*m_glContext);
    int iWidth = GetSize().x;
    int iHeight = GetSize().y;
    wxGLCanvas::OnSize(evt);

    glViewport(0, 0, (GLsizei)iWidth, (GLsizei)iHeight); // Set our viewport to the size of our window

    glMatrixMode(GL_PROJECTION); // Switch to the projection matrix so that we can manipulate how our scene is viewed
    glLoadIdentity(); // Reset the projection matrix to the identity matrix so that we don't get any artifacts (cleaning up)
    //Refer : http://www.opengl.org/sdk/docs/man/xhtml/glOrtho.xml
    glOrtho(-1,1,-1,1,-3,3);//multiply the projection matrix with orthographic matrix//LRBTNF
//    gluPerspective(120, (GLfloat)iWidth / (GLfloat)iHeight, 10.0,10.0); // Set the Field of view angle (in degrees), the aspect ratio of our window, and the new and far planes

    glMatrixMode(GL_MODELVIEW); // Switch back to the model view matrix, so that we can start drawing shapes correctly
}

void CRenderNode::Update()
{
    wxPaintEvent evt;
    Render(evt);
}

void CRenderNode::Render(wxPaintEvent& evt)
{
    //The proper approach is to use a more "sophisticated" data structure to represent the "orientation" of the
    //camera
    if(!IsShown()) return;
    //wxGLCanvas::SetCurrent(*m_context);
    wxPaintDC dc(this); // only to be used in paint events. use wxClientDC to paint outside the paint event
    SetCurrent(*m_glContext);
//    glMatrixMode(GL_MODELVIEW); // Switch back to the model view matrix, so that we can start drawing shapes correctly
//Rendering step1:Clear color and depth buffers; load identity matrix
//    gluLookAt(0.0,0.0,0.0,
//              0.0,0.0,10.0,
//              0.0,1.0,0.0);
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f); // grey Background
    glClearDepth(1.0f);	// Depth Buffer Setup
    glEnable(GL_DEPTH_TEST); // Enables Depth Testing
    glDepthFunc(GL_LEQUAL); // The Type Of Depth Testing To Do
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//Rendering step3:Load light(s) position(s)
    //normals used for opengl lighting and back face culling
    //findind normals and normalizing;Refer : http://www.falloutsoftware.com/tutorials/gl/gl8.htm
    float amb[4] = { 0.1, 0.1, 0.1, 1.0 };
    float dif[4] = { 0.1, 0.1, 0.1, 1.0 };
    float pos[4] = { 0.0, 0.0, 2.0, 0.0 };
    glLightfv(GL_LIGHT0, GL_AMBIENT,  amb);
    glLightfv(GL_LIGHT0, GL_SPECULAR,  dif);
    glLightfv(GL_LIGHT0, GL_POSITION, pos);
    glEnable(GL_LIGHTING);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_TRUE);//all polygons are lit on both the sides
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);//color for lights,if disabled white or grey light is seen.
    //lighting with normals;refer : http://www.songho.ca/opengl/gl_normaltransform.html
    glEnable(GL_NORMALIZE);//normalising is needed incase of using glscale which shortens the normals
    glShadeModel(GL_SMOOTH);
    glPointSize(5.0);
    glBegin(GL_POINTS);
      glColor3f(0.5f, 0.5f, 0.5f);
      glVertex3f(0.0f, 0.0f, 0.0f);
    glEnd();
    if(bFixinSpaceOnce){
      glScalef(1.7,2.4,1.5);
      glTranslatef(-0.075,-0.08,0.0);
      bFixinSpaceOnce = false;
    }
    Draw_Scalar_Graph_Axis_Method();
    Draw_Scalar_Graph_Method();
    glFlush();
    SwapBuffers();//SwapBuffer command takes care of flushing and command processing
}

void CRenderNode::Draw_Scalar_Graph_Method()  //*************GRAPH************//
{
    wxClientDC dc(this); // only to be used in paint events. use wxClientDC to paint outside the paint event
    SetCurrent(*m_glContext);
    int iIter;
    float fX_Min_Scale_Value = m_fX_Min_Value;
    float fX_Max_Scale_Value = m_fX_Max_Value;//X Axis min,max variables declared
    float fY_Min_Scale_Value = m_fY_Min_Value;
    float fY_Max_Scale_Value = m_fY_Max_Value; //Y Axis min,max variables declared

    float fX_Axis_Total_Range = 0.0,fX_Axis_Single_Div_Value = 0.0;     //X axis Total range & single division variables declared
    float fY_Axis_Total_Range = 0.0,fY_Axis_Single_Div_Value = 0.0;     //Y axis Total range & single division variables declared

    float fX_Axis_Length = 1.0;
    float fY_Axis_Length = 0.6;

    fX_Axis_Total_Range = fX_Max_Scale_Value - fX_Min_Scale_Value;  //$$
    fY_Axis_Total_Range = fY_Max_Scale_Value - fY_Min_Scale_Value;  //$$

    glPushMatrix();
    //Line Strip
    /*glLineWidth(2.0);
    glColor3f(1.0f,0.0f,0.0f);
    glBegin(GL_LINE_STRIP);
    for(iIter = 0; iIter < m_oRenderOrgNode.size(); iIter++)
    {
        fX_Axis_Single_Div_Value = ( (m_oRenderOrgNode[iIter]->iXcord)*(fX_Axis_Length) )/fX_Axis_Total_Range;
        fY_Axis_Single_Div_Value = ( (m_oRenderOrgNode[iIter]->iYcord)*(fY_Axis_Length) )/fY_Axis_Total_Range;

        if(fX_Axis_Single_Div_Value < 0)
        {
            fX_Axis_Single_Div_Value = (-fX_Axis_Single_Div_Value);
        }
        if(fY_Axis_Single_Div_Value < 0)
        {
            fY_Axis_Single_Div_Value = (-fY_Axis_Single_Div_Value);
        }

        glVertex3f((fX_Axis_Single_Div_Value - 0.4),(fY_Axis_Single_Div_Value - 0.2),0.0);
    }
    glEnd();*/

    //Fixed Plotted Points in Graph
    glPointSize(5.0);
    glColor3f(0.0f,1.0f,0.0f);
    glBegin(GL_POINTS);
    for(iIter = 0; iIter < m_oRenderOrgNode.size(); iIter++)
    {
        fX_Axis_Single_Div_Value = ( (m_oRenderOrgNode[iIter]->iXcord)*(fX_Axis_Length) )/fX_Axis_Total_Range;
        fY_Axis_Single_Div_Value = ( (m_oRenderOrgNode[iIter]->iYcord)*(fY_Axis_Length) )/fY_Axis_Total_Range;

        //cout<<"Y iv Value :"<<fX_Axis_Single_Div_Value<<"\n";
        if(fX_Axis_Single_Div_Value < 0)
        {
            fX_Axis_Single_Div_Value = (-fX_Axis_Single_Div_Value);
        }
        if(fY_Axis_Single_Div_Value < 0)
        {
            fY_Axis_Single_Div_Value = (-fY_Axis_Single_Div_Value);
        }
        glVertex3f((fX_Axis_Single_Div_Value - 0.4),(fY_Axis_Single_Div_Value - 0.2),0.0);
    }
    glEnd();

    //Animated Plotted Points in Graph
//    glPointSize(8.0);
//    glColor3f(0.0f,0.0f,1.0f);
//    glBegin(GL_POINTS);
//    for(iIter = 0;iIter < m_oRenderOrgNode.size();iIter++)
//    {
//          fX_Axis_Single_Div_Value = ( (m_oRenderOrgNode[iIter]->iXcord)*(fX_Axis_Length) )/fX_Axis_Total_Range;
//          fY_Axis_Single_Div_Value = ( (m_oRenderOrgNode[iIter]->iYcord)*(fY_Axis_Length) )/fY_Axis_Total_Range;
//
//        glVertex3f((fX_Axis_Single_Div_Value - 0.4),(fY_Axis_Single_Div_Value - 0.2),0.0);
//    }
//    glEnd();
    glPopMatrix();
}

void CRenderNode::Draw_Scalar_Graph_Axis_Method()
{
    wxClientDC dc(this); // only to be used in paint events. use wxClientDC to paint outside the paint event
    SetCurrent(*m_glContext);
    //General Variables
    int iIter = 0,iIter1 = 0;
    GLfloat fLine_Width = 2.0;

    //Value assigned to Variables through calculations
    GLfloat fXpos = -0.4,fYpos = 0.4,fZpos = 0.0,fCheck_var = 0.0;
    GLfloat fXAxis_Div_Count = 15;//m_oRenderOrgNode.size();
    GLfloat fYAxis_Div_Count = 15;//m_oRenderOrgNode.size();

    glPushMatrix();
    //******X-Y Axis(skeleton)*********//
                                       //X-Y Axis
    glColor3f(1.0f,1.0f,1.0f);         //
    glLineWidth(fLine_Width);
    glBegin(GL_LINES);
    glVertex3f(fXpos,fYpos,fZpos);        //X Axis
    glVertex3f(fXpos,fYpos - 0.6,fZpos);

    glVertex3f(fXpos,fYpos - 0.6,fZpos);  //Y Axis
    glVertex3f(fXpos + 1.0,fYpos - 0.6,fZpos);
    glEnd();

    //*******Y Axis Result Name*******//
    char rgcY_Axis_Result[] = "Stress(Kpa)";
    float fX_Result_Pos = -0.48,fY_Result_Pos = 0.44;
    glRasterPos2f(fX_Result_Pos,fY_Result_Pos);

    for(iIter = 0; iIter < sizeof(rgcY_Axis_Result); iIter++)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12,rgcY_Axis_Result[iIter]);
    }

    //*******X Axis Result Name******//
    char rgcX_Axis_Result[] = "Strain(MicroStrain)";
    fX_Result_Pos = 0.0,fY_Result_Pos = -0.31;
    glRasterPos2f(fX_Result_Pos,fY_Result_Pos);

    for(iIter = 0; iIter < sizeof(rgcX_Axis_Result); iIter++)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12,rgcX_Axis_Result[iIter]);
    }

    //******************Y Division points**************//
                                                       //Y Axis-Points
    glBegin(GL_POINTS);                                //
    for(iIter = 0; iIter < (fXAxis_Div_Count + 1); iIter++)
    {
        glVertex3f(fXpos,fYpos,fZpos);
        fYpos = (fYpos) - (0.6/fXAxis_Div_Count);
    }
    glEnd();

    //*******************X Division points**************//
                                                        //X-Axis Points
    fXpos = -0.4,fYpos = -0.2,fZpos = 0.0;              //
    glBegin(GL_POINTS);
    for(iIter = 0; iIter < (fYAxis_Div_Count + 1); iIter++)
    {
        glVertex3f(fXpos,fYpos,fZpos);
        fXpos = fXpos + (1.0/fXAxis_Div_Count);
    }
    glEnd();

    //********************XDiv Lines*******************//
                                                       //X Axis Division Lines
    fXpos = -0.4,fYpos = 0.4,fZpos = 0.0;              //
    glLineWidth(0.1);
    for(iIter = 0; iIter < fXAxis_Div_Count; iIter++)
    {
        glBegin(GL_LINES);
        glVertex3f(fXpos,fYpos,fZpos);
        glVertex3f(fXpos + 1.0,fYpos,fZpos);
        glEnd();
        fYpos = fYpos - (0.6/fXAxis_Div_Count);
    }

    //*********************YDiv Lines*******************//
    fXpos = -0.4,fYpos = -0.2,fZpos = 0.0;              //Y Axis Division Lines
    glLineWidth(0.1);
    for(iIter = 0; iIter < (fXAxis_Div_Count + 1); iIter++)
    {
        glBegin(GL_LINES);
        glVertex3f(fXpos,fYpos,fZpos);
        glVertex3f(fXpos,fYpos + 0.6,fZpos);
        glEnd();
        fXpos = fXpos + (1.0/fXAxis_Div_Count);
    }

    //**********************X scale values(calc Xaxis range,division and render values)
    char rgcX_Scale[5] = {NULL};
    float fX_Min_Scale_Value = 0.0,fX_Max_Scale_Value = 0.0;                   //min,max variables declared
    float fX_Axis_Total_Range = 0.0,fX_Axis_Single_Div_Value = 0.0;            //X axis Total range & single division variables declared

    if((m_oRenderOrgNode.size() != 0) && (m_oRenderOrgNode.size() != 1))
    {
        fX_Min_Scale_Value = m_fX_Min_Value;
        fX_Max_Scale_Value = m_fX_Max_Value;
    }
    fX_Axis_Total_Range = fX_Max_Scale_Value - fX_Min_Scale_Value;             //$$ Range Calculation
    fX_Axis_Single_Div_Value = (fX_Axis_Total_Range/fXAxis_Div_Count);  //$$ Single Division Value Calculation

    //************************########*******************//
    float fRaster_Scale_XPos = -0.4;
    float fRaster_Scale_YPos = -0.25;
    for(iIter = 0; iIter < (fXAxis_Div_Count + 1); iIter++)
    {
        glRasterPos2f(fRaster_Scale_XPos,fRaster_Scale_YPos);
        fRaster_Scale_XPos = fRaster_Scale_XPos + (1.0/fXAxis_Div_Count);      //X_scale_value pos increment

        sprintf(rgcX_Scale,"%.3f",fX_Min_Scale_Value);                           //X value increment
        fX_Min_Scale_Value = fX_Min_Scale_Value + fX_Axis_Single_Div_Value;

        for(iIter1 = 0; iIter1 < sizeof(rgcX_Scale); iIter1++)
        {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10,rgcX_Scale[iIter1]);
        }

    }

    //**********************Y scale values(calc Yaxis range,division and render values)
    char rgcY_Scale[5] = {NULL};                 //Restore the Character arrays to point NULL
    float fY_Min_Scale_Value = 0.0,fY_Max_Scale_Value = 0.0;                   //min,max variables declared
    float fY_Axis_Total_Range = 0.0,fY_Axis_Single_Div_Value = 0.0;            //X axis Total range & single division variables declared

    if((m_oRenderOrgNode.size() != 0) && (m_oRenderOrgNode.size() != 1))
    {
        fY_Min_Scale_Value = m_fY_Min_Value;
        fY_Max_Scale_Value = m_fY_Max_Value;
    }
    fY_Axis_Total_Range = fY_Max_Scale_Value - fY_Min_Scale_Value;             //$$ Range Calculation
    fY_Axis_Single_Div_Value = (fY_Axis_Total_Range/fYAxis_Div_Count);  //$$ Single Division Value Calculation

    //************************########*******************//
    fRaster_Scale_XPos = -0.5;
    fRaster_Scale_YPos = -0.2;
    glRasterPos2f(fRaster_Scale_XPos,fRaster_Scale_YPos);
    for(iIter = 0; iIter < (fYAxis_Div_Count + 1); iIter++)
    {
        glRasterPos2f(fRaster_Scale_XPos,fRaster_Scale_YPos);
        fRaster_Scale_YPos = fRaster_Scale_YPos + (0.6/fYAxis_Div_Count);      //X_scale_value pos increment

        sprintf(rgcY_Scale,"%.1f",fY_Min_Scale_Value);                         //X value increment
        fY_Min_Scale_Value = fY_Min_Scale_Value + fY_Axis_Single_Div_Value;

        for(iIter1 = 0; iIter1 < sizeof(rgcY_Scale); iIter1++)
        {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10,rgcY_Scale[iIter1]);
        }

    }
    glPopMatrix();

}

bool CRenderNode::IsGraphFramePresent()
{
    if(bIsGraphFramePresent)
        return true;
    else
        return false;
}

CRenderNode::~CRenderNode()
{
    Snodecoord *NodeCord = NULL;
    for(int i = 0; i < m_oRenderOrgNode.size(); i++)
    {
        NodeCord = m_oRenderOrgNode[i];
        delete NodeCord;
        m_oRenderOrgNode[i] = NULL;
        NodeCord = NULL;
    }
    m_oRenderOrgNode.clear();
    /**/
    bIsGraphFramePresent = false;
    if(pProgressBar)
      delete pProgressBar;
}

