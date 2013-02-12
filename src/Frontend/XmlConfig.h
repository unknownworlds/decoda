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

#ifndef XML_CONFIG_H
#define XML_CONFIG_H

#include "wx/wx.h"
#include "wx/confbase.h"

//
// Forward declarations.
//

class wxXmlNode;

/**
 * Configuration implement for loading/saving in an XML format.
 */
class XmlConfig : public wxConfigBase
{

public:

    explicit XmlConfig(wxXmlNode* root);

    virtual void SetPath(const wxString& strPath);
    // retrieve the current path (always as absolute path)
    virtual const wxString& GetPath() const;

    // enumeration: all functions here return false when there are no more items.
    // you must pass the same lIndex to GetNext and GetFirst (don't modify it)
    // enumerate subgroups
    virtual bool GetFirstGroup(wxString& str, long& lIndex) const;
    virtual bool GetNextGroup (wxString& str, long& lIndex) const;
    // enumerate entries
    virtual bool GetFirstEntry(wxString& str, long& lIndex) const;
    virtual bool GetNextEntry (wxString& str, long& lIndex) const;
    // get number of entries/subgroups in the current group, with or without
    // it's subgroups
    virtual size_t GetNumberOfEntries(bool bRecursive = false) const;
    virtual size_t GetNumberOfGroups(bool bRecursive = false) const;

    // tests of existence
    // returns true if the group by this name exists
    virtual bool HasGroup(const wxString& strName) const;
    // same as above, but for an entry
    virtual bool HasEntry(const wxString& strName) const;

    // permanently writes all changes
    virtual bool Flush(bool bCurrentOnly = false);

    // renaming, all functions return false on failure (probably because the new
    // name is already taken by an existing entry)
    // rename an entry
    virtual bool RenameEntry(const wxString& oldName,
                            const wxString& newName);
    // rename a group
    virtual bool RenameGroup(const wxString& oldName,
                            const wxString& newName);

    // delete entries/groups
    // deletes the specified entry and the group it belongs to if
    // it was the last key in it and the second parameter is true
    virtual bool DeleteEntry(const wxString& key,
                            bool bDeleteGroupIfEmpty = true);
    // delete the group (with all subgroups)
    virtual bool DeleteGroup(const wxString& key);
    // delete the whole underlying object (disk file, registry key, ...)
    // primarly for use by desinstallation routine.
    virtual bool DeleteAll();

    virtual bool DoReadString(const wxString& key, wxString *pStr) const;
    virtual bool DoReadLong(const wxString& key, long *pl) const;

    virtual bool DoWriteString(const wxString& key, const wxString& value);
    virtual bool DoWriteLong(const wxString& key, long value);

private:

    wxXmlNode* CreateSubNode(wxString& key);

    wxXmlNode* GetGroupNode(const wxArrayString& path) const;

private:

    wxXmlNode*  m_root;
    wxString    m_path;
    wxXmlNode*  m_pathNode;

};

#endif