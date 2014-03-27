; The name of the installer
Name "Decoda"

; The file to write
OutFile "output\Decoda.exe"

; The default installation directory
InstallDir $PROGRAMFILES\Decoda

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\Decoda" "Install_Dir"

Icon 			install.ico
UninstallIcon	uninstall.ico

BrandingText	" "

LicenseData		"eula.rtf"
LicenseBkColor	/windows
;--------------------------------

; Pages

Page license
Page components
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

;------------------------------------------------------------------------------
; un.RemoveSharedDLL
;
; Decrements a shared DLLs reference count, and removes if necessary.
; Use by passing one item on the stack (the full path of the DLL).
; Note: for use in the main installer (not the uninstaller), rename the
; function to RemoveSharedDLL.
; 
; Usage:
;   Push $SYSDIR\myDll.dll
;   Call un.RemoveShareDLL
;

Function un.RemoveSharedDLL
  Exch $9
  Push $0
  ReadRegDword $0 HKLM Software\Microsoft\Windows\CurrentVersion\SharedDLLs $9
  StrCmp $0 "" remove
    IntOp $0 $0 - 1
    IntCmp $0 0 rk rk uk
    rk:
      DeleteRegValue HKLM Software\Microsoft\Windows\CurrentVersion\SharedDLLs $9
    goto Remove
    uk:
      WriteRegDWORD HKLM Software\Microsoft\Windows\CurrentVersion\SharedDLLs $9 $0
    Goto noremove
  remove:
    Delete /REBOOTOK $9
  noremove:
  Pop $0
  Pop $9
FunctionEnd


;------------------------------------------------------------------------------
; AddSharedDLL
;
; Increments a shared DLLs reference count.
; Use by passing one item on the stack (the full path of the DLL).
;
; Usage: 
;   Push $SYSDIR\myDll.dll
;   Call AddSharedDLL
;

Function AddSharedDLL
  Exch $9
  Push $0
  ReadRegDword $0 HKLM Software\Microsoft\Windows\CurrentVersion\SharedDLLs $9
  IntOp $0 $0 + 1
  WriteRegDWORD HKLM Software\Microsoft\Windows\CurrentVersion\SharedDLLs $9 $0
  Pop $0
  Pop $9
FunctionEnd 

;--------------------------------

; The stuff to install
Section "Decoda (required)"

  SectionIn RO
  
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ; Put file there
  File "..\bin\release\Decoda.exe"
  File "..\docs\output\Decoda.chm"
  File "..\bin\release\LuaInject.dll"
  File "..\bin\release\Update.dll"
  File "..\bin\release\dbghelp.dll"
  File "..\bin\release\msvcp120.dll"
  File "..\bin\release\msvcr120.dll"
  File "..\COPYING.TXT"

  ; Setup the deproj file association
  WriteRegStr HKCR ".deproj" "" "Decoda.deproj"
  WriteRegStr HKCR "Decoda.deproj\DefaultIcon" "" "$INSTDIR\Decoda.exe,1"
  WriteRegStr HKCR "Decoda.deproj\shell\open\command" "" '$INSTDIR\Decoda.exe "%1"'
							
  System::Call 'Shell32::SHChangeNotify(i 0x8000000, i 0, i 0, i 0)'

  ; Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\Decoda "Install_Dir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Decoda" "DisplayName" "Decoda"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Decoda" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Decoda" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Decoda" "NoRepair" 1
  
  WriteUninstaller "uninstall.exe"
 
 SectionEnd

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"

  CreateDirectory "$SMPROGRAMS\Decoda"
  CreateShortCut "$SMPROGRAMS\Decoda\Decoda.lnk" "$INSTDIR\Decoda.exe" "" "$INSTDIR\Decoda.exe" 0
  CreateShortCut "$SMPROGRAMS\Decoda\Decoda Help.lnk" "$INSTDIR\Decoda.chm" "" "$INSTDIR\Decoda.chm" 0
  CreateShortCut "$SMPROGRAMS\Decoda\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  
SectionEnd

; Optional section (can be disabled by the user)
Section "Open .lua files with Decoda"

	WriteRegStr HKCR ".lua" "" "Decoda.lua"
	WriteRegStr HKCR "Decoda.lua\DefaultIcon" "" "$INSTDIR\Decoda.exe,2"
	WriteRegStr HKCR "Decoda.lua\shell\open\command" "" '$INSTDIR\Decoda.exe "%1"'
				
	System::Call 'Shell32::SHChangeNotify(i 0x8000000, i 0, i 0, i 0)'

SectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Decoda"
  DeleteRegKey HKLM SOFTWARE\Decoda

  ReadRegStr $1 HKCR ".deproj" ""
  StrCmp $1 "Decoda.deproj" 0 "Skip-Decoda.deproj"
  DeleteRegKey HKCR ".deproj"
"Skip-Decoda.deproj:"

  ReadRegStr $1 HKCR ".lua" ""
  StrCmp $1 "Decoda.lua" 0 "Skip-Decoda.lua"
  DeleteRegKey HKCR ".lua"
"Skip-Decoda.lua:"

  ; Remove the file association registry keys
  DeleteRegKey HKCR "Decoda.lua"
  DeleteRegKey HKCR "Decoda.deproj"  
  DeleteRegKey HKCR "Applications\Decoda.exe"

  ; Remove files and uninstaller
  Delete $INSTDIR\Decoda.exe
  Delete $INSTDIR\Decoda.chm
  Delete $INSTDIR\Decoda.chw  
  Delete $INSTDIR\LuaInject.dll
  Delete $INSTDIR\Update.dll
  Delete $INSTDIR\dbghelp.dll
  Delete $INSTDIR\uninstall.exe
  Delete $INSTDIR\msvcp120.dll
  Delete $INSTDIR\msvcr120.dll
  Delete $INSTDIR\COPYING.txt
  
  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\Decoda\*.*"

  ; Remove directories used
  RMDir "$SMPROGRAMS\Decoda"
  RMDir "$INSTDIR"
  
  ; Remove the application data for the current user
  RMDir /r "$APPDATA\Decoda"
  
  System::Call 'Shell32::SHChangeNotify(i 0x8000000, i 0, i 0, i 0)'

SectionEnd
