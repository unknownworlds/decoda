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

#ifndef MAIN_FRAME_DROP_TARGET_H
#define MAIN_FRAME_DROP_TARGET_H

#include <wx/wx.h>
#include <wx/dnd.h>

//
// Forward declarations.
//

class MainFrame;

/**
 * This class is used to handle drag and drop operations on to the main frame.
 */
class MainFrameDropTarget : public wxFileDropTarget
{

public:

    /** 
     * Constructor.
     */
    MainFrameDropTarget(MainFrame* mainFrame);

    /**
     * From wxFileDropTarget. Called when the user drags a file from Explorer onto
     * the window.
     */
    bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames);

    //wxDragResult OnDragOver(wxCoord x, wxCoord y, wxDragResult def);
    //wxDragResult OnEnter(wxCoord x, wxCoord y, wxDragResult def);
    //void OnLeave();

private:

    MainFrame*  m_mainFrame;

};

#endif