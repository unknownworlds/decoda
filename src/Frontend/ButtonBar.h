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

#ifndef BUTTON_BAR_H
#define BUTTON_BAR_H

#include <wx/wx.h>

#include <vector>

/**
 * Basic strip of buttons like a tool bar, but better suited for being placed
 * inside a window rather than docked to one of the edges.
 */
class ButtonBar : public wxWindow
{

    DECLARE_EVENT_TABLE()

public:

    /**
     * Constructor.
     */
    ButtonBar(wxWindow* parent, wxWindowID winId);

    /**
     * Destructor.
     */
    virtual ~ButtonBar();

    /**
     * Sets the size of the bitmaps for each button.
     */
    void SetToolBitmapSize(const wxSize& bitmapSize);

    /**
     * Adds a new radio style button.
     */
    void AddRadioTool(int toolId,  const wxString& label, const wxBitmap& bitmap1, const wxBitmap& bitmap2);

    /**
     * Adds a separator between the buttons. Radio tools should be grouped using
     * separators.
     */
    void AddSeparator();

    /**
     * Adds a new push button style button.
     */
    void AddTool(int toolId, const wxString& label, const wxBitmap& bitmap1, const wxBitmap& bitmap2);

    /**
     * Doesn't do anything, but included for compaibility with wxToolBar.
     */
    void Realize();

    /**
     * Toggles a tool on or off. This does not emit any events.
     */
    void ToggleTool(int toolId, bool toggle);

    /**
     * Sets whether or not the specified tool is enabled or disabled.
     */
    void SetToolEnabled(int toolId, bool enabled);

    /**
     * Selects a tool in a radio group. This does not emit any events.
     */
    void SetTool(int toolId);

    /**
     * Called when the control needs to be painted.
     */
    void OnPaint(wxPaintEvent& event);

    /**
     * Called when the left mouse button is pressed.
     */
    void OnLeftButtonDown(wxMouseEvent& event);

    /**
     * Called when the left mouse button is released.
     */
    void OnLeftButtonUp(wxMouseEvent& event);

    /**
     * Called when the mouse cursor is moved.
     */
    void OnMouseMove(wxMouseEvent& event);

    /**
     * From wxWindow.
     */
    virtual wxSize DoGetBestSize() const;

private:

    struct Tool
    {
        bool        radio;
        int         toolId;
        wxString    label;
        wxBitmap    bitmap1;
        wxBitmap    bitmap2;
        wxBitmap    disabledBitmap;
        bool        set;
        bool        enabled;
    };

    /**
     * Returns the client area rectangle occupied by the specified tool.
     */
    wxRect GetToolRect(unsigned int toolIndex) const;

    /**
     * Returns the index of the tool under the client area point. If there is no
     * tool under the point, the method returns -1.
     */
    unsigned int GetToolForPoint(const wxPoint& point) const;

    /**
     * Returns the index of the tool for the specified id. If there is no
     * corresponding tool the method returns -1.
     */
    unsigned int GetToolForId(unsigned int toolId);

    /**
     * Creates a bitmap used for disabled tools from the normal bitmap.
     */
    static void CreateDisabledBitmap(const wxBitmap& normalBitmap, wxBitmap& disabledBitmap);

private:

    unsigned int        m_pressedTool;

    wxSize              m_bitmapSize;
    wxSize              m_marginSize;
    wxSize              m_paddingSize;
    std::vector<Tool*>  m_tools;

    unsigned int        m_toolTipIndex;

};

#endif