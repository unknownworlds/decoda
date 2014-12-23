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

#include "SearchWindow.h"
#include "MainFrame.h"
#include "FontColorSettings.h"

DEFINE_EVENT_TYPE(wxEVT_SEARCH_KEY_DOWN)

BEGIN_EVENT_TABLE(SearchWindow, wxTextCtrl)
    EVT_LEFT_DCLICK(        SearchWindow::OnDoubleClick)
END_EVENT_TABLE()


SearchWindow::SearchWindow(MainFrame* mainFrame, wxWindowID winid)
    : wxTextCtrl(mainFrame, winid, _(""), wxDefaultPosition, wxSize(200,150), wxTE_MULTILINE | wxTE_READONLY | wxTE_DONTWRAP | wxTE_RICH | wxBORDER_SUNKEN)
{
    m_mainFrame = mainFrame;
}

void SearchWindow::SetFontColorSettings(const FontColorSettings& settings)
{

    // Since we don't save enough information to reapply styles to the existing text,
    // clear everything. This is lame, but changing font settings isn't something that
    // happens often enough for it to really make much difference.
    Clear();

    SetBackgroundColour(settings.GetColors(FontColorSettings::DisplayItem_Window).backColor);

    m_messageAttr.SetTextColour(settings.GetColors(FontColorSettings::DisplayItem_Window).foreColor);
    m_messageAttr.SetBackgroundColour(settings.GetColors(FontColorSettings::DisplayItem_Window).backColor);
    m_messageAttr.SetFont(settings.GetFont(FontColorSettings::DisplayItem_Window));
}

void SearchWindow::OnDoubleClick(wxMouseEvent& event)
{
    
    wxTextCoord col, row;
    HitTest(event.GetPosition(), &col, &row);

    wxString line = GetLineText(row);
    m_mainFrame->GotoError(line);

}

void SearchWindow::SearchMessage(const wxString& message)
{
    Freeze();
    SetDefaultStyle(m_messageAttr);
    AppendText(message + "\n");
    Thaw();
}

int SearchWindow::GetCurrentLine() const
{

    long pos = GetInsertionPoint();

    long x, y;
    PositionToXY(pos, &x, &y);

    return y;

}