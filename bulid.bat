cd D:\Program Files (x86)\Microsoft Visual Studio 14.0\VC
@echo off
echo 该批处理文件：
echo 所在完整路径：%~dp0
echo 当前目录 %cd%
echo 文件名为：%~n0.bat
pause >nul
(call "D:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat") && echo 子批处理处理成功
rem 切换到批处理所在文件的目录 cd D:\Lua Learning\notepad-plus-plus-6.8.8\scintilla\win32
cd %~dp0
rem (常规格式的编译)
nmake NOBOOST=1 -f scintilla.mak
rem (调试状态模式的编译)
rem nmake NOBOOST=1 DEBUG=1 -f scintilla.mak  (debugging build)
pause >nul
