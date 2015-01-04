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

#ifndef LIST_WINDOW_H
#define LIST_WINDOW_H

#include <wx/wx.h>

#include "ListView.h"
#include "FontColorSettings.h"

//
// Forward declarations.
//

class MainFrame;

/**
 * List control that can display an icon next to an item.
 */
class ListWindow : public ListView
{

public:

    enum Icon
    {
        Icon_YellowArrow,
        Icon_GreenArrow,
    };

    /**
     * Constructor.
     */
    ListWindow(MainFrame* mainFrame, wxWindowID winid);

    /**
    * Updates the colors of the panel to match the settings
    */
    void SetFontColorSettings(const FontColorSettings& settings);

    /**
     * Adds an item to the list.
     */
    void Append(const wxString& name);

    /**
     * Updates the text for an item.
     */
    void SetString(long item, const wxString& name);

    /**
     * Sets the icon for an item.
     */
    void SetItemIcon(long item, Icon icon);

    /**
     * Removes all icons for the items.
     */
    void ClearAllIcons();

    /**
     * Called when the window loses keyboard focus,
     */
    void OnKillFocus(wxFocusEvent& event);

    DECLARE_EVENT_TABLE()

private:

    /**
     * Unselects all of the currently selected items.
     */
    void ClearSelection();

};

#endif