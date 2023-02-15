#ifndef GRAPHGLPANE_H
#define GRAPHGLPANE_H

#include <wx/wx.h>
#include <wx/combobox.h>
#include <wx/wxchar.h>
#include "graphframe.h"
#include "graph/RenderNode.h"

class CGraphFrame : public wxFrame
{
    public:
        /** Default constructor */
        CGraphFrame(const wxString& title);
        void setstate(wxIdleEvent& event);
        void Plot(wxCommandEvent& event);
        bool isGraphWindowPresent();
        /** Default destructor */
        ~CGraphFrame();

    DECLARE_EVENT_TABLE()
    protected:
    private:
        //tmp pointer for nodes and elements
        Cnode *mNodeData;
        CElement *mElemData;
        wxComboBox* pComboBox1, *pComboBox2;
        bool bGraphWindowPresent;
    public:
        //opengl graphic pointer instance in graph frame
        static CRenderNode *pGraphGL;
};

#endif // GRAPHGLPANE_H
