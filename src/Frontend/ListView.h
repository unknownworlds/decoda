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

#ifndef LIST_VIEW_H
#define LIST_VIEW_H

#include <wx/wx.h>
#include <wx/listctrl.h>

/**
 * List view window. This is an "advanced" version of the wxListView class that
 * automatically handles resizing the columns to fill the available space and
 * optionally prevents the user from being able to resize columns.
 */
class ListView : public wxListView
{

    DECLARE_EVENT_TABLE()

public:

    /**
     * Constructor.
     */
    ListView(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
        long style = wxLC_ICON, const wxValidator& validator = wxDefaultValidator, const wxString& name = wxListCtrlNameStr);

    /**
     * Sets whether or not the user is allowed to resize the columns. This
     * is on by default.
     */
    void SetAllowColumnResizing(bool allowColumnResizing);

    /**
     * Called when the user starts dragging a column to resize it.
     */
    void OnColumnBeginDrag(wxListEvent& event);

    /**
     * Called when the window changes size.
     */
    void OnSize(wxSizeEvent& event);

    /**
     * Called when the window needs to be painted.
     */
    void OnPaint(wxPaintEvent& event);

private:

    /**
     * Handles resizing the columns so that they fill the width of the window.
     */
    void AutoSizeColumns();

private:
    
    bool    m_allowColumnResizing;

};

#endif