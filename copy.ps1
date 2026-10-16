# Copies one of the paste-ready files to the clipboard, so it can go straight
# into Windhawk. Comment lines are stripped from the YAML settings.
#
#   .\copy.ps1 mod      -> the Quiet Dock mod source (Windhawk: New mod, paste)
#   .\copy.ps1 theme    -> Taskbar Styler settings  (Settings, Textual mode)
#   .\copy.ps1 height   -> Taskbar height settings  (Settings, Textual mode)
param([Parameter(Mandatory)][ValidateSet('mod', 'theme', 'height')][string]$What)

$file = @{
    mod    = 'mod\quiet-dock.wh.cpp'
    theme  = 'theme\quiet-dock-frosted.yaml'
    height = 'theme\taskbar-height.yaml'
}[$What]

$text = Get-Content -Raw -Encoding utf8 (Join-Path $PSScriptRoot $file)
if ($What -ne 'mod') {
    $text = ($text -split "`r?`n" | Where-Object { $_ -notmatch '^\s*#' }) -join "`n"
}
Set-Clipboard -Value $text
Write-Host "Copied $file to the clipboard."
