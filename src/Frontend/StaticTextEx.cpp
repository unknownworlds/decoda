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

#include "StaticTextEx.h"

BEGIN_EVENT_TABLE(StaticTextEx, wxPanel)
    EVT_PAINT(StaticTextEx::OnPaint)
END_EVENT_TABLE()

StaticTextEx::StaticTextEx(wxWindow* parent, wxWindowID id, const wxString& label, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
    : wxPanel(parent, id, pos, size, style, name)
{
    m_label = label;
}

void StaticTextEx::OnPaint(wxPaintEvent& event)
{

    wxPaintDC dc(this);
    
    // Erase the background.
    wxBrush backBrush(GetBackgroundColour());
    dc.SetBackground(backBrush);
    dc.Clear();

    // Draw the text.
    dc.SetFont(GetFont());
    dc.SetTextForeground(GetForegroundColour());
    dc.DrawLabel(m_label, GetClientSize(), wxALIGN_CENTER);

}