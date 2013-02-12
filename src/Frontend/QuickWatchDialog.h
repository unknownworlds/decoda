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

#ifndef QUICK_WATCH_DIALOG_H
#define QUICK_WATCH_DIALOG_H

#include <wx/wx.h>
#include <wx/button.h>
#include "WatchCtrl.h"

//
// Forward declarations.
//

class WatchWindow;
class MainFrame;

/**
 * New process dialog.
 */
class QuickWatchDialog : public wxDialog
{

public:
	
    /**
     * Constructor.
     */
    QuickWatchDialog(wxWindow* parent, int id = -1, wxString title = wxT("Quick Watch"), wxPoint pos = wxDefaultPosition,
        wxSize size = wxSize( 464, 350 ), int style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

    /**
     * Sets the watch window that is populated when the user clicks "Add Watch".
     */
    void SetWatchWindow(WatchWindow* watchWindow, MainFrame* mainFrame);

    /**
     * Sets the VM and stack level that the window will query.
     */
    void SetContext(unsigned int vm, unsigned int stackLevel);

    /**
     * Sets the expression displayed in the window.
     */
    void SetExpression(const wxString& expression); 

    /**
     * Called when the user clicks the Recalculate button.
     */
    void OnRecalculate(wxCommandEvent& event);

    /**
     * Called when the user clicks the Add Watch button.
     */
    void OnAddWatch(wxCommandEvent& event);

    /**
     * Called when the user presses return in the expresison text box.
     */
    void OnExpressionBoxTextEnter(wxCommandEvent& event);
    
    DECLARE_EVENT_TABLE()

private:

    /**
     * Recalculates the expression in the UI.
     */
    void Recalculate();

private:

    enum
    {
        ID_Recalculate = 1,
        ID_AddWatch,
        ID_ExpressionBox,
    };	

    WatchWindow*    m_watchWindow;
    MainFrame*      m_mainFrame;

    wxStaticText*   m_staticText1;
	wxTextCtrl*     m_expressionBox;
	wxButton*       m_recalculateButton;
	wxButton*       m_addWatchButton;
	wxButton*       m_closeButton;
	WatchCtrl*      m_resultBox;
    wxTreeItemId    m_root;
	
};

#endif