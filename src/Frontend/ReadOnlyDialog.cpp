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

#include "ReadOnlyDialog.h"

BEGIN_EVENT_TABLE(ReadOnlyDialog, wxDialog)

    EVT_BUTTON(ReadOnlyDialog::ID_CheckOut,       ReadOnlyDialog::OnCheckOut)
    EVT_BUTTON(ReadOnlyDialog::ID_MakeWriteable,  ReadOnlyDialog::OnMakeWriteable)

END_EVENT_TABLE()

ReadOnlyDialog::ReadOnlyDialog( wxWindow* parent, const wxString& fileName, bool checkOutOption )
    : wxDialog( parent, wxID_ANY,  wxT("Edit of Read-Only File"), wxDefaultPosition, wxSize( 452,135 ), wxCAPTION|wxCLOSE_BOX|wxSYSTEM_MENU  )
{

    this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 2, 1, 0, 0 );
	fgSizer1->AddGrowableCol( 0 );
	fgSizer1->SetFlexibleDirection( wxHORIZONTAL );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxHORIZONTAL );
	
	wxGridSizer* gSizer1;
	gSizer1 = new wxGridSizer( 1, 1, 0, 0 );
	
	m_bitmap1 = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( -1,-1 ), 0 );
	m_bitmap1->SetIcon(wxIcon("wxICON_WARNING"));
    gSizer1->Add( m_bitmap1, 0, wxALL, 10 );
	
	bSizer1->Add( gSizer1, 1, wxEXPAND, 5 );
	
    wxString format;

    if (checkOutOption)
    {
        format = "The file %s, which you attempted to edit, is read-only on disk.\nWould you like to make the file writeable or check it out?";
    }
    else
    {
        format = "The file %s, which you attempted to edit, is read-only on disk.\nWould you like to make the file writeable?";    
    }
    
    m_staticText1 = new wxStaticText( this, wxID_ANY, wxString::Format(format, fileName), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer1->Add( m_staticText1, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 10 );
	
	fgSizer1->Add( bSizer1, 1, wxALIGN_CENTER|wxALL, 5 );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );
	
    if (checkOutOption)
    {
    	m_button2 = new wxButton( this, ID_CheckOut, wxT("Check Out"), wxDefaultPosition, wxDefaultSize, 0 );
	    bSizer3->Add( m_button2, 0, wxALL, 5 );
    }
    else
    {
        m_button2 = NULL;
    }
	
	m_button3 = new wxButton( this, ID_MakeWriteable, wxT("Make Writeable"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_button3, 0, wxALL, 5 );
	
	m_button4 = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_button4, 0, wxALL, 5 );
	
	fgSizer1->Add( bSizer3, 1, wxALIGN_CENTER, 5 );

    if (m_button2 != NULL)
    {
        m_button2->SetFocus();
    }
    else
    {
        m_button4->SetFocus();
    }
    
	this->SetSizer( fgSizer1 );
	this->Layout();

}

void ReadOnlyDialog::OnCheckOut(wxCommandEvent& event)
{
    EndModal(ID_CheckOut);
}

void ReadOnlyDialog::OnMakeWriteable(wxCommandEvent& event)
{
    EndModal(ID_MakeWriteable);
}