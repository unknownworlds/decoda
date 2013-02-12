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

#ifndef EDITOR_SETTINGS_H
#define EDITOR_SETTINGS_H

//
// Forward declarations.
//

class wxXmlNode;
class wxString;

/**
 * This class stores settings that apply to the code editor.
 */
class EditorSettings
{

public:

    /**
     * Constructor. Initializes the settings to the default values.
     */
    EditorSettings();

    /**
     * Sets the number of characters per indent.
     */ 
    void SetIndentSize(unsigned int indentSize);

    /**
     * Sets whether or not indenting inserts spaces or a tab character.
     */
    void SetUseTabs(bool useTabs);

    /**
     * Sets whether or not tabs are stripped from a file when it's loaded.
     */
    void SetRemoveTabsOnLoad(bool removeTabsOnLoad);

    /**
     * Returns the number of characters per indent.
     */ 
    unsigned int GetIndentSize() const;

    /**
     * Returns whether or not indenting inserts spaces or a tab character.
     */
    bool GetUseTabs() const;

    /**
     * Returns whether or not tabs are stripped from a file when it's loaded.
     */
    bool GetRemoveTabsOnLoad() const;

    /**
     * Returns whether or not line numbers should be shown in the margin of the
     * editor.
     */
    bool GetShowLineNumbers() const;

    /**
     * Sets whether or not line numbers should be shown in the margin of the
     * editor.
     */
    void SetShowLineNumbers(bool showLineNumbers);

    /**
     * Returns whether or not the UI automatically loads the last project that was open
     * when the program starts up.
     */
    bool GetLoadLastProjectOnStartup() const;

    /**
     * Sets whether or not the UI automatically loads the last project that was open
     * when the program starts up.
     */
    void SetLoadLastProjectOnStartup(bool loadLastProjectOnStartup);

    /**
     * Returns whether or not the tab switching uses the most recently used tab order.
     */
    bool GetMostRecentlyUsedTabSwitching() const;

    /**
     * Sets whether or not the tab switching uses the most recently used tab order.
     */
    void SetMostRecentlyUsedTabSwitching(bool mostRecentlyUsedTabSwitching);

    /**
     * Returns whether or not the editor makes suggestions on how to complete the typed
     * text.
     */
    bool GetEnableAutoComplete() const;

    /**
     * Sets whether or not the editor makes suggestions on how to complete the typed
     * text.
     */
    void SetEnableAutoComplete(bool enableAutoComplete);

    /**
     * Returns whether or not the editor will show the white spaces
     */
    bool GetShowWhiteSpace() const;

    /**
     * Sets whether or not the editor will show white spaces
     */
    void SetShowWhiteSpace(bool showWhiteSpace);

    /**
     * Saves the editor settings in XML format. The tag is the name that is given
     * to the root node.
     */
    wxXmlNode* Save(const wxString& tag) const;

    /**
     * Loads the editor settings from XML format.
     */
    void Load(wxXmlNode* root);

private:

    unsigned int    m_indentSize;
    bool            m_useTabs;
    bool            m_removeTabsOnLoad;
    bool            m_showLineNumbers;
    bool            m_loadLastProjectOnStartup;
    bool            m_mostRecentlyUsedTabSwitching;
    bool            m_enableAutoComplete;
    bool            m_showWhiteSpaces;

};

#endif