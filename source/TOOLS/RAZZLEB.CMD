@echo off
@rem
@rem Whoever is putting the PROMPT commands in this file, should put them
@rem in their CONFIG.SYS file instead.
@rem
set _ntdrive=e:
set tmp=e:\tmp
md e:\tmp 2>nul
cmd /K %_XPDRIVE%\NT\PUBLIC\TOOLS\ntenv.cmd %1 %2 %3 %4 %5 %6 %7 %8 %9
