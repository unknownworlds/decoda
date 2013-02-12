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

#ifndef EXCEPTION_DIALOG_H
#define EXCEPTION_DIALOG_H

#include <wx/wx.h>

/**
 * Dialog box that's displayed when an exception occurs while running a script.
 */
class ExceptionDialog : public wxDialog 
{
	
public:

    enum ID
    {
        ID_Break         = 1,
        ID_Ignore,
        ID_IgnoreAlways,
    };

    /**
     * Constructor.
     */
	ExceptionDialog( wxWindow* parent, const wxString& message, bool allowIgnoreAlways);

    /**
     * Called when the user selects the Break button.
     */
    void OnBreak(wxCommandEvent& event);

    /**
     * Called when the user selects the Ignore button.
     */
    void OnIgnore(wxCommandEvent& event);
    
    /**
     * Called when the user selects the Ignore Always button.
     */
    void OnIgnoreAlways(wxCommandEvent& event);

    DECLARE_EVENT_TABLE()

protected:

	wxStaticBitmap* m_bitmap1;
	wxStaticText*   m_staticText1;
    wxTextCtrl*     m_textCtrl1;
	wxButton*       m_button2;
	wxButton*       m_button3;
    wxButton*       m_button4;
	
};

#endif