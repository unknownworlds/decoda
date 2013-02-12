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

#include "ProjectFilterPopup.h"
#include "ProjectExplorerWindow.h"

#include "res/explorer.xpm"

BEGIN_EVENT_TABLE(ProjectFilterPopup, wxPopupTransientWindow)

    EVT_CHECKBOX( wxID_ANY, OnCheckBox )

END_EVENT_TABLE()

ProjectFilterPopup::ProjectFilterPopup( wxWindow* parent, ProjectExplorerWindow* explorerWindow )
    : wxPopupTransientWindow( parent, wxBORDER_RAISED)
{

    m_explorerWindow = explorerWindow;

    wxBoxSizer* bSizer1;
    bSizer1 = new wxBoxSizer( wxVERTICAL );

    // Note, because of a bug in wxPopupTransientWindow, the first child we add to this window
    // doesn't left button down events (they are consumed by the code that checks to see if
    // the window should be dismissed). In this case it's ok since the first check is this label
    // which doesn't require events, but could lead to hair-pulling down the road.
    wxStaticText* label = new wxStaticText( this, wxID_ANY, wxT("Display:") );

    int bitmapIndex[4] = { 0 };
    
    // Temporary files.

	m_checkBox[0] = new wxCheckBox( this, wxID_ANY, wxT("Temporary") );
    m_checkBoxFlag[0] = ProjectExplorerWindow::FilterFlag_Temporary;
    bitmapIndex[0] = 3;

    // Unversioned files.
    
    m_checkBox[1] = new wxCheckBox( this, wxID_ANY, wxT("Unversioned") );
    m_checkBoxFlag[1] = ProjectExplorerWindow::FilterFlag_Unversioned;
    bitmapIndex[1] = 1;

    // Checked in files.

	m_checkBox[2] = new wxCheckBox( this, wxID_ANY, wxT("Checked In") );
    m_checkBoxFlag[2] = ProjectExplorerWindow::FilterFlag_CheckedIn;
    bitmapIndex[2] = 4;

    // Checked out files.

	m_checkBox[3] = new wxCheckBox( this, wxID_ANY, wxT("Checked Out") );
    m_checkBoxFlag[3] = ProjectExplorerWindow::FilterFlag_CheckedOut;
    bitmapIndex[3] = 6;

    bSizer1->Add( label, 0, wxALL, 5 );

    // Load the icons.

    wxBitmap bitmap(explorer, wxBITMAP_TYPE_XPM);
    
    wxImageList* imageList = new wxImageList(18, 17);
    imageList->Add(bitmap, wxColour(0xFF, 0x9B, 0x77));

    // Add the check boxes to the window.

    for (unsigned int checkBoxIndex = 0; checkBoxIndex < 4; ++checkBoxIndex)
    {

        wxBoxSizer* sizer = new wxBoxSizer( wxHORIZONTAL );

        wxBitmap bitmap = imageList->GetBitmap(bitmapIndex[checkBoxIndex]);

        // Remove the label off the check box so we can insert an image between it
        // and the check box.
        wxString label = m_checkBox[checkBoxIndex]->GetLabel();
        m_checkBox[checkBoxIndex]->SetLabel(wxEmptyString);

        sizer->Add( m_checkBox[checkBoxIndex], 0, wxALL | wxALIGN_CENTER_VERTICAL, 0 );
        sizer->AddSpacer( 2 );
        sizer->Add( new wxStaticBitmap( this, wxID_ANY, bitmap ), 0, wxALL | wxALIGN_CENTER_VERTICAL, 0 );
        sizer->AddSpacer( 2 );
        sizer->Add( new wxStaticText( this, wxID_ANY, label ), 0, wxALL | wxALIGN_CENTER_VERTICAL, 0 );

        bSizer1->Add( sizer, 0, wxALL, 5);

    }

    delete imageList;
	
	SetSizer( bSizer1 );
	Layout();

    SetClientSize( bSizer1->CalcMin() );

}

void ProjectFilterPopup::Popup(wxWindow* focus)
{

    // Initialize the state of the check boxes from the currently set flags
    // in the project explorer window.

    if (m_explorerWindow != NULL)
    {

        unsigned int flags = m_explorerWindow->GetFilterFlags();

        for (unsigned int checkBoxIndex = 0; checkBoxIndex < 4; ++checkBoxIndex)
        {
            bool checked = (flags & m_checkBoxFlag[checkBoxIndex]) != 0;
            m_checkBox[checkBoxIndex]->SetValue(checked);
        }
    
    }

    wxPopupTransientWindow::Popup(focus);

}

void ProjectFilterPopup::OnCheckBox(wxCommandEvent& event)
{

    // Build up the new flags based on the current state of the check boxes.

    unsigned int flags = 0;

    for (unsigned int checkBoxIndex = 0; checkBoxIndex < 4; ++checkBoxIndex)
    {
        if (m_checkBox[checkBoxIndex]->IsChecked())
        {
            flags |= m_checkBoxFlag[checkBoxIndex];
        }
    }

    if (m_explorerWindow != NULL)
    {
        m_explorerWindow->SetFilterFlags(flags);
    }

}