# Check if there's a Keil folder in the current directory
if (-Not (Test-Path "Keil" -PathType Container)) {
    Write-Host "No Keil folder found in the current directory"
    return
}

$libName = "big-lib.lib"
$libDir = "BigLib"
$incDir = "$libDir\Include"
$hFiles = Get-ChildItem -Path "Keil\Include" -Filter *.h -Recurse

# If no peripherals.lib file is found in Keil\Objects, return
if (-Not (Test-Path "Keil\Objects\$libName" -PathType Leaf)) {
    Write-Host "No $libName found in Keil\Objects"
    return
}

# If no .h files are found in Keil\Include, return
if ($hFiles.Count -eq 0) {
    Write-Host "No .h files found in Keil\Include"
    return
}


# Create Peripherals\ and Peripherals\Include\ directories
# and copy peripherals.lib files from Keil\Objects
Remove-Item -Recurse -Force $libDir > $null 2>&1
New-Item -ItemType Directory -Path $libDir | Out-Null 
New-Item -ItemType Directory -Path $incDir | Out-Null
Copy-Item -Path "Keil\Objects\$libName" -Destination $libDir

# Copying .h files from Keil\Source\Libs to Peripherals\Include
$hFiles | ForEach-Object {
    Copy-Item -Path $_.FullName -Destination $incDir
}


# Cleaning directories created by Keil during build
"Objects", "Listings", "RTE" | ForEach-Object {
    $dir = "Keil\$_"
    if (Test-Path $dir) {
        Remove-Item -Recurse -Force $dir
    }
}


# Asking the user if he wants to copy the new library to another project directory
$stop = $false
while (-Not $stop) {
    switch ((Read-Host "Copy the library to another project directory? (Y/N)")) {
        "Y" {
            $projectDir = Read-Host "Directory"
            if (-Not (Test-Path "$projectDir\Keil" -PathType Container)) {
                Write-Host "No Keil folder found in $projectDir"
                break
            }

            New-Item -ItemType Directory -Path "$projectDir\Keil\Libs" -Force > $null 2>&1
            Remove-Item -Recurse -Force "$projectDir\Keil\Libs\$libDir" | Out-Null
            Copy-Item -Path $libDir -Destination "$projectDir\Keil\Libs" -Recurse | Out-Null
            $stop = $true
            break
        }
        "N" {
            $stop = $true
            break
        }
        default { Write-Host "Invalid option. Type Y or N." }
    }
}

$stop = $false
while (-Not $stop) {
    switch ((Read-Host "Need to tarball the library? (Y/N)")) {
        "Y" {
            $tarballName = "$libDir.tar.gz"
            Remove-Item -Force $tarballName | Out-Null

            if (-Not (Get-Command tar -ErrorAction SilentlyContinue)) {
                Write-Host "tar is not installed. Please install it and try again."
                return
            }

            tar -czf $tarballName $libDir
            $stop = $true
            break
        }
        "N" {
            $stop = $true
            break
        }
        default { Write-Host "Invalid option. Type Y or N." }
    }
}



