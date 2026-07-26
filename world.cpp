#include <iostream>
#include <GL/freeglut.h>

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

class World {
private:
    Size<int> _size;

public:
    World(Size<int> size): _size(size) {}
    ~World();
    Size<int> size() { return this->_size; };
};

class Screen {
private:
    Size<int> size;
    Size<float> position;

public:
    Screen(Size<int> size, Size<float> position);
    ~Screen();

};

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1, 0, 0);
    glBegin(GL_QUADS);
    glVertex2f(-0.2, -0.2);
    glVertex2f( 0.2, -0.2);
    glVertex2f( 0.2,  0.2);
    glVertex2f(-0.2,  0.2);
    glEnd();
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
