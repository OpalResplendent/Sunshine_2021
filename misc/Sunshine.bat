@echo off
set ProjectName=Sunshine
set ProjectDirectory=v:\Projects\Sunshine
set Editor=4coder
set Debugger=RemedyBG

set StartupFiles=""%ProjectDirectory%\code\%ProjectName%.cpp" "%ProjectDirectory%\code\x_%ProjectName%.cpp""

v:\Tools\bats\startup.bat
