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

#include "KeyBinderDialog.h"
#include "KeyMonitorTextCtrl.h"
#include "StlUtility.h"

#include <hash_map>
#include <algorithm>
#include <assert.h>

BEGIN_EVENT_TABLE(KeyBinderDialog, wxPanel)
    EVT_TREE_SEL_CHANGED( KeyBinderDialog::ID_CommandTree,  KeyBinderDialog::OnCommandTreeSelectionChanged)
    EVT_BUTTON( KeyBinderDialog::ID_Add,                    KeyBinderDialog::OnAdd)
    EVT_BUTTON( KeyBinderDialog::ID_Remove,                 KeyBinderDialog::OnRemove)
    EVT_BUTTON( KeyBinderDialog::ID_RemoveAll,              KeyBinderDialog::OnRemoveAll)
    EVT_LISTBOX( KeyBinderDialog::ID_ShortcutsList,         KeyBinderDialog::OnShortcutsListItemSelected)
    EVT_TEXT(   KeyBinderDialog::ID_KeyMonitor,             KeyBinderDialog::OnKeyMonitorTextChanged)
END_EVENT_TABLE()

KeyBinderDialog::KeyBinderDialog(wxWindow* parent, int id, wxPoint pos, wxSize size, int style)
    : wxPanel( parent, id, pos, size, style )
{

	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 2, 1, 0, 0 );
	fgSizer1->AddGrowableCol( 0 );
	fgSizer1->AddGrowableRow( 0 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 1, 2, 0, 0 );
	fgSizer2->AddGrowableCol( 0 );
	fgSizer2->AddGrowableRow( 0 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_NONE );
	
	wxFlexGridSizer* fgSizer3;
	fgSizer3 = new wxFlexGridSizer( 2, 1, 0, 0 );
	fgSizer3->AddGrowableCol( 0 );
	fgSizer3->AddGrowableRow( 1 );
	fgSizer3->SetFlexibleDirection( wxBOTH );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, wxT("Commands:"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( m_staticText1, 0, wxALL, 5 );
	
	m_commandTreeCtrl = new wxTreeCtrl( this, ID_CommandTree, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE);
	fgSizer3->Add( m_commandTreeCtrl, 0, wxALL|wxEXPAND, 5 );
	
	fgSizer2->Add( fgSizer3, 1, wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer5;
	fgSizer5 = new wxFlexGridSizer( 8, 1, 0, 0 );
	fgSizer5->AddGrowableRow( 1 );
	fgSizer5->SetFlexibleDirection( wxVERTICAL );
	
	m_staticText2 = new wxStaticText( this, wxID_ANY, wxT("Current Shortcuts:"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer5->Add( m_staticText2, 0, wxALL, 5 );
	
	m_shortcutsListCtrl = new wxListBox( this, ID_ShortcutsList, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	fgSizer5->Add( m_shortcutsListCtrl, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxHORIZONTAL );
	
	m_removeButton = new wxButton( this, ID_Remove, wxT("Remove"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer1->Add( m_removeButton, 0, wxALL, 5 );
	
	m_removeAllButton = new wxButton( this, ID_RemoveAll, wxT("Remove All"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer1->Add( m_removeAllButton, 0, wxALL, 5 );
	
	fgSizer5->Add( bSizer1, 1, wxEXPAND, 5 );
	
	m_staticText3 = new wxStaticText( this, wxID_ANY, wxT("New:"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer5->Add( m_staticText3, 0, wxALL, 5 );
	
	m_keyMonitorCtrl = new KeyMonitorTextCtrl( this, ID_KeyMonitor, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer5->Add( m_keyMonitorCtrl, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText4 = new wxStaticText( this, wxID_ANY, wxT("Currently Assigned:"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer5->Add( m_staticText4, 0, wxALL, 5 );
	
    m_currentlyAssignedCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	fgSizer5->Add( m_currentlyAssignedCtrl, 0, wxALL|wxEXPAND, 5 );
	
	m_addButton = new wxButton( this, ID_Add, wxT("Add"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer5->Add( m_addButton, 0, wxALL|wxEXPAND, 5 );
	
	fgSizer2->Add( fgSizer5, 1, wxEXPAND, 5 );
	
	fgSizer1->Add( fgSizer2, 1, wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer6;
	fgSizer6 = new wxFlexGridSizer( 3, 1, 0, 0 );
	fgSizer6->AddGrowableCol( 0 );
	fgSizer6->AddGrowableRow( 1 );
	fgSizer6->SetFlexibleDirection( wxBOTH );
	
	m_staticText9 = new wxStaticText( this, wxID_ANY, wxT("Description:"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer6->Add( m_staticText9, 0, wxALL, 5 );
	
	m_descriptionCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	m_descriptionCtrl->SetMinSize( wxSize( -1,50 ) );
	
	fgSizer6->Add( m_descriptionCtrl, 1, wxALL|wxEXPAND, 5 );

	fgSizer1->Add( fgSizer6, 1, wxEXPAND, 5 );
	
	this->SetSizer( fgSizer1 );
	this->Layout();

}

KeyBinderDialog::~KeyBinderDialog()
{
    ClearVector(m_commands);
}

void KeyBinderDialog::Initialize()
{
    
    // Add the commands to the tree.

    wxTreeItemId root = m_commandTreeCtrl->AddRoot("Commands");
    stdext::hash_map<std::string, wxTreeItemId> groups;

    for (unsigned int i = 0; i < m_commands.size(); ++i)
    {

        stdext::hash_map<std::string, wxTreeItemId>::const_iterator iterator;
        iterator = groups.find(m_commands[i]->group.ToAscii());

        wxTreeItemId groupNode;

        if (iterator == groups.end())
        {
            // We haven't encountered this group yet, so create a new node for it.
            groupNode = m_commandTreeCtrl->AppendItem(root, m_commands[i]->group);
            groups.insert(std::make_pair(m_commands[i]->group.ToAscii(), groupNode));
        }
        else
        {
            groupNode = iterator->second;
        }

        CommandData* data = new CommandData;
        data->command = m_commands[i];
    
        // Remove the & characters from the label.
        wxString label = m_commands[i]->name;
        label.Replace("&", "");

        m_commandTreeCtrl->AppendItem(groupNode, label, -1, -1, data);

    }

    m_commandTreeCtrl->Expand(root);

}

void KeyBinderDialog::OnCommandTreeSelectionChanged(wxTreeEvent& event)
{

    m_shortcutsListCtrl->Clear();
    m_descriptionCtrl->Clear();
    m_keyMonitorCtrl->Clear();

    CommandData* data = static_cast<CommandData*>(m_commandTreeCtrl->GetItemData(event.GetItem()));

    if (data != NULL)
    {

        KeyBinder::Command* command = data->command;

        for (unsigned int i = 0; i < command->keys.size(); ++i)
        {
            wxString text = KeyBinder::GetKeyBindingAsText(command->keys[i]);
            m_shortcutsListCtrl->Append(text);
        }

        m_descriptionCtrl->AppendText(command->help);

    }

    // Enable or disable the controls based on whether or not we have a command
    // selected.

    m_keyMonitorCtrl->Enable(data != NULL);
    m_shortcutsListCtrl->Enable(data != NULL);
    m_removeAllButton->Enable(data != NULL);
    m_removeButton->Enable(false);
    m_removeAllButton->Enable(!m_shortcutsListCtrl->IsEmpty());

}

void KeyBinderDialog::OnAdd(wxCommandEvent& event)
{

    wxTreeItemId item = m_commandTreeCtrl->GetSelection();

    if (item.IsOk())
    {

        CommandData* data = static_cast<CommandData*>(m_commandTreeCtrl->GetItemData(item));
        assert(data != NULL);

        if (data != NULL)
        {
            data->command->keys.push_back(KeyBinder::GetTextAsKeyBinding(m_keyMonitorCtrl->GetValue()));
            m_shortcutsListCtrl->Append(m_keyMonitorCtrl->GetValue());
            m_removeAllButton->Enable(true);
        }

    }

}

void KeyBinderDialog::OnRemove(wxCommandEvent& event)
{

    wxTreeItemId item = m_commandTreeCtrl->GetSelection();

    if (item.IsOk())
    {

        CommandData* data = static_cast<CommandData*>(m_commandTreeCtrl->GetItemData(item));
        assert(data != NULL);

        if (data != NULL)
        {
            
            unsigned int index = m_shortcutsListCtrl->GetSelection();
            assert(index != wxNOT_FOUND);

            data->command->keys.erase(data->command->keys.begin() + index);
            m_shortcutsListCtrl->Delete(index);

            m_removeButton->Enable(false);
            m_removeAllButton->Enable(!m_shortcutsListCtrl->IsEmpty());

        }

    }

}

void KeyBinderDialog::OnRemoveAll(wxCommandEvent& event)
{

    wxTreeItemId item = m_commandTreeCtrl->GetSelection();

    if (item.IsOk())
    {

        CommandData* data = static_cast<CommandData*>(m_commandTreeCtrl->GetItemData(item));
        assert(data != NULL);

        if (data != NULL)
        {

            data->command->keys.clear();
            m_shortcutsListCtrl->Clear();

            m_removeButton->Enable(false);
            m_removeAllButton->Enable(false);

        }

    }

}

void KeyBinderDialog::OnShortcutsListItemSelected(wxCommandEvent& event)
{
    m_removeButton->Enable(m_shortcutsListCtrl->GetSelection() != wxNOT_FOUND);
}

void KeyBinderDialog::OnKeyMonitorTextChanged(wxCommandEvent& event)
{

    m_currentlyAssignedCtrl->SetLabel("");

    // Find what the key is currently bound to (if anything).

    KeyBinder::Key key = KeyBinder::GetTextAsKeyBinding(m_keyMonitorCtrl->GetValue());
    m_addButton->Enable(key.code != 0);

    for (unsigned int i = 0; i < m_commands.size(); ++i)
    {
        for (unsigned int j = 0; j < m_commands[i]->keys.size(); ++j)
        {
            if (m_commands[i]->keys[j].code  == key.code &&
                m_commands[i]->keys[j].flags == key.flags)
            {
                m_currentlyAssignedCtrl->SetLabel(m_commands[i]->name);
                return;
            }
        }
    }
    
}

void KeyBinderDialog::AddCommand(const KeyBinder::Command& command)
{
    m_commands.push_back(new KeyBinder::Command(command));
}

unsigned int KeyBinderDialog::GetNumCommands() const
{
    return m_commands.size();
}

const KeyBinder::Command& KeyBinderDialog::GetCommand(unsigned int i) const
{
    return *m_commands[i];
}

