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

#ifndef TOOL_TIP_WINDOW_H
#define TOOL_TIP_WINDOW_H

#include "wx/wx.h"
#include "wx/popupwin.h"

/**
 * This class is used to display a simple tool tip. The creator is responsible
 * for dismissing the tool tip when the mouse moves, a key is pressed, etc.
 */
class ToolTipWindow : public wxPopupWindow
{

public:

    /**
     * Constructor.
     */
    ToolTipWindow(wxWindow *parent, const wxString& text, const wxPoint& position = wxDefaultPosition);

    /**
     * Destructor.
     */
    virtual ~ToolTipWindow();

    /**
     * Called when the window needs to be painted.
     */
    void OnPaint(wxPaintEvent& event);

    /**
     * Called whenever the mouse moves.
     */
    void OnMouseMotion(wxMouseEvent& event);

    /**
     * Called when the window loses the mouse capture.
     */
    void OnMouseCaptureLost(wxMouseCaptureLostEvent& event);

    /**
     * Sets the rectangle where the mouse may move before the tip is closed.
     */
    void SetRegion(const wxRect& region);

    DECLARE_EVENT_TABLE()

private:

    /**
     * Adjusts the size of the window to fit the text.
     */
    void SetText(const wxString& text);

private:
    
    static const wxCoord    s_textMargin = 3;

    wxArrayString           m_textLines;
    wxCoord                 m_heightLine;

    bool                    m_useRegion;
    wxRect                  m_region;

};

#endif