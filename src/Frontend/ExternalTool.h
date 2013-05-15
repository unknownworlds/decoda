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

#ifndef EXTERNAL_TOOL_H
#define EXTERNAL_TOOL_H

#include "wx/wx.h"
#include "KeyBinder.h"

//
// Forward declarations.
//

class MainFrame;
class wxCmd;

/**
 * Stores information about a tool the user has setup that runs outsite of the
 * debugger.
 */
class ExternalTool
{

public:

    /**
     * Constructor.
     */
    ExternalTool();

    /**
     * Sets the title for the tool. This is a human readable name that will be
     * displayed in the UI.
     */
    void SetTitle(const wxString& title);

    /**
     * Returns the title for the tool. This is a human readable name that will be
     * displayed in the UI.
     */
    const wxString& GetTitle() const;

    /**
     * Sets the command that will be executed for the tool. This is the application
     * name and path.
     */
    void SetCommand(const wxString& command);

    /**
     * Returns the command for the tool.
     */
    const wxString& GetCommand() const;

    /**
     * Sets the initial working directory for the tool.
     */
    void SetInitialDirectory(const wxString& initialDirectory);

    /**
     * Returns the initial working directory for the tool.
     */
    const wxString& GetInitialDirectory() const;

    /**
     * Sets the arguments that will be supplied to the application when it's
     * executed. This can contain embedded environment variables (i.e $(...)).
     */
    void SetArguments(const wxString& arguments);

    /**
     * Returns the arguments for the tool.
     */
    const wxString& GetArguments() const;

    /**
     * Sets the event name to auto execute this tool on.
     */
    void SetEvent(const wxString& setEvent);

    /**
     * Returns the event this tool gets executed on.
     */
    const wxString& GetEvent() const;

    /**
     * Sets the hot keys the tool.
     */
    void SetKeys(const std::vector<KeyBinder::Key>& keys);

    /**
     * Returns the hot keys for the tool.
     */
    const std::vector<KeyBinder::Key>& GetKeys() const;

    /**
     * Runs the tool. The new process that the tool is running is is returned. If
     * there was an error the method returns NULL.
     */
    wxProcess* Run(const MainFrame* mainFrame) const;

private:

    wxString                        m_title;
    wxString                        m_command;
    wxString                        m_arguments;
    wxString                        m_event;
    wxString                        m_initialDirectory;

    std::vector<KeyBinder::Key>     m_keys;

};

#endif