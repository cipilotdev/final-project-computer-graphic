#include <GL/freeglut.h>

void display() {
    // display function to render the scene
}

void reshape(int w, int h) {
    // reshape function for window resizing
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Project UAS Komgraf ( Kelompok - 4 )");

    glEnable(GL_DEPTH_TEST);

    glutMainLoop();
    return 0;
}
