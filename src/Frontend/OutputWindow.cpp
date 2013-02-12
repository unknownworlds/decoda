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

#include "OutputWindow.h"
#include "MainFrame.h"
#include "FontColorSettings.h"

DEFINE_EVENT_TYPE(wxEVT_OUTPUT_KEY_DOWN)

BEGIN_EVENT_TABLE(OutputWindow, wxTextCtrl)
    EVT_LEFT_DCLICK(        OutputWindow::OnDoubleClick)
    EVT_KEY_DOWN(           OutputWindow::OnKeyDown)
END_EVENT_TABLE()


OutputWindow::OutputWindow(MainFrame* mainFrame, wxWindowID winid)
    : wxTextCtrl(mainFrame, winid, _(""), wxDefaultPosition, wxSize(200,150), wxTE_MULTILINE | wxTE_READONLY | wxTE_DONTWRAP | wxTE_RICH | wxBORDER_SUNKEN)
{
    m_mainFrame = mainFrame;
}

void OutputWindow::SetFontColorSettings(const FontColorSettings& settings)
{

    // Since we don't save enough information to reapply styles to the existing text,
    // clear everything. This is lame, but changing font settings isn't something that
    // happens often enough for it to really make much difference.
    Clear();

    SetBackgroundColour(settings.GetColors(FontColorSettings::DisplayItem_Default).backColor);

    m_messageAttr.SetTextColour(settings.GetColors(FontColorSettings::DisplayItem_Default).foreColor);
    m_messageAttr.SetBackgroundColour(settings.GetColors(FontColorSettings::DisplayItem_Default).backColor);
    m_messageAttr.SetFont(settings.GetFont(FontColorSettings::DisplayItem_Default));

    m_warningAttr.SetTextColour(settings.GetColors(FontColorSettings::DisplayItem_Warning).foreColor);
    m_warningAttr.SetBackgroundColour(settings.GetColors(FontColorSettings::DisplayItem_Warning).backColor);
    m_warningAttr.SetFont(settings.GetFont(FontColorSettings::DisplayItem_Warning));
    
    m_errorAttr.SetTextColour(settings.GetColors(FontColorSettings::DisplayItem_Error).foreColor);
    m_errorAttr.SetBackgroundColour(settings.GetColors(FontColorSettings::DisplayItem_Error).backColor);
    m_errorAttr.SetFont(settings.GetFont(FontColorSettings::DisplayItem_Error));

}

void OutputWindow::OnDoubleClick(wxMouseEvent& event)
{
    
    wxTextCoord col, row;
    HitTest(event.GetPosition(), &col, &row);

    wxString line = GetLineText(row);
    m_mainFrame->GotoError(line);

}

void OutputWindow::OnKeyDown(wxKeyEvent& event)
{

    // Send the key event to our parent.

    wxKeyEvent event2(event);
    event2.SetEventType(wxEVT_OUTPUT_KEY_DOWN);

    if (GetParent() != NULL)
    {

        GetParent()->GetEventHandler()->ProcessEvent(event2);

        if (event2.GetSkipped())
        {
            event.Skip();
        }
    
    }

}

void OutputWindow::OutputMessage(const wxString& message)
{
    SharedOutput(message, m_messageAttr);
}

void OutputWindow::OutputWarning(const wxString& message)
{
    SharedOutput(message, m_warningAttr);
}

void OutputWindow::OutputError(const wxString& message)
{
    SharedOutput(message, m_errorAttr);
}

void OutputWindow::SharedOutput(const wxString& message, const wxTextAttr& textAttr)
{
    int beforeAppendPosition = GetInsertionPoint();
    int beforeAppendLastPosition = GetLastPosition();
    Freeze();
    SetDefaultStyle(textAttr);
    AppendText(message + "\n");
    Thaw();
    SetInsertionPoint(beforeAppendPosition);
    if (beforeAppendPosition == beforeAppendLastPosition)
    {
        SetInsertionPoint(GetLastPosition());
        ShowPosition(GetLastPosition());
        ScrollLines(-1);
    }
}

int OutputWindow::GetCurrentLine() const
{

    long pos = GetInsertionPoint();

    long x, y;
    PositionToXY(pos, &x, &y);

    return y;

}