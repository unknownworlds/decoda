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

#ifndef SOURCE_CONTROL_H
#define SOURCE_CONTROL_H

#include "CriticalSection.h"

#include <windows.h>
#include "scc.h"

#include <vector>
#include <string>
#include <queue>

/**
 * This class provides integration with Microsoft SCC plugins.
 */
class SourceControl
{

public:

    enum Status
    {
        Status_Error,
        Status_None,
        Status_CheckedOut,
        Status_CheckedOutByUser,
        Status_CheckedIn,
    };

    /**
     * Constructor.
     */
    SourceControl();

    /**
     * Destructor.
     */
    virtual ~SourceControl();

    /**
     * Returns the number of source control providers installed.
     */
    static unsigned int GetNumProviders();

    /**
     * Returns the name of the ith source control provider.
     */
    static const char* GetProviderName(unsigned int i);

    /**
     * Initializes the source control system with the specified provider. The name
     * should be one of the names returned by GetProviderName.
     */
    bool Initialize(const char* provider, HWND hWnd);

    /**
     * Initializes the source control system with the specified provider. The the index
     * is the index in the provider list.
     */
    bool Initialize(unsigned int provider, HWND hWnd);

    /**
     * Unloads the plugin (if one was loaded).
     */
    void Shutdown();

    /**
     * Returns true if the source control provider has been initialized.
     */
    bool GetIsInitialized() const;

    /**
     * Invokes the source control plugin's browse for project UI.
     */
    bool SelectProject();

    /**
     * Returns the name of the source control provider.
     */
    const char* GetName() const;

    /**
     * Opens a source control project.
     */
    bool OpenProject(const char* user, const char* projName, const char* localPath, const char* auxProjPath);

    /**
     * Returns the user for the current project.
     */
    const char* GetUser() const;

    /**
     * Returns the name for the current project.
     */
    const char* GetProjectName() const;

    /**
     * Returns the local path for the current project.
     */
    const char* GetLocalPath() const;

    /**
     * Returns the auxiliary path for the current project.
     */
    const char* GetAuxProjectPath() const;

    /**
     * Checks the specified files out of source control.
     */
    bool CheckOut(const std::vector<std::string>& fileNames, const char* comment);

    /**
     * Checks the specified files into source control.
     */
    bool CheckIn(const std::vector<std::string>& fileNames, const char* comment);

    /**
     * Reverts the specified files into source control.
     */
    bool UndoCheckOut(const std::vector<std::string>& fileNames);

    /**
     * Returns the status of a file.
     */
    Status GetFileStatus(const char* fileName);

    /**
     * Returns the status of a bunch of files.
     */
    bool GetFilesStatus(const std::vector<std::string>& files, std::vector<Status>& status);

    /**
     * Shows a diff of any changes that have be made to the local copy of a file.
     */
    void ShowDiff(const char* fileName);

    /**
     * Shows the check-in history of the specified files.
     */
    void ShowHistory(const std::vector<std::string>& files);

    /**
     * Adds the specified files to source control.
     */
    void AddFiles(const std::string& fileName, const char* comment);

    /**
     * Adds the specified files to source control.
     */
    void AddFiles(const std::vector<std::string>& fileNames, const char* comment);

private:

    struct Provider
    {
        TCHAR   name[SCC_NAME_LEN];
        TCHAR   fileName[_MAX_PATH];
    };

    /**
     * Base class for commands that are dispatched to the processing thread. This mechanism
     * is used because some SCC providers (namely Perforce) require all Scc* calls to come
     * from the same thread (not just non-concurrent).
     */
    class Command
    {

    public:

        /**
         * Constructor.
         */
        explicit Command(SourceControl* sourceControl);

        /**
         * Destructor.
         */
        virtual ~Command();
        
        /**
         * Signals that the command has completed.
         */
        void Complete();
        
        /**
         * Waits until the command has been flagged as complete.
         */
        void WaitForCompletion();
        
        /**
         * Runs the command. This should be implemented by derived classes.
         */
        virtual void Run() = 0;

    protected:

        SourceControl*  m_sourceControl;
        HANDLE          m_hCompleteEvent;
        
    };

    // Specific command implementations.

    class InitializeCommand;
    class UninitializeCommand;
    class OpenProjectCommand;
    class CloseProjectCommand;
    class SelectProjectCommand;
    class GetFilesStatusCommand;
    class CheckOutCommand;
    class CheckInCommand;
    class UndoCheckOutCommand;
    class ShowHistoryCommand;
    class AddFilesCommand;
    class ShowDiffCommand;

    // Functions loaded from the SCC plugin.

    typedef LONG   (*SccGetVersion_t)();
    typedef SCCRTN (*SccInitialize_t)(LPVOID*, HWND, LPCSTR, LPSTR, LPLONG, LPSTR, LPLONG, LPLONG);
    typedef SCCRTN (*SccUninitialize_t)(LPVOID);
    typedef SCCRTN (*SccOpenProject_t)(LPVOID, HWND, LPSTR, LPCSTR, LPCSTR, LPSTR, LPCSTR, LPTEXTOUTPROC, LONG);
    typedef SCCRTN (*SccCloseProject_t)(LPVOID);
    typedef SCCRTN (*SccAddFromScc_t)(LPVOID, HWND, LPLONG, LPCSTR**);
    typedef SCCRTN (*SccPopulateList_t)(LPVOID, SCCCOMMAND, LONG, LPCSTR*, POPLISTFUNC, LPVOID, LPLONG, LONG);
    typedef SCCRTN (*SccGetProjPath_t)(LPVOID, HWND, LPSTR, LPSTR, LPSTR, LPSTR, BOOL, LPBOOL);
    typedef SCCRTN (*SccCheckout_t)(LPVOID, HWND, LONG, LPCSTR*, LPCSTR, LONG, LPCMDOPTS);
    typedef SCCRTN (*SccCheckin_t)(LPVOID, HWND, LONG, LPSTR*, LPCSTR, LONG, LPCMDOPTS);
    typedef SCCRTN (*SccUncheckout_t)(LPVOID, HWND, LONG, LPCSTR*, LONG, LPCMDOPTS);
    typedef SCCRTN (*SccQueryInfo_t)(LPVOID, LONG, LPCSTR*, LPLONG);
    typedef SCCRTN (*SccDiff_t)(LPVOID, HWND, LPCSTR, LONG, LPCMDOPTS);
    typedef SCCRTN (*SccHistory_t)(LPVOID, HWND, LONG, LPCSTR*, LONG, LPCMDOPTS);
    typedef SCCRTN (*SccAdd_t)(LPVOID, HWND, LONG, LPCSTR*, LPCSTR, LONG*, LPCMDOPTS);
    typedef SCCRTN (*SccGetExtendedCapabilities_t)(LPVOID, LONG, LPBOOL);

    /**
     * Initializes the list of source control providers.
     */
    static bool InitializeProvidersList();

    /**
     * Displays an exception message box.
     */
    void DisplayException();

    /**
     * Closes the project (if it's open).
     */
    void CloseProject();

    /**
     * Entry point for the command processing thread. This method just calls
     * the non-static version.
     */
    static DWORD WINAPI CommandThread(LPVOID param);

    /**
     * Entry point for the command processing thread.
     */
    bool CommandThread();

    /**
     * Queues a command for processing in the command thread. This method blocks
     * and returns once the command has completed.
     */
    void DispatchCommand(Command* command);
    
    // Allow the command class to access our internals for convenience.
    friend class Command;
  
private:

    static bool                         s_providersInitialized;
    static std::vector<Provider>        s_providers;

    SccGetVersion_t                     SccGetVersion;
    SccInitialize_t                     SccInitialize;
    SccUninitialize_t                   SccUninitialize;
    SccOpenProject_t                    SccOpenProject;
    SccCloseProject_t                   SccCloseProject;
    SccAddFromScc_t                     SccAddFromScc;
    SccPopulateList_t                   SccPopulateList;
    SccGetProjPath_t                    SccGetProjPath;
    SccCheckout_t                       SccCheckout;
    SccCheckin_t                        SccCheckin;
    SccUncheckout_t                     SccUncheckout;
    SccQueryInfo_t                      SccQueryInfo;
    SccDiff_t                           SccDiff;
    SccHistory_t                        SccHistory;
    SccAdd_t                            SccAdd;
    SccGetExtendedCapabilities_t        SccGetExtendedCapabilities;

    HWND                                m_hWnd;
    unsigned int                        m_provider;
    HMODULE                             m_hModule;
    
    LPVOID                              m_context;
    
    CHAR                                m_name[SCC_NAME_LEN + 1];
    LONG                                m_caps;
    CHAR                                m_auxPathLabel[SCC_AUXLABEL_LEN + 1];
    LONG                                m_maxCheckoutCommentLength;
    LONG                                m_maxCommentLength;

    CHAR                                m_user[SCC_USER_LEN + 1];
    CHAR                                m_projName[SCC_PRJPATH_LEN + 1];
    CHAR                                m_localPath[_MAX_PATH + 1];
    CHAR                                m_auxProjPath[SCC_PRJPATH_LEN + 1];

    bool                                m_initialized;
    bool                                m_projectOpen;

    CriticalSection                     m_commandCriticalSection;

    HANDLE                              m_hCommandThread;
    HANDLE                              m_hCommandCompleteEvent;
    HANDLE                              m_hCommandAvailableEvent;
    std::queue<Command*>                m_commandQueue;
    bool                                m_shutdown;

    bool                                m_supportsMultiThreadedQueryInfo;

};

#endif