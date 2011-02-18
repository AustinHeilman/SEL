Option Explicit
Dim strComputer, strKey, strSubKey
Dim objRegistry
Dim arrSubKeys()
Dim strDisplayName, strDisplayVersion, strInstallLocation
Const HKEY_LOCAL_MACHINE = &H80000002

strComputer = "."
'strKey = "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall"
strKey = "SOFTWARE\Microsoft\Windows\CurrentVersion\Component Based Servicing\Packages"

' BEGIN CALLOUT A
Set objRegistry = GetObject("winmgmts:"   & _
	"{impersonationLevel=Impersonate}!\\" & _
	strComputer & "\root\default:StdRegProv")
' END CALLOUT A

' BEGIN CALLOUT B
objRegistry.EnumKey HKEY_LOCAL_MACHINE, strKey, arrSubKeys
' END CALLOUT B

' BEGIN CALLOUT C
On Error Resume Next
For Each strSubKey In arrSubKeys
	objRegistry.GetStringValue HKEY_LOCAL_MACHINE, _
		strKey & "\" & strSubKey, "DisplayName", strDisplayName
	objRegistry.GetStringValue HKEY_LOCAL_MACHINE, _
		strKey & "\" & strSubKey, "DisplayVersion", strDisplayVersion
	objRegistry.GetStringValue HKEY_LOCAL_MACHINE, _
		strKey & "\" & strSubKey, "InstallLocation", strInstallLocation
	WScript.Echo strSubKey
	WScript.Echo String(Len(strSubKey), "-")
	WScript.Echo "Display name:     " & strDisplayName
	WScript.Echo "Display version:  " & strDisplayVersion
	WScript.Echo "Install location: " & strInstallLocation
	WScript.Echo
	
	If ( InStr(LCase(strSubKey), "kb931573") ) Then
		WScript.Quit
	End If

	strDisplayName = vbEmpty
	strDisplayVersion = vbEmpty
	strInstallLocation = vbEmpty
Next
' END CALLOUT C
