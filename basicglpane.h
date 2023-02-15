#ifndef _glpane_
#define _glpane_
#include "cnode.h"
#include "celement.h"
#include "ctreeview.h"
#include <wx/wx.h>
#include <wx/menu.h>
#include <wx/glcanvas.h>
#include <wx/string.h>
#include <wx/progdlg.h>
#include <list>
#include <vector>

struct sDeerHead
{
    float fFaceNormal[3];
    float fFirstVertex[3], fSecondVertex[3], fThirdVertex[3];
};

class BasicGLPane : public wxGLCanvas
{
private:
    //data structure for nodes and elements
    Cnode oNodeData;
    CElement oElementData;

    wxGLContext *m_glContext;
    //stores deer head for room
    vector<sDeerHead> vecDeerHead;//vector is enough for deer head, as it is a small data and
    //using Cnode, Celement is a overkill and vector keeps the code clean.

public:
    BasicGLPane(wxFrame* parent, int* args);
    virtual ~BasicGLPane();

    //read and store cae model and result in datastructure
    void SetDataStructureRootKeys();
    void Read_and_Store_Files();
    void FindModelxyz_MinMaxInorder(Snode* &pInorderTraverse);
    void CalculateCoordinatesforDisplay_Inorder(Snode* &pInorderTraverse);
    void ConfigureModelCoordinatesforDisplay(Cnode& oNodeData);
    void FreeDataStructureMemory();
    void DrawElementsInorder(SBinaryTreeNode* pInorderTraverse, int iSubCase);
    void DrawNodesInorder(Snode* pInorderTraverse);

    //get node and element data
    Cnode GetNodeInfo(Cnode oNodeData);
    CElement* GetElementInfo();
    void WriteElementInfo();

    void ZoomIn();
    void ZoomOut();
    void resized(wxSizeEvent& evt);
    int getWidth();
    int getHeight();
    void Update();
    static void Update_BoundBox_Model();
    void SetSTLFileName(wxString wxstrFileName);
    void SetSTLFilePath(wxString wxstrFilePath);
    static void SetAutoRotation();
    static void UnsetAutoRotation();
    static bool GetAutoRotation();

    void Draw_ModelContainer();
    void Display_ScreenCentre();
    void Draw_Axis();
    void ReadStlFile();
    void Read_ModelCentre();
    void Draw_Room();
    void Draw_Model();
    void Draw_ModelBoundingBox();
    void Display_ModelCentre();

    void Render(wxPaintEvent& evt);
    void prepare3DViewport(int topleft_x, int topleft_y, int bottomrigth_x, int bottomrigth_y);
    void prepare2DViewport(int topleft_x, int topleft_y, int bottomrigth_x, int bottomrigth_y);

    //math
    void MatrixMultiplication_4x4(double frgA[16],double frgB[16]);

    // events
    void mouseMoved(wxMouseEvent& event);
    //rotation methods
    void Mouse2dVertex_to_TrackballHemispherePoint(double fMouse3dCord[3],wxMouseEvent& event);
    float TrackballVector_DirectionLength(double fMouse3dCord[3]);
    float AngleAxisPair(double fMouse3dCord[3],double fRotAxis[3],float fVecDirectionLength);
    void ConvertAngleAxisPair_to_Quarternion(double QuaternionMatrix[16],float fRotAngle,double fRotAxis[3]);
    void RotateModelwithMouse(wxMouseEvent& event);//....//

    void PanModelwithMouse(wxMouseEvent& event);
    void leftClick(wxMouseEvent& event);
    void mouseWheelMoved(wxMouseEvent& event);
    void mouseReleased(wxMouseEvent& event);
    void rightClick(wxMouseEvent& event);
    void MouseLeaveWindow(wxMouseEvent& event);
    void MouseEnteredWindow(wxMouseEvent& event);
    void keyPressed(wxKeyEvent& event);
    void keyReleased(wxKeyEvent& event);

    //view methods
    void ResetView();
    void FitView();
    void LeftView();
    void RightView();
    void TopView();
    void BottomView();
    void FrontView();
    void BackView();

    //data export methods
    void ExportNodeCoordinates();
    void ExportElementConnectivity();
    void ExportNodalOutputDisplacement();
    void ExportElementStressStrainResults();
    void ExportNodalAppliedLoads();
    void ExportOutputStrainEnergy();
    void ExportOutputKineticEnergy();
    void ExportOutputEnergyLoss();
    void ExportConstraintForces();
    void ExportAll();
    void ExportGraph_StrsStrnXX();
    void ExportGraph_StrsStrnYY();
    void ExportGraph_StrsStrnZZ();
    void ExportGraph_StrsStrnXY();
    void ExportGraph_StrsStrnYZ();
    void ExportGraph_StrsStrnXZ();

    void ExportGraph_StrsStrnShearAngle();
    void ExportGraph_StrsStrn1stPrincipal();
    void ExportGraph_StrsStrn2ndPrincipal();
    void ExportGraph_StrsStrn3rdPrincipal();
    void ExportGraph_StrsStrnMajorPrincipal();
    void ExportGraph_StrsStrnMinorPrincipal();
    void ExportGraph_StrsStrnVmises();

    //set treeview(wxtreectrl) components
    void SetTreeViewData();

    //set focus
    void CanSetFocus(bool bSetFocus);

    //display results
    void Reserve_minmax_Vectors();
    void Getrbg_with_StressMinMax(int iStressType);
    void FindStressX_MinMaxInorder(SBinaryTreeNode* pInorderTraverse, int iSubCase);
    void FindStressY_MinMaxInorder(SBinaryTreeNode* pInorderTraverse, int iSubCase);
    void FindStressZ_MinMaxInorder(SBinaryTreeNode* pInorderTraverse, int iSubCase);
    void FindStressXY_MinMaxInorder(SBinaryTreeNode* pInorderTraverse, int iSubCase);
    void FindStressYZ_MinMaxInorder(SBinaryTreeNode* pInorderTraverse, int iSubCase);
    void FindStressXZ_MinMaxInorder(SBinaryTreeNode* pInorderTraverse, int iSubCase);
    void Calculate_StressX_rgb_Inorder(SBinaryTreeNode* pInorderTraverse, int iSubCase);
    void Calculate_StressY_rgb_Inorder(SBinaryTreeNode* pInorderTraverse, int iSubCase);
    void Calculate_StressZ_rgb_Inorder(SBinaryTreeNode* pInorderTraverse, int iSubCase);
    void Calculate_StressXY_rgb_Inorder(SBinaryTreeNode* pInorderTraverse, int iSubCase);
    void Calculate_StressYZ_rgb_Inorder(SBinaryTreeNode* pInorderTraverse, int iSubCase);
    void Calculate_StressXZ_rgb_Inorder(SBinaryTreeNode* pInorderTraverse, int iSubCase);
    void DrawLegend();
    void DrawMessage();
    float *ComputeNormal(float *fVertex1, float *fVertex2, float *fVertex3);

    DECLARE_EVENT_TABLE()
private:
    bool bAssign;
    double mdQuatX,mdQuatY,mdQuatZ,mdQuatW;
    double mdAutoQuatX,mdAutoQuatY,mdAutoQuatZ,mdAutoQuatW;
    bool mbDataStructurePopulated, mbDeerHeadPopulated;
    float mfScaledValue;
    static bool mbAutorotation;
    static double mdrgQuaternionMatrix[16];
    static double mdrgAutorotationMatrix[16];
    static double mdrgTranslationMatrix[16];
    static GLdouble mdrgScaleViewMatrix[16];
    static double mdrgProductMatrix[16];
    //std vector
    std::vector<float> mvecXCoord;
    std::vector<float> mvecYCoord;
    std::vector<float> mvecZCoord;
    std::vector<float> mvecLength;
    std::vector<float> mvecfXCoordinate;
    std::vector<float> mvecfYCoordinate;
    std::vector<float> mvecfZCoordinate;
    //normal vectors
    std::vector<float> mvecNormalX;
    std::vector<float> mvecNormalY;
    std::vector<float> mvecNormalZ;
    float m_fLength,m_fHeight,m_fDepth,m_fMaxModelDim;
    static int m_InsertOnce;
    float m_fModelMaxAxisValue;
    float m_fWindWidth,m_fWindHeight;
    float m_fZoomFactor;
    float m_fModelCentre_XMax,m_fModelCentre_YMax,m_fModelCentre_ZMax;
    float m_fModelCentre_XMin,m_fModelCentre_YMin,m_fModelCentre_ZMin;
    float m_fModelCentre_X,m_fModelCentre_Y,m_fModelCentre_Z;
    static float m_fMouse3dCord[3];
    float m_fMouseX,m_fMouseY;
    static int m_iBoundBox;       //updates the bound box for newly loaded model
    static bool mbUpdateModelOnce;//updates the gui with newly loaded model
    static wxString m_wxstrFilePath;
    static wxString m_wxstrFileName;
    //progress bar displayed while loading model from file
    wxProgressDialog* pProgressBar;
    //set focus to BasicGLPane frame,according to active frame
    bool mbSetFocus, mbCalcOnce, mbread_deerhead;
    //for legend
    float m_fMinTmp, m_fMaxTmp, m_fTmp;
    string mstrResultId; /*1,2..nodes,11,22,..stress,111,222,..strain*/
    int miSubcaseId;
};
#endif
