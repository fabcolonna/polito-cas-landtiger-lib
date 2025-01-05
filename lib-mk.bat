@echo off

set LIB_DIR=Lib
set INC_DIR=Lib\Include
set ZIP_FILE=peripherals-lib.zip

REM Create the Lib and Include directories
mkdir %LIB_DIR%
if errorlevel 1 echo Directory %LIB_DIR% already exists.

mkdir %INC_DIR%
if errorlevel 1 echo Directory %INC_DIR% already exists.

REM Copy .lib files to the Lib directory
echo Copying .lib files...
copy Keil\Objects\*.lib %LIB_DIR% >nul 2>&1
if errorlevel 1 (
    echo No .lib files found.
    exit /b 1
)

REM Copy .h files to the Include folder
echo Copying .h files to Include folder...
copy Keil\Source\Libs\*.h %INC_DIR% >nul 2>&1

REM Loop through subdirectories in Keil\Source\Libs
for /d %%D in (Keil\Source\Libs\*) do (
    if exist "%%D\*.h" (
        echo Copying headers from %%D...
        copy "%%D\*.h" %INC_DIR% >nul 2>&1
    )
)

REM Cleanup unnecessary files and folders
echo Cleaning up unnecessary files...
rmdir /s /q Keil\Objects
rmdir /s /q Keil\Listings
rmdir /s /q Keil\RTE

REM Create a ZIP file using PowerShell
echo Creating a ZIP file...
powershell -Command "Compress-Archive -Path '%LIB_DIR%' -DestinationPath '%ZIP_FILE%'"
if errorlevel 1 (
    echo Failed to create ZIP file. Ensure PowerShell is available.
    exit /b 1
)

REM Remove temporary directories
rmdir /s /q %LIB_DIR%
echo Cleanup completed.

echo Library packaged successfully as %ZIP_FILE%.
exit /b 0