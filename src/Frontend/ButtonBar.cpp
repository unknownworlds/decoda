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

#include "ButtonBar.h"

#include <wx/renderer.h>

BEGIN_EVENT_TABLE( ButtonBar, wxWindow )

    EVT_PAINT(      ButtonBar::OnPaint )
    EVT_LEFT_DOWN(  ButtonBar::OnLeftButtonDown )
    EVT_LEFT_UP(    ButtonBar::OnLeftButtonUp )
    EVT_MOTION(     ButtonBar::OnMouseMove )

END_EVENT_TABLE() 

ButtonBar::ButtonBar(wxWindow* parent, wxWindowID winId)
    : wxWindow(parent, winId)
{

    m_bitmapSize    = wxSize(16, 15);
    m_paddingSize   = wxSize(4, 4);
    m_marginSize    = wxSize(2, 2);
    m_pressedTool   = -1;
    m_toolTipIndex  = -1;

}

ButtonBar::~ButtonBar()
{
    for (size_t i = 0; i < m_tools.size(); ++i)
    {
        delete m_tools[i];
    }
}

void ButtonBar::SetToolBitmapSize(const wxSize& bitmapSize)
{
    m_bitmapSize = bitmapSize;
}

void ButtonBar::AddRadioTool(int toolId,  const wxString& label, const wxBitmap& bitmap1, const wxBitmap& bitmap2)
{

    Tool* tool = new Tool;

    tool->radio   = true;
    tool->enabled = true;
    tool->toolId  = toolId;
    tool->label   = label;
    tool->bitmap1 = bitmap1;
    tool->bitmap2 = bitmap2;
    tool->set     = m_tools.size() == 0 || m_tools[m_tools.size() - 1] == NULL;

    CreateDisabledBitmap(tool->bitmap1, tool->disabledBitmap);

    m_tools.push_back(tool);

    InvalidateBestSize();

}

void ButtonBar::AddSeparator()
{
    m_tools.push_back(NULL);
    InvalidateBestSize();
}

void ButtonBar::AddTool(int toolId, const wxString& label, const wxBitmap& bitmap1, const wxBitmap& bitmap2)
{

    Tool* tool = new Tool;

    tool->radio   = false;
    tool->enabled = true;
    tool->toolId  = toolId;
    tool->label   = label;
    tool->bitmap1 = bitmap1;
    tool->bitmap2 = bitmap2;
    tool->set     = false;

    CreateDisabledBitmap(tool->bitmap1, tool->disabledBitmap);

    m_tools.push_back(tool);

}

void ButtonBar::Realize()
{
}

void ButtonBar::ToggleTool(int toolId, bool toggle)
{

    unsigned int toolIndex = GetToolForId(toolId);

    if (toolIndex != -1 && m_tools[toolIndex]->radio)
    {

        unsigned int groupStart = toolIndex;

        while (groupStart != 0 && m_tools[groupStart - 1] != NULL)
        {
            --groupStart;
        }

        unsigned int groupEnd = toolIndex;

        while (groupEnd + 1 < m_tools.size() && m_tools[groupEnd + 1] != NULL)
        {
            ++groupEnd;
        }

        // Unselect all of the radio buttons in the group
        for (unsigned int i = groupStart; i <= groupEnd; ++i)
        {
            m_tools[i]->set = false;
        }

        if (toggle)
        {
            // Set the selected radio button.
            m_tools[toolIndex]->set = true;
        }

    }

    Refresh();

}

unsigned int ButtonBar::GetToolForId(unsigned int toolId)
{

    for (unsigned int i = 0; i < m_tools.size(); ++i)
    {
        if (m_tools[i] != NULL && m_tools[i]->toolId == toolId)
        {
            return i;
        }
    }

    return -1;

}

void ButtonBar::SetToolEnabled(int toolId, bool enabled)
{

    unsigned int toolIndex = GetToolForId(toolId);

    if (toolIndex != -1 && m_tools[toolIndex]->enabled != enabled)
    {
        m_tools[toolIndex]->enabled = enabled;
        Refresh();
    }

}

void ButtonBar::OnPaint(wxPaintEvent& event)
{
    
    bool pressedOver = false;

    if (m_pressedTool != -1)
    {
    
        wxPoint point = wxGetMousePosition();
        point = ScreenToClient(point);

        pressedOver = (GetToolForPoint(point) == m_pressedTool);

    }
    
    wxPaintDC dc(this);
    wxRendererNative& renderer = wxRendererNative::Get();

    wxPen separatorPen( wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW) );

    for (unsigned int i = 0; i < m_tools.size(); ++i)
    {
        
        wxRect rect = GetToolRect(i);

        const Tool* tool = m_tools[i];

        if (tool == NULL)
        {
            dc.SetPen( separatorPen );
            dc.DrawLine( rect.GetTopLeft(), rect.GetBottomLeft());
        }
        else
        {

            // Draw a  button.

            int flags = 0;

            if (tool->set)
            {
                flags |= wxCONTROL_PRESSED;
            }

            if (i == m_pressedTool)
            {
                if (pressedOver)
                {
                    flags |= wxCONTROL_PRESSED;
                    flags |= wxCONTROL_CURRENT;
                }
            }

            if (!tool->enabled)
            {
                flags |= wxCONTROL_DISABLED;
            }
            
            renderer.DrawPushButton(this, dc, rect, flags);

            wxPoint point = rect.GetTopLeft() + m_paddingSize;

            if (flags & wxCONTROL_PRESSED)
            {
                point.x += 1;
                point.y += 1;
            }

            if (tool->enabled)
            {
                dc.DrawBitmap(tool->bitmap1, point.x, point.y, true);
            }
            else
            {
                dc.DrawBitmap(tool->disabledBitmap, point.x, point.y, true);
            }

        }

    }

}

wxRect ButtonBar::GetToolRect(unsigned int toolIndex) const
{

    int x = 0;

    for (unsigned int i = 0; i < toolIndex; ++i)
    {

        if (m_tools[i] == NULL)
        {
            x += m_marginSize.x + 1;
        }
        else
        {
            x += m_marginSize.x + m_paddingSize.x * 2 + m_bitmapSize.x;
        }

    }
    
    wxRect rect;

    rect.SetX( x );
    rect.SetY( 0 );

    if (m_tools[toolIndex] == NULL)
    {
        rect.SetWidth( 1 );
    }
    else
    {
        rect.SetWidth( m_bitmapSize.x + m_paddingSize.x * 2 );
    }

    rect.SetHeight( m_bitmapSize.y + m_paddingSize.y * 2 );

    return rect;

}

void ButtonBar::OnLeftButtonDown(wxMouseEvent& event)
{
    
    wxPoint point( event.GetX(), event.GetY() );
    m_pressedTool = GetToolForPoint(point);
   
    if (m_pressedTool != -1)
    {
        if (m_tools[m_pressedTool] != NULL && m_tools[m_pressedTool]->enabled && (!m_tools[m_pressedTool]->radio || !m_tools[m_pressedTool]->set))
        {
            wxRect rect = GetToolRect(m_pressedTool);
            Refresh(false, &rect);
            CaptureMouse();
        }
        else
        {
            m_pressedTool = -1;
        }
    }

}

void ButtonBar::OnLeftButtonUp(wxMouseEvent& event)
{

    if (m_pressedTool != -1)
    {
        ReleaseMouse();
    }

    wxPoint point( event.GetX(), event.GetY() );
    unsigned int selected = GetToolForPoint(point);

    if (selected != -1 && selected == m_pressedTool)
    {

        m_pressedTool = -1;
        ToggleTool(m_tools[selected]->toolId, true);
    
        // Send the event.
        wxCommandEvent newEvent(wxEVT_COMMAND_BUTTON_CLICKED, m_tools[selected]->toolId );
        GetEventHandler()->ProcessEvent( newEvent );

        /*
        wxCommandEvent newEvent( wxEVT_COMMAND_MENU_SELECTED, m_tools[selected]->toolId );
        GetEventHandler()->ProcessEvent( newEvent );
        */

    }
    else
    {
        m_pressedTool = -1;
    }

}

void ButtonBar::OnMouseMove(wxMouseEvent& event)
{

    if (m_pressedTool != -1)
    {
        wxRect rect = GetToolRect(m_pressedTool);
        Refresh(false, &rect);
    }
    else
    {

        // Update the tool tip.
        unsigned int toolIndex = GetToolForPoint( event.GetPosition() );
    
        if (toolIndex != m_toolTipIndex)
        {
            if (toolIndex != -1 && m_tools[toolIndex] != NULL)
            {
                SetToolTip( m_tools[toolIndex]->label );
            }
            else
            {
                SetToolTip( wxEmptyString );
            }
            m_toolTipIndex = toolIndex;
        }
    
    }

}

unsigned int ButtonBar::GetToolForPoint(const wxPoint& point) const
{
    
    for (unsigned int i = 0; i < m_tools.size(); ++i)
    {

        wxRect rect = GetToolRect(i);

        if (rect.Contains(point))
        {
            return i;
        }

    }

    return -1;

}

wxSize ButtonBar::DoGetBestSize() const
{

    wxSize bestSize(0, 0);
    
    if (!m_tools.empty())
    {

        wxRect rect = GetToolRect(static_cast<unsigned int>(m_tools.size()) - 1);
        wxPoint point = rect.GetBottomRight();

        bestSize.x = point.x;
        bestSize.y = point.y;

    }

    return bestSize;

}

void ButtonBar::CreateDisabledBitmap(const wxBitmap& normalBitmap, wxBitmap& disabledBitmap)
{

    wxImage image = normalBitmap.ConvertToImage();
    image = image.ConvertToGreyscale();

    unsigned char* data = image.GetData();

    int width  = image.GetWidth();
    int height = image.GetHeight();

    double fade = 0.4;

    for (int i = 0; i < width * height * 3; ++i)
    {
        int value = static_cast<int>(data[i] * fade + 255 * (1.0 - fade));
        data[i] = value;
    }

    disabledBitmap = wxBitmap(image);

}