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

#include "ExternalToolsDialog.h"
#include "ExternalTool.h"
#include "StlUtility.h"
#include "ShowHelpEvent.h"

BEGIN_EVENT_TABLE(ExternalToolsDialog, wxDialog)
    EVT_LISTBOX(ID_ListBox,                 OnListBoxChoiceSelected)
    EVT_TEXT(ID_TitleTextBox,               OnTitleTextBoxChanged)
    EVT_TEXT(ID_CommandTextBox,             OnCommandTextBoxChanged)
    EVT_TEXT(ID_ArgumentsTextBox,           OnArgumentsTextBoxChanged)
    EVT_TEXT(ID_InitialDirectoryTextBox,    OnInitialDirectoryTextBoxChanged)
    EVT_BUTTON(wxID_OK,                     OnOk)
    EVT_BUTTON(ID_ApplyButton,              OnApply)
    EVT_BUTTON(ID_AddButton,                OnAdd)
    EVT_BUTTON(ID_DeleteButton,             OnDelete)
    EVT_BUTTON(ID_MoveUpButton,             OnMoveUp)
    EVT_BUTTON(ID_MoveDownButton,           OnMoveDown)
    EVT_BUTTON(ID_CommandBrowseButton,      OnCommandBrowse)
    EVT_BUTTON(ID_ArgumentsBrowseButton,    OnArgumentsBrowse)
    EVT_BUTTON(ID_InitialDirBrowseButton,   OnInitialDirectoryBrowse)
    EVT_MENU(ID_ArgumentsItemPath,             OnArgumentsItemPath)
    EVT_MENU(ID_ArgumentsItemDirectory,        OnArgumentsItemDirectory)
    EVT_MENU(ID_ArgumentsItemFileName,         OnArgumentsItemFileName)
    EVT_MENU(ID_ArgumentsItemExtension,        OnArgumentsItemExtension)
    EVT_MENU(ID_InitialDirectoryItemPath,      OnInitialDirectoryItemPath)
    EVT_MENU(ID_InitialDirectoryItemDirectory, OnInitialDirectoryItemDirectory)
    EVT_MENU(ID_InitialDirectoryItemFileName,  OnInitialDirectoryItemFileName)
    EVT_MENU(ID_InitialDirectoryItemExtension, OnInitialDirectoryItemExtension)
    EVT_CHOICE(ID_EventHook,                OnEventHook)
    EVT_HELP(wxID_ANY,                      OnHelp) 
END_EVENT_TABLE()

ExternalToolsDialog::ExternalToolsDialog( wxWindow* parent, int id)
    : wxDialog( parent, id, wxT("External Tools"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER )
{
	
    //SetExtraStyle(wxWS_EX_CONTEXTHELP);

    wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 3, 1, 0, 0 );
	fgSizer1->AddGrowableCol( 0 );
	fgSizer1->AddGrowableRow( 0 );
	fgSizer1->SetFlexibleDirection( wxHORIZONTAL|wxVERTICAL );
	
	wxFlexGridSizer* fgSizer4;
	fgSizer4 = new wxFlexGridSizer( 2, 1, 0, 0 );
	fgSizer4->AddGrowableCol( 0 );
	fgSizer4->AddGrowableRow( 1 );
	fgSizer4->SetFlexibleDirection( wxHORIZONTAL|wxVERTICAL );
	
	fgSizer4->Add( new wxStaticText( this, wxID_ANY, wxT("Menu Contents:")), 0, wxALL, 5 );
	
	wxFlexGridSizer* fgSizer5;
	fgSizer5 = new wxFlexGridSizer( 1, 2, 0, 0 );
	fgSizer5->AddGrowableCol( 0 );
	fgSizer5->AddGrowableRow( 0 );
	fgSizer5->SetFlexibleDirection( wxHORIZONTAL|wxVERTICAL );

	m_listBox = new wxListBox(this, ID_ListBox, wxDefaultPosition, wxSize(300, -1), 0, NULL, wxLB_ALWAYS_SB);
	fgSizer5->Add( m_listBox, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	m_button5 = new wxButton( this, ID_AddButton, wxT("&Add"));
	bSizer2->Add( m_button5, 0, wxALL, 5 );
	
	m_button6 = new wxButton( this, ID_DeleteButton, wxT("&Delete"));
	bSizer2->Add( m_button6, 0, wxALL, 5 );
	
	m_button7 = new wxButton( this, ID_MoveUpButton, wxT("Move &Up"));
	bSizer2->Add( m_button7, 0, wxALL, 5 );
	
	m_button8 = new wxButton( this, ID_MoveDownButton, wxT("&Move Down"));
	bSizer2->Add( m_button8, 0, wxALL, 5 );
	
	fgSizer5->Add( bSizer2, 1, wxEXPAND, 5 );
	
	fgSizer4->Add( fgSizer5, 1, wxEXPAND, 5 );
	
	fgSizer1->Add( fgSizer4, 1, wxEXPAND | wxALL, 10 );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 4, 2, 0, 0 );
	fgSizer2->AddGrowableCol( 1 );
	fgSizer2->SetFlexibleDirection( wxHORIZONTAL );
	
	fgSizer2->Add( new wxStaticText(this, wxID_ANY, wxT("Title:")), 0, wxALL, 5 );
	
	m_titleTextBox = new wxTextCtrl( this, ID_TitleTextBox);
	fgSizer2->Add( m_titleTextBox, 0, wxALL|wxEXPAND, 5 );
	
    // Command.
	fgSizer2->Add( new wxStaticText( this, wxID_ANY, wxT("Command:")), 0, wxALL, 5 );
	
	wxFlexGridSizer* fgSizer6;
	fgSizer6 = new wxFlexGridSizer( 1, 2, 0, 0 );
	fgSizer6->AddGrowableCol( 0 );
	fgSizer6->SetFlexibleDirection( wxHORIZONTAL );
	
	m_commandTextBox = new wxTextCtrl( this, ID_CommandTextBox);
	fgSizer6->Add( m_commandTextBox, 0, wxALL|wxEXPAND, 5 );
	
	m_commandBrowseButton = new wxButton( this, ID_CommandBrowseButton, wxT("..."), wxDefaultPosition, wxSize( 25,-1 ), 0 );
	fgSizer6->Add( m_commandBrowseButton, 0, wxALL, 5 );
	
	fgSizer2->Add( fgSizer6, 1, wxEXPAND, 5 );
	
    // Arguments.
	fgSizer2->Add( new wxStaticText( this, wxID_ANY, wxT("Arguments:")), 0, wxALL, 5 );
	
	wxFlexGridSizer* fgSizer8;
	fgSizer8 = new wxFlexGridSizer( 1, 2, 0, 0 );
	fgSizer8->AddGrowableCol( 0 );
	fgSizer8->SetFlexibleDirection( wxHORIZONTAL );
	
	m_argumentsTextBox = new wxTextCtrl(this, ID_ArgumentsTextBox);
	fgSizer8->Add( m_argumentsTextBox, 0, wxALL|wxEXPAND, 5 );
	
	m_argumentsBrowseButton = new wxButton(this, ID_ArgumentsBrowseButton, wxT(">"), wxDefaultPosition, wxSize( 25,-1 ), 0 );
	fgSizer8->Add( m_argumentsBrowseButton, 0, wxALL, 5 );

	fgSizer2->Add( fgSizer8, 1, wxEXPAND, 5 );

    // Initial directory
	fgSizer2->Add( new wxStaticText(this, wxID_ANY, wxT("Initial Directory:")), 0, wxALL, 5 );
	
	wxFlexGridSizer* fgSizer9;
	fgSizer9 = new wxFlexGridSizer( 1, 2, 0, 0 );
	fgSizer9->AddGrowableCol( 0 );
	fgSizer9->SetFlexibleDirection( wxHORIZONTAL );
	
	m_initialDirectoryTextBox = new wxTextCtrl( this, ID_InitialDirectoryTextBox);
	fgSizer9->Add( m_initialDirectoryTextBox, 0, wxALL|wxEXPAND, 5 );
	
	m_initialDirectoryBrowseButton = new wxButton( this, ID_InitialDirBrowseButton, wxT(">"), wxDefaultPosition, wxSize( 25,-1 ), 0 );
	fgSizer9->Add( m_initialDirectoryBrowseButton, 0, wxALL, 5 );
	
	fgSizer2->Add( fgSizer9, 1, wxEXPAND, 5 );

    // Hook option.
    fgSizer2->Add( new wxStaticText( this, wxID_ANY, wxT("Auto-Execute On Event:")), 0, wxALL, 5 );

    m_externalEvents[0] = wxString("None");
    m_externalEvents[1] = wxString("File Save");
    m_eventHookList = new wxChoice(this, ID_EventHook, wxDefaultPosition, wxDefaultSize, 2, m_externalEvents);
    m_eventHookList->Select(0);
    fgSizer2->Add( m_eventHookList, 1, wxALL|wxEXPAND, 5 );
	
	fgSizer1->Add( fgSizer2, 1, wxEXPAND | wxALL, 5 );
	
    // Bottom buttons.
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button12 = new wxButton(this, wxID_OK, wxT("&Ok"));
	bSizer3->Add( m_button12, 0, wxALL, 5 );
	
	m_button13 = new wxButton(this, wxID_CANCEL, wxT("&Cancel"));
	bSizer3->Add( m_button13, 0, wxALL, 5 );
	
	m_button14 = new wxButton(this, ID_ApplyButton, wxT("&Apply"));
	bSizer3->Add( m_button14, 0, wxALL, 5 );
	
	fgSizer1->Add( bSizer3, 1, wxALIGN_RIGHT | wxALL, 5 );
	
	SetSizer( fgSizer1 );
	
    wxSize minSize = fgSizer1->GetMinSize();

    SetClientSize(minSize);
    SetMinSize(GetSize());

    Layout();

}

ExternalToolsDialog::~ExternalToolsDialog()
{
    ClearVector(m_workingTools);
}

void ExternalToolsDialog::SetTools(std::vector<ExternalTool*>* tools)
{

    m_appliedTools = tools;
    CopyVector(m_workingTools, *m_appliedTools);

    // Add the working tools to the list.

    for (unsigned int i = 0; i < m_workingTools.size(); ++i)
    {
        m_listBox->Append(m_workingTools[i]->GetTitle());
    }

    int selectedItem = 0;

    if (m_listBox->GetCount() == 0)
    {
        selectedItem = -1;
    }

    m_listBox->SetSelection(selectedItem);
    UpdateControlsForSelection(selectedItem);

}

void ExternalToolsDialog::OnListBoxChoiceSelected(wxCommandEvent& event)
{
    UpdateControlsForSelection(event.GetInt());
}

void ExternalToolsDialog::OnTitleTextBoxChanged(wxCommandEvent& event)
{

    ExternalTool* tool = GetSelectedTool();

    if (tool != NULL)
    {

        tool->SetTitle(m_titleTextBox->GetValue());
        int selectedItem = m_listBox->GetSelection();
        
        m_listBox->Delete(selectedItem);
        m_listBox->Insert(tool->GetTitle(), selectedItem);
        m_listBox->SetSelection(selectedItem);

    }

}

void ExternalToolsDialog::OnCommandTextBoxChanged(wxCommandEvent& event)
{
    ExternalTool* tool = GetSelectedTool();
    if (tool != NULL)
    {
        tool->SetCommand(m_commandTextBox->GetValue());
    }
}

void ExternalToolsDialog::OnArgumentsTextBoxChanged(wxCommandEvent& event)
{
    ExternalTool* tool = GetSelectedTool();
    if (tool != NULL)
    {
        tool->SetArguments(m_argumentsTextBox->GetValue());
    }
}

void ExternalToolsDialog::OnInitialDirectoryTextBoxChanged(wxCommandEvent& event)
{
    ExternalTool* tool = GetSelectedTool();
    if (tool != NULL)
    {
        tool->SetInitialDirectory(m_initialDirectoryTextBox->GetValue());
    }
}

void ExternalToolsDialog::OnOk(wxCommandEvent& event)
{
    Apply();
    EndModal(wxID_OK);
}

void ExternalToolsDialog::OnApply(wxCommandEvent& event)
{
    Apply();
}

void ExternalToolsDialog::OnAdd(wxCommandEvent& event)
{
    
    ExternalTool* tool = new ExternalTool;
    m_workingTools.push_back(tool);

    m_listBox->Append(tool->GetTitle());

    int newItem = m_listBox->GetCount() - 1;

    m_listBox->SetSelection(newItem);
    UpdateControlsForSelection(newItem);

    m_titleTextBox->SetFocus();

}

void ExternalToolsDialog::OnDelete(wxCommandEvent& event)
{

    int selectedItem = m_listBox->GetSelection();

    if (selectedItem != -1)
    {

        m_listBox->Delete(selectedItem);

        delete m_workingTools[selectedItem];
        m_workingTools.erase(m_workingTools.begin() + selectedItem);
    
        if (selectedItem >= static_cast<int>(m_listBox->GetCount()))
        {
            selectedItem = m_listBox->GetCount() - 1;
        }
        
        m_listBox->SetSelection(selectedItem);
        UpdateControlsForSelection(selectedItem);

    }

}

void ExternalToolsDialog::OnMoveUp(wxCommandEvent& event)
{

    int selectedItem = m_listBox->GetSelection();

    if (selectedItem != -1 && selectedItem > 0)
    {

        const ExternalTool* tool = m_workingTools[selectedItem];

        std::swap(m_workingTools[selectedItem], m_workingTools[selectedItem - 1]);

        m_listBox->Delete(selectedItem);
        m_listBox->Insert(tool->GetTitle(), selectedItem - 1);

        m_listBox->SetSelection(selectedItem - 1);
        UpdateControlsForSelection(selectedItem - 1);

    }

}

void ExternalToolsDialog::OnMoveDown(wxCommandEvent& event)
{

    int selectedItem = m_listBox->GetSelection();

    if (selectedItem != -1 && selectedItem + 1 < static_cast<int>(m_listBox->GetCount()))
    {

        const ExternalTool* tool = m_workingTools[selectedItem];

        std::swap(m_workingTools[selectedItem], m_workingTools[selectedItem + 1]);

        m_listBox->Delete(selectedItem);
        m_listBox->Insert(tool->GetTitle(), selectedItem + 1);

        m_listBox->SetSelection(selectedItem + 1);
        UpdateControlsForSelection(selectedItem + 1);

    }

}

void ExternalToolsDialog::OnCommandBrowse(wxCommandEvent& event)
{

    wxString command = wxFileSelector("Open", "", "", "", "Executable files (*.exe)|*.exe|All files (*.*)|*.*");

    if (!command.empty())
    {
        m_commandTextBox->SetValue(command);
    }

}

void ExternalToolsDialog::OnArgumentsBrowse(wxCommandEvent& event)
{

    wxPoint point = m_argumentsBrowseButton->GetPosition();
    point.x += m_argumentsBrowseButton->GetSize().x; 

    wxMenu menu;
    menu.Append(ID_ArgumentsItemPath,        _("Item &Path"));
    menu.Append(ID_ArgumentsItemDirectory,   _("&Item Directory"));
    menu.Append(ID_ArgumentsItemFileName,    _("Item &File Name"));
    menu.Append(ID_ArgumentsItemExtension,   _("Item E&xtension"));
    
    PopupMenu(&menu, point);

}

void ExternalToolsDialog::OnInitialDirectoryBrowse(wxCommandEvent& event)
{

    wxPoint point = m_initialDirectoryBrowseButton->GetPosition();
    point.x += m_initialDirectoryBrowseButton->GetSize().x; 

    wxMenu menu;
    menu.Append(ID_InitialDirectoryItemPath,        _("Item &Path"));
    menu.Append(ID_InitialDirectoryItemDirectory,   _("&Item Directory"));
    menu.Append(ID_InitialDirectoryItemFileName,    _("Item &File Name"));
    menu.Append(ID_InitialDirectoryItemExtension,   _("Item E&xtension"));
    
    PopupMenu(&menu, point);

}

void ExternalToolsDialog::OnArgumentsItemPath     (wxCommandEvent& event) { m_argumentsTextBox->WriteText("$(ItemPath)"    ); }
void ExternalToolsDialog::OnArgumentsItemDirectory(wxCommandEvent& event) { m_argumentsTextBox->WriteText("$(ItemDir)"     ); }
void ExternalToolsDialog::OnArgumentsItemFileName (wxCommandEvent& event) { m_argumentsTextBox->WriteText("$(ItemFileName)"); }
void ExternalToolsDialog::OnArgumentsItemExtension(wxCommandEvent& event) { m_argumentsTextBox->WriteText("$(ItemExt)"     ); }

void ExternalToolsDialog::OnInitialDirectoryItemPath     (wxCommandEvent& event) { m_initialDirectoryTextBox->WriteText("$(ItemPath)"    ); }
void ExternalToolsDialog::OnInitialDirectoryItemDirectory(wxCommandEvent& event) { m_initialDirectoryTextBox->WriteText("$(ItemDir)"     ); }
void ExternalToolsDialog::OnInitialDirectoryItemFileName (wxCommandEvent& event) { m_initialDirectoryTextBox->WriteText("$(ItemFileName)"); }
void ExternalToolsDialog::OnInitialDirectoryItemExtension(wxCommandEvent& event) { m_initialDirectoryTextBox->WriteText("$(ItemExt)"     ); }

void ExternalToolsDialog::OnEventHook(wxCommandEvent& event)
{
    ExternalTool* tool = GetSelectedTool();
    if (tool != NULL)
    {
        tool->SetEvent(m_externalEvents[m_eventHookList->GetSelection()]);
    }
}

void ExternalToolsDialog::UpdateControlsForSelection(int selectedItem)
{

    if (selectedItem != -1)
    {
        const ExternalTool* tool = m_workingTools[selectedItem];
        
        m_titleTextBox->SetValue(tool->GetTitle());
        m_commandTextBox->SetValue(tool->GetCommand());
        m_argumentsTextBox->SetValue(tool->GetArguments());
        m_initialDirectoryTextBox->SetValue(tool->GetInitialDirectory());

        int selectionIndex = 0;
        int numEventTypes = sizeof(m_externalEvents) / sizeof(wxString);
        for (int i = 0; i < numEventTypes; i++)
        {
            if (tool->GetEvent() == m_externalEvents[i])
            {
                selectionIndex = i;
                break;
            }
        }
        m_eventHookList->SetSelection(selectionIndex);
    }
    else
    {
        m_titleTextBox->Clear();
        m_commandTextBox->Clear();
        m_argumentsTextBox->Clear();
        m_initialDirectoryTextBox->Clear();
        m_eventHookList->SetSelection(0);
    }
    
    m_titleTextBox                ->Enable(selectedItem != -1);
    m_commandTextBox              ->Enable(selectedItem != -1);
    m_argumentsTextBox            ->Enable(selectedItem != -1);
    m_initialDirectoryTextBox     ->Enable(selectedItem != -1);
    m_eventHookList               ->Enable(selectedItem != -1);
    m_commandBrowseButton         ->Enable(selectedItem != -1);
    m_argumentsBrowseButton       ->Enable(selectedItem != -1);
    m_initialDirectoryBrowseButton->Enable(selectedItem != -1);
}

ExternalTool* ExternalToolsDialog::GetSelectedTool()
{
 
    int selectedItem = m_listBox->GetSelection();

    if (selectedItem != -1)
    {
        return m_workingTools[selectedItem];
    }

    return NULL;

}

void ExternalToolsDialog::Apply()
{

    // If a tool doesn't have a name, give it one since this prevents
    // us from putting it in the menu.
        
    int selectedItem = m_listBox->GetSelection();

    for (unsigned int i = 0; i < m_workingTools.size(); ++i)
    {

        ExternalTool* tool = m_workingTools[i];

        if (tool->GetTitle().IsEmpty())
        {

            tool->SetTitle("Unnamed Tool");
            
            m_listBox->Delete(i);
            m_listBox->Insert(tool->GetTitle(), i);

        }
    }

    m_listBox->SetSelection(selectedItem);
    UpdateControlsForSelection(selectedItem);

    // Apply the changes.

    CopyVector(*m_appliedTools, m_workingTools);

}

void ExternalToolsDialog::OnHelp(wxHelpEvent&)
{
    wxCommandEvent event( wxEVT_SHOW_HELP_EVENT, GetId() );
    event.SetEventObject( this );
    event.SetString( wxT("External Tools") );
    GetParent()->GetEventHandler()->ProcessEvent( event );
}