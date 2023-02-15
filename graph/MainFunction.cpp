//#include "node.h"
//#include "RenderNode.h"
//#include<windows.h>
//#include"GL/gl.h"
//#include"GL/glu.h"
//#include"GL/glut.h"
//#include<iostream>
//using namespace std;
//
//
//void  g_Timer(int iValue); //Forward declaration
//
///*Global object for class CRenderNode*/
//CRenderNode o_g_Call_glut_Methods;
//
///*Global Keyboard function*/
//void g_keyboardfunc(unsigned char iKey,int iXvalue,int iYvalue)
//{
//    o_g_Call_glut_Methods.Keyboard_Method(iKey,iXvalue,iYvalue);
//}
//
///*Global Mouse function*/
//void g_Mousefunc(int iBtn,int iState,int iXvalue,int iYvalue)
//{
//    o_g_Call_glut_Methods.Mouse_Method(iBtn,iState,iXvalue,iYvalue);
//}
//
///*Global Reshape function*/
//void g_Reshapefunc(int iView_Width,int iView_Height)
//{
//    o_g_Call_glut_Methods.Reshape_method(iView_Width,iView_Height);
//}
//
///*Global init function*/
//void g_Init()
//{
//    o_g_Call_glut_Methods.init();
//}
//
////Global Timer Function
//void  g_Timer(int iValue)
//{
//    if(iValue == 1)
//    glutTimerFunc(140,g_Timer,iValue);
//}
//
////Global Draw Function
//void g_Drawfunc()
//{
//    o_g_Call_glut_Methods.Draw_Method();
//}
//
//int main(int argc,char** argv)
//{
////    Cnode oCheck_Tree;
////    oCheck_Tree.Set_RootKey(55);
////    oCheck_Tree.Insert_RootNode_Attributes(0.001,75,0);
////    oCheck_Tree.Insert_Node_Attributes(3,0.001,75,0);
////    oCheck_Tree.Insert_Node_Attributes(4,0.001,75,0);
////    oCheck_Tree.Insert_Node_Attributes(2,0.001,75,0);
////    oCheck_Tree.Insert_Node_Attributes(1,0.001,75,0);
////    oCheck_Tree.Insert_Node_Attributes(30,0.001,75,0);
////    oCheck_Tree.Insert_Node_Attributes(31,0.001,75,0);
////
////    cout<<"No of Tree Elements :"<<oCheck_Tree.Count_No_of_Nodes()<<"\n";
////
////    oCheck_Tree.Delete_Node_At_ID(1);
////    oCheck_Tree.Delete_Node_At_ID(27);
////
////    oCheck_Tree.~Cnode();
////
////    cout<<"No of Tree Elements :"<<oCheck_Tree.Count_No_of_Nodes()<<"\n";
//
//    oCheck_Tree.Display_Node_Keys_Inorder();
//
//
//
//    glutInit(&argc,argv);//glut init() must be called before any other glut function
//    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
//    glutInitWindowSize(690,490);
//    glutInitWindowPosition(0,0);
//    glutCreateWindow("SIMPLE OPENGL APPLICATION");
//    glutKeyboardFunc(g_keyboardfunc);
//    glutMouseFunc(g_Mousefunc);
//    g_Init();
//    glutDisplayFunc(g_Drawfunc);
//    glutReshapeFunc(g_Reshapefunc);
//    glutMainLoop();
//   	return 0;
//}
