#include <fstream>
#include <iostream>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <wx/wx.h>
#include <wx/string.h>
#include <tchar.h>
#include <list>
#include <vector>

// OpenGL header files
#ifdef __WXMAC__
#include "OpenGL/glu.h"
#include "OpenGL/gl.h"
#else
#include "GL/glew.h"
#include "GL/glut.h"
#include "GL/glu.h"
#include "GL/gl.h"
#include "GL/glext.h"
#endif

#include "basicglpane.h"
#include "cbasicmenubar.h"
#include "exportdata.h"
#include "ctreeview.h"
#include "NastranOp2Parser/op2/nastranop2.h"
using namespace std;

#define ZEROD_ELEMENT                  1
#define SCALAR_ELEMENT                 2
#define ONED_ELEMENT                   3
#define TWOD_ELEMENT                   4
#define THREED_ELEMENT                 5
#define AXISYMMETRIC_ELEMENT           6
#define RIGID_ELEMENT                  7

bool BasicGLPane::mbUpdateModelOnce;
int BasicGLPane::m_iBoundBox;
int BasicGLPane::m_InsertOnce;
float BasicGLPane::m_fMouse3dCord[] = {0.0,0.0,0.0};
double BasicGLPane::mdrgQuaternionMatrix[] = {1.0,0.0,0.0,0.0,0.0,1.0,0.0,0.0,0.0,0.0,1.0,0.0,0.0,0.0,0.0,1.0};
double BasicGLPane::mdrgAutorotationMatrix[] = {1.0,0.0,0.0,0.0,0.0,1.0,0.0,0.0,0.0,0.0,1.0,0.0,0.0,0.0,0.0,1.0};
double BasicGLPane::mdrgTranslationMatrix[] = {1.0,0.0,0.0,0.0,0.0,1.0,0.0,0.0,0.0,0.0,1.0,0.0,0.0,0.0,0.0,1.0};
double BasicGLPane::mdrgScaleViewMatrix[] = {1.0,0.0,0.0,0.0,0.0,1.0,0.0,0.0,0.0,0.0,1.0,0.0,0.0,0.0,0.0,1.0};
double BasicGLPane::mdrgProductMatrix[] = {1.0,0.0,0.0,0.0,0.0,1.0,0.0,0.0,0.0,0.0,1.0,0.0,0.0,0.0,0.0,1.0};
bool BasicGLPane::mbAutorotation = false;
wxString BasicGLPane::m_wxstrFilePath;
wxString BasicGLPane::m_wxstrFileName;

BasicGLPane::BasicGLPane(wxFrame* parent, int* args) :
    wxGLCanvas(parent, wxID_ANY, args, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE)
{
    mbSetFocus = true;
    mbCalcOnce = false;
    mbDataStructurePopulated = false;
    mbDeerHeadPopulated = false;
    mfScaledValue = 1.0;
    m_InsertOnce = 0;
    m_iBoundBox = 0;
    mbUpdateModelOnce = true;
    m_fZoomFactor = 1.0;//model size remains constant
    m_fModelCentre_XMax = 321;
    m_fMaxTmp = 321;
    m_fMinTmp = 0.0;
    m_fTmp = 0.0;
    mstrResultId = ""; miSubcaseId = 0;
    /*read deer head once*/
    SetSTLFilePath("images/resources");
    SetSTLFileName("deer_head.stl");
    mbread_deerhead = true;
    ReadStlFile();
    mbread_deerhead = false;
    mbDeerHeadPopulated = true;
    /*-------------------*/
    // To avoid flashing on MSW
    SetBackgroundStyle(wxBG_STYLE_CUSTOM);
    m_glContext = new wxGLContext(this);
}

BasicGLPane::~BasicGLPane()
{
    if(m_glContext)
    {
        delete m_glContext;
        m_glContext = NULL;
    }
}

void BasicGLPane::resized(wxSizeEvent& evt)
{
    wxClientDC dc(this); // only to be used in paint events. use wxClientDC to paint outside the paint event
    SetCurrent(*m_glContext);
    int iWidth = getWidth();
    int iHeight = getHeight();
    wxGLCanvas::OnSize(evt);

    glViewport(0, 0, (GLsizei)iWidth, (GLsizei)iHeight); // Set our viewport to the size of our window

    glMatrixMode(GL_PROJECTION); // Switch to the projection matrix so that we can manipulate how our scene is viewed
    glLoadIdentity(); // Reset the projection matrix to the identity matrix so that we don't get any artifacts (cleaning up)
    //Refer : http://www.opengl.org/sdk/docs/man/xhtml/glOrtho.xml
    glOrtho(-1,1,-1,1,-3,3);//multiply the projection matrix with orthographic matrix//LRBTNF
//    gluPerspective(120, (GLfloat)iWidth / (GLfloat)iHeight, 10.0,10.0); // Set the Field of view angle (in degrees), the aspect ratio of our window, and the new and far planes

    glMatrixMode(GL_MODELVIEW); // Switch back to the model view matrix, so that we can start drawing shapes correctly
}

/** Inits the OpenGL viewport for drawing in 3D. */
void BasicGLPane::prepare3DViewport(int topleft_x, int topleft_y, int bottomrigth_x, int bottomrigth_y)
{
    wxClientDC dc(this); // only to be used in paint events. use wxClientDC to paint outside the paint event
    SetCurrent(*m_glContext);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black Background
    glClearDepth(1.0f);	// Depth Buffer Setup
    glEnable(GL_DEPTH_TEST); // Enables Depth Testing
    glDepthFunc(GL_LEQUAL); // The Type Of Depth Testing To Do
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glEnable(GL_COLOR_MATERIAL);

    glViewport(topleft_x, topleft_y, bottomrigth_x-topleft_x, bottomrigth_y-topleft_y);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    float ratio_w_h = (float)(bottomrigth_x-topleft_x)/(float)(bottomrigth_y-topleft_y);
    gluPerspective(45 /*view angle*/, ratio_w_h, 0.1 /*clip close*/, 200 /*clip far*/);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

/** Inits the OpenGL viewport for drawing in 2D. */
void BasicGLPane::prepare2DViewport(int topleft_x, int topleft_y, int bottomrigth_x, int bottomrigth_y)
{
    wxClientDC dc(this); // only to be used in paint events. use wxClientDC to paint outside the paint event
    SetCurrent(*m_glContext);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black Background
    glEnable(GL_TEXTURE_2D);   // textures
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    glViewport(topleft_x, topleft_y, bottomrigth_x-topleft_x, bottomrigth_y-topleft_y);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluOrtho2D(topleft_x, bottomrigth_x, bottomrigth_y, topleft_y);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int BasicGLPane::getWidth()
{
    return GetSize().x;
}

int BasicGLPane::getHeight()
{
    return GetSize().y;
}

void BasicGLPane::Update()
{
    wxPaintEvent evt;
    Render(evt);
}

void BasicGLPane::Update_BoundBox_Model()
{
    //mbUpdateModelOnce = true;
    m_iBoundBox = 1;
}

void BasicGLPane::SetSTLFileName(wxString wxstrFileName)
{
    Update_BoundBox_Model();
    for(int iIter = 0; iIter < 3; iIter++)
        m_fMouse3dCord[iIter] = 0.0;
    m_InsertOnce = 0;
    m_wxstrFileName = wxstrFileName;
}

void BasicGLPane::SetSTLFilePath(wxString wxstrFilePath)
{
    m_wxstrFilePath = wxstrFilePath;
}

void BasicGLPane::SetDataStructureRootKeys()
{
    //sets the roots of redblacktree to point null
    oNodeData.Set_RootKey(55);
    oElementData.Set_RootKey(55);
}

void BasicGLPane::Read_and_Store_Files()
{
    string strFilePathName;
    strFilePathName.append(m_wxstrFilePath);
    strFilePathName.append("\\");
    strFilePathName.append(m_wxstrFileName);
    if(strFilePathName.length() != 0)
    {
        if(!mbAutorotation)
            pProgressBar = new wxProgressDialog(wxT("Loading Model.."),wxT("please wait.."),100,NULL,wxPD_AUTO_HIDE | wxPD_APP_MODAL);
        oNodeData.Insert_RootNode_Attributes();
        oElementData.Insert_RootNode_Attributes();
        if(m_wxstrFileName.AfterFirst(wxT('.')) == "op2")
        {
            mbDataStructurePopulated = true;
            NastranOp2 oInstance;
            oInstance.ReadNastranOp2File(strFilePathName,oNodeData,oElementData);
        }
        else if(m_wxstrFileName.AfterFirst(wxT('.')) == "stl")
        {
            mbDataStructurePopulated = true;
            ReadStlFile();
        }
        ConfigureModelCoordinatesforDisplay(oNodeData);
        oElementData.SetNodeCoord2ConnectingNodes(oNodeData);//maps node coords to elements connectivity
        oElementData.SetNormal();
        if(!mbAutorotation)
            pProgressBar->Update(100,wxT(""),NULL);
        //removes already existing components if any and sets new tree view components
        SetTreeViewData();
    }
    Reserve_minmax_Vectors();//for legend
    mstrResultId = "";/*remove legend*/ miSubcaseId = 0; //reset subcase
    Update();
    //check for any past errors
    if(glGetError() == GL_STACK_OVERFLOW)
        cout<<"OpenGL Error: Matrix stack overflowed.\n";
    else if(glGetError() == GL_STACK_UNDERFLOW)
        cout<<"OpenGL Error: Matrix stack underflowed.\n";
    else if(glGetError() == GL_OUT_OF_MEMORY)
        cout<<"OpenGL Error: Out of memory.\n";
}

void BasicGLPane::FindModelxyz_MinMaxInorder(Snode* &pInorderTraverse)
{
    if(pInorderTraverse != NULL)
    {
        FindModelxyz_MinMaxInorder(pInorderTraverse->pNodeleft);  //Traverse and display leftside Tree nodes
        if(pInorderTraverse->uiIdentification != 0)
        {
            if(pInorderTraverse->NodeInfo.NodePosition.iNodeId != 0)
            {
                if(m_fModelCentre_XMax == 321)
                {
                    m_fModelCentre_XMax = pInorderTraverse->NodeInfo.NodePosition.fXCoordinate;
                    m_fModelCentre_YMax = pInorderTraverse->NodeInfo.NodePosition.fYCoordinate;
                    m_fModelCentre_ZMax = pInorderTraverse->NodeInfo.NodePosition.fZCoordinate;
                    m_fModelCentre_XMin = pInorderTraverse->NodeInfo.NodePosition.fXCoordinate;
                    m_fModelCentre_YMin = pInorderTraverse->NodeInfo.NodePosition.fYCoordinate;
                    m_fModelCentre_ZMin = pInorderTraverse->NodeInfo.NodePosition.fZCoordinate;
                }
                if(m_fModelCentre_XMax < pInorderTraverse->NodeInfo.NodePosition.fXCoordinate)
                    m_fModelCentre_XMax = pInorderTraverse->NodeInfo.NodePosition.fXCoordinate;
                if(m_fModelCentre_YMax < pInorderTraverse->NodeInfo.NodePosition.fYCoordinate)
                    m_fModelCentre_YMax = pInorderTraverse->NodeInfo.NodePosition.fYCoordinate;
                if(m_fModelCentre_ZMax < pInorderTraverse->NodeInfo.NodePosition.fZCoordinate)
                    m_fModelCentre_ZMax = pInorderTraverse->NodeInfo.NodePosition.fZCoordinate;
                if(m_fModelCentre_XMin > pInorderTraverse->NodeInfo.NodePosition.fXCoordinate)
                    m_fModelCentre_XMin = pInorderTraverse->NodeInfo.NodePosition.fXCoordinate;
                if(m_fModelCentre_YMin > pInorderTraverse->NodeInfo.NodePosition.fYCoordinate)
                    m_fModelCentre_YMin = pInorderTraverse->NodeInfo.NodePosition.fYCoordinate;
                if(m_fModelCentre_ZMin > pInorderTraverse->NodeInfo.NodePosition.fZCoordinate)
                    m_fModelCentre_ZMin = pInorderTraverse->NodeInfo.NodePosition.fZCoordinate;
            }
        }
        FindModelxyz_MinMaxInorder(pInorderTraverse->pNoderight); //Traverse and display Rightside Tree nodes
    }
}

void BasicGLPane::CalculateCoordinatesforDisplay_Inorder(Snode* &pInorderTraverse)
{
    if(pInorderTraverse != NULL)
    {
        CalculateCoordinatesforDisplay_Inorder(pInorderTraverse->pNodeleft);  //Traverse and display leftside Tree nodes
        if(pInorderTraverse->uiIdentification != 0)
        {
//            cout<<pInorderTraverse->uiIdentification<<"\n";
            //move model to screen centre
            pInorderTraverse->NodeInfo.NodePosition.fXCoordinate = pInorderTraverse->NodeInfo.NodePosition.fXCoordinate - m_fModelCentre_X;
            pInorderTraverse->NodeInfo.NodePosition.fYCoordinate = pInorderTraverse->NodeInfo.NodePosition.fYCoordinate - m_fModelCentre_Y;
            pInorderTraverse->NodeInfo.NodePosition.fZCoordinate = pInorderTraverse->NodeInfo.NodePosition.fZCoordinate - m_fModelCentre_Z;
            //scale the model coordinates
            pInorderTraverse->NodeInfo.NodePosition.fXCoordinate = (0.7*pInorderTraverse->NodeInfo.NodePosition.fXCoordinate)/m_fModelMaxAxisValue;
            pInorderTraverse->NodeInfo.NodePosition.fYCoordinate = (0.7*pInorderTraverse->NodeInfo.NodePosition.fYCoordinate)/m_fModelMaxAxisValue;
            pInorderTraverse->NodeInfo.NodePosition.fZCoordinate = (0.7*pInorderTraverse->NodeInfo.NodePosition.fZCoordinate)/m_fModelMaxAxisValue;
        }
        CalculateCoordinatesforDisplay_Inorder(pInorderTraverse->pNoderight); //Traverse and display Rightside Tree nodes
    }
}

void BasicGLPane::ConfigureModelCoordinatesforDisplay(Cnode& oNodeData)
{
    m_fModelCentre_XMax = 321;//random number
    //find the models xyz axes min max values
    FindModelxyz_MinMaxInorder(oNodeData.GetRootNode());
    //calculates model centre=>dist b/w screen centre and model centre
    m_fModelCentre_X = (m_fModelCentre_XMax + m_fModelCentre_XMin)/2;
    m_fModelCentre_Y = (m_fModelCentre_YMax + m_fModelCentre_YMin)/2;
    m_fModelCentre_Z = (m_fModelCentre_ZMax + m_fModelCentre_ZMin)/2;
    //move model & boundbox to screen centre
    m_fModelCentre_XMax = m_fModelCentre_XMax - m_fModelCentre_X;
    m_fModelCentre_YMax = m_fModelCentre_YMax - m_fModelCentre_Y;
    m_fModelCentre_ZMax = m_fModelCentre_ZMax - m_fModelCentre_Z;
    m_fModelCentre_XMin = m_fModelCentre_XMin - m_fModelCentre_X;
    m_fModelCentre_YMin = m_fModelCentre_YMin - m_fModelCentre_Y;
    m_fModelCentre_ZMin = m_fModelCentre_ZMin - m_fModelCentre_Z;
    //find Model's Maximum Axis Value value(both positive and negative directions)
    float fXYZMaxMinDiff[6] = {m_fModelCentre_XMax,m_fModelCentre_YMax,m_fModelCentre_ZMax,
                               m_fModelCentre_XMin,m_fModelCentre_YMin,m_fModelCentre_ZMin
                              };
    m_fModelMaxAxisValue = fXYZMaxMinDiff[0];
    for(int iIter = 1; iIter < 6; iIter++)
    {
        if(fXYZMaxMinDiff[iIter] < 0)
            fXYZMaxMinDiff[iIter] = -(fXYZMaxMinDiff[iIter]);
        if(m_fModelMaxAxisValue < fXYZMaxMinDiff[iIter])
        {
            m_fModelMaxAxisValue = fXYZMaxMinDiff[iIter];
        }
    }
    //scale the model axes max min values
    m_fModelCentre_XMax = (0.7*m_fModelCentre_XMax)/m_fModelMaxAxisValue;
    m_fModelCentre_YMax = (0.7*m_fModelCentre_YMax)/m_fModelMaxAxisValue;
    m_fModelCentre_ZMax = (0.7*m_fModelCentre_ZMax)/m_fModelMaxAxisValue;
    m_fModelCentre_XMin = (0.7*m_fModelCentre_XMin)/m_fModelMaxAxisValue;
    m_fModelCentre_YMin = (0.7*m_fModelCentre_YMin)/m_fModelMaxAxisValue;
    m_fModelCentre_ZMin = (0.7*m_fModelCentre_ZMin)/m_fModelMaxAxisValue;
    //move model to screen centre and scale the model coordinates
    CalculateCoordinatesforDisplay_Inorder(oNodeData.GetRootNode());
    //assign
    m_fLength = m_fModelCentre_XMax + (-m_fModelCentre_XMin);
    m_fHeight =  m_fModelCentre_YMax + (-m_fModelCentre_YMin);
    m_fDepth =  m_fModelCentre_ZMax + (-m_fModelCentre_ZMin);
    //max length of model is stored (used to create room)
    m_fMaxModelDim = m_fLength;
    if(m_fHeight > m_fMaxModelDim)
        m_fMaxModelDim = m_fHeight;
    if(m_fDepth > m_fMaxModelDim)
        m_fMaxModelDim = m_fDepth;
    //flag for one time model center calculation
    mbCalcOnce = true;
}

void BasicGLPane::FreeDataStructureMemory()
{
    oNodeData.Free_Memory();
    oElementData.ClearEelementNamesStrength();
    oElementData.Free_Memory();
}
//element connectivity node arrangement for different element types
//http://www.google.co.in/imgres?imgurl=http://www.grc.nasa.gov/WWW/cgns/CGNS_docs_beta/sids/conv.figs/tetra16.gif&imgrefurl=http://www.grc.nasa.gov/WWW/cgns/CGNS_docs_beta/sids/conv.html&usg=__JZPnkE5JX54-LqwdBOrGFDnQN0s=&h=252&w=600&sz=12&hl=en&start=5&zoom=1&tbnid=nAmycqynx9VECM:&tbnh=57&tbnw=135&ei=7q5SUZvHEsutrAfizYGQDw&prev=/search%3Fq%3Dtetra%2Belement%2Bconnectivity%26hl%3Den%26sa%3DX%26tbm%3Disch%26prmd%3Divnsb&itbs=1&sa=X&ved=0CDMQrQMwBA
void BasicGLPane::DrawElementsInorder(SBinaryTreeNode* pInorderTraverse, int iSubCase)
{
    if(pInorderTraverse != NULL)
    {
        DrawElementsInorder(pInorderTraverse->pNodeleft, iSubCase);  //Traverse and display leftside Tree nodes
        if(pInorderTraverse->uiIdentification != 0)
        {
            //for legend; 0,2,1 because stored order is r,b,&g
            if(mstrResultId == "stressx" && pInorderTraverse->ElementInfo.vecStressStrainData.size() != 0)
            {
                glColor3f(pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressX_rbg[0],
                          pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressX_rbg[2],
                          pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressX_rbg[1]);
            }
            else if(mstrResultId == "stressy" && pInorderTraverse->ElementInfo.vecStressStrainData.size() != 0)
            {
                glColor3f(pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressY_rbg[0],
                          pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressY_rbg[2],
                          pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressY_rbg[1]);
            }
            else if(mstrResultId == "stressz" && pInorderTraverse->ElementInfo.vecStressStrainData.size() != 0)
            {
                glColor3f(pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressZ_rbg[0],
                          pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressZ_rbg[2],
                          pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressZ_rbg[1]);
            }
            else if(mstrResultId == "stressxy" && pInorderTraverse->ElementInfo.vecStressStrainData.size() != 0)
            {
                glColor3f(pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressXY_rbg[0],
                          pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressXY_rbg[2],
                          pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressXY_rbg[1]);
            }
            else if(mstrResultId == "stressyz" && pInorderTraverse->ElementInfo.vecStressStrainData.size() != 0)
            {
                glColor3f(pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressYZ_rbg[0],
                          pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressYZ_rbg[2],
                          pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressYZ_rbg[1]);
            }
            else if(mstrResultId == "stressxz" && pInorderTraverse->ElementInfo.vecStressStrainData.size() != 0)
            {
                glColor3f(pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressXZ_rbg[0],
                          pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressXZ_rbg[2],
                          pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressXZ_rbg[1]);
            }
            else
                glColor3f(0.0,0.5,0.5);//cyan

            //draw elements
            if(pInorderTraverse->ElementInfo.iElementType == TWOD_ELEMENT)
            {
                if(pInorderTraverse->ElementInfo.ivecElementConnectivity.size() == 4)
                {
                    glNormal3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[0].fXNormal,pInorderTraverse->ElementInfo.ivecElementConnectivity[0].fYNormal,pInorderTraverse->ElementInfo.ivecElementConnectivity[0].fZNormal);
                    glBegin(GL_QUADS);
                    glVertex3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[0].fXCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[0].fYCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[0].fZCoordinate);
                    glVertex3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[1].fXCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[1].fYCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[1].fZCoordinate);
                    glVertex3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[2].fXCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[2].fYCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[2].fZCoordinate);
                    glVertex3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[3].fXCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[3].fYCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[3].fZCoordinate);
                    glEnd();
                }
                else if(pInorderTraverse->ElementInfo.ivecElementConnectivity.size() == 3)
                {
                    glNormal3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[0].fXNormal,pInorderTraverse->ElementInfo.ivecElementConnectivity[0].fYNormal,pInorderTraverse->ElementInfo.ivecElementConnectivity[0].fZNormal);
                    glBegin(GL_TRIANGLES);
                    glVertex3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[0].fXCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[0].fYCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[0].fZCoordinate);
                    glVertex3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[1].fXCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[1].fYCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[1].fZCoordinate);
                    glVertex3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[2].fXCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[2].fYCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[2].fZCoordinate);
                    glEnd();
                }

            }
            else if(pInorderTraverse->ElementInfo.iElementType == THREED_ELEMENT)
            {
                if(pInorderTraverse->ElementInfo.ivecElementConnectivity.size() == 8)
                {
                    glBegin(GL_QUADS);
                    glNormal3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[0].fXNormal,pInorderTraverse->ElementInfo.ivecElementConnectivity[0].fYNormal,pInorderTraverse->ElementInfo.ivecElementConnectivity[0].fZNormal);
                    glVertex3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[0].fXCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[0].fYCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[0].fZCoordinate);//front
                    glVertex3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[1].fXCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[1].fYCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[1].fZCoordinate);
                    glVertex3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[2].fXCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[2].fYCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[2].fZCoordinate);
                    glVertex3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[3].fXCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[3].fYCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[3].fZCoordinate);

                    glNormal3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[4].fXNormal,pInorderTraverse->ElementInfo.ivecElementConnectivity[4].fYNormal,pInorderTraverse->ElementInfo.ivecElementConnectivity[4].fZNormal);
                    glVertex3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[4].fXCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[4].fYCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[4].fZCoordinate);//back
                    glVertex3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[5].fXCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[5].fYCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[5].fZCoordinate);
                    glVertex3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[6].fXCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[6].fYCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[6].fZCoordinate);
                    glVertex3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[7].fXCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[7].fYCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[7].fZCoordinate);

                    glNormal3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[1].fXNormal,pInorderTraverse->ElementInfo.ivecElementConnectivity[1].fYNormal,pInorderTraverse->ElementInfo.ivecElementConnectivity[1].fZNormal);
                    glVertex3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[1].fXCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[1].fYCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[1].fZCoordinate);
                    glVertex3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[4].fXCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[4].fYCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[4].fZCoordinate);
                    glVertex3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[5].fXCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[5].fYCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[5].fZCoordinate);
                    glVertex3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[2].fXCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[2].fYCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[2].fZCoordinate);

                    glNormal3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[2].fXNormal,pInorderTraverse->ElementInfo.ivecElementConnectivity[2].fYNormal,pInorderTraverse->ElementInfo.ivecElementConnectivity[2].fZNormal);
                    glVertex3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[2].fXCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[2].fYCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[2].fZCoordinate);
                    glVertex3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[5].fXCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[5].fYCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[5].fZCoordinate);
                    glVertex3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[6].fXCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[6].fYCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[6].fZCoordinate);
                    glVertex3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[3].fXCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[3].fYCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[3].fZCoordinate);

                    glNormal3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[3].fXNormal,pInorderTraverse->ElementInfo.ivecElementConnectivity[3].fYNormal,pInorderTraverse->ElementInfo.ivecElementConnectivity[3].fZNormal);
                    glVertex3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[3].fXCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[3].fYCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[3].fZCoordinate);
                    glVertex3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[6].fXCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[6].fYCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[6].fZCoordinate);
                    glVertex3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[7].fXCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[7].fYCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[7].fZCoordinate);
                    glVertex3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[0].fXCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[0].fYCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[0].fZCoordinate);

                    glNormal3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[0].fXNormal,pInorderTraverse->ElementInfo.ivecElementConnectivity[0].fYNormal,pInorderTraverse->ElementInfo.ivecElementConnectivity[0].fZNormal);
                    glVertex3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[0].fXCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[0].fYCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[0].fZCoordinate);
                    glVertex3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[7].fXCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[7].fYCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[7].fZCoordinate);
                    glVertex3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[4].fXCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[4].fYCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[4].fZCoordinate);
                    glVertex3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[1].fXCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[1].fYCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[1].fZCoordinate);
                    glEnd();
                }
                else if(pInorderTraverse->ElementInfo.ivecElementConnectivity.size() == 6)
                {
                    glBegin(GL_TRIANGLES);
                    glNormal3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[0].fXNormal,pInorderTraverse->ElementInfo.ivecElementConnectivity[0].fYNormal,pInorderTraverse->ElementInfo.ivecElementConnectivity[0].fZNormal);
                    glVertex3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[0].fXCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[0].fYCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[0].fZCoordinate);
                    glVertex3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[1].fXCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[1].fYCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[1].fZCoordinate);
                    glVertex3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[2].fXCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[2].fYCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[2].fZCoordinate);

                    glNormal3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[3].fXNormal,pInorderTraverse->ElementInfo.ivecElementConnectivity[3].fYNormal,pInorderTraverse->ElementInfo.ivecElementConnectivity[3].fZNormal);
                    glVertex3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[3].fXCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[3].fYCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[3].fZCoordinate);
                    glVertex3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[4].fXCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[4].fYCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[4].fZCoordinate);
                    glVertex3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[5].fXCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[5].fYCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[5].fZCoordinate);
                    glEnd();

                    glBegin(GL_QUADS);
                    glNormal3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[0].fXNormal,pInorderTraverse->ElementInfo.ivecElementConnectivity[0].fYNormal,pInorderTraverse->ElementInfo.ivecElementConnectivity[0].fZNormal);
                    glVertex3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[0].fXCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[0].fYCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[0].fZCoordinate);
                    glVertex3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[2].fXCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[2].fYCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[2].fZCoordinate);
                    glVertex3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[5].fXCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[5].fYCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[5].fZCoordinate);
                    glVertex3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[3].fXCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[3].fYCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[3].fZCoordinate);

                    glNormal3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[1].fXNormal,pInorderTraverse->ElementInfo.ivecElementConnectivity[1].fYNormal,pInorderTraverse->ElementInfo.ivecElementConnectivity[1].fZNormal);
                    glVertex3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[1].fXCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[1].fYCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[1].fZCoordinate);
                    glVertex3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[2].fXCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[2].fYCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[2].fZCoordinate);
                    glVertex3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[5].fXCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[5].fYCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[5].fZCoordinate);
                    glVertex3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[4].fXCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[4].fYCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[4].fZCoordinate);

                    glNormal3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[0].fXNormal,pInorderTraverse->ElementInfo.ivecElementConnectivity[0].fYNormal,pInorderTraverse->ElementInfo.ivecElementConnectivity[0].fZNormal);
                    glVertex3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[0].fXCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[0].fYCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[0].fZCoordinate);
                    glVertex3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[1].fXCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[1].fYCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[1].fZCoordinate);
                    glVertex3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[4].fXCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[4].fYCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[4].fZCoordinate);
                    glVertex3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[3].fXCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[3].fYCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[3].fZCoordinate);
                    glEnd();
                }
                else if(pInorderTraverse->ElementInfo.ivecElementConnectivity.size() == 4)
                {
                    glBegin(GL_TRIANGLES);
                    glNormal3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[0].fXNormal,pInorderTraverse->ElementInfo.ivecElementConnectivity[0].fYNormal,pInorderTraverse->ElementInfo.ivecElementConnectivity[0].fZNormal);
                    glVertex3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[0].fXCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[0].fYCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[0].fZCoordinate);
                    glVertex3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[1].fXCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[1].fYCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[1].fZCoordinate);
                    glVertex3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[3].fXCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[3].fYCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[3].fZCoordinate);

                    glNormal3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[1].fXNormal,pInorderTraverse->ElementInfo.ivecElementConnectivity[1].fYNormal,pInorderTraverse->ElementInfo.ivecElementConnectivity[1].fZNormal);
                    glVertex3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[1].fXCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[1].fYCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[1].fZCoordinate);
                    glVertex3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[2].fXCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[2].fYCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[2].fZCoordinate);
                    glVertex3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[3].fXCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[3].fYCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[3].fZCoordinate);

                    glNormal3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[2].fXNormal,pInorderTraverse->ElementInfo.ivecElementConnectivity[2].fYNormal,pInorderTraverse->ElementInfo.ivecElementConnectivity[2].fZNormal);
                    glVertex3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[2].fXCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[2].fYCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[2].fZCoordinate);
                    glVertex3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[0].fXCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[0].fYCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[0].fZCoordinate);
                    glVertex3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[3].fXCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[3].fYCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[3].fZCoordinate);

                    glNormal3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[0].fXNormal,pInorderTraverse->ElementInfo.ivecElementConnectivity[0].fYNormal,pInorderTraverse->ElementInfo.ivecElementConnectivity[0].fZNormal);
                    glVertex3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[0].fXCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[0].fYCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[0].fZCoordinate);
                    glVertex3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[1].fXCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[1].fYCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[1].fZCoordinate);
                    glVertex3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[2].fXCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[2].fYCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[2].fZCoordinate);
                    glEnd();
                }
            }
            else if(pInorderTraverse->ElementInfo.iElementType == ONED_ELEMENT)
            {
                glBegin(GL_LINES);
                glVertex3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[0].fXCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[0].fYCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[0].fZCoordinate);
                glVertex3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[1].fXCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[1].fYCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[1].fZCoordinate);
                glEnd();
            }
            else if(pInorderTraverse->ElementInfo.iElementType == ZEROD_ELEMENT)
            {
                glBegin(GL_POINTS);
                glVertex3f(pInorderTraverse->ElementInfo.ivecElementConnectivity[0].fXCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[0].fYCoordinate,pInorderTraverse->ElementInfo.ivecElementConnectivity[0].fZCoordinate);
                glEnd();
            }
        }
        DrawElementsInorder(pInorderTraverse->pNoderight, iSubCase); //Traverse and display Rightside Tree nodes
    }
}

void BasicGLPane::DrawNodesInorder(Snode* pInorderTraverse)
{
    if(pInorderTraverse != NULL)
    {
        DrawNodesInorder(pInorderTraverse->pNodeleft);  //Traverse and display leftside Tree nodes
        if(pInorderTraverse->uiIdentification != 0)
        {
            glVertex3f(pInorderTraverse->NodeInfo.NodePosition.fXCoordinate,pInorderTraverse->NodeInfo.NodePosition.fYCoordinate,pInorderTraverse->NodeInfo.NodePosition.fZCoordinate);
        }
        DrawNodesInorder(pInorderTraverse->pNoderight); //Traverse and display Rightside Tree nodes
    }
}

//get node and element data
Cnode BasicGLPane::GetNodeInfo(Cnode oNodeData)
{
    oNodeData = this->oNodeData;
    return oNodeData;
}

CElement* BasicGLPane::GetElementInfo()
{
    //oElemData = this->oElementData;
    //return &oElementData;
    //CRenderNode::Set_Tree_Values(oElementData);
}

void BasicGLPane::WriteElementInfo()
{
    //oElementData.mpNodeRoot->ElementInfo.vecStressStrainData[0].fStrainX;
    //oElementData.mpNodeRoot->ElementInfo.vecStressStrainData[0].fStrainX;
}

void BasicGLPane::SetAutoRotation()
{
    mbAutorotation = true;
}

void BasicGLPane::UnsetAutoRotation()
{
    mbAutorotation = false;
}

bool BasicGLPane::GetAutoRotation()
{
    return mbAutorotation;
}

void BasicGLPane::Draw_ModelContainer()
{
    wxClientDC dc(this); // only to be used in paint events. use wxClientDC to paint outside the paint event
    SetCurrent(*m_glContext);
    float fLeftPos = -1.0,fRightPos = 1.0,fBottomPos = -1.0,fTopPos = 1.0,fNearPos = 1.0,fFarPos = -1.0;
    float fNormalCoord[] = {1.0,1.0,1.0},fXPerspective = 0.2,fYPerspective = 0.2;
    wxClientDC(this); // only to be used in paint events. use wxClientDC to paint outside the paint event
    glPushMatrix();
//    glColor3f(0.7f,0.7f,0.7f);
    glColor3f(0.0f,0.0f,0.0f);
    glBegin(GL_QUADS);
//    glNormal3f(1.0f,0.0,0.0);
//    glVertex3f(fLeftPos,fTopPos,fNearPos);//left-1265
//    glNormal3f(1.0f,0.0,0.0);
//    glVertex3f(fLeftPos,fBottomPos,fNearPos);
//    glNormal3f(1.0f,0.0,0.0);
//    glVertex3f(fLeftPos + fXPerspective,fBottomPos + fYPerspective,fFarPos);
//    glNormal3f(1.0f,0.0,0.0);
//    glVertex3f(fLeftPos + fXPerspective,fTopPos - fYPerspective,fFarPos);
//
//    glNormal3f(-1.0,0.0,0.0);
//    glVertex3f(fRightPos - fXPerspective,fTopPos - fYPerspective,fFarPos);//right-8734
//    glNormal3f(-1.0,0.0,0.0);
//    glVertex3f(fRightPos - fXPerspective,fBottomPos + fYPerspective,fFarPos);
//    glNormal3f(-1.0,0.0,0.0);
//    glVertex3f(fRightPos,fBottomPos,fNearPos);
//    glNormal3f(-1.0,0.0,0.0);
//    glVertex3f(fRightPos,fTopPos,fNearPos);
//
//    glNormal3f(0.0,1.0,0.0);
//    glVertex3f(fRightPos,fBottomPos,fNearPos);//bottom-3762
//    glNormal3f(0.0,1.0,0.0);
//    glVertex3f(fRightPos - fXPerspective,fBottomPos + fYPerspective,fFarPos);
//    glNormal3f(0.0,1.0,0.0);
//    glVertex3f(fLeftPos + fXPerspective,fBottomPos + fYPerspective,fFarPos);
//    glNormal3f(0.0,1.0,0.0);
//    glVertex3f(fLeftPos,fBottomPos,fNearPos);
//    glNormal3f(0.0,1.0,0.0);
//
//    glNormal3f(0.0,-1.0,0.0);
//    glVertex3f(fRightPos - fXPerspective,fTopPos - fYPerspective,fFarPos);//top-8415
//    glNormal3f(0.0,-1.0,0.0);
//    glVertex3f(fRightPos,fTopPos,fNearPos);
//    glNormal3f(0.0,-1.0,0.0);
//    glVertex3f(fLeftPos,fTopPos,fNearPos);
//    glNormal3f(0.0,-1.0,0.0);
//    glVertex3f(fLeftPos + fXPerspective,fTopPos - fYPerspective,fFarPos);

    glNormal3f(0.0,0.0,1.0);
    glVertex3f(fLeftPos + fXPerspective,fTopPos - fYPerspective,fFarPos);//rear-5678
    glNormal3f(0.0,0.0,1.0);
    glVertex3f(fLeftPos + fXPerspective,fBottomPos + fYPerspective,fFarPos);
    glNormal3f(0.0,0.0,1.0);
    glVertex3f(fRightPos - fXPerspective,fBottomPos + fYPerspective,fFarPos);
    glNormal3f(0.0,0.0,1.0);
    glVertex3f(fRightPos - fXPerspective,fTopPos - fYPerspective,fFarPos);
    glEnd();
//for dividing container into small quads for better lighting
//    glColor3f(1.0f,0.0f,0.0f);
//    for(int iRowCube = 0; iRowCube < 10; iRowCube++)
//    {
//        fTopPos = 1.0,fFarPos = -1.0;
//        for(int iVerCube = 0; iVerCube < 10; iVerCube++)
//        {
//            glBegin(GL_QUADS);
//            glNormal3f(0.3,0.3,0.0);
//            glVertex3f(fLeftPos,fTopPos,fFarPos);
//            glVertex3f(fLeftPos,fTopPos - 0.2,fFarPos);
//            glVertex3f(fLeftPos + 0.2,fTopPos - 0.2,fFarPos);
//            glVertex3f(fLeftPos + 0.2,fTopPos,fFarPos);
//            glEnd();
//            fTopPos = fTopPos - 0.2;
//        }
//        fLeftPos = fLeftPos + 0.2;
//    }
    glPopMatrix();
}

void BasicGLPane::Display_ScreenCentre()
{
    wxClientDC dc(this); // only to be used in paint events. use wxClientDC to paint outside the paint event
    SetCurrent(*m_glContext);
    //Screen Centre displayed as red dot
    glPointSize(5.0f);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_POINT_SMOOTH);
    glBegin(GL_POINTS);
    glColor3f(1.0f,0.0f,0.0f);
    glVertex3f(0.0f,0.0f,0.0f);
    glEnd();
}

void BasicGLPane::Draw_Axis()
{
    wxClientDC dc(this); // only to be used in paint events. use wxClientDC to paint outside the paint event
    SetCurrent(*m_glContext);  //means wxGLCanvas::SetCurrent(*m_context);
    /*-----------------------------------------------------------------------------------------------------*/
    //axes lines
    glPushMatrix();//glPushMatrix pushes the current matrix stack down by one,duplicating the current matrix.
    //glPushMatrix remembers the default position of the coordinate system
    glLoadIdentity();
    //Load camera position, so that we can start drawing shapes correctly
    gluLookAt(0.0,0.0,0.0+1.0,  //look from camera
              0.0,0.0,0.0,     //look at origin //centre of the screen
              0.0f,1.0f,0.0f);//YCoord axis up vector
    int iWidth = 900/5.0;
    int iHeight = 600/5.0;
    glViewport(28, 20, (GLsizei)iWidth, (GLsizei)iHeight); // Set our viewport to the size of our window
    // and multiply matrices to glmodelview's top stack and render
    glMultMatrixd(mdrgQuaternionMatrix);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_LINE_SMOOTH);
    glLineWidth(5.0);
    glBegin(GL_LINES);
    glColor3f(0.8f,0.0f,0.0f);//XCoord axis
    glVertex3f(0.0,0.0,0.0);
    glVertex3f(0.5,0.0,0.0);

    glColor3f(0.8f,0.8f,0.0f);//YCoord axis
    glVertex3f(0.0,0.0,0.0);
    glVertex3f(0.0,0.5,0.0);

    glColor3f(0.0f,0.8f,0.0f);//ZCoord axis
    glVertex3f(0.0,0.0,0.0);
    glVertex3f(0.0,0.0,0.5);
    glEnd();
    glPopMatrix();
    /*-----------------------------------------------------------------------------------------------------*/
    //axes cone x
    glPushMatrix();
    glLoadIdentity();
    //Load camera position, so that we can start drawing shapes correctly
    gluLookAt(0.0,0.0,0.0+1.0,   //look from camera
              0.0,0.0,0.0,      //look at origin //centre of the screen
              0.0f,1.0f,0.0f); //YCoord axis up vector
    iWidth = 900/5.0;
    iHeight = 600/5.0;
    glViewport(28, 20, (GLsizei)iWidth, (GLsizei)iHeight); // Set our viewport to the size of our window,
    // and multiply matrices to glmodelview's top stack and render
    glMultMatrixd(mdrgQuaternionMatrix);
    glTranslatef(0.5,0.0,0.0);
    glRotatef(90,0.0f,1.0f,0.0f);
    glColor3f(0.8f,0.0f,0.0f);//XCoord axis
    glutSolidCone(0.1,0.1,100,100);
    glPopMatrix();
    /*-----------------------------------------------------------------------------------------------------*/
    //axes cone y
    glPushMatrix();
    glLoadIdentity();
    //Load camera position, so that we can start drawing shapes correctly
    gluLookAt(0.0,0.0,0.0+1.0, //look from camera
              0.0,0.0,0.0,        //look at origin //centre of the screen
              0.0f,1.0f,0.0f);                                       //YCoord axis up vector
    iWidth = 900/5.0;
    iHeight = 600/5.0;
    glViewport(28, 20, (GLsizei)iWidth, (GLsizei)iHeight); // Set our viewport to the size of our window
    // and multiply matrices to glmodelview's top stack and render
    glMultMatrixd(mdrgQuaternionMatrix);
    glTranslatef(0.0,0.5,0.0);
    glRotatef(-90,1.0f,0.0f,0.0f);
    glColor3f(0.8f,0.8f,0.0f);//YCoord axis
    glutSolidCone(0.1,0.1,100,100);
    glPopMatrix();
    /*-----------------------------------------------------------------------------------------------------*/
    //axes cone z
    glPushMatrix();
    glLoadIdentity();
    //Load camera position, so that we can start drawing shapes correctly
    gluLookAt(0.0,0.0,0.0+1.0,  //look from camera
              0.0,0.0,0.0,     //look at origin //centre of the screen
              0.0f,1.0f,0.0f);//YCoord axis up vector
    iWidth = 900/5.0;
    iHeight = 600/5.0;
    glViewport(28, 20, (GLsizei)iWidth, (GLsizei)iHeight); // Set our viewport to the size of our window
    // and multiply matrices to glmodelview's top stack and render
    glMultMatrixd(mdrgQuaternionMatrix);
    glTranslatef(0.0,0.0,0.5);
    glColor3f(0.0f,0.8f,0.0f);//ZCoord axis
    glutSolidCone(0.1,0.1,100,100);
    glPopMatrix();
    /*-----------------------------------------------------------------------------------------------------*/
    //Coordinate axis Centre displayed as a small dot
    glPushMatrix();
    glLoadIdentity();
    //Load camera position, so that we can start drawing shapes correctly
    gluLookAt(0.0,0.0,0.0+1.0,  //look from camera
              0.0,0.0,0.0,     //look at origin //centre of the screen
              0.0f,1.0f,0.0f);//YCoord axis up vector
    iWidth = 900/5.0;
    iHeight = 600/5.0;
    glViewport(28, 20, (GLsizei)iWidth, (GLsizei)iHeight); // Set our viewport to the size of our window
    // and multiply matrices to glmodelview's top stack and render
    glMultMatrixd(mdrgQuaternionMatrix);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_BLEND);
    glEnable(GL_POINT_SMOOTH);
    glPointSize(10.0f);
    glBegin(GL_POINTS);
    glColor3f(1.0f,1.0f,1.0f);
    glVertex3f(0.0,0.0,0.0);
    glEnd();
    glPopMatrix();
    /*-----------------------------------------------------------------------------------------------------*/
}

void BasicGLPane::ReadStlFile()
{
    float fXCoordinate,fYCoordinate,fZCoordinate;
    float fNormalCoord[] = {0.0,0.0,1.0};
    float fNormal[] = {0.0,0.0,1.0};
    float fVectorLength;
    float fNormalisedCoord[] = {0.0,0.0,1.0};
    float fVertex1[3],fVertex2[3],fVertex3[3];
    float fVector1[3],fVector2[3];
    int iNodeId = 2,iElemId = 2, iElemCount = 0;
    vector<int> vecElementConnectivity;
    wxString strFilePathName;
    if(wcslen(m_wxstrFilePath) != 0)
    {
        strFilePathName.Append(m_wxstrFilePath);
        strFilePathName.Append("\\");
        strFilePathName.Append(m_wxstrFileName);
        cout<<"Loading file "<<strFilePathName<<"...."<<"\n";
        char str[60];
        char character[10];
        int iIncrement = 0;
        float fXCoordinate,fYCoordinate,fZCoordinate;
        ifstream fstrReadSTL(strFilePathName, ios::in);
        if(fstrReadSTL.is_open())
        {
            fstrReadSTL.getline(str,100);//Reads solid####
            while(!fstrReadSTL.eof())
            {
                iIncrement++;
                fstrReadSTL.getline(str,100);
                if(iIncrement == 1)
                {
                    sscanf(str,"%s %s %f %f %f", &character, &character, &fNormalCoord[0], &fNormalCoord[1], &fNormalCoord[2]);
                    //normalising is needed incase of using glscale which shortens the normals
                    //normalising is dividing the vector coordinates by its own length.
                    //normalised vector has length=1, which is its unit vector.
//                    fVectorLength = sqrt(pow(fNormalCoord[0],2)+pow(fNormalCoord[1],2)+pow(fNormalCoord[2],2));
//                    fNormalisedCoord[0] = fNormalCoord[0]/(fVectorLength);
//                    fNormalisedCoord[1] = fNormalCoord[1]/(fVectorLength);
//                    fNormalisedCoord[2] = fNormalCoord[2]/(fVectorLength);
//                    testforvalue1 = sqrt(pow(fNormalisedCoord[0],2)+pow(fNormalisedCoord[1],2)+pow(fNormalisedCoord[2],2));
                }
                if((iIncrement == 3) || (iIncrement == 4) || (iIncrement == 5))
                {
                    sscanf(str,"%s %f %f %f", &character, &fXCoordinate, &fYCoordinate, &fZCoordinate);
                    //insert nodes in oNodeData
                    if(!mbread_deerhead)
                        oNodeData.Insert_Node_Attributes(iNodeId,iNodeId,1,fXCoordinate,fYCoordinate,fZCoordinate,1,1,1);
                    iNodeId++;
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
                    }
                }
                if(iIncrement == 7)
                {
                    iIncrement = 0;
                    //insert elements in oElementData
                    vecElementConnectivity.push_back(iNodeId - 3);//eg 0
                    vecElementConnectivity.push_back(iNodeId - 2);//   1
                    vecElementConnectivity.push_back(iNodeId - 1);//   2
                    if(!mbread_deerhead)
                    {
                        oElementData.Insert_Node_Attributes(iElemId,4,"CTRIA3",0,vecElementConnectivity);//elem strength is entered as zero
                        iElemCount++;
                    }
                    /*store deer head once for all*/
                    if(mbread_deerhead)
                    {
                        sDeerHead sTriangleFace;
                        for(int i = 0; i < 3; i++)
                        {
                            sTriangleFace.fFaceNormal[i]   = fNormal[i];
                            sTriangleFace.fFirstVertex[i]  = fVertex1[i];
                            sTriangleFace.fSecondVertex[i] = fVertex2[i];
                            sTriangleFace.fThirdVertex[i]  = fVertex3[i];
                        }
                        vecDeerHead.push_back(sTriangleFace);
                    }
                    /**/
                    vecElementConnectivity.clear();
                    iElemId++;
                }
            }
            fstrReadSTL.close();
        }
    }
    oElementData.SetElemStrength(iElemCount);
}

void BasicGLPane::Read_ModelCentre()
{
    m_fModelCentre_XMax = 321;//random number
    float fXCoordinate,fYCoordinate,fZCoordinate;
    float fNormalCoord[] = {0.0,0.0,1.0};
    float fNormal[] = {0.0,0.0,1.0};
    float fVectorLength;
    float fNormalisedCoord[] = {0.0,0.0,1.0};
    float fVertex1[3],fVertex2[3],fVertex3[3];
    float fVector1[3],fVector2[3];
    float testforvalue1;
    wxString strFilePathName;
    if(wcslen(m_wxstrFilePath) != 0)
    {
        strFilePathName.Append(m_wxstrFilePath);
        strFilePathName.Append("/");
        strFilePathName.Append(m_wxstrFileName);
        char str[60];
        char character[10];
        int iIncrement = 0;
        float fXCoordinate,fYCoordinate,fZCoordinate;
        ifstream fstrReadSTL(strFilePathName, ios::in);
        if(fstrReadSTL.is_open())
        {
            fstrReadSTL.getline(str,100);//Reads solid####
            while(!fstrReadSTL.eof())
            {
                iIncrement++;
                fstrReadSTL.getline(str,100);
                if(iIncrement == 1)
                {
                    sscanf(str,"%s %s %f %f %f", &character, &character, &fNormalCoord[0], &fNormalCoord[1], &fNormalCoord[2]);
                    //normalising is needed incase of using glscale which shortens the normals
                    //normalising is dividing the vector coordinates by its own length.
                    //normalised vector has length=1, which is its unit vector.
//                    fVectorLength = sqrt(pow(fNormalCoord[0],2)+pow(fNormalCoord[1],2)+pow(fNormalCoord[2],2));
//                    fNormalisedCoord[0] = fNormalCoord[0]/(fVectorLength);
//                    fNormalisedCoord[1] = fNormalCoord[1]/(fVectorLength);
//                    fNormalisedCoord[2] = fNormalCoord[2]/(fVectorLength);
//                    testforvalue1 = sqrt(pow(fNormalisedCoord[0],2)+pow(fNormalisedCoord[1],2)+pow(fNormalisedCoord[2],2));
                }
                if((iIncrement == 3) || (iIncrement == 4) || (iIncrement == 5))
                {
                    sscanf(str,"%s %f %f %f", &character, &fXCoordinate, &fYCoordinate, &fZCoordinate);
                    //push triangle coordinates into stl vector
                    mvecfXCoordinate.push_back(fXCoordinate);
                    mvecfYCoordinate.push_back(fYCoordinate);
                    mvecfZCoordinate.push_back(fZCoordinate);
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
                        //push triangle coordinate normals into stl vector
                        mvecNormalX.push_back(fNormal[0]);
                        mvecNormalY.push_back(fNormal[1]);
                        mvecNormalZ.push_back(fNormal[2]);
                    }
                    if(m_fModelCentre_XMax == 321)
                    {
                        m_fModelCentre_XMax = fXCoordinate;
                        m_fModelCentre_YMax = fYCoordinate;
                        m_fModelCentre_ZMax = fZCoordinate;
                        m_fModelCentre_XMin = fXCoordinate;
                        m_fModelCentre_YMin = fYCoordinate;
                        m_fModelCentre_ZMin = fZCoordinate;
                    }
                    if(m_fModelCentre_XMax < fXCoordinate)
                        m_fModelCentre_XMax = fXCoordinate;
                    if(m_fModelCentre_YMax < fYCoordinate)
                        m_fModelCentre_YMax = fYCoordinate;
                    if(m_fModelCentre_ZMax < fZCoordinate)
                        m_fModelCentre_ZMax = fZCoordinate;
                    if(m_fModelCentre_XMin > fXCoordinate)
                        m_fModelCentre_XMin = fXCoordinate;
                    if(m_fModelCentre_YMin > fYCoordinate)
                        m_fModelCentre_YMin = fYCoordinate;
                    if(m_fModelCentre_ZMin > fZCoordinate)
                        m_fModelCentre_ZMin = fZCoordinate;
                }
                if(iIncrement == 7)
                    iIncrement = 0;
            }
        }
        fstrReadSTL.close();
        //calculates model centre=>dist b/w screen centre and model centre
        m_fModelCentre_X = (m_fModelCentre_XMax + m_fModelCentre_XMin)/2;
        m_fModelCentre_Y = (m_fModelCentre_YMax + m_fModelCentre_YMin)/2;
        m_fModelCentre_Z = (m_fModelCentre_ZMax + m_fModelCentre_ZMin)/2;
        //move model & boundbox to screen centre
        m_fModelCentre_XMax = m_fModelCentre_XMax - m_fModelCentre_X;
        m_fModelCentre_YMax = m_fModelCentre_YMax - m_fModelCentre_Y;
        m_fModelCentre_ZMax = m_fModelCentre_ZMax - m_fModelCentre_Z;
        m_fModelCentre_XMin = m_fModelCentre_XMin - m_fModelCentre_X;
        m_fModelCentre_YMin = m_fModelCentre_YMin - m_fModelCentre_Y;
        m_fModelCentre_ZMin = m_fModelCentre_ZMin - m_fModelCentre_Z;
        //find Model's Maximum Axis Value value(both positive and negative directions)
        float fXYZMaxMinDiff[6] = {m_fModelCentre_XMax,m_fModelCentre_YMax,m_fModelCentre_ZMax,
                                   m_fModelCentre_XMin,m_fModelCentre_YMin,m_fModelCentre_ZMin
                                  };
        m_fModelMaxAxisValue = fXYZMaxMinDiff[0];
        for(int iIter = 1; iIter < 6; iIter++)
        {
            if(fXYZMaxMinDiff[iIter] < 0)
                fXYZMaxMinDiff[iIter] = -(fXYZMaxMinDiff[iIter]);
            if(m_fModelMaxAxisValue < fXYZMaxMinDiff[iIter])
            {
                m_fModelMaxAxisValue = fXYZMaxMinDiff[iIter];
            }
        }
        //scale the model axes max min values
        m_fModelCentre_XMax = (0.7*m_fModelCentre_XMax)/m_fModelMaxAxisValue;
        m_fModelCentre_YMax = (0.7*m_fModelCentre_YMax)/m_fModelMaxAxisValue;
        m_fModelCentre_ZMax = (0.7*m_fModelCentre_ZMax)/m_fModelMaxAxisValue;
        m_fModelCentre_XMin = (0.7*m_fModelCentre_XMin)/m_fModelMaxAxisValue;
        m_fModelCentre_YMin = (0.7*m_fModelCentre_YMin)/m_fModelMaxAxisValue;
        m_fModelCentre_ZMin = (0.7*m_fModelCentre_ZMin)/m_fModelMaxAxisValue;
        //move model to screen centre
        for(int iIter = 0; iIter < mvecfXCoordinate.size(); iIter++)
        {
            mvecfXCoordinate[iIter] = mvecfXCoordinate[iIter] - m_fModelCentre_X;
            mvecfYCoordinate[iIter] = mvecfYCoordinate[iIter] - m_fModelCentre_Y;
            mvecfZCoordinate[iIter] = mvecfZCoordinate[iIter] - m_fModelCentre_Z;
        }
        //scale the model axes max min values
        for(int iIter = 0; iIter < mvecfXCoordinate.size(); iIter++)
        {
            mvecfXCoordinate[iIter] = (0.7*mvecfXCoordinate[iIter])/m_fModelMaxAxisValue;
            mvecfYCoordinate[iIter] = (0.7*mvecfYCoordinate[iIter])/m_fModelMaxAxisValue;
            mvecfZCoordinate[iIter] = (0.7*mvecfZCoordinate[iIter])/m_fModelMaxAxisValue;
        }
        //assign
        m_fLength = m_fModelCentre_XMax + (-m_fModelCentre_XMin);
        m_fHeight =  m_fModelCentre_YMax + (-m_fModelCentre_YMin);
        m_fDepth =  m_fModelCentre_ZMax + (-m_fModelCentre_ZMin);
    }
}

void BasicGLPane::Draw_Room()
{
    wxClientDC dc(this); // only to be used in paint events. use wxClientDC to paint outside the paint event
    SetCurrent(*m_glContext);
    if(mbCalcOnce)
    {
        /*******one time calculation*******/
        //scale the model centre coords
        m_fModelCentre_X = (0.7*m_fModelCentre_X)/m_fModelMaxAxisValue;
        m_fModelCentre_Y = (0.7*m_fModelCentre_Y)/m_fModelMaxAxisValue;
        m_fModelCentre_Z = (0.7*m_fModelCentre_Z)/m_fModelMaxAxisValue;
        //move model centre to screen centre
        m_fModelCentre_X = m_fModelCentre_X - m_fModelCentre_X;
        m_fModelCentre_Y = m_fModelCentre_Y - m_fModelCentre_Y;
        m_fModelCentre_Z = m_fModelCentre_Z - m_fModelCentre_Z;
        /**********************************/
        mbCalcOnce = false;
    }
    if(m_iBoundBox == 1)
    {
        float iFrontLength  = m_fMaxModelDim*2.2;
        float iFrontHeight  = m_fMaxModelDim*2.2;
        float iFrontDepth   = m_fMaxModelDim*2;
        float iRearLength   = m_fMaxModelDim/1.3;
        float iRearHeight   = m_fMaxModelDim/1.3;
        float iRearDepth    = m_fMaxModelDim*2;
        glPushMatrix();
        glLoadIdentity();
        //Load camera position, so that we can start drawing shapes correctly
        gluLookAt(m_fModelCentre_X,m_fModelCentre_Y,m_fMaxModelDim, //look from camera
                  m_fModelCentre_X,m_fModelCentre_Y,m_fModelCentre_Z,      //look at origin //centre of the screen
                  0.0f,1.0f,0.0f);                                        //YCoord axis up vector
        int iWidth = getWidth();
        int iHeight = getHeight();
        glViewport(0, 0, (GLsizei)iWidth, (GLsizei)iHeight); // Set our viewport to the size of our window
        // and multiply matrices to glmodelview's top stack and render
        //glMultMatrixd(mdrgScaleViewMatrix);//initial arg is identity matrix
        glScalef(0.4f,0.4f,0.4f);
        //glTranslatef(0.5,0.0,0.0);
        glRotatef(2,-1.0f,0.0f,0.0f);
        //glMultMatrixd(mdrgQuaternionMatrix);
//        glLineWidth(2.0);
//        glColor3f(1.0f,1.0f,1.0f);
//        glBegin(GL_LINES);
//        glVertex3f(-iRearLength,-iRearHeight,-iRearDepth);//5-6
//        glVertex3f(iRearLength,-iRearHeight,-iRearDepth);
//
//        glVertex3f(iRearLength,-iRearHeight,-iRearDeptmbDataStructurePopulatedh);//6-7
//        glVertex3f(iRearLength,iRearHeight,-iRearDepth);
//
//        glVertex3f(iRearLength,iRearHeight,-iRearDepth);//7-8
//        glVertex3f(-iRearLength,iRearHeight,-iRearDepth);
//
//        glVertex3f(-iRearLength,iRearHeight,-iRearDepth);//8-5
//        glVertex3f(-iRearLength,-iRearHeight,-iRearDepth);
//
//        glVertex3f(-iFrontLength,-iFrontHeight,iFrontDepth);//1-5
//        glVertex3f(-iRearLength,-iRearHeight,-iRearDepth);
//
//        glVertex3f(iFrontLength,-iFrontHeight,iFrontDepth);//2-6
//        glVertex3f(iRearLength,-iRearHeight,-iRearDepth);
//
//        glVertex3f(iFrontLength,iFrontHeight,iFrontDepth);//3-7
//        glVertex3f(iRearLength,iRearHeight,-iRearDepth);
//
//        glVertex3f(-iFrontLength,iFrontHeight,iFrontDepth);//4-8
//        glVertex3f(-iRearLength,iRearHeight,-iRearDepth);
//        glEnd();
        /****************************************************/
        glColor3f(0.5f, 0.5f, 0.5f);
        glBegin(GL_TRIANGLES);
        glNormal3f(1.0,0.0,0.0);
        glVertex3f(iFrontLength,-iFrontHeight,iFrontDepth);//2 6 7
        glVertex3f(iRearLength,-iRearHeight,-iRearDepth);
        glVertex3f(iRearLength,iRearHeight,-iRearDepth);

        glVertex3f(iFrontLength,-iFrontHeight,iFrontDepth);//2 7 3
        glVertex3f(iRearLength,iRearHeight,-iRearDepth);
        glVertex3f(iFrontLength,iFrontHeight,iFrontDepth);

        glNormal3f(-1.0,0.0,0.0);
        glVertex3f(-iRearLength,-iRearHeight,-iRearDepth);//5 1 4
        glVertex3f(-iFrontLength,-iFrontHeight,iFrontDepth);
        glVertex3f(-iFrontLength,iFrontHeight,iFrontDepth);

        glVertex3f(-iRearLength,-iRearHeight,-iRearDepth);//5 4 8
        glVertex3f(-iFrontLength,iFrontHeight,iFrontDepth);
        glVertex3f(-iRearLength,iRearHeight,-iRearDepth);

        glNormal3f(0.0,0.0,-1.0);
        glVertex3f(iRearLength,-iRearHeight,-iRearDepth);//6 5 8
        glVertex3f(-iRearLength,-iRearHeight,-iRearDepth);
        glVertex3f(-iRearLength,iRearHeight,-iRearDepth);

        glVertex3f(iRearLength,-iRearHeight,-iRearDepth);// 6 8 7
        glVertex3f(-iRearLength,iRearHeight,-iRearDepth);
        glVertex3f(iRearLength,iRearHeight,-iRearDepth);

        glNormal3f(0.0,1.0,0.0);
        glVertex3f(-iFrontLength,iFrontHeight,iFrontDepth);//4 3 7
        glVertex3f(iFrontLength,iFrontHeight,iFrontDepth);
        glVertex3f(iRearLength,iRearHeight,-iRearDepth);

        glVertex3f(-iFrontLength,iFrontHeight,iFrontDepth);//4 7 8
        glVertex3f(iRearLength,iRearHeight,-iRearDepth);
        glVertex3f(-iRearLength,iRearHeight,-iRearDepth);

        glNormal3f(0.0,-1.0,0.0);
        glVertex3f(-iFrontLength,-iFrontHeight,iFrontDepth);//1 2 6
        glVertex3f(iFrontLength,-iFrontHeight,iFrontDepth);
        glVertex3f(iRearLength,-iRearHeight,-iRearDepth);

        glVertex3f(-iFrontLength,-iFrontHeight,iFrontDepth);//1 6 5
        glVertex3f(iRearLength,-iRearHeight,-iRearDepth);
        glVertex3f(-iRearLength,-iRearHeight,-iRearDepth);
        glEnd();
        glPopMatrix();

        //draw deer head twice
        if(mbDeerHeadPopulated)
        {
            /****first head****/
            /*glPushMatrix();
            glLoadIdentity();
            //Load camera position, so that we can start drawing shapes correctly
            gluLookAt(m_fModelCentre_X,m_fModelCentre_Y,m_fMaxModelDim, //look from camera
                      m_fModelCentre_X,m_fModelCentre_Y,m_fModelCentre_Z,      //look at origin //centre of the screen
                      0.0f,1.0f,0.0f);                                        //YCoord axis up vector
            int iWidth = getWidth();
            int iHeight = getHeight();
            glViewport(0, 0, (GLsizei)iWidth, (GLsizei)iHeight); // Set our viewport to the size of our window
            // and multiply matrices to glmodelview's top stack and render
            glScalef(0.003f,0.003f,0.003f);
            glTranslatef(-280.0f,180.0f,0.0f);
            glRotatef(90,0.0f,0.0f,1.0f);
            glRotatef(70,1.0f,0.0f,0.0f);
            glColor3f(0.7294f, 0.5294f, 0.349f);
            glBegin(GL_TRIANGLES);
            for(int i = 0; i < vecDeerHead.size(); i++)
            {
                glNormal3f(vecDeerHead[i].fFaceNormal [0],   vecDeerHead[i].fFaceNormal[1],   vecDeerHead[i].fFaceNormal[2]);
                glVertex3f(vecDeerHead[i].fFirstVertex[0],  vecDeerHead[i].fFirstVertex[1],  vecDeerHead[i].fFirstVertex[2]);
                glVertex3f(vecDeerHead[i].fSecondVertex[0], vecDeerHead[i].fSecondVertex[1], vecDeerHead[i].fSecondVertex[2]);
                glVertex3f(vecDeerHead[i].fThirdVertex[0],  vecDeerHead[i].fThirdVertex[1],  vecDeerHead[i].fThirdVertex[2]);
            }
            glEnd();
            glPopMatrix();
            /****second head****/
            /*glPushMatrix();
            glLoadIdentity();
            //Load camera position, so that we can start drawing shapes correctly
            gluLookAt(m_fModelCentre_X,m_fModelCentre_Y,m_fMaxModelDim, //look from camera
                      m_fModelCentre_X,m_fModelCentre_Y,m_fModelCentre_Z,      //look at origin //centre of the screen
                      0.0f,1.0f,0.0f);                                        //YCoord axis up vector
            iWidth = getWidth();
            iHeight = getHeight();
            glViewport(0, 0, (GLsizei)iWidth, (GLsizei)iHeight); // Set our viewport to the size of our window
            // and multiply matrices to glmodelview's top stack and render
            glScalef(0.001f,0.001f,0.001f);
            glTranslatef(-500.0f,300.0f,0.0f);
            glRotatef(90,0.0f,0.0f,1.0f);
            glRotatef(70,1.0f,0.0f,0.0f);
            //            glColor3f(0.5f, 0.35f, 0.05f);
            glColor3f(0.7294f, 0.5294f, 0.349f);
            glBegin(GL_TRIANGLES);
            for(int i = 0; i < vecDeerHead.size(); i++)
            {
                glNormal3f(vecDeerHead[i].fFaceNormal[0],   vecDeerHead[i].fFaceNormal[1],   vecDeerHead[i].fFaceNormal[2]);
                glVertex3f(vecDeerHead[i].fFirstVertex[0],  vecDeerHead[i].fFirstVertex[1],  vecDeerHead[i].fFirstVertex[2]);
                glVertex3f(vecDeerHead[i].fSecondVertex[0], vecDeerHead[i].fSecondVertex[1], vecDeerHead[i].fSecondVertex[2]);
                glVertex3f(vecDeerHead[i].fThirdVertex[0],  vecDeerHead[i].fThirdVertex[1],  vecDeerHead[i].fThirdVertex[2]);
            }
            glEnd();
            glPopMatrix();*/
        }
    }
}

void BasicGLPane::Draw_Model()
{
    int iCounter = 0,iIncr = 0;
    wxClientDC dc(this); // only to be used in paint events. use wxClientDC to paint outside the paint event
    SetCurrent(*m_glContext);

    if(mbCalcOnce)
    {
        /*******one time calculation*******/
        //scale the model centre coords
        m_fModelCentre_X = (0.7*m_fModelCentre_X)/m_fModelMaxAxisValue;
        m_fModelCentre_Y = (0.7*m_fModelCentre_Y)/m_fModelMaxAxisValue;
        m_fModelCentre_Z = (0.7*m_fModelCentre_Z)/m_fModelMaxAxisValue;
        //move model centre to screen centre
        m_fModelCentre_X = m_fModelCentre_X - m_fModelCentre_X;
        m_fModelCentre_Y = m_fModelCentre_Y - m_fModelCentre_Y;
        m_fModelCentre_Z = m_fModelCentre_Z - m_fModelCentre_Z;
        /**********************************/
        mbCalcOnce = false;
    }
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
    glPushMatrix();//remembers the default position of the coordinate system
    glLoadIdentity();
    //Load camera position, so that we can start drawing shapes correctly
    gluLookAt(m_fModelCentre_X,m_fModelCentre_Y,m_fMaxModelDim, //look from camera
              m_fModelCentre_X,m_fModelCentre_Y,m_fModelCentre_Z,      //look at origin //centre of the screen
              0.0f,1.0f,0.0f);                                        //YCoord axis up vector
    int iWidth = getWidth();
    int iHeight = getHeight();
    glViewport(0, 0, (GLsizei)iWidth, (GLsizei)iHeight); // Set our viewport to the size of our window
    // and multiply matrices to glmodelview's top stack and render
    glMultMatrixd(mdrgTranslationMatrix);//initial arg is identity matrix
    glMultMatrixd(mdrgScaleViewMatrix);//initial arg is identity matrix
    glScalef(0.6f,0.6f,0.6f);
    if(mbAutorotation)
    {
        double dQuaternionLength,a11,a12,a13,a14,a21,a22,a23,a24,a31,a32,a33,a34,a41,a42,a43,a44;
        //quaternion recursive multiplication for autorotation
        mdQuatX = mdAutoQuatW*mdQuatX + mdAutoQuatX*mdQuatW + mdAutoQuatY*mdQuatZ + mdAutoQuatZ*mdQuatY;
        mdQuatY = mdAutoQuatW*mdQuatY + mdAutoQuatY*mdQuatW + mdAutoQuatZ*mdQuatX - mdAutoQuatX*mdQuatZ;
        mdQuatZ = mdAutoQuatW*mdQuatZ + mdAutoQuatZ*mdQuatW + mdAutoQuatX*mdQuatY - mdAutoQuatY*mdQuatX;
        mdQuatW = mdAutoQuatW*mdQuatW - mdAutoQuatX*mdQuatX - mdAutoQuatY*mdQuatY - mdAutoQuatZ*mdQuatZ;
        dQuaternionLength = sqrt(mdQuatW*mdQuatW + mdQuatX*mdQuatX  + mdQuatY*mdQuatY + mdQuatZ*mdQuatZ);//magnitude of a quaternion
        //normalizing multiplied quaternion
        mdQuatX = mdQuatX/dQuaternionLength;
        mdQuatY = mdQuatY/dQuaternionLength;
        mdQuatZ = mdQuatZ/dQuaternionLength;
        mdQuatW = mdQuatW/dQuaternionLength;
        //http://en.wikipedia.org/wiki/Rotation_matrix#Quaternion
        a11 = 1.0 - 2.0*(mdQuatY*mdQuatY)-2.0*(mdQuatZ*mdQuatZ);
        a12 = 2*mdQuatX*mdQuatY - 2*mdQuatZ*mdQuatW;
        a13 = 2*mdQuatX*mdQuatZ + 2*mdQuatY*mdQuatW;
        a14 = 0.0;
        a21 = 2*mdQuatX*mdQuatY + 2*mdQuatZ*mdQuatW;
        a22 = 1.0 - 2.0*(mdQuatX*mdQuatX)-2.0*(mdQuatZ*mdQuatZ);
        a23 = 2*mdQuatY*mdQuatZ - 2*mdQuatX*mdQuatW;
        a24 = 0.0;
        a31 = 2*mdQuatX*mdQuatZ - 2*mdQuatY*mdQuatW;
        a32 = 2*mdQuatY*mdQuatZ + 2*mdQuatX*mdQuatW;
        a33 = 1.0 - 2.0*(mdQuatX*mdQuatX)-2.0*(mdQuatY*mdQuatY);
        a34 = 0.0;
        a41 = 0.0;
        a42 = 0.0;
        a43 = 0.0;
        a44 = 1.0;
        //matrix stored as column order in array
        mdrgQuaternionMatrix[0] = a11;
        mdrgQuaternionMatrix[1] = a21;
        mdrgQuaternionMatrix[2] = a31;
        mdrgQuaternionMatrix[3] = a41;
        mdrgQuaternionMatrix[4] = a12;
        mdrgQuaternionMatrix[5] = a22;
        mdrgQuaternionMatrix[6] = a32;
        mdrgQuaternionMatrix[7] = a42;
        mdrgQuaternionMatrix[8] = a13;
        mdrgQuaternionMatrix[9] = a23;
        mdrgQuaternionMatrix[10] = a33;
        mdrgQuaternionMatrix[11] = a43;
        mdrgQuaternionMatrix[12] = a14;
        mdrgQuaternionMatrix[13] = a24;
        mdrgQuaternionMatrix[14] = a34;
        mdrgQuaternionMatrix[15] = a44;
    }
    glMultMatrixd(mdrgQuaternionMatrix);
    glColor3f(0.0,0.5,0.5);//cyan color

    if(mbDataStructurePopulated)
        DrawElementsInorder(oElementData.GetRootNode(), miSubcaseId);

    glPopMatrix();//restore the coordinate system back to its original state
    glDisable(GL_CULL_FACE);
    //Draw_ModelBoundingBox();
    Display_ModelCentre();
    if(mstrResultId.size() > 0)
        DrawLegend();
    m_InsertOnce++;
}

void BasicGLPane::Display_ModelCentre()
{
    wxClientDC dc(this); // only to be used in paint events. use wxClientDC to paint outside the paint event
    SetCurrent(*m_glContext);
    if(mbCalcOnce)
    {
        /*******one time calculation*******/
        //scale the model centre coords
        m_fModelCentre_X = (0.7*m_fModelCentre_X)/m_fModelMaxAxisValue;
        m_fModelCentre_Y = (0.7*m_fModelCentre_Y)/m_fModelMaxAxisValue;
        m_fModelCentre_Z = (0.7*m_fModelCentre_Z)/m_fModelMaxAxisValue;
        //move model centre to screen centre
        m_fModelCentre_X = m_fModelCentre_X - m_fModelCentre_X;
        m_fModelCentre_Y = m_fModelCentre_Y - m_fModelCentre_Y;
        m_fModelCentre_Z = m_fModelCentre_Z - m_fModelCentre_Z;
        /**********************************/
        mbCalcOnce = false;
    }
    //Model Centre displayed as green dot
    glPushMatrix();
    glLoadIdentity();
    //Load camera position, so that we can start drawing shapes correctly
    gluLookAt(m_fModelCentre_X,m_fModelCentre_Y,m_fMaxModelDim, //look from camera
              m_fModelCentre_X,m_fModelCentre_Y,m_fModelCentre_Z,      //look at origin //centre of the screen
              0.0f,1.0f,0.0f);                                        //YCoord axis up vector
    // and multiply matrices to glmodelview's top stack and render
    glPointSize(5.0f);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_POINT_SMOOTH);//old way of smoothening points and lines, go for anti-aliasing.

    //Immediate mode
    glBegin(GL_POINTS);
    glColor3f(0.0f,1.0f,0.0f);
    glVertex3f(m_fModelCentre_X,m_fModelCentre_Y,m_fModelCentre_Z);
    glEnd();
    //--------------

    glPopMatrix();
}

void BasicGLPane::Draw_ModelBoundingBox()
{
    wxClientDC dc(this); // only to be used in paint events. use wxClientDC to paint outside the paint event
    SetCurrent(*m_glContext);
    if(m_iBoundBox == 1)
    {
        glPushMatrix();
        glLoadIdentity();
        //Load camera position, so that we can start drawing shapes correctly
        gluLookAt(m_fModelCentre_X,m_fModelCentre_Y,m_fMaxModelDim+1.0, //look from camera
                  m_fModelCentre_X,m_fModelCentre_Y,m_fModelCentre_Z,      //look at origin //centre of the screen
                  0.0f,1.0f,0.0f);                                        //YCoord axis up vector
        int iWidth = getWidth();
        int iHeight = getHeight();
        glViewport(0, 0, (GLsizei)iWidth, (GLsizei)iHeight); // Set our viewport to the size of our window
        // and multiply matrices to glmodelview's top stack and render
        glMultMatrixd(mdrgTranslationMatrix);//initial arg is identity matrix
        glMultMatrixd(mdrgScaleViewMatrix);//initial arg is identity matrix
        if(mbAutorotation)
            glMultMatrixd(mdrgAutorotationMatrix);//multiplies stacktop by quaternion recursively
        glMultMatrixd(mdrgQuaternionMatrix);
        glLineWidth(2.0);
        glColor3f(1.0f,1.0f,1.0f);
        glBegin(GL_LINES);
        glVertex3f(-m_fLength/2,-m_fHeight/2,m_fDepth/2);//1-2
        glVertex3f(m_fLength/2,-m_fHeight/2,m_fDepth/2);

        glVertex3f(m_fLength/2,-m_fHeight/2,m_fDepth/2);//2-3
        glVertex3f(m_fLength/2,m_fHeight/2,m_fDepth/2);

        glVertex3f(m_fLength/2,m_fHeight/2,m_fDepth/2);//3-4
        glVertex3f(-m_fLength/2,m_fHeight/2,m_fDepth/2);

        glVertex3f(-m_fLength/2,m_fHeight/2,m_fDepth/2);//4-1
        glVertex3f(-m_fLength/2,-m_fHeight/2,m_fDepth/2);

        glVertex3f(-m_fLength/2,-m_fHeight/2,-m_fDepth/2);//5-6
        glVertex3f(m_fLength/2,-m_fHeight/2,-m_fDepth/2);

        glVertex3f(m_fLength/2,-m_fHeight/2,-m_fDepth/2);//6-7
        glVertex3f(m_fLength/2,m_fHeight/2,-m_fDepth/2);

        glVertex3f(m_fLength/2,m_fHeight/2,-m_fDepth/2);//7-8
        glVertex3f(-m_fLength/2,m_fHeight/2,-m_fDepth/2);

        glVertex3f(-m_fLength/2,m_fHeight/2,-m_fDepth/2);//8-5
        glVertex3f(-m_fLength/2,-m_fHeight/2,-m_fDepth/2);

        glVertex3f(-m_fLength/2,-m_fHeight/2,m_fDepth/2);//1-5
        glVertex3f(-m_fLength/2,-m_fHeight/2,-m_fDepth/2);

        glVertex3f(m_fLength/2,-m_fHeight/2,m_fDepth/2);//2-6
        glVertex3f(m_fLength/2,-m_fHeight/2,-m_fDepth/2);

        glVertex3f(m_fLength/2,m_fHeight/2,m_fDepth/2);//3-7
        glVertex3f(m_fLength/2,m_fHeight/2,-m_fDepth/2);

        glVertex3f(-m_fLength/2,m_fHeight/2,m_fDepth/2);//4-8
        glVertex3f(-m_fLength/2,m_fHeight/2,-m_fDepth/2);
        glEnd();
        glPopMatrix();
    }
}

void BasicGLPane::Render( wxPaintEvent& evt )
{
    //The proper approach is to use a more "sophisticated" data structure to represent the "orientation" of the
    //camera
    if(!IsShown())
        return;
    //wxGLCanvas::SetCurrent(*m_context);
    wxPaintDC dc(this); // only to be used in paint events. use wxClientDC to paint outside the paint event
    SetCurrent(*m_glContext);
//    glMatrixMode(GL_MODELVIEW); // Switch back to the model view matrix, so that we can start drawing shapes correctly
//Rendering step1:Clear color and depth buffers; load identity matrix
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
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
//Rendering step4:Render geometry
//Room, model, and model center has same eye position; Axis is in a different viewport
    Draw_Room();
    Draw_Model();
    Draw_Axis();
    if(mbAutorotation)
        Refresh();//calls drawmodel continuously and inturn multiplies stacktop matrix with current quaternion matrix continuously
    glFlush();
    SwapBuffers();//SwapBuffer command takes care of flushing and command processing
}

void BasicGLPane::MatrixMultiplication_4x4(double frgA[16],double frgB[16])
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

void BasicGLPane::mouseMoved(wxMouseEvent& event)
{
    wxClientDC dc(this); // only to be used in paint events. use wxClientDC to paint outside the paint event
    SetCurrent(*m_glContext);
    m_fWindWidth = getWidth();
    m_fWindHeight = getHeight();
    if(mbUpdateModelOnce)
    {
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
//        Rendering step2:Load camera position, so that we can start drawing shapes correctly
//        gluLookAt(m_fModelCentre_X,m_fModelCentre_Y,m_fmaxmodeldim+1.0, //look from camera
//                  m_fModelCentre_X,m_fModelCentre_Y,m_fModelCentre_Z,      //look at origin //centre of the screen
//                  0.0f,1.0f,0.0f);                                        //YCoord axis up vector
        int iWidth = getWidth();
        int iHeight = getHeight();
        glViewport(0, 0, (GLsizei)iWidth, (GLsizei)iHeight); // Set our viewport to the size of our window

        glMatrixMode(GL_PROJECTION); // Switch to the projection matrix so that we can manipulate how our scene is viewed
        glLoadIdentity(); // Reset the projection matrix to the identity matrix so that we don't get any artifacts (cleaning up)
        //Refer : http://www.opengl.org/sdk/docs/man/xhtml/glOrtho.xml
        glOrtho(-1,1,-1,1,-3,3);
        //glOrtho(-1,1,-1,1,-(m_fModelCentre_Z+(m_fmaxmodeldim/4.0)),(m_fModelCentre_Z+(m_fmaxmodeldim/4.0)));//multiply the projection matrix with orthographic matrix//LRBTNF
        //    gluPerspective(120, (GLfloat)iWidth / (GLfloat)iHeight, 1.0, 100.0); // Set the Field of view angle (in degrees), the aspect ratio of our window, and the new and far planes
        glMatrixMode(GL_MODELVIEW); // Switch back to the model view matrix
//        Refresh();
        ResetView();
        mbUpdateModelOnce = false;
    }
    if(event.LeftIsDown())//LeftDown and LeftIsDown: the former returns true when the event corresponds
    {
        //to the left mouse button click while the latter returns true if the left mouse
        //button is currently being pressed.
        RotateModelwithMouse(event);
    }
    else if(event.RightIsDown())
    {
        PanModelwithMouse(event);
    }
    else//value is updated, whenever mouse is moved
    {
        float fMouse3dCord[3];
        float fMouse3dCord_Length;
        m_fMouseX = event.GetX();
        m_fMouseY = event.GetY();

        //finding the point os hemisphere(radius=1) imagined to be projected on 2d screen
        fMouse3dCord[0] = (2.0*m_fMouseX - m_fWindWidth) / m_fWindWidth;
        fMouse3dCord[1] = (m_fWindHeight - 2.0*m_fMouseY) / m_fWindHeight;
        fMouse3dCord[2] = 0.0;
        fMouse3dCord[2] = sqrtf(1 - fMouse3dCord[0]*fMouse3dCord[0] + fMouse3dCord[1]*fMouse3dCord[1]);

        fMouse3dCord_Length = sqrt(fMouse3dCord[0]*fMouse3dCord[0] + fMouse3dCord[1]*fMouse3dCord[1] + fMouse3dCord[2]*fMouse3dCord[2]);

        fMouse3dCord[0] = fMouse3dCord[0] / fMouse3dCord_Length;
        fMouse3dCord[1] = fMouse3dCord[1] / fMouse3dCord_Length;
        fMouse3dCord[2] = fMouse3dCord[2] / fMouse3dCord_Length;//normalized
        //store curr mouse pos on hemisphere(trackball) which acts as first point of vector in mouse movement vector
        m_fMouse3dCord[0] = fMouse3dCord[0];
        m_fMouse3dCord[1] = fMouse3dCord[1];
        m_fMouse3dCord[2] = fMouse3dCord[2];
    }
}

void BasicGLPane::Mouse2dVertex_to_TrackballHemispherePoint(double fMouse3dCord[3],wxMouseEvent& event)
{
    //mouse 2d vertex to trackball hemisphere point.
//http://www.cse.ohio-state.edu/~crawfis/Graphics/VirtualTrackball.html
//http://viewport3d.com/trackball.htm
    double fCurrentXLoc,fCurrentYLoc,test;
    fCurrentXLoc = event.GetX();
    fCurrentYLoc = event.GetY();
    double fMouse3dCord_Length;
    //finding the point on hemisphere(radius=1) imagined to be projected on 2d screen
    fMouse3dCord[0] = (2.0*fCurrentXLoc - m_fWindWidth) / m_fWindWidth;
    fMouse3dCord[1] = (m_fWindHeight - 2.0*fCurrentYLoc) / m_fWindHeight;
    fMouse3dCord[2] = 0.0;
    fMouse3dCord[2] = sqrtf(1 - fMouse3dCord[0]*fMouse3dCord[0] + fMouse3dCord[1]*fMouse3dCord[1]);
    fMouse3dCord_Length = sqrt(fMouse3dCord[0]*fMouse3dCord[0] + fMouse3dCord[1]*fMouse3dCord[1] + fMouse3dCord[2]*fMouse3dCord[2]);
    //normalize trackball hemispherical point
    fMouse3dCord[0] = fMouse3dCord[0] / fMouse3dCord_Length;
    fMouse3dCord[1] = fMouse3dCord[1] / fMouse3dCord_Length;
    fMouse3dCord[2] = fMouse3dCord[2] / fMouse3dCord_Length;//normalized
    //if normalized properly,test == 1
    test = fMouse3dCord[0]*fMouse3dCord[0] + fMouse3dCord[1]*fMouse3dCord[1] + fMouse3dCord[2]*fMouse3dCord[2];
}

float BasicGLPane::TrackballVector_DirectionLength(double fMouse3dCord[3])
{
    //direction of the vector on hemisphere(from old point to new point)
    float fVecDirection[3];
    fVecDirection[0] = fMouse3dCord[0] - m_fMouse3dCord[0];
    fVecDirection[1] = fMouse3dCord[1] - m_fMouse3dCord[1];
    fVecDirection[2] = fMouse3dCord[2] - m_fMouse3dCord[2];
    float fVecDirectionLength = sqrt(fVecDirection[0]*fVecDirection[0] + fVecDirection[1]*fVecDirection[1] + fVecDirection[2]*fVecDirection[2]);
    return fVecDirectionLength;
}

float BasicGLPane::AngleAxisPair(double fMouse3dCord[3],double fRotAxis[3],float fVecDirectionLength)
{
    float fRotAngle;
    // Rotate about the axis that is perpendicular to the great circle connecting the mouse movements.
    //finds cross product of the vectors
    fRotAxis[0] = m_fMouse3dCord[1]*fMouse3dCord[2] - fMouse3dCord[1]*m_fMouse3dCord[2];
    fRotAxis[1] = m_fMouse3dCord[2]*fMouse3dCord[0] - m_fMouse3dCord[0]*fMouse3dCord[2];
    fRotAxis[2] = m_fMouse3dCord[0]*fMouse3dCord[1] - m_fMouse3dCord[1]*fMouse3dCord[0];
//    dRotAxisLength = sqrt(fRotAxis[0]*fRotAxis[0] + fRotAxis[1]*fRotAxis[1] + fRotAxis[2]*fRotAxis[2]);
//    fRotAxis[0] = fRotAxis[0]/dRotAxisLength;
//    fRotAxis[1] = fRotAxis[1]/dRotAxisLength;
//    fRotAxis[2] = fRotAxis[2]/dRotAxisLength;
    fRotAngle = fVecDirectionLength * 30;
    if(fRotAngle > 3)//if mouse is moved faster,rotation factor is decreased
        fRotAngle = fVecDirectionLength * 10;
    return fRotAngle;
}

void BasicGLPane::ConvertAngleAxisPair_to_Quarternion(double QuaternionMatrix[16],float fRotAngle,double fRotAxis[3])
{
//converting Angle-Axis pair to quarternions
//http://www.euclideanspace.com/maths/geometry/rotations/conversions/angleToQuaternion/index.htm
    double dQuatX,dQuatY,dQuatZ,dQuatW,q,a11,a12,a13,a14,a21,a22,a23,a24,a31,a32,a33,a34,a41,a42,a43,a44,test;
    if(bAssign == true)
    {
        mdQuatX = sin(fRotAngle/2) * fRotAxis[0];
        mdQuatY = sin(fRotAngle/2) * fRotAxis[1];
        mdQuatZ = sin(fRotAngle/2) * fRotAxis[2];
        mdQuatW = cos(fRotAngle/2);
        bAssign = false;
    }
    else
    {
        dQuatX = sin(fRotAngle/2) * fRotAxis[0];
        dQuatY = sin(fRotAngle/2) * fRotAxis[1];
        dQuatZ = sin(fRotAngle/2) * fRotAxis[2];
        dQuatW = cos(fRotAngle/2);
        //set current quaternion values for autorotation
        mdAutoQuatX = dQuatX;
        mdAutoQuatY = dQuatY;
        mdAutoQuatZ = dQuatZ;
        mdAutoQuatW = dQuatW;
        //quaternion multiplication
        mdQuatX = dQuatW*mdQuatX + dQuatX*mdQuatW + dQuatY*mdQuatZ + dQuatZ*mdQuatY;
        mdQuatY = dQuatW*mdQuatY + dQuatY*mdQuatW + dQuatZ*mdQuatX - dQuatX*mdQuatZ;
        mdQuatZ = dQuatW*mdQuatZ + dQuatZ*mdQuatW + dQuatX*mdQuatY - dQuatY*mdQuatX;
        mdQuatW = dQuatW*mdQuatW - dQuatX*mdQuatX - dQuatY*mdQuatY - dQuatZ*mdQuatZ;
    }
    q = sqrt(mdQuatW*mdQuatW + mdQuatX*mdQuatX  + mdQuatY*mdQuatY + mdQuatZ*mdQuatZ);//magnitude of a quaternion
    //normalizing a quaternion
    mdQuatX = mdQuatX/q;
    mdQuatY = mdQuatY/q;
    mdQuatZ = mdQuatZ/q;
    mdQuatW = mdQuatW/q;
//    unit quaternion has the following property w2 + x2 + y2 + z2=1
    test = dQuatW*dQuatW + dQuatX*dQuatX  + dQuatY*dQuatY + dQuatZ*dQuatZ;
    //http://en.wikipedia.org/wiki/Rotation_matrix#Quaternion
    a11 = 1.0 - 2.0*(mdQuatY*mdQuatY)-2.0*(mdQuatZ*mdQuatZ);
    a12 = 2*mdQuatX*mdQuatY - 2*mdQuatZ*mdQuatW;
    a13 = 2*mdQuatX*mdQuatZ + 2*mdQuatY*mdQuatW;
    a14 = 0.0;
    a21 = 2*mdQuatX*mdQuatY + 2*mdQuatZ*mdQuatW;
    a22 = 1.0 - 2.0*(mdQuatX*mdQuatX)-2.0*(mdQuatZ*mdQuatZ);
    a23 = 2*mdQuatY*mdQuatZ - 2*mdQuatX*mdQuatW;
    a24 = 0.0;
    a31 = 2*mdQuatX*mdQuatZ - 2*mdQuatY*mdQuatW;
    a32 = 2*mdQuatY*mdQuatZ + 2*mdQuatX*mdQuatW;
    a33 = 1.0 - 2.0*(mdQuatX*mdQuatX)-2.0*(mdQuatY*mdQuatY);
    a34 = 0.0;
    a41 = 0.0;
    a42 = 0.0;
    a43 = 0.0;
    a44 = 1.0;
    //matrix stored as column order in array
    QuaternionMatrix[0] = a11;
    QuaternionMatrix[1] = a21;
    QuaternionMatrix[2] = a31;
    QuaternionMatrix[3] = a41;
    QuaternionMatrix[4] = a12;
    QuaternionMatrix[5] = a22;
    QuaternionMatrix[6] = a32;
    QuaternionMatrix[7] = a42;
    QuaternionMatrix[8] = a13;
    QuaternionMatrix[9] = a23;
    QuaternionMatrix[10] = a33;
    QuaternionMatrix[11] = a43;
    QuaternionMatrix[12] = a14;
    QuaternionMatrix[13] = a24;
    QuaternionMatrix[14] = a34;
    QuaternionMatrix[15] = a44;
}

void BasicGLPane::RotateModelwithMouse(wxMouseEvent& event)
{
    wxClientDC dc(this); // only to be used in paint events. use wxClientDC to paint outside the paint event
    SetCurrent(*m_glContext);
    glMatrixMode( GL_MODELVIEW );
    float fRotAngle,fVecDirectionLength;
    double fMouse3dCord[3] = {0.0,0.0,0.0};
    //double fMouse3dCord[3] = {-170.0*0.005,-170.0*0.005,0.0*fScaleVal};
    double fRotAxis[3];
    Mouse2dVertex_to_TrackballHemispherePoint(fMouse3dCord,event);           //converts 2d mouse vertex to 3d hemispherical vertex
    fVecDirectionLength = TrackballVector_DirectionLength(fMouse3dCord);     //calculates trackball vector direction length
    fRotAngle = AngleAxisPair(fMouse3dCord,fRotAxis,fVecDirectionLength);    //calculates the angle-axis pair values
    ConvertAngleAxisPair_to_Quarternion(mdrgQuaternionMatrix,fRotAngle,fRotAxis);//converts angle-axis pair values into 4x4 quarternion matrix

    Refresh();
    //store curr mouse pos on hemisphere(trackball) which acts as first point of vector in mouse movement vector
    m_fMouse3dCord[0] = fMouse3dCord[0];
    m_fMouse3dCord[1] = fMouse3dCord[1];
    m_fMouse3dCord[2] = fMouse3dCord[2];
}

void BasicGLPane::PanModelwithMouse(wxMouseEvent& event)
{
    wxClientDC dc(this); // only to be used in paint events. use wxClientDC to paint outside the paint event
    SetCurrent(*m_glContext);
    double drgTranslationMatrix[16] = {1.0,0.0,0.0,0.0,0.0,1.0,0.0,0.0,0.0,0.0,1.0,0.0,0.0,0.0,0.0,1.0};
    float fCurrentXLoc = 0.0,fCurrentYLoc = 0.0,fMouseXPan_Distance = 0.0,fMouseYPan_Distance = 0.0;
    fCurrentXLoc = event.GetX();
    fCurrentYLoc = event.GetY();
    fMouseXPan_Distance = (fCurrentXLoc - m_fMouseX)/100;
    fMouseYPan_Distance = (fCurrentYLoc - m_fMouseY)/100;
    if(fMouseXPan_Distance != 0 || fMouseYPan_Distance != 0)
    {
        glPushMatrix();
        glLoadIdentity();
        glTranslatef(fMouseXPan_Distance,-fMouseYPan_Distance,0.0f);
        glGetDoublev(GL_MODELVIEW_MATRIX,drgTranslationMatrix);
        MatrixMultiplication_4x4(drgTranslationMatrix,mdrgTranslationMatrix);
        for(int iArrIter = 0; iArrIter < 16; iArrIter++)
            mdrgTranslationMatrix[iArrIter] = mdrgProductMatrix[iArrIter];
        glPopMatrix();
        Refresh();
    }
    m_fMouseX = fCurrentXLoc;
    m_fMouseY = fCurrentYLoc;
}

void BasicGLPane::ZoomIn()
{
    wxClientDC dc(this); // only to be used in paint events. use wxClientDC to paint outside the paint event
    SetCurrent(*m_glContext);
    //code for zoomin by altering projection matrix
//    int iWidth = getWidth();
//    int iHeight = getHeight();
//    glViewport(0, 0, (GLsizei)iWidth, (GLsizei)iHeight); // Set our viewport to the size of our window
//
//    glMatrixMode(GL_PROJECTION); // Switch to the projection matrix so that we can manipulate how our scene is viewed
//    glLoadIdentity(); // Reset the projection matrix to the identity matrix so that we don't get any artifacts (cleaning up)
//    Refer : http://www.opengl.org/sdk/docs/man/xhtml/glOrtho.xml
//    m_fZoomFactor = m_fZoomFactor - 0.2;
//    glOrtho(-1*m_fZoomFactor,1*m_fZoomFactor,-1*m_fZoomFactor,1*m_fZoomFactor,-3,3);//multiply the projection matrix with orthographic matrix//LRBTNF
//    glMatrixMode(GL_MODELVIEW); // Switch back to the model view matrix, so that we can start drawing shapes correctly
    //code for zoomin by altering modelview matrix
    double drgScaleViewMatrix[16] = {1.0,0.0,0.0,0.0,0.0,1.0,0.0,0.0,0.0,0.0,1.0,0.0,0.0,0.0,0.0,1.0};
    glPushMatrix();
    glLoadIdentity();
    glScalef(1.1,1.1,1.1);
    glGetDoublev(GL_MODELVIEW_MATRIX,drgScaleViewMatrix);
    MatrixMultiplication_4x4(drgScaleViewMatrix,mdrgScaleViewMatrix);
    for(int iArrIter = 0; iArrIter < 16; iArrIter++)
        mdrgScaleViewMatrix[iArrIter] = mdrgProductMatrix[iArrIter];
    glPopMatrix();
    Refresh();
}

void BasicGLPane::ZoomOut()
{
    wxClientDC dc(this); // only to be used in paint events. use wxClientDC to paint outside the paint event
    SetCurrent(*m_glContext);
    //code for zoomout by altering projection matrix
//    int iWidth = getWidth();
//    int iHeight = getHeight();
//
//    glViewport(0, 0, (GLsizei)iWidth, (GLsizei)iHeight); // Set our viewport to the size of our window
//
//    glMatrixMode(GL_PROJECTION); // Switch to the projection matrix so that we can manipulate how our scene is viewed
//    glLoadIdentity(); // Reset the projection matrix to the identity matrix so that we don't get any artifacts (cleaning up)
//    Refer : http://www.opengl.org/sdk/docs/man/xhtml/glOrtho.xml
//    m_fZoomFactor = m_fZoomFactor + 0.2;
//    glOrtho(-1*m_fZoomFactor,1*m_fZoomFactor,-1*m_fZoomFactor,1*m_fZoomFactor,-3,3);//multiply the projection matrix with orthographic matrix//LRBTNF
//    glMatrixMode(GL_MODELVIEW); // Switch back to the model view matrix, so that we can start drawing shapes correctly
    //code for zoomout by altering modelview matrix
    double drgScaleViewMatrix[16] = {1.0,0.0,0.0,0.0,0.0,1.0,0.0,0.0,0.0,0.0,1.0,0.0,0.0,0.0,0.0,1.0};
    glPushMatrix();
    glLoadIdentity();
    glScalef(0.83,0.83,0.83);
    glGetDoublev(GL_MODELVIEW_MATRIX,drgScaleViewMatrix);
    MatrixMultiplication_4x4(drgScaleViewMatrix,mdrgScaleViewMatrix);
    for(int iArrIter = 0; iArrIter < 16; iArrIter++)
        mdrgScaleViewMatrix[iArrIter] = mdrgProductMatrix[iArrIter];
    glPopMatrix();
    Refresh();
}

void BasicGLPane::mouseWheelMoved(wxMouseEvent& event)
{
    float fWheelRotationValue;
    fWheelRotationValue = event.GetWheelRotation();//returns the +/- mouse scroll float value
    if(fWheelRotationValue > 0)
        ZoomOut();
    else
        this->SetFocus();
    ZoomIn();
}

void BasicGLPane::mouseReleased(wxMouseEvent& event) {}
void BasicGLPane::leftClick(wxMouseEvent& event) {}
void BasicGLPane::rightClick(wxMouseEvent& event) {}
void BasicGLPane::MouseLeaveWindow(wxMouseEvent& event)
{

}
void BasicGLPane::MouseEnteredWindow(wxMouseEvent& event)
{
    if(mbSetFocus)//workaround, since EVT_MOUSEWHEEL event is not triggered after tree control gets the focus.
        this->SetFocus();
}
void BasicGLPane::keyPressed(wxKeyEvent& event)
{
    int iKey = event.GetKeyCode();
    int iSubcaseCount = oElementData.GetNbrofSubCases();
    if(iKey == WXK_RIGHT)
    {
        if(miSubcaseId < (iSubcaseCount-1))
            miSubcaseId++;
    }
    else if(iKey == WXK_LEFT)
    {
        if(miSubcaseId > 0)
            miSubcaseId--;
    }
    Refresh();
}
void BasicGLPane::keyReleased(wxKeyEvent& event) {}

//view methods
void BasicGLPane::ResetView()
{
    mbAutorotation = false;//autorotation is stopped for all views
    //identity matrix loaded in mdrgScaleViewMatrix
    mdrgScaleViewMatrix[0] = 1.0;
    mdrgScaleViewMatrix[1] = 0.0;
    mdrgScaleViewMatrix[2] = 0.0;
    mdrgScaleViewMatrix[3] = 0.0;
    mdrgScaleViewMatrix[4] = 0.0;
    mdrgScaleViewMatrix[5] = 1.0;
    mdrgScaleViewMatrix[6] = 0.0;
    mdrgScaleViewMatrix[7] = 0.0;
    mdrgScaleViewMatrix[8] = 0.0;
    mdrgScaleViewMatrix[9] = 0.0;
    mdrgScaleViewMatrix[10] = 1.0;
    mdrgScaleViewMatrix[11] = 0.0;
    mdrgScaleViewMatrix[12] = 0.0;
    mdrgScaleViewMatrix[13] = 0.0;
    mdrgScaleViewMatrix[14] = 0.0;
    mdrgScaleViewMatrix[15] = 1.0;
    //identity loaded in mdrgTranslationMatrix
    mdrgTranslationMatrix[0] = 1.0;
    mdrgTranslationMatrix[1] = 0.0;
    mdrgTranslationMatrix[2] = 0.0;
    mdrgTranslationMatrix[3] = 0.0;
    mdrgTranslationMatrix[4] = 0.0;
    mdrgTranslationMatrix[5] = 1.0;
    mdrgTranslationMatrix[6] = 0.0;
    mdrgTranslationMatrix[7] = 0.0;
    mdrgTranslationMatrix[8] = 0.0;
    mdrgTranslationMatrix[9] = 0.0;
    mdrgTranslationMatrix[10] = 1.0;
    mdrgTranslationMatrix[11] = 0.0;
    mdrgTranslationMatrix[12] = 0.0;
    mdrgTranslationMatrix[13] = 0.0;
    mdrgTranslationMatrix[14] = 0.0;
    mdrgTranslationMatrix[15] = 1.0;
    //identity loaded in mdrgQuaternionMatrix
    mdrgQuaternionMatrix[0] = 1.0;
    mdrgQuaternionMatrix[1] = 0.0;
    mdrgQuaternionMatrix[2] = 0.0;
    mdrgQuaternionMatrix[3] = 0.0;
    mdrgQuaternionMatrix[4] = 0.0;
    mdrgQuaternionMatrix[5] = 1.0;
    mdrgQuaternionMatrix[6] = 0.0;
    mdrgQuaternionMatrix[7] = 0.0;
    mdrgQuaternionMatrix[8] = 0.0;
    mdrgQuaternionMatrix[9] = 0.0;
    mdrgQuaternionMatrix[10] = 1.0;
    mdrgQuaternionMatrix[11] = 0.0;
    mdrgQuaternionMatrix[12] = 0.0;
    mdrgQuaternionMatrix[13] = 0.0;
    mdrgQuaternionMatrix[14] = 0.0;
    mdrgQuaternionMatrix[15] = 1.0;
    //identity values loaded in quaternion elements
    mdQuatX = 0.0;
    mdQuatY = 0.0;
    mdQuatZ = 0.0;
    mdQuatW = 1.0;
    bAssign = true;//assumes as the begining for rotation
    //identity values loaded in autorotation quaternion elements
    mdAutoQuatX = 0.0;
    mdAutoQuatY = 0.0;
    mdAutoQuatZ = 0.0;
    mdAutoQuatW = 1.0;
    Update();
}

void BasicGLPane::FitView()
{
    mbAutorotation = false;
    //identity matrix loaded in mdrgScaleViewMatrix
    mdrgScaleViewMatrix[0] = 1.0;
    mdrgScaleViewMatrix[1] = 0.0;
    mdrgScaleViewMatrix[2] = 0.0;
    mdrgScaleViewMatrix[3] = 0.0;
    mdrgScaleViewMatrix[4] = 0.0;
    mdrgScaleViewMatrix[5] = 1.0;
    mdrgScaleViewMatrix[6] = 0.0;
    mdrgScaleViewMatrix[7] = 0.0;
    mdrgScaleViewMatrix[8] = 0.0;
    mdrgScaleViewMatrix[9] = 0.0;
    mdrgScaleViewMatrix[10] = 1.0;
    mdrgScaleViewMatrix[11] = 0.0;
    mdrgScaleViewMatrix[12] = 0.0;
    mdrgScaleViewMatrix[13] = 0.0;
    mdrgScaleViewMatrix[14] = 0.0;
    mdrgScaleViewMatrix[15] = 1.0;
    //identity loaded in mdrgTranslationMatrix
    mdrgTranslationMatrix[0] = 1.0;
    mdrgTranslationMatrix[1] = 0.0;
    mdrgTranslationMatrix[2] = 0.0;
    mdrgTranslationMatrix[3] = 0.0;
    mdrgTranslationMatrix[4] = 0.0;
    mdrgTranslationMatrix[5] = 1.0;
    mdrgTranslationMatrix[6] = 0.0;
    mdrgTranslationMatrix[7] = 0.0;
    mdrgTranslationMatrix[8] = 0.0;
    mdrgTranslationMatrix[9] = 0.0;
    mdrgTranslationMatrix[10] = 1.0;
    mdrgTranslationMatrix[11] = 0.0;
    mdrgTranslationMatrix[12] = 0.0;
    mdrgTranslationMatrix[13] = 0.0;
    mdrgTranslationMatrix[14] = 0.0;
    mdrgTranslationMatrix[15] = 1.0;
    //identity values loaded in autorotation quaternion elements
    mdAutoQuatX = 0.0;
    mdAutoQuatY = 0.0;
    mdAutoQuatZ = 0.0;
    mdAutoQuatW = 1.0;
    Refresh();
}

void BasicGLPane::LeftView()
{
    mbAutorotation = false;
    //identity matrix loaded in mdrgScaleViewMatrix
    mdrgScaleViewMatrix[0] = 1.0;
    mdrgScaleViewMatrix[1] = 0.0;
    mdrgScaleViewMatrix[2] = 0.0;
    mdrgScaleViewMatrix[3] = 0.0;
    mdrgScaleViewMatrix[4] = 0.0;
    mdrgScaleViewMatrix[5] = 1.0;
    mdrgScaleViewMatrix[6] = 0.0;
    mdrgScaleViewMatrix[7] = 0.0;
    mdrgScaleViewMatrix[8] = 0.0;
    mdrgScaleViewMatrix[9] = 0.0;
    mdrgScaleViewMatrix[10] = 1.0;
    mdrgScaleViewMatrix[11] = 0.0;
    mdrgScaleViewMatrix[12] = 0.0;
    mdrgScaleViewMatrix[13] = 0.0;
    mdrgScaleViewMatrix[14] = 0.0;
    mdrgScaleViewMatrix[15] = 1.0;
    //identity loaded in mdrgTranslationMatrix
    mdrgTranslationMatrix[0] = 1.0;
    mdrgTranslationMatrix[1] = 0.0;
    mdrgTranslationMatrix[2] = 0.0;
    mdrgTranslationMatrix[3] = 0.0;
    mdrgTranslationMatrix[4] = 0.0;
    mdrgTranslationMatrix[5] = 1.0;
    mdrgTranslationMatrix[6] = 0.0;
    mdrgTranslationMatrix[7] = 0.0;
    mdrgTranslationMatrix[8] = 0.0;
    mdrgTranslationMatrix[9] = 0.0;
    mdrgTranslationMatrix[10] = 1.0;
    mdrgTranslationMatrix[11] = 0.0;
    mdrgTranslationMatrix[12] = 0.0;
    mdrgTranslationMatrix[13] = 0.0;
    mdrgTranslationMatrix[14] = 0.0;
    mdrgTranslationMatrix[15] = 1.0;
    //rotation matrix values for leftview
    mdrgQuaternionMatrix[0] = 0.0;
    mdrgQuaternionMatrix[1] = 0.0;
    mdrgQuaternionMatrix[2] = -1.0;
    mdrgQuaternionMatrix[3] = 0.0;
    mdrgQuaternionMatrix[4] = 0.0;
    mdrgQuaternionMatrix[5] = 1.0;
    mdrgQuaternionMatrix[6] = 0.0;
    mdrgQuaternionMatrix[7] = 0.0;
    mdrgQuaternionMatrix[8] = 1.0;
    mdrgQuaternionMatrix[9] = 0.0;
    mdrgQuaternionMatrix[10] = 0.0;
    mdrgQuaternionMatrix[11] = 0.0;
    mdrgQuaternionMatrix[12] = 0.0;
    mdrgQuaternionMatrix[13] = 0.0;
    mdrgQuaternionMatrix[14] = 0.0;
    mdrgQuaternionMatrix[15] = 1.0;
    //identity values loaded in quaternion elements
    mdQuatX = 0.0;
    mdQuatY = 0.7071067811865475;
    mdQuatZ = 0.0;
    mdQuatW = 0.7071067811865476;
    bAssign = false;//helps if view is called immediately after reset
    //identity values loaded in autorotation quaternion elements
    mdAutoQuatX = 0.0;
    mdAutoQuatY = 0.0;
    mdAutoQuatZ = 0.0;
    mdAutoQuatW = 1.0;
    Update();
}

void BasicGLPane::RightView()
{
    mbAutorotation = false;
    //identity matrix loaded in mdrgScaleViewMatrix
    mdrgScaleViewMatrix[0] = 1.0;
    mdrgScaleViewMatrix[1] = 0.0;
    mdrgScaleViewMatrix[2] = 0.0;
    mdrgScaleViewMatrix[3] = 0.0;
    mdrgScaleViewMatrix[4] = 0.0;
    mdrgScaleViewMatrix[5] = 1.0;
    mdrgScaleViewMatrix[6] = 0.0;
    mdrgScaleViewMatrix[7] = 0.0;
    mdrgScaleViewMatrix[8] = 0.0;
    mdrgScaleViewMatrix[9] = 0.0;
    mdrgScaleViewMatrix[10] = 1.0;
    mdrgScaleViewMatrix[11] = 0.0;
    mdrgScaleViewMatrix[12] = 0.0;
    mdrgScaleViewMatrix[13] = 0.0;
    mdrgScaleViewMatrix[14] = 0.0;
    mdrgScaleViewMatrix[15] = 1.0;
    //identity loaded in mdrgTranslationMatrix
    mdrgTranslationMatrix[0] = 1.0;
    mdrgTranslationMatrix[1] = 0.0;
    mdrgTranslationMatrix[2] = 0.0;
    mdrgTranslationMatrix[3] = 0.0;
    mdrgTranslationMatrix[4] = 0.0;
    mdrgTranslationMatrix[5] = 1.0;
    mdrgTranslationMatrix[6] = 0.0;
    mdrgTranslationMatrix[7] = 0.0;
    mdrgTranslationMatrix[8] = 0.0;
    mdrgTranslationMatrix[9] = 0.0;
    mdrgTranslationMatrix[10] = 1.0;
    mdrgTranslationMatrix[11] = 0.0;
    mdrgTranslationMatrix[12] = 0.0;
    mdrgTranslationMatrix[13] = 0.0;
    mdrgTranslationMatrix[14] = 0.0;
    mdrgTranslationMatrix[15] = 1.0;
    //rotation matrix values for rightview
    mdrgQuaternionMatrix[0] = 0.0;
    mdrgQuaternionMatrix[1] = 0.0;
    mdrgQuaternionMatrix[2] = 1.0;
    mdrgQuaternionMatrix[3] = 0.0;
    mdrgQuaternionMatrix[4] = 0.0;
    mdrgQuaternionMatrix[5] = 1.0;
    mdrgQuaternionMatrix[6] = 0.0;
    mdrgQuaternionMatrix[7] = 0.0;
    mdrgQuaternionMatrix[8] = -1.0;
    mdrgQuaternionMatrix[9] = 0.0;
    mdrgQuaternionMatrix[10] = 0.0;
    mdrgQuaternionMatrix[11] = 0.0;
    mdrgQuaternionMatrix[12] = 0.0;
    mdrgQuaternionMatrix[13] = 0.0;
    mdrgQuaternionMatrix[14] = 0.0;
    mdrgQuaternionMatrix[15] = 1.0;
    //right view values loaded in quaternion elements
    mdQuatX = 0.0;
    mdQuatY = -0.7071067811865475;
    mdQuatZ = 0.0;
    mdQuatW = 0.7071067811865476;
    bAssign = false;
    //identity values loaded in autorotation quaternion elements
    mdAutoQuatX = 0.0;
    mdAutoQuatY = 0.0;
    mdAutoQuatZ = 0.0;
    mdAutoQuatW = 1.0;
    Update();
}

void BasicGLPane::TopView()
{
    mbAutorotation = false;
    //identity matrix loaded in mdrgScaleViewMatrix
    mdrgScaleViewMatrix[0] = 1.0;
    mdrgScaleViewMatrix[1] = 0.0;
    mdrgScaleViewMatrix[2] = 0.0;
    mdrgScaleViewMatrix[3] = 0.0;
    mdrgScaleViewMatrix[4] = 0.0;
    mdrgScaleViewMatrix[5] = 1.0;
    mdrgScaleViewMatrix[6] = 0.0;
    mdrgScaleViewMatrix[7] = 0.0;
    mdrgScaleViewMatrix[8] = 0.0;
    mdrgScaleViewMatrix[9] = 0.0;
    mdrgScaleViewMatrix[10] = 1.0;
    mdrgScaleViewMatrix[11] = 0.0;
    mdrgScaleViewMatrix[12] = 0.0;
    mdrgScaleViewMatrix[13] = 0.0;
    mdrgScaleViewMatrix[14] = 0.0;
    mdrgScaleViewMatrix[15] = 1.0;
    //identity loaded in mdrgTranslationMatrix
    mdrgTranslationMatrix[0] = 1.0;
    mdrgTranslationMatrix[1] = 0.0;
    mdrgTranslationMatrix[2] = 0.0;
    mdrgTranslationMatrix[3] = 0.0;
    mdrgTranslationMatrix[4] = 0.0;
    mdrgTranslationMatrix[5] = 1.0;
    mdrgTranslationMatrix[6] = 0.0;
    mdrgTranslationMatrix[7] = 0.0;
    mdrgTranslationMatrix[8] = 0.0;
    mdrgTranslationMatrix[9] = 0.0;
    mdrgTranslationMatrix[10] = 1.0;
    mdrgTranslationMatrix[11] = 0.0;
    mdrgTranslationMatrix[12] = 0.0;
    mdrgTranslationMatrix[13] = 0.0;
    mdrgTranslationMatrix[14] = 0.0;
    mdrgTranslationMatrix[15] = 1.0;
    //rotation matrix values for Topview
    mdrgQuaternionMatrix[0] = 1.0;
    mdrgQuaternionMatrix[1] = 0.0;
    mdrgQuaternionMatrix[2] = 0.0;
    mdrgQuaternionMatrix[3] = 0.0;
    mdrgQuaternionMatrix[4] = 0.0;
    mdrgQuaternionMatrix[5] = 0.0;
    mdrgQuaternionMatrix[6] = 1.0;
    mdrgQuaternionMatrix[7] = 0.0;
    mdrgQuaternionMatrix[8] = 0.0;
    mdrgQuaternionMatrix[9] = -1.0;
    mdrgQuaternionMatrix[10] = 0.0;
    mdrgQuaternionMatrix[11] = 0.0;
    mdrgQuaternionMatrix[12] = 0.0;
    mdrgQuaternionMatrix[13] = 0.0;
    mdrgQuaternionMatrix[14] = 0.0;
    mdrgQuaternionMatrix[15] = 1.0;
    //top view values loaded in quaternion elements
    mdQuatX = 0.7071067811865476;
    mdQuatY = 0.0;
    mdQuatZ = 0.0;
    mdQuatW = 0.7071067811865475;
    bAssign = false;
    //identity values loaded in autorotation quaternion elements
    mdAutoQuatX = 0.0;
    mdAutoQuatY = 0.0;
    mdAutoQuatZ = 0.0;
    mdAutoQuatW = 1.0;
    Update();
}

void BasicGLPane::BottomView()
{
    mbAutorotation = false;
    //identity matrix loaded in mdrgScaleViewMatrix
    mdrgScaleViewMatrix[0] = 1.0;
    mdrgScaleViewMatrix[1] = 0.0;
    mdrgScaleViewMatrix[2] = 0.0;
    mdrgScaleViewMatrix[3] = 0.0;
    mdrgScaleViewMatrix[4] = 0.0;
    mdrgScaleViewMatrix[5] = 1.0;
    mdrgScaleViewMatrix[6] = 0.0;
    mdrgScaleViewMatrix[7] = 0.0;
    mdrgScaleViewMatrix[8] = 0.0;
    mdrgScaleViewMatrix[9] = 0.0;
    mdrgScaleViewMatrix[10] = 1.0;
    mdrgScaleViewMatrix[11] = 0.0;
    mdrgScaleViewMatrix[12] = 0.0;
    mdrgScaleViewMatrix[13] = 0.0;
    mdrgScaleViewMatrix[14] = 0.0;
    mdrgScaleViewMatrix[15] = 1.0;
    //identity loaded in mdrgTranslationMatrix
    mdrgTranslationMatrix[0] = 1.0;
    mdrgTranslationMatrix[1] = 0.0;
    mdrgTranslationMatrix[2] = 0.0;
    mdrgTranslationMatrix[3] = 0.0;
    mdrgTranslationMatrix[4] = 0.0;
    mdrgTranslationMatrix[5] = 1.0;
    mdrgTranslationMatrix[6] = 0.0;
    mdrgTranslationMatrix[7] = 0.0;
    mdrgTranslationMatrix[8] = 0.0;
    mdrgTranslationMatrix[9] = 0.0;
    mdrgTranslationMatrix[10] = 1.0;
    mdrgTranslationMatrix[11] = 0.0;
    mdrgTranslationMatrix[12] = 0.0;
    mdrgTranslationMatrix[13] = 0.0;
    mdrgTranslationMatrix[14] = 0.0;
    mdrgTranslationMatrix[15] = 1.0;
    //rotation matrix values for bottomview
    mdrgQuaternionMatrix[0] = 1.0;
    mdrgQuaternionMatrix[1] = 0.0;
    mdrgQuaternionMatrix[2] = 0.0;
    mdrgQuaternionMatrix[3] = 0.0;
    mdrgQuaternionMatrix[4] = 0.0;
    mdrgQuaternionMatrix[5] = 0.0;
    mdrgQuaternionMatrix[6] = -1.0;
    mdrgQuaternionMatrix[7] = 0.0;
    mdrgQuaternionMatrix[8] = 0.0;
    mdrgQuaternionMatrix[9] = 1.0;
    mdrgQuaternionMatrix[10] = 0.0;
    mdrgQuaternionMatrix[11] = 0.0;
    mdrgQuaternionMatrix[12] = 0.0;
    mdrgQuaternionMatrix[13] = 0.0;
    mdrgQuaternionMatrix[14] = 0.0;
    mdrgQuaternionMatrix[15] = 1.0;
    //bottom view values loaded in quaternion elements
    mdQuatX = 0.7071067811865476;
    mdQuatY = 0.0;
    mdQuatZ = 0.0;
    mdQuatW = -0.7071067811865475;
    bAssign = false;
    //identity values loaded in autorotation quaternion elements
    mdAutoQuatX = 0.0;
    mdAutoQuatY = 0.0;
    mdAutoQuatZ = 0.0;
    mdAutoQuatW = 1.0;
    Update();
}

void BasicGLPane::FrontView()
{
    mbAutorotation = false;
    //identity matrix loaded in mdrgScaleViewMatrix
    mdrgScaleViewMatrix[0] = 1.0;
    mdrgScaleViewMatrix[1] = 0.0;
    mdrgScaleViewMatrix[2] = 0.0;
    mdrgScaleViewMatrix[3] = 0.0;
    mdrgScaleViewMatrix[4] = 0.0;
    mdrgScaleViewMatrix[5] = 1.0;
    mdrgScaleViewMatrix[6] = 0.0;
    mdrgScaleViewMatrix[7] = 0.0;
    mdrgScaleViewMatrix[8] = 0.0;
    mdrgScaleViewMatrix[9] = 0.0;
    mdrgScaleViewMatrix[10] = 1.0;
    mdrgScaleViewMatrix[11] = 0.0;
    mdrgScaleViewMatrix[12] = 0.0;
    mdrgScaleViewMatrix[13] = 0.0;
    mdrgScaleViewMatrix[14] = 0.0;
    mdrgScaleViewMatrix[15] = 1.0;
    //identity loaded in mdrgTranslationMatrix
    mdrgTranslationMatrix[0] = 1.0;
    mdrgTranslationMatrix[1] = 0.0;
    mdrgTranslationMatrix[2] = 0.0;
    mdrgTranslationMatrix[3] = 0.0;
    mdrgTranslationMatrix[4] = 0.0;
    mdrgTranslationMatrix[5] = 1.0;
    mdrgTranslationMatrix[6] = 0.0;
    mdrgTranslationMatrix[7] = 0.0;
    mdrgTranslationMatrix[8] = 0.0;
    mdrgTranslationMatrix[9] = 0.0;
    mdrgTranslationMatrix[10] = 1.0;
    mdrgTranslationMatrix[11] = 0.0;
    mdrgTranslationMatrix[12] = 0.0;
    mdrgTranslationMatrix[13] = 0.0;
    mdrgTranslationMatrix[14] = 0.0;
    mdrgTranslationMatrix[15] = 1.0;
    //rotation matrix values for frontview
    mdrgQuaternionMatrix[0] = 1.0;
    mdrgQuaternionMatrix[1] = 0.0;
    mdrgQuaternionMatrix[2] = 0.0;
    mdrgQuaternionMatrix[3] = 0.0;
    mdrgQuaternionMatrix[4] = 0.0;
    mdrgQuaternionMatrix[5] = 1.0;
    mdrgQuaternionMatrix[6] = 0.0;
    mdrgQuaternionMatrix[7] = 0.0;
    mdrgQuaternionMatrix[8] = 0.0;
    mdrgQuaternionMatrix[9] = 0.0;
    mdrgQuaternionMatrix[10] = 1.0;
    mdrgQuaternionMatrix[11] = 0.0;
    mdrgQuaternionMatrix[12] = 0.0;
    mdrgQuaternionMatrix[13] = 0.0;
    mdrgQuaternionMatrix[14] = 0.0;
    mdrgQuaternionMatrix[15] = 1.0;
    //front view values loaded in quaternion elements
    mdQuatX = 0.0;
    mdQuatY = 0.0;
    mdQuatZ = 0.0;
    mdQuatW = 1.0;
    //identity values loaded in autorotation quaternion elements
    mdAutoQuatX = 0.0;
    mdAutoQuatY = 0.0;
    mdAutoQuatZ = 0.0;
    mdAutoQuatW = 1.0;
    Update();
}

void BasicGLPane:: BackView()
{
    mbAutorotation = false;
    //identity matrix loaded in mdrgScaleViewMatrix
    mdrgScaleViewMatrix[0] = 1.0;
    mdrgScaleViewMatrix[1] = 0.0;
    mdrgScaleViewMatrix[2] = 0.0;
    mdrgScaleViewMatrix[3] = 0.0;
    mdrgScaleViewMatrix[4] = 0.0;
    mdrgScaleViewMatrix[5] = 1.0;
    mdrgScaleViewMatrix[6] = 0.0;
    mdrgScaleViewMatrix[7] = 0.0;
    mdrgScaleViewMatrix[8] = 0.0;
    mdrgScaleViewMatrix[9] = 0.0;
    mdrgScaleViewMatrix[10] = 1.0;
    mdrgScaleViewMatrix[11] = 0.0;
    mdrgScaleViewMatrix[12] = 0.0;
    mdrgScaleViewMatrix[13] = 0.0;
    mdrgScaleViewMatrix[14] = 0.0;
    mdrgScaleViewMatrix[15] = 1.0;
    //identity loaded in mdrgTranslationMatrix
    mdrgTranslationMatrix[0] = 1.0;
    mdrgTranslationMatrix[1] = 0.0;
    mdrgTranslationMatrix[2] = 0.0;
    mdrgTranslationMatrix[3] = 0.0;
    mdrgTranslationMatrix[4] = 0.0;
    mdrgTranslationMatrix[5] = 1.0;
    mdrgTranslationMatrix[6] = 0.0;
    mdrgTranslationMatrix[7] = 0.0;
    mdrgTranslationMatrix[8] = 0.0;
    mdrgTranslationMatrix[9] = 0.0;
    mdrgTranslationMatrix[10] = 1.0;
    mdrgTranslationMatrix[11] = 0.0;
    mdrgTranslationMatrix[12] = 0.0;
    mdrgTranslationMatrix[13] = 0.0;
    mdrgTranslationMatrix[14] = 0.0;
    mdrgTranslationMatrix[15] = 1.0;
    //rotation matrix values for backview
    mdrgQuaternionMatrix[0] = -1.0;
    mdrgQuaternionMatrix[1] = 0.0;
    mdrgQuaternionMatrix[2] = 0.0;
    mdrgQuaternionMatrix[3] = 0.0;
    mdrgQuaternionMatrix[4] = 0.0;
    mdrgQuaternionMatrix[5] = 1.0;
    mdrgQuaternionMatrix[6] = 0.0;
    mdrgQuaternionMatrix[7] = 0.0;
    mdrgQuaternionMatrix[8] = 0.0;
    mdrgQuaternionMatrix[9] = 0.0;
    mdrgQuaternionMatrix[10] = -1.0;
    mdrgQuaternionMatrix[11] = 0.0;
    mdrgQuaternionMatrix[12] = 0.0;
    mdrgQuaternionMatrix[13] = 0.0;
    mdrgQuaternionMatrix[14] = 0.0;
    mdrgQuaternionMatrix[15] = 1.0;
    //back view values loaded in quaternion elements
    mdQuatX = 0.0;
    mdQuatY = -0.9999999979236466;
    mdQuatZ = 0.0;
    mdQuatW = 0.00008726646233757571;
    bAssign = false;
    //identity values loaded in autorotation quaternion elements
    mdAutoQuatX = 0.0;
    mdAutoQuatY = 0.0;
    mdAutoQuatZ = 0.0;
    mdAutoQuatW = 1.0;
    Update();
}

void BasicGLPane::ExportNodeCoordinates()
{
    ExportData::ExportNodeCoordinates(oNodeData);
}

void BasicGLPane::ExportElementConnectivity()
{
    ExportData::ExportElementConnectivity(oElementData);
}

void BasicGLPane::ExportNodalOutputDisplacement()
{
    ExportData::ExportNodalOutputDisplacement(oNodeData);
}

void BasicGLPane::ExportElementStressStrainResults()
{
    ExportData::ExportElementStressStrainResults(oElementData);
}

void BasicGLPane::ExportNodalAppliedLoads()
{
    ExportData::ExportNodalAppliedLoads(oNodeData);
}

void BasicGLPane::ExportOutputStrainEnergy()
{
    ExportData::ExportOutputStrainEnergy(oElementData);
}

void BasicGLPane::ExportOutputKineticEnergy()
{
    ExportData::ExportOutputKineticEnergy(oElementData);
}

void BasicGLPane::ExportOutputEnergyLoss()
{
    ExportData::ExportOutputEnergyLoss(oElementData);
}

void BasicGLPane::ExportConstraintForces()
{
    ExportData::ExportConstraintForces(oNodeData);
}

void BasicGLPane::ExportAll()
{
    ExportData::ExportAllDatatoTextFile(oNodeData,oElementData);
}

void BasicGLPane::ExportGraph_StrsStrnXX()
{
    ExportData::ExportGraph_StrsStrnXX(oElementData);
}

void BasicGLPane::ExportGraph_StrsStrnYY()
{
    ExportData::ExportGraph_StrsStrnYY(oElementData);
}

void BasicGLPane::ExportGraph_StrsStrnZZ()
{
    ExportData::ExportGraph_StrsStrnZZ(oElementData);
}

void BasicGLPane::ExportGraph_StrsStrnXY()
{
    ExportData::ExportGraph_StrsStrnXY(oElementData);
}

void BasicGLPane::ExportGraph_StrsStrnYZ()
{
    ExportData::ExportGraph_StrsStrnYZ(oElementData);
}

void BasicGLPane::ExportGraph_StrsStrnXZ()
{
    ExportData::ExportGraph_StrsStrnXZ(oElementData);
}

void BasicGLPane::ExportGraph_StrsStrnShearAngle()
{
    ExportData::ExportGraph_StrsStrnShearAngle(oElementData);
}

void BasicGLPane::ExportGraph_StrsStrn1stPrincipal()
{
    ExportData::ExportGraph_StrsStrn1stPrincipal(oElementData);
}

void BasicGLPane::ExportGraph_StrsStrn2ndPrincipal()
{
    ExportData::ExportGraph_StrsStrn2ndPrincipal(oElementData);
}

void BasicGLPane::ExportGraph_StrsStrn3rdPrincipal()
{
    ExportData::ExportGraph_StrsStrn3rdPrincipal(oElementData);
}

void BasicGLPane::ExportGraph_StrsStrnMajorPrincipal()
{
    ExportData::ExportGraph_StrsStrnMajorPrincipal(oElementData);
}

void BasicGLPane::ExportGraph_StrsStrnMinorPrincipal()
{
    ExportData::ExportGraph_StrsStrnMinorPrincipal(oElementData);
}

void BasicGLPane::ExportGraph_StrsStrnVmises()
{
    ExportData::ExportGraph_StrsStrnVmises(oElementData);
}

void BasicGLPane::SetTreeViewData()
{
    if(pProgressBar)
    {
        delete pProgressBar;
        pProgressBar = NULL;
    }
    CTreeView::SetTreeComponents(m_wxstrFileName,oElementData);
}

void BasicGLPane::CanSetFocus(bool bSetFocus)
{
    mbSetFocus = bSetFocus;
}

float* BasicGLPane::ComputeNormal(float *fVertex1, float *fVertex2, float *fVertex3)
{
    //finds cross product of the vectors
    float fVector1[3], fVector2[3], fNormal[3], fNormalized[3];
    fVector1[0]=fVertex2[0]-fVertex1[0];
    fVector1[1]=fVertex2[1]-fVertex1[1];
    fVector1[2]=fVertex2[2]-fVertex1[2];
    fVector2[0]=fVertex3[0]-fVertex1[0];
    fVector2[1]=fVertex3[1]-fVertex1[1];
    fVector2[2]=fVertex3[2]-fVertex1[2];
    fNormal[0]=fVector1[1]*fVector2[2] - fVector1[2]*fVector2[1];
    fNormal[1]=fVector1[2]*fVector2[0] - fVector1[0]*fVector2[2];
    fNormal[2]=fVector1[0]*fVector2[1] - fVector1[1]*fVector2[0];

    //normalising is needed incase of using glscale which shortens the normals
    //normalising is dividing the vector coordinates by its own length.
    //normalised vector has length=1, which is its unit vector.
    float fVectorLength = sqrt(pow(fNormal[0],2)+pow(fNormal[1],2)+pow(fNormal[2],2));
    fNormalized[0] = fNormal[0]/(fVectorLength);
    fNormalized[1] = fNormal[1]/(fVectorLength);
    fNormalized[2] = fNormal[2]/(fVectorLength);
    float testforvalue1 = sqrt(pow(fNormalized[0],2)+pow(fNormalized[1],2)+pow(fNormalized[2],2));
    return fNormalized;
}

void BasicGLPane::DrawLegend()
{
    wxClientDC dc(this); // only to be used in paint events. use wxClientDC to paint outside the paint event
    SetCurrent(*m_glContext);
    if(m_iBoundBox == 1)
    {
        glPushMatrix();
        glLoadIdentity();
        //Load camera position, so that we can start drawing shapes correctly
        gluLookAt(0.0,0.0,0.0+1.0,  //look from camera
                  0.0,0.0,0.0,     //look at origin //centre of the screen
                  0.0f,1.0f,0.0f);//YCoord axis up vector
        int iWidth = 500/8.0;
        int iHeight = 500/1.3;
        glViewport(800, 130, (GLsizei)iWidth, (GLsizei)iHeight); // Set our viewport to the size of our window
        // and multiply matrices to glmodelview's top stack and render

        glDisable(GL_LIGHTING);//for texts
        glColor3f(1.0f, 1.0f, 1.0f);
        //-----subcase-----//
        char cSubcase[15] = "Subcase: ";
        char cSubcaseId[6];
        snprintf(cSubcaseId, sizeof(cSubcaseId), "%d", miSubcaseId+1);//upto 1 lakh subcases
        strcat(cSubcase, cSubcaseId);
        glRasterPos2f(-0.3,0.75);
        for(int i = 0; i < sizeof(cSubcase); i++)
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12,cSubcase[i]);

        //-----get legend min max----//
        float fLegendMin, fLegendmax;
        if (mstrResultId == "stressx")
        {
            fLegendMin = oElementData.fStressX_min[miSubcaseId];
            fLegendmax = oElementData.fStressX_max[miSubcaseId];
        }
        else if (mstrResultId == "stressy")
        {
            fLegendMin = oElementData.fStressY_min[miSubcaseId];
            fLegendmax = oElementData.fStressY_max[miSubcaseId];
        }
        else if (mstrResultId == "stressz")
        {
            fLegendMin = oElementData.fStressZ_min[miSubcaseId];
            fLegendmax = oElementData.fStressZ_max[miSubcaseId];
        }
        else if (mstrResultId == "stressxy")
        {
            fLegendMin = oElementData.fStressXY_min[miSubcaseId];
            fLegendmax = oElementData.fStressXY_max[miSubcaseId];
        }
        else if (mstrResultId == "stressyz")
        {
            fLegendMin = oElementData.fStressYZ_min[miSubcaseId];
            fLegendmax = oElementData.fStressYZ_max[miSubcaseId];
        }
        else if (mstrResultId == "stressxz")
        {
            fLegendMin = oElementData.fStressXZ_min[miSubcaseId];
            fLegendmax = oElementData.fStressXZ_max[miSubcaseId];
        }
        else
        {
            fLegendMin = 0.0;
            fLegendmax = 0.0;
        }

        //-----display legend min value-----//
        glRasterPos2f(0.0,-1.0);
        char cLegendMinValue[7];
        snprintf(cLegendMinValue, sizeof(cLegendMinValue), "%f", fLegendMin);
        for(int i = 0; i < sizeof(cLegendMinValue); i++)
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12,cLegendMinValue[i]);

        //-----display legend max value-----//
        glRasterPos2f(0.0,0.65);
        char cLegendMaxValue[7];
        snprintf(cLegendMaxValue, sizeof(cLegendMaxValue), "%f", fLegendmax);
        for(int i = 0; i < sizeof(cLegendMaxValue); i++)
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12,cLegendMaxValue[i]);
        //--------

        float fVertex1[3], fVertex2[3], fVertex3[3];
        float *fNormal;

        glEnable(GL_LIGHTING);
        glBegin(GL_QUADS);
        //R region
        fVertex1[0] = 0.0, fVertex1[1] = 0.1, fVertex1[2] = 0.0;
        fVertex2[0] = 1.0, fVertex2[1] = 0.1, fVertex2[2] = 0.0;
        fVertex3[0] = 1.0, fVertex3[1] = 0.6, fVertex3[2] = 0.0;
        fNormal = ComputeNormal(fVertex1, fVertex2, fVertex3);

        glNormal3f(fNormal[0], fNormal[1], fNormal[2]);
        glColor3f(0.1f, 0.0f, 1.0f);//1-2-3-4 anti-clock
        glVertex3f(0.0,0.1,0.0);
        glVertex3f(1.0,0.1,0.0);
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3f(1.0,0.6,0.0);
        glVertex3f(0.0,0.6,0.0);
        glEnd();

        glBegin(GL_QUADS);
        //B region
        fVertex1[0] = 0.0, fVertex1[1] = -0.4,  fVertex1[2] = 0.0;
        fVertex2[0] = 1.0, fVertex2[1] = -0.4,  fVertex2[2] = 0.0;
        fVertex3[0] = 1.0, fVertex3[1] = 0.1, fVertex3[2] = 0.0;
        fNormal = ComputeNormal(fVertex1, fVertex2, fVertex3);

        glNormal3f(fNormal[0], fNormal[1], fNormal[2]);
        glColor3f(0.0f, 0.1f, 1.0f);//1-2-3-4 anti-clock
        glVertex3f(0.0,-0.4,0.0);
        glVertex3f(1.0,-0.4,0.0);
        glColor3f(0.1f, 0.0f, 1.0f);
        glVertex3f(1.0,0.1,0.0);
        glVertex3f(0.0,0.1,0.0);
        glEnd();

        glBegin(GL_QUADS);
        //G region
        fVertex1[0] = 0.0, fVertex1[1] = -0.9, fVertex1[2] = 0.0;
        fVertex2[0] = 1.0, fVertex2[1] = -0.9, fVertex2[2] = 0.0;
        fVertex3[0] = 1.0, fVertex3[1] = -0.4, fVertex3[2] = 0.0;
        fNormal = ComputeNormal(fVertex1, fVertex2, fVertex3);

        glNormal3f(fNormal[0], fNormal[1], fNormal[2]);
        glColor3f(0.0f, 1.0f, 0.0f);//1-2-3-4 anti-clock
        glVertex3f(0.0,-0.9,0.0);
        glVertex3f(1.0,-0.9,0.0);
        glColor3f(0.0f, 0.1f, 1.0f);
        glVertex3f(1.0,-0.4,0.0);
        glVertex3f(0.0,-0.4,0.0);
        glEnd();
        glPopMatrix();
    }
    DrawMessage();
}

void BasicGLPane::DrawMessage()
{
    wxClientDC dc(this); // only to be used in paint events. use wxClientDC to paint outside the paint event
    SetCurrent(*m_glContext);
    if(m_iBoundBox == 1)
    {
        glPushMatrix();
        glLoadIdentity();
        glClearColor(1.0f, 0.0f, 0.0f, 1.0f); // Black Background
        //Load camera position, so that we can start drawing shapes correctly
        gluLookAt(0.0,0.0,0.0+1.0,  //look from camera
                  0.0,0.0,0.0,     //look at origin //centre of the screen
                  0.0f,1.0f,0.0f);//YCoord axis up vector
        int iWidth = 500/3.0;
        int iHeight = 500/12.0;
        glViewport(500, 500, (GLsizei)iWidth, (GLsizei)iHeight); // Set our viewport to the size of our window
        // and multiply matrices to glmodelview's top stack and render

        glDisable(GL_LIGHTING);//for texts
        glColor3f(1.0f, 1.0f, 1.0f);

        //-----display result name----//
        string strResultType;
        if (mstrResultId == "stressx")
            strResultType = "Stress: X component";
        else if (mstrResultId == "stressy")
            strResultType = "Stress: Y component";
        else if (mstrResultId == "stressz")
            strResultType = "Stress: Z component";
        else if (mstrResultId == "stressxy")
            strResultType = "Stress: XY component";
        else if (mstrResultId == "stressyz")
            strResultType = "Stress: YZ component";
        else if (mstrResultId == "stressxz")
            strResultType = "Stress: XZ component";
        else
            strResultType = "";

        //-----display result name-----//
        int charlen = strResultType.length()+1;
        char *cResultType = new char[charlen];
        for(int i = 0; i < charlen-1; i++)
           cResultType[i] = strResultType[i];
        cResultType[charlen-1] = '\0';

        glRasterPos2f(-1.0,0.0);
        for(int i = 0; i < charlen; i++)
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12,cResultType[i]);
        delete [] cResultType;

        //--------
        glPopMatrix();
        glEnable(GL_LIGHTING);//back to light mode
    }
}

void BasicGLPane::Reserve_minmax_Vectors()
{
    int iNofSubcases = oElementData.GetNbrofSubCases();
    //reserve min vectors for stress components
    oElementData.fStressX_min.reserve(iNofSubcases);
    oElementData.fStressY_min.reserve(iNofSubcases);
    oElementData.fStressZ_min.reserve(iNofSubcases);
    oElementData.fStressXY_min.reserve(iNofSubcases);
    oElementData.fStressYZ_min.reserve(iNofSubcases);
    oElementData.fStressXZ_min.reserve(iNofSubcases);
    //reserve max vectors for stress components
    oElementData.fStressX_max.reserve(iNofSubcases);
    oElementData.fStressY_max.reserve(iNofSubcases);
    oElementData.fStressZ_max.reserve(iNofSubcases);
    oElementData.fStressXY_max.reserve(iNofSubcases);
    oElementData.fStressYZ_max.reserve(iNofSubcases);
    oElementData.fStressXZ_max.reserve(iNofSubcases);
}

//next three methods work together
void BasicGLPane::Getrbg_with_StressMinMax(int iStressType)
{
    if(oElementData.ElemStressPresence() == false)
        return;
    miSubcaseId = 0; //display first subcase
    switch (iStressType)
    {
    case 1:
        mstrResultId = "stressx";
        for(int iSubcase = 0; iSubcase < oElementData.GetNbrofSubCases(); iSubcase++)
        {
            FindStressX_MinMaxInorder(oElementData.GetRootNode(), iSubcase);
            oElementData.fStressX_max[iSubcase] = m_fMaxTmp;
            oElementData.fStressX_min[iSubcase] = m_fMinTmp;
            m_fMaxTmp = 321;//revert value
            Calculate_StressX_rgb_Inorder(oElementData.GetRootNode(), iSubcase);
        }
        break;
    case 2:
        mstrResultId = "stressy";
        for(int iSubcase = 0; iSubcase < oElementData.GetNbrofSubCases(); iSubcase++)
        {
            FindStressY_MinMaxInorder(oElementData.GetRootNode(), iSubcase);
            oElementData.fStressY_max[iSubcase] = m_fMaxTmp;
            oElementData.fStressY_min[iSubcase] = m_fMinTmp;
            m_fMaxTmp = 321;//revert value
            Calculate_StressY_rgb_Inorder(oElementData.GetRootNode(), iSubcase);
        }
        break;
    case 3:
        mstrResultId = "stressz";
        for(int iSubcase = 0; iSubcase < oElementData.GetNbrofSubCases(); iSubcase++)
        {
            FindStressZ_MinMaxInorder(oElementData.GetRootNode(), iSubcase);
            oElementData.fStressZ_max[iSubcase] = m_fMaxTmp;
            oElementData.fStressZ_min[iSubcase] = m_fMinTmp;
            m_fMaxTmp = 321;//revert value
            Calculate_StressZ_rgb_Inorder(oElementData.GetRootNode(), iSubcase);
        }
        break;
    case 4:
        mstrResultId = "stressxy";
        for(int iSubcase = 0; iSubcase < oElementData.GetNbrofSubCases(); iSubcase++)
        {
            FindStressXY_MinMaxInorder(oElementData.GetRootNode(), iSubcase);
            oElementData.fStressXY_max[iSubcase] = m_fMaxTmp;
            oElementData.fStressXY_min[iSubcase] = m_fMinTmp;
            m_fMaxTmp = 321;//revert value
            Calculate_StressXY_rgb_Inorder(oElementData.GetRootNode(), iSubcase);
        }
        break;
    case 5:
        mstrResultId = "stressyz";
        for(int iSubcase = 0; iSubcase < oElementData.GetNbrofSubCases(); iSubcase++)
        {
            FindStressYZ_MinMaxInorder(oElementData.GetRootNode(), iSubcase);
            oElementData.fStressYZ_max[iSubcase] = m_fMaxTmp;
            oElementData.fStressYZ_min[iSubcase] = m_fMinTmp;
            m_fMaxTmp = 321;//revert value
            Calculate_StressYZ_rgb_Inorder(oElementData.GetRootNode(), iSubcase);
        }
        break;
    case 6:
        mstrResultId = "stressxz";
        for(int iSubcase = 0; iSubcase < oElementData.GetNbrofSubCases(); iSubcase++)
        {
            FindStressXZ_MinMaxInorder(oElementData.GetRootNode(), iSubcase);
            oElementData.fStressXZ_max[iSubcase] = m_fMaxTmp;
            oElementData.fStressXZ_min[iSubcase] = m_fMinTmp;
            m_fMaxTmp = 321;//revert value
            Calculate_StressXZ_rgb_Inorder(oElementData.GetRootNode(), iSubcase);
        }
        break;
    }
    Refresh();
}

//for stress x
void BasicGLPane::FindStressX_MinMaxInorder(SBinaryTreeNode* pInorderTraverse,int iSubCase)
{
    if(pInorderTraverse != NULL)
    {
        FindStressX_MinMaxInorder(pInorderTraverse->pNodeleft, iSubCase);  //Traverse leftside tree nodes
        if(pInorderTraverse->uiIdentification != 0)
        {
            if(pInorderTraverse->ElementInfo.vecStressStrainData.size() != 0)
            {
                if(m_fMaxTmp == 321)
                {
                    m_fMaxTmp = pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressX;
                    m_fMinTmp = pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressX;
                }
                if(m_fMaxTmp < pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressX)
                    m_fMaxTmp = pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressX;
                if(m_fMinTmp > pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressX)
                    m_fMinTmp = pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressX;
            }
        }
        FindStressX_MinMaxInorder(pInorderTraverse->pNoderight, iSubCase); //Traverse rightside tree nodes
    }
}

void BasicGLPane::Calculate_StressX_rgb_Inorder(SBinaryTreeNode* pInorderTraverse,int iSubCase)
{
    if(pInorderTraverse != NULL)
    {
        Calculate_StressX_rgb_Inorder(pInorderTraverse->pNodeleft, iSubCase);  //Traverse leftside tree nodes
        if(pInorderTraverse->uiIdentification != 0)
        {
            if(pInorderTraverse->ElementInfo.vecStressStrainData.size() != 0)
            {
                //interpolation
                m_fTmp = (-1.0) * (pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressX - oElementData.fStressX_max[iSubCase]);
                m_fTmp = m_fTmp/(oElementData.fStressX_min[iSubCase] - oElementData.fStressX_max[iSubCase]);
                m_fTmp = m_fTmp + 1.0;
                //note: order is r, b &g. calc the r,g&b values (need to fine tune the formula)
                if(m_fTmp >= 0.5)
                {
                    pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressX_rbg[0] = m_fTmp;
                    pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressX_rbg[1] = 1 - m_fTmp;
                    pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressX_rbg[2] = 0.0;
                }
                else if(m_fTmp < 0.5)
                {
                    pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressX_rbg[0] = 0.0;
                    pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressX_rbg[1] = m_fTmp;
                    pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressX_rbg[2] = 1 - m_fTmp;
                }
            }
        }
        Calculate_StressX_rgb_Inorder(pInorderTraverse->pNoderight, iSubCase); //Traverse rightside tree nodes
    }
}

//for stress y
void BasicGLPane::FindStressY_MinMaxInorder(SBinaryTreeNode* pInorderTraverse,int iSubCase)
{
    if(pInorderTraverse != NULL)
    {
        FindStressY_MinMaxInorder(pInorderTraverse->pNodeleft, iSubCase);  //Traverse leftside tree nodes
        if(pInorderTraverse->uiIdentification != 0)
        {
            if(pInorderTraverse->ElementInfo.vecStressStrainData.size() != 0)
            {
                if(m_fMaxTmp == 321)
                {
                    m_fMaxTmp = pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressY;
                    m_fMinTmp = pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressY;
                }
                if(m_fMaxTmp < pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressY)
                    m_fMaxTmp = pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressY;
                if(m_fMinTmp > pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressY)
                    m_fMinTmp = pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressY;
            }
        }
        FindStressY_MinMaxInorder(pInorderTraverse->pNoderight, iSubCase); //Traverse rightside tree nodes
    }
}

void BasicGLPane::Calculate_StressY_rgb_Inorder(SBinaryTreeNode* pInorderTraverse,int iSubCase)
{
    if(pInorderTraverse != NULL)
    {
        Calculate_StressY_rgb_Inorder(pInorderTraverse->pNodeleft, iSubCase);  //Traverse leftside tree nodes
        if(pInorderTraverse->uiIdentification != 0)
        {
            if(pInorderTraverse->ElementInfo.vecStressStrainData.size() != 0)
            {
                //interpolation
                m_fTmp = (-1.0) * (pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressY - oElementData.fStressY_max[iSubCase]);
                m_fTmp = m_fTmp/(oElementData.fStressY_min[iSubCase] - oElementData.fStressY_max[iSubCase]);
                m_fTmp = m_fTmp + 1.0;
                //cal the r,g&b values (need to fine tune the formula)
                if(m_fTmp >= 0.5)
                {
                    pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressY_rbg[0] = m_fTmp;
                    pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressY_rbg[1] = 1 - m_fTmp;
                    pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressY_rbg[2] = 0.0;
                }
                else if(m_fTmp < 0.5)
                {
                    pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressY_rbg[0] = 0.0;
                    pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressY_rbg[1] = m_fTmp;
                    pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressY_rbg[2] = 1 - m_fTmp;
                }
            }
        }
        Calculate_StressY_rgb_Inorder(pInorderTraverse->pNoderight, iSubCase); //Traverse rightside tree nodes
    }
}

//for stress z
void BasicGLPane::FindStressZ_MinMaxInorder(SBinaryTreeNode* pInorderTraverse,int iSubCase)
{
    if(pInorderTraverse != NULL)
    {
        FindStressZ_MinMaxInorder(pInorderTraverse->pNodeleft, iSubCase);  //Traverse leftside tree nodes
        if(pInorderTraverse->uiIdentification != 0)
        {
            if(pInorderTraverse->ElementInfo.vecStressStrainData.size() != 0)
            {
                if(m_fMaxTmp == 321)
                {
                    m_fMaxTmp = pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressZ;
                    m_fMinTmp = pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressZ;
                }
                if(m_fMaxTmp < pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressZ)
                    m_fMaxTmp = pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressZ;
                if(m_fMinTmp > pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressZ)
                    m_fMinTmp = pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressZ;
            }
        }
        FindStressZ_MinMaxInorder(pInorderTraverse->pNoderight, iSubCase); //Traverse rightside tree nodes
    }
}

void BasicGLPane::Calculate_StressZ_rgb_Inorder(SBinaryTreeNode* pInorderTraverse,int iSubCase)
{
    if(pInorderTraverse != NULL)
    {
        Calculate_StressZ_rgb_Inorder(pInorderTraverse->pNodeleft, iSubCase);  //Traverse leftside tree nodes
        if(pInorderTraverse->uiIdentification != 0)
        {
            if(pInorderTraverse->ElementInfo.vecStressStrainData.size() != 0)
            {
                //interpolation
                m_fTmp = (-1.0) * (pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressZ - oElementData.fStressZ_max[iSubCase]);
                m_fTmp = m_fTmp/(oElementData.fStressZ_min[iSubCase] - oElementData.fStressZ_max[iSubCase]);
                m_fTmp = m_fTmp + 1.0;
                //cal the r,g&b values (need to fine tune the formula)
                if(m_fTmp >= 0.5)
                {
                    pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressZ_rbg[0] = m_fTmp;
                    pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressZ_rbg[1] = 1 - m_fTmp;
                    pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressZ_rbg[2] = 0.0;
                }
                else if(m_fTmp < 0.5)
                {
                    pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressZ_rbg[0] = 0.0;
                    pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressZ_rbg[1] = m_fTmp;
                    pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressZ_rbg[2] = 1 - m_fTmp;
                }
            }
        }
        Calculate_StressZ_rgb_Inorder(pInorderTraverse->pNoderight, iSubCase); //Traverse rightside tree nodes
    }
}

//for stress xy
void BasicGLPane::FindStressXY_MinMaxInorder(SBinaryTreeNode* pInorderTraverse,int iSubCase)
{
    if(pInorderTraverse != NULL)
    {
        FindStressXY_MinMaxInorder(pInorderTraverse->pNodeleft, iSubCase);  //Traverse leftside tree nodes
        if(pInorderTraverse->uiIdentification != 0)
        {
            if(pInorderTraverse->ElementInfo.vecStressStrainData.size() != 0)
            {
                if(m_fMaxTmp == 321)
                {
                    m_fMaxTmp = pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressXY;
                    m_fMinTmp = pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressXY;
                }
                if(m_fMaxTmp < pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressXY)
                    m_fMaxTmp = pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressXY;
                if(m_fMinTmp > pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressXY)
                    m_fMinTmp = pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressXY;
            }
        }
        FindStressXY_MinMaxInorder(pInorderTraverse->pNoderight, iSubCase); //Traverse rightside tree nodes
    }
}

void BasicGLPane::Calculate_StressXY_rgb_Inorder(SBinaryTreeNode* pInorderTraverse,int iSubCase)
{
    if(pInorderTraverse != NULL)
    {
        Calculate_StressXY_rgb_Inorder(pInorderTraverse->pNodeleft, iSubCase);  //Traverse leftside tree nodes
        if(pInorderTraverse->uiIdentification != 0)
        {
            if(pInorderTraverse->ElementInfo.vecStressStrainData.size() != 0)
            {
                //interpolation
                m_fTmp = (-1.0) * (pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressXY - oElementData.fStressXY_max[iSubCase]);
                m_fTmp = m_fTmp/(oElementData.fStressXY_min[iSubCase] - oElementData.fStressXY_max[iSubCase]);
                m_fTmp = m_fTmp + 1.0;
                //cal the r,g&b values (need to fine tune the formula)
                if(m_fTmp >= 0.5)
                {
                    pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressXY_rbg[0] = m_fTmp;
                    pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressXY_rbg[1] = 1 - m_fTmp;
                    pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressXY_rbg[2] = 0.0;
                }
                else if(m_fTmp < 0.5)
                {
                    pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressXY_rbg[0] = 0.0;
                    pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressXY_rbg[1] = m_fTmp;
                    pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressXY_rbg[2] = 1 - m_fTmp;
                }
            }
        }
        Calculate_StressXY_rgb_Inorder(pInorderTraverse->pNoderight, iSubCase); //Traverse rightside tree nodes
    }
}

//for stress yz
void BasicGLPane::FindStressYZ_MinMaxInorder(SBinaryTreeNode* pInorderTraverse,int iSubCase)
{
    if(pInorderTraverse != NULL)
    {
        FindStressYZ_MinMaxInorder(pInorderTraverse->pNodeleft, iSubCase);  //Traverse leftside tree nodes
        if(pInorderTraverse->uiIdentification != 0)
        {
            if(pInorderTraverse->ElementInfo.vecStressStrainData.size() != 0)
            {
                if(m_fMaxTmp == 321)
                {
                    m_fMaxTmp = pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressYZ;
                    m_fMinTmp = pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressYZ;
                }
                if(m_fMaxTmp < pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressYZ)
                    m_fMaxTmp = pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressYZ;
                if(m_fMinTmp > pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressYZ)
                    m_fMinTmp = pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressYZ;
            }
        }
        FindStressYZ_MinMaxInorder(pInorderTraverse->pNoderight, iSubCase); //Traverse rightside tree nodes
    }
}

void BasicGLPane::Calculate_StressYZ_rgb_Inorder(SBinaryTreeNode* pInorderTraverse,int iSubCase)
{
    if(pInorderTraverse != NULL)
    {
        Calculate_StressYZ_rgb_Inorder(pInorderTraverse->pNodeleft, iSubCase);  //Traverse leftside tree nodes
        if(pInorderTraverse->uiIdentification != 0)
        {
            if(pInorderTraverse->ElementInfo.vecStressStrainData.size() != 0)
            {
                //interpolation
                m_fTmp = (-1.0) * (pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressYZ - oElementData.fStressYZ_max[iSubCase]);
                m_fTmp = m_fTmp/(oElementData.fStressYZ_min[iSubCase] - oElementData.fStressYZ_max[iSubCase]);
                m_fTmp = m_fTmp + 1.0;
                //cal the r,g&b values (need to fine tune the formula)
                if(m_fTmp >= 0.5)
                {
                    pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressYZ_rbg[0] = m_fTmp;
                    pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressYZ_rbg[1] = 1 - m_fTmp;
                    pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressYZ_rbg[2] = 0.0;
                }
                else if(m_fTmp < 0.5)
                {
                    pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressYZ_rbg[0] = 0.0;
                    pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressYZ_rbg[1] = m_fTmp;
                    pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressYZ_rbg[2] = 1 - m_fTmp;
                }
            }
        }
        Calculate_StressYZ_rgb_Inorder(pInorderTraverse->pNoderight, iSubCase); //Traverse rightside tree nodes
    }
}

//for stress xz
void BasicGLPane::FindStressXZ_MinMaxInorder(SBinaryTreeNode* pInorderTraverse,int iSubCase)
{
    if(pInorderTraverse != NULL)
    {
        FindStressXZ_MinMaxInorder(pInorderTraverse->pNodeleft, iSubCase);  //Traverse leftside tree nodes
        if(pInorderTraverse->uiIdentification != 0)
        {
            if(pInorderTraverse->ElementInfo.vecStressStrainData.size() != 0)
            {
                if(m_fMaxTmp == 321)
                {
                    m_fMaxTmp = pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressXZ;
                    m_fMinTmp = pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressXZ;
                }
                if(m_fMaxTmp < pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressXZ)
                    m_fMaxTmp = pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressXZ;
                if(m_fMinTmp > pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressXZ)
                    m_fMinTmp = pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressXZ;
            }
        }
        FindStressXZ_MinMaxInorder(pInorderTraverse->pNoderight, iSubCase); //Traverse rightside tree nodes
    }
}

void BasicGLPane::Calculate_StressXZ_rgb_Inorder(SBinaryTreeNode* pInorderTraverse,int iSubCase)
{
    if(pInorderTraverse != NULL)
    {
        Calculate_StressXZ_rgb_Inorder(pInorderTraverse->pNodeleft, iSubCase);  //Traverse leftside tree nodes
        if(pInorderTraverse->uiIdentification != 0)
        {
            if(pInorderTraverse->ElementInfo.vecStressStrainData.size() != 0)
            {
                //interpolation
                m_fTmp = (-1.0) * (pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressXZ - oElementData.fStressXZ_max[iSubCase]);
                m_fTmp = m_fTmp/(oElementData.fStressXZ_min[iSubCase] - oElementData.fStressXZ_max[iSubCase]);
                m_fTmp = m_fTmp + 1.0;
                //cal the r,g&b values (need to fine tune the formula)
                if(m_fTmp >= 0.5)
                {
                    pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressXZ_rbg[0] = m_fTmp;
                    pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressXZ_rbg[1] = 1 - m_fTmp;
                    pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressXZ_rbg[2] = 0.0;
                }
                else if(m_fTmp < 0.5)
                {
                    pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressXZ_rbg[0] = 0.0;
                    pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressXZ_rbg[1] = m_fTmp;
                    pInorderTraverse->ElementInfo.vecStressStrainData[iSubCase].fStressXZ_rbg[2] = 1 - m_fTmp;
                }
            }
        }
        Calculate_StressXZ_rgb_Inorder(pInorderTraverse->pNoderight, iSubCase); //Traverse rightside tree nodes
    }
}
