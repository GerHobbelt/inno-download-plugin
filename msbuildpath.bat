@echo off

for /f "tokens=*" %%A in ('"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -prerelease -products * -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe') do SET "MSBUILD=%%A"

IF NOT EXIST "%MSBUILD%" goto MissingMSBuildExe

exit /b 0

goto:eof
::ERRORS
::---------------------
:MissingMSBuildExe
echo The MSBuild executable could not be found at '%MSBUILD%'
goto:eof
