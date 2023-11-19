#include "cbasictoolbar.h"
#include "basicglpane.h"
#include "ctextcontrol.h"
#include "caboutwindow.h"
#include "cloadingwindow.h"
#include "ctreeview.h"
#include <wx/wx.h>
#include <wx/menu.h>
#include <wx/combobox.h>
#include <wx/Toolbar.h>
#include <wx/listctrl.h>
#include <wx/treectrl.h>
#include <wx/statusbr.h>
#include "basicglpane.h"
//#include "graph/node.h"
//#include "graph/RenderNode.h"
#include "graphframe.h"
#include "ai.h"
//website showing images of menu,menubar,menuitem etc
//http://www.google.co.in/imgres?imgurl=http://cdn.computerhope.com/menu.gif&imgrefurl=http://www.computerhope.com/jargon/m/menubar.htm&usg=__tHugvnpy5UjiBCBm0m-xbvtjpsw=&h=289&w=391&sz=8&hl=en&start=18&zoom=1&tbnid=pE-fRCh3wVfTKM:&tbnh=91&tbnw=123&ei=-DwSUfLFMc2NrgeCj4EY&prev=/search%3Fq%3Dmenu,%2Bmenu%2Bitem,menubar%26hl%3Den%26tbas%3D0%26tbm%3Disch&itbs=1
class CBasicMenubar : public wxFrame
{
public:
    CBasicMenubar(const wxString& title);
    //event handled methods
    void setstate(wxIdleEvent &event);
    //data export methods
    void ExportNodeCoordinates(wxCommandEvent& event);
    void ExportElementConnectivity(wxCommandEvent& event);
    void ExportNodalOutputDisplacement(wxCommandEvent& event);
    void ExportElementStresstStrainResults(wxCommandEvent& event);
    void ExportNodalAppliedLoads(wxCommandEvent& event);
    void ExportOutputStrainEnergy(wxCommandEvent& event);
    void ExportOutputKineticEnergy(wxCommandEvent& event);
    void ExportOutputEnergyLoss(wxCommandEvent& event);
    void ExportConstraintForces(wxCommandEvent& event);
    void ExportAll(wxCommandEvent& event);
    //results display methods
    void Display_StressX(wxCommandEvent& event);
    void Display_StressY(wxCommandEvent& event);
    void Display_StressZ(wxCommandEvent& event);
    void Display_StressXY(wxCommandEvent& event);
    void Display_StressYZ(wxCommandEvent& event);
    void Display_StressXZ(wxCommandEvent& event);
    //ai methods

    //friend
    friend void GetElemInfoFromGlpane(BasicGLPane* &);

    void OnNew(wxCommandEvent& WXUNUSED(event));
    void OnOpen(wxCommandEvent& WXUNUSED(event));
    void OnSave(wxCommandEvent& WXUNUSED(event));

    void SetAutorotation(wxCommandEvent& event);
    void UnsetAutorotation(wxCommandEvent& event);
    void SetResetView(wxCommandEvent& event);
    void SetFitView(wxCommandEvent& event);
    void SetLeftView(wxCommandEvent& event);
    void SetRightView(wxCommandEvent& event);
    void SetTopView(wxCommandEvent& event);
    void SetBottomView(wxCommandEvent& event);
    void SetFrontView(wxCommandEvent& event);
    void SetBackView(wxCommandEvent& event);

    void ShowAboutFrame(wxCommandEvent& event);
    void LaunchPlotter(wxCommandEvent& event);
    void LaunchAi(wxCommandEvent& event);
    void DisplayLoadingWindow();
    void DeleteLoadingWindow();

    static void g_keyboardfunc(unsigned char iKey,int iXvalue,int iYvalue);
    static void g_Mousefunc(int iBtn,int iState,int iXvalue,int iYvalue);
    static void g_Reshapefunc(int iView_Width,int iView_Height);
    static void g_Init();
    static void g_Timer(int iValue);
    static void g_Drawfunc();

    // clear all memory and close the windows
    void FreeCreatedMemory();
    void OnQuit(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);
    void OnGraphQuit(wxCommandEvent& WXUNUSED(event));
    ~CBasicMenubar();

    DECLARE_EVENT_TABLE()
private:
    //please wait window
//    CLoadingWindow* pLoadMessage;
    CTreeView* treeviewModelHierarchy;
    //about window
    CAboutWindow* pAbout;
    wxMenuBar* menubarBasic;
    wxToolBar* toolbar1;
    wxToolBar* toolbar2;
    //file menu pointer instances
    wxMenu* menuFile;
    wxMenuItem* menuitemNew;
    wxMenuItem* menuitemOpen;
    wxMenuItem* menuitemSave;
    wxMenuItem* menuitemImport;
    wxMenu* menuImport;
    wxMenuItem* quit;
    //view menu pointer instances
    wxMenu* menuView;
    wxMenu* menuReset,*menuFit,*menuLeft,*menuRight,*menuTop,*menuBottom,*menuFront,*menuBack,*menuAutoRotation;

    //edit menu pointer instances
    wxMenu* menuEdit;
    //export menu pointer instances
    wxMenu* menuExport;
    wxMenu* menuMeshData;
    wxMenu* menuResultData;
    //result menu pointer instances
    wxMenu* menuResult;
    wxMenu* menuNodalResult, *menuElemResult;
    wxMenu* submenuNodalDisplacementComponents;
    wxMenu* submenuElemStressComponents;
    wxMenu* submenuElemTotalMechanicalStrain;
    wxMenu* submenuElemElasticStrain;
    wxMenu* submenuElemPlasticStrain;
    wxMenu* submenuElemCreepStrain;
    wxMenu* submenuElemThermalStrain;
    wxMenu* submenuElemTotalMechanicalandThermalStrain;
    //ai menu pointer instances
    wxMenu* menuAI;
    wxMenuItem* menuitemFeedback;
    //help menu pointer instances
    wxMenu* menuHelp;
    wxMenuItem* menuitemAbout;
    //plotter options
//    wxComboBox* pComboBox1, *pComboBox2;
    //main frame status bar
    wxStatusBar *pStatusBar;
public:
    //opengl graphic pointer instance
    static BasicGLPane* glPane;

    //wxtext control pointer instance
    static CTextControl* pOutputConsole;

    //graph frame
    CGraphFrame *pGraphFrame;
    static bool bGraphFramePresent, bAiFramePresent;
    static void GraphFramePresent(bool bPresent);
    static void AiFramePresent(bool bPresent);

    //ai frame
    AI *pAiFrame;

};
