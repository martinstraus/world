CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -g -O0 -I/opt/homebrew/include
LDFLAGS = -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo -L/opt/homebrew/lib -Wl,-rpath,/opt/homebrew/lib -lglut

world: world.cpp
	$(CXX) $(CXXFLAGS) -o world world.cpp $(LDFLAGS)

clean:
	rm -f world
