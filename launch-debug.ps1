$PSScriptRoot = Split-Path $MyInvocation.MyCommand.Path

$QT = Join-Path $PSScriptRoot 3rdParty/QT/4.8.0/bin
$OpenSceneGraph = Join-Path $PSScriptRoot 3rdParty/OpenSceneGraph/win32/bin
$Cogmatix = Join-Path $PSScriptRoot Build/Debug
$Launch = Join-Path $Cogmatix Cogmatix.exe

$env:Path += ";$QT;$OpenSceneGraph;$Cogmatix"

&$Launch
