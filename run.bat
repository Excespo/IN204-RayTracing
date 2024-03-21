@echo off
setlocal

cmake -B build
cmake --build build

echo.
echo Playing ray tracing ...

if not exist result mkdir result

set MESSAGE_TO=result\log.txt
type nul > %MESSAGE_TO%

set n_samples=10
set max_depth=10
set image_width=400
set n_threads=12

set output_name=result\image_%image_width%_%n_samples%
set extension=png
set output_file=%output_name%.%extension%
:: set output_file=cout

set MESSAGE=Ray tracing with %n_samples% samples, %max_depth% max depth, %image_width% image width, %n_threads% threads

:: Run the program
build\ray_tracing %output_file% -m "%MESSAGE%" -s %n_samples% -d %max_depth% -w %image_width% -a -p -n %n_threads% -f scene.json

endlocal
