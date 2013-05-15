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

#ifndef EXTERNAL_TOOLS_DIALOG_H
#define EXTERNAL_TOOLS_DIALOG_H

#include "wx/wx.h"
#include "wx/listctrl.h"

#include <vector>

//
// Forward declarations.
//

class ExternalTool;

/**
 * Dialog box for manipulating the external tools.
 */
class ExternalToolsDialog : public wxDialog
{
	
public:

    /**
     * Constructor.
     */
    ExternalToolsDialog( wxWindow* parent, int id = -1);

    /**
     * Destructor.
     */
    virtual ~ExternalToolsDialog();

    /**
     * Sets the tools that will be displayed in the dialog box and modified.
     * These will automatically be updated if the user presses Ok or Apply.
     */
    void SetTools(std::vector<ExternalTool*>* tools);

    /**
     * Called when the user selecteds a new item in the list of tools.
     */
    void OnListBoxChoiceSelected(wxCommandEvent& event);

    /**
     * Called when the user changes the text in the title text box.
     */
    void OnTitleTextBoxChanged(wxCommandEvent& event);
    
    /**
     * Called when the user changes the text in the command text box.
     */
    void OnCommandTextBoxChanged(wxCommandEvent& event);

    /**
     * Called when the user changes the text in the arguments text box.
     */
    void OnArgumentsTextBoxChanged(wxCommandEvent& event);

    /**
     * Called when the user changes the text in the initial directory text box.
     */
    void OnInitialDirectoryTextBoxChanged(wxCommandEvent& event);

    /**
     * Called when the user clicks the Ok button.
     */
    void OnOk(wxCommandEvent& event);

    /**
     * Called when the user clicks the Apply button.
     */
    void OnApply(wxCommandEvent& event);

    /**
     * Called when the user clicks the Add button.
     */
    void OnAdd(wxCommandEvent& event);

    /**
     * Called when the user clicks the Delete button.
     */
    void OnDelete(wxCommandEvent& event);

    /**
     * Called when the user clicks the Move Up button.
     */
    void OnMoveUp(wxCommandEvent& event);

    /**
     * Called when the user clicks the Move Down button.
     */
    void OnMoveDown(wxCommandEvent& event);

    /**
     * Called when the user clicks the ... button next to the command box.
     */
    void OnCommandBrowse(wxCommandEvent& event);

    /**
     * Called when the user clicks the > button next to the arguments box.
     */
    void OnArgumentsBrowse(wxCommandEvent& event);

    /**
     * Called when the user clicks the > button next to the initial directory box.
     */
    void OnInitialDirectoryBrowse(wxCommandEvent& event);

    /**
     * Called when the user selects Item Path from the popup menu for arguments.
     */
    void OnArgumentsItemPath(wxCommandEvent& event);

    /**
     * Called when the user selects Item Directory from the popup menu for arguments.
     */
    void OnArgumentsItemDirectory(wxCommandEvent& event);

    /**
     * Called when the user selects Item File Name from the popup menu for arguments.
     */
    void OnArgumentsItemFileName(wxCommandEvent& event);

    /**
     * Called when the user selects Item Extension from the popup menu for arguments.
     */
    void OnArgumentsItemExtension(wxCommandEvent& event);

    /**
     * Called when the user selects Item Path from the popup menu for the initial directory.
     */
    void OnInitialDirectoryItemPath(wxCommandEvent& event);

    /**
     * Called when the user selects Item Directory from the popup menu for the initial directory.
     */
    void OnInitialDirectoryItemDirectory(wxCommandEvent& event);

    /**
     * Called when the user selects Item File Name from the popup menu for the initial directory.
     */
    void OnInitialDirectoryItemFileName(wxCommandEvent& event);

    /**
     * Called when the user selects Item Extension from the popup menu for the initial directory.
     */
    void OnInitialDirectoryItemExtension(wxCommandEvent& event);

    /**
     * Called when the user selects an event to hook into from the list box.
     */
    void OnEventHook(wxCommandEvent& event);

    /**
     * Called when the user activates the content help either by hitting the ? button
     * or pressing F1.
     */
    void OnHelp(wxHelpEvent& event);

    DECLARE_EVENT_TABLE()

private:

    /**
     * Updates the controls on the dialog for the specified selected item in
     * the list box.
     */
    void UpdateControlsForSelection(int selectedItem);
    
    /**
     * Returns the tool that's currently selected in the list box, or NULL
     * if no tool is selected.
     */
    ExternalTool* GetSelectedTool();

    /**
     * Applies the changes in the dialog box.
     */
    void Apply();

private:

    enum
    {
        ID_ListBox      = 1,
        ID_TitleTextBox,
        ID_CommandTextBox,
        ID_ArgumentsTextBox,
        ID_InitialDirectoryTextBox,
        ID_ApplyButton,
        ID_AddButton,
        ID_DeleteButton,
        ID_MoveUpButton,
        ID_MoveDownButton,
        ID_CommandBrowseButton,
        ID_ArgumentsBrowseButton,
        ID_InitialDirBrowseButton,
        ID_ArgumentsItemPath,
        ID_ArgumentsItemDirectory,
        ID_ArgumentsItemFileName,
        ID_ArgumentsItemExtension,
        ID_InitialDirectoryItemPath,
        ID_InitialDirectoryItemDirectory,
        ID_InitialDirectoryItemFileName,
        ID_InitialDirectoryItemExtension,
        ID_EventHook,
    };

	wxListBox*          m_listBox;
	wxButton*           m_button5;
	wxButton*           m_button6;
	wxButton*           m_button7;
	wxButton*           m_button8;
	wxTextCtrl*         m_titleTextBox;
	wxTextCtrl*         m_commandTextBox;
	wxButton*           m_commandBrowseButton;
	wxTextCtrl*         m_argumentsTextBox;
	wxButton*           m_argumentsBrowseButton;
    wxChoice*           m_eventHookList;
	wxTextCtrl*         m_initialDirectoryTextBox;
	wxButton*           m_initialDirectoryBrowseButton;
	wxButton*           m_button12;
	wxButton*           m_button13;
    wxButton*           m_button14;

    wxString m_externalEvents[2];

    std::vector<ExternalTool*>* m_appliedTools;
    std::vector<ExternalTool*>  m_workingTools;
	
};

#endif