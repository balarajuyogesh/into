; NOTES
;
; Before building the installer you MUST run install_license.bat AND
; install_demo_images.bat.
;
; Get EnvVarUpdate.nsh from http://nsis.sourceforge.net/Path_Manipulation and
; copy it to C:\Program Files\NSIS\Include
;
; Create the installer with
; "C:\Program Files\NSIS\makensis" /nocd installer\windows\installer.nsi
;
; VERSION, LICENSEFILE, OUTFILE and COMPILER must be defined on makensis command line:
; makensis /DVERSION=... /DLICENSEFILE=... /DOUTFILE=... /DCOMPILER=...
; COMPILER is either msvc or mingw

!define APPNAME "Into"
!define APPNAMEANDVERSION "Into ${VERSION}"

; Main Install settings
Name "${APPNAMEANDVERSION}"
InstallDir "C:\Into\${VERSION}"
InstallDirRegKey HKLM "Software\${APPNAME}" ""
OutFile "${OUTFILE}"

; Modern interface settings
!include "MUI.nsh"
;!include "EnvVarUpdate.nsh"

!define MUI_ABORTWARNING

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE ${LICENSEFILE}
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

; Set languages (first is default language)
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_RESERVEFILE_LANGDLL

RequestExecutionLevel admin

!if ${COMPILER} == "msvc"
  !macro LIBFILE dir name
    File ${dir}\release\${name}.lib
  !macroend
!else
  !macro LIBFILE dir name
    File ${dir}\release\lib${name}.a
  !macroend
!endif

!macro LIBFILES dir name
  SetOutPath "$INSTDIR\bin"
  File ${dir}\release\${name}.dll
  SetOutPath "$INSTDIR\lib"
  !insertmacro LIBFILE ${dir} ${name}
!macroend


Section "Precompiled Binaries" BinarySection
  SetOverwrite on
  
  SetOutPath "$INSTDIR"
  ;File /oname=intopii.ico "z:\www\favicon.ico"
  File /oname=COPYING ${LICENSEFILE}

  SetOutPath "$INSTDIR\bin"
  File installer\windows\postinst.bat
  File installer\windows\intoenv.bat
  File demos\utils\release\demoutils.dll
  File tools\jsrunner\release\jsrunner.exe

  SetOutPath "$INSTDIR\bin\script"
  File script\release\piiscript.dll
  SetOutPath "$INSTDIR\bin\declarative"
  File declarative\release\piideclarative.dll

  !insertmacro LIBFILES plugins\base piibase
  !insertmacro LIBFILES plugins\calibration piicalibration
  !insertmacro LIBFILES plugins\camera piicamera
  !insertmacro LIBFILES plugins\classification piiclassification
  !insertmacro LIBFILES plugins\colors piicolors
  !insertmacro LIBFILES plugins\database piidatabase
  !insertmacro LIBFILES plugins\dsp piidsp
  !insertmacro LIBFILES plugins\features piifeatures
  !insertmacro LIBFILES plugins\flowcontrol piiflowcontrol
  !insertmacro LIBFILES plugins\geometry piigeometry
  !insertmacro LIBFILES plugins\image piiimage
  !insertmacro LIBFILES plugins\io piiio
  !insertmacro LIBFILES plugins\matching piimatching
  !insertmacro LIBFILES plugins\network piinetwork
  !insertmacro LIBFILES plugins\optimization piioptimization
  !insertmacro LIBFILES plugins\statistics piistatistics
  !insertmacro LIBFILES plugins\texture piitexture
  !insertmacro LIBFILES plugins\tracking piitracking
  !insertmacro LIBFILES plugins\transforms piitransforms
  !insertmacro LIBFILES plugins\video piivideo
  !insertmacro LIBFILES plugins\gui\classification piiclassificationgui
  !insertmacro LIBFILES core piicore2
  !insertmacro LIBFILES ydin piiydin2
  !insertmacro LIBFILES gui piigui2
  !insertmacro LIBFILES demos\utils demoutils

  ;File ..\..\lib\c*100.dll
  
  SetOutPath "$INSTDIR\lib" 
  !insertmacro LIBFILE script piiscript
  !insertmacro LIBFILE declarative piideclarative
  
  SetOutPath "$INSTDIR\demos"
  File /oname=intodemo.exe tools\demo\release\demo.exe
  File /r /x debug demos\*.exe
  File /r demos\*.png
  File /r demos\README
  SetOutPath "$INSTDIR\demos\images"
  File /r Z:\development\intodemo\images\*

  SetShellVarContext all
  CreateDirectory "$SMPROGRAMS\Into"
  CreateShortCut "$SMPROGRAMS\Into\Into Command Prompt - ${COMPILER}.lnk" "$WINDIR\System32\cmd.exe" "/K $INSTDIR\bin\intoenv.bat" "$WINDIR\System32\cmd.exe"
  CreateShortCut "$SMPROGRAMS\Into\Documentation Site.lnk" "http://doc.intopii.com/" "" "$WINDIR\System32\shell32.dll" 14
  CreateShortCut "$SMPROGRAMS\Into\Uninstall.lnk" "$INSTDIR\uninstall.exe"

  ;${EnvVarUpdate} $0 "PATH" "A" "HKCU" "$INSTDIR\bin"
SectionEnd

Section "Source Codes" SourceSection
  SetOverwrite on

  SetOutPath "$INSTDIR"
  File INSTALL

  SetOutPath "$INSTDIR\src"
  File *.pro
  File *.pri
  File *.h
  File installer\windows\install.bat

  SetOutPath "$INSTDIR\src\plugins"
  File /r /x face /x ocr /x .svn /x release /x debug /x Makefile* /x ui_*.h /x *~ /x *.rc plugins\*

  SetOutPath "$INSTDIR\src\3rdparty"
  File /r /x opencv /x qwt /x libsvm /x .svn /x release /x debug /x Makefile* /x include /x currera /x lib /x *~ /x *.rc 3rdparty\*

  SetOutPath "$INSTDIR\src\core"
  File /r /x .svn /x release /x debug /x Makefile* /x ui_*.h /x *~ /x *.rc core\*

  SetOutPath "$INSTDIR\src\ydin"
  File /r /x .svn /x release /x debug /x Makefile* /x ui_*.h /x *~ /x *.rc ydin\*

  SetOutPath "$INSTDIR\src\gui"
  File /r /x .svn /x release /x debug /x Makefile* /x ui_*.h /x *~ /x *.rc gui\*

  SetOutPath "$INSTDIR\src\tools"
  File tools\tools.pro
  SetOutPath "$INSTDIR\src\tools\demo"
  File /r /x .svn /x release /x debug /x Makefile* /x ui_*.h /x *~ /x *.rc tools\demo\*
  SetOutPath "$INSTDIR\src\tools\jsrunner"
  File /r /x .svn /x release /x debug /x Makefile* /x ui_*.h /x *~ /x *.rc tools\jsrunner\*
  

  SetOutPath "$INSTDIR\src\script"
  File /r /x .svn /x release /x debug /x Makefile* /x ui_*.h /x *~ /x *.rc script\*

  SetOutPath "$INSTDIR\src\declarative"
  File /r /x .svn /x release /x debug /x Makefile* /x ui_*.h /x *~ /x *.rc declarative\*

  SetOutPath "$INSTDIR\src\demos"
  File /r /x .svn /x geometry /x release /x debug /x Makefile* /x ui_*.h /x *~ /x *.rc /x image.png /x *.dll demos\*

SectionEnd

;Section "Documentation and Examples" DocumentationSection
;  SetOverwrite on
;
;  SetOutPath "$INSTDIR\doc"
  ;File /r "Z:\www\into_api\*"
;
;  SetOutPath "$INSTDIR\doc\examples\threshold"
;
;  File doc\examples\Threshold.cc
;  File doc\examples\threshold.pro
;
;  SetShellVarContext all
;  CreateShortCut "$SMPROGRAMS\Into\Local Documentation.lnk" "$INSTDIR\doc\index.html"
;SectionEnd

Section -FinishSection
  ExecWait '"$INSTDIR\bin\postinst.bat" "$INSTDIR"'
  WriteRegStr HKLM "Software\${APPNAME}" "" "$INSTDIR"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "DisplayName" "${APPNAME}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "UninstallString" "$INSTDIR\uninstall.exe"
  WriteUninstaller "$INSTDIR\uninstall.exe"
SectionEnd

; Modern install component descriptions
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${BinarySection} "Prebuilt binaries needed for run-time operation and for linking applications to Into."
  !insertmacro MUI_DESCRIPTION_TEXT ${SourceSection} "Source codes of the Into platform and the demo programs."
  ;!insertmacro MUI_DESCRIPTION_TEXT ${DocumentationSection} "Local copy of API documentation in HTML format and example programs."
!insertmacro MUI_FUNCTION_DESCRIPTION_END

; Initialization
Function .onInit
  ; Set binary section as selected and read-only
  IntOp $0 ${SF_SELECTED} | ${SF_RO}
  SectionSetFlags ${BinarySection} $0
FunctionEnd


;Uninstall section
Section Uninstall
  ;Remove from registry...
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}"
  DeleteRegKey HKLM "SOFTWARE\${APPNAME}"

  SetShellVarContext all
  ; Delete Shortcuts
  RMDir /r "$SMPROGRAMS\Into"

  ; Delete all files
  RMDir /r "$INSTDIR"

  ;${un.EnvVarUpdate} $0 "PATH" "R" "HKCU" "$INSTDIR\bin"
SectionEnd

BrandingText "Intopii - Making Things Think"
