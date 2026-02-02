@echo off
echo ==========================================
echo      IronTrack Railway System Builder
echo ==========================================

:: 1. Navigate to backend
cd ../backend

:: 2. Clean old build
if exist app.exe del app.exe

:: 3. Compile Main App
echo Compiling Backend Server...
g++ main.cpp -o app.exe -lws2_32 -lodbc32

:: 4. Compile Setup Tool
echo Compiling Database Setup Tool...
g++ setup.cpp -o setup.exe -lodbc32

:: 5. Check status
if exist app.exe (
    echo.
    echo [SUCCESS] Build Complete! 
    echo Run 'backend/app.exe' to start the server.
) else (
    echo.
    echo [ERROR] Compilation Failed. Check errors above.
)

pause