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

#ifndef CODE_EDIT_DROP_TARGET_H
#define CODE_EDIT_DROP_TARGET_H

#include <wx/wx.h>
#include <wx/dnd.h>

//
// Forward declarations.
//

class CodeEdit;
class MainFrame;

/**
 * This class is used to handle file drag and drop operations onto a code editor.
 */
class CodeEditDropTarget : public wxDropTarget
{

public:

    /** 
     * Constructor.
     */
    CodeEditDropTarget(CodeEdit* codeEdit, MainFrame* mainFrame);

    /**
     * From wxDropTarget.
     */
    virtual wxDragResult OnData(wxCoord x, wxCoord y, wxDragResult def);

    /**
     * From wxDropTarget.
     */
    virtual bool OnDataFiles(wxCoord x, wxCoord y, const wxArrayString& filenames);

    /**
     * From wxDropTarget.
     */
    virtual bool OnDataText(wxCoord x, wxCoord y, const wxString& data);

    /**
     * From wxDropTarget.
     */
    virtual wxDragResult OnDragOver (wxCoord x, wxCoord y, wxDragResult def);

    /**
     * From wxDropTarget.
     */
    virtual wxDragResult OnEnter (wxCoord x, wxCoord y, wxDragResult def);
    
    /**
     * From wxDropTarget.
     */
    virtual void OnLeave();

private:

    CodeEdit*           m_codeEdit;
    MainFrame*          m_mainFrame;

    wxFileDataObject*   m_file;
    wxTextDataObject*   m_text;

};

#endif