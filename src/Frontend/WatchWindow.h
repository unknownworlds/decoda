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

#ifndef WATCH_WINDOW_H
#define WATCH_WINDOW_H

#include <wx/wx.h>
#include "WatchCtrl.h"

/**
 * Window window class.
 */
class WatchWindow : public WatchCtrl
{

public:

    /**
     * Constructor.
     */
    WatchWindow(wxWindow* window, wxWindowID winid);

    /**
     * Updates the values of all the expressions being watched.
     */
    void UpdateItems();

    /**
     * Adds a new expression to the watch window.
     */
    void AddWatch(const wxString& expression);

    /**
     * Called when the user presses a key.
     */
    void OnKeyDown(wxTreeEvent& event);

    /**
     * Called when the user begins editing a label.
     */
    void OnBeginLabelEdit(wxTreeEvent& event);

    /**
     * Called when the user finishes editing a label.
     */
    void OnEndLabelEdit(wxTreeEvent& event);

    /**
     * Called when the user selects an item.
     */
    void OnItemSelected(wxTreeEvent& event);

    DECLARE_EVENT_TABLE()

private:

    /**
     * Creates a blank entry at the end of the list (for easily adding new items)
     * if there isn't one there already.
     */
    void CreateEmptySlotIfNeeded();

private:

    bool            m_editing;
    wxTreeItemId    m_root;

};

#endif