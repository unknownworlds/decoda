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

#include <wx/wx.h>
#include <wx/xml/xml.h>

#include "KeyBinder.h"
#include "XmlUtility.h"
#include "StlUtility.h"

KeyBinder::KeyBinder()
{
}

KeyBinder::~KeyBinder()
{
    ClearCommands();   
}

void KeyBinder::ClearCommands()
{
    ClearVector(m_commands);
}

void KeyBinder::AddCommand(const Command& command)
{
    if (GetCommandForId(command.id) == NULL)
    {
        m_commands.push_back(new Command(command));
    }
}

void KeyBinder::AddCommand(int id, const wxString& group, const wxString& name, const wxString& help)
{
    if (GetCommandForId(id) == NULL)
    {

        // Check that the command doesn't already exist.

        Command* command = new Command;
        
        command->id     = id;
        command->name   = name;
        command->group  = group;
        command->help   = help;

        m_commands.push_back(command);

    }
}

void KeyBinder::AddCommand(const wxString& group, wxMenuItem* menuItem)
{
    if (menuItem->GetKind() != wxITEM_SEPARATOR)
    {
        AddCommand(menuItem->GetId(), group, menuItem->GetText(), menuItem->GetHelp());
    }
}

void KeyBinder::AddCommandsFromMenuBar(wxMenuBar* menuBar)
{

    int numMenus = menuBar->GetMenuCount();

    for (int i = 0; i < numMenus; ++i)
    {
        AddCommandsFromMenu(menuBar->GetLabelTop(i), menuBar->GetMenu(i));
    }

}

void KeyBinder::AddCommandsFromMenu(const wxString& group, wxMenu* menu)
{
    
    const wxMenuItemList& list = menu->GetMenuItems();

    for (wxMenuItemList::Node* node = list.GetFirst(); node != NULL; node = node->GetNext())
    {
        wxMenuItem* item = node->GetData();
        AddCommand(group, item);
    }
    
}

void KeyBinder::RemoveCommand(int id)
{

    for (unsigned int i = 0; i < m_commands.size(); ++i)
    {
        if (m_commands[i]->id == id)
        {
            delete m_commands[i];
            m_commands.erase(m_commands.begin() + i);
            break;
        }
    }

}

void KeyBinder::UpdateWindow(wxWindow* window)
{

    // Build an updated accelerator table for the commands.

    std::vector<wxAcceleratorEntry> entries;
    entries.reserve(m_commands.size());

    for (unsigned int i = 0; i < m_commands.size(); ++i)
    {
        for (unsigned int j = 0; j < m_commands[i]->keys.size(); ++j)
        {

            const Key& key = m_commands[i]->keys[j];

            wxAcceleratorEntry entry;
            entry.Set(key.flags, key.code, m_commands[i]->id);

            entries.push_back(entry);

        }
    }

    m_accel = wxAcceleratorTable(entries.size(), &entries.front());

    window->SetAcceleratorTable(m_accel);

}

void KeyBinder::UpdateMenuBar(wxMenuBar* menuBar)
{

    int numMenus = menuBar->GetMenuCount();

    for (int i = 0; i < numMenus; ++i)
    {
        UpdateMenu(menuBar->GetMenu(i));
    }

}

bool KeyBinder::GetMenuItemText(wxMenuItem* item, wxString& label) const
{

    if (item->GetKind() != wxITEM_SEPARATOR)
    {
        
        Command* command = GetCommandForId(item->GetId());

        if (command != NULL)
        {
            
            if (label.IsEmpty())
            {
                label = command->name;
            }
            
            if (!command->keys.empty())
            {
                label += "\t" + GetKeyBindingAsText(command->keys[0]);
            }

            return true;

        }

    }

    return false;

}

void KeyBinder::UpdateMenu(wxMenu* menu)
{

    const wxMenuItemList& list = menu->GetMenuItems();

    for (wxMenuItemList::Node* node = list.GetFirst(); node != NULL; node = node->GetNext())
    {

        wxMenuItem* item = node->GetData();
        wxString label;

        if (GetMenuItemText(item, label))
        {
            item->SetText(label);
        }
        
    }

}

KeyBinder::Command* KeyBinder::GetCommandForId(int id) const
{

    for (unsigned int i = 0; i < m_commands.size(); ++i)
    {
        if (m_commands[i]->id == id)
        {
            return m_commands[i];
        }
    }

    return NULL;

}

void KeyBinder::SetShortcut(int id, int flags, int code)
{

    Command* command = GetCommandForId(id);

    if (command != NULL)
    {

        Key key;

        key.flags = flags;
        key.code  = code;
        
        command->keys.push_back(key);

    }

}

void KeyBinder::SetShortcut(int id, const wxString& text)
{

    Key key = GetTextAsKeyBinding(text);
    SetShortcut(id, key.flags, key.code);

}

KeyBinder::Key KeyBinder::GetTextAsKeyBinding(const wxString& text)
{

    Key key;
    key.flags = 0;

    // this search must be case-insensitive
    wxString str = text;
    str.MakeUpper();

    if (str.Contains(wxT("ALT")))
        key.flags |= wxACCEL_ALT;

    if (str.Contains(wxT("CTRL")))
        key.flags |= wxACCEL_CTRL;

    if (str.Contains(wxT("SHIFT")))
        key.flags |= wxACCEL_SHIFT;

    key.code = StringToKeyCode(str.AfterLast('+').AfterLast('-'));
    
    return key;

}

int KeyBinder::StringToKeyCode(const wxString &keyName)
{

    // a function key ?
    if (keyName.StartsWith(wxT("F")) && keyName.Len() > 1) {

        long n;
        keyName.Right(keyName.Len()-1).ToLong(&n);
        return WXK_F1+n-1;
    }

    // a special key ?
    if (keyName == wxT("BACK")) return WXK_BACK;
    if (keyName == wxT("ENTER")) return WXK_RETURN;
    if (keyName == wxT("RETURN")) return WXK_RETURN;
    if (keyName == wxT("TAB")) return WXK_TAB;
    if (keyName == wxT("ESCAPE")) return WXK_ESCAPE;
    if (keyName == wxT("SPACE")) return WXK_SPACE;
    if (keyName == wxT("DEL") || keyName == wxT("DELETE")) return WXK_DELETE;

    // it should be an ASCII key...
    return (int)keyName.GetChar(0);
}

wxString KeyBinder::GetKeyBindingAsText(const Key& key)
{

    wxString res;

    if (key.flags & wxACCEL_CTRL)
    {
        res += "Ctrl+";
    }

    if (key.flags & wxACCEL_SHIFT)
    {
        res += "Shift+";
    }

    if (key.flags & wxACCEL_ALT)
    {
        res += "Alt+";
    }

    switch (key.code)
    {
        // IGNORED KEYS
        // ---------------------------
    case WXK_START:
    case WXK_LBUTTON:
    case WXK_RBUTTON:
    case WXK_MBUTTON:
    case WXK_CLEAR:

    case WXK_PAUSE:
    case WXK_NUMLOCK:
    case WXK_SCROLL :
        return wxEmptyString;

        // these must be ABSOLUTELY ignored: they are key modifiers
        // we won't output any LOG message since these keys could be pressed
        // for long time while the user choose its preferred keycombination:
        // this would result into a long long queue of "ignored key" messages
        // which would be useless even in debug builds...
    case WXK_SHIFT:
    case WXK_CONTROL:
        return wxEmptyString;

        // FUNCTION KEYS
        // ---------------------------

    case WXK_F1: case WXK_F2:
    case WXK_F3: case WXK_F4:
    case WXK_F5: case WXK_F6:
    case WXK_F7: case WXK_F8:
    case WXK_F9: case WXK_F10:
    case WXK_F11: case WXK_F12:
    case WXK_F13: case WXK_F14:
    case WXK_F15: case WXK_F16:
    case WXK_F17: case WXK_F18:
    case WXK_F19: case WXK_F20:
    case WXK_F21: case WXK_F22:
    case WXK_F23: case WXK_F24:
        res << wxT('F') << key.code - WXK_F1 + 1;
        break;

        // MISCELLANEOUS KEYS
        // ---------------------------

    case WXK_BACK:
        res << wxT("Back"); break;
    case WXK_TAB:
        res << wxT("Tab"); break;
    case WXK_RETURN:
        res << wxT("Return"); break;
    case WXK_ESCAPE:
        res << wxT("Escape"); break;
    case WXK_SPACE:
        res << wxT("Space"); break;
    case WXK_DELETE:
        res << wxT("Del"); break;
    case WXK_MULTIPLY:
        res << wxT("*"); break;
    case WXK_ADD:
        res << wxT("+"); break;
    case WXK_SEPARATOR:
        res << wxT("Separator"); break;
    case WXK_SUBTRACT:
        res << wxT("-"); break;
    case WXK_DECIMAL:
        res << wxT("."); break;
    case WXK_DIVIDE:
        res << wxT("/"); break;
    case WXK_PAGEUP:
        res << wxT("PageUp"); break;
    case WXK_PAGEDOWN:
        res << wxT("PageDown"); break;
    case WXK_LEFT:
        res << wxT("Left"); break;
    case WXK_UP:
        res << wxT("Up"); break;
    case WXK_RIGHT:
        res << wxT("Right"); break;
    case WXK_DOWN:
        res << wxT("Down"); break;
    case WXK_SELECT:
        res << wxT("Select"); break;
    case WXK_PRINT:
        res << wxT("Print"); break;
    case WXK_EXECUTE:
        res << wxT("Execute"); break;
    case WXK_SNAPSHOT:
        res << wxT("Snapspot"); break;
    case WXK_INSERT:
        res << wxT("Insert"); break;
    case WXK_HELP:
        res << wxT("Help"); break;
    case WXK_CANCEL:
        res << wxT("Cancel"); break;
    case WXK_MENU:
        res << wxT("Menu"); break;
    case WXK_CAPITAL:
        res << wxT("Capital"); break;
    case WXK_END:
        res << wxT("End"); break;
    case WXK_HOME:
        res << wxT("Home"); break;
    // NUMPAD KEYS
    // ---------------------------

    case WXK_NUMPAD0:
    case WXK_NUMPAD1:
    case WXK_NUMPAD2:
    case WXK_NUMPAD3:
    case WXK_NUMPAD4:
    case WXK_NUMPAD5:
    case WXK_NUMPAD6:
    case WXK_NUMPAD7:
    case WXK_NUMPAD8:
    case WXK_NUMPAD9:
        res << key.code - WXK_NUMPAD0;
        break;

    case WXK_NUMPAD_SPACE:
        res << wxT("Space"); break;
    case WXK_NUMPAD_TAB:
        res << wxT("Tab"); break;
    case WXK_NUMPAD_ENTER:
        res << wxT("Enter"); break;

    case WXK_NUMPAD_F1:
    case WXK_NUMPAD_F2:
    case WXK_NUMPAD_F3:
    case WXK_NUMPAD_F4:
        res << wxT("F") << key.code - WXK_NUMPAD_F1;
        break;

    case WXK_NUMPAD_LEFT:
        res << wxT("Keft"); break;
    case WXK_NUMPAD_UP:
        res << wxT("Up"); break;
    case WXK_NUMPAD_RIGHT:
        res << wxT("Right"); break;
    case WXK_NUMPAD_DOWN:
        res << wxT("Down"); break;

    case WXK_NUMPAD_HOME:
        res << wxT("Home"); break;
    case WXK_NUMPAD_PAGEUP:
        res << wxT("PageUp"); break;
    case WXK_NUMPAD_PAGEDOWN:
        res << wxT("PageDown"); break;
    case WXK_NUMPAD_END:
        res << wxT("End"); break;
    case WXK_NUMPAD_BEGIN:
        res << wxT("Begin"); break;
    case WXK_NUMPAD_INSERT:
        res << wxT("Insert"); break;
    case WXK_NUMPAD_DELETE:
        res << wxT("Delete"); break;
    case WXK_NUMPAD_EQUAL:
        res << wxT("="); break;
    case WXK_NUMPAD_MULTIPLY:
        res << wxT("*"); break;
    case WXK_NUMPAD_ADD:
        res << wxT("+"); break;
    case WXK_NUMPAD_SEPARATOR:
        res << wxT("Separator"); break;
    case WXK_NUMPAD_SUBTRACT:
        res << wxT("-"); break;
    case WXK_NUMPAD_DECIMAL:
        res << wxT("."); break;
    case WXK_NUMPAD_DIVIDE:
        res << wxT("/"); break;
    default:

        // ASCII chars...
        if (wxIsalnum(key.code))
        {
            res << (wxChar)key.code;
            break;
        } else {
            // we couldn't create a description for the given keycode...
            return wxEmptyString;
        }
    }

    return res;
}

wxXmlNode* KeyBinder::Save(const wxString& tag) const
{

    wxXmlNode* root = new wxXmlNode(wxXML_ELEMENT_NODE, tag);    

    for (unsigned int i = 0; i < m_commands.size(); ++i)
    {
        if (!m_commands[i]->keys.empty())
        {

            wxXmlNode* node = new wxXmlNode(wxXML_ELEMENT_NODE, "command");    
            node->AddChild( WriteXmlNode("id", m_commands[i]->id) );

            for (unsigned int j = 0; j < m_commands[i]->keys.size(); ++j)
            {
                wxString key = GetKeyBindingAsText(m_commands[i]->keys[j]);
                node->AddChild( WriteXmlNode("key", key) );
            }

            root->AddChild(node);

        }
    }

    return root;

}

void KeyBinder::Load(wxXmlNode* root)
{

    // Erase all of the existing key bindings.

    for (unsigned int i = 0; i < m_commands.size(); ++i)
    {
        m_commands[i]->keys.clear();
    }

    wxXmlNode* node = root->GetChildren();
    while (node != NULL)
    {

        if (node->GetName() == "command")
        {
            LoadCommand(node);
        }

        node = node->GetNext();

    }

}

void KeyBinder::LoadCommand(wxXmlNode* root)
{

    unsigned int        id = 0;
    std::vector<Key>    keys;

    wxXmlNode* node = root->GetChildren();

    while (node != NULL)
    {

        wxString key;

        if (ReadXmlNode(node, "key", key))
        {
            keys.push_back(GetTextAsKeyBinding(key));
        }
        else
        {
            ReadXmlNode(node, "id", id);
        }

        node = node->GetNext();

    }

    Command* command = GetCommandForId(id);

    if (command != NULL)
    {
        command->keys = keys;
    }

}

unsigned int KeyBinder::GetNumCommands() const
{
    return m_commands.size();
}

const KeyBinder::Command& KeyBinder::GetCommand(unsigned int i) const
{
    return *m_commands[i];
}

