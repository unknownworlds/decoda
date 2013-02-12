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

#include "SearchTextCtrl.h"

BEGIN_EVENT_TABLE(SearchTextCtrl, wxTextCtrl)
    EVT_KEY_DOWN( OnKeyDown )
END_EVENT_TABLE()

SearchTextCtrl::SearchTextCtrl(wxWindow* parent, wxWindowID id, const wxString& value, const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator, const wxString& name)
    : wxTextCtrl(parent, id, value, pos, size, style, validator, name)
{
    m_nextWindow = NULL;
}

void SearchTextCtrl::OnKeyDown(wxKeyEvent& event)
{
    if (event.GetKeyCode() == WXK_DOWN && event.GetModifiers() == 0)
    {
        // Move to the next window.
        if (m_nextWindow != NULL)
        {
            m_nextWindow->SetFocus();
        }
    }
    else
    {
        // Keep processing the event.
        event.Skip(true);
    }
}

void SearchTextCtrl::SetNextWindow(wxWindow* nextWindow)
{
    m_nextWindow = nextWindow;
}