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

#ifndef SYMBOL_PARSER_H
#define SYMBOL_PARSER_H

#include "Project.h"
#include "SymbolParserThread.h"

#include <wx/wx.h>

//
// Forward declarations.
//

class SymbolParserEvent;

/**
 * This class is used to parse the symbols. The symbols are parsed by a background
 * thread and events are sent when they are ready.
 */
class SymbolParser : public wxEvtHandler
{

    DECLARE_EVENT_TABLE()

public:

    /**
     * Constructor.
     */
    SymbolParser();

    /**
     * Destructor.
     */
    virtual ~SymbolParser();

    /**
     * Sets the project for which files will be parsed. This must be called before calling
     * QueueForParsing. It's safe to change the project while the files are still queued;
     * The symbols for unparsed files that belong to another project will be discarded. All
     * of the files in the project are automatically queued for parsing.
     */
    void SetProject(Project* project);

    /**
     * Sets the event handler where events are dispatched when symbols are done parsing.
     */
    void SetEventHandler(wxEvtHandler* eventHandler);

    /**
     * Queues a file to have its symbols parsed. The symbols will be parsed in the
     * background and an event will be sent when they are done. The parser makes copies
     * of the necessary data and doesn't require that the file pointer remain valid after
     * the function is called.
     */
    void QueueForParsing(Project::File* file);

    /**
     * Called when symbols for a file are done parsing.
     */
    void OnSymbolsParsed(SymbolParserEvent& event);

private:

    /**
     * Reads the contents of the file into the buffer.
     */
    bool ReadFile(const wxString& fileName, wxString& contents);

private:

    SymbolParserThread          m_symbolParserThread;
    Project*                    m_project;
    wxEvtHandler*               m_eventHandler;

};

#endif