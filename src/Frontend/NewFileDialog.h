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

#ifndef NEW_FILE_DIALOG_H
#define NEW_FILE_DIALOG_H

#include <wx/wx.h>
#include <wx/statline.h>
#include <wx/listctrl.h>
#include <wx/filename.h>

/**
 * Dialog for creating a new file.
 */
class NewFileDialog : public wxDialog
{

public:

    /**
     * Constructor.
     */
    NewFileDialog( wxWindow* parent, bool enableSourceControl);

    /**
     * Destructor.
     */
    virtual ~NewFileDialog();

    /**
     * Sets the default path for the new file.
     */
    void SetPath(const wxString& path);

    /**
     * Called when the user clicks the Browse button.
     */
    void OnBrowse(wxCommandEvent& event);

    /**
     * Called when the user clicks the Ok button.
     */
    void OnOk(wxCommandEvent& event);

    /**
     * Called when the user pressed enter in the file name text control.
     */
    void OnFileNameTextEnter(wxCommandEvent& event);

    /**
     * Returns the file name that the user entered.
     */
    wxFileName GetFileName() const;

    /**
     * Returns true if the user selected "Add to Source Control"
     */
    bool GetAddToSourceContrl() const;

    DECLARE_EVENT_TABLE()

private:

    /**
     * Returns true if the specified file name is valid for the file system
     * and does not include any illegal characters.
     */
    bool GetIsFileNameValid(const wxFileName& fileName) const;

    /**
     * Verifies that the file name entered is valid and ends the dialog.
     */
    void VerifyFileNameAndEnd();

private:

    enum ID
    {
        ID_Browse   = 1,
        ID_FileName,
    };

    wxListCtrl*                 m_listCtrl;
    wxStaticText*               m_staticText1;
    wxTextCtrl*                 m_fileNameCtrl;
    wxStaticText*               m_staticText2;
    wxTextCtrl*                 m_pathCtrl;
    wxButton*                   m_browseButton;
    wxStaticLine*               m_staticline3;
    wxStdDialogButtonSizer*     m_sdbSizer1;
    wxImageList*                m_imageList;
    wxCheckBox*                 m_sourceControlCheck;

};

#endif