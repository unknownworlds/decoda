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

#ifndef SEARCH_WINDOW_H
#define SEARCH_WINDOW_H

#include <wx/wx.h>

//
// Forward declarations.
//

class MainFrame;
class FontColorSettings;

/**
 *
 */
class SearchWindow : public wxTextCtrl
{

public:

    /**
     * Constructor.
     */
    SearchWindow(MainFrame* mainFrame, wxWindowID winid);
    
    /**
     * Sets the font and color settings used in the output window.
     */
    void SetFontColorSettings(const FontColorSettings& settings);

    /**
     * Called when the user double clicks in the window.
     */
    void OnDoubleClick(wxMouseEvent& event);

    /**
     * Adds a message to the end of the log.
     */
    void SearchMessage(const wxString& message);    

    /**
     * Returns the line that the cursor is positioned on.
     */
    int GetCurrentLine() const;

    DECLARE_EVENT_TABLE()

private:

    MainFrame*  m_mainFrame;

    wxTextAttr  m_messageAttr;    
};

#endif