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
#include "NewProcessDialog.h"
#include "SourceControl.h"
#include "MainFrame.h"

#include "wx/filedlg.h"
#include "wx/filename.h"

BEGIN_EVENT_TABLE(NewProcessDialog, wxDialog)
    EVT_BUTTON(NewProcessDialog::ID_BrowseCommandLine,      NewProcessDialog::OnBrowseCommandLine)
    EVT_BUTTON(NewProcessDialog::ID_BrowseWorkingDirectory, NewProcessDialog::OnBrowseWorkingDirectory)
    EVT_BUTTON(NewProcessDialog::ID_BrowseSymbolsDirectory, NewProcessDialog::OnBrowseSymbolsDirectory)
    EVT_BUTTON(NewProcessDialog::ID_BrowseSccProject,       NewProcessDialog::OnBrowseSccProject)
    EVT_CHOICE(NewProcessDialog::ID_SelectScc,              NewProcessDialog::OnSelectScc)
END_EVENT_TABLE()

NewProcessDialog::NewProcessDialog( wxWindow* parent )
    : wxDialog( parent, -1, wxT("Project Settings"), wxDefaultPosition, wxSize( 548,335 ), wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER )
{

    m_symbolsDirectoryBox   = NULL;
    m_workingDirectoryBox   = NULL;
    m_commandLineBox        = NULL;

	wxFlexGridSizer* fgSizer17;
	fgSizer17 = new wxFlexGridSizer( 3, 1, 0, 0 );
	fgSizer17->AddGrowableCol( 0 );
	fgSizer17->AddGrowableRow( 2 );
	fgSizer17->SetFlexibleDirection( wxBOTH );
	
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( this, -1, wxT("Debugging") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 2, 3, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxHORIZONTAL );

    const int textBoxSize = 350;

    wxStaticText* staticText = NULL;
    
#ifndef DEDICATED_PRODUCT_VERSION

    // Command controls.
	
	staticText = new wxStaticText( this, wxID_ANY, wxT("Command:"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( staticText, 0, wxALL, 5 );
	
	m_commandLineBox = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(textBoxSize, -1), 0 );
	fgSizer1->Add( m_commandLineBox, 0, wxALL|wxEXPAND, 5 );
	
	m_button1 = new wxButton( this, ID_BrowseCommandLine, wxT("..."), wxDefaultPosition, wxSize( 23,-1 ), 0 );
	fgSizer1->Add( m_button1, 0, wxALL, 5 );

#endif

    // Command line arguments controls.
	
	staticText = new wxStaticText( this, wxID_ANY, wxT("Command Arguments:"), wxDefaultPosition, wxSize( 110,-1 ), 0 );
	fgSizer1->Add( staticText, 0, wxALL, 5 );
	
	m_commandArgumentsBox = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(textBoxSize, -1), 0 );
	fgSizer1->Add( m_commandArgumentsBox, 0, wxALL|wxEXPAND, 5 );
	
	fgSizer1->Add( 0, 0, 1, wxEXPAND, 0 );

#ifndef DEDICATED_PRODUCT_VERSION

    // Working directory controls.
	
	staticText = new wxStaticText( this, wxID_ANY, wxT("Working Directory:"), wxDefaultPosition, wxSize( 90,-1 ), 0 );
	fgSizer1->Add( staticText, 0, wxALL, 5 );
	
	m_workingDirectoryBox = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(textBoxSize, -1), 0 );
	fgSizer1->Add( m_workingDirectoryBox, 0, wxALL|wxEXPAND, 5 );
	
	m_button21 = new wxButton( this, ID_BrowseWorkingDirectory, wxT("..."), wxDefaultPosition, wxSize( 23,-1 ), 0 );
	fgSizer1->Add( m_button21, 0, wxALL, 5 );

    // Symbols directory controls.
	
	staticText = new wxStaticText( this, wxID_ANY, wxT("Symbols Directory:"), wxDefaultPosition, wxSize( 90,-1 ), 0 );
	fgSizer1->Add( staticText, 0, wxALL, 5 );
	
	m_symbolsDirectoryBox = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(textBoxSize, -1), 0 );
	fgSizer1->Add( m_symbolsDirectoryBox, 0, wxALL|wxEXPAND, 5 );
	
	m_button21 = new wxButton( this, ID_BrowseSymbolsDirectory, wxT("..."), wxDefaultPosition, wxSize( 23,-1 ), 0 );
	fgSizer1->Add( m_button21, 0, wxALL, 5 );

#endif
	
	sbSizer1->Add( fgSizer1, 1, wxEXPAND, 5 );
	
	fgSizer17->Add( sbSizer1, 1, wxALL|wxEXPAND, 10 );
	
	wxStaticBoxSizer* sbSizer3;
	sbSizer3 = new wxStaticBoxSizer( new wxStaticBox( this, -1, wxT("Source Control") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer4;
	fgSizer4 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer4->AddGrowableCol( 1 );
	fgSizer4->SetFlexibleDirection( wxHORIZONTAL );
	fgSizer4->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_NONE );
	
	staticText = new wxStaticText( this, wxID_ANY, wxT("Provider:"), wxDefaultPosition, wxSize( 110,-1 ), 0 );
	fgSizer4->Add( staticText, 0, wxALL, 5 );
	
	wxString m_choice1Choices[] = { "None"  };
	int m_choice1NChoices = sizeof( m_choice1Choices ) / sizeof( wxString );
	m_sccProviderChoice = new wxChoice( this, ID_SelectScc, wxDefaultPosition, wxDefaultSize, m_choice1NChoices, m_choice1Choices, 0 );
	fgSizer4->Add( m_sccProviderChoice, 0, wxALL|wxEXPAND, 5 );
	
	staticText = new wxStaticText( this, wxID_ANY, wxT("Project:"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer4->Add( staticText, 0, wxALL, 5 );
	
	wxFlexGridSizer* fgSizer5;
	fgSizer5 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer5->AddGrowableCol( 0 );
	fgSizer5->SetFlexibleDirection( wxHORIZONTAL );
	
	m_sccProjectBox = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	fgSizer5->Add( m_sccProjectBox, 0, wxALL|wxEXPAND, 5 );
	
	m_sccProjectBrowseButton = new wxButton( this, ID_BrowseSccProject, wxT("Browse..."), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer5->Add( m_sccProjectBrowseButton, 0, wxALL, 5 );
	
	fgSizer4->Add( fgSizer5, 1, wxEXPAND, 5 );
	
	sbSizer3->Add( fgSizer4, 1, wxEXPAND, 5 );
	
	fgSizer17->Add( sbSizer3, 1, wxALL|wxEXPAND, 10 );
	
	m_sdbSizer1 = new wxStdDialogButtonSizer();
	m_sdbSizer1->AddButton( new wxButton( this, wxID_OK ) );
	m_sdbSizer1->AddButton( new wxButton( this, wxID_CANCEL ) );
	m_sdbSizer1->Realize();
	fgSizer17->Add( m_sdbSizer1, 0, wxALIGN_RIGHT|wxALL, 10 );
	    
    // Add the source control providers to the dropdown list.
    for (unsigned int i = 0; i < SourceControl::GetNumProviders(); ++i)
    {
        m_sccProviderChoice->Append(SourceControl::GetProviderName(i));
    }

    m_sccProviderChoice->SetSelection(0);
    UpdateSccControls();

	SetSizer( fgSizer17 );

    wxSize minSize = fgSizer17->GetMinSize();
    SetClientSize(minSize);

    SetMinSize(GetSize());
    SetMaxSize(wxSize(-1, GetSize().y));

	Layout();

}
    
void NewProcessDialog::OnBrowseCommandLine(wxCommandEvent& event)
{

    wxString fileName = wxFileSelector("Open", "", m_commandLineBox->GetValue(), "", "Executable files (*.exe)|*.exe|All files (*.*)|*.*", 0, this);

    if (!fileName.empty())
    {

        wxASSERT(m_commandLineBox != NULL);
        m_commandLineBox->SetValue(fileName);
        
        wxASSERT(m_workingDirectoryBox != NULL);
        m_workingDirectoryBox->SetValue(wxFileName(fileName).GetPath());
        
    }

}
    
void NewProcessDialog::OnBrowseWorkingDirectory(wxCommandEvent& event)
{

    wxString directory = wxDirSelector("Choose the working directory", m_workingDirectoryBox->GetValue(), 0, wxDefaultPosition, this);

    if (!directory.empty())
    {
        wxASSERT(m_workingDirectoryBox != NULL);
        m_workingDirectoryBox->SetValue(directory);
    }

}

void NewProcessDialog::OnBrowseSymbolsDirectory(wxCommandEvent& event)
{

    wxString directory = wxDirSelector("Choose the symbols directory", m_symbolsDirectoryBox->GetValue(), 0, wxDefaultPosition, this);

    if (!directory.empty())
    {
        wxASSERT(m_symbolsDirectoryBox != NULL);
        m_symbolsDirectoryBox->SetValue(directory);
    }

}

void NewProcessDialog::SetCommandLine(const wxString& commandLine)
{
    if (m_commandLineBox != NULL)
    {
        m_commandLineBox->SetValue(commandLine);
    }
}

wxString NewProcessDialog::GetCommandLine() const
{
    if (m_commandLineBox != NULL)
    {
        return m_commandLineBox->GetValue();
    }
    return wxEmptyString;
}

void NewProcessDialog::SetCommandArguments(const wxString& commandArguments)
{
    m_commandArgumentsBox->SetValue(commandArguments);
}

wxString NewProcessDialog::GetCommandArguments() const
{
    return m_commandArgumentsBox->GetValue();
}

void NewProcessDialog::SetWorkingDirectory(const wxString& workingDirectory)
{
    if (m_workingDirectoryBox != NULL)
    {
        m_workingDirectoryBox->SetValue(workingDirectory);
    }
}

wxString NewProcessDialog::GetWorkingDirectory() const
{
    if (m_workingDirectoryBox != NULL)
    {
        return m_workingDirectoryBox->GetValue();
    }
    return wxEmptyString;
}

void NewProcessDialog::SetSymbolsDirectory(const wxString& symbolsDirectory)
{
    if (m_symbolsDirectoryBox != NULL)
    {
        m_symbolsDirectoryBox->SetValue(symbolsDirectory);
    }
}

wxString NewProcessDialog::GetSymbolsDirectory() const
{
    if (m_symbolsDirectoryBox != NULL)
    {
        return m_symbolsDirectoryBox->GetValue();
    }
    return wxEmptyString;
}

wxString NewProcessDialog::GetSccProvider() const
{
    
    int selection = m_sccProviderChoice->GetSelection();

    if (selection > 0)
    {
        return SourceControl::GetProviderName(selection - 1);
    }
    else
    {
        // Empty string means none.
        return wxEmptyString;
    }

}

void NewProcessDialog::SetSccProvider(const wxString& sccProvider)
{

    for (unsigned int i = 0; i < SourceControl::GetNumProviders(); ++i)
    {
        if (sccProvider == SourceControl::GetProviderName(i))
        {
            m_sccProviderChoice->SetSelection(i + 1);
            UpdateSccControls();
            return;
        }
    }

    // None.
    m_sccProviderChoice->SetSelection(0);
    UpdateSccControls();

}

wxString NewProcessDialog::GetSccUser() const
{
    return m_sccUser;
}

void NewProcessDialog::SetSccUser(const wxString& sccUser)
{
    m_sccUser = sccUser;
}

wxString NewProcessDialog::GetSccProjectName() const
{
    return m_sccProjName;
}

void NewProcessDialog::SetSccProjectName(const wxString& sccProjName)
{
    m_sccProjName = sccProjName;
    m_sccProjectBox->SetValue(m_sccProjName);
}

wxString NewProcessDialog::GetSccLocalPath() const
{
    return m_sccLocalPath;
}

void NewProcessDialog::SetSccLocalPath(const wxString& sccLocalPath)
{
    m_sccLocalPath = sccLocalPath;
}

wxString NewProcessDialog::GetSccAuxProjectPath() const
{
    return m_sccAuxProjPath;
}

void NewProcessDialog::SetSccAuxProjectPath(const wxString& sccAuxProjPath)
{
    m_sccAuxProjPath = sccAuxProjPath;
}

void NewProcessDialog::OnBrowseSccProject(wxCommandEvent& event)
{

    int selection = m_sccProviderChoice->GetSelection();

    if (selection > 0)
    {
    
        unsigned int provider = selection - 1;

        SourceControl sourceControl;
        
        if (!sourceControl.Initialize(provider, static_cast<HWND>(GetHandle())))
        {
            wxMessageBox(_("The source control plugin could not be intiailized."), MainFrame::GetApplicationName());
        }
        else if (sourceControl.SelectProject())
        {

            m_sccUser           = sourceControl.GetUser();
            m_sccProjName       = sourceControl.GetProjectName();
            m_sccLocalPath      = sourceControl.GetLocalPath();
            m_sccAuxProjPath    = sourceControl.GetAuxProjectPath();;

            m_sccProjectBox->SetValue(m_sccProjName);
        
        }
    
    }

}

void NewProcessDialog::OnSelectScc(wxCommandEvent& event)
{
    UpdateSccControls();
}

void NewProcessDialog::UpdateSccControls()
{
    bool enable = m_sccProviderChoice->GetSelection() != 0;
    
    m_sccProjectBox->Enable(enable);
    m_sccProjectBrowseButton->Enable(enable);

}