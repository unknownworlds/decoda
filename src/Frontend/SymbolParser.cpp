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

#include "SymbolParser.h"
#include "SymbolParserEvent.h"
#include "DebugFrontend.h"
#include "Symbol.h"
#include "StlUtility.h"

#include <wx/file.h>

BEGIN_EVENT_TABLE( SymbolParser, wxEvtHandler )

    EVT_SYMBOL_PARSER( OnSymbolsParsed )
    
END_EVENT_TABLE()

SymbolParser::SymbolParser()
{

    m_symbolParserThread.Create();
    m_symbolParserThread.SetPriority(WXTHREAD_MIN_PRIORITY);
    m_symbolParserThread.Run();
    m_symbolParserThread.SetEventHandler(this);
    
    m_project       = NULL;
    m_eventHandler  = NULL;

}

SymbolParser::~SymbolParser()
{
    m_symbolParserThread.Stop();
}

void SymbolParser::SetProject(Project* project)
{
    
    m_project = project;

    // Queue all of the files in the project.

    if (m_project != NULL)
    {
        for (unsigned int fileIndex = 0; fileIndex < m_project->GetNumFiles(); ++fileIndex)
        {
            QueueForParsing( m_project->GetFile(fileIndex) );
        }
    }

}

void SymbolParser::SetEventHandler(wxEvtHandler* eventHandler)
{
    m_eventHandler = eventHandler;
}

void SymbolParser::QueueForParsing(Project::File* file)
{

    wxASSERT(m_project != NULL);
    
    // Our parser is only handling Lua, so if the file isn't a Lua file don't
    // queue it up for parsing.

    if (file->GetFileType().CmpNoCase("lua") == 0)
    {

        wxString code;

        if (file->scriptIndex != -1)
        {
            const DebugFrontend::Script* script = DebugFrontend::Get().GetScript(file->scriptIndex);
            code = script->source.c_str();
        }
        else if (wxFileExists(file->fileName.GetFullPath()))
        {
            ReadFile(file->fileName.GetFullPath(), code);
        }
        else
        {
            return;
        }

        m_symbolParserThread.QueueForParsing(code, file->fileId);
    
    }

}

bool SymbolParser::ReadFile(const wxString& fileName, wxString& contents)
{

    wxFile file;

    if (!file.Open(fileName))
    {
        return false;
    }

    size_t length = file.Length();
    
    if (length > 0)
    {
        file.Read(wxStringBuffer(contents, length), length);
    }
    else
    {
        contents.Clear();
    }
    
    return true;

}

void SymbolParser::OnSymbolsParsed(SymbolParserEvent& event)
{

    if (m_project != NULL)
    {

        unsigned int fileId = event.GetFileId();

        // Find the file that this corresponds to. It's possible that the file no longer
        // exists if some the project changed after the file was queued for parsing.

        Project::File* file = m_project->GetFileById(fileId);

        if (file != NULL)
        {

            ClearVector(file->symbols);
            file->symbols = event.GetSymbols();

            // Pass along the event to the specified event handler.
            if (m_eventHandler != NULL)
            {
                m_eventHandler->AddPendingEvent(event);
            }

            return;

        }

    }

    // Need to delete the symbols or else we'll leak.

    const std::vector<Symbol*>& symbols = event.GetSymbols();

    for (unsigned int i = 0; i < symbols.size(); ++i)
    {
        delete symbols[i];
    }

}
