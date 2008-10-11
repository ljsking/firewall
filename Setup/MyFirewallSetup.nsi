# This example shows how to handle silent installers.
# In short, you need IfSilent and the /SD switch for MessageBox to make your installer
# really silent when the /S switch is used.

Name "MyFirewallSetup"
OutFile "MyFirewallSetup.exe"
AutoCloseWindow true
RequestExecutionLevel user

# uncomment the following line to make the installer silent by default.
; SilentInstall silent

Section
  SetOutPath "c:\windows\temp"
  File "MyFirewallSetup.msi"
  ExecWait "msiexec /quiet /i MyFirewallSetup.msi"
  StrCpy $0 "c:\\program files\\myfirewall\\mydriver.sys" ;Path of copy file from
  StrCpy $1 "c:\\windows\\system32\\drivers\\mydriver.sys"   ;Path of copy file to
  StrCpy $2 1 ; only 0 or 1, set 0 to overwrite file if it already exists
  System::Call 'kernel32::CopyFile(t r0, t r1, b r2) l'
  
  Exec "c:\program files\myfirewall\myfirewall.exe"
  delete "MyFirewallSetup.msi"
SectionEnd
