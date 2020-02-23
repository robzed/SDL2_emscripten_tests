# SDL2_emscripten_tests
Some code to test SDL2 on the web via emscripten

I have problem with frame rate on converting an SDL2 game to the web. There was questions on what was causing the slow down.

# Instruction
See header of file for command line to build the code. You obviously need emscripten installed to build.
I've added an old version of the code here: 
http://robprobin.com/SDL2_emscripten_tests/

Some discussion of the results here:
https://discourse.libsdl.org/t/emscripten-sdl2-performance/27236

NOTE: Web browsers often cap the frame rate, although there are command line options to disable the behaviour. 


# Conclusion
Some machines seem to run really well.
Safari, Chrome, Firefox on the 5 year old MBP Core i7 seems to be very slow.
Older Linux machine aren't great.
Phone are very slow.

