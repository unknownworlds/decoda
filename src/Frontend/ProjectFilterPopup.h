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

#ifndef PROJECT_FILTER_POPUP_H
#define PROJECT_FILTER_POPUP_H

#include <wx/wx.h>
#include <wx/popupwin.h>

#include "ProjectExplorerWindow.h"

/**
 * Menu that displays the items that can be filtered.
 */
class ProjectFilterPopup : public wxPopupTransientWindow 
{
	
public:
    
    /**
     * Constructor.
     */
    ProjectFilterPopup( wxWindow* parent, ProjectExplorerWindow* explorerWindow );

    /**
     * From wxPopupTransientWindow.
     */
    virtual void Popup(wxWindow *focus = NULL);
    
    /**
     * Called when a check box is clicked.
     */
    void OnCheckBox(wxCommandEvent& event);
        
    DECLARE_EVENT_TABLE()

private:

    ProjectExplorerWindow*              m_explorerWindow;
	wxCheckBox*                         m_checkBox[4];
    ProjectExplorerWindow::FilterFlag   m_checkBoxFlag[4];

};

#endif