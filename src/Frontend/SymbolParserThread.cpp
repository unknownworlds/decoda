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

#include "SymbolParserThread.h"
#include "SymbolParserEvent.h"
#include "Symbol.h"
#include "StlUtility.h"
#include "Tokenizer.h"

#include <wx/wfstream.h>
#include <wx/sstream.h>

SymbolParserThread::SymbolParserThread() : wxThread(wxTHREAD_JOINABLE), m_itemsAvailable(0, 1)
{
    m_exit          = false;
    m_headItem      = NULL;
    m_eventHandler  = NULL;
}

void SymbolParserThread::SetEventHandler(wxEvtHandler* eventHandler)
{
    m_eventHandler = eventHandler;
}

void SymbolParserThread::Stop()
{

    // Clear the event handler so that we don't post a new message to it. If we did,
    // that message could be processed in the next event loop after the stop.
    m_headLock.Enter();
    m_eventHandler = NULL;
    m_headLock.Leave();

    // Tell the thread to exit.
    m_exit = true;
    m_itemsAvailable.Post();

    Wait();

}

wxThread::ExitCode SymbolParserThread::Entry()
{
    
    while (!TestDestroy() && !m_exit)
    {

        // Wait for something to show up in the queue.
        m_itemsAvailable.Wait();

        while (!TestDestroy())
        {

            m_itemsLock.Enter();

            if (m_items.empty())
            {
                m_itemsLock.Leave();
                break;
            }

            wxCriticalSectionLocker locker(m_headLock);

            m_headItem = m_items.back();
            m_items.pop_back();

            m_itemsLock.Leave();

            if (m_eventHandler != NULL)
            {

                std::vector<Symbol*> symbols;
                wxStringInputStream input(m_headItem->code);

                ParseFileSymbols(input, symbols);

                m_itemsLock.Enter();
                bool isLastItem=m_items.empty();
                m_itemsLock.Leave();

                // Dispatch the message to event handler.
                SymbolParserEvent event(m_headItem->fileId, symbols, isLastItem);
                m_eventHandler->AddPendingEvent(event);

            }

            delete m_headItem;
            m_headItem = NULL;

        }

    }

    wxCriticalSectionLocker locker1(m_itemsLock);
    ClearVector(m_items);

    wxCriticalSectionLocker locker2(m_headLock);
    delete m_headItem;
    m_headItem = NULL;
    
    return 0;

}

void SymbolParserThread::QueueForParsing(const wxString& code, unsigned int fileId)
{

    QueueItem* item = new QueueItem;
    
    item->code = code;
    item->fileId = fileId;

    {
        wxCriticalSectionLocker locker(m_itemsLock);
        m_items.push_back(item);
    }
    
    // Signal that we have data available.
    m_itemsAvailable.Post();

}

void SymbolParserThread::ParseFileSymbols(wxInputStream& input, std::vector<Symbol*>& symbols)
{

    if (!input.IsOk())
    {
        return;
    }

    wxString token;

    unsigned int lineNumber = 1;

    while (GetToken(input, token, lineNumber))
    {
        if (token == "function")
        {

            unsigned int defLineNumber = lineNumber;

            // Lua functions can have these forms:
            //    function (...)
            //    function Name (...)
            //    function Module.Function (...)
            //    function Class:Method (...)

            wxString t1;
            if (!GetToken(input, t1, lineNumber)) break;

            if (t1 == "(")
            {
                // The form function (...) which doesn't have a name. If we
                // were being really clever we could check to see what is being
                // done with this value, but we're not.
                continue;
            }

            wxString t2;
            
            if (!GetToken(input, t2, lineNumber)) break;

            if (t2 == "(")
            {
                // The form function Name (...).
                symbols.push_back(new Symbol("", t1, defLineNumber));
            }
            else
            {
                
                wxString t3;
                if (!GetToken(input, t3, lineNumber)) break;

                if (t2 == "." || t2 == ":")
                {
                    symbols.push_back(new Symbol(t1, t3, defLineNumber));
                }

            }


        }
    }

}