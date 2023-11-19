#include "cbasicmenubar.h"
#include "cbasictoolbar.h"
#include "basicglpane.h"
#include "border.h"
#include "cloadingwindow.h"
#include "cviewerintroimage.h"
#include "graph/node.h"
#include "graph/RenderNode.h"
#include "graphframe.h"
#include <wx/menu.h>
#include <wx/dialog.h>
#include <wx/treectrl.h>
#include <wx/event.h>
#include <wx/statusbr.h>
#include "basicglpane.h"
//#include "graph/node.h"
//#include "graph/RenderNode.h"
#include "GL/glut.h"
#include "GL/GLU.h"
#include "GL/GL.h"
#define ID_ENABLE_AUTOROTATION	      2001
#define ID_DISABLE_AUTOROTATION       2002
#define ID_RESETVIEW	              2003
#define ID_FITVIEW	                  2004
#define ID_LEFT_VIEW	              2005
#define ID_RIGHT_VIEW	              2006
#define ID_TOP_VIEW	                  2007
#define ID_BOTTOM_VIEW	              2008
#define ID_FRONT_VIEW	              2009
#define ID_BACK_VIEW	              2010
#define ID_ABOUT    	              2011

#define ID_NODE_COORDINTES            2012
#define ID_ELEMENT_CONNECTIVITY       2013
#define ID_NODE_DISPLACEMENT          2014
#define ID_ELEMENT_STRESSSTRAIN       2015
#define ID_APPLIEDLOADS_IN_NODES      2016
#define ID_OUTPUT_STRAINENERGY        2017
#define ID_OUTPUT_KINETICENERGY       2018
#define ID_OUTPUT_ENERGYLOSS          2019
#define ID_CONSTRAINTFORCES           2020
#define ID_EXPORTALL                  2021

#define ID_GRAPH       	              2022
#define ID_GRAPH_EXIT       	      2023

#define wxID_DISPLAY_STRESSX          2024
#define wxID_DISPLAY_STRESSY          2025
#define wxID_DISPLAY_STRESSZ          2026
#define wxID_DISPLAY_STRESSXY         2027
#define wxID_DISPLAY_STRESSYZ         2028
#define wxID_DISPLAY_STRESSXZ         2029
#define wxID_FEEDBACK                 2030

//forward declaration for global and static entities
//static CRenderNode *pGraph;// = new CRenderNode(wxT("Graph"));
//CRenderNode *CBasicMenubar::pGraph;
BasicGLPane *CBasicMenubar::glPane;
CTextControl *CBasicMenubar::pOutputConsole;
//void g_keyboardfunc(unsigned char iKey,int iXvalue,int iYvalue);
//void g_Mousefunc(int iBtn,int iState,int iXvalue,int iYvalue);
//void g_Reshapefunc(int iView_Width,int iView_Height);
//void g_Init();
//void  g_Timer(int iValue);
//void g_Drawfunc();
bool CBasicMenubar::bGraphFramePresent;
bool CBasicMenubar::bAiFramePresent;

CBasicMenubar::CBasicMenubar(const wxString& title) : wxFrame(NULL, wxID_ANY, title,  wxPoint(-1,-1), wxSize(1200,725), wxDEFAULT_FRAME_STYLE | wxFRAME_EX_METAL)
{
    //ctr
    //intro image is displayed before viewer construction
//    CViewerIntroImage* pIntroImage = new CViewerIntroImage(NULL,wxT("IntroductionImage"));
//    pIntroImage->Show(true);
    menubarBasic = new wxMenuBar;
    /*********file menu*******/
    menuFile = new wxMenu();
    menuitemNew = new wxMenuItem(menuFile,wxID_NEW, wxT("&New"), wxT("&help"),true,NULL);
    menuitemOpen = new wxMenuItem(menuFile,wxID_OPEN, wxT("&Open"), wxT("&help"),true,NULL);
    menuFile->Append(menuitemNew);
    menuFile->Append(menuitemOpen);

    quit = new wxMenuItem(menuFile, wxID_EXIT, wxT("&Exit\tctrl+W"), wxT("&help"),true,NULL);
    menuFile->Append(quit);
    menubarBasic->Append(menuFile, wxT("&File"));
    /*********view menu*******/
    menuAutoRotation = new wxMenu();
    menuAutoRotation->Append(ID_ENABLE_AUTOROTATION, wxT("&Enable"));
    menuAutoRotation->Append(ID_DISABLE_AUTOROTATION, wxT("&Disable"));

    menuView = new wxMenu();
    menuView->Append(ID_RESETVIEW, wxT("&Reset View"));
    menuView->Append(ID_FITVIEW, wxT("&Fit View"));
    menuView->Append(ID_LEFT_VIEW, wxT("&Left View"));
    menuView->Append(ID_RIGHT_VIEW, wxT("&Right View"));
    menuView->Append(ID_TOP_VIEW, wxT("&Top View"));
    menuView->Append(ID_BOTTOM_VIEW, wxT("&Bottom View"));
    menuView->Append(ID_FRONT_VIEW, wxT("&Front View"));
    menuView->Append(ID_BACK_VIEW, wxT("&Back View"));
    menuView->AppendSubMenu(menuAutoRotation, wxT("&Autorotation"));

    menubarBasic->Append(menuView, wxT("&View"));
    /********export**********/
    menuMeshData = new wxMenu();
    menuMeshData->Append(ID_NODE_COORDINTES, wxT("&Node Coordinates"));
    menuMeshData->Append(ID_ELEMENT_CONNECTIVITY, wxT("&Element Connectivity"));
    menuMeshData->Append(ID_APPLIEDLOADS_IN_NODES, wxT("&Applied Loads in Nodes"));
    menuMeshData->Append(ID_CONSTRAINTFORCES, wxT("&Constraint Forces"));

    menuResultData = new wxMenu();
    menuResultData->Append(ID_NODE_DISPLACEMENT, wxT("&Node Displacement"));
    menuResultData->Append(ID_ELEMENT_STRESSSTRAIN, wxT("&Element Stress/Strain"));
    menuResultData->Append(ID_OUTPUT_STRAINENERGY, wxT("&Strain Energy"));
    menuResultData->Append(ID_OUTPUT_KINETICENERGY, wxT("&Kinetic Energy"));
    menuResultData->Append(ID_OUTPUT_ENERGYLOSS, wxT("&Energy Loss"));

    menuExport = new wxMenu();
    menuExport->AppendSubMenu(menuMeshData, wxT("&Input Mesh (text)"));
    menuExport->AppendSubMenu(menuResultData, wxT("&Output Results (text)"));
    menuExport->Append(ID_EXPORTALL,wxT("Both the Above (text)"));

    menubarBasic->Append(menuExport, wxT("&Export"));
    /********result menu******/
    submenuNodalDisplacementComponents = new wxMenu();
    submenuNodalDisplacementComponents->Append(wxID_ANY, wxT("&TranslationX"));
    submenuNodalDisplacementComponents->Append(wxID_ANY, wxT("&TranslationY"));
    submenuNodalDisplacementComponents->Append(wxID_ANY, wxT("&TranslationZ"));
    submenuNodalDisplacementComponents->Append(wxID_ANY, wxT("Translation Vector Sum"));
    submenuNodalDisplacementComponents->Append(wxID_ANY, wxT("&RotationX"));
    submenuNodalDisplacementComponents->Append(wxID_ANY, wxT("&RotationY"));
    submenuNodalDisplacementComponents->Append(wxID_ANY, wxT("&RotationZ"));
    submenuNodalDisplacementComponents->Append(wxID_ANY, wxT("&Rotation Vector Sum"));

    submenuElemStressComponents = new wxMenu();
    submenuElemStressComponents->Append(wxID_DISPLAY_STRESSX, wxT("&X Component"));
    submenuElemStressComponents->Append(wxID_DISPLAY_STRESSY, wxT("&Y Component"));
    submenuElemStressComponents->Append(wxID_DISPLAY_STRESSZ, wxT("&Z Component"));
    submenuElemStressComponents->Append(wxID_DISPLAY_STRESSXY, wxT("&XY Component"));
    submenuElemStressComponents->Append(wxID_DISPLAY_STRESSYZ, wxT("&YZ Component"));
    submenuElemStressComponents->Append(wxID_DISPLAY_STRESSXZ, wxT("&XZ Component"));
    submenuElemStressComponents->Append(wxID_ANY, wxT("&1st Principal Stress"));
    submenuElemStressComponents->Append(wxID_ANY, wxT("&2nd Principal Stress"));
    submenuElemStressComponents->Append(wxID_ANY, wxT("&3rd Principal Stress"));
    submenuElemStressComponents->Append(wxID_ANY, wxT("&Stress Intensity"));
    submenuElemStressComponents->Append(wxID_ANY, wxT("Vonmises Stress"));

    submenuElemTotalMechanicalStrain = new wxMenu();
    submenuElemTotalMechanicalStrain->Append(wxID_ANY, wxT("&X Component"));
    submenuElemTotalMechanicalStrain->Append(wxID_ANY, wxT("&Y Component"));
    submenuElemTotalMechanicalStrain->Append(wxID_ANY, wxT("&Z Component"));
    submenuElemTotalMechanicalStrain->Append(wxID_ANY, wxT("&XY Component"));
    submenuElemTotalMechanicalStrain->Append(wxID_ANY, wxT("&YZ Component"));
    submenuElemTotalMechanicalStrain->Append(wxID_ANY, wxT("&XZ Component"));
    submenuElemTotalMechanicalStrain->Append(wxID_ANY, wxT("&1st Principal Strain"));
    submenuElemTotalMechanicalStrain->Append(wxID_ANY, wxT("&2nd Principal Strain"));
    submenuElemTotalMechanicalStrain->Append(wxID_ANY, wxT("&3rd Principal Strain"));
    submenuElemTotalMechanicalStrain->Append(wxID_ANY, wxT("&Total Mechanical Strain Intensity"));
    submenuElemTotalMechanicalStrain->Append(wxID_ANY, wxT("&Total Mechanical Vonmises Strain"));

    submenuElemElasticStrain = new wxMenu();
    submenuElemElasticStrain->Append(wxID_ANY, wxT("&X"));
    submenuElemElasticStrain->Append(wxID_ANY, wxT("&Y"));
    submenuElemElasticStrain->Append(wxID_ANY, wxT("&Z"));
    submenuElemElasticStrain->Append(wxID_ANY, wxT("&XY"));
    submenuElemElasticStrain->Append(wxID_ANY, wxT("&YZ"));
    submenuElemElasticStrain->Append(wxID_ANY, wxT("&XZ"));
    submenuElemElasticStrain->Append(wxID_ANY, wxT("&1st Principal"));
    submenuElemElasticStrain->Append(wxID_ANY, wxT("&2nd Principal"));
    submenuElemElasticStrain->Append(wxID_ANY, wxT("&3rd Principal"));
    submenuElemElasticStrain->Append(wxID_ANY, wxT("&Elastic Strain Intensity"));
    submenuElemElasticStrain->Append(wxID_ANY, wxT("&Vonmises Elastic Strain"));

    submenuElemPlasticStrain = new wxMenu();
    submenuElemPlasticStrain->Append(wxID_ANY, wxT("&X"));
    submenuElemPlasticStrain->Append(wxID_ANY, wxT("&Y"));
    submenuElemPlasticStrain->Append(wxID_ANY, wxT("&Z"));
    submenuElemPlasticStrain->Append(wxID_ANY, wxT("&XY"));
    submenuElemPlasticStrain->Append(wxID_ANY, wxT("&YZ"));
    submenuElemPlasticStrain->Append(wxID_ANY, wxT("&XZ"));
    submenuElemPlasticStrain->Append(wxID_ANY, wxT("&1st Principal"));
    submenuElemPlasticStrain->Append(wxID_ANY, wxT("&2nd Principal"));
    submenuElemPlasticStrain->Append(wxID_ANY, wxT("&3rd Principal"));
    submenuElemPlasticStrain->Append(wxID_ANY, wxT("&Plastic Strain Intensity"));
    submenuElemPlasticStrain->Append(wxID_ANY, wxT("&Vonmises Plastic Strain"));
    submenuElemPlasticStrain->Append(wxID_ANY, wxT("&Equivalent Plastic Strain"));

    submenuElemCreepStrain = new wxMenu();
    submenuElemCreepStrain->Append(wxID_ANY, wxT("&X"));
    submenuElemCreepStrain->Append(wxID_ANY, wxT("&Y"));
    submenuElemCreepStrain->Append(wxID_ANY, wxT("&Z"));
    submenuElemCreepStrain->Append(wxID_ANY, wxT("&XY"));
    submenuElemCreepStrain->Append(wxID_ANY, wxT("&YZ"));
    submenuElemCreepStrain->Append(wxID_ANY, wxT("&XZ"));
    submenuElemCreepStrain->Append(wxID_ANY, wxT("&1st Principal"));
    submenuElemCreepStrain->Append(wxID_ANY, wxT("&2nd Principal"));
    submenuElemCreepStrain->Append(wxID_ANY, wxT("&3rd Principal"));
    submenuElemCreepStrain->Append(wxID_ANY, wxT("&Creep Strain Intensity"));
    submenuElemCreepStrain->Append(wxID_ANY, wxT("&Vonmises Creep Strain"));
    submenuElemCreepStrain->Append(wxID_ANY, wxT("&Equivalent Creep Strain"));

    submenuElemThermalStrain = new wxMenu();
    submenuElemThermalStrain->Append(wxID_ANY, wxT("&X"));
    submenuElemThermalStrain->Append(wxID_ANY, wxT("&Y"));
    submenuElemThermalStrain->Append(wxID_ANY, wxT("&Z"));
    submenuElemThermalStrain->Append(wxID_ANY, wxT("&XY"));
    submenuElemThermalStrain->Append(wxID_ANY, wxT("&YZ"));
    submenuElemThermalStrain->Append(wxID_ANY, wxT("&XZ"));
    submenuElemThermalStrain->Append(wxID_ANY, wxT("&1st Principal"));
    submenuElemThermalStrain->Append(wxID_ANY, wxT("&2nd Principal"));
    submenuElemThermalStrain->Append(wxID_ANY, wxT("&3rd Principal"));
    submenuElemThermalStrain->Append(wxID_ANY, wxT("&Thermal Strain Intensity"));
    submenuElemThermalStrain->Append(wxID_ANY, wxT("&Vonmises Thermal Strain"));

    submenuElemTotalMechanicalandThermalStrain = new wxMenu();
    submenuElemTotalMechanicalandThermalStrain->Append(wxID_ANY, wxT("&X"));
    submenuElemTotalMechanicalandThermalStrain->Append(wxID_ANY, wxT("&Y"));
    submenuElemTotalMechanicalandThermalStrain->Append(wxID_ANY, wxT("&Z"));
    submenuElemTotalMechanicalandThermalStrain->Append(wxID_ANY, wxT("&XY"));
    submenuElemTotalMechanicalandThermalStrain->Append(wxID_ANY, wxT("&YZ"));
    submenuElemTotalMechanicalandThermalStrain->Append(wxID_ANY, wxT("&XZ"));
    submenuElemTotalMechanicalandThermalStrain->Append(wxID_ANY, wxT("&1st Principal"));
    submenuElemTotalMechanicalandThermalStrain->Append(wxID_ANY, wxT("&2nd Principal"));
    submenuElemTotalMechanicalandThermalStrain->Append(wxID_ANY, wxT("&3rd Principal"));
    submenuElemTotalMechanicalandThermalStrain->Append(wxID_ANY, wxT("&Total Mechanical and Thermal Strain Intensity"));
    submenuElemTotalMechanicalandThermalStrain->Append(wxID_ANY, wxT("&Vonmises Total Mechanical and Thermal Strain"));

    menuNodalResult = new wxMenu();
    menuNodalResult->AppendSubMenu(submenuNodalDisplacementComponents, wxT("&Displacement"));

    menuElemResult = new wxMenu();
    menuElemResult->AppendSubMenu(submenuElemStressComponents, wxT("&Stress"));
    menuElemResult->AppendSubMenu(submenuElemTotalMechanicalStrain, wxT("&Total Mechanical Strain"));
//    menuElemResult->AppendSubMenu(submenuElemElasticStrain, wxT("&Elastic Strain"));
//    menuElemResult->AppendSubMenu(submenuElemPlasticStrain, wxT("&Plastic Strain"));
//    menuElemResult->AppendSubMenu(submenuElemCreepStrain, wxT("&Creep Strain"));
//    menuElemResult->AppendSubMenu(submenuElemThermalStrain, wxT("&Thermal Strain"));
//    menuElemResult->AppendSubMenu(submenuElemTotalMechanicalandThermalStrain, wxT("&Total Mechanical and Thermal Strain"));

    menuResult = new wxMenu();
    menuResult->AppendSubMenu(menuNodalResult, wxT("&Display Nodal Results"));
    menuResult->AppendSubMenu(menuElemResult, wxT("&Display Element Results"));

    menubarBasic->Append(menuResult, wxT("&Results"));

    /*********AI menu*******/
    menuAI = new wxMenu();
    menuitemFeedback = new wxMenuItem(menuAI,wxID_FEEDBACK, wxT("&Research Data"), wxT("&ai"),true,NULL);
    menuAI->Append(menuitemFeedback);

    menubarBasic->Append(menuAI, wxT("&AI"));
    /*********help menu*******/
    menuHelp = new wxMenu();
    menuitemAbout = new wxMenuItem(menuHelp,ID_ABOUT, wxT("&About"), wxT("&help"),true,NULL);
    menuHelp->Append(menuitemAbout);

    menubarBasic->Append(menuHelp, wxT("&Help"));
    //add basic menubar
    SetMenuBar(menubarBasic);

    //status bar for main frame
    pStatusBar = new wxStatusBar(this,wxID_ANY,wxST_SIZEGRIP,wxT("statusbar"));
    int iFieldCount = 200;
    const int* ipFieldCount = &iFieldCount;
    pStatusBar->SetFieldsCount(1,ipFieldCount);
    int iStatusStyle = wxSB_FLAT;
    const int* ipStatusStyle = &iStatusStyle;
    pStatusBar->SetStatusStyles(1,ipStatusStyle);
    pStatusBar->SetStatusText(wxT("Supported Files: Nastran OP2, Ascii STL."),0);//Use an empty string ("") to clear the field.//The field to set, starting from zero.

    //Methods
    Connect(wxID_EXIT, wxEVT_COMMAND_MENU_SELECTED,wxCommandEventHandler(CBasicMenubar::OnQuit));
    Connect(wxID_NEW,  wxEVT_COMMAND_MENU_SELECTED,wxCommandEventHandler(CBasicMenubar::OnNew));
    Connect(wxID_OPEN, wxEVT_COMMAND_MENU_SELECTED,wxCommandEventHandler(CBasicMenubar::OnOpen));

    //view methods
    Connect(ID_ENABLE_AUTOROTATION, wxEVT_COMMAND_MENU_SELECTED,wxCommandEventHandler(CBasicMenubar::SetAutorotation));
    Connect(ID_DISABLE_AUTOROTATION, wxEVT_COMMAND_MENU_SELECTED,wxCommandEventHandler(CBasicMenubar::UnsetAutorotation));

    Connect(ID_GRAPH_EXIT, wxEVT_COMMAND_MENU_SELECTED,wxCommandEventHandler(CBasicMenubar::OnGraphQuit));
    //data export methods
    Connect(ID_NODE_COORDINTES, wxEVT_COMMAND_MENU_SELECTED,wxCommandEventHandler(CBasicMenubar::ExportNodeCoordinates));
    Connect(ID_ELEMENT_CONNECTIVITY, wxEVT_COMMAND_MENU_SELECTED,wxCommandEventHandler(CBasicMenubar::ExportElementConnectivity));
    Connect(ID_NODE_DISPLACEMENT, wxEVT_COMMAND_MENU_SELECTED,wxCommandEventHandler(CBasicMenubar::ExportNodalOutputDisplacement));
    Connect(ID_ELEMENT_STRESSSTRAIN, wxEVT_COMMAND_MENU_SELECTED,wxCommandEventHandler(CBasicMenubar::ExportElementStresstStrainResults));
    Connect(ID_APPLIEDLOADS_IN_NODES, wxEVT_COMMAND_MENU_SELECTED,wxCommandEventHandler(CBasicMenubar::ExportNodalAppliedLoads));
    Connect(ID_OUTPUT_STRAINENERGY, wxEVT_COMMAND_MENU_SELECTED,wxCommandEventHandler(CBasicMenubar::ExportOutputStrainEnergy));
    Connect(ID_OUTPUT_KINETICENERGY, wxEVT_COMMAND_MENU_SELECTED,wxCommandEventHandler(CBasicMenubar::ExportOutputKineticEnergy));
    Connect(ID_OUTPUT_ENERGYLOSS, wxEVT_COMMAND_MENU_SELECTED,wxCommandEventHandler(CBasicMenubar::ExportOutputEnergyLoss));
    Connect(ID_CONSTRAINTFORCES, wxEVT_COMMAND_MENU_SELECTED,wxCommandEventHandler(CBasicMenubar::ExportConstraintForces));
    Connect(ID_EXPORTALL, wxEVT_COMMAND_MENU_SELECTED,wxCommandEventHandler(CBasicMenubar::ExportAll));

    //results coloring methods
    Connect(wxID_DISPLAY_STRESSX, wxEVT_COMMAND_MENU_SELECTED,wxCommandEventHandler(CBasicMenubar::Display_StressX));
    Connect(wxID_DISPLAY_STRESSY, wxEVT_COMMAND_MENU_SELECTED,wxCommandEventHandler(CBasicMenubar::Display_StressY));
    Connect(wxID_DISPLAY_STRESSZ, wxEVT_COMMAND_MENU_SELECTED,wxCommandEventHandler(CBasicMenubar::Display_StressZ));
    Connect(wxID_DISPLAY_STRESSXY, wxEVT_COMMAND_MENU_SELECTED,wxCommandEventHandler(CBasicMenubar::Display_StressXY));
    Connect(wxID_DISPLAY_STRESSYZ, wxEVT_COMMAND_MENU_SELECTED,wxCommandEventHandler(CBasicMenubar::Display_StressYZ));
    Connect(wxID_DISPLAY_STRESSXZ, wxEVT_COMMAND_MENU_SELECTED,wxCommandEventHandler(CBasicMenubar::Display_StressXZ));

    //ai methods
    Connect(wxID_FEEDBACK, wxEVT_COMMAND_MENU_SELECTED,wxCommandEventHandler(CBasicMenubar::LaunchAi));

    //toolbars
    wxImage::AddHandler( new wxPNGHandler );
    wxBitmap bitmapOpen(wxT("images/open24.png"), wxBITMAP_TYPE_PNG);
    //wxBitmap bitmapExit(wxT("images/exit24.png"), wxBITMAP_TYPE_PNG);

    wxBitmap bitmapResetView(wxT("images/reset24.png"), wxBITMAP_TYPE_PNG);
    wxBitmap bitmapFitView(wxT("images/fitview24.png"), wxBITMAP_TYPE_PNG);
    wxBitmap bitmapLeftView(wxT("images/pqXPlus24.png"), wxBITMAP_TYPE_PNG);
    wxBitmap bitmapRightView(wxT("images/pqXMinus24.png"), wxBITMAP_TYPE_PNG);
    wxBitmap bitmapTopView(wxT("images/pqYMinus24.png"), wxBITMAP_TYPE_PNG);
    wxBitmap bitmapBottomView(wxT("images/pqYPlus24.png"), wxBITMAP_TYPE_PNG);
    wxBitmap bitmapFrontView(wxT("images/pqZMinus24.png"), wxBITMAP_TYPE_PNG);
    wxBitmap bitmapBackView(wxT("images/pqZPlus24.png"), wxBITMAP_TYPE_PNG);

    wxBitmap bitmapGraph(wxT("images/graph.png"), wxBITMAP_TYPE_PNG);


    toolbar1 = new wxToolBar(this, wxID_ANY,wxDefaultPosition,wxDefaultSize,wxTB_3DBUTTONS,wxT(""));
    toolbar1->AddTool(wxID_OPEN, bitmapOpen, wxT("Open Model"));
    //toolbar1->AddTool(wxID_EXIT, bitmapExit, wxT("Exit application"));
    toolbar1->Realize();//realize method constructs and displays the toolbar tools

    toolbar2 = new wxToolBar(this, wxID_ANY,wxDefaultPosition,wxDefaultSize,wxTB_3DBUTTONS,wxT(""));
    toolbar2->AddTool(ID_RESETVIEW, bitmapResetView, wxT(""));
    toolbar2->AddTool(ID_FITVIEW, bitmapFitView, wxT(""));
    toolbar2->AddTool(ID_LEFT_VIEW, bitmapLeftView, wxT(""));
    toolbar2->AddTool(ID_RIGHT_VIEW, bitmapRightView, wxT(""));
    toolbar2->AddTool(ID_TOP_VIEW, bitmapTopView, wxT(""));
    toolbar2->AddTool(ID_BOTTOM_VIEW, bitmapBottomView, wxT(""));
    toolbar2->AddTool(ID_FRONT_VIEW, bitmapFrontView, wxT(""));
    toolbar2->AddTool(ID_BACK_VIEW, bitmapBackView, wxT(""));
    toolbar2->AddTool(ID_GRAPH, bitmapGraph, wxT(""));
    toolbar2->Realize();

    Connect(wxID_OPEN,  wxEVT_COMMAND_TOOL_CLICKED,wxCommandEventHandler(CBasicMenubar::OnOpen));
    Connect(wxID_EXIT,  wxEVT_COMMAND_TOOL_CLICKED,wxCommandEventHandler(CBasicMenubar::OnQuit));

    Connect(ID_RESETVIEW, wxEVT_COMMAND_TOOL_CLICKED,wxCommandEventHandler(CBasicMenubar::SetResetView));
    Connect(ID_FITVIEW, wxEVT_COMMAND_TOOL_CLICKED,wxCommandEventHandler(CBasicMenubar::SetFitView));
    Connect(ID_LEFT_VIEW, wxEVT_COMMAND_TOOL_CLICKED,wxCommandEventHandler(CBasicMenubar::SetLeftView));
    Connect(ID_RIGHT_VIEW, wxEVT_COMMAND_TOOL_CLICKED,wxCommandEventHandler(CBasicMenubar::SetRightView));
    Connect(ID_TOP_VIEW, wxEVT_COMMAND_TOOL_CLICKED,wxCommandEventHandler(CBasicMenubar::SetTopView));
    Connect(ID_BOTTOM_VIEW, wxEVT_COMMAND_TOOL_CLICKED,wxCommandEventHandler(CBasicMenubar::SetBottomView));
    Connect(ID_FRONT_VIEW, wxEVT_COMMAND_TOOL_CLICKED,wxCommandEventHandler(CBasicMenubar::SetFrontView));
    Connect(ID_BACK_VIEW, wxEVT_COMMAND_TOOL_CLICKED,wxCommandEventHandler(CBasicMenubar::SetBackView));
    Connect(ID_ABOUT, wxEVT_COMMAND_TOOL_CLICKED,wxCommandEventHandler(CBasicMenubar::ShowAboutFrame));

    Connect(ID_GRAPH, wxEVT_COMMAND_TOOL_CLICKED,wxCommandEventHandler(CBasicMenubar::LaunchPlotter));

    treeviewModelHierarchy = new CTreeView(wxT("panelname"),this);

    //sizer arrangement
    wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);//sizer enables us to put several widgets into a row or a column.
    wxBoxSizer* hbox1 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* hbox2 = new wxBoxSizer(wxHORIZONTAL);

    int args[] = {WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16, 0};//alternate for
    //glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH)
    glPane = new BasicGLPane(this, args);

    hbox1->Add(toolbar1, 0, wxEXPAND | wxALL, 0);//options 1-window/sizeer,2-proportion with other windows,3-flags,4-border
    hbox1->Add(toolbar2, 0, wxEXPAND | wxALL, 0);

    hbox2->Add(treeviewModelHierarchy, 5, wxEXPAND | wxALL, 5);
    hbox2->Add(glPane, 20, wxEXPAND | wxALL, 5);

    vbox->Add(hbox1);
    vbox->Add(hbox2, 20, wxEXPAND | wxALL, 5);
    vbox->Add(pStatusBar, 0.5, wxEXPAND | wxALL, 0);

    SetSizer(vbox);

    //displays output console
    pOutputConsole = new CTextControl(NULL,wxT("Output Console"));
    pOutputConsole->Show(true);

    //to know graph memory/ai is allocated or not
    pGraphFrame = NULL;
    pAiFrame = NULL;

    Centre();
    //intro image is destroyed
//    pIntroImage->Close(true);
}

void CBasicMenubar::setstate(wxIdleEvent& event)
{
    if(glPane == NULL)
        return;

    if(this->IsActive())
        glPane->CanSetFocus(true);
    else
        glPane->CanSetFocus(false);
}

void CBasicMenubar::OnNew(wxCommandEvent& WXUNUSED(event))
{
    if(treeviewModelHierarchy)
        CTreeView::DeleteTreeItems();
    glPane->FreeDataStructureMemory();          //free already created memory
    glPane->Update();
}

void CBasicMenubar::OnOpen(wxCommandEvent& WXUNUSED(event))
{
    int iCheck = 0;
    wxFileDialog openFileDialog(this, _("Open CAE file"), "", "",
                                "All Files (*.op2;*.stl)|*.op2;*.stl|Op2 Files (*.op2)|*.op2|Stl Files(*.stl)|*.stl", wxFD_OPEN|wxFD_FILE_MUST_EXIST);

    if (openFileDialog.ShowModal() == wxID_CANCEL)
    {
        return;     // the user changed idea...
    }


    // proceed loading the file chosen by the user;
    // this can be done with e.g. wxWidgets input streams:
    glPane->SetSTLFilePath(openFileDialog.GetDirectory());
    glPane->SetSTLFileName(openFileDialog.GetFilename());
    glPane->FreeDataStructureMemory();          //free already created memory
    glPane->SetDataStructureRootKeys();         //set roots to null
//    DisplayLoadingWindow();
    glPane->Read_and_Store_Files();    //read file and store in datastructure
//    DeleteLoadingWindow();
//    if (!input_stream.IsOk())
//    if (iCheck==0)
//    {
//        wxLogError("Cannot Open file '%s'.", openFileDialog.GetPath());
//        return;
//    }
}

void CBasicMenubar::DisplayLoadingWindow()
{
    //shows please wait message
//    pLoadMessage = new CLoadingWindow(this,wxT("Loading.."));
//    pLoadMessage->Show(true);
}

void CBasicMenubar::DeleteLoadingWindow()
{
//    pLoadMessage->Close(true);
}

void CBasicMenubar::ExportNodeCoordinates(wxCommandEvent& event)
{
    glPane->ExportNodeCoordinates();
}

void CBasicMenubar::ExportElementConnectivity(wxCommandEvent& event)
{
    glPane->ExportElementConnectivity();
}

void CBasicMenubar::ExportNodalOutputDisplacement(wxCommandEvent& event)
{
    glPane->ExportNodalOutputDisplacement();
}

void CBasicMenubar::ExportElementStresstStrainResults(wxCommandEvent& event)
{
    glPane->ExportElementStressStrainResults();
}

void CBasicMenubar::ExportNodalAppliedLoads(wxCommandEvent& event)
{
    glPane->ExportNodalAppliedLoads();
}
void CBasicMenubar::ExportOutputStrainEnergy(wxCommandEvent& event)
{
    glPane->ExportOutputStrainEnergy();
}

void CBasicMenubar::ExportOutputKineticEnergy(wxCommandEvent& event)
{
    glPane->ExportOutputKineticEnergy();
}

void CBasicMenubar::ExportOutputEnergyLoss(wxCommandEvent& event)
{
    glPane->ExportOutputEnergyLoss();
}

void CBasicMenubar::ExportConstraintForces(wxCommandEvent& event)
{
    glPane->ExportConstraintForces();
}

void CBasicMenubar::ExportAll(wxCommandEvent& event)
{
    glPane->ExportAll();
}

void CBasicMenubar::Display_StressX(wxCommandEvent& event)
{
    glPane->Getrbg_with_StressMinMax(1);
}

void CBasicMenubar::Display_StressY(wxCommandEvent& event)
{
    glPane->Getrbg_with_StressMinMax(2);
}

void CBasicMenubar::Display_StressZ(wxCommandEvent& event)
{
    glPane->Getrbg_with_StressMinMax(3);
}

void CBasicMenubar::Display_StressXY(wxCommandEvent& event)
{
    glPane->Getrbg_with_StressMinMax(4);
}

void CBasicMenubar::Display_StressYZ(wxCommandEvent& event)
{
    glPane->Getrbg_with_StressMinMax(5);
}

void CBasicMenubar::Display_StressXZ(wxCommandEvent& event)
{
    glPane->Getrbg_with_StressMinMax(6);
}

void CBasicMenubar::OnGraphQuit(wxCommandEvent& WXUNUSED(event))
{
    //pGraph = NULL;
}

void CBasicMenubar::SetAutorotation(wxCommandEvent& event)
{
    BasicGLPane::SetAutoRotation();
}

void CBasicMenubar::UnsetAutorotation(wxCommandEvent& event)
{
    BasicGLPane::UnsetAutoRotation();
}

void CBasicMenubar::SetResetView(wxCommandEvent& event)
{
    glPane->ResetView();
}

void CBasicMenubar:: SetFitView(wxCommandEvent& event)
{
    glPane->FitView();
}

void CBasicMenubar:: SetLeftView(wxCommandEvent& event)
{
    glPane->LeftView();
}

void CBasicMenubar:: SetRightView(wxCommandEvent& event)
{
    glPane->RightView();
}

void CBasicMenubar:: SetTopView(wxCommandEvent& event)
{
    glPane->TopView();
}

void CBasicMenubar:: SetBottomView(wxCommandEvent& event)
{
    glPane->BottomView();
}

void CBasicMenubar:: SetFrontView(wxCommandEvent& event)
{
    glPane->FrontView();
}

void CBasicMenubar::SetBackView(wxCommandEvent& event)
{
    glPane->BackView();
}

void CBasicMenubar::ShowAboutFrame(wxCommandEvent& event)
{
    //about window
    pAbout = new CAboutWindow(this,wxT("About Eethal"));
    pAbout->Show(true);
}

void CBasicMenubar::LaunchPlotter(wxCommandEvent& event)
{
    //deletes the old graph frame and creates a new one
    if(bGraphFramePresent) {
        delete pGraphFrame;
        pGraphFrame == NULL;
    }
    pGraphFrame = new CGraphFrame(wxT("Plotter"));
    pGraphFrame->SetAutoLayout(true);
    pGraphFrame->SetIcons(wxIconBundle("images/graph.png",wxBITMAP_TYPE_PNG));
    pGraphFrame->Show(true);
    //pGraphFrame->StoreElemptrInGraphFrame(glPane->oElementData);

    //pGraphFrame->Set_Tree_Values(glPane->GetElementInfo());
    //set glu callbacks
    int argc = 1;
    char *argv[1] = {(char*)"CmdLineArgumentInC"};
    //glutInit(&argc, argv);//glut init() must be called before any other glut function
    //glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    //glutInitWindowSize(690,490);
    //glutInitWindowPosition(0,0);
    //glutCreateWindow("SIMPLE OPENGL APPLICATION");
    //glutKeyboardFunc(g_keyboardfunc);
    //glutMouseFunc(g_Mousefunc);
    //g_Init();
    //glutDisplayFunc(g_Drawfunc);
    //glutReshapeFunc(g_Reshapefunc);
    //glutMainLoop();

    //pGraph->SetAutoLayout(true);
    //pGraph->SetIcons(wxIconBundle("images/logo/mainframe.png",wxBITMAP_TYPE_PNG));
    //pGraph->Show(true);
}

void CBasicMenubar::GraphFramePresent(bool bPresent)
{
    bGraphFramePresent = bPresent;
}

void CBasicMenubar::AiFramePresent(bool bPresent)
{
    bAiFramePresent = bPresent;
}

void CBasicMenubar::LaunchAi(wxCommandEvent& event)
{
    //deletes the old ai frame and creates a new one
    if(bAiFramePresent) {
        delete pAiFrame;
        pAiFrame == NULL;
    }
    pAiFrame = new AI(wxT("AI"));
    pAiFrame->SetAutoLayout(true);
    pAiFrame->Show(true);
}

/*Static Keyboard function*/
//void CBasicMenubar::g_keyboardfunc(unsigned char iKey,int iXvalue,int iYvalue)
//{
//    //pGraph->Keyboard_Method(iKey,iXvalue,iYvalue);
//}

/*Static Mouse function*/
//void CBasicMenubar::g_Mousefunc(int iBtn,int iState,int iXvalue,int iYvalue)
//{
//    //pGraph->Mouse_Method(iBtn,iState,iXvalue,iYvalue);
//}

/*Static Reshape function*/
//void CBasicMenubar::g_Reshapefunc(int iView_Width,int iView_Height)
//{
//    //pGraph->Reshape_method(iView_Width,iView_Height);
//}

/*Static init function*/
void CBasicMenubar::g_Init()
{
    //pGraph->init();
}

//Static Timer Function
void  CBasicMenubar::g_Timer(int iValue)
{
    if(iValue == 1)
        glutTimerFunc(140,g_Timer,iValue);
}

//Static Draw Function
//void CBasicMenubar::g_Drawfunc()
//{
//    //pGraph->Draw_Method();
//}

void CBasicMenubar::FreeCreatedMemory()
{
    //clear embedded items and other frames
    //delete the treeview pointer
    if(treeviewModelHierarchy)
    {
        delete treeviewModelHierarchy;
        treeviewModelHierarchy = NULL;
    }

    // Delete the opengl window
    // ஓபன்சிஎள் சாளரத்தை நீக்கவும்
    if(glPane)
    {
        glPane->FreeDataStructureMemory();       //delete memory (op2 data)
        delete glPane;
        glPane = NULL;
    }

    //viewer frame closes the output console with it
    if(pOutputConsole && pOutputConsole->isOutputConsolePresent())
    {
        pOutputConsole->SetOutputConsoleStatus(false);
        delete pOutputConsole;
        pOutputConsole = NULL;

    }

    //viewer frame closes the graph frame with it
    if(pGraphFrame && pGraphFrame->isGraphWindowPresent())
    {
        delete pGraphFrame;
        pGraphFrame = NULL;
    }

    //viewer frame closes the ai frame with it
    if(pAiFrame && pAiFrame->isAiWindowPresent())
    {
        delete pAiFrame;
        pAiFrame = NULL;
    }
}

// Called when File > Exit option is selected
void CBasicMenubar::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // Free memory created for embedded items and windows
    FreeCreatedMemory();

    BasicGLPane::UnsetAutoRotation();
    //crash ceck //pAiFrame->FreeDataStructureMemory();    //delete memory (bdf data)
                    //closes viewer frame
    //if(pOutputConsole && pOutputConsole->isOutputConsolePresent())
    //    pOutputConsole->Close(true);//viewer frame closes the output console with it

    //Close(true);
}

void CBasicMenubar::OnClose(wxCloseEvent& event)
{
    // Free memory created for embedded items and windows
    FreeCreatedMemory();

    BasicGLPane::UnsetAutoRotation();
    //crash ceck //pAiFrame->FreeDataStructureMemory();    //delete memory (bdf data)
                    //closes viewer frame
    //if(pOutputConsole && pOutputConsole->isOutputConsolePresent())
    //    pOutputConsole->Close(true);//viewer frame closes the output console with it

    //Close(true);
}

CBasicMenubar::~CBasicMenubar()
{
    // delete if any embedded windows and related memories are there
    if(treeviewModelHierarchy || glPane || pOutputConsole || pGraphFrame || pAiFrame)
        FreeCreatedMemory();
}
