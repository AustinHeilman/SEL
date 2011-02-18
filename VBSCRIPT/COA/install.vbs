strComputer = "."

Set objWMIService = GetObject("winmgmts:" & _
    "{impersonationLevel=impersonate}!\\" & strComputer & "\root\cimv2")

Set colSoftware = objWMIService.ExecQuery ("SELECT * FROM Win32_Product")
If ( IsNull(colSoftware) ) THEN 
	WScript.Echo "ERROR (1)"
	WScript.Quit
End If

If ( colSoftware.Count > 0 ) Then

    Set objFSO = CreateObject("Scripting.FileSystemObject")
    Set objTextFile = objFSO.CreateTextFile("SoftwareList.txt", True)

    For Each objSoftware in colSoftware
        objTextFile.WriteLine objSoftware.Caption & vbtab & objSoftware.Version
    Next

    objTextFile.Close

    WScript.Echo "Done."

Else
    WScript.Echo "Cannot retrieve software from this computer."
End If
