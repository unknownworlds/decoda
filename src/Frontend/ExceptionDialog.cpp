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

#include "ExceptionDialog.h"

BEGIN_EVENT_TABLE(ExceptionDialog, wxDialog)

    EVT_BUTTON(ExceptionDialog::ID_Break,           ExceptionDialog::OnBreak)
    EVT_BUTTON(ExceptionDialog::ID_Ignore,          ExceptionDialog::OnIgnore)
    EVT_BUTTON(ExceptionDialog::ID_IgnoreAlways,    ExceptionDialog::OnIgnoreAlways)

END_EVENT_TABLE()



ExceptionDialog::ExceptionDialog( wxWindow* parent, const wxString& message, bool allowIgnoreAlways )
    : wxDialog( parent, wxID_ANY, wxT("Exception"), wxDefaultPosition, wxDefaultSize, wxCAPTION|wxCLOSE_BOX|wxSYSTEM_MENU|wxRESIZE_BORDER  )
{

	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 2, 1, 0, 0 );
	fgSizer1->AddGrowableCol( 0 );
	fgSizer1->AddGrowableRow( 0 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxHORIZONTAL );
	
	m_bitmap1 = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( -1,-1 ), 0 );
	m_bitmap1->SetIcon(wxIcon("wxICON_WARNING"));
    bSizer1->Add( m_bitmap1, 0, wxALL, 10 );
	
	wxGridSizer* gSizer2;
	gSizer2 = new wxGridSizer( 1, 1, 0, 0 );
	
	m_textCtrl1 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 350,-1 ), wxTE_READONLY | wxVSCROLL | wxTE_MULTILINE );
    m_textCtrl1->SetValue("An exception has been caught:\r\n" + message);
	gSizer2->Add( m_textCtrl1, 0, wxALL|wxEXPAND, 5 );
	
	bSizer1->Add( gSizer2, 1, wxEXPAND, 5 );
	
	fgSizer1->Add( bSizer1, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button2 = new wxButton( this, ID_Break, wxT("&Break"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_button2, 0, wxALL, 5 );
	
	m_button3 = new wxButton( this, ID_Ignore, wxT("&Continue"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_button3, 0, wxALL, 5 );
	
	m_button4 = new wxButton( this, ID_IgnoreAlways, wxT("&Ignore"), wxDefaultPosition, wxDefaultSize, 0 );
    m_button4->Enable(allowIgnoreAlways);
	bSizer3->Add( m_button4, 0, wxALL, 5 );
	
	fgSizer1->Add( bSizer3, 1, wxALIGN_CENTER|wxALL, 5 );
	
	SetSizer( fgSizer1 );
	
    wxSize minSize = fgSizer1->GetMinSize();
    SetClientSize(minSize);

    SetMinSize(GetSize());
    
    Layout();

}

void ExceptionDialog::OnBreak(wxCommandEvent& event)
{
    EndModal(ID_Break);
}

void ExceptionDialog::OnIgnore(wxCommandEvent& event)
{
    EndModal(ID_Ignore);
}

void ExceptionDialog::OnIgnoreAlways(wxCommandEvent& event)
{
    EndModal(ID_IgnoreAlways);
}