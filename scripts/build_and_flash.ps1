# PEARL Automated Build & Flash Script (PowerShell)
# Usage: Run in project root after connecting Pico in BOOTSEL mode

$buildDir = "build"
$uf2File = "$buildDir\PEARL.uf2"

if (!(Test-Path $buildDir)) {
    cmake -G "Ninja" ..
}
ninja

if (Test-Path $uf2File) {
    Write-Host "\nBuild successful. Searching for Pico..."
    $drives = Get-PSDrive -PSProvider FileSystem | Where-Object { $_.Free -gt 0 -and (Test-Path (Join-Path $_.Root "INFO_UF2.TXT")) }
    if ($drives) {
        $picoDrive = $drives[0].Root
        Copy-Item $uf2File (Join-Path $picoDrive "PEARL.uf2")
        Write-Host "Flashed PEARL.uf2 to Pico ($picoDrive)"
    } else {
        Write-Host "Pico not found. Please connect in BOOTSEL mode."
    }
} else {
    Write-Host "Build failed: PEARL.uf2 not found."
}
