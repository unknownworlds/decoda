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

#include "ToolTipWindow.h"

BEGIN_EVENT_TABLE(ToolTipWindow, wxPopupWindow)
    
    EVT_PAINT(              ToolTipWindow::OnPaint )
    EVT_MOTION(             ToolTipWindow::OnMouseMotion )
    EVT_MOUSE_CAPTURE_LOST( ToolTipWindow::OnMouseCaptureLost )

END_EVENT_TABLE()

ToolTipWindow::ToolTipWindow(wxWindow *parent, const wxString& text, const wxPoint& position)
{

    m_region    = wxRect(0, 0, 0, 0);
    m_useRegion = false;

    Create(parent);
    SetText(text);
        
    wxPoint point;

    if (position == wxDefaultPosition)
    {

        // Move the window to the cursor position.
        wxGetMousePosition(&point.x, &point.y);

        // We want to show the tip below the mouse, not over it
        //
        // NB: the reason we use "/ 2" here is that we don't know where the current
        //     cursors hot spot is... it would be nice if we could find this out
        //     though
        point.y += wxSystemSettings::GetMetric(wxSYS_CURSOR_Y) / 2;
    
    }
    else
    {
        point = position;
    }

    Position(point, wxSize(0, 0));

    Show(true);

}

ToolTipWindow::~ToolTipWindow()
{
    // If we still have the mouse capture, release it.
    if (GetCapture() == this)
    {
        ReleaseMouse();
    }
}

void ToolTipWindow::SetRegion(const wxRect& region)
{

    m_region    = region;
    m_useRegion = true;

    // Capture the mouse events so we can tell when the cursor moves.
    CaptureMouse();

}

void ToolTipWindow::SetText(const wxString& text)
{

    wxClientDC dc(this);
    dc.SetFont(GetFont());

    // calculate the length: we want each line be no longer than maxLength
    // pixels and we only break lines at words boundary
    wxString current;
    wxCoord height, width, widthMax = 0;
    m_heightLine = 0;

    bool breakLine = false;
    for ( const wxChar *p = text.c_str(); ; p++ )
    {
        if ( *p == _T('\n') || *p == _T('\0') )
        {
            dc.GetTextExtent(current, &width, &height);
            if ( width > widthMax )
                widthMax = width;

            if ( height > m_heightLine )
                m_heightLine = height;

            m_textLines.Add(current);

            if ( !*p )
            {
                // end of text
                break;
            }

            current.clear();
            breakLine = false;
        }
        else if ( breakLine && (*p == _T(' ') || *p == _T('\t')) )
        {
            // word boundary - break the line here
            m_textLines.Add(current);
            current.clear();
            breakLine = false;
        }
        else // line goes on
        {
            current += *p;
            dc.GetTextExtent(current, &width, &height);

            if ( width > widthMax )
                widthMax = width;

            if ( height > m_heightLine )
                m_heightLine = height;
        }
    }

    // take into account the border size and the margins
    width  = 2*(s_textMargin + 1) + widthMax;
    height = 2*(s_textMargin + 1) + wx_truncate_cast(wxCoord, m_textLines.GetCount()) * m_heightLine;
    SetClientSize(width, height);
    SetSize(0, 0, width, height);
}


void ToolTipWindow::OnPaint(wxPaintEvent& event)
{

    wxPaintDC dc(this);

    wxRect rect;
    wxSize size = GetClientSize();
    rect.width = size.x;
    rect.height = size.y;

    // First fill the background.

    wxColour bgColor = wxSystemSettings::GetColour(wxSYS_COLOUR_INFOBK);
    wxColour fgColor = wxSystemSettings::GetColour(wxSYS_COLOUR_INFOTEXT);

    dc.SetBrush(wxBrush(bgColor, wxSOLID));
    dc.SetPen( wxPen(fgColor, 1, wxSOLID) );
    dc.DrawRectangle(rect);

    // Draw the text line by line.

    dc.SetTextBackground(bgColor);
    dc.SetTextForeground(fgColor);
    dc.SetFont(GetFont());

    wxPoint pt;
    pt.x = s_textMargin;
    pt.y = s_textMargin;
    
    size_t count = m_textLines.GetCount();

    for ( size_t n = 0; n < count; n++ )
    {
        dc.DrawText(m_textLines[n], pt);
        pt.y += m_heightLine;
    }

}

void ToolTipWindow::OnMouseMotion(wxMouseEvent& event)
{

    wxPoint point = ClientToScreen(event.GetPosition());
    
    if (m_useRegion && !m_region.Contains(point))
    {
        // If the cursor has left the region, destroy the tool tip.
        if (!wxPendingDelete.Member(this))
        {
            wxPendingDelete.Append(this);    
        }
    }
    
    event.Skip();

}

void ToolTipWindow::OnMouseCaptureLost(wxMouseCaptureLostEvent& event)
{
    if (!wxPendingDelete.Member(this))
    {
        wxPendingDelete.Append(this);    
    }
}