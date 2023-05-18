python ./scripts/build.py emscripten 
if %errorlevel% neq 0 exit /b %errorlevel%
./run-server.bat