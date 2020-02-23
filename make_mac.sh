
g++ -Wall -I /Users/rob/Current_Projects/Forlorn_Fox/SDL/include -c -o sdl2_test.o sdl2_test.cpp 
#g++ sdl2_test.o -o sdl2_test -framework SDL2 -F /Users/rob/Current_Projects/Forlorn_Fox/runtime_libs_2.0.8
g++ sdl2_test.o -o sdl2_test -lSDL2 libSDL2.a  -L. -L/usr/local/lib  -lm -liconv -Wl,-framework,OpenGL -Wl,-framework,ForceFeedback -lobjc -Wl,-framework,Cocoa -Wl,-framework,Carbon -Wl,-framework,IOKit -Wl,-framework,CoreAudio -Wl,-framework,AudioToolbox -Wl,-framework,AudioUnit -Wl,-framework,CoreVideo -Wl,-framework,CoreMedia -Wl,-framework,Metal 
 
mkdir sdl2_test.app
mkdir sdl2_test.app/Contents
mkdir sdl2_test.app/Contents/MacOS
mkdir sdl2_test.app/Contents/Resources
mv sdl2_test sdl2_test.app/Contents/MacOS/sdl2_test
cp -r img/. sdl2_test.app/Contents/Resources


