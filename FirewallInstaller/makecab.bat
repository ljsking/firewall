cd C:\codes\MyFirewall\FirewallInstaller
cabarc.exe N FirewallInstaller.cab ..\debug\FirewallInstaller.ocx FirewallInstaller.inf 
signcode /k mycert.pvk -spc mycert.spc FirewallInstaller.cab
