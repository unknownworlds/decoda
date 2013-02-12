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

#ifndef SEARCH_TEXT_CTRL_H
#define SEARCH_TEXT_CTRL_H

#include <wx/wx.h>
#include <wx/textctrl.h>

/**
 * Text control that moves to another window when the down key is pressed.
 */
class SearchTextCtrl : public wxTextCtrl
{
public:

    /**
     * Constructor.
     */
    SearchTextCtrl(wxWindow* parent, wxWindowID id, const wxString& value = "",
        const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
        long style = 0, const wxValidator& validator = wxDefaultValidator, const wxString& name = wxTextCtrlNameStr);

    /**
     * Called when the user presses a key in the control.
     */
    void OnKeyDown(wxKeyEvent& event);

    /**
     * Sets the window that the focus will be changed to when the down key is
     * pressed.
     */
    void SetNextWindow(wxWindow* nextWindow);

    DECLARE_EVENT_TABLE()

private:

    wxWindow*   m_nextWindow;

};

#endif