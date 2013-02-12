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

#include "ListView.h"

BEGIN_EVENT_TABLE(ListView, wxListView)

    EVT_PAINT(                              OnPaint)
    EVT_SIZE(                               OnSize)

    EVT_LIST_COL_BEGIN_DRAG(    wxID_ANY,   OnColumnBeginDrag )

END_EVENT_TABLE()

ListView::ListView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
        long style, const wxValidator& validator, const wxString& name )
    : wxListView(parent, id, pos, size, style, validator, name)
{
    m_allowColumnResizing = true;
}

void ListView::SetAllowColumnResizing(bool allowColumnResizing)
{
    m_allowColumnResizing = allowColumnResizing;
}

void ListView::AutoSizeColumns()
{

    wxSize size = GetClientSize();
    int width = size.x - GetColumnWidth(0);

    if (GetColumnWidth(1) != width)
    {
    
        SetColumnWidth(1, width);

        // This is a bit hacky, but toggling on and off the scroll bar style
        // will remove any scroll bar that was shown when we resized (even
        // though resizing the column should make the scroll bar unnecessary).
        ToggleWindowStyle(wxHSCROLL);
        ToggleWindowStyle(wxHSCROLL);

    }

}

void ListView::OnSize(wxSizeEvent& event)
{
    AutoSizeColumns();
    event.Skip();
}

void ListView::OnPaint(wxPaintEvent& event)
{

    // Resize the columns if necessary. We do this in the paint since
    // we can't trap all of the events with column resizing.
    AutoSizeColumns();

    // Do regular paint processing.
    event.Skip();

}

void ListView::OnColumnBeginDrag(wxListEvent& event)
{
    if (!m_allowColumnResizing)
    {
        // Don't allow resizing.
        event.Veto();
    }
    else
    {
        event.Skip();
    }
}
