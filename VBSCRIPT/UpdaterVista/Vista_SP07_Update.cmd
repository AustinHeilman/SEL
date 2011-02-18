@echo off

:: ESTABLISH SERVER CONNECTION
net use y: \\Cypress\FileStore /u:CYPRESS\sswload Tor11NovV0.01


:: ## RUN UPDATE
cscript /nologo y:\Vista_Update_SP07\xUpdateScript.vbs
pause

:: TERMINATE SERVER CONNECTION
net use y: /DELETE 


