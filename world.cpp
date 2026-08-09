#include <iostream>
#include <GL/freeglut.h>
#include <vector>
#include <cmath>
#include <memory>
#include <utility>

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
    virtual void draw(Point<float>) {
        std::cout << "[draw not implemented]\n";
    }
};

class Square : public Shape {
private:
    Size<float> _size;

public:
    Square(Size<float> size) : _size(size) {}

    void draw(Point<float> location) override {
        glColor3f(1, 0, 0);
    
        float halfWidth = _size.width() / 2;
        float halfHeight = _size.height() / 2;
        
        glPushMatrix();
        glTranslatef(location.x(), location.y(), 0.0f);

        glBegin(GL_QUADS);
        glVertex2f(-halfWidth, -halfHeight);
        glVertex2f(halfWidth, -halfHeight);
        glVertex2f(halfWidth,  halfHeight);
        glVertex2f(-halfWidth,  halfHeight);
        glEnd();

        glPopMatrix();
    }
};

class Triangle: public Shape {
private:
    float _side; // Size

public:
    Triangle(float side) : _side(side) {}

    void draw(Point<float> location) override {
        float h = std::sqrt(3.0f) * _side * 0.5f;

        glColor3f(1, 0, 0);
        
        glPushMatrix();
        glTranslatef(location.x(), location.y(), 0.0f);

        glBegin(GL_TRIANGLES);

        // Top vertex
        glVertex2f(0.0f,  2.0f * h / 3.0f);

        // Bottom-left
        glVertex2f(-_side / 2.0f, -h / 3.0f);

        // Bottom-right
        glVertex2f( _side / 2.0f, -h / 3.0f);
        
        glEnd();

        glPopMatrix();
    }
};

class Unit {
private:
    std::unique_ptr<Shape> _shape;
    Point<float> _location;

public:
    Unit(std::unique_ptr<Shape> shape, Point<float> location)
        : _shape(std::move(shape)), _location(location) {}

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
        _units.push_back(std::move(unit));
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

class Camera {
private:
    Point<float> _position;
    float _zoom;

public:
    Camera(Point<float> position, float zoom) : _position(position), _zoom(zoom) {}

    void position(Size<float> viewportSize) {
        float halfWidth  = viewportSize.width() / (2.0f * _zoom);
        float halfHeight = viewportSize.height() / (2.0f * _zoom);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        glOrtho(
            _position.x() - halfWidth,
            _position.x() + halfWidth,
            _position.y() - halfHeight,
            _position.y() + halfHeight,
            -1.0f, 1.0f
        );

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }
};

World* world;
Camera* camera;
Size<float> viewportSize = Size<float>(0.0f, 0.0f);

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    world->render();
    glutSwapBuffers();
}

void reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    camera->position(Size<float>((float)w, (float)h));
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
    world->addUnit(Unit(
        std::make_unique<Square>(Size<float>(10.0f, 10.0f)),
        Point<float>(50.0f, 50.0f)
    ));
    world->addUnit(Unit(
        std::make_unique<Triangle>(10.0f),
        Point<float>(200.0f, 50.0f)
    ));

    viewportSize = Size<float>((float)worldSize.width(), (float)worldSize.height());
    camera = new Camera(
        Point<float>(
            worldSize.width() / 2.0f, 
            worldSize.height() / 2.0f
        ), 
        1.0f
    );
    
    glutInitWindowSize(windowSize.width(), windowSize.height());
    glutInitWindowPosition(windowPosition.x(), windowPosition.y());
    glutCreateWindow("World");

    glViewport(0, 0, windowSize.width(), windowSize.height());

    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_EXIT);

    glClearColor(0, 0, 0, 1);
    
    camera->position(viewportSize);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    glutMainLoop();
    return 0;
}
