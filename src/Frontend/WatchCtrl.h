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

#ifndef WATCH_CTRL_H
#define WATCH_CTRL_H

#include <wx/wx.h>
#include "treelistctrl.h"

//
// Forward declarations.
//

class wxXmlNode;

/**
 * Watch control. This is the base class for the tree controls used to display
 * the results of evaluating expressions.
 */
class WatchCtrl : public wxTreeListCtrl
{

public:

    WatchCtrl(wxWindow *parent, wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
        long style = wxTR_HIDE_ROOT | wxTR_FULL_ROW_HIGHLIGHT | wxTR_ROW_LINES | wxTR_HAS_BUTTONS, const wxValidator &validator = wxDefaultValidator, const wxString& name = ""); 

    /**
     * Parses a compound expression ({ key = value, key = value, etc. }) and adds it's
     * key value pairs as subitems for the specified item in the tree.
     */
    bool AddCompoundExpression(wxTreeItemId parent, wxXmlNode* node);

    /**
     * Updates the value for the express in the index spot in the list.
     */
    void UpdateItem(wxTreeItemId item);

    /**
     * Sets the font used to display values.
     */
    void SetValueFont(const wxFont& font);

    /**
     * Sets the virtual machine that the watches are evaluated in.
     */
    void SetContext(unsigned int vm, unsigned int stackLevel);

    /**
     * Called after the user resizes one of of the column headings.
     */
    void OnColumnEndDrag(wxListEvent& event);

    /**
     * Called when the window changes size.
     */
    void OnSize(wxSizeEvent& event);

    /**
     * Collapses a node down into a single line of text.
     */
    static wxString GetNodeAsText(wxXmlNode* node, wxString& type);

    static wxString GetTableAsText(wxXmlNode* root);

    DECLARE_EVENT_TABLE()

private:

    static const unsigned int s_numColumns = 3;

    /**
     * Updates the variable that stores the proprotion of the first column
     * relative to the width of the control.
     */
    void UpdateColumnSizes();

    /**
     * Gets the size of each of the columns in pixels.
     */
    void GetColumnSizes(int totalSize, int columnSize[s_numColumns]) const;

    /**
     * Updates the font for all values in the tree.
     */
    void UpdateFont(wxTreeItemId item);

private:

    float                       m_columnSize[s_numColumns];

    unsigned int                m_vm;
    unsigned int                m_stackLevel;

    float                       m_columnProportion;

    wxFont                      m_valueFont;

};

#endif