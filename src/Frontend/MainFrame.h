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

#ifndef MAIN_FRAME_H
#define MAIN_FRAME_H

#include <wx/wx.h>
#include <wx/filename.h>
#include <wx/listctrl.h>
#include <wx/aui/aui.h>
#include <wx/process.h>
#include <wx/fdrepdlg.h>
#include <wx/treectrl.h>
#include <wx/docview.h>
#include <wx/help.h>
#include <wx/cshelp.h>

#include "CodeEdit.h"
#include "Project.h"
#include "KeyBinder.h"
#include "FontColorSettings.h"
#include "EditorSettings.h"
#include "SystemSettings.h"
#include "SourceControl.h"
#include "FileChangeWatcher.h"
#include "Updater.h"
#include "StringHistory.h"
#include "AutoCompleteManager.h"
#include "DebugFrontend.h"
#include "ProcessOutputSink.h"

#include <vector>
#include <string>

//
// Forward declarations.
//

class ExternalTool;
class WatchWindow;
class OutputWindow;
class BreakpointsWindow;
class SearchWindow;
class ProjectExplorerWindow;
class wxXmlNode;
class FileStatusThread;
class ThreadEvent;
class FileEvent;
class wxDebugEvent;
class ListWindow;
class SymbolParser;
class SymbolParserEvent;

/**
 * Main application window.
 */
class MainFrame: public wxFrame
{

public:

    /**
     * File that's open the the notebook. The fileIndex is the index of the
     * file in the project.
     */
    struct OpenFile
    {
        CodeEdit*               edit;
        Project::File*          file;
        time_t                  timeStamp;
    };

    /**
     * Returns the name of the application used for displaying in window title bars, etc.
     */
    static const wxString& GetApplicationName();

    /**
     * Constructor.
     */
    MainFrame(const wxString& title, int openFilesMessage, const wxPoint& pos, const wxSize& size);

    /**
     * Destructor..
     */
    virtual ~MainFrame();

    /**
     * Called when the window is closed.
     */
    void OnClose(wxCloseEvent& event);

    /**
     * Called when the user selects File/New Project from the menu.
     */
    void OnFileNewProject(wxCommandEvent& event);

    /**
     * Called when the user selects Project/Open Project from the menu.
     */
    void OnFileOpenProject(wxCommandEvent& event);

    /**
     * Called when the user selects Project/Save Project from the menu.
     */
    void OnFileSaveProject(wxCommandEvent& event);

    /**
     * Called when the user selects Project/Save Project As from the menu.
     */
    void OnFileSaveProjectAs(wxCommandEvent& event);

    /**
     * Called when the user selects File/Exit from the menu.
     */
    void OnFileExit(wxCommandEvent& event);
    
    /**
     * Called when the user selects File/New from the menu.
     */
    void OnFileNew(wxCommandEvent& event);

    /**
     * Called when the user selects File/Open from the menu.
     */
    void OnFileOpen(wxCommandEvent& event);

    /**
     * Called when the user selects File/Save from the menu.
     */
    void OnFileSave(wxCommandEvent& event);

    /**
     * Called when the user selects File/Save As from the menu.
     */
    void OnFileSaveAs(wxCommandEvent& event);

    /**
     * Called when the user selects Show File from the menu.
     */
    void OnFileShow(wxCommandEvent& event);

    /**
     * Called when the user selects Edit/Undo from the menu.
     */
    void OnEditUndo(wxCommandEvent& event);

    /**
     * Called when the user selects Edit/Redo from the menu.
     */
    void OnEditRedo(wxCommandEvent& event);

    /**
     * Called when the user selects Edit/Cut from the menu.
     */
    void OnEditCut(wxCommandEvent& event);

    /**
     * Called when the user selects Edit/Copy from the menu.
     */
    void OnEditCopy(wxCommandEvent& event);

    /**
     * Called when the user selects Edit/Paste from the menu.
     */
    void OnEditPaste(wxCommandEvent& event);
    
    /**
     * Called when the user selects Edit/Delete from the menu.
     */
    void OnEditDelete(wxCommandEvent& event);

    /**
     * Called when the user selects Edit/Delete from the menu.
     */
    void OnEditSelectAll(wxCommandEvent& event);

    /**
     * Called when the user selects Edit/Find from the menu.
     */
    void OnEditFind(wxCommandEvent& event);

    /**
     * Called when the user selects Edit/Replace from the menu.
     */
    void OnEditReplace(wxCommandEvent& event);

    /**
     * Called when the user selects Edit/Find Next from the menu.
     */
    void OnEditFindNext(wxCommandEvent& event);

    /**
     * Called when the user selects Edit/Find Previous from the menu.
     */
    void OnEditFindPrevious(wxCommandEvent& event);

    /**
     * Called when the user selects Edit/Find in Files from the menu.
     */
    void OnEditFindInFiles(wxCommandEvent& event);

    /**
     * Called when the user selects Edit/Goto Line from the menu.
     */
    void OnEditGotoLine(wxCommandEvent& event);

    /**
     * Called when the user selects Edit/Untabify Selection from the menu.
     */
    void OnEditUntabify(wxCommandEvent& event);

    /**
     * Called when the user selects Edit/Comment Selection from the menu.
     */
    void OnEditComment(wxCommandEvent& event);

    /**
     * Called when the user selects Edit/Uncomment Selection from the menu.
     */
    void OnEditUncomment(wxCommandEvent& event);

    /**
     * Called when the user selects Project/Add Existing File from the menu.
     */
    void OnProjectAddExistingFile(wxCommandEvent& event);

    /**
     * Called when the user selects Project/Add New File from the menu.
     */
    void OnProjectAddNewFile(wxCommandEvent& event);

    /**
     * Called when the user selects Project/Settings from the menu.
     */
    void OnProjectSettings(wxCommandEvent& event);

    /**
     * Called when the user selects Debug/Start from the menu.
     */
    void OnDebugStart(wxCommandEvent& event);

    /**
     * Called when the Debug/Start menu item needs to be updated to reflect
     * the current status.
     */
    void OnUpdateDebugStart(wxUpdateUIEvent& event);

    /**
     * Called when the user selects Debug/Start Without Debugging from the menu.
     */
    void OnDebugStartWithoutDebugging(wxCommandEvent& event);

    /**
     * Called when the user selects Debug/Stop from the menu.
     */
    void OnDebugStop(wxCommandEvent& event);

    /**
     * Called when the Debug/Stop menu item needs to be updated to reflect
     * the current status.
     */
    void OnUpdateDebugStop(wxUpdateUIEvent& event);

    /**
     * Called when the user selects Debug/Attach To Host from the menu.
     */
    void OnDebugAttachToHost(wxCommandEvent& event);

    /**
     * Called when the user selects Debug/Detach from the menu.
     */
    void OnDebugDetach(wxCommandEvent& event);

    /**
     * Called when the user selects Debug/Break from the menu.
     */
    void OnDebugBreak(wxCommandEvent& event);

    /**
     * Called when the Debug/Break menu item needs to be updated to reflect
     * the current status.
     */
    void OnUpdateDebugBreak(wxUpdateUIEvent& event);

    /**
     * Called when the user selects Debug/Step Into from the menu.
     */
    void OnDebugStepInto(wxCommandEvent& event);

    /**
     * Called when the Debug/Step Into menu item needs to be updated to reflect
     * the current status.
     */
    void OnUpdateDebugStepInto(wxUpdateUIEvent& event);

    void OnDebugStepOver(wxCommandEvent& event);

    void OnDebugQuickWatch(wxCommandEvent& event);

    /**
     * Called when the user selects Debug/Processt from the menu.
     */
    void OnDebugProcess(wxCommandEvent& event);

    /**
     * Called when the user selects Debug/Toggle Breakpoint from the menu.
     */
    void OnDebugToggleBreakpoint(wxCommandEvent& event);

    /**
     * Called when the user selects Debug/Delete All Breakpoints from the menu.
     */
    void OnDebugDeleteAllBreakpoints(wxCommandEvent& event);

    /**
     * Called when the user selects Tools/Settings from the menu.
     */
    void OnToolsSettings(wxCommandEvent& event);

    /**
     * Called when the user selects Tools/External Tools from the menu.
     */
    void OnToolsExternalTools(wxCommandEvent& event);

    /**
     * Called when the user selects Window/Project Explorer from the menu.
     */
    void OnWindowProjectExplorer(wxCommandEvent& event);

    /**
     * Called when the user selects Window/Call Stack from the menu.
     */
    void OnWindowCallStack(wxCommandEvent& event);

    /**
     * Called when the user selects Window/Output from the menu.
     */
    void OnWindowOutput(wxCommandEvent& event);

    /**
     * Called when the user selects Window/Search Results from the menu.
     */
    void OnWindowSearch(wxCommandEvent& event);

    /**
     * Called when the user selects Window/Watch from the menu.
     */
    void OnWindowWatch(wxCommandEvent& event);

    /**
     * Called when the user selects Window/Virtual Machines from the menu.
     */
    void OnWindowVirtualMachines(wxCommandEvent& event);

    /**
     * Called when the user selects Window/Breakpoints from the menu.
     */
    void OnWindowBreakpoints(wxCommandEvent& event);

    /**
     * Called when the user selects Window/Next Document from the menu.
     */
    void OnWindowNextDocument(wxCommandEvent& event);

    /**
     * Called when the user selects Window/Previous Document from the menu.
     */
    void OnWindowPreviousDocument(wxCommandEvent& event);

    /**
     * Called when the user selects Window/Close All Documents from the menu.
     */
    void OnWindowCloseAllDocuments(wxCommandEvent& event);

    /**
     * Called when the user selects Window/Close from the menu.
     */
    void OnWindowClose(wxCommandEvent& event);

    /**
     * Called when the user selects Help/About from the menu.
     */
    void OnHelpAbout(wxCommandEvent& event);

    /**
     * Called when the user selects Help/Support from the menu.
     */
    void OnHelpSupport(wxCommandEvent& event);

    /**
     * Called when the user selects Help/Contents from the menu.
     */
    void OnHelpContents(wxCommandEvent& event);

    /**
     * Called when the user selects Help/Check For Update from the menu.
     */
    void OnHelpCheckForUpdate(wxCommandEvent& event);

    /**
     * Called when the user selects a menu item that isn't handled elswewhere. This is used
     * to catch the items that don't have fixed ids like the external tools.
     */
    void OnMenu(wxCommandEvent& event);

    /**
     * Called when the a debug event comes from the debugger frontend.
     */
    void OnDebugEvent(wxDebugEvent& event);

    /**
     * Called when a file event happens, like when the read-only status of a file
     * that's being tracked changes.
     */
    void OnFileEvent(FileEvent& event);
    
    /**
     * Called when the user closes a file in the tabbed notebook.
     */
    void OnNotebookPageClose(wxAuiNotebookEvent& event);
    
    /**
     * Called when the user changes the page in the tabbed notebook.
     */
    void OnNotebookPageChanged(wxAuiNotebookEvent& event);
    
    /**
     * Called when the user clicks the middle mouse button on a notebook tab.
     */
    void OnNotebookTabMiddleUp(wxAuiNotebookEvent& event);

    /**
     * Called when the user clicks the right mouse button on a notebook tab.
     */
    void OnNotebookTabRightDown(wxAuiNotebookEvent& event);

    /**
     * Called when the user clicks the right mouse button on a notebook tab.
     */
    void OnNotebookTabRightUp(wxAuiNotebookEvent& event);

    /**
     * Called when the user selects the Check Out option from the notebook tab
     * context menu.
     */
    void OnNotebookTabCheckOut(wxCommandEvent& event);

    /**
     * Called when the user selects the Check In option from the notebook tab
     * context menu.
     */
    void OnNotebookTabCheckIn(wxCommandEvent& event);

    /**
     * Called when the user selects the Undo Check Out option from the notebook
     * tab context menu.
     */
    void OnNotebookTabUndoCheckOut(wxCommandEvent& event);

    /**
     * Called when the user selects the Diff option from the notebook tab
     * context menu.
     */
    void OnNotebookTabDiff(wxCommandEvent& event);

    /**
     * Called when the user selects the Show History option from the notebook
     * tab context menu.
     */
    void OnNotebookTabShowHistory(wxCommandEvent& event);

    /**
     * Called to update the Check Out and Show Changes options on the notebook
     * tab context menu.
     */
    void OnUpdateNotebookTabCheckOut(wxUpdateUIEvent& event);

    /**
     * Called to update the Check In and Undo Check Out options on the notebook
     * tab menu.
     */
    void OnUpdateNotebookTabCheckIn(wxUpdateUIEvent& event);

    /**
     * Called to update the Show History option on the notebook tab context menu.
     */
    void OnUpdateNotebookTabShowHistory(wxUpdateUIEvent& event);

    /**
     * Called when the user double clicks in the call stack window.
     */
    void OnCallStackDoubleClick(wxListEvent& event);
    
    /**
     * Called when the user double clicks in the vm list window.
     */
    void OnVmListDoubleClick(wxListEvent& event);

    /**
     * Called when the mouse has dwelled in one spot on a code editor for a while.
     */
    void OnCodeEditDwellStart(wxScintillaEvent& event);

    /**
     * Called when the mouse is no longer dwelling on a code editor.
     */
    void OnCodeEditDwellEnd(wxScintillaEvent& event);

    /**
     * Called when the the text in the code editor has left it's last saved
     * state modified.
     */
    void OnCodeEditSavePointLeft(wxScintillaEvent& event);

    /**
     * Called when the the text in the code editor has been restored to it's
     * save state.
     */
    void OnCodeEditSavePointReached(wxScintillaEvent& event);

    /**
     * Called when the the UI needs to be updated for the code editor.
     */
    void OnCodeEditUpdateUI(wxScintillaEvent& event);

    /**
     * Called when the user clicks in the margin of the code editor.
     */
    void OnCodeEditMarginClick(wxScintillaEvent& event);

    /**
     * Called when the user attempts to modify the contents of an editor
     * that represents a read-only file.
     */
    void OnCodeEditReadOnlyModifyAttempt(wxScintillaEvent& event);

    /**
     * Called when the user changes the text in the editor.
     */
    void OnCodeEditModified(wxScintillaEvent& event);

    /**
     * Called when the debugger breaks on a new line.
     */
    void OnBreak(wxDebugEvent& event);

    /**
     * Called when the debugging sessing has ended.
     */
    void OnSessionEnd(wxDebugEvent& event);

    /**
     * Called when an error occurs while loading a script.
     */
    void OnLoadError(wxDebugEvent& event);
    
    /**
     * Called when the debugger sends a text message.
     */
    void OnMessage(wxDebugEvent& event);
    
    /**
     * Called when the timer elapses.
     */
    void OnTimer(wxTimerEvent& WXUNUSED(event));

    /**
     * Called when an idle event occurs.
     */
    void OnIdle(wxIdleEvent& event);

    /**
     * Called when a process terminates.
     */
    void OnProcessTerminate(wxProcessEvent& event);

    /**
     * Called when the user activates an item in the project explorer by double
     * clicking it.
     */
    void OnProjectExplorerItemActivated(wxTreeEvent& event);

    /**
     * Called when the user presses a key in the project explorer window.
     */
    void OnProjectExplorerKeyDown(wxTreeEvent& event);
    
    /**
     * Called when the user presses a key in the output window.
     */
    void OnOutputKeyDown(wxKeyEvent& event);

    /**
     * Called when the user selects Source Control/Check In from the menu.
     */
    void OnSourceControlCheckIn(wxCommandEvent& event);

    /**
     * Called to update the Source Control/Check In menu item.
     */
    void OnUpdateSourceControlCheckIn(wxUpdateUIEvent& event);

    /**
     * Called when the user selects Source Control/Check Out from the menu.
     */
    void OnSourceControlCheckOut(wxCommandEvent& event);

    /**
     * Called to update the Source Control/Check Out menu item.
     */
    void OnUpdateSourceControlCheckOut(wxUpdateUIEvent& event);

    /**
     * Called when the user selects Source Control/Undo Check Out from the menu.
     */
    void OnSourceControlUndoCheckOut(wxCommandEvent& event);

    /**
     * Called to update the Source Control/Undo Check Out menu item.
     */
    void OnUpdateSourceControlUndoCheckOut(wxUpdateUIEvent& event);

    /**
     * Called when the user selects Source Control/Refresh File Status from the menu.
     */
    void OnSourceControlRefresh(wxCommandEvent& event);

    /**
     * Called to update the Source Control/Refresh File Status menu.
     */
    void OnUpdateSourceControlRefresh(wxUpdateUIEvent& event);

    /**
     * Called when the user closes the find dialog.
     */
    void OnFindClose(wxFindDialogEvent& event);

    /** 
     * Called when the user presses the Find button in the find dialog.
     */
    void OnFindFind(wxFindDialogEvent& event);

    /** 
     * Called when the user presses the Replace button in the find dialog.
     */
    void OnFindReplace(wxFindDialogEvent& event);

    /** 
     * Called when the user presses the Replace All button in the find dialog.
     */
    void OnFindReplaceAll(wxFindDialogEvent& event);

    /** 
     * Called when the user presses the find next button in the find dialog.
     */
    void OnFindNext(wxFindDialogEvent& event);

    /**
     * Called when the user selects the hot key to go to the project explorer
     * filter window.
     */
    void OnMiscOpenInFilter(wxCommandEvent& event);

    /**
     * Called when the window is activated or deactivated.
     */
    void OnActivate(wxActivateEvent& event);

    /**
     * From wxFileDropTarget. Called when the user drags a file from Explorer onto
     * the window.
     */
    bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames);

    /**
     * Called when the user selects the Check Out option from the project
     * explorer context menu.
     */
    void OnContextCheckOut(wxCommandEvent& event);

    /**
     * Called when the user selects the Check In option from the project
     * explorer context menu.
     */
    void OnContextCheckIn(wxCommandEvent& event);

    /**
     * Called when the user selects the Undo Check Out option from the project
     * explorer context menu.
     */
    void OnContextUndoCheckOut(wxCommandEvent& event);

    /**
     * Called when the user selects the Diff option from the project
     * explorer context menu.
     */
    void OnContextDiff(wxCommandEvent& event);

    /**
     * Called when the user selects the Show History option from the project
     * explorer context menu.
     */
    void OnContextShowHistory(wxCommandEvent& event);

    /**
     * Called when the user selects the Remove option from the project
     * explorer context menu.
     */
    void OnContextRemove(wxCommandEvent& event);

    /**
     * Called to update the Check Out and Show Changes options on the context menu.
     */
    void OnUpdateContextCheckOut(wxUpdateUIEvent& event);

    /**
      * Called to update the Check In and Undo Check Out options on the context menu.
     */
    void OnUpdateContextCheckIn(wxUpdateUIEvent& event);

    /**
     * Called to update the Show History option on the context menu.
     */
    void OnUpdateContextShowHistory(wxUpdateUIEvent& event);

    /**
     * Called when one of the worker threads exits.
     */
    void OnThreadExit(ThreadEvent& event);

    /**
     * Called when the application should display help in response to a user action.
     */
    void OnShowHelp(wxCommandEvent& event);

    /**
     * Called when information about updates has been retrieved from the server.
     */
    void OnUpdateInfo(wxCommandEvent& event);

    /** 
     * Called when the symbols for a file are finished being parsed.
     */
    void OnSymbolsParsed(SymbolParserEvent& event);

    /**
     * Moves the caret to the line in the script indicated in an error message 
     * and brings the editor into focus.
     */
    void GotoError(const wxString& message);

    /**
     * Opens the specified file and goes to the line.
     */
    void GotoNewLine(Project::File* file, unsigned int newLine);

    /**
     * Substitutes variable arguments of the form $(...) into the text based
     * on the current environment.
     */
    void SubstituteVariableArguments(wxString& text) const;
    
    /**
     * Enables the item when the debugger is broken (at a breakpoint).
     */
    void EnableWhenBroken(wxUpdateUIEvent& event);

    /**
     * Enables the item when the debugger is not running.
     */
    void EnableWhenInactive(wxUpdateUIEvent& event);

    /**
     * Enables the item when a file is open for editing.
     */
    void EnableWhenFileIsOpen(wxUpdateUIEvent& event);

    /**
     * Enables the item when a file is open for editing and has the keyboard focus,
     */
    void EnableWhenFileHasFocus(wxUpdateUIEvent& event);

    /**
     * Enables the item when a text is selected in an open file and the file has keyboard focus.
     */
    void EnableWhenTextIsSelected(wxUpdateUIEvent& event);

    /**
     * Opens a new project in the editor. If the reportError flag is set and the project
     * could not be opened, an error dialog is displayed.
     */
    bool OpenProject(const wxString& fileName, bool reportError = false);

    /**
     * Creates a new, unnamed project.
     */
    void NewProject();

    /**
     * Sets the project currently being edited.
     */
    void SetProject(Project* project);

    /**
     * Opens the specified document into the editor.
     */
    OpenFile* OpenDocument(const wxString& fileName);

    /**
     * Opens all of the specified documents in the editor.
     */
    void OpenDocuments(const wxArrayString& fileNames);

    /**
     * Begins debugging of the specified executable. This overrides the project
     * settings.
     */
    void DebugExe(const wxString& fileName);

    /**
     * Shows the watch window if it's not already displayed.
     */
    void ShowWatchWindow();

    /**
     * Shows the output window if it's not already displayed.
     */
    void ShowOutputWindow();

    /**
     * Shows the search window if it's not already displayed.
     */
    void ShowSearchWindow();

    /**
     * From wxWindow.
     */
    virtual bool MSWProcessMessage(WXMSG* pMsg);

    /**
     * Talks to the mothership to see if there's an updated version of the
     * application available. This is done asynchronously.
     */
    void CheckForUpdate();

    /**
     * Moves the window and shows it based on the saved window placement.
     */
    void ApplyWindowPlacement();

    /**
     * Loads the last project if the user has selected that opion.
     */
    void AutoOpenLastProject();
    
    /**
     * Toggles a break point on or off on the line of the file specified.
     */
    void ToggleBreakpoint(Project::File* file, unsigned int newLine);

    /**
     * Removes all breakpoints. This presents the user with a confirmation dialog.
     */
    void DeleteAllBreakpoints();

    /**
     * Maps a line number from the currently open page to the backend script.
     */
    unsigned int NewToOldLine(Project::File* file, unsigned int newLine);

    /**
     * Maps a line number from the backend script to the currently open page.
     */
    unsigned int OldToNewLine(Project::File* file, unsigned int oldLine);

    DECLARE_EVENT_TABLE()

private:
    
    enum Mode
    {
        Mode_Editing,
        Mode_Debugging,
        Mode_NumModes,  // Always keep this last.
    };

    /**
     * Creates the menu for the window.
     */
    void InitializeMenu();

    /**
     * Sets the line that is currently being displayed. This will open
     * the script and scoll to the line.
     */
    void ShowScriptLine(unsigned int scriptIndex, unsigned int line);

    /**
     * Removes the small green arrow that indicates the current line selected
     * in the call stack from whatever file its currently on.
     */
    void ClearCurrentLineMarker();

    /**
     * Removes the small yellow arrow that indicates the line that execution is
     * broken on from whatever file its currently on.
     */
    void ClearBreakLineMarker();

    /**
     * Moves the caret to the line in the edit window indicated and brings the
     * editor into focus.
     */
    void GotoNewLine(CodeEdit* edit, unsigned int newLine, bool center);

    /**
     * Moves the caret to the line in the script indicated and brings the editor
     * into focus.
     */
    OpenFile* GotoOldLine(unsigned int scriptIndex, unsigned int newLine, bool center);

    /**
     * Extracts the topic from an error message which is formatted to correspond
     * to a help section. The messages are in the form "Warning ####: " or
     * "Error ####: ". If the format matches one of these, the topic will be
     * "warning_####" or "error_####".
     */
    bool ParseHelpMessage(const wxString& message, wxString& topic) const;

    /**
     * Extracts the file name and line number from an error message. If the
     * error message doesn't have a recognizable form, the method returns false.
     */
    bool ParseErrorMessage(const wxString& error, wxString& fileName, unsigned int& line) const;

    /**
     * Extracts the file name and line number from a Lua-style error message. A Lua-style error
     * message has the form "filename:line: message". If the error message doesn't have
     * that form, the method returns false.
     */
    bool ParseLuaErrorMessage(const wxString& error, wxString& fileName, unsigned int& line) const;

    /**
     * Extracts the file name and line number from a Luac-style error message. A Luac-style error
     * message has the form "appname: filename:line: message". If the error message doesn't have
     * that form, the method returns false.
     */
    bool ParseLuacErrorMessage(const wxString& error, wxString& fileName, unsigned int& line) const;

    /**
     * Gets the index of the script associated with an edit control. If there's no script
     * associated with it the method returns -1.
     */
    unsigned int GetScriptIndex(wxScintilla* edit) const;

    /**
     * Sets the vm that the UI is controlling/inspecting.
     */
    void SetContext(unsigned int vm, unsigned int stackLevel);

    /**
     * Adds a new process to the list of processes that are running. Running
     * processes will have their output redirected to the output window.
     */
    void AddRunningProcess(wxProcess* process);

    /**
     * Removes a process from the list of running processes. This should be done
     * when the process terminates.
     */
    void RemoveRunningProcess(wxProcess* process);

    /**
     * Updates the tools menu items based on the current external tools.
     */
    void UpdateToolsMenu();

    /**
     * Run the passed in tool and handle all book keeping.
     */
    void RunTool(ExternalTool* tool);

    /**
     * Loads the options from disk.
     */
    void LoadOptions();

    /**
     * Saves the options to disk.
     */
    void SaveOptions();

    /**
     * Loads the options for an external tool.
     */
    void LoadExternalTool(wxXmlNode* node);

    /**
     * Creates a new XML node representing the tool.
     */
    wxXmlNode* SaveExternalTool(const ExternalTool* tool) const;

    /**
     * Loads the positioning for the window.
     */
    void LoadWindowPlacement(wxXmlNode* node);

    /**
     * Saves the positioning for the window.
     */
    wxXmlNode* SaveWindowPlacement() const;

    /**
     * Saves the current project. If the project doesn't have a name, this will prompt
     * the user with a save dialog box. If the operation fails or the user cancels, this
     * will return false.
     */
    bool SaveProject(bool promptForName = false);

    /**
     * Saves the project file if it's been modified since it was last saved/loaded.
     * Returns false if there was an error saving or the user cancelled.
     */
    bool SaveProjectIfNeeded();

    /**
     * Returns the open file that corresponds to the scriptIndex. If the
     * file is not opened the method retrns NULL.
     */
    OpenFile* GetFileForScript(unsigned int scriptIndex) const;

    /**
     * Opens script code from the debugger into the editor.
     */
    OpenFile* OpenScript(unsigned int scriptIndex);

    /**
     * Closes all of the open files. If any of the open files need to be saved, the
     * used will be prompted. If the user cancels the save, the method returns false.
     */
    bool CloseAllFiles();

    /**
     * Updates the caption for the window based on the current state (debugging
     * status, project name, etc.)
     */
    void UpdateCaption();

    /**
     * Updates the UI when the debugging state has changed.
     */
    void UpdateForNewState();

    /**
     * Opens a file from the project into the editor.
     */
    OpenFile* OpenProjectFile(Project::File* file);

    /**
     * Removes a file from the project (does not delete the file on disk).
     */
    void DeleteProjectFile(Project::File* file);

    /**
     * Updates the break point marker in an open file. If set is enabled the
     * marker will be added at the specified line, otherwise it will be removed.
     * Note this only updates the UI and does not actually set breakpoints.
     */
    void UpdateFileBreakpoint(OpenFile* file, unsigned int line, bool set);

    /**
     * Adds the virtual machine to the list of virtual machines handled by the
     * debugger.
     */
    void AddVmToList(unsigned int vm);

    /**
     * Removes the specified virtual machine from the list of virtual machines
     * handled by the debugger.
     */
    void RemoveVmFromList(unsigned int vm);

    /**
     * Updates the name of a virtual machine.
     */
    void SetVmName(unsigned int vm, const wxString& name);

    /**
     * Saves thge specified file. If the file does not have an appropriate file
     * name associated with it, the user will be prompted for one.
     */
    bool SaveFile(OpenFile* file, bool promptForName);

    /**
     * Closes the document in the specified page of the notebook.
     */
    void CloseDocument(int pageIndex, bool promptForSave = true);

    /**
     * Saves all of the open files that need to be saved.
     */
    void SaveAllFiles();

    /** 
     * Updates all of the open editors with the current font, color, etc.
     * options.
     */
    void UpdateEditorOptions();

    /**
     * Searches for the specified text in the open file.
     */
    void FindText(OpenFile* file, const wxString& text, int flags);

    /**
     * Performs a find next operation in the currently open document.
     */
    void FindNext(const wxString& text, int flags);

    /** 
     * Adds the default profile to the hot keys.
     */
    void SetDefaultHotKeys();

    /**
     * Updates the text in the line and column fields on the status bar.
     */
    void UpdateStatusBarLineAndColumn();

    /**
     * Checks all of the open files needs to see if any need to be reloaded
     * because they have changed on disk.
     */
    void CheckReload();

    /**
     * Checks to see if the specified file needs to be reloaded because it has
     * changed on disk.
     */
    void CheckReload(OpenFile* file);

    /**
     * Reloads the contents of the specified file from disk.
     */
    void ReloadFile(OpenFile* file);

    /**
     * Searches the files for the specified text. If baseDirectory is specified,
     * the filenames are all displayed relative to the directory.
     */
    void FindInFiles(const wxString& text, const wxArrayString& fileNames,
        bool matchCase, bool matchWholeWord, const wxString& baseDirectory);

    /**
     * Searches a line for a particular piece of text. The comparision is case
     * sensitive (convert both to lower case for an insensitive search). If
     * matchWholeWord is true, the text must be separated by delimiters on each
     * side.
     */
    bool FindInLine(const wxString& text, const wxString& line, bool matchWholeWord) const;

    /**
     * Returns the time when the file was last modified. If the file does not exist
     * the method returns 0.
     */
    time_t GetFileModifiedTime(const wxString& fileName) const;

    /**
     * Removes any temporary files from the project that aren't open in the
     * editor. This is useful after a debugging session has ended and temporary
     * files that were automatically opened during debugging need to be removed.
     */
    void CleanUpTemporaryFiles();

    /**
     * Returns the index into the open files array for the specified project file. If
     * the project file is not open, the method returns -1.
     */
    unsigned int GetOpenFileIndex(Project::File* file) const;

    /**
     * Initiates debugging using the current project settings. If no executable is
     * specified in the project settings, the user will be prompted to enter one.
     */
    void StartProcess(bool debug, bool startBroken);

    /**
     * Initiates debugging using the specified settings.
     */
    void StartProcess(const wxString& command, const wxString& commandArguments,
        const wxString& workingDirectory, const wxString& symbolsDirectory, bool debug, bool startBroken);

    /**
     * Returns the index of the currently selected page (or open file index) or -1
     * if no document is open.
     */
    int GetSelectedPage() const;

    /**
     * Removes all of the breakpoints from the specified file. This will also remove
     * the markers if the file is open in the editor. Note this won't remove the
     * breakpoints from the backend.
     */
    void RemoveAllLocalBreakpoints(Project::File* file);

    /**
     * Initialize source control for the current project, if this project is
     * setup to use source control. Otherwise it has no effect.
     */
    bool InitializeSourceControl();

    /**
     * Updates the source control status of all the files in the project.
     */
    void UpdateProjectFileStatus();

    /**
     * Updates the soruce control status of a single file in the project.
     */
    void UpdateProjectFileStatus(Project::File* file);

    /**
     * Gets the names of all the files selected in the project explorer widow.
     */
    void GetProjectSelectedFileNames(std::vector<std::string>& fileNames) const;

    /**
     * Gets the names of all the files selected in the notebook. Note there is only
     * ever one tab selected current explorer window.
     */
    void GetNotebookTabSelectedFileNames(std::vector<std::string>& fileNames) const;

    /**
     * Changes to a new mode and rearranges the layout.
     */
    void SetMode(Mode mode);

    /**
     * Returns the directory where application data should be stored.
     */
    wxString GetAppDataDirectory() const;

    /**
     * Shows the project settings dialog. Returns true if the user hits ok.
     */
    bool ShowProjectSettingsDialog();

    /**
     * Updates the read-only status of the currently open document's editor.
     */
    void UpdateDocumentReadOnlyStatus();

    /**
     * Callback used to notify the application when information about updates
     * available is ready.
     */
    static void UpdateCallback(Updater* updater, void* param);
    
    /**
     * Handles applying an update if one is available. If no update is available
     * the method does nothing.
     */
    void HandleUpdate();

    /**
     * Updates the syntax coloring for the file based on the file name extension.
     */
    void UpdateSyntaxColoring(OpenFile* openFile);

    /**
     * Call this before closing a notebook page to handle saving, cleaning up
     * temporary files, etc. If the function returns false, the user cancelled
     * the close.
     */
    bool PreNotebookPageClose(int page, bool promptForSave = true);

    /**
     * Updates the internally stored startup placement fields based on the current
     * state of the window.
     */
    void UpdateStartUpPlacement();

    /**
     * Brings the window to the front of the z-order.
     */
    void BringToFront();

    /**
     * Matches up a project file to a script file.
     */
    Project::File* GetFileMatchingSource(const wxFileName& fileName, const std::string& source) const;

    /**
     * Called when a new file is added to the project.
     */
    void UpdateForNewFile(Project::File* file);

    /**
     * Updates the line mapper for the specified page. If the line mapper is
     * up-to-date this method does nothing.
     */
    void UpdateLineMappingIfNecessary(Project::File* file);

    /**
     * Sets the status for a file based on the specified source control status.
     */
    void SetFileStatus(Project::File* file, SourceControl::Status status);

    /**
     * Updates the line mapping for script file source based on the a diff with a disk file.
     */
    void UpdateScriptLineMappingFromFile(const Project::File* file, DebugFrontend::Script* script);

    /**
     * Marks the specified page as 'most recently used', i.e. moves it to the front of m_tabOrder.
     * If m_tabOrder does not contain pageIndex, it will be added.
     */
    void SetMostRecentlyUsedPage(int pageIndex);

    /**
     * Removes the specified page from m_tabOrder and decrement all page indices greater than pageIndex.
     */
    void RemovePageFromTabOrder(int pageIndex);
        
private:

    /**
     * The values for these enums are important because they are stored in the
     * configuration file that saves key bindings. To avoid invalidating existing
     * key bindings, they should not be changed.
     */
    enum
    {
        ID_Notebook                         = 1,
        ID_CallStack                        = 2,
        ID_VmList                           = 3,
        ID_Output                           = 4,
        ID_Watch                            = 5,
        ID_ProjectExplorer                  = 6,
        ID_FileNewProject                   = 7,
        ID_FileSaveProject                  = 8,
        ID_FileOpenProject                  = 9,
        ID_FileExit                         = 10,
        ID_FileOpen                         = 11,
        ID_FileSave                         = 12,
        ID_EditUndo                         = 13,
        ID_EditRedo                         = 14,
        ID_EditUntabify                     = 15,
        ID_EditFind                         = 16,
        ID_EditFindNext                     = 17,
        ID_ProjectAddExistingFile           = 18,
        ID_ProjectSettings                  = 19,
        ID_DebugStart                       = 20,
        ID_DebugStop                        = 21,
        ID_DebugDetach                      = 22,
        ID_DebugBreak                       = 24,
        ID_DebugStepInto                    = 25,
        ID_DebugStepOver                    = 26,
        ID_DebugQuickWatch                  = 27,
        ID_DebugToggleBreakpoint            = 28,
        ID_ToolsSettings                    = 29,
        ID_ToolsExternalTools               = 30,
        ID_WindowProjectExplorer            = 32,
        ID_WindowCallStack                  = 33,
        ID_WindowOutput                     = 34,
        ID_WindowWatch                      = 35,  
        ID_WindowVirtualMachines            = 36,
        ID_WindowClose                      = 37,
        ID_WindowCloseAllDocuments          = 38,
        ID_WindowNextDocument               = 39,
        ID_WindowPreviousDocument           = 40,
        ID_HelpAbout                        = 41,
        ID_DebugAttachToHost                = 42,
        ID_MiscOpenInFilter                 = 43,
        ID_ProjectAddNewFile                = 44,
        ID_EditFindInFiles                  = 45,
        ID_FileSaveAs                       = 46,
        ID_EditGotoLine                     = 47,
        ID_FileSaveProjectAs                = 48,
        ID_FileNew                          = 49,
        
        ID_ContextCheckOut                  = 50,
        ID_ContextCheckIn                   = 51,
        ID_ContextUndoCheckOut              = 52,
        ID_ContextDiff                      = 53,
        ID_ContextShowHistory               = 54,
        ID_ContextRemove                    = 55,
        
        ID_SourceControlRefresh             = 56,
        ID_SourceControlCheckOut            = 57,
        ID_SourceControlCheckIn             = 58,
        ID_SourceControlUndoCheckOut        = 59,
        ID_EditReplace                      = 60,
        ID_EditComment                      = 61,
        ID_EditUncomment                    = 62,
        ID_HelpContents                     = 63,
        ID_HelpCheckForUpdate               = 64,
        ID_DebugStartWithoutDebugging       = 65,
        ID_DebugProcess                     = 66,
        // 67 was help/register
        ID_EditFindPrevious                 = 68,

        ID_NotebookTabCheckOut              = 69,
        ID_NotebookTabCheckIn               = 70,
        ID_NotebookTabUndoCheckOut          = 71,
        ID_NotebookTabDiff                  = 72,
        ID_NotebookTabShowHistory           = 73,
        ID_NotebookTabSave                  = 74, 
        ID_NotebookTabClose                 = 75,

        ID_EditCut                          = 76,
        ID_EditCopy                         = 77,
        ID_EditPaste                        = 78,
        ID_EditDelete                       = 79,
        ID_EditSelectAll                    = 80,
        
        ID_Breakpoints                      = 81,
        ID_WindowBreakpoints                = 82,

        ID_HelpSupport                      = 83,
        
        ID_DebugDeleteAllBreakpoints        = 84,

        ID_NotebookTabShowFile              = 85,

        ID_Search                           = 86,
        ID_WindowSearch                     = 87,
        
        ID_FirstExternalTool                = 1000,
        ID_FirstRecentFile                  = 2000,
        ID_FirstRecentProjectFile           = 3000,
    };

    static const wxString           s_scriptExtensions;
    static const wxString           s_applicationName;
    static const wxString           s_modeName[Mode_NumModes];
    static const char*              s_updateUrl;
 
    int                             m_openFilesMessage;

    Updater                         m_updater;

    Project*                        m_project;       
    std::vector<OpenFile*>          m_openFiles;
    SymbolParser*                   m_symbolParser;
    bool                            m_waitForFinalSymbolParse; //For batch loading files more efficiently 
    
    wxAuiManager                    m_mgr;
    wxString                        m_modeLayout[Mode_NumModes];
    wxAuiNotebook*                  m_notebook;
    wxStatusBar*                    m_statusBar;
    Mode                            m_mode;

    wxMenu*                         m_menuTools;

    ProjectExplorerWindow*          m_projectExplorer;
    ListWindow*                     m_callStack;
    ListWindow*                     m_vmList;
    OutputWindow*                   m_output;    
    WatchWindow*                    m_watch;
    BreakpointsWindow*              m_breakpointsWindow;
    SearchWindow*                   m_searchWindow;

    unsigned int                    m_vm;
    std::vector<unsigned int>       m_vms;
    unsigned int                    m_stackLevel;

    unsigned int                    m_currentScriptIndex;
    unsigned int                    m_currentLine;
    
    unsigned int                    m_breakScriptIndex;
    unsigned int                    m_breakLine;

    std::vector<ExternalTool*>      m_tools;
    std::vector<wxProcess*>         m_runningProcesses;
    wxTimer                         m_timerIdleWakeUp;

    wxFindReplaceDialog*            m_findDialog;
    wxFindReplaceData               m_findData;
    int                             m_findStart;
    int                             m_lastFindPosition;

    KeyBinder                       m_keyBinder;
    FontColorSettings               m_fontColorSettings;
    EditorSettings                  m_editorSettings;
    SystemSettings                  m_systemSettings;

    bool                            m_attachToHost;

    wxFileHistory                   m_fileHistory;
    wxFileHistory                   m_projectFileHistory;
    wxString                        m_lastProjectLoaded;

    wxFileHistory                   m_findDirectoryHistory;
    StringHistory                   m_findTextHistory;

    SourceControl                   m_sourceControl;

    wxMenu*                         m_contextMenu;
    wxMenu*                         m_notebookTabMenu;

    FileStatusThread*               m_fileStatusThread[2];            
    
    FileChangeWatcher               m_fileChangeWatcher;

    wxCHMHelpController             m_helpController;

    wxRect                          m_startUpRect;
    bool                            m_startUpMaximized;

    AutoCompleteManager             m_autoCompleteManager;
    ProcessOutputSink               m_processOutputSink;

    std::vector<int>                m_tabOrder;
    int                             m_tabOrderIndex;
};

#endif
