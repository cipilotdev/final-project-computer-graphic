#include <GL/freeglut.h>

#define OBRT 0.57735

static int step = 0;

GLfloat vertices[] = { 
    1, -1, 1,
    1, 1, 1,
    1, 1, -1,
    1, -1, -1,
    -1, -1, 1,
    -1, 1, 1,
    -1, 1, -1,
    -1, -1, -1
};

static unsigned char stripIndices0[] = {5, 4, 1, 0, 2, 3, 6, 7, 5, 4};
static unsigned char stripIndices1[] = {0, 4, 3, 7};
static unsigned char stripIndices2[] = {6, 5, 2, 1};

static float normals[] = 
{
    OBRT, -OBRT, OBRT, 
    OBRT, OBRT, OBRT, 
    OBRT, OBRT, -OBRT, 
    OBRT, -OBRT, -OBRT, 
    -OBRT, -OBRT, OBRT, 
    -OBRT, OBRT, OBRT,
    -OBRT, OBRT, -OBRT,
    -OBRT, -OBRT, -OBRT,
};

void display(void)
{
    float matAmbAndDif1[] = {0.9, 0.0, 0.0, 1.0};
    float matAmbAndDif2[] = {0.0, 0.9, 0.0, 1.0};
    float matSpec[] = {1.0, 1.0, 1.0, 1.0, 1.0};
    float matShine[] = {20.0};

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(0.0, 3.0, 3.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, matAmbAndDif1);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpec);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, matShine);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glNormalPointer(GL_FLOAT, 0, normals);

    glDrawElements(GL_TRIANGLE_STRIP, 10, GL_UNSIGNED_BYTE, stripIndices0);
    glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, stripIndices1);

    glPushMatrix();
    glTranslatef(0.0, 1.0, -1.0);
    glRotatef(float(step), -1, 0, 0);
    glTranslatef(0.0, -1.0, 1.0);
    glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, stripIndices2);
    glPopMatrix();

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, matAmbAndDif2);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glutSolidSphere(1.0, 40, 40);

    glDisable(GL_CULL_FACE);
    glutSwapBuffers();
};

void setup(void)
{
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

    float LightAmb[] = {0.0, 0.0, 0.0, 1.0};
    float lightDifAndSpec[] = {1.0, 1.0, 1.0, 1.0};
    float lightPos[] = {0.0, 1.5, 3.0, 1.0};
    float globAmb[] = {0.2, 0.2, 0.2, 1.0};

    glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDifAndSpec);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightDifAndSpec);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    glEnable(GL_LIGHT0);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globAmb);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
}

void spKey(int key, int x, int y) {
    if (key == GLUT_KEY_UP) {
        if (step < 180) {
            step++;
        }
    }

    if (key == GLUT_KEY_DOWN) {
        if (step > 0) {
            step--;
        }
    }

    glutPostRedisplay();
}

void keyInput(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 27:
            exit(0);
            break;
        default:
            break;
    }
}

void myReshape(int w, int h)
{
    glViewport(0, 0, w, h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(60.0, (float)w / (float)h, 1.0, 20.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    glutCreateWindow("Sphere in Light");
    setup();
    glutDisplayFunc(display);
    glutReshapeFunc(myReshape);
    glutKeyboardFunc(keyInput);
    glutSpecialFunc(spKey);
    glutMainLoop();
    return 0;
}