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

#ifndef STATIC_TEXT_EX_H
#define STATIC_TEXT_EX_H

#include <wx/wx.h>

/**
 * Extended static text control. The text is centered vertically in this control.
 */
class StaticTextEx : public wxPanel
{

public:

    /**
     * Constructor.
     */
    StaticTextEx(wxWindow* parent, wxWindowID id, const wxString& label, const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = "staticText");

    /**
     * Called when the window needs to be painted.
     */
    void OnPaint(wxPaintEvent& event);

    DECLARE_EVENT_TABLE()

private:

    wxString    m_label;

};

#endif