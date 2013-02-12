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

#include "Config.h"
#include "AboutDialog.h"
#include "MainApp.h"

AboutDialog::AboutDialog( wxWindow* parent ) :
    wxDialog( parent, wxID_ANY, wxT("About"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE )
{

	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 2, 1, 0, 0 );
	fgSizer1->AddGrowableCol( 0 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	
	wxFlexGridSizer* fgSizer3;
	fgSizer3 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer3->AddGrowableRow( 0 );
	fgSizer3->AddGrowableRow( 1 );
	fgSizer3->SetFlexibleDirection( wxBOTH );
	
	fgSizer3->SetMinSize(wxSize( -1,70 )); 
	m_bitmap31 = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( -1,-1 ), 0 );
	m_bitmap31->SetIcon(wxICON(decodaIcon));
    fgSizer3->Add( m_bitmap31, 0, wxALL, 15 );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText3 = new wxStaticText( this, wxID_ANY, wxT("Decoda"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->SetFont( wxFont( 14, 74, 90, 92, false, wxT("Tahoma") ) );
	
	bSizer4->Add( m_staticText3, 0, wxALL, 5 );
	
    wxString message = MainApp::s_versionDesc;
    
#ifdef DEDICATED_PRODUCT_VERSION
    message += wxString::Format( wxT("\n%s edition"), GetDedicatedProductName() );
#endif

	m_staticText13 = new wxStaticText( this, wxID_ANY, message, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_staticText13, 0, wxALL, 5 );
	
	fgSizer3->Add( bSizer4, 1, wxALIGN_CENTER_VERTICAL, 5 );
	
	fgSizer1->Add( fgSizer3, 1, wxALIGN_CENTER | wxALL, 15 );
	
	wxFlexGridSizer* fgSizer6;
	fgSizer6 = new wxFlexGridSizer( 1, 1, 0, 0 );
	fgSizer6->AddGrowableCol( 0 );
	fgSizer6->AddGrowableRow( 0 );
	fgSizer6->SetFlexibleDirection( wxBOTH );

	wxFlexGridSizer* fgSizer4;
	fgSizer4 = new wxFlexGridSizer( 1, 1, 0, 0 );
	fgSizer4->AddGrowableCol( 0 );
	fgSizer4->AddGrowableRow( 0 );
	fgSizer4->SetFlexibleDirection( wxBOTH );
	
	fgSizer4->SetMinSize(wxSize( -1,50 )); 
	m_staticText15 = new wxHyperlinkCtrl( this, wxID_ANY, wxT("Unknown Worlds Entertainment"), wxT("http://www.unknownworlds.com"), wxDefaultPosition, wxSize( -1,30 ));
	m_staticText15->SetFont( wxFont( 8, 74, 90, 90, false, wxT("Tahoma") ) );
	
	fgSizer4->Add( m_staticText15, 0, wxALIGN_CENTER|wxALL, 5 );
	
	fgSizer1->Add( fgSizer4, 1, wxALIGN_CENTER, 5 );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer2->AddGrowableRow( 1 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	
	m_staticText9 = new wxStaticText( this, wxID_ANY, wxT("Created by:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText9->SetFont( wxFont( 8, 74, 90, 92, false, wxT("Tahoma") ) );
	
	fgSizer2->Add( m_staticText9, 0, wxALL, 5 );
	
	m_staticText10 = new wxStaticText( this, wxID_ANY, wxT("Max McGuire"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText10->SetFont( wxFont( 8, 74, 90, 90, false, wxT("Tahoma") ) );
	
	fgSizer2->Add( m_staticText10, 0, wxALL, 5 );
	
	m_staticText11 = new wxStaticText( this, wxID_ANY, wxT("Special thanks:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText11->SetFont( wxFont( 8, 74, 90, 92, false, wxT("Tahoma") ) );
	
	fgSizer2->Add( m_staticText11, 0, wxALL, 5 );
	
	m_staticText12 = new wxStaticText( this, wxID_ANY, wxT("Charlie Cleveland, Bram de Greve, Garry Newman, Kurt Miller,\nJames Clarendon, Dan Higgins, Paul Nettle, Peter Loveday, Jeff Petkau,\nBrian Weed and the developers of wxWidgets, Scintilla and Lua"), wxDefaultPosition, wxSize( -1,70 ), 0 );
	m_staticText12->SetFont( wxFont( 8, 74, 90, 90, false, wxT("Tahoma") ) );
	
	fgSizer2->Add( m_staticText12, 0, wxALL, 5 );
	
	fgSizer1->Add( fgSizer2, 1, wxALIGN_CENTER|wxALL, 10 );
	
	m_sdbSizer2 = new wxStdDialogButtonSizer();
	m_sdbSizer2->AddButton( new wxButton( this, wxID_OK ) );
	m_sdbSizer2->Realize();
	fgSizer1->Add( m_sdbSizer2, 0, wxALIGN_CENTER|wxALL, 10 );
	
	SetSizer( fgSizer1 );
	
    wxSize minSize = fgSizer1->GetMinSize();
    SetClientSize(minSize);
    SetMinSize(GetSize());
    SetMaxSize(GetSize());
    
    Layout();

}