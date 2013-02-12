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

#ifndef CHOICE_DIALOG_H
#define CHOICE_DIALOG_H

#include <wx/wx.h>

/**
 * Slight modification of the basic wxMultiChoiceDialog that allows a "No" button.
 */
class ChoiceDialog : public wxMultiChoiceDialog
{

public:

    /**
     * Constructor.
     */
    ChoiceDialog(wxWindow *parent, const wxString& message, const wxString& caption, const wxArrayString& choices,
        long styleDlg = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxYES | wxNO | wxCANCEL | wxCENTRE, const wxPoint& pos = wxDefaultPosition);

    /**
     * Called when the user presses the no button.
     */
    virtual void OnNo(wxCommandEvent& event);

    DECLARE_EVENT_TABLE()

};

#endif