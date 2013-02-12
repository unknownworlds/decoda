//***************************************************************************
// scc.h
//
// This module contains the prototypes and definitions for Microsoft's
// Source Code Control API.  This file and the SCC API are covered by
// Non-Disclosure Agreements (NDA) and are not to be redistributed in any
// for without permission by Microsoft.
//
// Terms used in this module:
//  IDE = Visual Studio Integrated Development Environment
//  dir = directory on local/client machine
//
// This module has been updated to include the following:
//	May '96:	
//	 * The SCC version number has been bumped to 1.1.
//	 * 16-bit is no longer supported.
//	 * A new capability bit, SCC_CAP_REENTRANT, has been added.  A provider
//		which returns this bit can handle multiple SCC context values (eg:
//		multiple simultaneous open projects), and reentrant calls for those
//		contexts (ie: thread safe).
//   * A new option, SCC_OPT_SCCCHECKOUTONLY has been added. This is used
//      to disallow scc operations from outside of integration.  Integration
//      hosts like MS Access use this to prevent files from being checked out
//      without also being imported into scc.
//  March '98:
//   * The SetOption SCC_OPT_SHARESUBPROJ was added to allow destination dir 
//      for share. Setting this option changes the semantics of SccAddFromScc 
//      to accept the target as input.
//  May '02:
//   * The SCC version number has been bumped to 1.2.
//   * The following new capability bits have been added:
//      SCC_CAP_CREATESUBPROJECT,
//      SCC_CAP_GETPARENTPROJECT,
//      SCC_CAP_BATCH,
//      SCC_CAP_DIRECTORYSTATUS,
//      SCC_CAP_DIRECTORYDIFF,
//      SCC_CAP_MULTICHECKOUT,
//      SCC_CAP_SCCFILE
//   * The following new functions corresponding to the new capability bits
//     have been added:
//      SccCreateSubProject
//      SccGetParentProjectPath
//      SccBeginBatch
//      SccEndBatch
//      SccDirQueryInfo
//      SccDirDiff
//      SccIsMultiCheckoutEnabled
//      SccWillCreateSccFile
//   * The new enum SccDirStatus has been added for use with SccDirQueryInfo
//
// Copyright (c) Microsoft Corporation 1995-1996, All rights reserved.
//***************************************************************************
#ifndef _SCC_DEFS
#define _SCC_DEFS

#include <stdlib.h>


/****************************************************************************
    Make it easy to export functions
****************************************************************************/
#if !defined( EXTFUN )
#if defined( WIN32 )
#define EXTFUN __declspec(dllexport)
#else
#define EXTFUN __export
#endif
#endif

#if defined( __cplusplus )
#define SCCEXTERNC extern "C"
#else
#define SCCEXTERNC
#endif



/****************************************************************************
    Linkage for external functions will be C naming mode.
****************************************************************************/
#if defined( __cplusplus )
extern "C" {
#endif



/****************************************************************************
    Take care of type information based on platform and settings.
****************************************************************************/
//#if defined( UNICODE )
#include <winnls.h>
//#else
//typedef LPSTR LPTSTR;
//typedef LPCSTR LPCTSTR;
//typedef char TCHAR;
//#endif

#if defined( WIN16 )
typedef BOOL far * LPBOOL;
#endif


/****************************************************************************
    Version Flag.  Major is HIWORD, Minor is LOWORD
****************************************************************************/
#define SCC_MAJOR_VER_VAL   1
#define SCC_MINOR_VER_VAL   2
#define SCC_VER_NUM         MAKELONG(SCC_MINOR_VER_VAL, SCC_MAJOR_VER_VAL)
#define SCC_GET_MAJOR_VER(ver)  HIWORD(ver)
#define SCC_GET_MINOR_VER(ver)  LOWORD(ver)



/****************************************************************************
    Following strings are the keys for accessing the registry to find
    the SCC provider.
****************************************************************************/
#if !defined( _SCC_REG_KEYS )
#define _SCC_REG_KEYS
#define STR_SCC_PROVIDER_REG_LOCATION   "Software\\SourceCodeControlProvider"
#define STR_PROVIDERREGKEY              "ProviderRegKey"
#define STR_SCCPROVIDERPATH             "SCCServerPath"
#define STR_SCCPROVIDERNAME             "SCCServerName"
#define STR_SCC_INI_SECTION             "Source Code Control"
#define STR_SCC_INI_KEY                 "SourceCodeControlProvider"
#define SCC_PROJECTNAME_KEY             "SCC_Project_Name"
#define SCC_PROJECTAUX_KEY              "SCC_Aux_Path"
#define SCC_STATUS_FILE                 "MSSCCPRJ.SCC"
#define SCC_KEY                         "SCC"
#define SCC_FILE_SIGNATURE              "This is a source code control file"
#endif /* _SCC_REG_KEYS */



/****************************************************************************
    String lengths (each length does *not* include the terminating '\0')
****************************************************************************/
#define SCC_NAME_LEN            31      // lpSccName, SCCInitialize
#define SCC_AUXLABEL_LEN        31      // lpAuxPathLabel, SCCInitialize
#define SCC_USER_LEN            31      // lpUser, SCCOpenProject
#define SCC_PRJPATH_LEN         300     // lpAuxProjPath, SCCGetProjPath



/****************************************************************************
    These are the error codes that may be returned from a function.
    All errors are < 0, warnings are > 0, and success is 0.  Use the
    macros provided for quick checking.  
****************************************************************************/
typedef long SCCRTN;
typedef SCCRTN FAR * LPSCCRTN;

#define IS_SCC_ERROR(rtn) (((rtn) < 0) ? TRUE : FALSE)
#define IS_SCC_SUCCESS(rtn) (((rtn) == SCC_OK) ? TRUE : FALSE)
#define IS_SCC_WARNING(rtn) (((rtn) > 0) ? TRUE : FALSE)


#define SCC_I_SHARESUBPROJOK				7
#define SCC_I_FILEDIFFERS						6
#define SCC_I_RELOADFILE						5
#define SCC_I_FILENOTAFFECTED                   4
#define SCC_I_PROJECTCREATED                    3
#define SCC_I_OPERATIONCANCELED                 2
#define SCC_I_ADV_SUPPORT                       1

#define SCC_OK                                  0

#define SCC_E_INITIALIZEFAILED                  -1
#define SCC_E_UNKNOWNPROJECT                    -2
#define SCC_E_COULDNOTCREATEPROJECT             -3
#define SCC_E_NOTCHECKEDOUT                     -4
#define SCC_E_ALREADYCHECKEDOUT                 -5
#define SCC_E_FILEISLOCKED                      -6
#define SCC_E_FILEOUTEXCLUSIVE                  -7
#define SCC_E_ACCESSFAILURE                     -8
#define SCC_E_CHECKINCONFLICT                   -9
#define SCC_E_FILEALREADYEXISTS                 -10
#define SCC_E_FILENOTCONTROLLED                 -11
#define SCC_E_FILEISCHECKEDOUT                  -12
#define SCC_E_NOSPECIFIEDVERSION                -13
#define SCC_E_OPNOTSUPPORTED                    -14
#define SCC_E_NONSPECIFICERROR                  -15
#define SCC_E_OPNOTPERFORMED                    -16
#define SCC_E_TYPENOTSUPPORTED                  -17
#define SCC_E_VERIFYMERGE                       -18
#define SCC_E_FIXMERGE                          -19
#define SCC_E_SHELLFAILURE                      -20
#define SCC_E_INVALIDUSER                       -21
#define SCC_E_PROJECTALREADYOPEN                -22
#define SCC_E_PROJSYNTAXERR                     -23
#define SCC_E_INVALIDFILEPATH                   -24
#define SCC_E_PROJNOTOPEN                       -25
#define SCC_E_NOTAUTHORIZED                     -26
#define SCC_E_FILESYNTAXERR                     -27
#define SCC_E_FILENOTEXIST                      -28
#define SCC_E_CONNECTIONFAILURE					-29



#ifndef _SCC_STATUS_DEFINED
#define _SCC_STATUS_DEFINED
/****************************************************************************
    The SCC_STATUS_xxx macros define the state of a file
****************************************************************************/
enum  SccStatus 
{
    SCC_STATUS_INVALID          = -1L,		// Status could not be obtained, don't rely on it
    SCC_STATUS_NOTCONTROLLED    = 0x0000L,	// File is not under source control
    SCC_STATUS_CONTROLLED       = 0x0001L,	// File is under source code control
    SCC_STATUS_CHECKEDOUT       = 0x0002L,	// Checked out to current user at local path
    SCC_STATUS_OUTOTHER         = 0x0004L,	// File is checked out to another user
    SCC_STATUS_OUTEXCLUSIVE     = 0x0008L,	// File is exclusively check out
    SCC_STATUS_OUTMULTIPLE      = 0x0010L,	// File is checked out to multiple people
    SCC_STATUS_OUTOFDATE        = 0x0020L,	// The file is not the most recent
    SCC_STATUS_DELETED          = 0x0040L,	// File has been deleted from the project
    SCC_STATUS_LOCKED           = 0x0080L,	// No more versions allowed
    SCC_STATUS_MERGED           = 0x0100L,	// File has been merged but not yet fixed/verified
    SCC_STATUS_SHARED           = 0x0200L,	// File is shared between projects
    SCC_STATUS_PINNED           = 0x0400L,	// File is shared to an explicit version
    SCC_STATUS_MODIFIED         = 0x0800L,	// File has been modified/broken/violated
    SCC_STATUS_OUTBYUSER        = 0x1000L,	// File is checked out by current user someplace
    SCC_STATUS_NOMERGE          = 0x2000L,	// File is never mergeable and need not be saved before a GET
    SCC_STATUS_RESERVED_1       = 0x4000L,	// Status bit reserved for internal use
    SCC_STATUS_RESERVED_2       = 0x8000L	// Status bit reserved for internal use
};
#endif /* _SCC_STATUS_DEFINED */



#ifndef _SCC_DIRSTATUS_DEFINED
#define _SCC_DIRSTATUS_DEFINED
/****************************************************************************
    The SCC_DIRSTATUS_xxx macros define the state of a directory
****************************************************************************/
enum  SccDirStatus 
{
    SCC_DIRSTATUS_INVALID       = -1L,		// Status could not be obtained, don't rely on it
    SCC_DIRSTATUS_NOTCONTROLLED = 0x0000L,	// Directory is not under source control
											//   i.e. there is no project corresponding to the directory
    SCC_DIRSTATUS_CONTROLLED    = 0x0001L,	// Directory is under source code control
											//   i.e. there exists a project corresponding to the directory
    SCC_DIRSTATUS_EMPTYPROJ     = 0x0002L	// Project corresponding to directory is empty
};
#endif /* _SCC_DIRSTATUS_DEFINED */



/****************************************************************************
    SccOpenProject flags
****************************************************************************/
#define SCC_OP_CREATEIFNEW      0x00000001L
#define SCC_OP_SILENTOPEN       0x00000002L


/****************************************************************************
    Keep checked out
****************************************************************************/
#define SCC_KEEP_CHECKEDOUT     0x1000


/****************************************************************************
    Add flags
****************************************************************************/
#define SCC_FILETYPE_AUTO       0x00
#define SCC_FILETYPE_TEXT       0x01
#define SCC_FILETYPE_BINARY     0x02
#define SCC_ADD_STORELATEST     0x04


/****************************************************************************
    Diff flags.  The SCC_DIFF_QD_xxx flags are mutually exclusive.  If any
	one of the three are specified, then no visual feed back is to be given.
	If one is specified but not supported, then the next best one is chosen.
****************************************************************************/
#define SCC_DIFF_IGNORECASE     0x0002
#define SCC_DIFF_IGNORESPACE    0x0004
#define SCC_DIFF_QD_CONTENTS	0x0010
#define SCC_DIFF_QD_CHECKSUM	0x0020
#define SCC_DIFF_QD_TIME		0x0040
#define SCC_DIFF_QUICK_DIFF		0x0070		/* Any QD means no display     */


/****************************************************************************
    Get flags
****************************************************************************/
#define SCC_GET_ALL             0x00000001L
#define SCC_GET_RECURSIVE       0x00000002L


/****************************************************************************
    PopulateList flags
****************************************************************************/
#define SCC_PL_DIR				0x00000001L


/****************************************************************************
    Options for SccGetCommandOptions and SccPopulateList
****************************************************************************/
typedef LPVOID LPCMDOPTS;
#ifndef SCCCOMMAND_DEFINED
#define SCCCOMMAND_DEFINED
enum  SCCCOMMAND 
{
	SCC_COMMAND_GET,
	SCC_COMMAND_CHECKOUT,
	SCC_COMMAND_CHECKIN,
	SCC_COMMAND_UNCHECKOUT,
  	SCC_COMMAND_ADD,
	SCC_COMMAND_REMOVE,
	SCC_COMMAND_DIFF,
	SCC_COMMAND_HISTORY,
	SCC_COMMAND_RENAME,
	SCC_COMMAND_PROPERTIES,
	SCC_COMMAND_OPTIONS
};
#endif /* SCCCOMMAND_DEFINED */

typedef BOOL (*POPLISTFUNC)  (LPVOID pvCallerData, BOOL bAddKeep, LONG nStatus, LPCSTR lpFile);
typedef BOOL (*POPLISTFUNCA) (LPVOID pvCallerData, BOOL bAddKeep, LONG nStatus, LPCSTR lpFile);
typedef BOOL (*POPLISTFUNCW) (LPVOID pvCallerData, BOOL bAddKeep, LONG nStatus, LPCWSTR lpFile);
#ifdef UNICODE
typedef POPLISTFUNCW POPLISTFUNCT;
#else
typedef POPLISTFUNCA POPLISTFUNCT;
#endif

/****************************************************************************
    The SCC_CAP_xxx flags are used to determine what capabilites a provider
    has.
****************************************************************************/
#define SCC_CAP_REMOVE            0x00000001L   // Supports the SCC_Remove command
#define SCC_CAP_RENAME            0x00000002L   // Supports the SCC_Rename command
#define SCC_CAP_DIFF              0x00000004L   // Supports the SCC_Diff command
#define SCC_CAP_HISTORY           0x00000008L   // Supports the SCC_History command
#define SCC_CAP_PROPERTIES        0x00000010L   // Supports the SCC_Properties command
#define SCC_CAP_RUNSCC            0x00000020L   // Supports the SCC_RunScc command
#define SCC_CAP_GETCOMMANDOPTIONS 0x00000040L   // Supports the SCC_GetCommandOptions command
#define SCC_CAP_QUERYINFO         0x00000080L   // Supports the SCC_QueryInfo command
#define SCC_CAP_GETEVENTS         0x00000100L   // Supports the SCC_GetEvents command
#define SCC_CAP_GETPROJPATH       0x00000200L   // Supports the SCC_GetProjPath command
#define SCC_CAP_ADDFROMSCC        0x00000400L   // Supports the SCC_AddFromScc command
#define SCC_CAP_COMMENTCHECKOUT   0x00000800L   // Supports a comment on Checkout
#define SCC_CAP_COMMENTCHECKIN    0x00001000L   // Supports a comment on Checkin
#define SCC_CAP_COMMENTADD        0x00002000L   // Supports a comment on Add
#define SCC_CAP_COMMENTREMOVE     0x00004000L   // Supports a comment on Remove
#define SCC_CAP_TEXTOUT           0x00008000L   // Writes text to an IDE-provided output function
#define SCC_CAP_CREATESUBPROJECT  0x00010000L   // Supports the SccCreateSubProject command
#define SCC_CAP_GETPARENTPROJECT  0x00020000L   // Supports the SccGetParentProjectPath command
#define SCC_CAP_BATCH             0x00040000L   // Supports the SccBeginBatch and SccEndBatch commands
#define SCC_CAP_DIRECTORYSTATUS   0x00080000L   // Supports the querying of directory status
#define SCC_CAP_DIRECTORYDIFF     0x00100000L   // Supports differencing on directories
#define SCC_CAP_ADD_STORELATEST   0x00200000L   // Supports storing files without deltas
#define SCC_CAP_HISTORY_MULTFILE  0x00400000L   // Multiple file history is supported
#define SCC_CAP_IGNORECASE        0x00800000L   // Supports case insensitive file comparison
#define SCC_CAP_IGNORESPACE       0x01000000L   // Supports file comparison that ignores white space
#define SCC_CAP_POPULATELIST      0x02000000L   // Supports finding extra files
#define SCC_CAP_COMMENTPROJECT    0x04000000L   // Supports comments on create project
#define SCC_CAP_MULTICHECKOUT     0x08000000L   // Supports multiple checkouts on a file
												//   (subject to administrator override)
#define SCC_CAP_DIFFALWAYS        0x10000000L   // Supports diff in all states if under control
#define SCC_CAP_GET_NOUI          0x20000000L	// Provider doesn't support a UI for SccGet,
												//   but IDE may still call SccGet function.
#define SCC_CAP_REENTRANT		  0x40000000L	// Provider is reentrant and thread safe.
#define SCC_CAP_SCCFILE           0x80000000L   // Supports the MSSCCPRJ.SCC file
												//   (subject to user/administrator override)

#define SCC_EXCAP_THREADSAFE_QUERY_INFO 0x00000008L

/****************************************************************************
	The following flags are used for the print call-back that the IDE
	provides on SccInitialize.  
	
	If the IDE supports cancel, it may get one of the Cancel messages.
	In this case, the provider will inform the IDE to show the Cancel
	button with SCC_MSG_STARTCANCEL.  After this, any set of normal
	messages may be sent.  If any of these return SCC_MSG_RTN_CANCEL,
	then the provider will quit the operation and return.  The Provider
	will also poll periodically with SCC_MSG_DOCANCEL to see if the
	user has canceled the operation.  When all operations are done, or
	the user has canceled, SCC_MSG_STOPCANCEL will be sent through.

	The SCC_MSG_INFO, WARNING, and ERROR types are used for messages that
	get displayed in the scrolling list of messages.  SCC_MSG_STATUS is
	a special type that indicates that the text should show up in a 
	status bar or temporary display area.  This message type should not
	remain permanently in the list.
****************************************************************************/
enum
{
	// Return codes
	SCC_MSG_RTN_CANCEL=-1,				// Returned from call-back to indicate cancel
	SCC_MSG_RTN_OK=0,					// Returned from call-back to continue
	// Message types
	SCC_MSG_INFO=1,						// Message is informational
	SCC_MSG_WARNING,					// Message is a warning
	SCC_MSG_ERROR,						// Message is an error
	SCC_MSG_STATUS,						// Message is meant for status bar
	// IDE supports Cancel operation
	SCC_MSG_DOCANCEL,					// No text, IDE returns 0 or SCC_MSG_RTN_CANCEL
	SCC_MSG_STARTCANCEL,				// Start a cancel loop
	SCC_MSG_STOPCANCEL					// Stop the cancel loop
};

#ifndef _LPTEXTOUTPROC_DEFINED
#define _LPTEXTOUTPROC_DEFINED
typedef long (*LPTEXTOUTPROC) (LPCSTR, DWORD);
#endif /* _LPTEXTOUTPROC_DEFINED */


/****************************************************************************
    nOption values for SccSetOption.
****************************************************************************/
#define SCC_OPT_EVENTQUEUE      0x00000001L     // Set status of the event queue
#define SCC_OPT_USERDATA        0x00000002L     // Specify user data for 
												//	SCC_OPT_NAMECHANGEPFN
#define SCC_OPT_HASCANCELMODE	0x00000003L		// The IDE can handle Cancel 
												//	of long running operations
#define SCC_OPT_NAMECHANGEPFN   0x00000004L     // Set a callback for name changes
#define SCC_OPT_SCCCHECKOUTONLY 0x00000005L		// Disable SS explorer checkout, 
												//  and don't set working dir
#define SCC_OPT_SHARESUBPROJ 0x00000006L		// Allow destination dir for share


/* SCC_OPT_EVENTQUEUE values */
#define SCC_OPT_EQ_DISABLE      0x00L           // Suspend event queue activity
#define SCC_OPT_EQ_ENABLE       0x01L           // Enable event queue logging

/* SCC_OPT_NAMECHANGEPFN callback typedef */
typedef void (*OPTNAMECHANGEPFN)(LONG pvCallerData, 
                    LPCSTR pszOldName, LPCSTR pszNewName);

/****************************************************************************
	Values for SCC_OPT_HASCANCELMODE.  By default, it is assumed that the IDE
	will not allow for canceling a long running operation.  The provider must
	handle this on their own in this case.  If the IDE, however, sets this
	option to SCC_OPT_HCM_YES, it means that it will handle canceling the
	operation.  In this case, use the SCC_MSG_xxx flags with the output
	call-back to tell the IDE what messages to display while the operation
	is running.
****************************************************************************/
#define SCC_OPT_HCM_NO			0L				// (Default) Has no cancel mode,
												//	Provider must supply if desired
#define SCC_OPT_HCM_YES			1L				// IDE handles cancel

/****************************************************************************
	Values for SCC_OPT_SCCCHECKOUTONLY.  By default, it is assumed that 
	the user may use the gui to get and checkout files from this project,
	and that a working dir should be set,  If this option is explicitly turned on,
	then no working dir is set for the project, and the files may only be gotten
	or checked in or out from scc integration, never from the gui.
****************************************************************************/
#define SCC_OPT_SCO_NO			0L				// (Default) OK to checkout from GUI
												//	Working dir is set.
#define SCC_OPT_SCO_YES			1L				// no GUI checkout, no working dir



/****************************************************************************
	The SCC_PROC_NAMES macro will expand for each function in the API.
	You must define SCC_PROC to do what you want.  For this file, it
	creates an enum (see code just after the table), by concatenating
	SCC_CMD_ with the function name.  This enum is used in the sccuser.cpp
	module as an index for function pointers of the provider.  sccuser.cpp
	also redefines SCC_PROC to expand a table of character string function
	names.  This table, when used with the enum, allows for adding and
	removing of function names from the API will only a recompile required
	to fix up addresses and offsets.  You can #undef SCC_PROC and redefine
	it to do other tables of your own if required.
****************************************************************************/
#define SCC_PROC(nam)   SCC_CMD_ ## nam

#define SCC_PROC_NAMES \
    SCC_PROC(SccInitialize), \
    SCC_PROC(SccUninitialize), \
    SCC_PROC(SccOpenProject), \
    SCC_PROC(SccCloseProject), \
    SCC_PROC(SccGet), \
    SCC_PROC(SccCheckout), \
    SCC_PROC(SccUncheckout), \
    SCC_PROC(SccCheckin), \
    SCC_PROC(SccAdd), \
    SCC_PROC(SccRemove), \
    SCC_PROC(SccRename), \
    SCC_PROC(SccDiff), \
    SCC_PROC(SccDirDiff), \
    SCC_PROC(SccHistory), \
    SCC_PROC(SccProperties), \
    SCC_PROC(SccRunScc), \
    SCC_PROC(SccGetCommandOptions), \
    SCC_PROC(SccQueryInfo), \
    SCC_PROC(SccDirQueryInfo), \
    SCC_PROC(SccGetEvents), \
    SCC_PROC(SccGetProjPath), \
    SCC_PROC(SccPopulateList), \
    SCC_PROC(SccAddFromScc), \
    SCC_PROC(SccSetOption), \
    SCC_PROC(SccGetVersion), \
    SCC_PROC(SccCreateSubProject), \
    SCC_PROC(SccGetParentProjectPath), \
    SCC_PROC(SccBeginBatch), \
    SCC_PROC(SccEndBatch), \
    SCC_PROC(SccIsMultiCheckoutEnabled), \
    SCC_PROC(SccWillCreateSccFile)

enum 
{
    SCC_PROC_NAMES
};

#define SCC_CMD_COUNT           (SCC_CMD_SccWillCreateSccFile + 1)




/****************************************************************************
    Following are the ASCII definitions of the functions.
****************************************************************************/



/*******************************************************************************
	Returns a 4 byte version of the provider.  This can be used to check for 
	SCC spec conformance.
*******************************************************************************/
SCCEXTERNC LONG EXTFUN SccGetVersion(void);

/*******************************************************************************
	Call this function once per instance of a provider.
*******************************************************************************/
SCCEXTERNC SCCRTN EXTFUN SccInitialize(
                        LPVOID * ppContext, 
                        HWND hWnd, 
                        LPCSTR lpCallerName,
                        LPSTR lpSccName, 
                        LPLONG lpSccCaps, 
                        LPSTR lpAuxPathLabel, 
                        LPLONG pnCheckoutCommentLen, 
                        LPLONG pnCommentLen
                        );

/*******************************************************************************
	Call this function once for every instance of a provider, when it is going
	away.  You must call SccInitialize before calling this function, and should
	not call it with any open projects.
*******************************************************************************/
SCCEXTERNC SCCRTN EXTFUN SccUninitialize(
                        LPVOID pContext
                        );

/*******************************************************************************
	Opens a project.  This function should never be called with an already open
	project on pContext.  The lpUser, lpProjName, and lpAuxProjPath values
	may be modified by the provider.
*******************************************************************************/
SCCEXTERNC SCCRTN EXTFUN SccOpenProject(
                        LPVOID pContext,
                        HWND hWnd, 
                        LPSTR lpUser,
                        LPSTR lpProjName,
                        LPCSTR lpLocalProjPath,
                        LPSTR lpAuxProjPath,
                        LPCSTR lpComment,
                        LPTEXTOUTPROC lpTextOutProc,
                        LONG dwFlags
                        );

/*******************************************************************************
	Called to close a project opened by SccOpenProject.
*******************************************************************************/
SCCEXTERNC SCCRTN EXTFUN SccCloseProject(
                        LPVOID pContext
                        );

/*******************************************************************************
	Prompts the user for provider project information.  This may include the
	path to a certain project.  The caller must be prepared to accept changes
	to lpUser, lpProjName, lpLocalPath, and lpAuxProjPath.  lpProjName and
	lpAuxProjPath are then used in a call to SccOpenProject.  They should not
	be modified by the caller upon return.  The caller should avoid displaying
	these two parameters upon return, as the provider might use a formatted
	string that is not ready for view.
*******************************************************************************/
SCCEXTERNC SCCRTN EXTFUN SccGetProjPath(
                        LPVOID pContext, 
                        HWND hWnd, 
                        LPSTR lpUser,
                        LPSTR lpProjName, 
                        LPSTR lpLocalPath,
                        LPSTR lpAuxProjPath,
                        BOOL bAllowChangePath,
                        LPBOOL pbNew
                        );

/*******************************************************************************
	Retrieves a read only copy of a set of files.  The array is a set of files
	on the local disk.  The paths must be fully qualified.
*******************************************************************************/
SCCEXTERNC SCCRTN EXTFUN SccGet(
                        LPVOID pContext, 
                        HWND hWnd, 
                        LONG nFiles, 
                        LPCSTR* lpFileNames, 
                        LONG dwFlags,
                        LPCMDOPTS pvOptions
                        );

/*******************************************************************************
	Checks out the array of files.  The array is a set of fully qualified local
	path names.
*******************************************************************************/
SCCEXTERNC SCCRTN EXTFUN SccCheckout(
                        LPVOID pContext, 
                        HWND hWnd, 
                        LONG nFiles, 
                        LPCSTR* lpFileNames, 
                        LPCSTR lpComment, 
                        LONG dwFlags,
                        LPCMDOPTS pvOptions
                        );

/*******************************************************************************
	Undo a checkout of an array of files.  The array is a set of fully qualified
	local path names.
*******************************************************************************/
SCCEXTERNC SCCRTN EXTFUN SccUncheckout(
                        LPVOID pContext, 
                        HWND hWnd, 
                        LONG nFiles, 
                        LPCSTR* lpFileNames, 
                        LONG dwFlags,
                        LPCMDOPTS pvOptions
                        );

/*******************************************************************************
	Make the modifications the user has made to an array of files permanent. The
	file names must be fully qualified local paths.
*******************************************************************************/
SCCEXTERNC SCCRTN EXTFUN SccCheckin(
                        LPVOID pContext, 
                        HWND hWnd, 
                        LONG nFiles, 
                        LPCSTR* lpFileNames, 
                        LPCSTR lpComment, 
                        LONG dwFlags,
                        LPCMDOPTS pvOptions
                        );

/*******************************************************************************
	Add an array of fully qualified files to the source control system.  The 
	array of flags describe the type of file.  See the SCC_FILETYPE_xxxx flags.
*******************************************************************************/
SCCEXTERNC SCCRTN EXTFUN SccAdd(
                        LPVOID pContext, 
                        HWND hWnd, 
                        LONG nFiles, 
                        LPCSTR* lpFileNames, 
                        LPCSTR lpComment, 
                        LONG * pdwFlags,
                        LPCMDOPTS pvOptions
                        );

/*******************************************************************************
	Removes the array of fully qualified files from the source control system.
	The files are not removed from the user's disk, unless advanced options
	are set by the user.  Advaned options are defined by the provider.
*******************************************************************************/
SCCEXTERNC SCCRTN EXTFUN SccRemove(
                        LPVOID pContext, 
                        HWND hWnd, 
                        LONG nFiles, 
                        LPCSTR* lpFileNames,
                        LPCSTR lpComment,
                        LONG dwFlags,
                        LPCMDOPTS pvOptions
                        );

/*******************************************************************************
	Enables us to avoid prompting for user input more than once during a
	"batched" operation. SccBeginBatch and SccEndBatch are used as a pair
	to indicate the beginning and end of an operation. They cannot be nested.
	SccBeginBatch sets flag indicating that a batch operation is in progress.
*******************************************************************************/
SCCEXTERNC SCCRTN EXTFUN SccBeginBatch(void);

/*******************************************************************************
	Enables us to avoid prompting for user input more than once during a
	"batched" operation. SccBeginBatch and SccEndBatch are used as a pair
	to indicate the beginning and end of an operation. They cannot be nested.
	SccEndBatch clears the batch operation in progress flag.
*******************************************************************************/
SCCEXTERNC SCCRTN EXTFUN SccEndBatch(void);

/*******************************************************************************
	Renames the given file to a new name in the source control system.  The
	provider should not attempt to access the file on disk.  It is the
	caller's responsibility to rename the file on disk.
*******************************************************************************/
SCCEXTERNC SCCRTN EXTFUN SccRename(
                        LPVOID pContext, 
                        HWND hWnd, 
                        LPCSTR lpFileName,
                        LPCSTR lpNewName
                        );

/*******************************************************************************
	Show the differences between the local users fully qualified file and the
	version under source control.
*******************************************************************************/
SCCEXTERNC SCCRTN EXTFUN SccDiff(
                        LPVOID pContext, 
                        HWND hWnd, 
                        LPCSTR lpFileName, 
                        LONG dwFlags,
                        LPCMDOPTS pvOptions
                        );

/*******************************************************************************
	Show the differences between the local user's fully qualified directory and
	the project under source control.
*******************************************************************************/
SCCEXTERNC SCCRTN EXTFUN SccDirDiff(
                        LPVOID pContext, 
                        HWND hWnd, 
                        LPCSTR lpDirName, 
                        LONG dwFlags,
                        LPCMDOPTS pvOptions
                        );

/*******************************************************************************
	Show the history for an array of fully qualified local file names.  The
	provider may not always support an array of files, in which case only the
	first files history will be shown.
*******************************************************************************/
SCCEXTERNC SCCRTN EXTFUN SccHistory(
                        LPVOID pContext, 
                        HWND hWnd, 
                        LONG nFiles, 
                        LPCSTR* lpFileNames, 
                        LONG dwFlags,
                        LPCMDOPTS pvOptions
                        );

/*******************************************************************************
	Show the properties of a fully qualified file.  The properties are defined
	by the provider and may be different for each one.
*******************************************************************************/
SCCEXTERNC SCCRTN EXTFUN SccProperties(
                        LPVOID pContext, 
                        HWND hWnd, 
                        LPCSTR lpFileName
                        );

/*******************************************************************************
	Examine a list of fully qualified files for their current status.  The
	return array will be a bitmask of SCC_STATUS_xxxx bits.  A provider may
	not support all of the bit types.  For example, SCC_STATUS_OUTOFDATE may
	be expensive for some provider to provide.  In this case the bit is simply
	not set.
*******************************************************************************/
SCCEXTERNC SCCRTN EXTFUN SccQueryInfo(
                        LPVOID pContext, 
                        LONG nFiles, 
                        LPCSTR* lpFileNames, 
                        LPLONG lpStatus
                        );

/*******************************************************************************
	Examine a list of fully qualified dirs for their current status.  The
	return array will be a bitmask of SCC_DIRSTATUS_xxxx bits.  A provider may
	not support all of the bit types.  For example, SCC_DIRSTATUS_EMPTYPROJ may
	be expensive for some provider to provide.  In this case the bit is simply
	not set.
*******************************************************************************/
SCCEXTERNC SCCRTN EXTFUN SccDirQueryInfo(
                        LPVOID pContext, 
                        LONG nDirs, 
                        LPCSTR* lpDirNames, 
                        LPLONG lpStatus
                        );

/*******************************************************************************
	Like SccQueryInfo, this function will examine the list of files for their
	current status.  In addition, it will use the pfnPopulate function to 
	notify the caller when a file does not match the critera for the nCommand.
	For example, if the command is SCC_COMMAND_CHECKIN, and a file in the list
	is not checked out, then the callback is used to tell the caller this.  
	Finally, the provider may find other files that could be part of the command
	and add them.  This allows a VB user to check out a .bmp file that is used
	by their VB project, but does not appear in the VB makefile.
*******************************************************************************/
SCCEXTERNC SCCRTN EXTFUN SccPopulateList(
                        LPVOID pContext, 
                        enum SCCCOMMAND nCommand, 
                        LONG nFiles, 
                        LPCSTR* lpFileNames, 
                        POPLISTFUNC pfnPopulate, 
                        LPVOID pvCallerData,
						LPLONG lpStatus, 
						LONG dwFlags
                        );

/*******************************************************************************
	SccGetEvents runs in the background checking the status of files that the
	caller has asked about (via SccQueryInfo).  When the status changes, it 
	builds a list of those changes that the caller may exhaust on idle.  This
	function must take virtually no time to run, or the performance of the 
	caller will start to degrade.  For this reason, some providers may choose
	not to implement this function.
*******************************************************************************/
SCCEXTERNC SCCRTN EXTFUN SccGetEvents(
                        LPVOID pContext, 
                        LPSTR lpFileName,
                        LPLONG lpStatus,
                        LPLONG pnEventsRemaining
                        );

/*******************************************************************************
	This function allows a user to access the full range of features of the
	source control system.  This might involve launching the native front end
	to the product.  Optionally, a list of files are given for the call.  This
	allows the provider to immediately select or subset their list.  If the
	provider does not support this feature, it simply ignores the values.
*******************************************************************************/
SCCEXTERNC SCCRTN EXTFUN SccRunScc(
                        LPVOID pContext, 
                        HWND hWnd, 
                        LONG nFiles, 
                        LPCSTR* lpFileNames
                        );

/*******************************************************************************
	This function will prompt the user for advaned options for the given
	command.  Call it once with ppvOptions==NULL to see if the provider
	actually supports the feature.  Call it again when the user wants to see
	the advaned options (usually implemented as an Advaned button on a dialog).
	If a valid *ppvOptions is returned from the second call, then this value
	becomes the pvOptions value for the SccGet, SccCheckout, SccCheckin, etc...
	functions.
*******************************************************************************/
SCCEXTERNC SCCRTN EXTFUN SccGetCommandOptions(
                        LPVOID pContext, 
                        HWND hWnd, 
                        enum SCCCOMMAND nCommand,
                        LPCMDOPTS * ppvOptions
                        );


/*******************************************************************************
	This function allows the user to browse for files that are already in the
	source control system and then make those files part of the current project.
	This is handy, for example, to get a common header file into the current
	project without having to copy the file.  The return array of files
	(lplpFileNames) contains the list of files that the user wants added to
	the current makefile/project.
*******************************************************************************/
SCCEXTERNC SCCRTN EXTFUN SccAddFromScc(
                        LPVOID pContext, 
                        HWND hWnd, 
                        LPLONG pnFiles,
                        LPCSTR** lplpFileNames
                        );

/*******************************************************************************
	SccSetOption is a generic function used to set a wide variety of options.
	Each option starts with SCC_OPT_xxx and has its own defined set of values.
*******************************************************************************/
SCCEXTERNC SCCRTN EXTFUN SccSetOption(
                        LPVOID pContext,
                        LONG nOption,
                        LONG dwVal
                        );

/*******************************************************************************
	Creates a subproject with the given name under the existing parent project. 
	If a subproject with the name already exists the function can change 
	the default name in order to create a unique one, for example by adding 
	"_<number>" to it. The caller must be prepared to accept changes
	to lpUser, lpSubProjPath, and lpAuxProjPath. lpSubProjPath and
	lpAuxProjPath are then used in a call to SccOpenProject.  They should not
	be modified by the caller upon return.  The caller should avoid displaying
	these two parameters upon return, as the provider might use a formatted
	string that is not ready for view.
*******************************************************************************/
SCCEXTERNC SCCRTN EXTFUN SccCreateSubProject(
                        LPVOID pContext, 
                        HWND hWnd, 
                        LPSTR lpUser,
                        LPCSTR lpParentProjPath, 
                        LPCSTR lpSubProjName,
                        LPSTR lpAuxProjPath,
                        LPSTR lpSubProjPath
                        );

/*******************************************************************************
	Returns parent path of the given project (the project must exist). 
	For the store root folder, the function should return the given path 
	(i.e. the same root folder path). The caller must be prepared to accept changes 
	to lpUser, lpParentProjPath, and lpAuxProjPath. lpParentProjPath and lpAuxProjPath 
	are then used in a call to SccOpenProject.  They should not be modified 
	by the caller upon return. The caller should avoid displaying these two parameters 
	upon return, as the provider might use a formatted string that is not ready for view.
*******************************************************************************/
SCCEXTERNC SCCRTN EXTFUN SccGetParentProjectPath(
                        LPVOID pContext, 
                        HWND hWnd, 
                        LPSTR lpUser,
                        LPCSTR lpProjPath, 
                        LPSTR lpAuxProjPath,
                        LPSTR lpParentProjPath
                        );

/*******************************************************************************
	Checks if multiple checkouts on a file are allowed.
*******************************************************************************/
SCCEXTERNC SCCRTN EXTFUN SccIsMultiCheckoutEnabled(
                        LPVOID pContext, 
						LPBOOL pbMultiCheckout
                        );

/*******************************************************************************
	Checks if the provider will create MSSCCPRJ.SCC files in the same
	directories as the given files if the given files are placed under source
	control. The file paths must be fully qualified.
*******************************************************************************/
SCCEXTERNC SCCRTN EXTFUN SccWillCreateSccFile(
                        LPVOID pContext, 
                        LONG nFiles,
                        LPCSTR* lpFileNames,
						LPBOOL pbSccFiles
                        );

#if defined( __cplusplus )
}
#endif





#endif // _SCC_DEFS

//******* EOF ********
