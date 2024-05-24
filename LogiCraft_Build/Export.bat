set "DirName=LogiCraft_V2.0"

md ..\%DirName%\bin
md ..\%DirName%\Ressources
copy /Y ..\x64\Release\*.dll ..\%DirName%\bin\*.dll
copy /Y ..\x64\Release\LogiCraft.exe ..\%DirName%\bin\LogiCraft.exe
copy /Y ..\x64\Release\Exportation_LogiCraft.exe ..\%DirName%\bin\Exportation_LogiCraft.exe
copy /Y *.ini ..\%DirName%\bin\*.ini
Robocopy ..\Ressources ..\%DirName%\Ressources /E
exit 0