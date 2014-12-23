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

#ifndef PROJECT_FILE_INFO_CTRL_H
#define PROJECT_FILE_INFO_CTRL_H

#include "Project.h"
#include <wx/wx.h>
#include "FontColorSettings.h"

//
// Forward declarations.
//

class ToolTipWindow;

/**
 * Control that displays basic information about a file in the project.
 */
class ProjectFileInfoCtrl : public wxControl
{

public:

    /**
     * Constructor.
     */
    explicit ProjectFileInfoCtrl(wxWindow* parent, wxWindowID id = wxID_ANY);
  

    /**
    * Updates the colors of the panel to match the settings
    */
    void SetFontColorSettings(const FontColorSettings& settings);

    /**
     * Called when the info box needs to be repainted.
     */
    void OnPaint(wxPaintEvent& event);

    /**
     * Called when the mouse cursor enters the bounds of the window.
     */
    void OnMouseEnterWindow(wxMouseEvent& event);

    /**
     * Sets the file which the info in the box refers to.
     */
    void SetFile(const Project::File* file);

    /**
     * Returns the file which the info in the box refers to.
     */
    const Project::File* GetFile() const;

    DECLARE_EVENT_TABLE()

private:

    /**
     * Displays the tool tip control.
     */
    void ShowToolTip(const wxString& text);

private:
    
    static const int        s_padding = 3;
    
    const Project::File*    m_file;
    bool                    m_textIsClipped;
    wxColor                 m_fontColor;

};

#endif