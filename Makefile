CC = g++
CFLAGS = -Wall -std=c++17 -I/opt/homebrew/include
LDFLAGS = -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo -L/opt/homebrew/lib -Wl,-rpath,/opt/homebrew/lib -lglut

world: world.cpp
	$(CC) $(CFLAGS) -o world world.cpp $(LDFLAGS)

clean:
	rm -f world
