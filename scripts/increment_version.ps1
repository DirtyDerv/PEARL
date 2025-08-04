# Version increment script for BG Reader project (PowerShell)
# Usage: .\increment_version.ps1 "description"

param(
    [string]$Description = ""
)

$CurrentDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$VersionFile = Join-Path $CurrentDir "version.h"
$CMakeFile = Join-Path $CurrentDir "CMakeLists.txt"

# Read current version
$VersionContent = Get-Content $VersionFile
$CurrentVersionLine = $VersionContent | Where-Object { $_ -match '#define PROJECT_VERSION_STRING' }
$CurrentVersion = ($CurrentVersionLine -split '"')[1]
$Parts = $CurrentVersion -split '\.'
$Major = [int]$Parts[0]
$Minor = [int]$Parts[1]

# Increment minor version by 1 (0.01)
$NewMinor = $Minor + 1
$NewVersion = "$Major.$($NewMinor.ToString('00'))"

Write-Host "Current version: $CurrentVersion"
Write-Host "New version: $NewVersion"

# Update version.h
(Get-Content $VersionFile) | ForEach-Object {
    $_ -replace '#define PROJECT_VERSION_MINOR \d+', "#define PROJECT_VERSION_MINOR $NewMinor" `
       -replace '#define PROJECT_VERSION_STRING ".*"', "#define PROJECT_VERSION_STRING `"$NewVersion`""
} | Set-Content $VersionFile

# Update CMakeLists.txt
(Get-Content $CMakeFile) | ForEach-Object {
    $_ -replace 'pico_set_program_version\(Bg_Reader ".*"\)', "pico_set_program_version(Bg_Reader `"$NewVersion`")"
} | Set-Content $CMakeFile

# Add version comment if description provided
if ($Description -ne "") {
    $Timestamp = Get-Date -Format "yyyy-MM-dd"
    $Comment = "// $NewVersion - $Timestamp - $Description"
    
    $Content = Get-Content $VersionFile
    $HistoryIndex = ($Content | Select-String "// Version History \(for reference\)").LineNumber
    $Content = $Content[0..($HistoryIndex-1)] + $Comment + $Content[$HistoryIndex..($Content.Length-1)]
    $Content | Set-Content $VersionFile
}

Write-Host "Version updated to $NewVersion"
Write-Host "Build with: ninja -C build"
Write-Host "Commit with: git add . && git commit -m `"v$NewVersion`: [description]`""
