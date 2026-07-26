#include <iostream>
#include <GL/freeglut.h>

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
    int screenW = glutGet(GLUT_SCREEN_WIDTH);
    int screenH = glutGet(GLUT_SCREEN_HEIGHT);
    int winW = 1024;
    int winH = 768;
    int winX = (screenW - winW) / 2;
    int winY = (screenH - winH) / 2;
    glutInitWindowSize(winW, winH);
    glutInitWindowPosition(winX, winY);
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
