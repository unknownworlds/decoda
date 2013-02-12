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

#ifndef SYSTEM_SETTINGS_H
#define SYSTEM_SETTINGS_H

//
// Forward declarations
//

class wxXmlNode;
class wxString;

/**
 * Stores the settings pertaining to the system.
 */
class SystemSettings
{

public:

    /**
     * Constructor.
     */
    SystemSettings();

    /**
     * Returns whether or not the program should automatically check for updates.
     */
    bool GetCheckForUpdates() const;

    /**
     * Sets whether or not the program should automatically check for updates.
     */
    void SetCheckForUpdates(bool checkForUpdates);

    /**
     * Saves the font and color settings in XML format. The tag is the name that is given
     * to the root node.
     */
    wxXmlNode* Save(const wxString& tag) const;

    /**
     * Loads the font and color settings from XML format.
     */
    void Load(wxXmlNode* root);

private:

    bool    m_checkForUpdates;

};

#endif