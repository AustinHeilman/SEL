'--------------------------
' VAIO Updater Script
' Source: http://msdn2.microsoft.com/en-us/library/aa387102.aspx
'--------------------------
' WSH Documentation:
' http://www.pctools.com/guides/scripting/detail/144/?act=reference
'--------------------------
' History
' Austin Heilman (04-05-07)
'	- Added boxes around steps in the process.
'	- Replaced proxy settings in IE with NetSH commands (breaks XP compatibility)
'	- Updated batch file to support 'Run as Administrator' in Vista
' Austin Heilman (04-04-07)
'	- Added CSV file and support for error codes when updating.
' Austin Heilman (04-03-07)
'	- Added proxy setup support
'	- Broke code into functions
'
'--------------------------
' GLOBAL VARIABLES
Const VERSION_ID = "1.05"
Const LAST_MODIFIED = "4/24/2007 10:10:29am (Austin Heilman)"
Const CONST_TITLE = "VAIO Vista Updater"
Const PROXY_ADDRESS = "10.0.1.10:3128"
'Get a windows shell object - Needed to interact with windows outside of the scripts
Set win_shell = WScript.CreateObject("WScript.Shell")
'--------------------------

Call DoHeader()
WScript.Echo "| WARNING : "
WScript.Echo "| ALLOW THIS SCRIPT TO FULLY COMPLETE AND REMOVE PROXY SETTINGS."
WScript.Echo "| EARLY TERMINATION OF THIS WINDOW WILL RESULT IN THE CUSTOMER"
WScript.Echo "| HAVING NO INTERNET CONNECTIVITY."
Call DoFooter()

WScript.Sleep(1)

Call DoHeader()
WScript.Echo "| VAIO (Vista) Updater (v"&VERSION_ID&") "
WScript.Echo "| Last Updated "&LAST_MODIFIED
Call DoFooter()

'Call SetProxy(win_shell)
WScript.Echo
Call RunUpdater()
WScript.Echo
'Call RemoveProxy(win_shell)

Call DoHeader()
WScript.Echo "| Press enter to quit"
Call DoFooter()
WScript.StdIn.Readline

Function RunUpdater()
	' INITIALIZE ERROR HANDLING
		
	On Error Resume Next

	Set updateSession = CreateObject("Microsoft.Update.Session")
	Set updateSearcher = updateSession.CreateupdateSearcher()

	Call DoHeader()
	WScript.Echo "|	Searching for updates...     "
	Set searchResult = updateSearcher.Search("IsInstalled=0 and Type='Software'")
	If ( Err.Number <> 0 ) Then
		WScript.Echo "| Error attempting to connect to updates site!"
		WScript.Echo "| Error: " & Err.Number
		WScript.Echo "| Error (Hex): 0x" & Hex(Err.Number)
		WScript.Echo "| Source: " &  Err.Source
		err_desc = Err.Description
		If ( Len(err_desc) > 0 ) Then
			WScript.Echo "| Description: " &  err_desc
		Else
			err_desc = FindErrorMeaning("DecErrorCode", Err.Number)
			WScript.Echo "| Error Description: "
			WScript.Echo err_desc
		End If
		Err.Clear
		Call DoFooter()
		Exit Function
	End If
		
	WScript.Echo "|	List of applicable items on the machine:"
	For I = 0 To searchResult.Updates.Count-1
		Set update = searchResult.Updates.Item(I)
		WScript.Echo "| " & I + 1 & "> " & update.Title
	Next
	If searchResult.Updates.Count = 0 Then
		WScript.Echo "| There are no applicable updates."
		Call DoFooter()
		Exit Function
	End If
	DoFooter()

	Call DoHeader()
	WScript.Echo "|	Creating collection of updates to download:"
	Set updatesToDownload = CreateObject("Microsoft.Update.UpdateColl")
	For I = 0 to searchResult.Updates.Count-1
		Set update = searchResult.Updates.Item(I)
		WScript.Echo "| " & I + 1 & "> adding: " & update.Title
		updatesToDownload.Add(update)
	Next
	Call DoFooter()

	Call DoHeader()
	WScript.Echo "|	Downloading updates..."
	Call DoFooter()
	
	
	Set downloader = updateSession.CreateUpdateDownloader()
	downloader.Updates = updatesToDownload
	downloader.Download()
	total_downloaded = 0
	
	Call DoHeader()
	WScript.Echo  vbCRLF & "|	Download results:"
	For I = 0 To searchResult.Updates.Count-1
		Set update = searchResult.Updates.Item(I)
		WScript.Echo I + 1 & "> " & update.Title
		If update.IsDownloaded Then
			WScript.Echo "|    + Downloaded."
			total_downloaded = total_downloaded + 1
		Else
			WScript.Echo "|    - WAS NOT DOWNLOADED!"
		End If
	Next

	If ( total_downloaded < 1 ) Then
		WScript.Echo "|"
		WScript.Echo "| UPDATES FAILED. NO UPDATES COULD BE DOWNLOADED!"
		WScript.Echo "|"
		Exit Function
	ElseIf ( total_downloaded < searchResult.Updates.Count ) Then
		WScript.Echo "|"
		WScript.Echo "| WARNING: "&total_downloaded&"/"&searchResult.Updates.Count&" updates downloaded."
		WScript.Echo "|"
	End If
	Call DoFooter()

	Set updatesToInstall = CreateObject("Microsoft.Update.UpdateColl")

	Call DoHeader()
	WScript.Echo "	Downloaded updates that will be installed:"
	For I = 0 To searchResult.Updates.Count-1
		set update = searchResult.Updates.Item(I)
		If update.IsDownloaded = true Then
			WScript.Echo "| " & I + 1 & "> " & update.Title
			updatesToInstall.Add(update)
		End If
	Next
	Call DoFooter()

	'WScript.Echo  vbCRLF & "Would you like to install updates now? (Y/N)"
	'str_input = WScript.StdIn.Readline
	str_input = "Y"
	
	If (str_input = "N" or str_input = "n") Then
		Exit Function
	ElseIf (str_input = "Y" or str_input = "y") Then
		Call DoHeader()
		WScript.Echo "|	Installing updates..."
		Set installer = updateSession.CreateUpdateInstaller()
		installer.Updates = updatesToInstall
		Set installationResult = installer.Install()
		WScript.Echo "|"
		'Output results of install
		WScript.Echo "|	Install Result Code: " & installationResult.ResultCode
		WScript.Echo "|	Reboot Required    : " & installationResult.RebootRequired
		Call DoFooter()
		
		Call DoHeader()
		WScript.Echo "| Updates installed and results:"

		For I = 0 to updatesToInstall.Count - 1
			'WScript.Echo I + 1 & "> " & updatesToInstall.Item(i).Title & ": " & installationResult.GetUpdateResult(i).ResultCode
			install_result = ""
			Select Case Int(installationResult.GetUpdateResult(i).ResultCode)
				Case 1
					install_result = " = In progress."
				Case 2
					install_result = " + Installed."
				Case 3
					install_result = " ! Operation complete, but with errors."
				Case 4
					install_result = "!!! OPERATION FAILED !!!"
				Case 5
					install_result = "!!! OPERATION ABORTED !!!"
				Case Else
					install_result = "Install result ID unknown ("&installationResult.GetUpdateResult(i).ResultCode&")"
			End Select
			WScript.Echo "|" & I + 1 & "> " & updatesToInstall.Item(i).Title
			WScript.Echo "|	" & install_result
		Next
		DoFooter()
	End If
End Function

Function SetProxy(ByRef win_shell)
	Call DoHeader()
	result = RunExec(win_shell, "netsh.exe winhttp set proxy "&PROXY_ADDRESS)
	WScript.Echo "| Proxy set as "&PROXY_ADDRESS
	Call DoFooter()
	SetProxy = True

	'win_shell.RegWrite "HKCU\Software\Microsoft\Windows\CurrentVersion\Internet Settings\ProxyEnable","1","REG_DWORD"
	'win_shell.RegWrite "HKCU\Software\Microsoft\Windows\CurrentVersion\Internet Settings\ProxyServer",PROXY_ADDRESS,"REG_SZ"
	'Kludge needed to get the proxy settings to work with the updater?!
	'Set obj_ie = CreateObject("InternetExplorer.Application")
	'WScript.Sleep(600)
	'obj_ie.Navigate "about:blank"
	'obj_ie.Visible = 1
	'WScript.Sleep(600)
	'obj_ie.Quit
End Function

Function RemoveProxy(ByRef win_shell)
	Call DoHeader()
	result = RunExec(win_shell, "netsh.exe winhttp reset proxy")
	WScript.Echo "| Proxy settings removed..."
	Call DoFooter()
	'win_shell.RegWrite "HKCU\Software\Microsoft\Windows\CurrentVersion\Internet Settings\ProxyEnable","0","REG_DWORD"
	'win_shell.RegDelete "HKCU\Software\Microsoft\Windows\CurrentVersion\Internet Settings\ProxyServer"'
End Function

Function FindErrorMeaning(col_name, err_id)
	On Error Resume Next

	Const CSV_DEBUG = 0
	Const ADOR_OPN_STATIC = 3

	file_path = Left(WScript.ScriptFullName, Len(WScript.ScriptFullName)-Len(WScript.ScriptName))
	
	If ( CSV_DEBUG > 0 ) Then
		WScript.Echo "| Looking for ErrorCodes.csv in " & file_path
	End If

	Set rs = CreateObject("ador.recordset")
	str_conn = "Driver={Microsoft Text Driver (*.txt; *.csv)};DefaultDir="&file_path&";"

	If ( VarType(err_id) = vbString ) Then
		' Pad strings with at ' on both sides for the query to work
		err_id = "'"&err_id&"'"
	End If

	rs.open "SELECT * FROM ErrorCodes.csv WHERE "&col_name&"="&err_id&"", str_conn, ADOR_OPN_STATIC

	If ( CSV_DEBUG > 0 ) Then
		WScript.Echo "| SELECT * FROM ErrorCodes.csv SEARCH "&col_name&"="&err_id&", str_conn, ADOR_OPN_STATIC"
	End If

	If ( Err.Number <> 0 ) Then
		WScript.Echo "| Error attempting to read file."
		WScript.Echo "| Error: " & Err.Number
		WScript.Echo "| Error (Hex): 0x" & Hex(Err.Number)
		WScript.Echo "| Source: " &  Err.Source
		WScript.Echo "| Description: " & Err.Description
		Err.Clear

	ElseIf ( CSV_DEBUG = 10 ) Then
		WScript.Echo "| Recordcount: " & rs.recordcount
		WScript.Echo "| Fields per record: " & rs.fields.count
		s = "Field names: "
		dl = ""
		for each f in rs.fields
			s = s & dl & f.name: dl = "; "
		next
		WScript.Echo "| " & s

		do until rs.eof
 			s = ""
  			for each f in rs.fields
	    			s = s & f.name & "=" & f.value & vbcrlf
    			Next
  			WScript.Echo "| " & s
  			rs.movenext
		loop
		WScript.Echo "| done..."

	ElseIf ( rs.recordcount < 1 ) Then
		FindErrorMeaning = "| Error code ID has no description."

	Else
		meaning = ""
		do until rs.eof
 			s = ""
  			for each f in rs.fields
	    			s = s & "| " & f.name & "=" & f.value & vbCRLF
    			Next
  			meaning = meaning & s
  			rs.movenext
		loop
		FindErrorMeaning = meaning
	End If
	
	rs.close

End Function

Function RunExec(ByRef win_shell, ByVal cmd)
	Dim sh: Set sh = CreateObject("WScript.Shell")
	Dim wsx: Set wsx = win_shell.Exec(cmd)

	If wsx.ProcessID = 0 And wsx.Status = 1 Then
		Err.Raise vbObjectError, ,"WshShell.Exec failed."
	End If

	Do
		Dim Status: Status = wsx.Status
		If ( Len(wsx.StdOut.ReadAll()) > 0 ) Then
			WScript.StdOut.Write "| " &  wsx.StdOut.ReadAll() & vbcrlf
		End If
		If ( Len(wsx.StdErr.ReadAll()) > 0 ) Then
			WScript.StdErr.Write "| " & wsx.StdErr.ReadAll() & vbcrlf
		End If
		
		If ( Status <> 0 ) Then
			Exit Do
		End If
		
		WScript.Sleep(10)
	Loop

	RunExec = wsx.ExitCode
End Function

Function DoHeader()
	WScript.Echo " __________________________________________________"
End Function

Function DoFooter()
	WScript.Echo "|__________________________________________________" & vbCRLF
End Function