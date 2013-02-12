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

#ifndef EDITOR_SETTINGS_PANEL_H
#define EDITOR_SETTINGS_PANEL_H

#include <wx/wx.h>
#include <wx/radiobut.h>

#include "EditorSettings.h"

/**
 * This class handles the UI for modifying the editor settings page of the
 * settings dialog.
 */
class EditorSettingsPanel : public wxPanel 
{
	
public:

	EditorSettingsPanel( wxWindow* parent, int id = wxID_ANY, wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 461,438 ), int style = wxTAB_TRAVERSAL );

    void Initialize();
    
    void SetSettings(const EditorSettings& settings);
    const EditorSettings& GetSettings() const;
    
    /**
     * Called when "Insert spaces" radio button is selected.
     */
    void OnInsertSpaces(wxCommandEvent& event);

    /**
     * Called when the "Keep tabs" radio button is selected.
     */
    void OnKeepTabs(wxCommandEvent& event);

    /**
     * Called when the "Convert tabs to spaces on load" check box is changed.
     */
    void OnRemoveTabsOnLoad(wxCommandEvent& event);

    /**
     * Called when the text in the indent size text control is changed.
     */
    void OnIndentSizeChanged(wxCommandEvent& event);

    /**
     * Called when the "Show line numbers in margin" check box is changed.
     */
    void OnShowLineNumbersChanged(wxCommandEvent& event);

    /**
     * Called when the "Load last project on startup" check box is changed.
     */
    void OnLoadLastProjectOnStartup(wxCommandEvent& event);

    /**
     * Called when the "Most recently used tab switching" check box is changed.
     */
    void OnMostRecentlyUsedTabSwitching(wxCommandEvent& event);

    /**
     * Called when the "Enable auto complete" check box is changed.
     */
    void OnEnableAutoComplete(wxCommandEvent& event);

    /**
     * Called when the "Show white space" check box is changed.
     */
    void OnShowWhiteSpace(wxCommandEvent& event);
    
    DECLARE_EVENT_TABLE()

private:

    enum ID
    {
        ID_InsertSpaces      = 1,
        ID_KeepTabs,   
        ID_IndentSize,
        ID_RemoveTabsOnLoad,
        ID_ShowLineNumbers,
        ID_LoadLastProjectOnStartup,
        ID_MostRecentlyUsedTabSwitching,
        ID_EnableAutoComplete,
        ID_ShowWhiteSpace,
    };

	wxStaticText*   m_staticText5;
	wxTextCtrl*     m_indentSizeCtrl;
	wxRadioButton*  m_useTabs;
	wxRadioButton*  m_useSpaces;
	wxCheckBox*     m_removeTabsOnLoad;
    wxCheckBox*     m_showLineNumbers;
    wxCheckBox*     m_loadLastProjectOnStartup;
    wxCheckBox*     m_mostRecentlyUsedTabSwitching;
    wxCheckBox*     m_enableAutoComplete;
    wxCheckBox*     m_showWhiteSpace;

    EditorSettings  m_settings;
	
};

#endif