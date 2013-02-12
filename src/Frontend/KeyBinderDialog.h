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

#ifndef KEY_BINDER_DIALOG_H
#define KEY_BINDER_DIALOG_H

#include <wx/wx.h>
#include <wx/button.h>
#include <wx/treectrl.h>

#include "KeyBinder.h"

//
// Forward declarations.
//

class KeyMonitorTextCtrl;

/**
 * Dialog for customizing the key bindings.
 */
class KeyBinderDialog : public wxPanel
{

public:

    /**
     * Constructor.
     */
    KeyBinderDialog(wxWindow* parent, int id = -1, wxPoint pos = wxDefaultPosition,
        wxSize size = wxSize( 430, 420 ), int style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

    /**
     * Destructor.
     */
    virtual ~KeyBinderDialog();

    /**
     * Finializes the initialization. Should be called after the commands have been added.
     */
    void Initialize();

    /**
     * Called when the user selects something in the command tree.
     */
    void OnCommandTreeSelectionChanged(wxTreeEvent& event);

    /**
     * Called when the user clicks the Add button.
     */
    void OnAdd(wxCommandEvent& event);

    /**
     * Called when the user clicks the Remove button.
     */
    void OnRemove(wxCommandEvent& event);

    /**
     * Called when the user clicks the Remove All button.
     */
    void OnRemoveAll(wxCommandEvent& event);

    /**
     * Called when the user selects an item in the shortcuts list.
     */
    void OnShortcutsListItemSelected(wxCommandEvent& event);

    /**
     * Called when the text in the key monitor control changes (i.e. when the
     * user enters a key).
     */
    void OnKeyMonitorTextChanged(wxCommandEvent& event);

    /**
     * Adds a command that can be configured using the dialog.
     */
    void AddCommand(const KeyBinder::Command& command);

    /**
     * Returns the number of commands that have been assigned.
     */
    unsigned int GetNumCommands() const;

    /**
     * Returns the ith command.
     */
    const KeyBinder::Command& GetCommand(unsigned int i) const;

    DECLARE_EVENT_TABLE()

private:

    enum ID
    {
        ID_CommandTree  = 1,
        ID_ShortcutsList,
        ID_Add,
        ID_KeyMonitor,
        ID_Remove,
        ID_RemoveAll,
    };

    struct CommandData : public wxTreeItemData
    {
        KeyBinder::Command* command;
    };

	wxStaticText*                       m_staticText1;
	wxTreeCtrl*                         m_commandTreeCtrl;
	wxStaticText*                       m_staticText2;
	wxListBox*                          m_shortcutsListCtrl;
	wxButton*                           m_removeButton;    
	wxButton*                           m_removeAllButton;
	wxStaticText*                       m_staticText3;
	KeyMonitorTextCtrl*                 m_keyMonitorCtrl;
	wxStaticText*                       m_staticText4;
	wxTextCtrl*                         m_currentlyAssignedCtrl;
	wxButton*                           m_addButton;
	wxStaticText*                       m_staticText9;
	wxTextCtrl*                         m_descriptionCtrl;
	wxStdDialogButtonSizer*             m_sdbSizer3;

    std::vector<KeyBinder::Command*>    m_commands;

};

#endif