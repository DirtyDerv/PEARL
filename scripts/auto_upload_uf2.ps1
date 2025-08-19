
param(
    [string]$Port = "COM6"
    [string]$UF2Path = "$PSScriptRoot\..\build\PEARL.uf2"
)

# PowerShell script to reboot Pico into BOOTSEL mode and upload firmware automatically
# Usage: .\auto_upload_uf2.ps1 -Port COMx -UF2Path path\to\firmware.uf2

# Function to check and close the port if it is open
function Close-SerialPortIfOpen($portName) {
    try {
        $port = new-Object System.IO.Ports.SerialPort $portName,9600,None,8,one
        if ($port.IsOpen) {
            Write-Host "Port $portName is open. Closing it."
            $port.Close()
        } else {
            # Try to open and close in case it was left in a bad state
            $port.Open()
            $port.Close()
        }
    } catch {
        Write-Host "Port $portName could not be closed (may not exist or already closed)."
    }
}

Close-SerialPortIfOpen $Port
Write-Host "Sending BOOTSEL command to Pico on $Port..."
$portObj = new-Object System.IO.Ports.SerialPort $Port,9600,None,8,one
$portObj.Open()
$portObj.Write("B")
$portObj.Close()

Write-Host "Waiting for Pico to appear as USB drive..."
$timeout = 30
$found = $false
for ($i=0; $i -lt $timeout; $i++) {
    $drives = Get-PSDrive -PSProvider FileSystem | Where-Object { Test-Path (Join-Path $_.Root "INFO_UF2.TXT") }
    if ($drives) {
        $picoDrive = $drives[0].Root
        Write-Host "Pico found at $picoDrive. Copying UF2..."
        Copy-Item $UF2Path (Join-Path $picoDrive (Split-Path $UF2Path -Leaf))
        Write-Host "Upload complete."
        $found = $true
        break
    }
    Start-Sleep -Seconds 1
}
if (-not $found) {
    Write-Host "ERROR: Pico USB drive not found after $timeout seconds."
}
