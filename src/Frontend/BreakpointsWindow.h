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

#ifndef BREAKPOINTS_WINDOW_H
#define BREAKPOINTS_WINDOW_H

#include "Project.h"
#include "FontColorSettings.h"

#include <wx/wx.h>
#include <wx/listctrl.h>

//
// Forward declarations.
//

class MainFrame;
class ButtonBar;
class ListView;

/**
 * Breakpoints window.
 */
class BreakpointsWindow : public wxPanel
{

    DECLARE_EVENT_TABLE()

public:

    /**
     * Constructor.
     */
    BreakpointsWindow(MainFrame* mainFrame, wxWindowID winid);

    /**
    * Updates the colors of the panel to match the settings
    */
    void SetFontColorSettings(const FontColorSettings& settings);

    /**
     * Sets the project we're displaying the breakpoints for.
     */
    void SetProject(Project* project);
    
    /**
     * Updates the breakpoint in the window for the project.
     */
    void UpdateBreakpoints();

    /**
     * Removes the breakpoints for the specified file. This should be called
     * when the file is removed.
     */
    void RemoveFile(Project::File* file);

    /**
     * Updates breakpoints for the specified file in the project.
     */
    void UpdateBreakpoints(Project::File* file);

    /**
     * Called when an item is deleted from the list.
     */
    void OnDeleteItem(wxListEvent& event);

    /**
     * Called when all items are deleted from the list.
     */
    void OnDeleteAllItems(wxListEvent& event);

    /**
     * Called when the user double clicks or presses enter on an item in the
     * list.
     */
    void OnItemActivated(wxListEvent& event);
    
    /**
     * Called when a new item is selected in the breakpoints list.
     */
    void OnItemSelected(wxListEvent& event);
    
    /**
     * Called when a new item is selected in the breakpoints list.
     */
    void OnItemDeselected(wxListEvent& event);

    /**
     * Called when the delete breakpoint button is pressed.
     */
    void OnDeleteBreakpoint(wxCommandEvent& event);

    /**
     * Called when the delete all breakpoints button is pressed.
     */
    void OnDeleteAllBreakpoints(wxCommandEvent& event);

    /**
     * Called when the go to source code button is pressed.
     */
    void OnGoToSourceCode(wxCommandEvent& event);

private:

    enum
    {
        ID_DeleteBreakpoint,
        ID_DeleteAllBreakpoints,
        ID_GoToSourceCode,
    };

    /**
     * Structure to store information about a break point. One of these is associated
     * with each entry in the list control.
     */
    struct Breakpoint
    {
        Project::File*  file;
        unsigned int    line;
    };

    /**
     * Adds the breakpoints to the window for the specified project file.
     */
    void AddBreakpointsForFile(Project::File* file);

    /**
     * Updates the status of the buttons on the tool bar based on the current
     * list of breakpoints.
     */
    void UpdateToolBarStatus();

private:

    ButtonBar*      m_buttonBar;
    ListView*       m_breakpointList;
    MainFrame*      m_mainFrame;
    Project*        m_project;

};

#endif