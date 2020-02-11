@echo off
call msbuildpath.bat

echo.
echo ++++++++++++ Building Unicode IDP ++++++++++++
"%MSBUILD%" "%~dp0InnoDownloadPlugin.sln" /t:idp -property:Configuration=Release
pause

echo.
echo ++++++++++++ Building ANSI IDP ++++++++++++
"%MSBUILD%" "%~dp0InnoDownloadPlugin.sln" /t:idp -property:Configuration="Release ANSI"
pause

echo.
echo ++++++++++++ Building DOCs ++++++++++++
cd doc
call "build.bat"

@echo off
echo.
echo ++++++++++++ Building IDP installation ++++++++++++
cd ..
"%ProgramFiles(x86)%\Inno Setup 6\ISCC.exe" setup.iss
pause
