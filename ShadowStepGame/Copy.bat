rd /s /q Framework
xcopy /e /y C:\work\GameFramework\GameFramework\GameFramework Framework\
robocopy /MOVE /E Framework\asset\ asset\
rd /s /q Framework\asset\
robocopy /MOVE /E Framework\data\ data\
rd /s /q Framework\data\
robocopy /MOVE /E Framework\shader\ shader\
rd /s /q Framework\shader\
robocopy /MOVE /E Framework\shader\ shader\
rd /s /q Framework\shader\
del /s /q Framework\Scene.h
del /s /q Framework\assimp-vc143-mtd.dll
pause