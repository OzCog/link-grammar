@echo off
%= This batch file is invoked in the post-build step of the post-build =%
%= event of the LinkGrammar project build.                             =%
%= Generate a batch file which executes the compilation result.        =%
%= To be invoked as a Post-Build event.                                =%
%= Argument is project target.                                         =%

if "%1"=="" (echo "%~f0: Missing argument" 1>&2 & exit /b)

setlocal
if defined ProgramW6432 set ProgramFiles=%ProgramW6432%

%= Command name to create. =%
set lgcmd=link-parser

echo %~f0: Info: Creating %lgcmd%.bat in %cd%

(
	echo @echo off
	echo setlocal
	echo.

	echo REM This file was auto-generated by %~f0
	echo REM at the LinkGrammar Solution directory %cd%
	echo REM in the post-build event of the LinkGrammar project.
	echo.
	echo REM !!! Don't change it manually in that directory,
	echo REM since your changes will get overwritten !!!
	echo.
	echo REM Copy it to a directory in your PATH and modify it if needed.
	echo.

	echo REM PATH customization - uncomment and change if needed
	echo.
	echo REM DLLs central directory
	echo REM set "PATH=%%PATH%%;%HOMEDRIVE%%HOMEPATH%\DLLs"
	echo.
	echo REM For USE_WORDGRAPH_DISPLAY
	echo REM Path for "dot.exe"
	echo REM set "PATH=%%PATH%%;C:\cygwin64\bin"
	echo set "PATH=%%PATH%%;%ProgramFiles(x86)%\Graphviz2.38\bin"
	echo REM Path for "PhotoViewer.dll"
	echo set "PATH=%%PATH%%;%ProgramFiles%\Windows Photo Viewer"
	echo.

	echo REM Chdir to the link-grammar source directory so the data directory is found.
	echo cd /D %cd%\..
	echo %1 %%*
) > %lgcmd%.bat
