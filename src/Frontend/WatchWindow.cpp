/*

Decoda
Copyright (C) 2007-2013 Unknown Worlds Entertainment, Inc. 

This file is part of Decoda.

Decoda is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Decoda is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Decoda.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "WatchWindow.h"
#include "WatchDropTarget.h"
#include "Tokenizer.h"

BEGIN_EVENT_TABLE(WatchWindow, WatchCtrl)
    EVT_TREE_BEGIN_LABEL_EDIT(wxID_ANY,     WatchWindow::OnBeginLabelEdit)
    EVT_TREE_END_LABEL_EDIT(wxID_ANY,       WatchWindow::OnEndLabelEdit)
    EVT_TREE_ITEM_ACTIVATED(wxID_ANY,       WatchWindow::OnItemSelected)
    EVT_TREE_KEY_DOWN(wxID_ANY,             WatchWindow::OnKeyDown)
END_EVENT_TABLE()

WatchWindow::WatchWindow(wxWindow* parent, wxWindowID winid)
    : WatchCtrl(parent, winid, wxDefaultPosition, wxDefaultSize)
{

    m_root = AddRoot(_T("Root"));
    SetItemText(m_root, 1, _T("Root"));
    
    SetDropTarget(new WatchDropTarget(this));
    
    CreateEmptySlotIfNeeded();

    m_editing = false;

}

void WatchWindow::UpdateItems()
{

    wxTreeItemIdValue cookie;
    wxTreeItemId item = GetFirstChild(m_root, cookie);

    while (item.IsOk())
    {
        UpdateItem(item);        
        item = GetNextSibling(item);
    }

}

void WatchWindow::AddWatch(const wxString& expression)
{

    wxString temp = expression;
    temp.Trim().Trim(false);

    if (!temp.empty())
    {

        wxTreeItemIdValue cookie;
        wxTreeItemId lastItem = GetLastChild(m_root, cookie);

        // The last item should be our blank item.
        assert(lastItem.IsOk());
        lastItem = GetPrevSibling(lastItem);

        wxTreeItemId item;

        if (lastItem.IsOk())
        {
            item = InsertItem(m_root, lastItem, temp);
        }
        else
        {
            item = PrependItem(m_root, temp);
        }

        UpdateItem(item);

    }

}

void WatchWindow::OnKeyDown(wxTreeEvent& event)
{

    if (event.GetKeyCode() == WXK_DELETE ||
        event.GetKeyCode() == WXK_BACK)
    {
        wxTreeItemId item = GetSelection();

        if (item.IsOk() && GetItemParent(item) == m_root)
        {

            wxTreeItemId next = GetNextSibling(item);

            Delete(item);
            CreateEmptySlotIfNeeded();

            // Select the next item.

            if (!next.IsOk())
            {
                wxTreeItemIdValue cookie;
                next = GetLastChild(GetRootItem(), cookie);
            }

            SelectItem(next);

        }
    }
    else
    {

        // If we're not currently editing a field, begin editing. This
        // eliminates the need to double click to begin editing.

        int code = event.GetKeyCode();

        if (!m_editing && code < 256 && (isgraph(code) || IsSpace(code)))
        {

            // Get the currently selected item in the list.

            wxTreeItemId item = GetSelection();
            
            if (item.IsOk())
            {
                if (IsSpace(code))
                {
                    EditLabel(item, "");
                }
                else
                {
                    EditLabel(item, wxString(static_cast<char>(code)));
                }

                event.Skip(false);
            }

        }
        else
        {
            event.Skip(true);
        }


    }

}

void WatchWindow::OnBeginLabelEdit(wxTreeEvent& event)
{
    
    if (GetItemParent(event.GetItem()) == m_root)
    {
        m_editing = true;
    }    
    else
    {
        event.Veto();
    }
        
}

void WatchWindow::OnEndLabelEdit(wxTreeEvent& event)
{
    if (!event.IsEditCancelled())
    {

        wxTreeItemId item = event.GetItem();
        wxTreeItemId next = GetNextSibling(item);

        wxString expression = event.GetLabel();
        expression.Trim().Trim(false);

        if (expression.empty())
        {
            Delete(item);
            event.Veto();
        }
        else
        {
            SetItemText(item, expression);
            UpdateItem(item);
        }

        CreateEmptySlotIfNeeded();

        // Select the next item.

        if (!next.IsOk())
        {
            wxTreeItemIdValue cookie;
            next = GetLastChild(GetRootItem(), cookie);
        }

        SelectItem(next);

    }

    m_editing = false;
}

void WatchWindow::OnItemSelected(wxTreeEvent& event)
{
    // Initiate the label editing.
    EditLabel(event.GetItem());
    event.Skip();
}

void WatchWindow::CreateEmptySlotIfNeeded()
{

    bool needsEmptySlot = true;

    int numItems = GetChildrenCount(m_root, false);
    
    if (numItems > 0)
    {
        wxTreeItemIdValue cookie;
        wxTreeItemId lastItem = GetLastChild(m_root, cookie);
        needsEmptySlot = !GetItemText(lastItem).empty();
    }

    if (needsEmptySlot)
    {
        wxTreeItemId item = AppendItem(m_root, _T(""));
    }

}