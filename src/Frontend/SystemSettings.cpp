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

#include "SystemSettings.h"
#include "XmlUtility.h"

#include <wx/wx.h>
#include <wx/xml/xml.h>

SystemSettings::SystemSettings()
{
    m_checkForUpdates = true;
}

bool SystemSettings::GetCheckForUpdates() const
{
    return m_checkForUpdates;
}

void SystemSettings::SetCheckForUpdates(bool checkForUpdates)
{
    m_checkForUpdates = checkForUpdates;
}

wxXmlNode* SystemSettings::Save(const wxString& tag) const
{

    wxXmlNode* root = new wxXmlNode(wxXML_ELEMENT_NODE, tag);    
    root->AddChild( WriteXmlNodeBool("check_for_updates", m_checkForUpdates) );
    return root;

}

void SystemSettings::Load(wxXmlNode* root)
{

    wxXmlNode* node = root->GetChildren();

    while (node != NULL)
    {
        ReadXmlNode(node, "check_for_updates", m_checkForUpdates);
        node = node->GetNext();
    }

}