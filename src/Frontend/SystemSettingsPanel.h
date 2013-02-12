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

#ifndef SYSTEM_SETTINGS_PANEL_H
#define SYSTEM_SETTINGS_PANEL_H

#include <wx/wx.h>
#include <vector>

#include "SystemSettings.h"

/**
 * This class handles the UI for the systems panel in the settings dialog.
 */
class SystemSettingsPanel : public wxPanel 
{
	
public:
	
    /**
     * Constructor.
     */
    SystemSettingsPanel( wxWindow* parent, int id = wxID_ANY, wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 491,352 ), int style = wxTAB_TRAVERSAL );
    
    /**
     * Finializes the initialization. Should be called after the file types have been added.
     */
    void Initialize();

    /**
     * Adds a new file type that can be associated with the application. The
     * ext should be of the form ".ext". The icon specifies the index of
     * the icon in the executables resource.
     */
    void AddFileType(const wxString& ext, int icon);

    /**
     * Updates the file associations in the registry based on the users selections.
     */
    void ApplyFileAssociations();

    /**
     * Adds a new item to be displayed.
     */
    void SetSettings(const SystemSettings& settings);

    /**
     * Returns the settings set by the user.
     */
    const SystemSettings& GetSettings() const;

    /**
     * Called when the user changes the check for updates check box.
     */
    void OnCheckForUpdates(wxCommandEvent& event);
    
    DECLARE_EVENT_TABLE()

private:

    /**
     * Returns the command line used to open a file with the current application.
     */
    wxString GetOpenCommand() const;

    /**
     * Returns true if the extension is registered to open with the specified command.
     */
    bool GetIsRegistered(const wxString& ext, const wxString& openCommand) const;

private:

    enum ID
    {
        ID_CheckForUpdates = 1,
    };

    struct FileType
    {
        wxString        ext;
        int             icon;
    };

	wxCheckListBox*         m_fileTypesBox;
    std::vector<FileType>   m_fileTypes;

    wxCheckBox*             m_checkForUpdates;

    SystemSettings          m_settings;

};

#endif