 # Checking if Doxygen is installed
if (-Not (Get-Command doxygen -ErrorAction SilentlyContinue)) {
    Write-Host "Doxygen is not installed. Please install it and try again."
    return
}

# Checking if the working directory is clean using git status --porcelain
if ((git status --porcelain)) {
    Write-Host "The working directory is not clean. Please commit or stash your changes and try again."
    return
}

# Checking if the Doxyfile exists
if (-Not (Test-Path Doxyfile)) {
    Write-Host "Doxyfile not found. Please run this script from the root of the project."
    return
}

# Creating a temporary directory in %TEMP%
$dir = New-Item -ItemType Directory -Path "$env:TEMP\Doxygen" -Force
doxygen.exe .\Doxyfile > $null 2>&1
Move-Item -Force "Docs\html\*" -Destination $dir
Remove-Item -Recurse -Force "Docs\"

# Moving to gh-pages
git checkout gh-pages
Remove-Item -Recurse -Force *
Move-Item -Force "$dir\*" -Destination .\
Remove-Item -Recurse -Force $dir

# Committing the changes
git add .
git commit -m "Update Doxygen documentation"
git push origin gh-pages

# Moving back to the original branch
git checkout -

# Done
Write-Host "Doxygen documentation updated successfully."