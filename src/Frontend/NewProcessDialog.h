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

#ifndef NEW_PROCESS_DIALOG_H
#define NEW_PROCESS_DIALOG_H

#include <wx/wx.h>
#include <wx/button.h>

/**
 * New process dialog.
 */
class NewProcessDialog : public wxDialog
{

public:
	
    /**
     * Constructor.
     */
    NewProcessDialog(wxWindow* parent);

    /**
     * Called when the user clicks the ... button for browsing the command line.
     */
    void OnBrowseCommandLine(wxCommandEvent& event);

    /**
     * Called when the user clicks the ... button for browsing the working directory.
     */
    void OnBrowseWorkingDirectory(wxCommandEvent& event);

    /**
     * Called when the user clicks the ... button for browsing the symbols directory.
     */
    void OnBrowseSymbolsDirectory(wxCommandEvent& event);

    /**
     * Called when the user clicks the Browse button for browsing the source control project.
     */
    void OnBrowseSccProject(wxCommandEvent& event);

    /**
     * Called when the user selects an option in the source control dropdown.
     */
    void OnSelectScc(wxCommandEvent& event);

    /**
     * Sets the command line that is displayed in the dialog.
     */
    void SetCommandLine(const wxString& commandLine);

    /**
     * Returns the command line set in the dialog.
     */
    wxString GetCommandLine() const;

    /**
     * Sets the command line arguments that is displayed in the dialog.
     */
    void SetCommandArguments(const wxString& commandArguments);

    /**
     * Returns the command line arguments set in the dialog.
     */
    wxString GetCommandArguments() const;

    /**
     * Sets the working directory that is displayed in the dialog.
     */
    void SetWorkingDirectory(const wxString& workingDirectory);

    /**
     * Returns the working directory set in the dialog.
     */
    wxString GetWorkingDirectory() const;

    /**
     * Sets the symbols directory that is displayed in the dialog.
     */
    void SetSymbolsDirectory(const wxString& symbolsDirectory);

    /**
     * Returns the symbols directory set in the dialog.
     */
    wxString GetSymbolsDirectory() const;

    /**
     * Returns the name of the source control provider.
     */
    wxString GetSccProvider() const;

    /**
     * Sets the name of the source control provider.
     */
    void SetSccProvider(const wxString& sccProvider);

    /**
     * Gets the source control user name.
     */
    wxString GetSccUser() const;

    /**
     * Sets the source control user name.
     */
    void SetSccUser(const wxString& sccUser);

    /**
     * Gets the source control project name.
     */
    wxString GetSccProjectName() const;

    /**
     * Sets the source control project name.
     */
    void SetSccProjectName(const wxString& sccProjName);

    /**
     * Gets the source control local path.
     */
    wxString GetSccLocalPath() const;

    /**
     * Sets the source control local path.
     */
    void SetSccLocalPath(const wxString& sccProjName);

    /**
     * Gets the source control auxiliary project path.
     */
    wxString GetSccAuxProjectPath() const;

    /**
     * Sets the source control auxiliary project path.
     */
    void SetSccAuxProjectPath(const wxString& sccAuxProjPath);

    DECLARE_EVENT_TABLE()

private:

    /**
     * Updates the source control related controls on the dialog based
     * on the current settings.
     */
    void UpdateSccControls();

private:

    enum
    {
        ID_BrowseCommandLine        = 1,
        ID_BrowseWorkingDirectory,
        ID_BrowseSymbolsDirectory,
        ID_SelectScc,
        ID_BrowseSccProject,
    };	

    wxTextCtrl*                 m_commandLineBox;
    wxTextCtrl*                 m_commandArgumentsBox;
	wxTextCtrl*                 m_workingDirectoryBox;
    wxTextCtrl*                 m_symbolsDirectoryBox;
    wxChoice*                   m_sccProviderChoice;
    wxTextCtrl*                 m_sccProjectBox;

	wxButton*                   m_button1;
	wxButton*                   m_button21;
	wxButton*                   m_sccProjectBrowseButton;
	wxStdDialogButtonSizer*     m_sdbSizer1;

    wxString                    m_sccUser;
    wxString                    m_sccProjName;
    wxString                    m_sccLocalPath;
    wxString                    m_sccAuxProjPath;

};

#endif