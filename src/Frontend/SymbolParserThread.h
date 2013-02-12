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

#ifndef SYMBOL_PARSER_THREAD_H
#define SYMBOL_PARSER_THREAD_H

#include <wx/wx.h>
#include <wx/thread.h>
#include <vector>

//
// Forward declarations.
//

class Symbol;

/**
 * This thread class is reponsible for loading files and parsing them to determine
 * the symbols for display in the Project Explorer window.
 */
class SymbolParserThread : public wxThread
{

public:

    /**
     * Constructor.
     */
    SymbolParserThread();

    /**
     * Sets the event handler that receives notification when a file is done parsing.
     * Note this function isn't thread safe and should be called before the thread is
     * started.
     */
    void SetEventHandler(wxEvtHandler* eventHandler);

    /**
     * Ends the thread.
     */
    void Stop();

    /**
     * Entry point for the symbol parser thread.
     */
    virtual ExitCode Entry();

    /**
     * Queues a file to have its symbols parsed.
     */
    void QueueForParsing(const wxString& code, unsigned int fileId);

private:

    /**
     * Parses the symbols for the file.
     */
    void ParseFileSymbols(wxInputStream& input, std::vector<Symbol*>& symbols);

private:

    /**
     * Item enqueued for parsing.
     */
    struct QueueItem
    {
        unsigned int    fileId;
        wxString        code;
    };

    wxEvtHandler*               m_eventHandler;

    wxSemaphore                 m_itemsAvailable;
    
    wxCriticalSection           m_itemsLock;
    std::vector<QueueItem*>     m_items;

    QueueItem*                  m_headItem;
    wxCriticalSection           m_headLock;

    bool                        m_exit;

};

#endif