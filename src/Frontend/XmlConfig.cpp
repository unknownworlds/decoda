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

#include "XmlConfig.h"
#include "XmlUtility.h"

#include "wx/xml/xml.h"

XmlConfig::XmlConfig(wxXmlNode* root)
{
    m_root      = root;
    m_pathNode  = root;
}

void XmlConfig::SetPath(const wxString& path)
{

    wxArrayString aParts;

    if (!path.empty() )
    {
        if ( path[0] == wxCONFIG_PATH_SEPARATOR )
        {
            // Absolute path
            wxSplitPath(aParts, path);
        }
        else
        {
            // Relative path, combine with current one
            wxString strFullPath = m_path;
            strFullPath << wxCONFIG_PATH_SEPARATOR << path;
            wxSplitPath(aParts, strFullPath);
        }
    }

    // recombine path parts in one variable
    m_path.Empty();
    for (unsigned int n = 0; n < aParts.Count(); n++ ) {
        m_path << wxCONFIG_PATH_SEPARATOR << aParts[n];
    }

    m_pathNode = GetGroupNode(aParts);

}

const wxString& XmlConfig::GetPath() const
{
    return m_path;
}

bool XmlConfig::GetFirstGroup(wxString& str, long& lIndex) const
{

    wxXmlNode* node = m_pathNode->GetChildren();

    lIndex = reinterpret_cast<long>(node);

    if (node != NULL)
    {
        str = node->GetName();
        return true;
    }
    else
    {
        return false;
    }

}

bool XmlConfig::GetNextGroup (wxString& str, long& lIndex) const
{

    wxXmlNode* node = reinterpret_cast<wxXmlNode*>(lIndex);
    node = node->GetNext();

    lIndex = reinterpret_cast<long>(node);

    if (node != NULL)
    {
        str = node->GetName();
        return true;
    }
    else
    {
        return false;
    }

}

bool XmlConfig::GetFirstEntry(wxString& str, long& lIndex) const
{

    wxXmlNode* child = m_pathNode->GetChildren();
    lIndex = reinterpret_cast<long>(child);

    if (child != NULL)
    {
        str = child->GetName();
    }

    return child != NULL;

}

bool XmlConfig::GetNextEntry(wxString& str, long& lIndex) const
{

    wxXmlNode* child = reinterpret_cast<wxXmlNode*>(lIndex);
    child = child->GetNext();
    
    lIndex = reinterpret_cast<long>(child);

    if (child != NULL)
    {
        str = child->GetName();
    }

    return child != NULL;

}

size_t XmlConfig::GetNumberOfEntries(bool bRecursive) const
{
    return 0;
}

size_t XmlConfig::GetNumberOfGroups(bool bRecursive) const
{
    return 0;
}

bool XmlConfig::HasGroup(const wxString& strName) const
{
    return false;
}

bool XmlConfig::HasEntry(const wxString& strName) const
{
    return FindChildNode(m_pathNode, strName) != NULL;
}

bool XmlConfig::Flush(bool bCurrentOnly)
{
    return false;
}

bool XmlConfig::RenameEntry(const wxString& oldName, const wxString& newName)
{
    return false;
}

bool XmlConfig::RenameGroup(const wxString& oldName, const wxString& newName)
{
    return false;
}

bool XmlConfig::DeleteEntry(const wxString& key, bool bDeleteGroupIfEmpty)
{
    return false;
}

bool XmlConfig::DeleteGroup(const wxString& key)
{
    return false;
}

bool XmlConfig::DeleteAll()
{
    return false;
}

bool XmlConfig::DoReadString(const wxString& key, wxString *pStr) const
{

    wxXmlNode* node = FindChildNode(m_pathNode, key);

    if (node != NULL && ReadXmlNode(node, key, *pStr))
    {
        return true;
    }
    else
    {
        return false;
    }

}

bool XmlConfig::DoReadLong(const wxString& key, long *pl) const
{

    wxXmlNode* node = FindChildNode(m_pathNode, key);

    wxString data;

    if (node != NULL && ReadXmlNode(node, key, data))
    {
        return data.ToLong(pl);
    }
    else
    {
        return false;
    }

}

bool XmlConfig::DoWriteString(const wxString& key, const wxString& value)
{
    wxString name = key;
    wxXmlNode* node = CreateSubNode(name);
    node->AddChild(WriteXmlNode(name, value));
    return true;
}

bool XmlConfig::DoWriteLong(const wxString& key, long value)
{
    wxString name = key;
    wxXmlNode* node = CreateSubNode(name);
    node->AddChild(WriteXmlNode(name, value));
    return true;
}

wxXmlNode* XmlConfig::CreateSubNode(wxString& key)
{

    wxXmlNode* node = m_root;

    while (1)
    {
    
        int sep = key.Find("//");

        if (sep == -1)
        {
            break;
        }
        else
        {
            wxXmlNode* child = node->GetChildren();
            wxString sub = key.Left(sep);

            while (child != NULL)
            {
                if (child->GetName() == sub)
                {
                    break;
                }
                child = child->GetNext();
            }

            if (child == NULL)
            {
                child = new wxXmlNode(wxXML_ELEMENT_NODE, sub);
                node->AddChild(child);
            }

            node = child;
            key.erase(0, sep + 2);

        }

    }

    return node;

}

wxXmlNode* XmlConfig::GetGroupNode(const wxArrayString& parts) const
{

    wxXmlNode* node = m_root;

    for (unsigned int i = 0; i < parts.Count(); ++i)
    {
    
        wxXmlNode* child = FindChildNode(node, parts[i]);

        if (child == NULL)
        {
            return NULL;
        }

        node = child;

    }

    return node;

}