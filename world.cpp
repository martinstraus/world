#include <iostream>
#include <GL/freeglut.h>
#include <vector>

// General purpose types

template <typename T>
class Point {
private:
    T _x, _y;

public:
    Point(T x, T y) : _x(x), _y(y) {};
    ~Point() {};
    T x() { return this->_x; }
    T y() { return this->_y; }
};

template <typename T>
class Size {
private:
    T _width, _height;

public:
    Size(T width, T height) : _width(width), _height(height) {};
    ~Size() {};
    T width() { return this->_width; }
    T height() { return this->_height; }
};

// World types

class Shape {

public:
    virtual ~Shape() = default;
    virtual void draw(Point<float> location) {
        std::cout << "[draw not implemented]\n";
    }
};

class Square : public Shape {
private:
    Size<float> _size;

public:
    Square(Size<float> size) : _size(size) {}

    void draw(Point<float> location) override {
        glClear(GL_COLOR_BUFFER_BIT);
        glColor3f(1, 0, 0);
        glBegin(GL_QUADS);
        glVertex2f(-0.2, -0.2);
        glVertex2f( 0.2, -0.2);
        glVertex2f( 0.2,  0.2);
        glVertex2f(-0.2,  0.2);
        glEnd();
    }
};

class Unit {
private:
    // Unit does not own this pointer; the caller must keep the Shape alive.
    Shape* _shape;
    Point<float> _location;

public:
    Unit(Shape* shape, Point<float> location) : _shape(shape), _location(location) {}
    void draw() {
        this->_shape->draw(this->_location);
    }    
};

class World {
private:
    Size<int> _size;
    std::vector<Unit> _units;

public:
    World(Size<int> size): _size(size) { }
    ~World();
    Size<int> size() { return this->_size; };
    void addUnit(Unit unit) {
        _units.push_back(unit);
    }
    void render() {
        for (Unit& unit : _units) {
            unit.draw();
        } 
    }
};

// Graphics types

class Screen {
private:
    Size<int> size;
    Size<float> position;

public:
    Screen(Size<int> size, Size<float> position);
    ~Screen();

};

World* world;

void display() {
    world->render();
    glutSwapBuffers();
}

void closeHandler(int) {
    glutLeaveMainLoop();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

    Size<int> screenSize{glutGet(GLUT_SCREEN_WIDTH), glutGet(GLUT_SCREEN_HEIGHT)};
    Size<int> windowSize{1024, 768};
    Point<float> windowPosition{
        (float)(screenSize.width() - windowSize.width()) / 2,
        (float)(screenSize.height() - windowSize.height()) / 2
    }; 

    Size<int> worldSize{1024, 768};
    world = new World(worldSize);
    Square square(Size<float>(10.0f, 10.0f));
    Unit unit(&square, Point<float>(50.0f, 50.0f));
    world->addUnit(unit);

    
    glutInitWindowSize(windowSize.width(), windowSize.height());
    glutInitWindowPosition(windowPosition.x(), windowPosition.y());
    glutCreateWindow("World");

    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_EXIT);

    glClearColor(0, 0, 0, 1);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1, 1, -1, 1, -1, 1);
    glutDisplayFunc(display);

    glutMainLoop();
    return 0;
}
