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

#ifndef KEY_BINDER_H
#define KEY_BINDER_H

#include <wx/accel.h>
#include <vector>

//
// Forward declarations.
//

class wxWindow;
class wxMenuBar;
class wxXmlNode;

/**
 * Binds keys to commands.
 */
class KeyBinder
{

public:

    struct Key
    {
        int                 flags;
        int                 code;
    };

    struct Command
    {
        int                 id;
        wxString            name;
        wxString            group;
        wxString            help;
        std::vector<Key>    keys;
    };

    /**
     * Constructor.
     */
    KeyBinder();

    /**
     * Destructor.
     */
    virtual ~KeyBinder();

    /**
     * Removes all of the commands from the key binder.
     */
    void ClearCommands();

    /**
     * Finds the command that is bound to the specified id. If there is no such
     * command the method returns NULL.
     */
    Command* GetCommandForId(int id) const;

    /**
     * Adds a command that can be hot keyed.
     */
    void AddCommand(const Command& command);

    /**
     * Adds a command that can be hot keyed.
     */
    void AddCommand(int id, const wxString& group, const wxString& name, const wxString& help);

    /**
     * Adds a command for the specified menu item.
     */
    void AddCommand(const wxString& group, wxMenuItem* menuItem);

    /**
     * Adds all of the commands on a menu bar to the key binder.
     */
    void AddCommandsFromMenuBar(wxMenuBar* menu);

    /**
     * Adds all of the commands on a menu to the key binder.
     */
    void AddCommandsFromMenu(const wxString& group, wxMenu* menu);

    /**
     * Removes the command with the specified id.
     */
    void RemoveCommand(int id);

    /**
     * Updates the accelerator for a window with the key bindings.
     */
    void UpdateWindow(wxWindow* window);

    /**
     * Updates the text on all of the menus in the menu bar to reflect the
     * current key bindings.
     */
    void UpdateMenuBar(wxMenuBar* menuBar);

    /**
     * Updates the text on the menu item to reflect the current key binding.
     * If a name is supplied it's used as the label on the menu, otherwise
     * the corresponding command name is used.
     */
    bool GetMenuItemText(wxMenuItem* item, wxString& label) const;

    /**
     * Sets the shortcut for the specified id.
     */
    void SetShortcut(int id, int flags, int key);

    /**
     * Sets the shortcut for the specified id. The shortcut should have a form
     * like "Ctrl+F3".
     */
    void SetShortcut(int id, const wxString& shortcut);

    /**
     * Saves the key bindings in XML format. The tag is the name that is given
     * to the root node.
     */
    wxXmlNode* Save(const wxString& tag) const;

    /**
     * Loads the key bindings from XML format.
     */
    void Load(wxXmlNode* root);

    /**
     * Returns the number of commands managed by the key binder.
     */
    unsigned int GetNumCommands() const;

    /**
     * Returns the ith command.
     */
    const Command& GetCommand(unsigned int i) const;

    /**
     * Returns the human readable string representing the key binding.
     */
    static wxString GetKeyBindingAsText(const Key& key);

    /**
     * Parses a human readable string into a key binding.
     */
    static Key GetTextAsKeyBinding(const wxString& text);

private:

    /**
     * Updates the text on the menu to reflect the current key bindings.
     */
    void UpdateMenu(wxMenu* menu);

    /**
     */
    static int StringToKeyCode(const wxString &keyName);

    /**
     * Loads a command key binding in XML format. The root node passed in
     * should be a"command" node.
     */
    void LoadCommand(wxXmlNode* root);

private:

    std::vector<Command*>   m_commands;
    wxAcceleratorTable      m_accel;

};

#endif