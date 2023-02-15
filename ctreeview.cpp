#include "ctreeview.h"
#include "cnode.h"
#include "celement.h"
#include <wx/colour.h>
#include <wx/tracker.h>
using namespace std;

wxTreeCtrl* CTreeView::treecontrol = NULL;

CTreeView::CTreeView(const wxString& title,wxWindow *parent) : wxPanel(parent,wxID_ANY,wxDefaultPosition,wxDefaultSize,wxTAB_TRAVERSAL | wxNO_BORDER,wxPanelNameStr)
{
//    //manager dialog box
    treecontrol = new wxTreeCtrl(this, wxID_ANY, wxPoint(0,0), wxSize(260,610), wxTR_HAS_BUTTONS | wxTR_SINGLE);
}

void CTreeView::SetTreeComponents(wxString m_wxstrFileName,CElement &oElementData)
{
    //removes already existing tree components
    treecontrol->DeleteAllItems();

    //assigns images for root and child components
    wxImageList* imageList = new wxImageList(20, 20);
    wxImage::AddHandler( new wxPNGHandler );

    wxBitmap bitmapRoot(wxT("images/treeview/root.png"), wxBITMAP_TYPE_PNG);
    wxBitmap bitmapChildren(wxT("images/treeview/children.png"), wxBITMAP_TYPE_PNG);
    wxColour maskColour;
    imageList->Add(bitmapRoot,maskColour);
    imageList->Add(bitmapChildren,maskColour);
    treecontrol->AssignImageList(imageList);

    //A wxTreeItemId is not meant to be constructed explicitly by the user; only those returned by the wxTreeCtrl functions should be used.
    wxTreeItemId rootId = treecontrol->AddRoot(m_wxstrFileName, 0, 0, new wxTreeItemData());
    //element names are added as children
    for(int iIter = 0; iIter < oElementData.NumberofElementTypes(); iIter++)
    {
        if(oElementData.ElementName(iIter) != NULL)
            wxTreeItemId itemId1 = treecontrol->AppendItem(rootId, oElementData.ElementName(iIter), 1, 1, new wxTreeItemData());
        else
            wxTreeItemId itemId1 = treecontrol->AppendItem(rootId, wxT("Unknown"), 1, 1, new wxTreeItemData());
    }

    //expanded tree view is displayed
    treecontrol->ExpandAll();
}

void CTreeView::DeleteTreeItems()
{
    //removes already existing tree components
    treecontrol->DeleteAllItems();
}

CTreeView::~CTreeView()
{
    if(treecontrol) {
        delete treecontrol;
        treecontrol = NULL;
    }
}
