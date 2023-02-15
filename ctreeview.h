#ifndef CTREEVIEW_H
#define CTREEVIEW_H
#include "cnode.h"
#include "celement.h"
#include <wx/wx.h>
#include <wx/treectrl.h>

class CTreeView : public wxPanel
{
public:
    CTreeView(const wxString& title,wxWindow *parent);
    static void SetTreeComponents(wxString m_wxstrFileName,CElement &oElementData);
    static void DeleteTreeItems();
    ~CTreeView();
protected:
private:
    //tree control
    static wxTreeCtrl* treecontrol;
};

#endif // CTREEVIEW_H
