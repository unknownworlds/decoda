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

#ifndef PROCESSES_DIALOG_H
#define PROCESSES_DIALOG_H

#include <wx/wx.h>
#include "DebugFrontend.h"

//
// Forward declarations.
//

class wxListCtrl;
class wxListEvent;

/**
 * Dialog box for allowing the user to attach to an existing process.
 */
class ProcessesDialog : public wxDialog 
{

public:
	

    /**
     * Constructor.
     */
    explicit ProcessesDialog( wxWindow* parent);

    /**
     * Called when the user presses the Attach button.
     */
    void OnAttach(wxCommandEvent& event);

    /**
     * Called when the user presses the Refresh button.
     */
    void OnRefresh(wxCommandEvent& event);

    /**
     * Called when the user presses the Cancel button.
     */
    void OnCancel(wxCommandEvent& event);

    /**
     * Called when the user selects a new item in the process list.
     */
    void OnProcessSelected(wxListEvent& event);

    /**
     * Called when the user unselects an item in the process list.
     */
    void OnProcessDeselected(wxListEvent& event);

    /**
     * Called when the user clicks on one of the column headings in the
     * process list.
     */
    void OnProcessColumnClicked(wxListEvent& event);

    /**
     * Called when the user double clicks on one of the items in the processes
     * list.
     */
    void OnProcessActivated(wxListEvent& event);

    /**
     * Returns the process id that the user has selected.
     */
    unsigned int GetProcessId() const;

    /**
     * Selects the process with the specified module name (if one exists) in
     * the dialog.
     */
    void SelectProcessByModule(const wxString& name);

    DECLARE_EVENT_TABLE()

private:

    enum ID
    {
        ID_Refresh  = 1,
    };

    /**
     * Updates the list of processes.
     */
    void Refresh();

    /**
     * Updates the status of the Attach button based on whether or not a process is
     * selected.
     */
    void UpdateAttachStatus();

    /**
     * Sorts the process list by the specified column.
     */
    void SortColumns();

    /**
     * Function used to sort the columns of the list box.
     */
    static int wxCALLBACK SortCompareFunction(long item1, long item2, long sortData);

private:

    unsigned int                        m_processId;
    std::vector<DebugFrontend::Process> m_processes;

	wxListCtrl*                         m_processList;
	wxButton*                           m_attach;
	wxButton*                           m_refresh;
	wxButton*                           m_close;

    int                                 m_sortColumn;
    bool                                m_sortForward;
		
};

#endif