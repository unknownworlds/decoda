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

#include "SourceControl.h"
#include "CriticalSectionLock.h"
    
bool                                    SourceControl::s_providersInitialized = false;
std::vector<SourceControl::Provider>    SourceControl::s_providers;

SourceControl::Command::Command(SourceControl* sourceControl)
{
    m_sourceControl = sourceControl;
    m_hCompleteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
}

SourceControl::Command::~Command()
{
    CloseHandle(m_hCompleteEvent);
}

void SourceControl::Command::Complete()
{
    SetEvent(m_hCompleteEvent);
}

void SourceControl::Command::WaitForCompletion()
{
    WaitForSingleObject(m_hCompleteEvent, INFINITE);
}

/**
 * Source control initialization command.
 */
class SourceControl::InitializeCommand : public SourceControl::Command
{

public:

    /**
     * Constructor.
     */
    explicit InitializeCommand(SourceControl* sourceControl) : Command(sourceControl)
    {
        m_result = SCC_E_NONSPECIFICERROR;
    }
   
    /**
     * From Command.
     */
    virtual void Run()
    {

        __try
        {
            
            m_result = m_sourceControl->SccInitialize(&m_sourceControl->m_context, m_sourceControl->m_hWnd, "Decoda", m_sourceControl->m_name,
                &m_sourceControl->m_caps, m_sourceControl->m_auxPathLabel, &m_sourceControl->m_maxCheckoutCommentLength, &m_sourceControl->m_maxCommentLength);
        
            if (m_result >= 0)
            {

                // Get the version of the SCC plugin interface.
                LONG version = m_sourceControl->SccGetVersion();
                WORD major = HIWORD(version);
                WORD minor = LOWORD(version);

                // Check if the plug-in supports querying the file status from multiple
                // threads.
                if ((major > 1 || (major == 1 && minor >= 3)) && m_sourceControl->SccGetExtendedCapabilities != NULL)
                {
                    
                    BOOL supported;
                    m_sourceControl->SccGetExtendedCapabilities(m_sourceControl->m_context, SCC_EXCAP_THREADSAFE_QUERY_INFO, &supported);

                    if (supported != FALSE)
                    {
                        m_sourceControl->m_supportsMultiThreadedQueryInfo = true;
                    }

                }        
            
            }

        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            m_sourceControl->DisplayException();
        }

    }

    SCCRTN GetResult() const
    {
        return m_result;
    }

private:    
    
    SCCRTN m_result;

};

/**
 * Source control uninitialize command.
 */
class SourceControl::UninitializeCommand : public SourceControl::Command
{

public:

    /**
     * Constructor.
     */
    explicit UninitializeCommand(SourceControl* sourceControl) : Command(sourceControl)
    {
    }
   
    /**
     * From Command.
     */
    virtual void Run()
    {

        __try
        {
            m_sourceControl->SccUninitialize(m_sourceControl->m_context);
        }
        __except(EXCEPTION_EXECUTE_HANDLER)
        {
            m_sourceControl->DisplayException();
        }

    }

};

/**
 * Source control project opening command.
 */
class SourceControl::OpenProjectCommand : public SourceControl::Command
{

public:

    /**
     * Constructor.
     */
    explicit OpenProjectCommand(SourceControl* sourceControl) : Command(sourceControl)
    {
        m_result = SCC_E_NONSPECIFICERROR;
    }
   
    /**
     * From Command.
     */
    virtual void Run()
    {

        __try
        {
            m_result = m_sourceControl->SccOpenProject(m_sourceControl->m_context, m_sourceControl->m_hWnd, m_sourceControl->m_user,
                m_sourceControl->m_projName, m_sourceControl->m_localPath, m_sourceControl->m_auxProjPath, "", NULL, SCC_OP_SILENTOPEN);
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            m_sourceControl->DisplayException();
        }

    }

    SCCRTN GetResult() const
    {
        return m_result;
    }

private:    
    
    SCCRTN m_result;

};

/**
 * Source control project closing command.
 */
class SourceControl::CloseProjectCommand : public SourceControl::Command
{

public:

    /**
     * Constructor.
     */
    explicit CloseProjectCommand(SourceControl* sourceControl) : Command(sourceControl)
    {
    }
   
    /**
     * From Command.
     */
    virtual void Run()
    {

        __try
        {
            m_sourceControl->SccCloseProject(m_sourceControl->m_context);
        }
        __except(EXCEPTION_EXECUTE_HANDLER)
        {
            m_sourceControl->DisplayException();
        }

    }

};

/**
 * Source control select project closing command.
 */
class SourceControl::SelectProjectCommand : public SourceControl::Command
{

public:

    /**
     * Constructor.
     */
    explicit SelectProjectCommand(SourceControl* sourceControl) : Command(sourceControl)
    {

        m_result = SCC_E_NONSPECIFICERROR;

        m_user[0]           = 0;
        m_projName[0]       = 0;
        m_localPath[0]      = 0;
        m_auxProjPath[0]    = 0;

    }
   
    /**
     * From Command.
     */
    virtual void Run()
    {
    
        BOOL newProj = FALSE;

        __try
        {
            m_result = m_sourceControl->SccGetProjPath(m_sourceControl->m_context, m_sourceControl->m_hWnd,
                m_user, m_projName, m_localPath, m_auxProjPath, TRUE, &newProj);
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            m_sourceControl->DisplayException();
        }

    }

    /**
     * Returns the result of the command.
     */
    SCCRTN GetResult() const
    {
        return m_result;
    }

    /**
     * Returns the user name returned by the command.
     */
    const CHAR* GetUser() const
    {
        return m_user;
    }

    /**
     * Returns the project name returned by the command.
     */
    const CHAR* GetProjName() const
    {
        return m_projName;
    }

    /**
     * Returns the local path returned by the command.
     */
    const CHAR* GetLocalPath() const
    {
        return m_localPath;
    }

    /**
     * Returns the auxiliary project path returned by the command.
     */
    const CHAR* GetAuxProjPath() const
    {
        return m_localPath;
    }

private:

    SCCRTN  m_result;

    CHAR    m_user[SCC_USER_LEN + 1];
    CHAR    m_projName[SCC_PRJPATH_LEN + 1];
    CHAR    m_localPath[_MAX_PATH + 1];
    CHAR    m_auxProjPath[SCC_PRJPATH_LEN + 1];

};

/**
 * Source control get file status command.
 */
class SourceControl::GetFilesStatusCommand : public SourceControl::Command
{

public:

    /**
     * Constructor.
     */
    explicit GetFilesStatusCommand(SourceControl* sourceControl, const std::vector<std::string>& fileNames, std::vector<SourceControl::Status>& status)
        : Command(sourceControl), m_fileNames(fileNames), m_status(status)
    {
        m_result = false;
    }
   
    /**
     * From Command.
     */
    virtual void Run()
    {

        m_status.clear();

        char** tempFileNames = new char*[m_fileNames.size()];

        for (unsigned int i = 0; i < m_fileNames.size(); ++i)
        {
            // Even though the SccQueryInfo function is supposed to take a const
            // char* array, Perforce modifies the strings (it replaces them with
            // the capitalization of the file name under Perforce). So, we make a
            // copy first.
            tempFileNames[i] = _strdup(m_fileNames[i].c_str());
        }

        LONG* tempStatus = new LONG[m_fileNames.size()];

        SCCRTN result = SCC_E_NONSPECIFICERROR;
        
        __try
        {
            result = m_sourceControl->SccQueryInfo(m_sourceControl->m_context, m_fileNames.size(), (LPCSTR*)(tempFileNames), tempStatus) >= 0;
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            m_sourceControl->DisplayException();
        }

        if (result >= 0)
        {
            
            m_status.reserve(m_fileNames.size());

            for (unsigned int i = 0; i < m_fileNames.size(); ++i)
            {
                if (tempStatus[i] == 0 || tempStatus[i] & SCC_STATUS_DELETED)
                {
                    m_status.push_back(Status_None);
                }
                else if (tempStatus[i] & SCC_STATUS_OUTBYUSER)
                {
                    m_status.push_back(Status_CheckedOutByUser);
                }
                else if (tempStatus[i] & SCC_STATUS_CHECKEDOUT)
                {
                    m_status.push_back(Status_CheckedOut);
                }
                else
                {
                    m_status.push_back(Status_CheckedIn);
                }
            }

        }

        delete [] tempStatus;
        tempStatus = NULL;

        for (unsigned int i = 0; i < m_fileNames.size(); ++i)
        {
            free(tempFileNames[i]);
        }

        delete [] tempFileNames;
        tempFileNames = NULL;

        m_result = (result >= 0);

    }

    /**
     * Returns true if the command was successful.
     */
    bool GetResult() const
    {
        return m_result;
    }

private:

    const std::vector<std::string>&         m_fileNames;
    std::vector<SourceControl::Status>&     m_status;

    bool                                    m_result;

};

/**
 * Source control check out command.
 */
class SourceControl::CheckOutCommand : public SourceControl::Command
{

public:

    /**
     * Constructor.
     */
    explicit CheckOutCommand(SourceControl* sourceControl, const std::vector<std::string>& fileNames, const char* comment)
        : Command(sourceControl), m_fileNames(fileNames), m_comment(comment)
    {
        m_result = SCC_E_NONSPECIFICERROR;
    }
   
    /**
     * From Command.
     */
    virtual void Run()
    {

        const char** tempFileNames = new const char*[m_fileNames.size()];

        for (unsigned int i= 0; i < m_fileNames.size(); ++i)
        {
            tempFileNames[i] = m_fileNames[i].c_str();
        }

        __try
        {
            m_result = m_sourceControl->SccCheckout(m_sourceControl->m_context, m_sourceControl->m_hWnd,
                m_fileNames.size(), tempFileNames, m_comment, 0, NULL);
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            m_sourceControl->DisplayException();
        }
    
        delete [] tempFileNames;
        tempFileNames = NULL;

    }

    SCCRTN GetResult() const
    {
        return m_result;
    }

private:    
    
    SCCRTN                              m_result;
    const std::vector<std::string>&     m_fileNames;
    const char*                         m_comment;

};

/**
 * Source control check in command.
 */
class SourceControl::CheckInCommand : public SourceControl::Command
{

public:

    /**
     * Constructor.
     */
    explicit CheckInCommand(SourceControl* sourceControl, const std::vector<std::string>& fileNames, const char* comment)
        : Command(sourceControl), m_fileNames(fileNames), m_comment(comment)
    {
        m_result = SCC_E_NONSPECIFICERROR;
    }
   
    /**
     * From Command.
     */
    virtual void Run()
    {

        char** tempFileNames = new char*[m_fileNames.size()];

        for (unsigned int i= 0; i < m_fileNames.size(); ++i)
        {
            tempFileNames[i] = _strdup(m_fileNames[i].c_str());
        }

        __try
        {
            m_result = m_sourceControl->SccCheckin(m_sourceControl->m_context, m_sourceControl->m_hWnd,
                m_fileNames.size(), tempFileNames, m_comment, 0, NULL);
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            m_sourceControl->DisplayException();
        }
    
        for (unsigned int i = 0; i < m_fileNames.size(); ++i)
        {
            free(tempFileNames[i]);
        }

        delete [] tempFileNames;
        tempFileNames = NULL;

    }

    SCCRTN GetResult() const
    {
        return m_result;
    }

private:    
    
    SCCRTN                              m_result;
    const std::vector<std::string>&     m_fileNames;
    const char*                         m_comment;

};

/**
 * Source control undo check out command.
 */
class SourceControl::UndoCheckOutCommand : public SourceControl::Command
{

public:

    /**
     * Constructor.
     */
    explicit UndoCheckOutCommand(SourceControl* sourceControl, const std::vector<std::string>& fileNames)
        : Command(sourceControl), m_fileNames(fileNames)
    {
        m_result = SCC_E_NONSPECIFICERROR;
    }
   
    /**
     * From Command.
     */
    virtual void Run()
    {

        const char** tempFileNames = new const char*[m_fileNames.size()];

        for (unsigned int i= 0; i < m_fileNames.size(); ++i)
        {
            tempFileNames[i] = m_fileNames[i].c_str();
        }

        __try
        {
            m_result = m_sourceControl->SccUncheckout(m_sourceControl->m_context, m_sourceControl->m_hWnd,
                m_fileNames.size(), tempFileNames, 0, NULL);
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            m_sourceControl->DisplayException();
        }
    
        delete [] tempFileNames;
        tempFileNames = NULL;

    }

    SCCRTN GetResult() const
    {
        return m_result;
    }

private:    
    
    SCCRTN                              m_result;
    const std::vector<std::string>&     m_fileNames;

};

/**
 * Source control show history command.
 */
class SourceControl::ShowHistoryCommand : public SourceControl::Command
{

public:

    /**
     * Constructor.
     */
    explicit ShowHistoryCommand(SourceControl* sourceControl, const std::vector<std::string>& fileNames)
        : Command(sourceControl), m_fileNames(fileNames)
    {
    }
   
    /**
     * From Command.
     */
    virtual void Run()
    {

        const char** tempFileNames = new const char*[m_fileNames.size()];

        for (unsigned int i= 0; i < m_fileNames.size(); ++i)
        {
            tempFileNames[i] = m_fileNames[i].c_str();
        }

        __try
        {
            m_sourceControl->SccHistory(m_sourceControl->m_context, m_sourceControl->m_hWnd,
                m_fileNames.size(), tempFileNames, 0, NULL);
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            m_sourceControl->DisplayException();
        }
    
        delete [] tempFileNames;
        tempFileNames = NULL;

    }

private:    
    
    const std::vector<std::string>&     m_fileNames;

};

/**
 * Source control add files command.
 */
class SourceControl::AddFilesCommand : public SourceControl::Command
{

public:

    /**
     * Constructor.
     */
    explicit AddFilesCommand(SourceControl* sourceControl, const std::vector<std::string>& fileNames, const char* comment)
        : Command(sourceControl), m_fileNames(fileNames), m_comment(comment)
    {
        m_result = SCC_E_NONSPECIFICERROR;
    }
   
    /**
     * From Command.
     */
    virtual void Run()
    {

        const char** tempFileNames = new const char*[m_fileNames.size()];

        for (unsigned int i= 0; i < m_fileNames.size(); ++i)
        {
            tempFileNames[i] = m_fileNames[i].c_str();
        }

        __try
        {
            m_result = m_sourceControl->SccAdd(m_sourceControl->m_context, m_sourceControl->m_hWnd,
                m_fileNames.size(), tempFileNames, m_comment, NULL, NULL);
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            m_sourceControl->DisplayException();
        }

        delete [] tempFileNames;
        tempFileNames = NULL;

    }

    SCCRTN GetResult() const
    {
        return m_result;
    }

private:    
    
    SCCRTN                              m_result;
    const std::vector<std::string>&     m_fileNames;
    const char*                         m_comment;

};

/**
 * Source control show diff command.
 */
class SourceControl::ShowDiffCommand : public SourceControl::Command
{

public:

    /**
     * Constructor.
     */
    explicit ShowDiffCommand(SourceControl* sourceControl, const char* fileName)
        : Command(sourceControl), m_fileName(fileName)
    {
    }
   
    /**
     * From Command.
     */
    virtual void Run()
    {

        __try
        {
            m_sourceControl->SccDiff(m_sourceControl->m_context, m_sourceControl->m_hWnd, m_fileName, 0, NULL);
        }
        __except(EXCEPTION_EXECUTE_HANDLER)
        {
            m_sourceControl->DisplayException();
        }

    }

private:

    const char* m_fileName;

};

unsigned int SourceControl::GetNumProviders()
{
    
    if (!s_providersInitialized)
    {
        InitializeProvidersList();
        s_providersInitialized = true;
    }

    return s_providers.size();

}

const char* SourceControl::GetProviderName(unsigned int i)
{
    return s_providers[i].name;
}

SourceControl::SourceControl()
{

    m_hWnd                      = NULL;
    m_hModule                   = NULL;
    m_hCommandThread            = NULL;
    m_hCommandCompleteEvent     = NULL;
    m_hCommandAvailableEvent    = NULL;

    m_context                   = NULL;

    SccGetVersion               = NULL;
    SccInitialize               = NULL;
    SccUninitialize             = NULL;
    SccOpenProject              = NULL;
    SccCloseProject             = NULL;
    SccAddFromScc               = NULL;
    SccPopulateList             = NULL;
    SccGetProjPath              = NULL;
    SccCheckout                 = NULL;
    SccCheckin                  = NULL;
    SccQueryInfo                = NULL;
    SccDiff                     = NULL;
    SccHistory                  = NULL;
    SccAdd                      = NULL;

    m_user[0]                   = 0;
    m_projName[0]               = 0;
    m_localPath[0]              = 0;
    m_auxProjPath[0]            = 0;
    
    m_initialized               = false;
    m_projectOpen               = false;
    m_shutdown                  = false;

}

SourceControl::~SourceControl()
{
    Shutdown();
}

bool SourceControl::Initialize(const char* provider, HWND hWnd)
{
    for (unsigned int i = 0; i < GetNumProviders(); ++i)
    {
        if (strcmp(provider, GetProviderName(i)) == 0)
        {
            return Initialize(i, hWnd);
        }
    }
    return false;
}

bool SourceControl::Initialize(unsigned int provider, HWND hWnd)
{

    Shutdown();

    //m_hWnd      = hWnd;
    m_provider  = provider;

    // Create the thread for processing commands.

    DWORD threadId;

    m_hCommandCompleteEvent     = CreateEvent(NULL, FALSE, FALSE, NULL);
    m_hCommandAvailableEvent    = CreateEvent(NULL, FALSE, FALSE, NULL);
    m_hCommandThread            = CreateThread(NULL, 0, CommandThread, this, 0, &threadId);

    // Wait for the thread to finish initialization (or for it to quit).
   
    HANDLE handles[] = 
        {
            m_hCommandCompleteEvent,
            m_hCommandThread
        };

    if (WaitForMultipleObjects(2, handles, FALSE, INFINITE) != WAIT_OBJECT_0)
    {
        // Initialization failed.
        Shutdown();
        return false;
    }
  
    // Dispatch an initialize command.

    InitializeCommand* command = new InitializeCommand(this);
    DispatchCommand( command );

    m_initialized = (command->GetResult() >= 0);
    delete command;

    return m_initialized;

}

bool SourceControl::GetIsInitialized() const
{
    return m_initialized;
}

void SourceControl::Shutdown()
{

    if (m_hModule != NULL)
    {

        CloseProject();

        // Dispatch an uninitialize command.
        UninitializeCommand* command = new UninitializeCommand(this);
        DispatchCommand( command );
        delete command;

        // Signal that we're shutting down. This will cause the command thread to
        // exit once there are no more commands to process.
        m_shutdown = true;
        SetEvent(m_hCommandAvailableEvent);

        if (m_hCommandThread != NULL)
        {
            WaitForSingleObject(m_hCommandThread, INFINITE);
        }

        FreeLibrary(m_hModule);
        m_hModule = NULL;
    
    }

    if (m_hCommandThread != NULL)
    {
        CloseHandle(m_hCommandThread);
        m_hCommandThread = NULL;
    }

    if (m_hCommandCompleteEvent != NULL)
    {
        CloseHandle(m_hCommandCompleteEvent);
        m_hCommandCompleteEvent = NULL;
    }

    if (m_hCommandAvailableEvent != NULL)
    {
        CloseHandle(m_hCommandAvailableEvent);
        m_hCommandAvailableEvent = NULL;
    }
        
    m_hWnd                      = NULL;

    m_context                   = NULL;

    SccGetVersion               = NULL;
    SccInitialize               = NULL;
    SccUninitialize             = NULL;
    SccOpenProject              = NULL;
    SccCloseProject             = NULL;
    SccAddFromScc               = NULL;
    SccPopulateList             = NULL;
    SccGetProjPath              = NULL;
    SccCheckout                 = NULL;
    SccCheckin                  = NULL;
    SccUncheckout               = NULL;
    SccQueryInfo                = NULL;
    SccDiff                     = NULL;
    SccHistory                  = NULL;
    SccGetExtendedCapabilities  = NULL;

    m_initialized               = false;
    m_shutdown                  = false;

}

const char* SourceControl::GetName() const
{
    return m_name;
}

bool SourceControl::SelectProject()
{

    CloseProject();

    // Dispatch a select project command.

    SelectProjectCommand* command = new SelectProjectCommand(this);
    DispatchCommand( command );

    bool result = false;

    if (command->GetResult() == SCC_OK)
    {
        strcpy(m_user, command->GetUser());
        strcpy(m_projName, command->GetProjName());
        strcpy(m_localPath, command->GetLocalPath());
        strcpy(m_auxProjPath, command->GetAuxProjPath());
        result = true;
    }

    delete command;
    return result;

}

bool SourceControl::OpenProject(const char* user, const char* projName, const char* localPath, const char* auxProjPath)
{

    CloseProject();
    
    strncpy(m_user, user, sizeof(m_user));
    strncpy(m_projName, projName, sizeof(m_projName));
    strncpy(m_localPath, localPath, sizeof(m_localPath));
    strncpy(m_auxProjPath, auxProjPath, sizeof(m_auxProjPath));
    
    // Dispatch open project command.

    OpenProjectCommand* command = new OpenProjectCommand(this);
    DispatchCommand( command );

    m_projectOpen = (command->GetResult() >= 0);
    delete command;    
    
    return m_projectOpen;

}

const char* SourceControl::GetUser() const
{
    return m_user;
}

const char* SourceControl::GetProjectName() const
{
    return m_projName;
}

const char* SourceControl::GetLocalPath() const
{
    return m_localPath;
}

const char* SourceControl::GetAuxProjectPath() const
{
    return m_auxProjPath;
}

bool SourceControl::CheckOut(const std::vector<std::string>& fileNames, const char* comment)
{

    // Dispatch a check out command.

    CheckOutCommand* command = new CheckOutCommand(this, fileNames, comment);
    DispatchCommand( command );

    bool result = (command->GetResult() >= 0);
    delete command;

    return result;

}

bool SourceControl::CheckIn(const std::vector<std::string>& fileNames, const char* comment)
{

    // Dispatch a check in command.

    CheckInCommand* command = new CheckInCommand(this, fileNames, comment);
    DispatchCommand( command );

    bool result = (command->GetResult() >= 0);
    delete command;

    return result;

}

bool SourceControl::UndoCheckOut(const std::vector<std::string>& fileNames)
{

    // Dispatch an undo check out command.

    UndoCheckOutCommand* command = new UndoCheckOutCommand(this, fileNames);
    DispatchCommand( command );

    bool result = (command->GetResult() >= 0);
    delete command;

    return result;

}

SourceControl::Status SourceControl::GetFileStatus(const char* fileName)
{
    
    std::vector<std::string> fileNames;
    fileNames.push_back(fileName);

    std::vector<Status> status;
    
    if (!GetFilesStatus(fileNames, status))
    {
        return Status_Error;
    }
    else
    {
        return status[0];
    }

}

bool SourceControl::GetFilesStatus(const std::vector<std::string>& fileNames, std::vector<Status>& status)
{

    if (fileNames.empty())
    {
        return true;
    }

    GetFilesStatusCommand* command = new GetFilesStatusCommand(this, fileNames, status);

    if (m_supportsMultiThreadedQueryInfo)
    {
        // This plugin supports multithreaded status queries, so just do it in this thread.
        command->Run();
    }
    else
    {
        // Dispatch a get files status command to the background thread.
        DispatchCommand( command );
    }

    bool result = command->GetResult();
    delete command;

    return result;

}

void SourceControl::ShowDiff(const char* fileName)
{

    // Dispatch a show diff command.

    ShowDiffCommand* command = new ShowDiffCommand(this, fileName);
    DispatchCommand( command );
    delete command;

}
    
void SourceControl::ShowHistory(const std::vector<std::string>& fileNames)
{

    // Dispatch a show history command.

    ShowHistoryCommand* command = new ShowHistoryCommand(this, fileNames);
    DispatchCommand( command );
    delete command;

}

void SourceControl::AddFiles(const std::vector<std::string>& fileNames, const char* comment)
{

    // Dispatch an add files command.

    AddFilesCommand* command = new AddFilesCommand(this, fileNames, comment);
    DispatchCommand( command );

    bool result = (command->GetResult() >= 0);
    delete command;

}

void SourceControl::AddFiles(const std::string& fileName, const char* comment)
{
    
    std::vector<std::string> fileNames(1);
    fileNames[0] = fileName;
    
    AddFiles(fileNames, comment);

}

bool SourceControl::InitializeProvidersList()
{

    // This is from http://www.codeproject.com/tools/sccswitcher.asp

    // Open registy key, where current scc provider is specified
    HKEY providerKey;
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\SourceCodeControlProvider", 0, KEY_WRITE|KEY_READ, &providerKey) !=  ERROR_SUCCESS)
    {
        return false;
    }

    // Open registy key, where all installed scc providers are listed
    HKEY installedProviderKey;
    if (RegOpenKeyEx(providerKey, "InstalledSCCProviders", 0, KEY_READ, &installedProviderKey) != ERROR_SUCCESS)
    {
        return false;
    }

    // # of subkeys is the # of installed providers
    DWORD numProviders;
    if (RegQueryInfoKey(installedProviderKey, NULL, NULL, NULL, NULL, NULL, NULL, &numProviders, NULL, NULL, NULL, NULL) !=  ERROR_SUCCESS)
    {
        return false;
    }

    for (unsigned int i = 0; i < numProviders; ++i)
    {

        Provider provider = { 0 };

        DWORD valueType;

        TCHAR registryPath[_MAX_PATH];
        DWORD registryPathLength = _MAX_PATH;

        char keyName[2048];
        DWORD keyNameLength = 2048;

        if (RegEnumValue(installedProviderKey, i, keyName, &keyNameLength, NULL, &valueType, reinterpret_cast<BYTE*>(registryPath), &registryPathLength) != ERROR_SUCCESS)
        {
            break;    
        }

        strncpy(provider.name, keyName, sizeof(provider.name) - 1);

        // Open the key for the key provider.
        HKEY realProviderKey;
        if (RegOpenKeyEx( HKEY_LOCAL_MACHINE, registryPath, 0, KEY_READ, &realProviderKey) == ERROR_SUCCESS)
        {

            DWORD fileNameLength = sizeof(provider.fileName);

            if (RegQueryValueEx( realProviderKey, "SCCServerPath", 0, &valueType, reinterpret_cast<BYTE*>(provider.fileName), &fileNameLength ) == ERROR_SUCCESS )
            {
                // We found everything we need, so add the provider.
                s_providers.push_back(provider);
            }

            RegCloseKey(realProviderKey);
            realProviderKey = NULL;

        }
    
    }

    RegCloseKey(installedProviderKey);
    installedProviderKey = NULL;
    
    RegCloseKey(providerKey);
    providerKey = NULL;

    return true;

}

void SourceControl::DisplayException()
{
    MessageBox(m_hWnd, "The source control plugin has generated an exception.", NULL, MB_OK);
}

void SourceControl::CloseProject()
{
    if (m_projectOpen)
    {
 
        // Dispatch open project close.

        CloseProjectCommand* command = new CloseProjectCommand(this);
        DispatchCommand( command );

        delete command;    

        m_projectOpen = false;

    }
}

void SourceControl::DispatchCommand(Command* command)
{

	{
        // Add the command to the queue.
		CriticalSectionLock lock(m_commandCriticalSection);
        m_commandQueue.push(command);
	}

    // Signal that there's something in the queue.
    SetEvent(m_hCommandAvailableEvent);

    // Wait for the command to finish executing in the command thread.
	command->WaitForCompletion();
	
}

bool SourceControl::CommandThread()
{

    // Load the plug-in DLL.

    m_hModule = LoadLibrary(s_providers[m_provider].fileName);   

    if (m_hModule == NULL)
    {
        return false;
    }

    // Load the important functions.

    SccGetVersion               = reinterpret_cast<SccGetVersion_t>(GetProcAddress(m_hModule,               "SccGetVersion"));
    SccInitialize               = reinterpret_cast<SccInitialize_t>(GetProcAddress(m_hModule,               "SccInitialize"));
    SccUninitialize             = reinterpret_cast<SccUninitialize_t>(GetProcAddress(m_hModule,             "SccUninitialize"));
    SccOpenProject              = reinterpret_cast<SccOpenProject_t>(GetProcAddress(m_hModule,              "SccOpenProject"));
    SccCloseProject             = reinterpret_cast<SccCloseProject_t>(GetProcAddress(m_hModule,             "SccCloseProject"));
    SccAddFromScc               = reinterpret_cast<SccAddFromScc_t>(GetProcAddress(m_hModule,               "SccAddFromScc"));
    SccPopulateList             = reinterpret_cast<SccPopulateList_t>(GetProcAddress(m_hModule,             "SccPopulateList"));
    SccGetProjPath              = reinterpret_cast<SccGetProjPath_t>(GetProcAddress(m_hModule,              "SccGetProjPath"));
    SccCheckout                 = reinterpret_cast<SccCheckout_t>(GetProcAddress(m_hModule,                 "SccCheckout"));
    SccCheckin                  = reinterpret_cast<SccCheckin_t>(GetProcAddress(m_hModule,                  "SccCheckin"));
    SccUncheckout               = reinterpret_cast<SccUncheckout_t>(GetProcAddress(m_hModule,               "SccUncheckout"));
    SccQueryInfo                = reinterpret_cast<SccQueryInfo_t>(GetProcAddress(m_hModule,                "SccQueryInfo"));
    SccDiff                     = reinterpret_cast<SccDiff_t>(GetProcAddress(m_hModule,                     "SccDiff"));
    SccHistory                  = reinterpret_cast<SccHistory_t>(GetProcAddress(m_hModule,                  "SccHistory"));
    SccAdd                      = reinterpret_cast<SccAdd_t>(GetProcAddress(m_hModule,                      "SccAdd"));
    SccGetExtendedCapabilities  = reinterpret_cast<SccGetExtendedCapabilities_t>(GetProcAddress(m_hModule,  "SccGetExtendedCapabilities"));

    // Signal that we're ready for commands.
    SetEvent(m_hCommandCompleteEvent);

    m_supportsMultiThreadedQueryInfo = false;

    // Using our actual window handle causes problems with the SCC plugin, presumably because of
    // the multithreading issues, or conflicts between Qt and wxWidgets. Using the desktop window
    // seems to make everyone happy.
    m_hWnd = GetDesktopWindow();

    bool done = false;

    while (!done)
	{

        bool empty;

        {
            CriticalSectionLock lock(m_commandCriticalSection);
            empty = m_commandQueue.empty();
        }

        if (empty)
        {
            if (m_shutdown)
            {
                done = true;
            }
            else
            {
                // Wait for the queue to have something in it.
                WaitForSingleObject(m_hCommandAvailableEvent, INFINITE);
            }
        }

        Command* command = NULL;

        {

            CriticalSectionLock lock(m_commandCriticalSection);

            if (!m_commandQueue.empty())
            {
                command = m_commandQueue.front();
	            m_commandQueue.pop();
            }

        }

        if (command != NULL)
        {
            command->Run();
		    command->Complete();
        }
        else
        {
            done = true;
        }
	
	}

    return true;

}

DWORD WINAPI SourceControl::CommandThread(LPVOID param)
{
    
    SourceControl* sourceControl = reinterpret_cast<SourceControl*>(param);
    
    if (sourceControl->CommandThread())
    {
        return 1;
    }
    else
    {
        return 0;
    }

}