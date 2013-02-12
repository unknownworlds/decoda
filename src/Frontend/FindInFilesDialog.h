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

#ifndef FIND_IN_FILES_DIALOG_H
#define FIND_IN_FILES_DIALOG_H

#include <wx/wx.h>
#include <wx/button.h>
#include <wx/filename.h>

/**
 * Dialog box for searching in files.
 */
class FindInFilesDialog : public wxDialog 
{

public:

    /**
     * Constructor.
     */
    FindInFilesDialog(wxWindow* parent, int id = wxID_ANY);

    /**
     * Adds a string to the look in box.
     */
    void AddLookIn(const wxString& lookIn);

    /**
     * Sets the text that user is searching for.
     */
    void SetText(const wxString& text);

    /**
     * Adds a string to the search text history drop down.
     */
    void AddText(const wxString& text);

    /**
     * Called when the user clicks the Browse button.
     */
    void OnBrowse(wxCommandEvent& event);

    /**
     * Returns the directory that the user selected.
     */
    wxString GetDirectory() const;

    /**
     * Returns the text that the user is searching for.
     */
    wxString GetText() const;

    /**
     * Returns the file types specified by the user.
     */
    wxString GetFileTypes() const;

    /**
     * Returns true if the user selected match case.
     */
    bool GetMatchCase() const;

    /**
     * Returns true if the user selected to include sub-directories.
     */
    bool GetIncludeSubDirectories() const;

    /**
     * Returns true if the user selected the match whole word option.
     */
    bool GetMatchWholdWord() const;

    DECLARE_EVENT_TABLE()

private:

    enum ID
    {
        ID_Browse   = 1,
    };

	wxStaticText*           m_staticText33;
	wxComboBox*             m_findWhatBox;
	wxStaticText*           m_staticText34;
	wxComboBox*             m_lookInBox;
	wxButton*               m_button14;
	wxCheckBox*             m_matchCaseCheck;
	wxCheckBox*             m_includeSubDirectoriesCheck;
	wxCheckBox*             m_matchWholeWordCheck;
	wxStaticText*           m_staticText38;
	wxComboBox*             m_fileTypesBox;
	wxStdDialogButtonSizer* m_sdbSizer3;
	
};

#endif
