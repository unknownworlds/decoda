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

#include "Config.h"
#include "MainFrame.h"
#include "MainApp.h"
#include "DebugFrontend.h"
#include "KeyBinderDialog.h"
#include "NewProcessDialog.h"
#include "QuickWatchDialog.h"
#include "WatchWindow.h"
#include "OutputWindow.h"
#include "BreakpointsWindow.h"
#include "SearchWindow.h"
#include "ProjectExplorerWindow.h"
#include "ExternalTool.h"
#include "ExternalToolsDialog.h"
#include "CodeEdit.h"
#include "FileUtility.h"
#include "StlUtility.h"
#include "XmlUtility.h"
#include "XmlConfig.h"
#include "Project.h"
#include "NewFileDialog.h"
#include "FindInFilesDialog.h"
#include "MainFrameDropTarget.h"
#include "CodeEditDropTarget.h"
#include "SettingsDialog.h"
#include "FontColorSettingsPanel.h"
#include "EditorSettingsPanel.h"
#include "SystemSettingsPanel.h"
#include "AboutDialog.h"
#include "FileStatusThread.h"
#include "ThreadEvent.h"
#include "DebugEvent.h"
#include "FileEvent.h"
#include "ShowHelpEvent.h"
#include "ChoiceDialog.h"
#include "ReadOnlyDialog.h"
#include "ProcessesDialog.h"
#include "ExceptionDialog.h"
#include "Symbol.h"
#include "ListWindow.h"
#include "SymbolParser.h"
#include "SymbolParserEvent.h"
#include "Tokenizer.h"

#include <wx/txtstrm.h>
#include <wx/xml/xml.h>
#include <wx/fontdlg.h>
#include <wx/file.h>
#include <wx/dir.h>
#include <wx/wfstream.h>
#include <wx/string.h>
#include <wx/numdlg.h>
#include <wx/sstream.h>

#include <shlobj.h>

#include <algorithm>
#include <hash_map>


// Event used to communicate when the update data has been downloaded.
DECLARE_EVENT_TYPE(wxEVT_UPDATE_INFO_EVENT, -1)
DEFINE_EVENT_TYPE(wxEVT_UPDATE_INFO_EVENT)

BEGIN_EVENT_TABLE(MainFrame, wxFrame)

    EVT_CLOSE(                                      MainFrame::OnClose)

    // File menu events.
    EVT_MENU(ID_FileExit,                           MainFrame::OnFileExit)
    EVT_MENU(ID_FileNew,                            MainFrame::OnFileNew)
    EVT_MENU(ID_FileOpen,                           MainFrame::OnFileOpen)
    EVT_MENU(ID_FileSave,                           MainFrame::OnFileSave)
    EVT_UPDATE_UI(ID_FileSave,                      MainFrame::EnableWhenFileIsOpen)
    EVT_MENU(ID_FileSaveAs,                         MainFrame::OnFileSaveAs)
    EVT_UPDATE_UI(ID_FileSaveAs,                    MainFrame::EnableWhenFileIsOpen)
    
    // Edit menu events.
    EVT_MENU(ID_EditUndo,                           MainFrame::OnEditUndo)
    EVT_UPDATE_UI(ID_EditUndo,                      MainFrame::EnableWhenFileIsOpen)
    EVT_MENU(ID_EditRedo,                           MainFrame::OnEditRedo)
    EVT_UPDATE_UI(ID_EditRedo,                      MainFrame::EnableWhenFileIsOpen)
    EVT_MENU(ID_EditCut,                            MainFrame::OnEditCut)
    EVT_UPDATE_UI(ID_EditCut,                       MainFrame::EnableWhenTextIsSelected)
    EVT_MENU(ID_EditCopy,                           MainFrame::OnEditCopy)
    EVT_UPDATE_UI(ID_EditCopy,                      MainFrame::EnableWhenTextIsSelected)
    EVT_MENU(ID_EditPaste,                          MainFrame::OnEditPaste)
    EVT_UPDATE_UI(ID_EditPaste,                     MainFrame::EnableWhenFileHasFocus)
    EVT_MENU(ID_EditDelete,                         MainFrame::OnEditDelete)
    EVT_UPDATE_UI(ID_EditDelete,                    MainFrame::EnableWhenTextIsSelected)
    EVT_MENU(ID_EditSelectAll,                      MainFrame::OnEditSelectAll)
    EVT_UPDATE_UI(ID_EditSelectAll,                 MainFrame::EnableWhenFileIsOpen)
    EVT_MENU(ID_EditFind,                           MainFrame::OnEditFind)
    EVT_UPDATE_UI(ID_EditFind,                      MainFrame::EnableWhenFileIsOpen)
    EVT_MENU(ID_EditReplace,                        MainFrame::OnEditReplace)
    EVT_UPDATE_UI(ID_EditReplace,                   MainFrame::EnableWhenFileIsOpen)
    EVT_MENU(ID_EditFindNext,                       MainFrame::OnEditFindNext)
    EVT_UPDATE_UI(ID_EditFindNext,                  MainFrame::EnableWhenFileIsOpen)
    EVT_MENU(ID_EditFindPrevious,                   MainFrame::OnEditFindPrevious)
    EVT_UPDATE_UI(ID_EditFindPrevious,              MainFrame::EnableWhenFileIsOpen)
    EVT_MENU(ID_EditFindInFiles,                    MainFrame::OnEditFindInFiles)
    EVT_MENU(ID_EditGotoLine,                       MainFrame::OnEditGotoLine)
    EVT_UPDATE_UI(ID_EditGotoLine,                  MainFrame::EnableWhenFileIsOpen)
    EVT_MENU(ID_EditUntabify,                       MainFrame::OnEditUntabify)
    EVT_UPDATE_UI(ID_EditUntabify,                  MainFrame::EnableWhenTextIsSelected)
    EVT_MENU(ID_EditComment,                        MainFrame::OnEditComment)
    EVT_UPDATE_UI(ID_EditComment,                   MainFrame::EnableWhenTextIsSelected)
    EVT_MENU(ID_EditUncomment,                      MainFrame::OnEditUncomment)
    EVT_UPDATE_UI(ID_EditUncomment,                 MainFrame::EnableWhenTextIsSelected)
    
    // Project menu events.
    EVT_MENU(ID_ProjectAddExistingFile,             MainFrame::OnProjectAddExistingFile)
    EVT_MENU(ID_ProjectAddNewFile,                  MainFrame::OnProjectAddNewFile)
    EVT_MENU(ID_ProjectSettings,                    MainFrame::OnProjectSettings)
    EVT_MENU(ID_FileNewProject,                     MainFrame::OnFileNewProject)
    EVT_UPDATE_UI(ID_FileNewProject,                MainFrame::EnableWhenInactive)
    EVT_MENU(ID_FileSaveProject,                    MainFrame::OnFileSaveProject)
    EVT_MENU(ID_FileSaveProjectAs,                  MainFrame::OnFileSaveProjectAs)
    EVT_MENU(ID_FileOpenProject,                    MainFrame::OnFileOpenProject)
    EVT_UPDATE_UI(ID_FileOpenProject,               MainFrame::EnableWhenInactive)

    // SCC menu events.
    EVT_MENU(ID_SourceControlCheckIn,               MainFrame::OnSourceControlCheckIn)
    EVT_UPDATE_UI(ID_SourceControlCheckIn,          MainFrame::OnUpdateSourceControlCheckIn)
    EVT_MENU(ID_SourceControlCheckOut,              MainFrame::OnSourceControlCheckOut)
    EVT_UPDATE_UI(ID_SourceControlCheckOut,         MainFrame::OnUpdateSourceControlCheckOut)
    EVT_MENU(ID_SourceControlUndoCheckOut,          MainFrame::OnSourceControlUndoCheckOut)
    EVT_UPDATE_UI(ID_SourceControlUndoCheckOut,     MainFrame::OnUpdateSourceControlUndoCheckOut)
    EVT_MENU(ID_SourceControlRefresh,               MainFrame::OnSourceControlRefresh)
    EVT_UPDATE_UI(ID_SourceControlRefresh,          MainFrame::OnUpdateSourceControlRefresh)

    // Debug menu events.
    EVT_MENU(ID_DebugStart,                         MainFrame::OnDebugStart)
    EVT_UPDATE_UI(ID_DebugStart,                    MainFrame::OnUpdateDebugStart)
    EVT_MENU(ID_DebugStartWithoutDebugging,         MainFrame::OnDebugStartWithoutDebugging)
    EVT_UPDATE_UI(ID_DebugStartWithoutDebugging,    MainFrame::EnableWhenInactive)
    EVT_MENU(ID_DebugProcess,                       MainFrame::OnDebugProcess)
    EVT_UPDATE_UI(ID_DebugProcess,                  MainFrame::EnableWhenInactive)
    EVT_MENU(ID_DebugStop,                          MainFrame::OnDebugStop)
    EVT_UPDATE_UI(ID_DebugStop,                     MainFrame::OnUpdateDebugStop)
    EVT_MENU(ID_DebugAttachToHost,                  MainFrame::OnDebugAttachToHost)
    EVT_MENU(ID_DebugDetach,                        MainFrame::OnDebugDetach)
    EVT_UPDATE_UI(ID_DebugDetach,                   MainFrame::OnUpdateDebugStop)
    EVT_MENU(ID_DebugBreak,                         MainFrame::OnDebugBreak)
    EVT_UPDATE_UI(ID_DebugBreak,                    MainFrame::OnUpdateDebugBreak)
    EVT_MENU(ID_DebugStepInto,                      MainFrame::OnDebugStepInto)
    EVT_UPDATE_UI(ID_DebugStepInto,                 MainFrame::OnUpdateDebugStepInto)
    EVT_MENU(ID_DebugStepOver,                      MainFrame::OnDebugStepOver)
    EVT_UPDATE_UI(ID_DebugStepOver,                 MainFrame::EnableWhenBroken)
    EVT_MENU(ID_DebugQuickWatch,                    MainFrame::OnDebugQuickWatch)
    EVT_UPDATE_UI(ID_DebugQuickWatch,               MainFrame::EnableWhenBroken)
    EVT_MENU(ID_DebugToggleBreakpoint,              MainFrame::OnDebugToggleBreakpoint)
    EVT_UPDATE_UI(ID_DebugToggleBreakpoint,         MainFrame::EnableWhenFileIsOpen)
    EVT_MENU(ID_DebugDeleteAllBreakpoints,          MainFrame::OnDebugDeleteAllBreakpoints)

    // Tools menu events.
    EVT_MENU(ID_ToolsExternalTools,                 MainFrame::OnToolsExternalTools)
    EVT_MENU(ID_ToolsSettings,                      MainFrame::OnToolsSettings)

    // Window menu events.
    EVT_MENU(ID_WindowProjectExplorer,              MainFrame::OnWindowProjectExplorer)
    EVT_MENU(ID_WindowCallStack,                    MainFrame::OnWindowCallStack)
    EVT_MENU(ID_WindowOutput,                       MainFrame::OnWindowOutput)
    EVT_MENU(ID_WindowSearch,                       MainFrame::OnWindowSearch)
    EVT_MENU(ID_WindowWatch,                        MainFrame::OnWindowWatch)
    EVT_MENU(ID_WindowVirtualMachines,              MainFrame::OnWindowVirtualMachines)
    EVT_MENU(ID_WindowBreakpoints,                  MainFrame::OnWindowBreakpoints)
    EVT_MENU(ID_WindowNextDocument,                 MainFrame::OnWindowNextDocument)
    EVT_MENU(ID_WindowPreviousDocument,             MainFrame::OnWindowPreviousDocument)
    EVT_MENU(ID_WindowClose,                        MainFrame::OnWindowClose)
    EVT_UPDATE_UI(ID_WindowClose,                   MainFrame::EnableWhenFileIsOpen)
    EVT_MENU(ID_WindowCloseAllDocuments,            MainFrame::OnWindowCloseAllDocuments)

    // Help menu events.
    EVT_MENU(ID_HelpAbout,                          MainFrame::OnHelpAbout)
    EVT_MENU(ID_HelpSupport,                        MainFrame::OnHelpSupport)
    EVT_MENU(ID_HelpCheckForUpdate,                 MainFrame::OnHelpCheckForUpdate)
    EVT_MENU(ID_HelpContents,                       MainFrame::OnHelpContents)

    // Notebook tab context menu events.
    EVT_MENU(ID_NotebookTabCheckOut,                MainFrame::OnNotebookTabCheckOut)
    EVT_UPDATE_UI(ID_NotebookTabCheckOut,           MainFrame::OnUpdateNotebookTabCheckOut)
    EVT_MENU(ID_NotebookTabCheckIn,                 MainFrame::OnNotebookTabCheckIn)
    EVT_UPDATE_UI(ID_NotebookTabCheckIn,            MainFrame::OnUpdateNotebookTabCheckIn)
    EVT_MENU(ID_NotebookTabDiff,                    MainFrame::OnNotebookTabDiff)
    EVT_UPDATE_UI(ID_NotebookTabDiff,               MainFrame::OnUpdateNotebookTabCheckIn)
    EVT_MENU(ID_NotebookTabUndoCheckOut,            MainFrame::OnNotebookTabUndoCheckOut)
    EVT_UPDATE_UI(ID_NotebookTabUndoCheckOut,       MainFrame::OnUpdateNotebookTabCheckIn)
    EVT_MENU(ID_NotebookTabShowHistory,             MainFrame::OnNotebookTabShowHistory)
    EVT_UPDATE_UI(ID_NotebookTabShowHistory,        MainFrame::OnUpdateNotebookTabShowHistory)
    EVT_MENU(ID_NotebookTabSave,                    MainFrame::OnFileSave)
    EVT_MENU(ID_NotebookTabClose,                   MainFrame::OnWindowClose)
    EVT_MENU(ID_NotebookTabShowFile,                MainFrame::OnFileShow)

    // Project explorer related events.
    EVT_MENU(ID_MiscOpenInFilter,                   MainFrame::OnMiscOpenInFilter)
    EVT_TREE_ITEM_ACTIVATED(ID_ProjectExplorer,     MainFrame::OnProjectExplorerItemActivated)
    EVT_TREE_KEY_DOWN(ID_ProjectExplorer,           MainFrame::OnProjectExplorerKeyDown)
    
    EVT_MENU(ID_ContextCheckOut,                    MainFrame::OnContextCheckOut)
    EVT_MENU(ID_ContextCheckIn,                     MainFrame::OnContextCheckIn)
    EVT_MENU(ID_ContextUndoCheckOut,                MainFrame::OnContextUndoCheckOut)
    EVT_MENU(ID_ContextDiff,                        MainFrame::OnContextDiff)
    EVT_MENU(ID_ContextShowHistory,                 MainFrame::OnContextShowHistory)
    EVT_MENU(ID_ContextRemove,                      MainFrame::OnContextRemove)
    EVT_UPDATE_UI(ID_ContextCheckOut,               MainFrame::OnUpdateContextCheckOut)
    EVT_UPDATE_UI(ID_ContextCheckIn,                MainFrame::OnUpdateContextCheckIn)
    EVT_UPDATE_UI(ID_ContextUndoCheckOut,           MainFrame::OnUpdateContextCheckIn)
    EVT_UPDATE_UI(ID_ContextShowHistory,            MainFrame::OnUpdateContextShowHistory)
    EVT_UPDATE_UI(ID_ContextDiff,                   MainFrame::OnUpdateContextCheckIn)
    
    EVT_MENU(wxID_ANY,                              MainFrame::OnMenu)

    EVT_DEBUG(                                      MainFrame::OnDebugEvent)
    EVT_FILE(                                       MainFrame::OnFileEvent)

    // Notebook related events.
    EVT_AUINOTEBOOK_PAGE_CLOSE(ID_Notebook,         MainFrame::OnNotebookPageClose)
    EVT_AUINOTEBOOK_PAGE_CHANGED(ID_Notebook,       MainFrame::OnNotebookPageChanged)
    EVT_AUINOTEBOOK_TAB_MIDDLE_UP(ID_Notebook,      MainFrame::OnNotebookTabMiddleUp)
    EVT_AUINOTEBOOK_TAB_RIGHT_UP(ID_Notebook,       MainFrame::OnNotebookTabRightUp)
    EVT_AUINOTEBOOK_TAB_RIGHT_DOWN(ID_Notebook,     MainFrame::OnNotebookTabRightDown)


    EVT_LIST_ITEM_ACTIVATED(ID_CallStack,           MainFrame::OnCallStackDoubleClick)
    EVT_LIST_ITEM_ACTIVATED(ID_VmList,              MainFrame::OnVmListDoubleClick)
    
    EVT_SCI_DWELLSTART(wxID_ANY,                    MainFrame::OnCodeEditDwellStart)
    EVT_SCI_DWELLEND(wxID_ANY,                      MainFrame::OnCodeEditDwellEnd)
    EVT_SCI_SAVEPOINTLEFT(wxID_ANY,                 MainFrame::OnCodeEditSavePointLeft)
    EVT_SCI_SAVEPOINTREACHED(wxID_ANY,              MainFrame::OnCodeEditSavePointReached)
    EVT_SCI_UPDATEUI(wxID_ANY,                      MainFrame::OnCodeEditUpdateUI)
    EVT_SCI_MARGINCLICK(wxID_ANY,                   MainFrame::OnCodeEditMarginClick)
    EVT_SCI_ROMODIFYATTEMPT(wxID_ANY,               MainFrame::OnCodeEditReadOnlyModifyAttempt)
    EVT_SCI_MODIFIED(wxID_ANY,                      MainFrame::OnCodeEditModified)
    
    EVT_IDLE(                                       MainFrame::OnIdle)
    EVT_TIMER(wxID_ANY,                             MainFrame::OnTimer)
    EVT_END_PROCESS(wxID_ANY,                       MainFrame::OnProcessTerminate)

    EVT_OUTPUT_KEY_DOWN(ID_Output,                  MainFrame::OnOutputKeyDown)
    
    EVT_FIND_CLOSE(wxID_ANY,                        MainFrame::OnFindClose)
    EVT_FIND(wxID_ANY,                              MainFrame::OnFindFind)
    EVT_FIND_REPLACE(wxID_ANY,                      MainFrame::OnFindReplace)
    EVT_FIND_REPLACE_ALL(wxID_ANY,                  MainFrame::OnFindReplaceAll)
    EVT_FIND_NEXT(wxID_ANY,                         MainFrame::OnFindNext)
    
    EVT_ACTIVATE(                                   MainFrame::OnActivate)
    EVT_THREAD(                                     MainFrame::OnThreadExit)
    
    EVT_COMMAND(wxID_ANY, wxEVT_SHOW_HELP_EVENT,    MainFrame::OnShowHelp)
    EVT_COMMAND(wxID_ANY, wxEVT_UPDATE_INFO_EVENT,  MainFrame::OnUpdateInfo)

    EVT_SYMBOL_PARSER(                              MainFrame::OnSymbolsParsed)

END_EVENT_TABLE()

const wxString MainFrame::s_scriptExtensions        = _("Lua files (*.lua)|*.lua|All files (*.*)|*.*");
const wxString MainFrame::s_applicationName         = _("Decoda");

const wxString MainFrame::s_modeName[]              = { wxT("editing"), wxT("debugging") };
const char* MainFrame::s_updateUrl                  = "http://www.unknownworlds.com/download.php?file=decoda_builds.xml";

wxString GetExecutablePath()
{

    static bool found = false;
    static wxString path;

    if (found)
        return path;
    else
    {
#ifdef __WXMSW__

        char buf[512];
        *buf = '\0';
        GetModuleFileName(NULL, buf, 511);
        path = wxPathOnly(buf);

#elif defined(__WXMAC__)

        ProcessInfoRec processinfo;
        ProcessSerialNumber procno ;
        FSSpec fsSpec;

        procno.highLongOfPSN = NULL ;
        procno.lowLongOfPSN = kCurrentProcess ;
        processinfo.processInfoLength = sizeof(ProcessInfoRec);
        processinfo.processName = NULL;
        processinfo.processAppSpec = &fsSpec;

        GetProcessInformation( &procno , &processinfo ) ;
        path = wxMacFSSpec2MacFilename(&fsSpec);
#else
        wxString argv0 = wxTheApp->argv[0];

        if (wxIsAbsolutePath(argv0))
            path = argv0;
        else
        {
            wxPathList pathlist;
            pathlist.AddEnvList(wxT("PATH"));
            path = pathlist.FindAbsoluteValidPath(argv0);
        }

        wxFileName filename(path);
        filename.Normalize();
        path = filename.GetPath();
#endif
        found = true;
        return path;
    }
}

MainFrame::MainFrame(const wxString& title, int openFilesMessage, const wxPoint& pos, const wxSize& size)
    : wxFrame(NULL, -1, title, pos, size), m_timerIdleWakeUp(this)
{

    m_project = NULL;

    Center();
    UpdateStartUpPlacement();

    m_lastFindPosition  = -1;
    m_findStart         = -1;

    m_openFilesMessage = openFilesMessage;

    wxIcon appIcon(wxICON(decodaIcon));
    SetIcon(appIcon);

    SetDropTarget(new MainFrameDropTarget(this));

    m_attachToHost = false;

    // Notify wxAUI which frame to use
    m_mgr.SetManagedWindow(this);
                                    
    // Create the output window.
    m_output = new OutputWindow(this, ID_Output);
    
    // Create the call stack window.
    m_callStack = new ListWindow(this, ID_CallStack);

    // Create the virtual machine list window.
    m_vmList = new ListWindow(this, ID_VmList);

    // Create the watch window.
    m_watch = new WatchWindow(this, ID_Watch);

    // Create the project explorer window.
    m_projectExplorer = new ProjectExplorerWindow(this, ID_ProjectExplorer);

    m_breakpointsWindow = new BreakpointsWindow(this, ID_Breakpoints);

    m_searchWindow = new SearchWindow(this, ID_Search);

    // Create the notebook that holds all of the open scripts.
    m_notebook = new wxAuiNotebook(this, ID_Notebook, wxDefaultPosition, wxDefaultSize, wxAUI_NB_WINDOWLIST_BUTTON | wxAUI_NB_DEFAULT_STYLE);
        
    // Add the panes to the manager
    
    m_mgr.AddPane(m_output, wxBOTTOM, wxT("Output"));
    m_mgr.GetPane(m_output).Name("output");

    m_mgr.AddPane(m_callStack, wxBOTTOM, wxT("Call Stack"));
    m_mgr.GetPane(m_callStack).Name("callstack");

    m_mgr.AddPane(m_vmList, wxBOTTOM, wxT("Virtual Machines"));
    m_mgr.GetPane(m_vmList).Name("vmlist");

    m_mgr.AddPane(m_watch,wxBOTTOM, wxT("Watch"));
    m_mgr.GetPane(m_watch).Name("watch");

    m_mgr.AddPane(m_projectExplorer, wxLEFT, wxT("Project Explorer"));
    m_mgr.GetPane(m_projectExplorer).Name("projectexplorer");

    m_mgr.AddPane(m_breakpointsWindow, wxBOTTOM, wxT("Breakpoints"));
    m_mgr.GetPane(m_breakpointsWindow).Name("breakpoints");

    m_mgr.AddPane(m_searchWindow, wxBOTTOM, wxT("Search Results"));
    m_mgr.GetPane(m_searchWindow).Name("search");

    m_mgr.AddPane(m_notebook, wxCENTER);
    m_mgr.GetPane(m_notebook).Name("notebook");

    m_statusBar = CreateStatusBar();

    int statusWidth[] = { -1, 100, 100 };
    m_statusBar->SetFieldsCount(3, statusWidth);

    m_mode = Mode_Editing;

    // Save the default configuration for debugging.
    m_modeLayout[Mode_Debugging] = m_mgr.SavePerspective();
    
    // Save the default configuration for editing.
    m_mgr.GetPane(m_watch).Show(false);
    m_mgr.GetPane(m_callStack).Show(false);
    m_mgr.GetPane(m_vmList).Show(false);
    m_modeLayout[Mode_Editing] = m_mgr.SavePerspective();

    // Since the options contain the layout, load them after we've setup the panes.
    LoadOptions();
    
    // Tell the manager to "commit" all the changes just made
    m_mgr.Update();

    m_vm = 0;
    m_stackLevel = 0;
    DebugFrontend::Get().SetEventHandler(this);
        
    m_currentScriptIndex    = -1;
    m_currentLine           = -1;

    m_breakScriptIndex      = -1;
    m_breakLine             = -1;

    m_findDialog            = NULL;

    // Create the context menu for files in the project explorer.
    m_contextMenu = new wxMenu;
    m_contextMenu->Append(ID_ContextCheckOut,       _("Check &Out"));
    m_contextMenu->Append(ID_ContextCheckIn,        _("Check &In"));
    m_contextMenu->Append(ID_ContextUndoCheckOut,   _("&Undo Check Out"));
    m_contextMenu->AppendSeparator();
    m_contextMenu->Append(ID_ContextDiff,           _("&Show Changes..."));
    m_contextMenu->Append(ID_ContextShowHistory,    _("Show &History..."));
    m_contextMenu->AppendSeparator();
    m_contextMenu->Append(ID_ContextRemove,         _("&Remove"));

    m_projectExplorer->SetFileContextMenu(m_contextMenu);

    m_notebookTabMenu = new wxMenu;
    
    m_notebookTabMenu->Append(ID_NotebookTabSave,           _("&Save"));
    m_notebookTabMenu->Append(ID_NotebookTabClose,          _("&Close"));
    m_notebookTabMenu->Append(ID_NotebookTabShowFile,       _("Show in &Folder"));
    m_notebookTabMenu->AppendSeparator();
    m_notebookTabMenu->Append(ID_NotebookTabCheckOut,       _("Check &Out"));
    m_notebookTabMenu->Append(ID_NotebookTabCheckIn,        _("Check &In"));
    m_notebookTabMenu->Append(ID_NotebookTabUndoCheckOut,   _("&Undo Check Out"));
    m_notebookTabMenu->AppendSeparator();
    m_notebookTabMenu->Append(ID_NotebookTabDiff,           _("Sho&w Changes..."));
    m_notebookTabMenu->Append(ID_NotebookTabShowHistory,    _("Show &History..."));

    m_fileStatusThread[0] = NULL;
    m_fileStatusThread[1] = NULL;

    // Make down the default option for find operaitons, since that's a lot
    // more common than up.
    m_findData.SetFlags(wxFR_DOWN);

    // Set the file change watcher to report events to us.
    m_fileChangeWatcher.SetEventHandler(this);

    // Setup the help.
    
    wxHelpControllerHelpProvider* provider = new wxHelpControllerHelpProvider;
    wxHelpProvider::Set(provider);

    provider->SetHelpController(&m_helpController);

    wxString path = GetExecutablePath();
    m_helpController.Initialize(path + _T("\\Decoda.chm"));

    m_symbolParser = new SymbolParser;
    m_symbolParser->SetEventHandler(this);
    m_waitForFinalSymbolParse = false;

    // Creating a new project will clear this out, so save it.
    wxString lastProjectLoaded = m_lastProjectLoaded;

    // Start off with an empty project.
    NewProject();
    UpdateCaption();

    m_lastProjectLoaded = lastProjectLoaded;
    m_tabOrderIndex = -1;
}

MainFrame::~MainFrame()
{

    m_fileChangeWatcher.Shutdown();

    if (m_fileStatusThread[0] != NULL)
    {
        m_fileStatusThread[0]->Wait();
    }

    delete m_fileStatusThread[0];
    delete m_fileStatusThread[1];

    delete m_contextMenu;
    m_contextMenu = NULL;

    delete m_notebookTabMenu;
    m_notebookTabMenu = NULL;
    
    // Remove the drop target, since otherwise we get a crash.
    SetDropTarget(NULL);

    ClearVector(m_openFiles);

    delete m_findDialog;
    m_findDialog = NULL;

    SaveOptions();

    // Since the explorer references files in the project, we need to clear out
    // its references before we delete the project.
    m_projectExplorer->SetProject(NULL);
    m_symbolParser->SetProject(NULL);
    m_breakpointsWindow->SetProject(NULL);

    delete m_project;
    m_project = NULL;

    delete m_symbolParser;
    m_symbolParser = NULL;

    // deinitialize the frame manager
    m_mgr.UnInit();

    ClearVector(m_tools);

    DebugFrontend::Destroy();

}

void MainFrame::OnClose(wxCloseEvent& event)
{

    if ((CloseAllFiles() && SaveProjectIfNeeded()) || !event.CanVeto())
    {

        // Detach the debugger if it's currently running. This will allow the application
        // to continue to execute.
        if (DebugFrontend::Get().GetState() != DebugFrontend::State_Inactive)
        {
            DebugFrontend::Get().Stop(false);
        }

        Destroy();

    }
    
}

void MainFrame::InitializeMenu()
{

    // File menu.

    wxMenu* menuRecentFiles = new wxMenu;
    m_fileHistory.UseMenu(menuRecentFiles);
    m_fileHistory.SetBaseId(ID_FirstRecentFile);

    wxMenu* menuFile = new wxMenu;
    menuFile->Append(ID_FileNew,                        _("&New..."));
    menuFile->Append(ID_FileOpen,                       _("&Open..."));
    menuFile->Append(ID_FileSave,                       _("&Save"),     _("Saves the current document"));
    menuFile->Append(ID_FileSaveAs,                     _("Save &As..."), _("Save the current document with a new name"));
    menuFile->AppendSeparator();
    menuFile->AppendSubMenu(menuRecentFiles,            _("&Recent Files..."));
    menuFile->AppendSeparator();
    menuFile->Append(ID_FileExit,                       _("E&xit"),     _("Exits the application"));

    // Edit menu.

    wxMenu* menuEdit = new wxMenu;
    menuEdit->Append(ID_EditUndo,                       _("&Undo"));
    menuEdit->Append(ID_EditRedo,                       _("&Redo"));
    menuEdit->AppendSeparator();
    menuEdit->Append(ID_EditCut,                        _("Cu&t"));
    menuEdit->Append(ID_EditCopy,                       _("&Copy"));
    menuEdit->Append(ID_EditPaste,                      _("&Paste"));
    menuEdit->Append(ID_EditDelete,                     _("&Delete"));
    menuEdit->Append(ID_EditSelectAll,                  _("Select A&ll"));
    menuEdit->AppendSeparator();
    menuEdit->Append(ID_EditFind,                       _("&Find..."));
    menuEdit->Append(ID_EditReplace,                    _("&Replace..."));
    menuEdit->Append(ID_EditFindNext,                   _("Find &Next"));
    menuEdit->Append(ID_EditFindPrevious,               _("Find &Previous"));
    menuEdit->AppendSeparator();
    menuEdit->Append(ID_EditFindInFiles,                _("F&ind In Files"));
    menuEdit->AppendSeparator();
    menuEdit->Append(ID_EditGotoLine,                   _("G&o To Line..."));
    menuEdit->AppendSeparator();
    menuEdit->Append(ID_EditUntabify,                   _("Unta&bify Selection"), _("Removes all tabs from the selected text"));
    menuEdit->Append(ID_EditComment,                    _("Co&mment Selection"));
    menuEdit->Append(ID_EditUncomment,                  _("&Uncomment Selection"));

    // Project menu.

    wxMenu* menuRecentProjects = new wxMenu;
    m_projectFileHistory.UseMenu(menuRecentProjects);
    m_projectFileHistory.SetBaseId(ID_FirstRecentProjectFile);

    wxMenu* menuProject = new wxMenu;
    menuProject->Append(ID_FileNewProject,              _("&New Project"));
    menuProject->Append(ID_FileOpenProject,             _("Open &Project..."));
    menuProject->Append(ID_FileSaveProject,             _("&Save Project"));
    menuProject->Append(ID_FileSaveProjectAs,           _("Save Project &As..."));
    menuProject->AppendSeparator();
    menuProject->Append(ID_ProjectAddNewFile,           _("Add Ne&w File..."));
    menuProject->Append(ID_ProjectAddExistingFile,      _("Add Existin&g File..."));
    menuProject->AppendSeparator();
    menuProject->AppendSubMenu(menuRecentProjects,      _("&Recent Files..."));
    menuProject->AppendSeparator();
    menuProject->Append(ID_ProjectSettings,             _("&Settings..."));

    // Source control menu.

    wxMenu* menuSourceControl = new wxMenu;
    menuSourceControl->Append(ID_SourceControlCheckIn,      _("Check &In"),             _("Checks in the currently active document"));
    menuSourceControl->Append(ID_SourceControlCheckOut,     _("Check &Out"),            _("Checks out the currently active document"));
    menuSourceControl->Append(ID_SourceControlUndoCheckOut, _("&Undo Check Out"),       _("Undoes the check out of the currently active document"));
    menuSourceControl->AppendSeparator();
    menuSourceControl->Append(ID_SourceControlRefresh,      _("&Refresh All Files"),    _("Refreshes the checked out status of the files in the project"));

    // Debug menu.

    wxMenu* menuDebug = new wxMenu;
    menuDebug->Append(ID_DebugStart,                    _("&Start Debugging"));
    menuDebug->Append(ID_DebugStartWithoutDebugging,    _("Start Wit&hout Debugging"));
    menuDebug->Append(ID_DebugStop,                     _("Stop D&ebugging"));
    menuDebug->Append(ID_DebugDetach,                   _("De&tach"));
    menuDebug->Append(ID_DebugProcess,                  _("&Processes..."),             _("Attaches the debugger to a process that is already running"));
    menuDebug->AppendSeparator();
    menuDebug->AppendCheckItem(ID_DebugAttachToHost,    _("&Attach System Debugger"),   _("Attaches the system default debugger to the host application on startup"));
    menuDebug->AppendSeparator();
    menuDebug->Append(ID_DebugBreak,                    _("&Break"));
    menuDebug->AppendSeparator();
    menuDebug->Append(ID_DebugStepInto,                 _("Step &Into"));
    menuDebug->Append(ID_DebugStepOver,                 _("Step &Over"));
    menuDebug->Append(ID_DebugQuickWatch,               _("&Quick Watch..."));
    menuDebug->AppendSeparator();
    menuDebug->Append(ID_DebugToggleBreakpoint,         _("To&ggle Breakpoint"),        _("Toggles a breakpoint on the current line"));
    menuDebug->Append(ID_DebugDeleteAllBreakpoints,     _("&Delete All Breakpoints"),   _("Removes all breakpoints from the project"));

    // Tools menu.

    m_menuTools = new wxMenu;
    UpdateToolsMenu();

    // Window menu.

    wxMenu* menuWindow = new wxMenu;
    menuWindow->Append(ID_WindowNextDocument,           _("&Next Document"));
    menuWindow->Append(ID_WindowPreviousDocument,       _("&Previous Document"));
    menuWindow->AppendSeparator();
    menuWindow->Append(ID_WindowClose,                  _("&Close Document"));
    menuWindow->Append(ID_WindowCloseAllDocuments,      _("C&lose All Documents"));
    menuWindow->AppendSeparator();
    menuWindow->Append(ID_MiscOpenInFilter,             _("Open In &Filter"), _("Sets the keyboard focus to the project explorer filter search box"));
    menuWindow->AppendSeparator();
    menuWindow->Append(ID_WindowProjectExplorer,        _("Project &Explorer"));
    menuWindow->Append(ID_WindowCallStack,              _("Call &Stack"));
    menuWindow->Append(ID_WindowOutput,                 _("&Output"));
    menuWindow->Append(ID_WindowSearch,                 _("&Search Results"));
    menuWindow->Append(ID_WindowWatch,                  _("&Watch"));
    menuWindow->Append(ID_WindowVirtualMachines,        _("&Virtual Machines"));
    menuWindow->Append(ID_WindowBreakpoints,            _("&Breakpoints"));
        
    // Help menu.

    wxMenu* menuHelp = new wxMenu;
    menuHelp->Append(ID_HelpContents,                   _("&Contents"));
    menuHelp->AppendSeparator();

    menuHelp->Append(ID_HelpSupport,                    _("&Support"));
    menuHelp->AppendSeparator();
    menuHelp->Append(ID_HelpAbout,                      _("&About"));

    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append( menuFile,                          _("&File"));
    menuBar->Append( menuEdit,                          _("&Edit"));
    menuBar->Append( menuProject,                       _("&Project"));
    menuBar->Append( menuDebug,                         _("&Debug"));
    menuBar->Append( menuSourceControl,                 _("&SCC"));
    menuBar->Append( m_menuTools,                       _("&Tools"));
    menuBar->Append( menuWindow,                        _("&Window"));
    menuBar->Append( menuHelp,                          _("&Help"));

    SetMenuBar( menuBar );

}

void MainFrame::OnFileNewProject(wxCommandEvent& WXUNUSED(event))
{
    if (SaveProjectIfNeeded())
    {
        NewProject();    
    }
}

void MainFrame::OnFileOpenProject(wxCommandEvent& WXUNUSED(event))
{

    wxString fileName = wxFileSelector("Open Project", "", "", "", "Decoda Project files (*.deproj)|*.deproj|All files (*.*)|*.*", 0, this);

    if (!fileName.empty())
    {
        if (SaveProjectIfNeeded())
        {
            OpenProject(fileName, true);
        }
    }

}

bool MainFrame::OpenProject(const wxString& fileName, bool reportError)
{

    m_output->Clear();
    m_searchWindow->Clear();

    Project* project = new Project;

    if (!project->Load(fileName))
    {
        delete project;
        if (reportError)
        {
            wxMessageBox(_("Error loading project."), s_applicationName, wxOK | wxICON_ERROR, this);
        }
        return false;
    }

    m_projectFileHistory.AddFileToHistory(fileName);

    SetProject(project);

    // Remember that we loaded this project.
    m_lastProjectLoaded = fileName;

    return true;

}

void MainFrame::NewProject()
{
    SetProject(new Project);
}

void MainFrame::SetProject(Project* project)
{

    CloseAllFiles();

    m_output->Clear();
    m_searchWindow->Clear();

    m_lastProjectLoaded.Empty();

    delete m_project;
    m_project = project;
    
    UpdateCaption();

    if (!m_project->GetFileName().IsEmpty())
    {
        m_waitForFinalSymbolParse = true;
        m_statusBar->SetStatusText("Loading Symbols", 0);
    }
    m_projectExplorer->SetProject(m_project);
    m_symbolParser->SetProject(m_project);
    m_breakpointsWindow->SetProject(m_project);

    m_autoCompleteManager.BuildFromProject(project);

    InitializeSourceControl();

}

void MainFrame::OnFileSaveProject(wxCommandEvent& WXUNUSED(event))
{
    SaveProject();
}

void MainFrame::OnFileSaveProjectAs(wxCommandEvent& WXUNUSED(event))
{
    SaveProject(true);
}

void MainFrame::OnFileExit(wxCommandEvent& WXUNUSED(event))
{
    if (SaveProjectIfNeeded())
    {
        Close(TRUE);
    }
}

void MainFrame::OnFileNew(wxCommandEvent& event)
{

    Project::File* file = m_project->AddTemporaryFile("");
    UpdateForNewFile(file);
    OpenProjectFile(file);

}

void MainFrame::OnFileOpen(wxCommandEvent& event)
{

    wxFileDialog dialog(this, "Open file", "", "", s_scriptExtensions, wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE);
    
    if (dialog.ShowModal() == wxID_OK)
    {

        wxArrayString fileNames;
        dialog.GetPaths(fileNames);

        OpenDocuments(fileNames);

    }

}

void MainFrame::OnFileSave(wxCommandEvent& event)
{

    int pageIndex = GetSelectedPage();

    if (pageIndex != -1)
    {
        SaveFile(m_openFiles[pageIndex], false);
    }

}

void MainFrame::OnFileSaveAs(wxCommandEvent& event)
{

    int pageIndex = GetSelectedPage();

    if (pageIndex != -1)
    {
        SaveFile(m_openFiles[pageIndex], true);
    }

}

void MainFrame::OnFileShow (wxCommandEvent& event) {
  int pageIndex = GetSelectedPage();

  if (pageIndex != -1)
  {
    wxFileName fileName = m_openFiles[pageIndex]->file->fileName;
    ShowFileInFolder(fileName);
  }
}

void MainFrame::OnEditUndo(wxCommandEvent& event)
{

    int pageIndex = GetSelectedPage();

    if (pageIndex != -1)
    {
        m_openFiles[pageIndex]->edit->Undo();
    }

}

void MainFrame::OnEditRedo(wxCommandEvent& event)
{

    int pageIndex = GetSelectedPage();

    if (pageIndex != -1)
    {
        m_openFiles[pageIndex]->edit->Redo();
    }

}

void MainFrame::OnEditCut(wxCommandEvent& event)
{

    int pageIndex = GetSelectedPage();

    if (pageIndex != -1)
    {
        m_openFiles[pageIndex]->edit->Cut();
    }

}

void MainFrame::OnEditCopy(wxCommandEvent& event)
{

    int pageIndex = GetSelectedPage();

    if (pageIndex != -1)
    {
        m_openFiles[pageIndex]->edit->Copy();
    }

}

void MainFrame::OnEditPaste(wxCommandEvent& event)
{

    int pageIndex = GetSelectedPage();

    if (pageIndex != -1)
    {
        m_openFiles[pageIndex]->edit->Paste();
    }

}

void MainFrame::OnEditDelete(wxCommandEvent& event)
{

    int pageIndex = GetSelectedPage();

    if (pageIndex != -1)
    {
        m_openFiles[pageIndex]->edit->DeleteBack();
    }

}

void MainFrame::OnEditSelectAll(wxCommandEvent& event)
{

    int pageIndex = GetSelectedPage();

    if (pageIndex != -1)
    {
        m_openFiles[pageIndex]->edit->SelectAll();
    }

}

void MainFrame::OnEditFind(wxCommandEvent& event)
{

    // If we have something selected, populate the find dialog with it.

    wxString findText;

    int pageIndex = GetSelectedPage();

    if (pageIndex != -1)
    {
        findText = m_openFiles[pageIndex]->edit->GetSelectedText();
    }

    if (!findText.IsEmpty() && findText != m_findData.GetFindString())
    {
        
        m_findData.SetFindString(findText);
        
        // If the dialog is already shown, we need to delete it and recreate it
        // for the text to be updated.
        delete m_findDialog;
        m_findDialog = NULL;

    }

    if (m_findDialog == NULL)
    {
        m_findDialog = new wxFindReplaceDialog(this, &m_findData, _T("Find"));
        m_findDialog->Show();
    }

}

void MainFrame::OnEditReplace(wxCommandEvent& event)
{

    // If we have something selected, populate the replace dialog with it.

    wxString findText;

    int pageIndex = GetSelectedPage();

    if (pageIndex != -1)
    {
        findText = m_openFiles[pageIndex]->edit->GetSelectedText();
    }

    if (!findText.IsEmpty() && findText != m_findData.GetFindString())
    {
        
        m_findData.SetFindString(findText);
        
        // If the dialog is already shown, we need to delete it and recreate it
        // for the text to be updated.
        delete m_findDialog;
        m_findDialog = NULL;

    }

    if (m_findDialog == NULL)
    {
        m_findDialog = new wxFindReplaceDialog(this, &m_findData, _T("Replace"), wxFR_REPLACEDIALOG);
        m_findDialog->Show();
    }

}

void MainFrame::OnEditFindNext(wxCommandEvent& event)
{
    FindNext(m_findData.GetFindString(), m_findData.GetFlags());
}

void MainFrame::OnEditFindPrevious(wxCommandEvent& event)
{
    
    // Reverse the search direction specified in the flags.

    int flags = m_findData.GetFlags();

    if (flags & wxFR_DOWN)
    {
        flags &= ~wxFR_DOWN;
    }
    else
    {
        flags |= wxFR_DOWN;
    }
    
    FindNext(m_findData.GetFindString(), flags);

}

void MainFrame::OnEditFindInFiles(wxCommandEvent& event)
{

    FindInFilesDialog dialog(this);

    dialog.AddLookIn("Current Project");

    // Add the directories we've looked in before as options.
    for (unsigned int i = 0; i < m_findDirectoryHistory.GetCount(); ++i)
    {
        dialog.AddLookIn(m_findDirectoryHistory.GetHistoryFile(i));
    }

    // Add the text we've search for before.
    for (unsigned int i = 0; i < m_findTextHistory.GetNumStrings(); ++i)
    {
        dialog.AddText(m_findTextHistory.GetString(i));
    }

    // If we have something selected, populate the find dialog with it.

    int pageIndex = GetSelectedPage();

    if (pageIndex != -1)
    {
        dialog.SetText(m_openFiles[pageIndex]->edit->GetSelectedText());
    }

    if (dialog.ShowModal() == wxID_OK)
    {

        wxString text       = dialog.GetText();
        wxString lookIn     = dialog.GetDirectory();
        wxString fileTypes  = dialog.GetFileTypes();

        // Ignore case on the file types.
        wxString caseFileTypes = fileTypes.Lower();

        wxArrayString fileNames;
        wxString baseDirectory;

        // Multiple file types can be specified separated by ; characters, but we can only
        // search for one type at a time with wxWidgets, so separate them out.

        wxArrayString fileTypesArray;
        unsigned int start = 0;

        while (start < caseFileTypes.Length())
        {

            unsigned int end = caseFileTypes.find(';', start);

            if (end == wxString::npos)
            {
                end = fileTypes.Length();
            }

            wxString fileType = caseFileTypes.Mid(start, end - start);
            fileType.Trim(false);

            if (fileType.Length() > 0)
            {
                fileTypesArray.Add(fileType);
            }

            start = end + 1;
        
        }

        if (lookIn == "Current Project")
        {

            for (unsigned int i = 0; i < m_project->GetNumFiles(); ++i)
            {

                const Project::File* file = m_project->GetFile(i);

                if (!file->temporary)
                {

                    wxString fileName = file->fileName.GetFullPath();
                    wxString caseFileName = fileName.Lower();

                    for (unsigned int j = 0; j < fileTypesArray.Count(); ++j)
                    {
                        if (caseFileName.Matches(fileTypesArray[j]))
                        {
                            fileNames.Add(fileName);
                        }
                    }

                }
            }

            baseDirectory = wxFileName(m_project->GetFileName()).GetPath();

        }
        else
        {

            m_findDirectoryHistory.AddFileToHistory(lookIn);

            int flags = wxDIR_FILES;
            
            if (dialog.GetIncludeSubDirectories())
            {
                // Recurse into sub-directories.
                flags |= wxDIR_DIRS;
            }

            wxFileName directory(lookIn);

            for (unsigned int i = 0; i < fileTypesArray.Count(); ++i)
            {
                wxDir::GetAllFiles(directory.GetFullPath(), &fileNames, fileTypesArray[i], flags);
            }

        }
         
        m_findTextHistory.Add(text);

        m_searchWindow->Clear();
        ShowSearchWindow();

        m_searchWindow->SearchMessage(wxString::Format("Find all \"%s\"", text.ToAscii()));

        FindInFiles(text, fileNames, dialog.GetMatchCase(), dialog.GetMatchWholdWord(), baseDirectory);

    }

}

void MainFrame::OnEditGotoLine(wxCommandEvent& event)
{
    
    int pageIndex = GetSelectedPage();

    if (pageIndex != -1)
    {
        
        CodeEdit* edit = m_openFiles[pageIndex]->edit;

        long currentLine = edit->GetCurrentLine() + 1;
        long numLines    = edit->GetLineCount();

        wxString message;
        message.Printf("Enter a number between 1 and %d", numLines);

        long line = wxGetNumberFromUser(message, "", "Go To Line", currentLine, 1, numLines, this);

        if (line != -1)
        {
            GotoNewLine(edit, line - 1, true);
        }

    }

}

void MainFrame::OnEditUntabify(wxCommandEvent& event)
{

    int pageIndex = GetSelectedPage();

    if (pageIndex != -1)
    {
        m_openFiles[pageIndex]->edit->UntabifySelection();
    }

}

void MainFrame::OnEditComment(wxCommandEvent& event)
{
    
    int pageIndex = GetSelectedPage();

    if (pageIndex != -1)
    {
        m_openFiles[pageIndex]->edit->CommentSelection();
    }

}

void MainFrame::OnEditUncomment(wxCommandEvent& event)
{
    
    int pageIndex = GetSelectedPage();

    if (pageIndex != -1)
    {
        m_openFiles[pageIndex]->edit->UncommentSelection();
    }

}

void MainFrame::OnProjectAddNewFile(wxCommandEvent& WXUNUSED(event))
{

    while (1)
    {

        wxFileName projectFileName(m_project->GetFileName());
        wxString projectPath = projectFileName.GetPathWithSep();

        // If the project hasn't been saved yet and we're using source control,
        // use the path in source control.
        if (projectPath.IsEmpty() && m_sourceControl.GetIsInitialized())
        {
            projectPath = m_sourceControl.GetLocalPath();
        }

        NewFileDialog dialog(this, m_sourceControl.GetIsInitialized());
        dialog.SetPath(projectPath);

        if (dialog.ShowModal() == wxID_OK)
        {

            wxFileName fileName = dialog.GetFileName();
            wxFile file;

            // Check to see if the file already exists.
            if (fileName.FileExists())
            {
                int result = wxMessageBox(_("A file with that name already exists, do you want to overwrite it?"), s_applicationName, wxICON_QUESTION | wxYES_NO | wxCANCEL, this);
                if (result == wxCANCEL)
                {
                    // Just exit the add operation.
                    break;
                }
                else if (result == wxNO)
                {
                    // Try and get another file name.
                    continue;
                }
            }
            
            // Create a new empty file.
            if (fileName.Mkdir(0777, wxPATH_MKDIR_FULL) && file.Open(fileName.GetFullPath(), wxFile::write))
            {

                file.Close();
                Project::File* file = m_project->AddFile(fileName.GetFullPath());
                
                UpdateForNewFile(file);
            
                if (m_sourceControl.GetIsInitialized() && dialog.GetAddToSourceContrl())
                {
                    // Add the file to source control.
                    m_sourceControl.AddFiles(std::string(fileName.GetFullPath()), NULL);
                }

                // Update the status for the new files.
                UpdateProjectFileStatus(file);

                // Open the file in the editor.
                OpenProjectFile(file);
            
            }
            else
            {
                wxMessageBox(_("Error creating file."), s_applicationName, wxOK | wxICON_ERROR, this);
            }

        }

        break;

    }

}

void MainFrame::OnProjectAddExistingFile(wxCommandEvent& WXUNUSED(event))
{
    
    wxFileDialog dialog(this, _("Add existing file"), "", "", s_scriptExtensions, wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE);
    
    if (dialog.ShowModal() == wxID_OK)
    {

        wxArrayString fileNames;
        dialog.GetPaths(fileNames);

        for (unsigned int i = 0; i < fileNames.Count(); ++i)
        {
            Project::File* file = m_project->AddFile(fileNames[i]);
            if (file != NULL)
            {
                
                UpdateForNewFile(file);
                
                // Update the status for the new files.
                UpdateProjectFileStatus(file);            

            }
        }
        
    }

}

void MainFrame::OnProjectSettings(wxCommandEvent& WXUNUSED(event))
{
    ShowProjectSettingsDialog();
}

void MainFrame::OnDebugStart(wxCommandEvent& WXUNUSED(event))
{

    // Check to see if this is a start or a continue based on whether or not
    // the application is running.

    if (DebugFrontend::Get().GetState() == DebugFrontend::State_Inactive)
    {
        StartProcess(true, false);
    }
    else
    {
        // The user wants to continue since we're already running.
        DebugFrontend::Get().Continue(m_vm);
    }

    UpdateForNewState();

}

void MainFrame::OnUpdateDebugStart(wxUpdateUIEvent& event)
{

    DebugFrontend::State state = DebugFrontend::Get().GetState();

    wxString label;

    if (state == DebugFrontend::State_Broken ||
        state == DebugFrontend::State_Running)
    {
        label = "&Continue";
        event.Enable(state == DebugFrontend::State_Broken);
    }
    else
    {
        label = "&Start Debugging";
        event.Enable(true);
    }

    wxMenuItem* item = GetMenuBar()->FindItem(ID_DebugStart);

    if (item != NULL)
    {
        m_keyBinder.GetMenuItemText(item, label);
    }
    
    event.SetText(label);

}

void MainFrame::OnDebugStartWithoutDebugging(wxCommandEvent& WXUNUSED(event))
{
    StartProcess(false, false);
}

void MainFrame::OnDebugStop(wxCommandEvent& WXUNUSED(event))
{
    DebugFrontend::Get().Stop(true);
}

void MainFrame::OnUpdateDebugStop(wxUpdateUIEvent& event)
{
    bool running = (DebugFrontend::Get().GetState() != DebugFrontend::State_Inactive);
    event.Enable(running);
}

void MainFrame::OnDebugAttachToHost(wxCommandEvent& WXUNUSED(event))
{
    
    // If the option is unchecked, check it (and vice versa).

    m_attachToHost = !m_attachToHost;

    wxMenuItem* item = GetMenuBar()->FindItem(ID_DebugAttachToHost);
    item->Check(m_attachToHost);

    if (m_attachToHost)
    {
        DebugFrontend::Get().AttachDebuggerToHost();
    }

}

void MainFrame::OnDebugDetach(wxCommandEvent& WXUNUSED(event))
{
    DebugFrontend::Get().Stop(false);
}

void MainFrame::OnDebugBreak(wxCommandEvent& WXUNUSED(event))
{
    DebugFrontend::Get().Break(m_vm);
}

void MainFrame::OnUpdateDebugBreak(wxUpdateUIEvent& event)
{
    bool running = (DebugFrontend::Get().GetState() == DebugFrontend::State_Running);
    event.Enable(running);
}

void MainFrame::OnDebugStepInto(wxCommandEvent& WXUNUSED(event))
{
    if (DebugFrontend::Get().GetState() == DebugFrontend::State_Inactive)
    {
        StartProcess(true, true);
    }
    else
    {
        DebugFrontend::Get().StepInto(m_vm);
    }
    UpdateForNewState();
}

void MainFrame::OnUpdateDebugStepInto(wxUpdateUIEvent& event)
{
    bool running = (DebugFrontend::Get().GetState() == DebugFrontend::State_Running);
    event.Enable(!running);
}

void MainFrame::OnDebugStepOver(wxCommandEvent& WXUNUSED(event))
{
    DebugFrontend::Get().StepOver(m_vm);
    UpdateForNewState();
}

void MainFrame::OnDebugQuickWatch(wxCommandEvent& WXUNUSED(event))
{

    if (m_vm != 0)
    {

        wxString expression;

        int pageIndex = GetSelectedPage();

        if (pageIndex != -1)
        {
            expression = m_openFiles[pageIndex]->edit->GetSelectedText();
        }

        QuickWatchDialog dialog(this);
        
        dialog.SetContext(m_vm, m_stackLevel);
        dialog.SetExpression(expression);
        dialog.SetWatchWindow(m_watch, this);

        dialog.CenterOnParent();
        dialog.ShowModal();
    
    }

}

void MainFrame::OnDebugProcess(wxCommandEvent& event)
{

    ProcessesDialog dialog(this);

    // Try to select our project's EXE if it's running.
    dialog.SelectProcessByModule(m_project->GetCommandLine());

    if (dialog.ShowModal() == wxID_OK)
    {

        m_output->Clear();

        unsigned int id = dialog.GetProcessId();
        
        if (DebugFrontend::Get().Attach(id, m_project->GetSymbolsDirectory()))
        {
            SetMode(Mode_Debugging);
            m_output->OutputMessage("Debugging session started");
            if (m_attachToHost)
            {
                DebugFrontend::Get().AttachDebuggerToHost();
            }
        }

        UpdateForNewState();

    }

}

void MainFrame::OnDebugToggleBreakpoint(wxCommandEvent& event)
{

    int pageIndex = GetSelectedPage();

    if (pageIndex != -1)
    {

        OpenFile* openFile = m_openFiles[pageIndex];
        unsigned int newLine = openFile->edit->GetCurrentLine();
        
        ToggleBreakpoint(openFile->file, newLine);

    }

}

void MainFrame::OnDebugDeleteAllBreakpoints(wxCommandEvent& event)
{
    DeleteAllBreakpoints();
}

void MainFrame::OnWindowProjectExplorer(wxCommandEvent& WXUNUSED(event))
{
    m_mgr.GetPane(m_projectExplorer).Show();
    m_mgr.Update();
}

void MainFrame::OnWindowCallStack(wxCommandEvent& WXUNUSED(event))
{
    m_mgr.GetPane(m_callStack).Show();
    m_mgr.Update();
}

void MainFrame::OnWindowOutput(wxCommandEvent& WXUNUSED(event))
{
    ShowOutputWindow();
}

void MainFrame::OnWindowSearch(wxCommandEvent& WXUNUSED(event))
{
    ShowSearchWindow();
}

void MainFrame::OnWindowWatch(wxCommandEvent& WXUNUSED(event))
{
    ShowWatchWindow();
}

void MainFrame::OnWindowVirtualMachines(wxCommandEvent& WXUNUSED(event))
{
    m_mgr.GetPane(m_vmList).Show();
    m_mgr.Update();
}

void MainFrame::OnWindowBreakpoints(wxCommandEvent& WXUNUSED(event))
{
    m_mgr.GetPane(m_breakpointsWindow).Show();
    m_mgr.Update();
}

void MainFrame::OnWindowNextDocument(wxCommandEvent& event)
{

    int pageIndex = m_notebook->GetSelection();
    int numPages  = m_notebook->GetPageCount();

    if (pageIndex != -1 && numPages > 0)
    {
        if (m_editorSettings.GetMostRecentlyUsedTabSwitching() && !m_tabOrder.empty())
        {
            if (m_tabOrderIndex == -1)
            {
                m_tabOrderIndex = 0;
            }
            m_tabOrderIndex = (m_tabOrderIndex + 1) % m_tabOrder.size();            
            pageIndex = m_tabOrder[m_tabOrderIndex];            
        }
        else
        {
            pageIndex = (pageIndex + 1) % numPages;
        }
        m_notebook->SetSelection(pageIndex);
    }

}

void MainFrame::OnWindowPreviousDocument(wxCommandEvent& event)
{

    int pageIndex = m_notebook->GetSelection();
    int numPages  = m_notebook->GetPageCount();

    if (pageIndex != -1 && numPages > 0)
    {
        if (m_editorSettings.GetMostRecentlyUsedTabSwitching() && !m_tabOrder.empty())
        {
            if (m_tabOrderIndex == -1)
            {
                m_tabOrderIndex = 0;
            }
            m_tabOrderIndex = (m_tabOrderIndex - 1) % m_tabOrder.size();
            pageIndex = m_tabOrder[m_tabOrderIndex];            
        }
        else
        {
            pageIndex = (pageIndex + numPages - 1) % numPages;
        }
        m_notebook->SetSelection(pageIndex);
    }

}

void MainFrame::OnWindowClose(wxCommandEvent& WXUNUSED(event))
{

    if (m_notebook->GetPageCount() > 0)
    {

        int pageIndex = GetSelectedPage();

        if (pageIndex != -1)
        {
            CloseDocument(pageIndex);
        }

    }

}

void MainFrame::OnWindowCloseAllDocuments(wxCommandEvent& WXUNUSED(event))
{
    CloseAllFiles();
}

void MainFrame::OnHelpContents(wxCommandEvent& WXUNUSED(event))
{
    m_helpController.DisplayContents();
}

void MainFrame::OnHelpCheckForUpdate(wxCommandEvent& event)
{
    m_updater.CheckForUpdates(s_updateUrl, MainApp::s_buildNumber, GetHandle());
    HandleUpdate();
}

void MainFrame::OnHelpAbout(wxCommandEvent& WXUNUSED(event))
{
    AboutDialog dialog(this);
    dialog.ShowModal();
}

void MainFrame::OnHelpSupport(wxCommandEvent& event)
{
    m_helpController.DisplaySection(wxT("contact"));
}

void MainFrame::OnMiscOpenInFilter(wxCommandEvent& event)
{
    m_projectExplorer->SetFocusToFilter();
}

void MainFrame::OnMenu(wxCommandEvent& event)
{
    
    if (event.GetId() >= ID_FirstExternalTool && event.GetId() < ID_FirstExternalTool + static_cast<int>(m_tools.size()))
    {

        // Save all of the files, since the tool may access them. This is how
        // MSVC behaves with external tools and is also similar to how it
        // saves before compiling.
        SaveAllFiles();
        
        ExternalTool* tool = m_tools[event.GetId() - ID_FirstExternalTool];
        RunTool(tool);

    }
    else if (event.GetId() >= ID_FirstRecentFile && event.GetId() < ID_FirstRecentFile + static_cast<int>(m_fileHistory.GetCount()))
    {
        int index = event.GetId() - ID_FirstRecentFile;
        OpenDocument(m_fileHistory.GetHistoryFile(index));
    }
    else if (event.GetId() >= ID_FirstRecentProjectFile && event.GetId() < ID_FirstRecentProjectFile + static_cast<int>(m_projectFileHistory.GetCount()))
    {
        if (SaveProjectIfNeeded())
        {
            int index = event.GetId() - ID_FirstRecentProjectFile;
            OpenProject(m_projectFileHistory.GetHistoryFile(index), true);
        }
    }

}


void MainFrame::UpdateScriptLineMappingFromFile(const Project::File* file, DebugFrontend::Script* script)
{

    if (file->fileName.FileExists())
    {

        // Read the file from disk.
        wxFile diskFile(file->fileName.GetFullPath());

        if (diskFile.IsOpened())
        {
        
            unsigned int diskFileSize = file->fileName.GetSize().GetLo();
            char* diskFileSource = new char[diskFileSize + 1];

            diskFileSize = diskFile.Read(diskFileSource, diskFileSize);
            diskFileSource[diskFileSize] = 0; 
            
            script->lineMapper.Update(script->source, diskFileSource);

            delete [] diskFileSource;
            diskFileSource = NULL;

        }
    }

}

void MainFrame::SetMostRecentlyUsedPage(int pageIndex)
{

    if (m_tabOrderIndex == -1 && pageIndex != -1)
    {
        int nextPageIndex = pageIndex;
        for (std::vector<int>::iterator i = m_tabOrder.begin(); i != m_tabOrder.end(); ++i)
        {
            int tempPageIndex = *i;
            *i = nextPageIndex;

            if (tempPageIndex == pageIndex)
            {
                nextPageIndex = -1;
                break;
            }
            else
            {
                nextPageIndex = tempPageIndex;
            }
        }
        if (nextPageIndex != -1)
        {
            m_tabOrder.push_back(nextPageIndex);
        }
    }

}

void MainFrame::RemovePageFromTabOrder(int pageIndex)
{
    int nextPageIndex = -1;
    std::vector<int>::iterator copyFrom = m_tabOrder.begin();
    for (std::vector<int>::iterator copyTo = m_tabOrder.begin(); copyTo != m_tabOrder.end(); ++copyFrom)
    {
        if (copyFrom == m_tabOrder.end())
        {
            assert(copyTo + 1 == m_tabOrder.end()); // At most only one item can be removed
            m_tabOrder.pop_back();
            break;
        }
        else if (*copyFrom != pageIndex)
        {
            if (*copyFrom > pageIndex)
            {
                *copyTo = *copyFrom - 1;
            }
            else
            {
                *copyTo = *copyFrom;
            }

            ++copyTo;
        }
    }

}

void MainFrame::OnDebugEvent(wxDebugEvent& event)
{

    char vmText[256];
    sprintf(vmText, "0x%08x: ", event.GetVm());

    switch (event.GetEventId())
    {
    case EventId_LoadScript:
        {

            // Sync up the breakpoints for this file.

            unsigned int scriptIndex = event.GetScriptIndex();
            Project::File* file = m_project->GetFileForScript(scriptIndex);

            if (file == NULL)
            {
                // Check to see if one of the existing files' contents match this script.
                DebugFrontend::Script* script = DebugFrontend::Get().GetScript(scriptIndex);
                file = GetFileMatchingSource( wxFileName(DebugFrontend::Get().GetScript(scriptIndex)->name), script->source );
            
                if (file != NULL)
                {
                    // Map lines in case the loaded script is different than what we have on disk.
                    UpdateScriptLineMappingFromFile(file, script);
                }

            }

            if (file == NULL)
            {
                // Add the file to the project as a temporary file so that it's
                // easy for the user to add break points.
                file = m_project->AddTemporaryFile(scriptIndex);
                file->type = "Lua";
                UpdateForNewFile(file);
            }
            else
            {
                // Check that we haven't already assigned this guy an index. If
                // we have, overwriting it will cause the previous index to no
                // longer exist in our project.
                assert(file->scriptIndex == -1);
            }

            if (file != NULL)
            {

                // The way this mechanism works, the front end sends all of the breakpoints
                // to the backend, and then the backend sends back commands to enable breakpoints
                // on the valid lines. So, we make a temporary copy of the breakpoints and clear
                // out our stored breakpoints before proceeding.

                std::vector<unsigned int> breakpoints = file->breakpoints;
                RemoveAllLocalBreakpoints(file);

                DebugFrontend::Script* script = DebugFrontend::Get().GetScript(scriptIndex);
                
                file->scriptIndex = scriptIndex;
                for (unsigned int i = 0; i < breakpoints.size(); ++i)
                {
                    unsigned int newLine = breakpoints[i];
                    unsigned int oldLine = script->lineMapper.GetOldLine(newLine);
                    // If a line is changed, the breakpoint will be removed.
                    // Note, since we removed all breakpoints from the file already, if we
                    // don't add a breakpoint back it will be automatically deleted.
                    if (oldLine != LineMapper::s_invalidLine)
                    {
                        DebugFrontend::Get().ToggleBreakpoint(event.GetVm(), scriptIndex, oldLine);
                    }
                }

            }

            // Tell the backend we're done processing this script for loading.
            DebugFrontend::Get().DoneLoadingScript(event.GetVm());

        }
        break;
    case EventId_CreateVM:
        m_output->OutputMessage(wxString::Format("%sVM created", vmText));
        AddVmToList(event.GetVm());
        break;
    case EventId_DestroyVM:
        m_output->OutputMessage(wxString::Format("%sVM destroyed", vmText));
        RemoveVmFromList(event.GetVm());
        break;
    case EventId_Break:
        OnBreak(event);
        break;
    case EventId_SetBreakpoint:
        {
            
            unsigned int scriptIndex = event.GetScriptIndex();

            Project::File* file = m_project->GetFileForScript(scriptIndex);
            unsigned int newLine = OldToNewLine(file, event.GetLine());

            OpenFile* openFile = GetFileForScript(scriptIndex);

            if (openFile != NULL)
            {
                UpdateFileBreakpoint(openFile, newLine, event.GetEnabled());
            }

            m_project->SetBreakpoint(scriptIndex, newLine, event.GetEnabled());
            m_breakpointsWindow->UpdateBreakpoints(file);

        }
        break;
    case EventId_Exception:
        {

            // Add the exception to the output window.
            m_output->OutputError(event.GetMessage());

            // Check if we're ignoring this exception.

            ExceptionDialog dialog(this, event.GetMessage(), true);
            int result = dialog.ShowModal();

            if (result == ExceptionDialog::ID_Ignore)
            {
                // Resume the backend.
                DebugFrontend::Get().Continue(m_vm);
                UpdateForNewState();
            }
            else if (result == ExceptionDialog::ID_IgnoreAlways)
            {
                DebugFrontend::Get().IgnoreException(event.GetMessage().ToAscii());
                DebugFrontend::Get().Continue(m_vm);
                UpdateForNewState();
            }
            
        }
        break;
    case EventId_LoadError:
        OnLoadError(event);
        break;
    case EventId_SessionEnd:
        OnSessionEnd(event);
        break;
    case EventId_Message:
        OnMessage(event);
        break;

    case EventId_NameVM:
        SetVmName(event.GetVm(), event.GetMessage());
        break;

    }

}

void MainFrame::OnFileEvent(FileEvent& event)
{
    UpdateDocumentReadOnlyStatus();
}

void MainFrame::OnToolsSettings(wxCommandEvent& event)
{

    SettingsDialog dialog(this);

    KeyBinderDialog* keyBinder = dialog.GetKeyBinderDialog();

    for (unsigned int i = 0; i < m_keyBinder.GetNumCommands(); ++i)
    {
        keyBinder->AddCommand(m_keyBinder.GetCommand(i));
    }

    FontColorSettingsPanel* fontColorSettings = dialog.GetFontColorSettingsPanel();
    fontColorSettings->SetSettings(m_fontColorSettings);
   
    EditorSettingsPanel* editorSettings = dialog.GetEditorSettingsPanel();
    editorSettings->SetSettings(m_editorSettings);

    SystemSettingsPanel* systemSettings = dialog.GetSystemSettingsPanel();
    systemSettings->SetSettings(m_systemSettings);

    systemSettings->AddFileType(".deproj", 1);
    systemSettings->AddFileType(".lua", 2);

    if (dialog.ShowModal() == wxID_OK)
    {

        systemSettings->ApplyFileAssociations();

        m_keyBinder.ClearCommands();

        for (unsigned int i = 0; i < keyBinder->GetNumCommands(); ++i)
        {
            m_keyBinder.AddCommand(keyBinder->GetCommand(i));
        }

        m_keyBinder.UpdateWindow(this);
        m_keyBinder.UpdateMenuBar(GetMenuBar());

        m_fontColorSettings = fontColorSettings->GetSettings();
        m_editorSettings    = editorSettings->GetSettings();
        m_systemSettings    = systemSettings->GetSettings();

        UpdateEditorOptions();
        
    }

}


void MainFrame::UpdateFileBreakpoint(OpenFile* file, unsigned int newLine, bool set)
{

    if (file != NULL)
    {
        if (set)
        {
            file->edit->MarkerAdd(newLine, CodeEdit::Marker_Breakpoint);
        }
        else
        {
            file->edit->MarkerDelete(newLine, CodeEdit::Marker_Breakpoint);
        }
    }

}

void MainFrame::OnToolsExternalTools(wxCommandEvent& event)
{

    // Save off the hot keys for the tools so we can restore them. The tools
    // are removed and will be readded later.

    for (unsigned int i = 0; i < m_tools.size(); ++i)
    {
        
        int id = ID_FirstExternalTool + i;

        KeyBinder::Command* command = m_keyBinder.GetCommandForId(id);
        assert(command != NULL);

        m_tools[i]->SetKeys(command->keys);
        m_keyBinder.RemoveCommand(id);
       
    }

    ExternalToolsDialog dialog(this);

    dialog.SetTools(&m_tools);
    dialog.CenterOnParent();
    dialog.ShowModal();

    UpdateToolsMenu();

    // Add the tools menu options to the key binder.
    m_keyBinder.AddCommandsFromMenuBar(GetMenuBar());

    // Reassign the hot keys for the tools

    for (unsigned int i = 0; i < m_tools.size(); ++i)
    {

        int id = ID_FirstExternalTool + i;

        KeyBinder::Command* command = m_keyBinder.GetCommandForId(id);
        assert(command != NULL);

        if (command != NULL)
        {
            command->keys = m_tools[i]->GetKeys();
        }

    }

    // Since we've changed the menu, reapply the key bindings.
    m_keyBinder.UpdateWindow(this);
    m_keyBinder.UpdateMenuBar(GetMenuBar());
    
}
    
void MainFrame::OnNotebookPageClose(wxAuiNotebookEvent& event)
{
    if (!PreNotebookPageClose(m_notebook->GetSelection()))
    {
        event.Veto();
    }
}

void MainFrame::OnNotebookPageChanged(wxAuiNotebookEvent& event)
{

    m_lastFindPosition = -1;

    UpdateDocumentReadOnlyStatus();
    
    int pageIndex = GetSelectedPage();

    if (pageIndex != -1)
    {
        OpenFile* file = m_openFiles[pageIndex];
        m_fileChangeWatcher.SetFile(file->file->fileName);
        
        SetMostRecentlyUsedPage(pageIndex);

        if (m_openFiles[pageIndex]->edit != NULL)
        {
            m_openFiles[pageIndex]->edit->SetFocus();
        }
    }    
    
    UpdateStatusBarLineAndColumn();

}

void MainFrame::OnNotebookTabMiddleUp(wxAuiNotebookEvent& event)
{
    int page = event.GetSelection();
    CloseDocument(page);
}

void MainFrame::OnNotebookTabRightDown(wxAuiNotebookEvent& event)
{
    // Select the tab when we click the right mouse. This is MSVC-like and makes
    // the popup menu behavior make more sense.
    int page = event.GetSelection();
    m_notebook->SetSelection(page);
}

void MainFrame::OnNotebookTabRightUp(wxAuiNotebookEvent& event)
{
    // Show the context menu for the tab.
    m_notebook->PopupMenu(m_notebookTabMenu);
}

void MainFrame::OnNotebookTabCheckOut(wxCommandEvent& event)
{

    std::vector<std::string> fileNames;
    GetNotebookTabSelectedFileNames(fileNames);

    m_sourceControl.CheckOut(fileNames, NULL);
    UpdateProjectFileStatus();

}

void MainFrame::OnNotebookTabCheckIn(wxCommandEvent& event)
{

    std::vector<std::string> fileNames;
    GetNotebookTabSelectedFileNames(fileNames);

    m_sourceControl.CheckIn(fileNames, NULL);
    UpdateProjectFileStatus();

}

void MainFrame::OnNotebookTabUndoCheckOut(wxCommandEvent& event)
{

    std::vector<std::string> fileNames;
    GetNotebookTabSelectedFileNames(fileNames);

    m_sourceControl.UndoCheckOut(fileNames);
    UpdateProjectFileStatus();

}

void MainFrame::OnNotebookTabDiff(wxCommandEvent& event)
{

    std::vector<std::string> fileNames;
    GetNotebookTabSelectedFileNames(fileNames);

    if (fileNames.size() > 0)
    {
        m_sourceControl.ShowDiff(fileNames[0].c_str());
    }

}

void MainFrame::OnNotebookTabShowHistory(wxCommandEvent& event)
{

    std::vector<std::string> fileNames;
    GetNotebookTabSelectedFileNames(fileNames);

    m_sourceControl.ShowHistory(fileNames);

}

void MainFrame::OnUpdateNotebookTabCheckOut(wxUpdateUIEvent& event)
{

    int page = GetSelectedPage();
    bool allowed = false;

    if (page != -1)
    {
        allowed = (m_openFiles[page]->file->status == Project::Status_CheckedIn);
    }

    event.Enable(allowed);

}

void MainFrame::OnUpdateNotebookTabCheckIn(wxUpdateUIEvent& event)
{

    int page = GetSelectedPage();
    bool allowed = false;

    if (page != -1)
    {
        allowed = (m_openFiles[page]->file->status == Project::Status_CheckedOut);
    }

    event.Enable(allowed);

}

void MainFrame::OnUpdateNotebookTabShowHistory(wxUpdateUIEvent& event)
{

    int page = GetSelectedPage();
    bool allowed = false;

    if (page != -1)
    {
        allowed = (m_openFiles[page]->file->status != Project::Status_None);
    }

    event.Enable(allowed);

}

void MainFrame::OnCallStackDoubleClick(wxListEvent& event)
{

    int selectedItem = event.GetIndex();

    DebugFrontend& frontend = DebugFrontend::Get();
    const DebugFrontend::StackFrame& stackFrame = frontend.GetStackFrame(selectedItem);

    ShowScriptLine(stackFrame.scriptIndex, stackFrame.line);

    // Change the context in which we're evaluating watches.
    SetContext(m_vm, selectedItem);

}

void MainFrame::OnVmListDoubleClick(wxListEvent& event)
{
    int selectedItem = event.GetIndex();
    SetContext(m_vms[selectedItem], 0);
}

void MainFrame::OnCodeEditDwellStart(wxScintillaEvent& event)
{

    CodeEdit* edit = static_cast<CodeEdit*>(event.GetEventObject());

    // Hide the tooltip, so that even if we don't show a new one we still
    // dismiss the old one. Probably not necessary, but can't hurt.
    edit->HideToolTip();

    if (DebugFrontend::Get().GetState() == DebugFrontend::State_Broken)
    {

        // Check that the mouse cursor is within the code edit window.
        // This seems like something Scintilla should handle, but it
        // doesn't.

        wxPoint mousePosition;
        wxGetMousePosition(&mousePosition.x, &mousePosition.y);

        mousePosition = edit->ScreenToClient(mousePosition);
        wxRect clientRect = wxRect(edit->GetClientSize());

        if (!clientRect.Contains(mousePosition))
        {
            return;
        }

        int position = event.GetPosition();

        wxString expression;
        if (edit->GetHoverText(position, expression))
        {

            // Scintilla uses 0 based line numbers, we use 1 based.
            unsigned int line = edit->LineFromPosition(position) + 1;
            
            std::string result;

            if (DebugFrontend::Get().Evaluate(m_vm, expression, m_stackLevel, result))
            {

                wxStringInputStream stream(result.c_str());
                wxXmlDocument document;

                wxLogNull logNo;
        
                if (document.Load(stream))
                {
                    wxString text;
                    wxString type;
                    
                    text += expression;
                    text += " = ";
                    text += WatchCtrl::GetNodeAsText(document.GetRoot(), type);

                    edit->ShowToolTip(event.GetPosition(), text);
                }
                
            }

        }

    }

}

void MainFrame::OnCodeEditDwellEnd(wxScintillaEvent& event)
{
    CodeEdit* edit = static_cast<CodeEdit*>(event.GetEventObject());
    edit->HideToolTip();
}

void MainFrame::OnCodeEditSavePointLeft(wxScintillaEvent& event)
{
    for (unsigned int page = 0; page < m_openFiles.size(); ++page)
    {
        if (m_openFiles[page]->edit == event.GetEventObject())
        {
            m_notebook->SetPageText(page, m_openFiles[page]->file->GetDisplayName() + "*");
        }
    }
}

void MainFrame::OnCodeEditSavePointReached(wxScintillaEvent& event)
{
    for (unsigned int page = 0; page < m_openFiles.size(); ++page)
    {
        if (m_openFiles[page]->edit == event.GetEventObject())
        {
            m_notebook->SetPageText(page, m_openFiles[page]->file->GetDisplayName());
        }
    }
}

void MainFrame::OnCodeEditUpdateUI(wxScintillaEvent& event)
{
    UpdateStatusBarLineAndColumn();
}

void MainFrame::OnCodeEditMarginClick(wxScintillaEvent& event)
{

    // Toggle a break point.

    int pageIndex = GetSelectedPage();

    if (pageIndex != -1)
    {

        unsigned int pos  = event.GetPosition();
        unsigned int newLine = m_openFiles[pageIndex]->edit->LineFromPosition(pos);

        OpenFile* openFile = m_openFiles[pageIndex];
        ToggleBreakpoint(openFile->file, newLine);

    }

}

unsigned int MainFrame::NewToOldLine(Project::File* file, unsigned int newLine)
{
    
    DebugFrontend::Script* script = DebugFrontend::Get().GetScript(file->scriptIndex);
        
    if (script == NULL)
    {
        // This file isn't being debugged, so we don't need to map lines.
        return newLine;
    }
    else
    {
        UpdateLineMappingIfNecessary(file);
        return script->lineMapper.GetOldLine( newLine );
    }

}

unsigned int MainFrame::OldToNewLine(Project::File* file, unsigned int oldLine)
{

    DebugFrontend::Script* script = DebugFrontend::Get().GetScript(file->scriptIndex);
    wxASSERT(script != NULL);

    if (script == NULL)
    {
        // This file isn't being debugged, so we don't need to map lines.
        return oldLine;
    }
    else
    {
        UpdateLineMappingIfNecessary(file);
        return script->lineMapper.GetNewLine( oldLine );
    }

}

void MainFrame::UpdateLineMappingIfNecessary(Project::File* file)
{    
    
    DebugFrontend::Script* script = DebugFrontend::Get().GetScript(file->scriptIndex);

    // Check if the file is open in the editor.

    unsigned int openFileIndex = GetOpenFileIndex(file);
    
    if (openFileIndex != -1)
    {

        OpenFile* openFile = m_openFiles[openFileIndex];
        
        if (openFile->edit->GetIsLineMappingDirty())
        {
            script->lineMapper.Update( script->source, std::string(openFile->edit->GetText()) );
            openFile->edit->SetIsLineMappingDirty(false);
        }

    }

}

void MainFrame::OnCodeEditReadOnlyModifyAttempt(wxScintillaEvent& event)
{

    // Show a message indicating the file is ready only.

    int pageIndex = GetSelectedPage();

    if (pageIndex != -1)
    {

        OpenFile* file = m_openFiles[pageIndex];

        // If thi is an "unavailable" file, don't let the user edit it at all.
        if (file->file->state != CodeState_Normal)
        {
            return;
        }

        bool checkOutOption = false;

        // Check to see if the file is under source control.
        if (m_sourceControl.GetIsInitialized())
        {
        
            SourceControl::Status status;
            status = m_sourceControl.GetFileStatus(file->file->fileName.GetFullPath());

            if (status == SourceControl::Status_CheckedOut || status == SourceControl::Status_CheckedIn)
            {
                checkOutOption = true;
            }

        }

        wxString fileName = file->file->fileName.GetFullPath();
        ReadOnlyDialog dialog(this, file->file->fileName.GetFullName(), checkOutOption);
        
        int result = dialog.ShowModal();

        if (result == ReadOnlyDialog::ID_CheckOut)
        {
            
            std::vector<std::string> fileNames;
            fileNames.push_back(std::string(fileName));
            
            m_sourceControl.CheckOut(fileNames, NULL);
            UpdateDocumentReadOnlyStatus();
            UpdateProjectFileStatus(file->file);

        }
        else if (result == ReadOnlyDialog::ID_MakeWriteable)
        {

            DWORD attributes = GetFileAttributes(fileName);
            attributes = attributes & ~FILE_ATTRIBUTE_READONLY;
            
            SetFileAttributes(fileName, attributes);
            
            UpdateDocumentReadOnlyStatus();

        }
    
    }

}

void MainFrame::OnCodeEditModified(wxScintillaEvent& event)
{

    int linesAdded = event.GetLinesAdded();

    if (linesAdded == 0)
    {
        // No lines added, so nothing needs to be done.
        return;
    }

    int pageIndex = GetSelectedPage();

    // If for some reason the event didn't come from the edit we have open,
    // search for the correct one.
    if (pageIndex != -1 && m_openFiles[pageIndex]->edit->GetId() != event.GetId())
    {
        pageIndex = -1;
        for (unsigned int i = 0; i < m_openFiles.size(); ++i)
        {
            if (m_openFiles[i]->edit->GetId() == event.GetId())
            {
                pageIndex = i;
            }
        }
    }

    if (pageIndex == -1)
    {
        // No matching file.
        return;
    }

    CodeEdit* edit = m_openFiles[pageIndex]->edit;

    unsigned int position = event.GetPosition();
    
    unsigned int line = edit->LineFromPosition(position);
    unsigned int lineStartPosition = edit->PositionFromLine(line);

    // Shift the breakpoints (note Scintilla automatically moves the markers).

    Project::File* file = m_openFiles[pageIndex]->file;

    if (linesAdded > 0)
    {

        // We're at the beginning of the line, so move the breakpoints
        // on this line as well.
        bool includeCurrentLine = (position == lineStartPosition);

        for (unsigned int i = 0; i < file->breakpoints.size(); ++i)
        {
            if ( (file->breakpoints[i] > line) || (includeCurrentLine && file->breakpoints[i] >= line) )
            {
                file->breakpoints[i] += linesAdded;
            }
        }

    }
    else
    {

        std::vector<unsigned int>::iterator iterator = file->breakpoints.begin();

        bool collapsedBreakpoint = false;

        while (iterator != file->breakpoints.end())
        {

            unsigned int l = *iterator;

            // Note, the breakpoint marker on the first line is not decremented by Scintinlla
            // so we reproduce this (undesireable) behavior here to keep things in sync.
            if (*iterator >= line)
            {
                if (*iterator >= line - linesAdded)
                {
                    // Shift the breakpoint up the appropriate number of lines.
                    *iterator += linesAdded;
                    ++iterator;
                }
                else 
                {
                    // This breakpoint was in the range of lines we deleted, so we collapse it down
                    // to the first deleted line.
                    if (collapsedBreakpoint)
                    {
                        iterator = file->breakpoints.erase(iterator);
                    }
                    else
                    {
                        *iterator = line;
                        collapsedBreakpoint = true;
                        ++iterator;
                    }
                }
            }
            else
            {
                ++iterator;
            }
        }

    }

    m_breakpointsWindow->UpdateBreakpoints(file);

}

MainFrame::OpenFile* MainFrame::OpenScript(unsigned int scriptIndex)
{

    Project::File* file = m_project->GetFileForScript(scriptIndex);
    assert(file != NULL);

    return OpenProjectFile(file);

}

void MainFrame::ShowScriptLine(unsigned int scriptIndex, unsigned int line)
{

    if (scriptIndex != -1)
    {
        
        ClearCurrentLineMarker();

        OpenFile* file = GotoOldLine(scriptIndex, line, false);
  
        m_currentScriptIndex    = scriptIndex;
        m_currentLine           = line;

        // We don't want the current and break line markers at the same time.
        if (m_currentLine != m_breakLine || m_currentScriptIndex != m_breakScriptIndex)
        {
            unsigned int newLine = OldToNewLine(file->file, m_currentLine);
            file->edit->MarkerAdd(newLine, CodeEdit::Marker_CurrentLine);
        }

    }

}

MainFrame::OpenFile* MainFrame::GetFileForScript(unsigned int scriptIndex) const
{

    for (unsigned int i = 0; i < m_openFiles.size(); ++i)
    {
        if (m_openFiles[i]->file->scriptIndex == scriptIndex)
        {
            return m_openFiles[i];
        }
    }

    return NULL;

}

void MainFrame::OnBreak(wxDebugEvent& event)
{

    UpdateForNewState();

    // Bring ourself to the top of the z-order.
    BringToFront();

    ClearBreakLineMarker();

    m_breakScriptIndex  = event.GetScriptIndex();
    m_breakLine         = event.GetLine();

    unsigned int stackLevel = 0;

    DebugFrontend& frontend = DebugFrontend::Get();
    unsigned int numStackFrames = frontend.GetNumStackFrames();
    
    if (m_breakScriptIndex != -1)
    {

        OpenFile* file = GetFileForScript(m_breakScriptIndex);

        if (file == NULL)
        {
            file = OpenScript(m_breakScriptIndex);
        }

        unsigned int newLine = OldToNewLine(file->file, m_breakLine);
        file->edit->MarkerAdd(newLine, CodeEdit::Marker_BreakLine);

        ShowScriptLine(event.GetScriptIndex(), event.GetLine());
    
    }
    else
    {

        ClearCurrentLineMarker();

        // Find the first script function in the call stack.

        for (unsigned int i = 0; i < numStackFrames; ++i)
        {

            const DebugFrontend::StackFrame& stackFrame = frontend.GetStackFrame(i);

            if (stackFrame.scriptIndex != -1)
            {
                ShowScriptLine(stackFrame.scriptIndex, stackFrame.line);
                stackLevel = i;
                break;
            }

        }
    
    }

    // Update the call stack.
    m_callStack->DeleteAllItems();
    
    for (unsigned int i = 0; i < numStackFrames; ++i)
    {

        const DebugFrontend::StackFrame& stackFrame = frontend.GetStackFrame(i);
        
        wxString item;

        if (stackFrame.scriptIndex != -1)
        {

            Project::File* file = m_project->GetFileForScript(stackFrame.scriptIndex);
            unsigned int lineNumber = OldToNewLine(file, stackFrame.line);

            const DebugFrontend::Script* script = frontend.GetScript(stackFrame.scriptIndex);
            item.Printf(_("%s!%s Line %d"), script->name.c_str(), stackFrame.function.c_str(), lineNumber + 1);

        }
        else
        {
            item.Printf(_("%s"), stackFrame.function.c_str());
        }
        
        m_callStack->Append(item);
    
    }

    // Set the VM the debugger is working with to the one that this event came
    // from. Note this will update the watch values.
    SetContext(event.GetVm(), stackLevel);

}

void MainFrame::OnLoadError(wxDebugEvent& event)
{
    
    GotoError(event.GetMessage());

    m_output->OutputError(event.GetMessage());
    wxMessageBox(event.GetMessage(), _("Load Error"), wxOK | wxICON_WARNING, this);

}

void MainFrame::OnMessage(wxDebugEvent& event)
{

    MessageType type = event.GetMessageType();

    if (type == MessageType_Warning)
    {
        m_output->OutputWarning(event.GetMessage());
    }
    else if (type == MessageType_Error)
    {
        m_output->OutputError(event.GetMessage());
    }
    else
    {
        m_output->OutputMessage(event.GetMessage());
    }

}

void MainFrame::OnSessionEnd(wxDebugEvent& event)
{

    ClearBreakLineMarker();
    ClearCurrentLineMarker();

    // Check if all of the VMs have been closed.
    if (!m_vms.empty())
    {
        m_output->OutputWarning("Warning 1003: Not all virtual machines were destroyed");
    }

    m_project->CleanUpAfterSession();
    CleanUpTemporaryFiles();

    // Clean up after the debugger.
    DebugFrontend::Get().Shutdown();

    UpdateForNewState();

    SetContext(0, 0);
    m_vms.clear();
    m_vmList->DeleteAllItems();

    SetMode(Mode_Editing);
    m_output->OutputMessage("Debugging session ended");

}

void MainFrame::ClearCurrentLineMarker()
{
    
    if (m_currentScriptIndex != -1 && m_currentLine != -1)
    {

        OpenFile* file = GetFileForScript(m_currentScriptIndex);

        if (file != NULL)
        {
            unsigned int newLine = OldToNewLine(file->file, m_currentLine);
            file->edit->MarkerDelete(newLine, CodeEdit::Marker_CurrentLine);
        }

        m_currentScriptIndex = -1;
        m_currentLine        = -1;

    }

}

void MainFrame::ClearBreakLineMarker()
{
    
    if (m_breakScriptIndex != -1 && m_breakLine != -1)
    {

        OpenFile* file = GetFileForScript(m_breakScriptIndex);

        if (file != NULL)
        {
            unsigned int newLine = OldToNewLine(file->file, m_breakLine);
            file->edit->MarkerDelete(newLine, CodeEdit::Marker_BreakLine);
        }

        m_breakScriptIndex  = -1;
        m_breakLine         = -1;

    }

}

void MainFrame::GotoError(const wxString& message)
{

    wxString target;
    unsigned int newLine;

    if (ParseHelpMessage(message, target))
    {
        m_helpController.DisplaySection(target);
    }
    else if (ParseErrorMessage(message, target, newLine))
    {

        OpenFile* file = OpenDocument(target);

        if (file != NULL)
        {
            GotoNewLine(file->edit, newLine - 1, true);
        }

    }

}

void MainFrame::GotoNewLine(Project::File* file, unsigned int newLine)
{

    OpenFile* openFile = OpenProjectFile(file);

    if (openFile != NULL)
    {
        GotoNewLine(openFile->edit, newLine, true);
    }

}

void MainFrame::GotoNewLine(CodeEdit* edit, unsigned int newLine, bool center)
{

    // Show the file.

    int pageIndex = m_notebook->GetPageIndex(edit);
    m_notebook->SetSelection(pageIndex);

    if (center)
    {

        // Scroll so that the line is in the center of the display.

        int linesOnScreen = edit->LinesOnScreen();
        int startLine     = std::max<int>(0, newLine - linesOnScreen / 2);

        edit->ScrollToLine(startLine);

    }
    else
    {
        edit->GotoLine(newLine);
    }

    edit->SetFocus();
    
    int pos = edit->PositionFromLine(newLine);
    
    edit->SetSelectionStart( pos );
    edit->SetCurrentPos( pos );

}

MainFrame::OpenFile* MainFrame::GotoOldLine(unsigned int scriptIndex, unsigned int oldLine, bool center)
{

    // Open the file if necessary.

    OpenFile* file = GetFileForScript(scriptIndex);

    if (file == NULL)
    {
        file = OpenScript(scriptIndex);
    }

    unsigned int newLine = OldToNewLine(file->file, oldLine);
    GotoNewLine(file->edit, newLine, center);

    return file;

}

bool MainFrame::ParseHelpMessage(const wxString& message, wxString& topic) const
{

    int topicId;

    if (sscanf(message.c_str(), "Warning %d :", &topicId) == 1)
    {
        topic.Printf("Decoda_warning_%d.html", topicId);
        return true;
    }
    else if (sscanf(message.c_str(), "Error %d :", &topicId) == 1)
    {
        topic.Printf("Decoda_error_%d.html", topicId);
        return true;
    }

    return false;
}

bool MainFrame::ParseErrorMessage(const wxString& error, wxString& fileName, unsigned int& line) const
{

    if (ParseLuaErrorMessage (error, fileName, line) ||
        ParseLuacErrorMessage(error, fileName, line))
    {

        // Check if the target stars with "...". Luac does this if the file name is too long.
        // In that case, we find the closest matching file in the project.

        if ( fileName.StartsWith(wxT("...")) )
        {

            bool foundMatch = false;
            wxString partialName = wxFileName(fileName.Mid(3)).GetFullPath();

            for (unsigned int fileIndex = 0; fileIndex < m_openFiles.size() && !foundMatch; ++fileIndex)
            {
                
                OpenFile* file = m_openFiles[fileIndex];
                wxString fullName = file->file->fileName.GetFullPath();

                if (fullName.EndsWith(partialName))
                {
                    fileName = fullName;
                    foundMatch = true;
                }

            }

            for (unsigned int fileIndex = 0; fileIndex < m_project->GetNumFiles() && !foundMatch; ++fileIndex)
            {
                
                Project::File* file = m_project->GetFile(fileIndex);
                wxString fullName = file->fileName.GetFullPath();

                if (fullName.EndsWith(partialName))
                {
                    fileName = fullName;
                    foundMatch = true;
                }

            }
        
        }

        return true;

    }

    return false;

}

bool MainFrame::ParseLuaErrorMessage(const wxString& error, wxString& fileName, unsigned int& line) const
{

    // Error messages have the form "filename:line: message"

    fileName = error;
    fileName.Trim(false);

    int fileNameEnd;
    
    if (fileName.Length() >= 3 && isalpha(fileName[0]) && fileName[1] == ':' && wxIsPathSeparator(fileName[2]))
    {
        // The form appears to have a drive letter in front of the path.
        fileNameEnd = fileName.find(':', 3);
    }
    else
    {
        fileNameEnd = fileName.find(':');
    }

    if (fileNameEnd == wxNOT_FOUND)
    {
        return false;
    }

    if (sscanf(fileName.c_str() + fileNameEnd, ":%d:", &line) == 1)
    {
        fileName = fileName.Left(fileNameEnd);
        return true;
    }

    return false;

}

bool MainFrame::ParseLuacErrorMessage(const wxString& error, wxString& fileName, unsigned int& line) const
{

    // "appname: filename:line: message"

    int appNameEnd = error.Find(wxT(": "));

    if (appNameEnd == wxNOT_FOUND)
    {
        return false;
    }

    wxString temp = error.Right(error.Length() - appNameEnd - 1);
    return ParseLuaErrorMessage(temp, fileName, line);

}

unsigned int MainFrame::GetScriptIndex(wxScintilla* edit) const
{
    for (unsigned int i = 0; i < m_openFiles.size(); ++i)
    {
        if (m_openFiles[i]->edit == edit)
        {
            return m_openFiles[i]->file->scriptIndex;
        }
    }
    return -1;
}

void MainFrame::SetContext(unsigned int vm, unsigned int stackLevel)
{

    m_vm = vm;
    m_stackLevel = stackLevel;
    m_watch->SetContext(m_vm, m_stackLevel);
    m_watch->UpdateItems();

    // Update the selection in the VM list.

    m_vmList->ClearAllIcons();
    unsigned int vmIndex = std::find(m_vms.begin(), m_vms.end(), vm) - m_vms.begin();

    if (vmIndex < m_vms.size())
    {
        m_vmList->SetItemIcon(vmIndex, ListWindow::Icon_YellowArrow);
    }

    // Update the icons in the call stack.

    m_callStack->ClearAllIcons();

    if (stackLevel < static_cast<unsigned int>(m_callStack->GetItemCount()))
    {

        m_callStack->SetItemIcon(0, ListWindow::Icon_YellowArrow);

        if (stackLevel != 0)
        {
            m_callStack->SetItemIcon(stackLevel, ListWindow::Icon_GreenArrow);
        }

    }

}

void MainFrame::AddRunningProcess(wxProcess* process)
{

    if (m_runningProcesses.empty())
    {
        // We want to start getting the timer events to ensure that a
        // steady stream of idle events comes in -- otherwise we
        // wouldn't be able to poll the child process input
        m_timerIdleWakeUp.Start(100);
    }

    m_runningProcesses.push_back(process);

}

void MainFrame::RemoveRunningProcess(wxProcess* process)
{
    
    m_runningProcesses.erase(std::find(m_runningProcesses.begin(), m_runningProcesses.end(), process));

    if (m_runningProcesses.empty())
    {
        // We don't need to get idle events all the time any more
        m_timerIdleWakeUp.Stop();
    }

}

void MainFrame::OnIdle(wxIdleEvent& event)
{

    // Grab any input from the running processes and output it to the window.

    for (unsigned int i = 0; i < m_runningProcesses.size(); ++i)
    {

        wxProcess* process = m_runningProcesses[i];
        
        if (m_processOutputSink.Pump(*m_output, *process))
        {
            // We have more input to process, so get another idle.
            event.RequestMore();
        }

    }

    // Detect when the control key is released and complete MRU paging.
    if (m_tabOrderIndex != -1 && !wxGetKeyState(WXK_CONTROL))
    {
        m_tabOrderIndex = -1;
        SetMostRecentlyUsedPage(m_notebook->GetSelection());
    }
}

void MainFrame::OnTimer(wxTimerEvent& WXUNUSED(event))
{
    // Send an idle event. By processing in idle instead of directly in here we
    // make things play nicer with the event queue.
    wxWakeUpIdle();
}
    
void MainFrame::OnProcessTerminate(wxProcessEvent& event)
{

    // Remove the process from our list.

    std::vector<wxProcess*>::iterator iterator = m_runningProcesses.begin();

    while (iterator != m_runningProcesses.end())
    {

        wxProcess* process = *iterator;

        if (process->GetPid() == event.GetPid())
        {

            // Grab any remaning output from this process.
            m_processOutputSink.Dump(*m_output, *process);

            // Ring the bell to alert the user.
            wxBell();

            m_output->OutputMessage("Output completed - Normal Termination");
            iterator = m_runningProcesses.erase(iterator);

        }
        else
        {
            ++iterator;
        }
    }

    // Running a process could cause some of our open files to change (if one
    // of the files is an output from the process) so check if we need to reload
    // them.
    CheckReload();

}

void MainFrame::OnProjectExplorerItemActivated(wxTreeEvent& event)
{

    wxTreeItemId item = event.GetItem();

    if (!item.IsOk())
    {
        // When the keyboard is used, the event item comes through as invalid.
        // This seems like a bug in wxWidgets, so we just work around it like
        // this.
        item = m_projectExplorer->GetSelection();
    }

    if (!item.IsOk())
    {
        // This shouldn't ever happen.
        return;
    }
    
    ProjectExplorerWindow::ItemData* data = m_projectExplorer->GetDataForItem(item);

    if (data != NULL && data->file != NULL)
    {

        OpenFile* file = OpenProjectFile(data->file);

        if (file == NULL)
        {
            wxMessageBox(wxString::Format("The file '%s' could not be opened.", data->file->fileName.GetFullPath()));
        }
        else
        {
            // If we selected a symbol, go to the line where the symbol is defined.
            if (data->symbol != NULL)
            {
                GotoNewLine(file->edit, data->symbol->line, true);
            }
        }

    }

}

void MainFrame::OnProjectExplorerKeyDown(wxTreeEvent& event)
{
    if (event.GetKeyEvent().GetKeyCode() == WXK_DELETE)
    {

        std::vector<Project::File*> files;
        m_projectExplorer->GetSelectedFiles(files);

        bool alltemp = true;

        for (unsigned int i = 0; i < files.size(); ++i)
        {
            if (!files[i]->temporary)
            {
                DeleteProjectFile(files[i]);
                alltemp = false;
            }
        }

        if (alltemp)
        {
            // Tell the user why we didn't delete any of the files the selected.
            wxMessageBox("Temporary files cannot be removed from the Project Explorer. They are\nautomatically removed when the debugging session ends or the file is closed."); 
        }

    }
}

void MainFrame::OnOutputKeyDown(wxKeyEvent& event)
{

    if (event.GetKeyCode() == WXK_F1)
    {

        int row = m_output->GetCurrentLine();

        wxString message = m_output->GetLineText(row);
        wxString target;

        if (ParseHelpMessage(message, target))
        {
            m_helpController.DisplaySection(target);
        }

    }
    else
    {
        event.Skip();
    }

}

void MainFrame::OnSourceControlCheckIn(wxCommandEvent& event)
{

    int pageIndex = GetSelectedPage();

    if (pageIndex != -1)
    {
        
        OpenFile* file = m_openFiles[pageIndex];

        std::vector<std::string> fileNames;
        fileNames.push_back(std::string(file->file->fileName.GetFullPath()));

        m_sourceControl.CheckIn(fileNames, NULL);
        UpdateProjectFileStatus();
    
    }

}

void MainFrame::OnUpdateSourceControlCheckIn(wxUpdateUIEvent& event)
{

    bool allow = false;

    int pageIndex = GetSelectedPage();

    if (pageIndex != -1)
    {
        OpenFile* file = m_openFiles[pageIndex];
        allow = file->file->status == Project::Status_CheckedOut;
    }

    event.Enable(allow);

}

void MainFrame::OnSourceControlCheckOut(wxCommandEvent& event)
{

    int pageIndex = GetSelectedPage();

    if (pageIndex != -1)
    {
        
        OpenFile* file = m_openFiles[pageIndex];

        std::vector<std::string> fileNames;
        fileNames.push_back(std::string(file->file->fileName.GetFullPath()));

        m_sourceControl.CheckOut(fileNames, NULL);
        UpdateProjectFileStatus();
    
    }

}

void MainFrame::OnUpdateSourceControlCheckOut(wxUpdateUIEvent& event)
{

    bool allow = false;

    int pageIndex = GetSelectedPage();

    if (pageIndex != -1)
    {
        OpenFile* file = m_openFiles[pageIndex];
        allow = file->file->status == Project::Status_CheckedIn;
    }

    event.Enable(allow);

}

void MainFrame::OnSourceControlUndoCheckOut(wxCommandEvent& event)
{
    
    int pageIndex = GetSelectedPage();

    if (pageIndex != -1)
    {
        
        OpenFile* file = m_openFiles[pageIndex];

        std::vector<std::string> fileNames;
        fileNames.push_back(std::string(file->file->fileName.GetFullPath()));

        m_sourceControl.UndoCheckOut(fileNames);
        UpdateProjectFileStatus();
    
    }
   
}

void MainFrame::OnUpdateSourceControlUndoCheckOut(wxUpdateUIEvent& event)
{

    bool allow = false;

    int pageIndex = GetSelectedPage();

    if (pageIndex != -1)
    {
        OpenFile* file = m_openFiles[pageIndex];
        allow = file->file->status == Project::Status_CheckedOut;
    }

    event.Enable(allow);

}

void MainFrame::OnSourceControlRefresh(wxCommandEvent& event)
{
    UpdateProjectFileStatus();
}

void MainFrame::OnUpdateSourceControlRefresh(wxUpdateUIEvent& event)
{
    event.Enable(m_sourceControl.GetIsInitialized());
}

void MainFrame::OnFindClose(wxFindDialogEvent& event)
{
    m_findDialog->Destroy();
    m_findDialog = NULL;
}

void MainFrame::OnFindFind(wxFindDialogEvent& event)
{
    FindNext(event.GetFindString(), event.GetFlags());
}

void MainFrame::OnFindReplace(wxFindDialogEvent& event)
{

    int pageIndex = GetSelectedPage();

    if (pageIndex != -1)
    {

        OpenFile* file = m_openFiles[pageIndex];

        // Check to see if the currently selected text matches our search string.

        int flags = 0;

        if (event.GetFlags() & wxFR_WHOLEWORD)
        {
            flags |= wxSCI_FIND_WHOLEWORD;
        }

        if (event.GetFlags() & wxFR_MATCHCASE)
        {
            flags |= wxSCI_FIND_MATCHCASE;
        }

        int searchStart = file->edit->GetSelectionStart();
        int searchEnd   = file->edit->GetSelectionEnd();    
        
        int length;
        int start = file->edit->FindText(searchStart, searchEnd, event.GetFindString(), flags, &length);

        if (start == searchStart)
        {
            // The selection matches, so replace it.
            file->edit->ReplaceSelection(event.GetReplaceString());
        }

        // Search for the next instance.
        FindNext(event.GetFindString(), event.GetFlags());
    
    }

}

void MainFrame::OnFindReplaceAll(wxFindDialogEvent& event)
{
    
    int pageIndex = GetSelectedPage();

    if (pageIndex != -1)
    {
        
        OpenFile* file = m_openFiles[pageIndex];

        file->edit->BeginUndoAction();

        int searchStart = 0;
        int searchEnd   = file->edit->GetLength();

        int flags = 0;

        if (event.GetFlags() & wxFR_WHOLEWORD)
        {
            flags |= wxSCI_FIND_WHOLEWORD;
        }

        if (event.GetFlags() & wxFR_MATCHCASE)
        {
            flags |= wxSCI_FIND_MATCHCASE;
        }

        int replaces = 0;

        while (searchStart < searchEnd)
        {

            int length;
            int start = file->edit->FindText(searchStart, searchEnd, event.GetFindString(), flags, &length);

            if (start != -1)
            {

                // Replace the text.

                file->edit->SetTargetStart(start);
                file->edit->SetTargetEnd(start + length);
                file->edit->ReplaceTarget(event.GetReplaceString());

                searchStart = start + event.GetReplaceString().Length();
                ++replaces;

            }
            else
            {
                // No more matches.
                searchStart = searchEnd;
            }

        }

        file->edit->EndUndoAction();

        // Disable the find dialog while we should the message box so that the message
        // box feels like it's modal to the entire application.
        m_findDialog->Enable(false);

        if (replaces == 0)
        {
            wxMessageBox("The specified text could not be found.", s_applicationName, wxOK | wxICON_INFORMATION, this);
        }
        else
        {
            wxMessageBox(wxString::Format("Replaced %d occurances.", replaces), s_applicationName, wxOK | wxICON_INFORMATION, this);
        }

        m_findDialog->Enable(true);
        m_findDialog->SetFocus();

    }
   
}

void MainFrame::OnFindNext(wxFindDialogEvent& event)
{
    FindNext(event.GetFindString(), event.GetFlags());
}

void MainFrame::FindText(OpenFile* file, const wxString& text, int flags)
{

    bool firstSearch = false;
    bool passedStart = false;

    if (file->edit->GetCurrentPos() != m_lastFindPosition)
    {
        m_findStart = file->edit->GetCurrentPos();
        firstSearch = true;
    }

    int searchStart = file->edit->GetCurrentPos();
    int searchEnd   = file->edit->GetLength();

    int f = 0;

    if (flags & wxFR_WHOLEWORD)
    {
        f |= wxSCI_FIND_WHOLEWORD;
    }

    if (flags & wxFR_MATCHCASE)
    {
        f |= wxSCI_FIND_MATCHCASE;
    }

    if (!(flags & wxFR_DOWN))
    {
        searchEnd = 0;

        if (file->edit->GetCurrentPos() == m_lastFindPosition)
        {
            --searchStart;
        }

    }


    int length;
    int start = file->edit->FindText(searchStart, searchEnd, text, f, &length);

    if (start == -1)
    {

        if (!firstSearch)
        {
            if (searchStart <= m_findStart && m_findStart <= searchEnd)
            {
                passedStart = true;
            }
            else if (searchEnd <= m_findStart && m_findStart <= searchStart)
            {
                passedStart = true;
            }
        }

        // Loop around and search from the beginning of the file (or end
        // depending on the search direction).

        if (flags & wxFR_DOWN)
        {
            searchStart = 0;
            searchEnd   = file->edit->GetCurrentPos();
        }
        else
        {
            searchEnd   = file->edit->GetCurrentPos() + 1;
            searchStart = file->edit->GetLength();
        }

        start = file->edit->FindText(searchStart, searchEnd, text, f, &length);

    }

    if (start != -1 && text.Length() > 0)
    {

        // Select the found text.
        file->edit->SetSelection(start, start + length);
        m_lastFindPosition = file->edit->GetCurrentPos();

        // Check to see if we reached the start point of the search.

        if (!firstSearch)
        {
            if (searchStart <= m_findStart && m_findStart <= searchEnd && start >= m_findStart)
            {
                passedStart = true;
            }
            if (searchEnd <= m_findStart && m_findStart <= searchStart && start <= m_findStart)
            {
                passedStart = true;
            }
        }

        if (passedStart)
        {

            if (m_findDialog != NULL)
            {
                // Disable the find dialog while we show the message box so that the message
                // box feels like it's modal to the entire application.
                m_findDialog->Enable(false);
            }
            
            wxMessageBox("Find reached the start point of the search.", s_applicationName, wxOK | wxICON_INFORMATION, this);

            if (m_findDialog != NULL)
            {
                m_findDialog->Enable(true);
                m_findDialog->SetFocus();
            }

        }

    }
    else
    {
        
        m_lastFindPosition = -1;

        if (m_findDialog != NULL)
        {
            // Disable the find dialog while we show the message box so that the message
            // box feels like it's modal to the entire application.
            m_findDialog->Enable(false);
        }

        wxMessageBox("The specified text could not be found.", s_applicationName, wxOK | wxICON_INFORMATION, this);
        
        if (m_findDialog != NULL)
        {
            m_findDialog->Enable(true);
            m_findDialog->SetFocus();
        }

    }

}

void MainFrame::SubstituteVariableArguments(wxString& text) const
{
    
    wxFileName itemFileName;

    int pageIndex = GetSelectedPage();

    if (pageIndex != -1)
    {
        itemFileName = m_openFiles[pageIndex]->file->fileName;
    }

    text.Replace("$(ItemPath)",     itemFileName.GetFullPath());
    text.Replace("$(ItemDir)",      itemFileName.GetPath());
    text.Replace("$(ItemFileName)", itemFileName.GetName());
    text.Replace("$(ItemExt)",      itemFileName.GetExt());

}

void MainFrame::UpdateToolsMenu()
{

    // Remove everything from the menu.
    while (m_menuTools->GetMenuItemCount() > 0)
    {
        m_menuTools->Destroy(m_menuTools->FindItemByPosition(0));
    }

    // Add the external tools.
    for (unsigned int i = 0; i < m_tools.size(); ++i)
    {
        const wxString& title = m_tools[i]->GetTitle();
        if (!title.empty())
        {
            wxMenuItem* menuItem = m_menuTools->Append(ID_FirstExternalTool + i, m_tools[i]->GetTitle());
        }
    }

    m_menuTools->Append(ID_ToolsExternalTools,    _("&External Tools..."));
    m_menuTools->AppendSeparator();
    m_menuTools->Append(ID_ToolsSettings,         _("&Settings..."));

}

void MainFrame::RunTool(ExternalTool* tool)
{

    assert(tool);

    if (tool)
    {
        wxProcess* process = tool->Run(this);        
            
        if (process != NULL)
        {

            // Clear the output window.
            m_output->Clear();
            m_output->OutputMessage("------------------------- " + tool->GetTitle() + " -------------------------");

            // We need to get the terminate event from this process, so set the handler.
            process->SetNextHandler(this);

            AddRunningProcess(process);

        }
        else
        {
            wxMessageBox(_("Error running tool."), _("External Tool"), wxOK | wxICON_ERROR, this);
        }
    }

}

void MainFrame::LoadOptions()
{
    
    // Disable logging since it's ok if the options file is not there.
    wxLogNull logNo;

    ClearVector(m_tools);

    wxXmlNode* keyBindingNode = NULL;

    wxXmlDocument document;
    if (document.Load(GetAppDataDirectory() + "options.xml"))
    {
        
        wxXmlNode* root = document.GetRoot();

        if (root->GetName() == "options")
        {
            wxXmlNode* node = root->GetChildren();
            while (node != NULL)
            {

                wxString data;

                if (node->GetName() == "tool")
                {
                    LoadExternalTool(node);
                }
                else if (node->GetName() == "editor")
                {
                    m_editorSettings.Load(node);
                }
                else if (node->GetName() == "fontcolor")
                {
                    m_fontColorSettings.Load(node);
                }
                else if (node->GetName() == "system")
                {
                    m_systemSettings.Load(node);
                }
                else if (node->GetName() == "keybindings")
                {
                    // Save the node and we'll load when we're done.
                    keyBindingNode = node;
                }
                else if (node->GetName() == "recent_projects")
                {
                    XmlConfig config(node);
                    m_projectFileHistory.Load(config);
                }
                else if (node->GetName() == "recent_files")
                {
                    XmlConfig config(node);
                    m_fileHistory.Load(config);
                }
                else if (node->GetName() == "find_directories")
                {
                    XmlConfig config(node);
                    m_findDirectoryHistory.Load(config);
                }
                else if (ReadXmlNode(node, "last_project_loaded", m_lastProjectLoaded))
                {
                }
                else if (ReadXmlNode(node, "layout", data))
                {

                    wxString mode = node->GetPropVal("mode", "");
                    
                    for (unsigned int i = 0; i < Mode_NumModes; ++i)
                    {
                        if (mode.IsSameAs(s_modeName[i]))
                        {
                            m_modeLayout[i] = data;
                            break;
                        }
                    }
  
                }
                else if (node->GetName() == "window")
                {
                    LoadWindowPlacement(node);
                }

                node = node->GetNext();

            }
        }

    }

    // Initialize the menu now so that it contains the tools and so that it's
    // available when the key bindings are loaded.
    InitializeMenu();

    m_keyBinder.AddCommandsFromMenuBar(GetMenuBar());

    if (keyBindingNode != NULL)
    {
        m_keyBinder.Load(keyBindingNode);
    }
    else
    {
        SetDefaultHotKeys();
    }

    m_keyBinder.UpdateWindow(this);
    m_keyBinder.UpdateMenuBar(GetMenuBar());

    UpdateEditorOptions();

    // Add the saved recent project files to the menu.
    m_fileHistory.AddFilesToMenu();
    m_projectFileHistory.AddFilesToMenu();

    // Update the current layout.
    m_mgr.LoadPerspective(m_modeLayout[m_mode]);

}

void MainFrame::LoadExternalTool(wxXmlNode* node)
{

    wxString title;
    node->GetPropVal("title", &title);

    wxString command;
    node->GetPropVal("command", &command);

    wxString arguments;
    node->GetPropVal("arguments", &arguments);

    wxString initialDirectory;
    node->GetPropVal("initialdirectory", &initialDirectory);

    ExternalTool* tool = new ExternalTool;

    tool->SetTitle(title);
    tool->SetCommand(command);
    tool->SetArguments(arguments);
    tool->SetInitialDirectory(initialDirectory);

    m_tools.push_back(tool);

}

void MainFrame::LoadWindowPlacement(wxXmlNode* root)
{

    wxXmlNode* node = root->GetChildren();
    
    while (node != NULL)
    {

        ReadXmlNode(node, "maximized", m_startUpMaximized) ||
        ReadXmlNodeRect(node, "rect", m_startUpRect);

        node = node->GetNext();

    }    

}

wxXmlNode* MainFrame::SaveWindowPlacement() const
{

    wxXmlNode* root = new wxXmlNode(wxXML_ELEMENT_NODE, "window");    

    root->AddChild( WriteXmlNodeBool("maximized", m_startUpMaximized) );
    root->AddChild( WriteXmlNodeRect("rect", m_startUpRect) );

    return root;

}

void MainFrame::SaveOptions()
{

    // Disable logging since it's ok if the options file saving isn't successful.
    wxLogNull logNo;

    wxXmlDocument document;
    
    wxXmlNode* root = new wxXmlNode(wxXML_ELEMENT_NODE, "options");
    document.SetRoot(root);

    for (unsigned int i = 0; i < m_tools.size(); ++i)
    {
        wxXmlNode* node = SaveExternalTool(m_tools[i]);
        root->AddChild(node);
    }

    // Save the position of the window.
    UpdateStartUpPlacement();
    root->AddChild( SaveWindowPlacement() );

    // Save the layouts for all of the modes.

    m_modeLayout[m_mode] = m_mgr.SavePerspective();

    for (unsigned int i = 0; i < Mode_NumModes; ++i)
    {
        wxXmlNode* node = WriteXmlNode("layout", m_modeLayout[i]);
        node->AddProperty("mode", s_modeName[i]);
        root->AddChild(node);
    }

    // Save the font and color settings.
    root->AddChild(m_fontColorSettings.Save("fontcolor"));

    // Save the editor settins.
    root->AddChild(m_editorSettings.Save("editor"));

    // Save the system bindings.
    root->AddChild(m_systemSettings.Save("system"));

    // Save the key bindings.
    root->AddChild(m_keyBinder.Save("keybindings"));

    // Save the recent files.

    wxXmlNode* recentFilesNode = new wxXmlNode(wxXML_ELEMENT_NODE, "recent_files");
    root->AddChild(recentFilesNode);

    XmlConfig recentFilesConfig(recentFilesNode);
    m_fileHistory.Save(recentFilesConfig);

    // Save the recent project files.

    wxXmlNode* recentProjectsNode = new wxXmlNode(wxXML_ELEMENT_NODE, "recent_projects");
    root->AddChild(recentProjectsNode);

    XmlConfig recentProjectsConfig(recentProjectsNode);
    m_projectFileHistory.Save(recentProjectsConfig);

    // Save the last project we loaded.

    if (!m_lastProjectLoaded.IsEmpty())
    {
        root->AddChild( WriteXmlNode("last_project_loaded", m_lastProjectLoaded) );
    }

    // Save the find directory history.
                                                                        
    wxXmlNode* findDirectoriesNode = new wxXmlNode(wxXML_ELEMENT_NODE, "find_directories");
    root->AddChild(findDirectoriesNode);

    XmlConfig findDirectoriesConfig(findDirectoriesNode);
    m_findDirectoryHistory.Save(findDirectoriesConfig);

    wxString directory = GetAppDataDirectory();

    // Make sure the directory exists.

    wxFileName fileName(GetAppDataDirectory(), "options.xml");
    fileName.Mkdir(0777, wxPATH_MKDIR_FULL);

    document.Save(fileName.GetFullPath());

}

wxXmlNode* MainFrame::SaveExternalTool(const ExternalTool* tool) const
{

    wxXmlNode* node = new wxXmlNode(wxXML_ELEMENT_NODE, "tool");

    node->AddProperty("title", tool->GetTitle());
    node->AddProperty("command", tool->GetCommand());
    node->AddProperty("arguments", tool->GetArguments());
    node->AddProperty("initialdirectory", tool->GetInitialDirectory());

    return node;

}

void MainFrame::CheckForUpdate()
{

    // Only check for updates in the non-dedicated version of Decoda. When
    // Decoda is distributed with another application, we assume the application
    // will provide updates with the rest of the software.

#ifndef DEDICATED_PRODUCT_VERSION

    if (!m_systemSettings.GetCheckForUpdates())
    {
        // The user has turned off checking for updates.
        return;
    }

    m_updater.CheckForUpdates(s_updateUrl, MainApp::s_buildNumber, UpdateCallback, this);

#endif

}

bool MainFrame::SaveProject(bool promptForName)
{

    wxString fileName = m_project->GetFileName();

    if (fileName.empty() || promptForName)
    {
        fileName = wxFileSelector("Save Project", "", "", "", "Decoda Project files (*.deproj)|*.deproj|All files (*.*)|*.*", wxSAVE, this);
    }

    if (!fileName.empty())
    {
        if (!m_project->Save(fileName))
        {
            wxMessageBox("Error saving project", s_applicationName, wxOK | wxICON_ERROR, this);
            return false;
        }
        else
        {
            m_projectFileHistory.AddFileToHistory(fileName);
            UpdateCaption();
            m_lastProjectLoaded = fileName;
            return true;
        }
    }

    return false;
}

bool MainFrame::SaveProjectIfNeeded()
{

    if (m_project->GetNeedsSave())
    {
        
        wxString message;

        if (!m_project->GetFileName().IsEmpty())
        {
            message = "File \'" + m_project->GetFileName() + "\' is modified. Save changes?";
        }
        else
        {
            message = "The project has been modified. Save changes?";
        }
        
        int result = wxMessageBox(message, s_applicationName, wxYES | wxNO | wxCANCEL | wxICON_WARNING, this);

        if (result == wxCANCEL)
        {
            return false;

        }
        else if (result == wxYES && !SaveProject())
        {
            return false;
        }

    }

    return true;

}

bool MainFrame::CloseAllFiles()
{

    // Check if any of the files need to be saved.

    wxArrayString unsavedFiles;
    wxArrayInt    selections;

    for (unsigned int i = 0; i < m_openFiles.size(); ++i)
    {
        if (m_openFiles[i]->edit->GetModify())
        {
            selections.Add(unsavedFiles.Count());
            unsavedFiles.Add(m_openFiles[i]->file->GetDisplayName());
        }
    }

    if (!unsavedFiles.IsEmpty())
    {

        ChoiceDialog dialog(this, "Save changes to the following items?", s_applicationName, unsavedFiles);
        dialog.SetSelections(selections);

        int result = dialog.ShowModal();

        if (result == wxID_CANCEL)
        {
            return false;
        }
        else if (result == wxID_YES)
        {

            selections = dialog.GetSelections();

            // Save all of the files that the user selected.

            unsigned int unsavedIndex = 0;

            for (unsigned int i = 0; i < m_openFiles.size(); ++i)
            {
                if (m_openFiles[i]->edit->GetModify())
                {
                    if (selections.Index(unsavedIndex) != wxNOT_FOUND)
                    {
                        // User selected this item to be saved.
                        if (!SaveFile(m_openFiles[i], false))
                        {
                            return false;
                        }
                    }
                    ++unsavedIndex;
                }
            }

        }

    }

    for (unsigned int i = 0; i < m_openFiles.size(); ++i)
    {
        // Mark the file as not needing to be saved so that we
        // aren't prompted again when the noteboad page is closed.
        m_openFiles[i]->edit->SetSavePoint();
    }

    // Close all of the files.

    m_notebook->SetSelection(-1);

    while (!m_openFiles.empty())
    {
        CloseDocument(m_openFiles.size() - 1, false);
    }

    m_notebook->SetSelection(-1);

    return true;

}

void MainFrame::UpdateCaption()
{

    wxString title;

    title  = m_project->GetName();
    title += " - ";
    title += s_applicationName;

    switch (DebugFrontend::Get().GetState())
    {
    case DebugFrontend::State_Running:
        title += " [run]";
        break;
    case DebugFrontend::State_Broken:
        title += " [break]";
        break;
    }
    
    SetTitle(title);

}

void MainFrame::UpdateForNewState()
{
    
    UpdateCaption();

    if (DebugFrontend::Get().GetState() != DebugFrontend::State_Broken)
    {
    
        // Clear the call stack.
        m_callStack->DeleteAllItems();

        // Clear the context.
        SetContext(0, 0);

        // Clear the indicators showing the current execution line.
        ClearCurrentLineMarker();
        ClearBreakLineMarker();

    }

}

MainFrame::OpenFile* MainFrame::OpenProjectFile(Project::File* file)
{

    // Cancel MRU paging.
    m_tabOrderIndex = -1;

    // Check to see if the file is already open.
    unsigned int index = GetOpenFileIndex(file);

    if (index != -1)
    {
        m_notebook->SetSelection(m_notebook->GetPageIndex(m_openFiles[index]->edit));
        return m_openFiles[index];
    }

    // The file isn't open, so open it.

    OpenFile* openFile = new OpenFile;

    openFile->file = file;
    
    openFile->edit = new CodeEdit;
    openFile->edit->SetAutoCompleteManager( &m_autoCompleteManager );
    openFile->edit->Create(m_notebook);
    openFile->edit->SetDropTarget( new CodeEditDropTarget(openFile->edit, this) );
    
    wxString fileName = openFile->file->fileName.GetFullPath();

    if (file->state == CodeState_Unavailable)
    {
        // Set a save point so that the editor doesn't think we need to save this file.
        openFile->edit->SetText("The source code for this script is not available.  To view script source code,\nthe script file must be added to the current project, or the debugger must be\nattached when the client loads the file.");
        openFile->edit->SetSavePoint();
        openFile->edit->EmptyUndoBuffer();
    }
    else if (file->state == CodeState_Binary)
    {
        // Set a save point so that the editor doesn't think we need to save this file.
        openFile->edit->SetText("This script was loaded as a compiled binary file.  To view script source code,\nthe uncompiled script file must be added to the current project.");
        openFile->edit->SetSavePoint();
        openFile->edit->EmptyUndoBuffer();
    }
    else if (fileName.IsEmpty())
    {
        // This is a new file, no need to do anything since it will start out empty.
    }
    else if (wxFileExists(fileName))
    {
        openFile->edit->LoadFile(file->fileName.GetFullPath());
    }
    else if (file->scriptIndex != -1)
    {
    
        const DebugFrontend::Script* script = DebugFrontend::Get().GetScript(file->scriptIndex);
    
        // Set a save point so that the editor doesn't think we need to save this file.
        openFile->edit->SetText(script->source.c_str());
        openFile->edit->SetSavePoint();
        openFile->edit->EmptyUndoBuffer();

    }
    else
    {
        delete openFile->edit;
        delete openFile;
        return NULL;
    }
    
    openFile->timeStamp = GetFileModifiedTime(openFile->file->fileName.GetFullPath());

    UpdateSyntaxColoring(openFile);
    
    openFile->edit->SetFontColorSettings(m_fontColorSettings);
    openFile->edit->SetEditorSettings(m_editorSettings);

    wxString tabName = file->GetDisplayName();

    if (!m_editorSettings.GetUseTabs() && m_editorSettings.GetRemoveTabsOnLoad())
    {
        if (openFile->edit->Untabify())
        {
            tabName += "*";
        }
    }

    // Add the breakpoints.
    for (unsigned int i = 0; i < openFile->file->breakpoints.size(); ++i)
    {
        openFile->edit->MarkerAdd(openFile->file->breakpoints[i], CodeEdit::Marker_Breakpoint);
    }

    // Add the current line and break line.
    if (DebugFrontend::Get().GetState() == DebugFrontend::State_Broken)
    {

        if (m_breakScriptIndex == openFile->file->scriptIndex)
        {
            unsigned int newLine = OldToNewLine(openFile->file, m_breakLine);
            openFile->edit->MarkerAdd(newLine, CodeEdit::Marker_BreakLine);
        }
    
        // We don't want the current and break line markers at the same time.
        if (m_currentScriptIndex == openFile->file->scriptIndex && (m_currentLine != m_breakLine || m_currentScriptIndex != m_breakScriptIndex))
        {
            unsigned int newLine = OldToNewLine(openFile->file, m_currentLine);
            openFile->edit->MarkerAdd(newLine, CodeEdit::Marker_CurrentLine);
        }

    }
    
    m_openFiles.push_back(openFile);
    m_notebook->AddPage(openFile->edit, tabName, true);

    return openFile;

}

void MainFrame::DeleteProjectFile(Project::File* file)
{

    for (unsigned int i = 0; i < m_openFiles.size(); ++i)
    {
        if (m_openFiles[i]->file == file)
        {
            m_notebook->DeletePage(i);
            m_openFiles.erase(m_openFiles.begin() + i);
            RemovePageFromTabOrder(i);
            break;
        }
    }

    m_projectExplorer->RemoveFile(file);
    m_project->RemoveFile(file);
    m_breakpointsWindow->RemoveFile(file);

}

void MainFrame::AddVmToList(unsigned int vm)
{

    assert(std::find(m_vms.begin(), m_vms.end(), vm) == m_vms.end());
    m_vms.push_back(vm);

    char vmText[256];
    sprintf(vmText, "0x%08x", vm);

    m_vmList->Append(vmText);
    
}

void MainFrame::RemoveVmFromList(unsigned int vm)
{

    std::vector<unsigned int>::iterator iterator;
    iterator = std::find(m_vms.begin(), m_vms.end(), vm);

    if (iterator != m_vms.end())
    {
        unsigned int index = iterator - m_vms.begin();
        m_vmList->DeleteItem(index);
        m_vms.erase(iterator);
    }

}

void MainFrame::SetVmName(unsigned int vm, const wxString& name)
{

    std::vector<unsigned int>::iterator iterator;
    iterator = std::find(m_vms.begin(), m_vms.end(), vm);

    if (iterator != m_vms.end())
    {
        unsigned int index = iterator - m_vms.begin();
        m_vmList->SetString(index, name);
    }

}

bool MainFrame::SaveFile(OpenFile* file, bool promptForName)
{
                    
    wxLogNull logNo;

    wxFileName fileName = file->file->fileName;

    if (!fileName.IsOk())
    {
        // If the file name is not well formatted, ask the user to input one.
        // This can be the case for files opened automatically by the debugger
        // which have names assigned by the debugee.
        promptForName = true;
    }
    else if (!fileName.IsAbsolute())
    {

        // If this file has a relative file name, use that as the default when
        // displaying the Save As dialog. This happens when files are opened
        // in the debugger and have reasonable names.

        wxString workingDirectory = m_project->GetWorkingDirectory();
        workingDirectory.Trim().Trim(false);

        if (workingDirectory.IsEmpty())
        {
            workingDirectory = wxFileName(m_project->GetCommandLine()).GetPath();
        }

        fileName.MakeAbsolute(workingDirectory);
        promptForName = true;

    }

    wxString fullPath = fileName.GetFullPath();

    if (fullPath.IsEmpty() || promptForName)
    {
        

        wxString p = fileName.GetPath();

        fullPath = wxFileSelector(_("Save As"), fileName.GetPath(),
            file->file->GetDisplayName(), "", s_scriptExtensions, wxSAVE, this);

        if (fullPath.IsEmpty())
        {
            // User cancelled.
            return false;
        }

    }

    if (!file->edit->SaveFile(fullPath))
    {
        wxMessageBox(_("Error saving file"), s_applicationName, wxOK | wxICON_ERROR, this);
        return false;
    }

    file->file->fileName = fullPath;
    file->timeStamp = GetFileModifiedTime(fullPath);

    // Reparse the symbols for the file on save.
    m_symbolParser->QueueForParsing(file->file);

    m_fileHistory.AddFileToHistory(fullPath);

    // Set the save point which will force the tab name to be updated with the
    // current file name.
    file->edit->SetSavePoint();

    // The file name may have changed, so update the syntax coloring.
    UpdateSyntaxColoring(file);

    // Update the breakpoints.
    m_breakpointsWindow->UpdateBreakpoints();

    for (unsigned int t = 0; t < m_tools.size(); t++)
    {
        ExternalTool* currentTool = m_tools[t];
        if (currentTool->GetEvent() == "File Save")
        {
            RunTool(currentTool);
        }
    }
    
    return true;

}

void MainFrame::OpenDocuments(const wxArrayString& fileNames)
{
    for (unsigned int i = 0; i < fileNames.Count(); ++i)
    {
        OpenDocument(fileNames[i]);
    }
}

MainFrame::OpenFile* MainFrame::OpenDocument(const wxString& fileName)
{

    wxString param = fileName;
    long lineNumber = 0;

    // Check for a line number using the syntax File.lua:100
    int lineNumberPos = param.Find(':', true);
    if (lineNumberPos != wxNOT_FOUND)
    {
        if (param.SubString(lineNumberPos + 1, param.Length()).ToLong(&lineNumber))
        {
            param.Remove(lineNumberPos);
        }
        else
        {
            lineNumber = 0;
        }
    }

    OpenFile* openFile = NULL;

    // Check to see if the file is already open.
    for (unsigned int i = 0; i < m_openFiles.size(); ++i)
    {
        if (m_openFiles[i]->file->fileName.SameAs(param))
        {
            // Select it.
            int pageIndex = m_notebook->GetPageIndex(m_openFiles[i]->edit);
            m_notebook->SetSelection(pageIndex);
            openFile = m_openFiles[i];
            break;
        }
    }

    if (openFile == NULL)
    {

        // Check to see if the file is in the project.

        Project::File* file = m_project->GetFileForFileName(param);

        if (file == NULL)
        {

            // If the file doesn't actually exist, don't add it.
            if (!wxFileExists(param))
            {
                return NULL;
            }

            // The file does not exist in the project, so add it as a temporary file.
            file = m_project->AddTemporaryFile(param);
            UpdateForNewFile(file);

        }

        m_fileHistory.AddFileToHistory(param);
        openFile = OpenProjectFile(file);

    }

    if (openFile != NULL && lineNumber != 0)
    {
        GotoNewLine(openFile->edit, lineNumber - 1, true);
    }

    return openFile;

}

void MainFrame::CloseDocument(int page, bool promptForSave)
{

    CodeEdit* edit = m_openFiles[page]->edit;

    if (PreNotebookPageClose(page))
    {
        m_notebook->RemovePage(page);
        delete edit;
    }

}

void MainFrame::SaveAllFiles()
{
    for (unsigned int i = 0; i < m_openFiles.size(); ++i)
    {
        if (m_openFiles[i]->edit->GetModify())
        {
            SaveFile(m_openFiles[i], false);
        }
    }
}

void MainFrame::UpdateEditorOptions()
{
    
    for (unsigned int i = 0; i < m_openFiles.size(); ++i)
    {
        m_openFiles[i]->edit->SetFontColorSettings(m_fontColorSettings);
        m_openFiles[i]->edit->SetEditorSettings(m_editorSettings);
    }

    // Set the font of the output window to match the code editor.
    m_output->SetFontColorSettings(m_fontColorSettings);

    // Set the font of the watch window to match the code editor so
    // that non-ASCII text will be displayed properly.
    m_watch->SetValueFont( m_fontColorSettings.GetFont() );

}

void MainFrame::SetDefaultHotKeys()
{

    // Setup the hotkeys.

    m_keyBinder.SetShortcut(ID_FileNew,                     wxT("Ctrl+N"));
    m_keyBinder.SetShortcut(ID_FileSave,                    wxT("Ctrl+S"));
    m_keyBinder.SetShortcut(ID_EditUndo,                    wxT("Ctrl+Z"));
    m_keyBinder.SetShortcut(ID_EditRedo,                    wxT("Ctrl+Y"));

    m_keyBinder.SetShortcut(ID_EditCut,                     wxT("Ctrl+X"));
    m_keyBinder.SetShortcut(ID_EditCopy,                    wxT("Ctrl+C"));
    m_keyBinder.SetShortcut(ID_EditPaste,                   wxT("Ctrl+V"));
    
    // Removed for now until we can filter hotkeys so they don't interfer with
    // other windows.
    //m_keyBinder.SetShortcut(ID_EditDelete,                  "Del");

    m_keyBinder.SetShortcut(ID_EditSelectAll,               wxT("Ctrl+A"));
    m_keyBinder.SetShortcut(ID_EditFind,                    wxT("Ctrl+F"));
    m_keyBinder.SetShortcut(ID_EditReplace,                 wxT("Ctrl+H"));
    m_keyBinder.SetShortcut(ID_EditFindNext,                wxT("F3"));
    m_keyBinder.SetShortcut(ID_EditFindPrevious,            wxT("Shift+F3"));
    m_keyBinder.SetShortcut(ID_EditFindInFiles,             wxT("Ctrl+Shift+F"));

    m_keyBinder.SetShortcut(ID_ProjectAddExistingFile,      wxT("Shift+Alt+A"));
    m_keyBinder.SetShortcut(ID_ProjectAddNewFile,           wxT("Ctrl+Shift+A"));

    m_keyBinder.SetShortcut(ID_WindowWatch,                 wxT("Alt+3"));
    m_keyBinder.SetShortcut(ID_WindowCallStack,             wxT("Alt+7"));
    m_keyBinder.SetShortcut(ID_WindowBreakpoints,           wxT("Alt+F9"));
    m_keyBinder.SetShortcut(ID_WindowNextDocument,          wxT("Ctrl+Tab"));
    m_keyBinder.SetShortcut(ID_WindowPreviousDocument,      wxT("Shift+Ctrl+Tab"));
    m_keyBinder.SetShortcut(ID_WindowClose,                 wxT("Ctrl+F4"));

    m_keyBinder.SetShortcut(ID_DebugStart,                  wxT("F5"));
    m_keyBinder.SetShortcut(ID_DebugStartWithoutDebugging,  wxT("Ctrl+F5"));
    m_keyBinder.SetShortcut(ID_DebugStepInto,               wxT("F11"));
    m_keyBinder.SetShortcut(ID_DebugStepOver,               wxT("F10"));
    m_keyBinder.SetShortcut(ID_DebugQuickWatch,             wxT("Shift+F9"));
    m_keyBinder.SetShortcut(ID_DebugToggleBreakpoint,       wxT("F9"));
    m_keyBinder.SetShortcut(ID_DebugDeleteAllBreakpoints,   wxT("Ctrl+Shift+F9"));

    m_keyBinder.SetShortcut(ID_MiscOpenInFilter,            wxT("Ctrl+Q"));

}

void MainFrame::FindNext(const wxString& text, int flags)
{

    int pageIndex = GetSelectedPage();

    if (pageIndex != -1)
    {
        OpenFile* file = m_openFiles[pageIndex];
        FindText(file, text, flags);
    }

}

void MainFrame::UpdateStatusBarLineAndColumn()
{

    // Update the line and column number in the status bar.

    int pageIndex = GetSelectedPage();

    if (pageIndex != -1)
    {

        CodeEdit* edit = m_openFiles[pageIndex]->edit;

        int pos  = edit->GetCurrentPos();
        int line = edit->LineFromPosition(pos) + 1;
        int col  = edit->GetColumn(pos) + 1;

        m_statusBar->SetStatusText(wxString::Format("Ln %d", line), 1);
        m_statusBar->SetStatusText(wxString::Format("Col %d", col), 2);

    }
    else
    {

        // Clear out the line and column indicator.
        m_statusBar->SetStatusText(_(""), 1);
        m_statusBar->SetStatusText(_(""), 2);

    }

}

void MainFrame::ToggleBreakpoint(Project::File* file, unsigned int newLine)
{

    unsigned int openFileIndex = GetOpenFileIndex(file);
    OpenFile* openFile = NULL;

    if (openFileIndex != -1)
    {
        openFile = m_openFiles[openFileIndex];
    }

    if (openFile != NULL)
    {

        // Make sure we're not past the end of the document. Note Scintlla adds one extra line at the line
        // that doesn't have text. We don't allow breakpoints on that line since we don't get deletion
        // notifications for that line.
        if (newLine >= static_cast<unsigned int>(openFile->edit->GetLineCount()))
        {
            if (openFile->edit->GetLine(newLine - 1).length() == 0)
            {
                return;
            }
        }
    
    }

    unsigned int scriptIndex = file->scriptIndex;

    if (scriptIndex != -1)
    {
        
        // Make from old lines to new lines in case the user has changed the file.
        unsigned int oldLine = NewToOldLine(file, newLine);

        // If the line exists in the backend, set it through the debugger. Otherwise
        // we'll just set it "locally".
        if (oldLine != LineMapper::s_invalidLine)
        {
            DebugFrontend::Get().ToggleBreakpoint(m_vm, scriptIndex, oldLine);
            return;
        }

    }

    // This file does not have a counterpart in the script debugger, so just
    // set the break point "locally". When the debugger encounters the file
    // we'll send it the break points.
    bool set = m_project->ToggleBreakpoint(file, newLine);
    m_breakpointsWindow->UpdateBreakpoints();
    
    if (openFile != NULL)
    {
        UpdateFileBreakpoint(openFile, newLine, set);
    }

}

void MainFrame::DeleteAllBreakpoints()
{

    if (wxMessageBox(_("Do you want to delete all breakpoints?"), s_applicationName, wxYES_NO | wxICON_EXCLAMATION, this) != wxYES)
    {
        return;
    }

    for (unsigned int fileIndex = 0; fileIndex < m_project->GetNumFiles(); ++fileIndex)
    {

        Project::File* file = m_project->GetFile(fileIndex);

        unsigned int openFileIndex = GetOpenFileIndex(file);
        OpenFile* openFile = NULL;

        if (openFileIndex != -1)
        {
            openFile = m_openFiles[openFileIndex];
        }

        unsigned int scriptIndex = file->scriptIndex;

        for (unsigned int breakpointIndex = 0; breakpointIndex < file->breakpoints.size(); ++breakpointIndex)
        {

            unsigned int newLine = file->breakpoints[breakpointIndex];

            // This file does not have a counterpart in the script debugger, so just
            // set the break point "locally". When the debugger encounters the file
            // we'll send it the break points.
    
            if (openFile != NULL)
            {
                UpdateFileBreakpoint(openFile, newLine, false);
            }

        }

        m_project->DeleteAllBreakpoints(file);
    }

    DebugFrontend::Get().RemoveAllBreakPoints(0);

    m_breakpointsWindow->UpdateBreakpoints();

}

void MainFrame::CheckReload()
{
    for (unsigned int i = 0; i < m_openFiles.size(); ++i)
    {
        CheckReload(m_openFiles[i]);
    }
}

void MainFrame::CheckReload(OpenFile* file)
{

    wxString fileName = file->file->fileName.GetFullPath();
    time_t modifiedTimeStamp = GetFileModifiedTime(fileName);

    if (modifiedTimeStamp > file->timeStamp)
    {

        // Update the time stamp even if we didn't reload so that we don't keep
        // asking the user if they want to reload the file. This is done first
        // since the dismissing the dialog box will create an activate event.
        file->timeStamp = modifiedTimeStamp;

        wxString message;
        message.Printf("File '%s' has been changed. Reload from disk?", fileName.ToAscii());

        if (wxMessageBox(message, s_applicationName, wxYES_NO | wxICON_EXCLAMATION, this) == wxYES)
        {
            ReloadFile(file);
        }

    }

}

void MainFrame::OnActivate(wxActivateEvent& event)
{

    if (event.GetActive())
    {
        CheckReload();
    }

    event.Skip();

}

bool MainFrame::OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& fileNames)
{

    // Check to see if there's a project file.

    wxArrayString   trimmedFileNames;
    wxString        projectFileName;
    wxString        exeFileName;

    for (unsigned int i = 0; i < fileNames.Count(); ++i)
    {

        wxFileName fileName(fileNames[i]);
        wxString extension = fileName.GetExt().Lower();

        if (extension == wxT("deproj"))
        {
            projectFileName = fileNames[i];
        }
        else if (extension == wxT("exe"))
        {
            exeFileName = fileNames[i];
        }
        else
        {
            trimmedFileNames.Add(fileNames[i]);
        }

    }

    if (DebugFrontend::Get().GetState() == DebugFrontend::State_Inactive)
    {
        if (!projectFileName.IsEmpty())
        {
            if (SaveProjectIfNeeded())
            {
                OpenProject(projectFileName, true);
            }    
        }
        else if (!exeFileName.IsEmpty())
        {
            if (SaveProjectIfNeeded())
            {
                NewProject();
                m_project->SetCommandLine(exeFileName);
            }    
        }
    }

    OpenDocuments(trimmedFileNames);
    return true;

}

void MainFrame::OnContextCheckOut(wxCommandEvent& event)
{

    std::vector<std::string> fileNames;
    GetProjectSelectedFileNames(fileNames);

    m_sourceControl.CheckOut(fileNames, NULL);
    UpdateProjectFileStatus();

}

void MainFrame::OnContextCheckIn(wxCommandEvent& event)
{

    std::vector<std::string> fileNames;
    GetProjectSelectedFileNames(fileNames);

    m_sourceControl.CheckIn(fileNames, NULL);
    UpdateProjectFileStatus();

}

void MainFrame::OnContextUndoCheckOut(wxCommandEvent& event)
{

    std::vector<std::string> fileNames;
    GetProjectSelectedFileNames(fileNames);

    m_sourceControl.UndoCheckOut(fileNames);
    UpdateProjectFileStatus();

}

void MainFrame::OnContextDiff(wxCommandEvent& event)
{

    std::vector<std::string> fileNames;
    GetProjectSelectedFileNames(fileNames);

    if (fileNames.size() > 0)
    {
        m_sourceControl.ShowDiff(fileNames[0].c_str());
    }

}

void MainFrame::OnContextShowHistory(wxCommandEvent& event)
{

    std::vector<std::string> fileNames;
    GetProjectSelectedFileNames(fileNames);

    m_sourceControl.ShowHistory(fileNames);

}

void MainFrame::OnContextRemove(wxCommandEvent& event)
{

    std::vector<Project::File*> selectedFiles;
    m_projectExplorer->GetSelectedFiles(selectedFiles);

    for (unsigned int i = 0; i < selectedFiles.size(); ++i)
    {
        DeleteProjectFile(selectedFiles[i]);
    }
    
}

void MainFrame::OnUpdateContextCheckOut(wxUpdateUIEvent& event)
{

    // Check to see if any of the selected files are checked in.

    std::vector<Project::File*> selectedFiles;
    m_projectExplorer->GetSelectedFiles(selectedFiles);
    
    bool allowed = false;

    for (unsigned int i = 0; i < selectedFiles.size() && !allowed; ++i)
    {
        if (selectedFiles[i]->status == Project::Status_CheckedIn)
        {
            allowed = true;
        }
    }

    event.Enable(allowed);

}

void MainFrame::OnUpdateContextShowHistory(wxUpdateUIEvent& event)
{

    // Check to see if any of the selected files are under source control.

    std::vector<Project::File*> selectedFiles;
    m_projectExplorer->GetSelectedFiles(selectedFiles);
    
    bool allowed = false;

    for (unsigned int i = 0; i < selectedFiles.size() && !allowed; ++i)
    {
        if (selectedFiles[i]->status != Project::Status_None)
        {
            allowed = true;
        }
    }

    event.Enable(allowed);

}

void MainFrame::OnUpdateContextCheckIn(wxUpdateUIEvent& event)
{

    // Check to see if any of the selected files are checked out.

    std::vector<Project::File*> selectedFiles;
    m_projectExplorer->GetSelectedFiles(selectedFiles);
    
    bool allowed = false;

    for (unsigned int i = 0; i < selectedFiles.size() && !allowed; ++i)
    {
        if (selectedFiles[i]->status == Project::Status_CheckedOut)
        {
            allowed = true;
        }
    }

    event.Enable(allowed);

}

void MainFrame::OnThreadExit(ThreadEvent& event)
{

    // Copy all of the file statuses into a hash table so that we can sync them up with
    // the project files in linear time.

    FileStatusThread* thread = m_fileStatusThread[0];
    stdext::hash_map<std::string, SourceControl::Status> statusMap;

    for (unsigned int i = 0; i < thread->GetNumFiles(); ++i)
    {
        statusMap.insert(std::make_pair(thread->GetFileName(i), thread->GetFileStatus(i)));
    }

    for (unsigned int i = 0; i < m_project->GetNumFiles(); ++i)
    {

        Project::File* file = m_project->GetFile(i);

        stdext::hash_map<std::string, SourceControl::Status>::iterator iterator;
        iterator = statusMap.find(std::string(file->fileName.GetFullPath()));

        if (iterator != statusMap.end())
        {
            SourceControl::Status status = iterator->second;
            SetFileStatus(file, status);
        }

    }

    m_projectExplorer->UpdateFileImages();

    m_fileStatusThread[0]->Wait();
    delete m_fileStatusThread[0];

    m_fileStatusThread[0] = m_fileStatusThread[1];

    m_fileStatusThread[1] = NULL;
    
    // Kick off the next update.
    if (m_fileStatusThread[0] != NULL)
    {
        m_fileStatusThread[0]->Create();
        m_fileStatusThread[0]->Run();
    }

}

void MainFrame::OnShowHelp(wxCommandEvent& event)
{
    const wxString& section = event.GetString();
    m_helpController.DisplaySection(section);
}

void MainFrame::OnUpdateInfo(wxCommandEvent& event)
{
    HandleUpdate();
}

void MainFrame::GetProjectSelectedFileNames(std::vector<std::string>& fileNames) const
{

    std::vector<Project::File*> selectedFiles;
    m_projectExplorer->GetSelectedFiles(selectedFiles);

    for (unsigned int i = 0; i < selectedFiles.size(); ++i)
    {

        Project::File* file = selectedFiles[i];
        
        if (file->fileName.IsOk())
        {
            fileNames.push_back(std::string(file->fileName.GetFullPath()));
        }

    }

}

void MainFrame::GetNotebookTabSelectedFileNames(std::vector<std::string>& fileNames) const
{

    int page = GetSelectedPage();

    if (page != -1)
    {
        fileNames.push_back(std::string(m_openFiles[page]->file->fileName.GetFullPath()));
    }

}

void MainFrame::ReloadFile(OpenFile* file)
{

    CodeEdit& editor = *file->edit;
    unsigned int oldScrollPos = std::max(editor.GetScrollPos(wxVSCROLL), 0);

    //Disable modified events so OnCodeEditModified is not called
    editor.SetModEventMask(0);

    editor.LoadFile(file->file->fileName.GetFullPath());
    file->timeStamp = GetFileModifiedTime(file->file->fileName.GetFullPath());

    editor.SetModEventMask(wxSCI_MODEVENTMASKALL);
    
    unsigned int newLineCount = editor.GetLineCount();
    
    std::vector<unsigned int>& breakpoints = file->file->breakpoints;
    
    //Erase breakpoints past the end of the file and re-add the markers for the rest
    for (std::vector<unsigned int>::iterator it = breakpoints.begin(); it != breakpoints.end() ;)
    {
        if (*it >= newLineCount)
        {
          it = breakpoints.erase(it);
        }
        else
        {
          UpdateFileBreakpoint(file, *it, true);
          it++;
        }
    }

    m_breakpointsWindow->UpdateBreakpoints(file->file);

    //Scroll the editor back to the same line we were before the reload
    editor.ScrollToLine(oldScrollPos < newLineCount ? oldScrollPos : newLineCount);
}

void MainFrame::FindInFiles(const wxString& text, const wxArrayString& fileNames, bool matchCase, bool matchWholeWord, const wxString& baseDirectory)
{

    wxString caseText = text;

    if (!matchCase)
    {
        caseText.MakeLower();
    }

    unsigned int numMatches = 0;
    unsigned int numMatchingFiles = 0;

    wxString messages;
    
    for (unsigned int i = 0; i < fileNames.Count(); ++i)
    {

        FILE* file = fopen(fileNames[i].c_str(), "rb");
        if (file == NULL)
        {
            messages.Append("Error: Couldn't open \'" + fileNames[i] + "\'\n");
            continue;
        }

        fseek(file, 0, SEEK_END);
        size_t length = ftell(file);
        fseek(file, 0, SEEK_SET);

        char* buffer = static_cast<char*>(malloc(length + 1));
        length = fread(buffer, 1, length, file);
        buffer[length] = 0;
        fclose(file);
                
        if (length > 0)
        {
            wxStringInputStream input(buffer);
            wxTextInputStream textInput( input );

            // Do a line by line search.

            unsigned int lineNumber = 1;
            bool hasMatch = false;
            
            while (input.IsOk())
            {

                wxString line = textInput.ReadLine();
                wxString caseLine = line;

                if (!matchCase)
                {
                    caseLine.MakeLower();
                }

                if (FindInLine(caseText, caseLine, matchWholeWord))
                {

                    wxFileName fileName = fileNames[i];

                    if (!baseDirectory.IsEmpty())
                    {
                        fileName.MakeRelativeTo(baseDirectory);
                    }

                    messages.Append(wxString::Format("%s:%d: %s\n",
                                    fileName.GetFullPath().ToAscii(), lineNumber, line.ToAscii()));
                    
                    ++numMatches;
                    hasMatch = true;
                
                }

                ++lineNumber;

            }

            if (hasMatch)
            {
                ++numMatchingFiles;
            }

        }
        else
        {
            messages.Append("Error: Couldn't open \'" + fileNames[i] + "\'\n");
        }

        free(buffer);

    }

    // Output some statistics.
    m_searchWindow->SearchMessage(messages);
    m_searchWindow->SearchMessage(wxString::Format("Total found: %d\tMatching files: %d\tTotal files searched: %d",
        numMatches, numMatchingFiles, fileNames.Count()));

}

bool MainFrame::FindInLine(const wxString& text, const wxString& line, bool matchWholeWord) const
{
    if (matchWholeWord)
    {

        int start = line.find(text);

        while (start != wxNOT_FOUND)
        {

            int end = start + text.Length();

            // Check if the characters before and after the text are separators.

            bool sepBefore = start == 0 || IsSpace(line[start - 1]) || IsSymbol(line[start - 1]);
            bool sepAfter  = end == line.Length() || IsSpace(line[end]) || IsSymbol(line[end]);

            if (sepBefore && sepAfter)
            {
                return true;
            }

            // Keep searching.
            start = line.find(text, start + 1);
        
        }

        return false;

    }
    else
    {
        return line.Find(text) != wxNOT_FOUND;
    }
}

time_t MainFrame::GetFileModifiedTime(const wxString& fileName) const
{
    if (wxFileExists(fileName))
    {
        return wxFileModificationTime(fileName);
    }
    else
    {
        return 0;
    }
}

void MainFrame::CleanUpTemporaryFiles()
{

    std::vector<Project::File*> files;

    for (unsigned int i = 0; i < m_project->GetNumFiles(); ++i)
    {

        Project::File* file = m_project->GetFile(i);

        if (file->temporary && GetOpenFileIndex(file) == -1)
        {
            files.push_back(file);
        }

    }

    m_projectExplorer->RemoveFiles(files);

    for (unsigned int i = 0; i < files.size(); ++i)
    {
        m_project->RemoveFile(files[i]);
    }

    m_breakpointsWindow->UpdateBreakpoints();

}

unsigned int MainFrame::GetOpenFileIndex(Project::File* file) const
{

    for (unsigned int i = 0; i < m_openFiles.size(); ++i)
    {
        if (m_openFiles[i]->file == file)
        {
            return i;
        }
    }

    return -1;

}

void MainFrame::StartProcess(bool debug, bool startBroken)
{

    wxString command;
    wxString commandArguments;
    wxString workingDirectory;
    wxString symbolsDirectory;

#ifndef DEDICATED_PRODUCT_VERSION

    // Check to see if the project has a startup application associated with it.

    if (m_project->GetCommandLine().IsEmpty())
    {
        if (!ShowProjectSettingsDialog())
        {
            // User cancelled, so just skip the startup.
            return;
        }
    }

    command          = m_project->GetCommandLine();
    workingDirectory = m_project->GetWorkingDirectory();
    symbolsDirectory = m_project->GetSymbolsDirectory();
    commandArguments = m_project->GetCommandArguments();

#else

    command          = GetDedicatedProductCommand();
    commandArguments = GetDedicatedProductCommandArguments(m_project->GetCommandArguments());

#endif

    if (!command.IsEmpty())
    {
        StartProcess(command, commandArguments, workingDirectory, symbolsDirectory, debug, startBroken);
    }
    
}

void MainFrame::StartProcess(const wxString& command, const wxString& commandArguments, const wxString& workingDirectory, const wxString& symbolsDirectory, bool debug, bool startBroken)
{

    m_output->Clear();
    
    // Save all of the open files, like MSVC does.
    SaveAllFiles();

    if (!DebugFrontend::Get().Start(command, commandArguments, workingDirectory, symbolsDirectory, debug, startBroken))
    {
        wxMessageBox("Error starting process", s_applicationName, wxOK | wxICON_ERROR, this);
    }
    else if (debug)
    {
        SetMode(Mode_Debugging);
        m_output->OutputMessage("Debugging session started");
        if (m_attachToHost)
        {
            DebugFrontend::Get().AttachDebuggerToHost();
        }
    }

}

void MainFrame::EnableWhenBroken(wxUpdateUIEvent& event)
{
    bool running = (DebugFrontend::Get().GetState() == DebugFrontend::State_Broken);
    event.Enable(running);
}

void MainFrame::EnableWhenInactive(wxUpdateUIEvent& event)
{
    bool inactive = (DebugFrontend::Get().GetState() == DebugFrontend::State_Inactive);
    event.Enable(inactive);
}

void MainFrame::EnableWhenFileIsOpen(wxUpdateUIEvent& event)
{
    int pageIndex = GetSelectedPage();
    event.Enable(pageIndex != -1);
}

void MainFrame::EnableWhenFileHasFocus(wxUpdateUIEvent& event)
{
    
    int pageIndex = GetSelectedPage();

    if (pageIndex != -1 && m_openFiles[pageIndex]->edit->GetSCIFocus())
    {
        event.Enable(true);
    }
    else
    {
        event.Enable(false);
    }
   
}

void MainFrame::EnableWhenTextIsSelected(wxUpdateUIEvent& event)
{
    
    bool selected = false;

    int pageIndex = GetSelectedPage();

    if (pageIndex != -1 && m_openFiles[pageIndex]->edit->GetSCIFocus())
    {
        int length = m_openFiles[pageIndex]->edit->GetSelectionEnd() - m_openFiles[pageIndex]->edit->GetSelectionStart();
        selected = length > 0;
    }

    event.Enable(selected);

}

int MainFrame::GetSelectedPage() const
{

    if (m_notebook->GetPageCount() > 0)
    {
        const wxWindow* selection = m_notebook->GetPage(m_notebook->GetSelection());
        
        for (unsigned int i = 0; i < m_openFiles.size(); ++i)
        {
            if (m_openFiles[i]->edit == selection)
            {
                return i;
            }
        }
    }

    return -1;

}

void MainFrame::RemoveAllLocalBreakpoints(Project::File* file)
{

    unsigned int openIndex = GetOpenFileIndex(file);

    if (openIndex != -1)
    {
        for (unsigned int i = 0; i < file->breakpoints.size(); ++i)
        {
            UpdateFileBreakpoint(m_openFiles[openIndex], file->breakpoints[i], false);
        }
    }
    
    file->breakpoints.clear();
    m_breakpointsWindow->UpdateBreakpoints(file);

}

bool MainFrame::InitializeSourceControl()
{

    m_sourceControl.Shutdown();

    if (!m_project->GetSccProvider().IsEmpty())
    {
        if (!m_sourceControl.Initialize(m_project->GetSccProvider(), static_cast<HWND>(GetHandle())))
        {
            m_output->OutputError("Error: Couldn't initialize source control provider");
            return false;
        }
        if (!m_sourceControl.OpenProject(m_project->GetSccUser(), m_project->GetSccProjectName(), m_project->GetSccLocalPath(), m_project->GetSccAuxProjectPath()))
        {
            m_sourceControl.Shutdown();
            m_output->OutputError("Error: Couldn't open source control project");
            return false;
        }
    }

    UpdateProjectFileStatus();
    return true;

}

void MainFrame::UpdateProjectFileStatus()
{

    if (m_sourceControl.GetIsInitialized())
    {

        // Check if we've already queued up a thread.

        FileStatusThread* thread = NULL;

        if (m_fileStatusThread[0] == NULL)
        {
            thread = new FileStatusThread(this, m_sourceControl);
            m_fileStatusThread[0] = thread;
        }
        else if (m_fileStatusThread[1] == NULL)
        {
            thread  = new FileStatusThread(this, m_sourceControl);
            m_fileStatusThread[1] = thread;
        }
        else
        {
            thread = m_fileStatusThread[1];
        }

        for (unsigned int i = 0; i < m_project->GetNumFiles(); ++i)
        {
            Project::File* file = m_project->GetFile(i);
            thread->AddFileName(file->fileName.GetFullPath());
        }

        if (thread == m_fileStatusThread[0])
        {
            thread->Create();
            thread->Run();
        }

    }

}

void MainFrame::UpdateProjectFileStatus(Project::File* file)
{

    if (m_sourceControl.GetIsInitialized())
    {
        
        SourceControl::Status status = m_sourceControl.GetFileStatus(file->fileName.GetFullPath());
        SetFileStatus(file, status);

        m_projectExplorer->UpdateFileImages();
    
    }

}

void MainFrame::DebugExe(const wxString& fileName)
{

    // If the project already is setup to use this file, don't override it since
    // there may be useful info in the working directory or command line options.

    wxFileName temp(fileName);

    if (temp.SameAs(m_project->GetCommandLine()))
    {
        StartProcess(true, false);
    }
    else
    {
        StartProcess(fileName, "", "", "", true, false);
    }

}

void MainFrame::ShowOutputWindow()
{
    m_mgr.GetPane(m_output).Show();
    m_mgr.Update();
}

void MainFrame::ShowWatchWindow()
{
    m_mgr.GetPane(m_watch).Show();
    m_mgr.Update();
}

void MainFrame::ShowSearchWindow()
{
    m_mgr.GetPane(m_searchWindow).Show();
    m_mgr.Update();
}

void MainFrame::SetMode(Mode mode)
{

    if (m_mode != mode)
    {

        // Save off the layout for the current mode.
        m_modeLayout[m_mode] = m_mgr.SavePerspective();

        // Load up the layout for the new mode.
        m_mode = mode;
        m_mgr.LoadPerspective(m_modeLayout[m_mode]);

    }

}

bool MainFrame::MSWProcessMessage(WXMSG* pMsg)
{

    MSG* msg = static_cast<MSG*>(pMsg);

    if (msg->message == m_openFilesMessage)
    {

        // Handle the special message other instances send to us to tell us to
        // open files.

        wxString command = wxGetApp().GetInstanceCommandLine();
        
        wxStringInputStream stream(command);
        wxTextInputStream text(stream, ";");
        
        while (!stream.Eof())
        {
            wxString fileName = text.ReadWord();
            if (!fileName.IsEmpty())
            {
                OpenDocument(fileName);
            }
        }

        // Restore the window if it's minimized.
        Maximize(false);
        Show(true);

        return true;

    }
    else
    {
        return wxFrame::MSWProcessMessage(pMsg);
    }

}

wxString MainFrame::GetAppDataDirectory() const
{
    TCHAR path[_MAX_PATH];
    SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, path);
    strcat(path, "\\Decoda\\");
    return path;
}

bool MainFrame::ShowProjectSettingsDialog()
{
  
    NewProcessDialog dialog(this);

    dialog.CenterOnParent();

    dialog.SetCommandLine(m_project->GetCommandLine());
    dialog.SetCommandArguments(m_project->GetCommandArguments());
    dialog.SetWorkingDirectory(m_project->GetWorkingDirectory());
    dialog.SetSymbolsDirectory(m_project->GetSymbolsDirectory());

    dialog.SetSccProvider(m_project->GetSccProvider());
    dialog.SetSccUser(m_project->GetSccUser());
    dialog.SetSccProjectName(m_project->GetSccProjectName());
    dialog.SetSccLocalPath(m_project->GetSccLocalPath());
    dialog.SetSccAuxProjectPath(m_project->GetSccAuxProjectPath());

    if (dialog.ShowModal() == wxID_OK)
    {

        m_project->SetCommandLine(dialog.GetCommandLine());
        m_project->SetCommandArguments(dialog.GetCommandArguments());
        m_project->SetWorkingDirectory(dialog.GetWorkingDirectory());
        m_project->SetSymbolsDirectory(dialog.GetSymbolsDirectory());
    
        m_project->SetSccProvider(dialog.GetSccProvider());
        m_project->SetSccUser(dialog.GetSccUser());
        m_project->SetSccProjectName(dialog.GetSccProjectName());
        m_project->SetSccLocalPath(dialog.GetSccLocalPath());
        m_project->SetSccAuxProjectPath(dialog.GetSccAuxProjectPath());

        InitializeSourceControl();
        return true;
    
    }

    return false;

}

void MainFrame::UpdateDocumentReadOnlyStatus()
{

    int pageIndex = GetSelectedPage();

    if (pageIndex != -1)
    {

        OpenFile* file = m_openFiles[pageIndex];
        bool writeable = true;

        if (!file->file->state == CodeState_Normal)
        {
            writeable = false;
        }
        else if (wxFileExists(file->file->fileName.GetFullPath()))
        {
            writeable = file->file->fileName.IsFileWritable();
        }

        file->edit->SetReadOnly(!writeable);
    
    }

}

void MainFrame::UpdateCallback(Updater* updater, void* param)
{

    MainFrame* mainFrame = reinterpret_cast<MainFrame*>(param);

    wxCommandEvent event( wxEVT_UPDATE_INFO_EVENT );
    mainFrame->ProcessEvent( event );

}

void MainFrame::HandleUpdate()
{

    if (m_updater.ShowUpdateNotice(GetHandle()))
    {

        // Show the downloading progress dialog.
        if (!m_updater.DownloadUpdate(GetHandle()))
        {
            return;
        }

        if (m_updater.InstallUpdate())
        { 
            // Close the app so the updater will start.
            Close();
        }

    }

}

void MainFrame::UpdateSyntaxColoring(OpenFile* openFile)
{

    Project::File* file = openFile->file;

    if (file->GetFileType().CmpNoCase("lua") == 0 && file->state == CodeState_Normal)
    {
        openFile->edit->SetLuaLexer();
    }
    else
    {
        openFile->edit->SetDefaultLexer();
    }

    openFile->edit->Recolor();

}

bool MainFrame::PreNotebookPageClose(int page, bool promptForSave)
{

    OpenFile* openFile = m_openFiles[page];

    // Check if we need to save the document.

    Project::File* file = openFile->file;

    if (openFile->edit->GetModify() && promptForSave)
    {

        wxString message;
        message.Printf("Save changes to \'%s\'?", file->GetDisplayName());

        int result = wxMessageBox(message, s_applicationName, wxYES_NO | wxCANCEL | wxICON_QUESTION, this);

        if (result == wxCANCEL)
        {
            return false;
        }
        else if (result == wxYES)
        {
            if (!SaveFile(openFile, false))
            {
                // Since saving failed, don't close the file.
                return false;
            }
        }
        else if (result == wxNO)
        {

            // Undo the text modifications to the file so that we'll appropriate update
            // any data like breakpoints, etc.
            
            openFile->edit->Freeze();
            
            while (openFile->edit->CanUndo())
            {
                openFile->edit->Undo();
            }

            openFile->edit->Thaw();
        
        }

    }

    // Remove the file associate with the page.
    m_openFiles.erase(m_openFiles.begin() + page);
    delete openFile;

    if (page == m_notebook->GetSelection())
    {
        // We can't call UpdateStatusBarLineAndColumn from here since the selection
        // in the notebook is currently wrong (hasn't been updated). So just clear
        // out the line and column indicator.
        m_statusBar->SetStatusText(_(""), 1);
        m_statusBar->SetStatusText(_(""), 2);
    }

    // If the file is temporary and wasn't opened automatically during debugging,
    // remove it from the project.
    if (file->temporary && file->scriptIndex == -1)
    {
        m_projectExplorer->RemoveFile(file);
        m_project->RemoveFile(file);
        m_breakpointsWindow->RemoveFile(file);
    }

    // Update the tab order
    RemovePageFromTabOrder(page);

    return true;

}

void MainFrame::ApplyWindowPlacement()
{

    WINDOWPLACEMENT placement;

    placement.length = sizeof(placement);
    placement.flags  = 0;

    if (m_startUpMaximized)
    {
        placement.showCmd = SW_SHOWMAXIMIZED;
    }
    else
    {
        placement.showCmd = SW_SHOW;
    }

    placement.ptMinPosition.x = -1;
    placement.ptMinPosition.y = -1;
    placement.ptMaxPosition.x = -1;
    placement.ptMaxPosition.y = -1;

    placement.rcNormalPosition.left   = m_startUpRect.x;
    placement.rcNormalPosition.top    = m_startUpRect.y;
    placement.rcNormalPosition.right  = m_startUpRect.x + m_startUpRect.width;
    placement.rcNormalPosition.bottom = m_startUpRect.y + m_startUpRect.height;

    SetWindowPlacement( static_cast<HWND>(GetHandle()), &placement);

}

void MainFrame::UpdateStartUpPlacement()
{

    WINDOWPLACEMENT placement;

    placement.length = sizeof(WINDOWPLACEMENT);
    placement.flags  = 0;
    GetWindowPlacement( static_cast<HWND>(GetHandle()), &placement);

    m_startUpRect.x      = placement.rcNormalPosition.left;
    m_startUpRect.y      = placement.rcNormalPosition.top;
    m_startUpRect.width  = placement.rcNormalPosition.right - placement.rcNormalPosition.left;
    m_startUpRect.height = placement.rcNormalPosition.bottom - placement.rcNormalPosition.top;
    m_startUpMaximized   = (placement.showCmd == SW_MAXIMIZE);

}

void MainFrame::BringToFront()
{
    
    Raise();

#ifdef __WXMSW__
    // Make the window the top most temporarily since we can't make ourself
    // a foreground window in Windows anymore (just flashes the toolbar)
    HWND hWnd = static_cast<HWND>(GetHandle());
    SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
    SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
#endif

    // If we're debugging a game, they may have restricted the mouse to their
    // client area which makes it hard to use the debugger, so reset that.
#ifdef __WXMSW__
    ShowCursor(TRUE);
    ClipCursor(NULL);
#endif

}

Project::File* MainFrame::GetFileMatchingSource(const wxFileName& fileName, const std::string& source) const
{

    for (unsigned int i = 0; i < m_project->GetNumFiles(); ++i)
    {

        Project::File* file = m_project->GetFile(i);
        
        if (file->scriptIndex == -1 && file->fileName.GetFullName().CmpNoCase(fileName.GetFullName()) == 0)
        {
            return file;
        }

    }

    return NULL;

}

void MainFrame::AutoOpenLastProject()
{

    if (m_editorSettings.GetLoadLastProjectOnStartup() && !m_lastProjectLoaded.IsEmpty())
    {
        if (!OpenProject(m_lastProjectLoaded, false))
        {
            // If we couldn't load the project, don't try to load it the next
            // time we statup.
            m_lastProjectLoaded.Empty();
        }
    }

}

void MainFrame::OnSymbolsParsed(SymbolParserEvent& event)
{

    unsigned int fileId = event.GetFileId();
    Project::File* file = m_project->GetFileById(fileId);

    //If we are batch loading files, wait for the final symbol parse of that
    //batch before adding symbol data to tree
    if (m_waitForFinalSymbolParse)
    {
        if (!event.GetIsFinalQueueItem())
        {
            //Don't add symbol data yet.
            return;
        }
        else
        {
            //Batch loading is done
            m_waitForFinalSymbolParse = false;
            m_statusBar->SetStatusText("", 0);

            //Set the project to itself so we can trigger Rebuild()
            m_projectExplorer->SetProject(m_project);
            
            return;
        }
    }

    if (file != NULL)
    {
        m_projectExplorer->UpdateFile(file);
    }

    m_autoCompleteManager.BuildFromProject(m_project);

}

void MainFrame::UpdateForNewFile(Project::File* file)
{
    m_projectExplorer->InsertFile(file);
    m_symbolParser->QueueForParsing(file);
    m_autoCompleteManager.BuildFromProject(m_project);
}

void MainFrame::SetFileStatus(Project::File* file, SourceControl::Status status)
{
    if (status == SourceControl::Status_CheckedOutByUser)
    {
        file->status = Project::Status_CheckedOut;
    }
    else if (status == SourceControl::Status_CheckedIn ||
             status == SourceControl::Status_CheckedOut)
    {
        file->status = Project::Status_CheckedIn;
    }
    else
    {
        file->status = Project::Status_None;
    }
}

const wxString& MainFrame::GetApplicationName()
{
    return s_applicationName;
}
