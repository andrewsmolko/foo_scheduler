@echo off

mkdir releases\current

copy /y build\Release\foo_scheduler.dll releases\current\foo_scheduler.dll
copy /y build\Release\foo_scheduler.pdb releases\current\foo_scheduler.pdb
