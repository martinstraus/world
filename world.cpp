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

class Color {
private:
    float _red, _green, _blue;

public:
    Color(float red, float green, float blue)
        : _red(red), _green(green), _blue(blue) {}

    float red() { return _red; }
    float green() { return _green; }
    float blue() { return _blue; }
};

// World types

class Shape {

public:
    virtual ~Shape() = default;
    virtual void draw(Point<float>) {
        std::cout << "[draw not implemented]\n";
    }
    virtual bool contains(Point<float>) {
        return false;
    }
};

class Square : public Shape {
private:
    Size<float> _size;

public:
    Square(Size<float> size) : _size(size) {}

    void draw(Point<float> location) override {
        // Define the square around the origin, then translate it to its unit.
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

    bool contains(Point<float> point) override {
        return std::fabs(point.x()) <= _size.width() / 2.0f
            && std::fabs(point.y()) <= _size.height() / 2.0f;
    }
};

class Triangle: public Shape {
private:
    float _side; // Size

public:
    Triangle(float side) : _side(side) {}

    void draw(Point<float> location) override {
        // Derive the height of an equilateral triangle from its side length.
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

    bool contains(Point<float> point) override {
        float height = std::sqrt(3.0f) * _side * 0.5f;
        float top = 2.0f * height / 3.0f;
        float bottom = -height / 3.0f;

        if (point.y() < bottom || point.y() > top) {
            return false;
        }

        float halfWidthAtY = (_side / 2.0f) * (top - point.y()) / height;
        return std::fabs(point.x()) <= halfWidthAtY;
    }
};

class Circle : public Shape {
private:
    float _radius;
    int _segments;

public:
    Circle(float radius, int segments = 48)
        : _radius(radius), _segments(segments) {}

    void draw(Point<float> location) override {
        // A triangle fan joins the center to consecutive edge points.
        constexpr float pi = 3.14159265358979323846f;

        glColor3f(1, 0, 0);

        glPushMatrix();
        glTranslatef(location.x(), location.y(), 0.0f);

        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(0.0f, 0.0f);

        // Repeat the first edge point at the end to close the fan.
        for (int i = 0; i <= _segments; ++i) {
            float angle = 2.0f * pi * i / _segments;
            glVertex2f(
                _radius * std::cos(angle),
                _radius * std::sin(angle)
            );
        }

        glEnd();
        glPopMatrix();
    }

    bool contains(Point<float> point) override {
        return point.x() * point.x() + point.y() * point.y()
            <= _radius * _radius;
    }
};

class Unit {
private:
    std::unique_ptr<Shape> _shape;
    Point<float> _location;
    Point<float> _targetLocation;
    float _speed;

public:
    Unit(std::unique_ptr<Shape> shape, Point<float> location, float speed)
        : _shape(std::move(shape)), _location(location), _targetLocation(location), _speed(speed) {}

    void draw() {
        // Delegate drawing to the shape stored at this unit's world position.
        this->_shape->draw(this->_location);
    }

    bool contains(Point<float> worldPoint) {
        // Test against local coordinates because shapes are drawn at the origin.
        Point<float> localPoint(
            worldPoint.x() - _location.x(),
            worldPoint.y() - _location.y()
        );
        return _shape->contains(localPoint);
    }

    void moveTo(Point<float> location) {
        // Store a destination; update() advances toward it over time.
        _targetLocation = location;
    }

    bool update(float elapsedSeconds) {
        float deltaX = _targetLocation.x() - _location.x();
        float deltaY = _targetLocation.y() - _location.y();
        float distance = std::sqrt(deltaX * deltaX + deltaY * deltaY);
        float distanceThisFrame = _speed * elapsedSeconds;

        if (distance == 0.0f) {
            return false;
        }
        if (distance <= distanceThisFrame) {
            _location = _targetLocation;
            return true;
        }

        // Normalize the direction so speed remains constant on diagonal paths.
        _location = Point<float>(
            _location.x() + deltaX / distance * distanceThisFrame,
            _location.y() + deltaY / distance * distanceThisFrame
        );
        return true;
    }
};

class World {
private:
    Size<int> _size;
    Color _backgroundColor;
    std::vector<Unit> _units;

public:
    World(Size<int> size, Color backgroundColor)
        : _size(size), _backgroundColor(backgroundColor) { }
    ~World();
    Size<int> size() { return this->_size; };
    Point<float> center() {
        // World coordinates start at the lower-left corner, so half each
        // dimension gives the center point.
        return Point<float>(
            _size.width() / 2.0f,
            _size.height() / 2.0f
        );
    }
    void addUnit(Unit unit) {
        // Unit owns a unique_ptr, so it must be moved into the vector.
        _units.push_back(std::move(unit));
    }
    void clear() {
        // Set the color used by glClear, then erase the previous frame.
        glClearColor(
            _backgroundColor.red(),
            _backgroundColor.green(),
            _backgroundColor.blue(),
            1.0f
        );
        glClear(GL_COLOR_BUFFER_BIT);
    }
    void render() {
        // Units are drawn in insertion order.
        for (Unit& unit : _units) {
            unit.draw();
        }
    }
    Unit* unitAt(Point<float> worldPoint) {
        // Search in reverse rendering order, selecting the visible top-most unit.
        for (auto unit = _units.rbegin(); unit != _units.rend(); ++unit) {
            if (unit->contains(worldPoint)) {
                return &*unit;
            }
        }
        return nullptr;
    }
    bool update(float elapsedSeconds) {
        bool changed = false;
        for (Unit& unit : _units) {
            if (unit.update(elapsedSeconds)) {
                changed = true;
            }
        }
        return changed;
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
    Point<float> _targetPosition;
    float _zoom;
    float _targetZoom;

public:
    Camera(Point<float> position, float zoom)
        : _position(position), _targetPosition(position), _zoom(zoom), _targetZoom(zoom) {}

    void move(float x, float y) {
        // Update only the target; update() smoothly moves the visible camera.
        _targetPosition = Point<float>(
            _targetPosition.x() + x,
            _targetPosition.y() + y
        );
    }

    void setPosition(Point<float> position) {
        // Used for recentering without snapping the visible camera instantly.
        _targetPosition = position;
    }

    void zoomBy(float factor) {
        // Store a bounded zoom target to avoid invalid or impractical views.
        constexpr float minimumZoom = 0.1f;
        constexpr float maximumZoom = 10.0f;

        _targetZoom *= factor;
        if (_targetZoom < minimumZoom) {
            _targetZoom = minimumZoom;
        } else if (_targetZoom > maximumZoom) {
            _targetZoom = maximumZoom;
        }
    }

    float worldUnitsPerScreenUnit() {
        // At higher zoom, a screen-pixel drag covers fewer world units.
        return 1.0f / _targetZoom;
    }

    Point<float> screenToWorld(int screenX, int screenY, Size<float> viewportSize) {
        // GLUT mouse coordinates start at the top-left; world coordinates at
        // the bottom-left, so the screen Y axis must be inverted.
        float halfWidth = viewportSize.width() / (2.0f * _zoom);
        float halfHeight = viewportSize.height() / (2.0f * _zoom);
        return Point<float>(
            _position.x() - halfWidth + screenX / _zoom,
            _position.y() + halfHeight - screenY / _zoom
        );
    }

    bool update(float elapsedSeconds) {
        // Exponential interpolation gives a frame-rate-independent easing curve.
        constexpr float smoothingRate = 12.0f;
        constexpr float positionThreshold = 0.01f;
        constexpr float zoomThreshold = 0.001f;
        float blend = 1.0f - std::exp(-smoothingRate * elapsedSeconds);

        float nextX = _position.x() + (_targetPosition.x() - _position.x()) * blend;
        float nextY = _position.y() + (_targetPosition.y() - _position.y()) * blend;
        float nextZoom = _zoom + (_targetZoom - _zoom) * blend;

        // Snap very small remainders to their targets so animation can finish.
        if (std::fabs(_targetPosition.x() - nextX) < positionThreshold) {
            nextX = _targetPosition.x();
        }
        if (std::fabs(_targetPosition.y() - nextY) < positionThreshold) {
            nextY = _targetPosition.y();
        }
        if (std::fabs(_targetZoom - nextZoom) < zoomThreshold) {
            nextZoom = _targetZoom;
        }

        bool changed = nextX != _position.x()
            || nextY != _position.y()
            || nextZoom != _zoom;
        _position = Point<float>(nextX, nextY);
        _zoom = nextZoom;
        return changed;
    }

    void apply(Size<float> viewportSize) {
        // Convert the camera center and zoom into the visible world rectangle.
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
bool scrollButtonPressed = false;
int lastMouseX = 0;
int lastMouseY = 0;
int lastUpdateTime = 0;
Unit* selectedUnit = nullptr;

void display() {
    // Clear the back buffer, draw the world, then present the completed frame.
    world->clear();
    world->render();
    glutSwapBuffers();
}

void reshape(int w, int h)
{
    // GLUT supplies pixel dimensions; keep them for projection and input.
    viewportSize = Size<float>(static_cast<float>(w), static_cast<float>(h));
    glViewport(0, 0, w, h);
    camera->apply(viewportSize);
}

void mouseButton(int button, int state, int x, int y) {
    if (button == GLUT_MIDDLE_BUTTON) {
        // Save the drag origin whenever middle-button state changes.
        scrollButtonPressed = state == GLUT_DOWN;
        lastMouseX = x;
        lastMouseY = y;
        return;
    }

    if (state != GLUT_DOWN) {
        return;
    }

    Point<float> worldPoint = camera->screenToWorld(x, y, viewportSize);
    if (button == GLUT_LEFT_BUTTON) {
        // Clicking empty space clears the current selection.
        selectedUnit = world->unitAt(worldPoint);
    } else if (button == GLUT_RIGHT_BUTTON && selectedUnit != nullptr) {
        // A right-click sets a destination for the selected unit.
        selectedUnit->moveTo(worldPoint);
    }

    glutPostRedisplay();
}

void mouseMotion(int x, int y) {
    if (!scrollButtonPressed) {
        return;
    }

    // Screen Y grows downward, hence the reversed signs in the camera delta.
    float worldUnitsPerScreenUnit = camera->worldUnitsPerScreenUnit();
    camera->move(
        (lastMouseX - x) * worldUnitsPerScreenUnit,
        (y - lastMouseY) * worldUnitsPerScreenUnit
    );
    lastMouseX = x;
    lastMouseY = y;

    camera->apply(viewportSize);
    glutPostRedisplay();
}

void mouseWheel(int, int direction, int, int) {
    // Each wheel notch changes the target zoom by ten percent.
    constexpr float zoomFactor = 1.1f;

    if (direction > 0) {
        camera->zoomBy(zoomFactor);
    } else if (direction < 0) {
        camera->zoomBy(1.0f / zoomFactor);
    }

    camera->apply(viewportSize);
    glutPostRedisplay();
}

void keyboard(unsigned char key, int, int) {
    if (key == 'c' || key == 'C') {
        // Recenter by updating the target, allowing the normal easing to run.
        camera->setPosition(world->center());
        camera->apply(viewportSize);
        glutPostRedisplay();
    }
}

void idle() {
    // GLUT calls this between events; use elapsed time rather than frame count.
    int currentTime = glutGet(GLUT_ELAPSED_TIME);
    if (lastUpdateTime == 0) {
        lastUpdateTime = currentTime;
        return;
    }

    float elapsedSeconds = (currentTime - lastUpdateTime) / 1000.0f;
    lastUpdateTime = currentTime;
    // Avoid a large visual jump after the app has been paused or blocked.
    if (elapsedSeconds > 0.1f) {
        elapsedSeconds = 0.1f;
    }

    bool cameraChanged = camera->update(elapsedSeconds);
    bool worldChanged = world->update(elapsedSeconds);
    if (cameraChanged) {
        // Projection changes only when the camera has moved or zoomed.
        camera->apply(viewportSize);
    }
    if (cameraChanged || worldChanged) {
        // Animate while either the camera or at least one unit is in motion.
        glutPostRedisplay();
    }
}

void closeHandler(int) {
    glutLeaveMainLoop();
}

int main(int argc, char** argv) {
    // Initialize GLUT before querying display details or creating a window.
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

    // Center the fixed-size application window on the primary display.
    Size<int> screenSize{glutGet(GLUT_SCREEN_WIDTH), glutGet(GLUT_SCREEN_HEIGHT)};
    Size<int> windowSize{1024, 768};
    Point<float> windowPosition{
        (float)(screenSize.width() - windowSize.width()) / 2,
        (float)(screenSize.height() - windowSize.height()) / 2
    }; 

    // Populate a small sample world with one unit of each available shape.
    Size<int> worldSize{1024, 768};
    world = new World(worldSize, Color(0.05f, 0.10f, 0.20f));
    world->addUnit(Unit(
        std::make_unique<Square>(Size<float>(10.0f, 10.0f)),
        Point<float>(50.0f, 50.0f),
        80.0f
    ));
    world->addUnit(Unit(
        std::make_unique<Triangle>(10.0f),
        Point<float>(200.0f, 50.0f),
        120.0f
    ));
    world->addUnit(Unit(
        std::make_unique<Circle>(5.0f),
        Point<float>(350.0f, 100.0f),
        60.0f
    ));

    // Start with a camera centered over the whole world at 1× zoom.
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

    // Register rendering and input callbacks before yielding control to GLUT.
    camera->apply(viewportSize);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouseButton);
    glutMotionFunc(mouseMotion);
    glutMouseWheelFunc(mouseWheel);
    glutKeyboardFunc(keyboard);
    glutIdleFunc(idle);

    glutMainLoop();
    return 0;
}
