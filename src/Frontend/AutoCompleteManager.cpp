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

#include "AutoCompleteManager.h"
#include "Symbol.h"

#include <algorithm>

AutoCompleteManager::Entry::Entry()
{
}

AutoCompleteManager::Entry::Entry(const wxString& _name, Type _type, const wxString& _scope)
    : name(_name), type(_type), scope(_scope)
{
    lowerCaseName = name.Lower();
}

bool AutoCompleteManager::Entry::operator<(const Entry& entry) const
{
    return name.Cmp(entry.name) < 0;
}

void AutoCompleteManager::BuildFromProject(const Project* project)
{
    
    m_entries.clear();

    for (unsigned int fileIndex = 0; fileIndex < project->GetNumFiles(); ++fileIndex)
    {
        BuildFromFile(project->GetFile(fileIndex));
    }
    for (unsigned int directoryIndex = 0; directoryIndex < project->GetNumDirectories(); ++directoryIndex)
    {
      Project::Directory const *directory = project->GetDirectory(directoryIndex);

      for (unsigned int fileIndex = 0; fileIndex < directory->files.size(); ++fileIndex)
      {
        BuildFromFile(directory->files[fileIndex]);
      }
    }

    // Sort the autocompletions (necessary for binary search).
    std::sort(m_entries.begin(), m_entries.end());

}

void AutoCompleteManager::BuildFromFile(const Project::File* file)
{

    for (unsigned int symbolIndex = 0; symbolIndex < file->symbols.size(); ++symbolIndex)
    {
        const Symbol* symbol = file->symbols[symbolIndex];
        m_entries.push_back( Entry(symbol->name, Type_Function, symbol->module) );
    }

}

void AutoCompleteManager::GetMatchingItems(const wxString& token, const wxString& prefix, bool member, wxString& items) const
{
    
    // Autocompletion selection is case insensitive so transform everything
    // to lowercase.
    wxString test = token.Lower();

    // Add the items to the list that begin with the specified prefix. This
    // could be done much fater with a binary search since our items are in
    // alphabetical order.

    for (unsigned int i = 0; i < m_entries.size(); ++i)
    {
        // Check that the scope is correct.
        
        bool inScope = false;

        if (/**/true)
        {
            // We've got no way of knowing the type of the variable in Lua (since
            // variables don't have types, only values have types), so we display
            // all members if the prefix contains a member selection operator (. or :)
            if (!m_entries[i].scope.IsEmpty() && member)
            {
              inScope = m_entries[i].scope == prefix;
            }
            else
            {
              inScope = m_entries[i].scope.IsEmpty() != member;
            }
        }
        
        if (inScope && m_entries[i].lowerCaseName.StartsWith(test))
        {
            
            items += m_entries[i].name;

            // Add the appropriate icon for the type of the identifier.
            if (m_entries[i].type != Type_Unknown)
            {
                items += "?";
                items += '0' + m_entries[i].type;
            }

            items += ' ';

        }
    }

}