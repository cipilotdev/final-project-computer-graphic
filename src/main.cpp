#include <GL/freeglut.h>
#include <cmath>

#define PI 3.14159265f

void drawCone(float cx, float cy, float cz, float r, float h, int num_segments) {
    float apexX = cx;
    float apexY = cy + h;
    float apexZ = cz;

    glBegin(GL_LINES);
        for(int i = 0; i <= num_segments; i++) {
            float angle = i * 2.0f * PI / num_segments;
            float x = cx + r * cos(angle);
            float z = cz + r * sin(angle);

            glVertex3f(apexX, apexY, apexZ);
            glVertex3f(x, cy, z);
        }
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(0.0, 0.0, 5.0,   
              0.0, 0.0, 0.0,   
              0.0, 1.0, 0.0);  

    glColor3f(1, 0, 0); // Cone warna merah

    drawCone(0.0f, -1.0f, 0.0f,   
             1.0f, 2.0f,          
             50);                 

    glutSwapBuffers();
    glutPostRedisplay(); 
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (float)w / (float)h, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Project UAS Komgraf (Kelompok 4)");

    glEnable(GL_DEPTH_TEST); 
    glClearColor(0, 0, 0, 1);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    glutMainLoop();
    return 0;
}
