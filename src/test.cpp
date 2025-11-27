/**
 * @file test.cpp
 * @brief 3D Teardrop Shape Renderer using OpenGL
 * @author Group 4
 * @version 1.0
 *
 * This program renders a 3D teardrop shape by modifying a standard sphere.
 * The upper hemisphere is tapered to create a water droplet effect.
 *
 * @section controls Controls
 * - Arrow Keys: Rotate the object (X and Y axis)
 * - W/A/S/D: Translate the object
 * - +/-: Scale the object (zoom in/out)
 * - ESC: Exit the program
 */

#include <GL/freeglut.h>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <vector>

/** @brief Mathematical constant PI for trigonometric calculations */
const float PI = 3.14159265f;

/** @brief Current rotation angle around the X-axis in degrees */
float angleX = 15.0f;

/** @brief Current rotation angle around the Y-axis in degrees */
float angleY = 0.0f;

/** @brief Current translation offset along the X-axis */
float transX = 0.0f;

/** @brief Current translation offset along the Y-axis */
float transY = 0.0f;

/** @brief Current translation offset along the Z-axis */
float transZ = 0.0f;

/** @brief Current uniform scale factor for the object */
float scaleValue = 1.0f;


/**
 * @brief Renders a 3D teardrop shape using triangle strips
 *
 * This function generates a teardrop by modifying a parametric sphere.
 * For vertices in the upper hemisphere (y > 0), the radius is pinched
 * inward while the y-coordinate is stretched upward, creating the
 * characteristic pointed tip of a water droplet.
 *
 * The shape is constructed using horizontal triangle strips from top
 * to bottom, with normals calculated for proper lighting.
 *
 * @param radius Base radius of the teardrop at its widest point
 * @param slices Number of longitudinal divisions (horizontal detail)
 * @param stacks Number of latitudinal divisions (vertical detail)
 */

void drawTeardrop(float radius, int slices, int stacks)
{
    for (int i = 0; i < stacks; ++i)
    {
        float phi1 = (float)i / stacks * PI;
        float phi2 = (float)(i + 1) / stacks * PI;

        glBegin(GL_TRIANGLE_STRIP);

        for (int j = 0; j <= slices; ++j)
        {
            float theta = (float)j / slices * 2.0f * PI;

            float r1 = radius;
            float y1 = radius * cos(phi1);

            if (y1 > 0)
            {
                y1 = y1 + (y1 * y1) / radius;
                float pinchFactor = 1.0f - (y1 / (radius * 2.0f));
                r1 *= pinchFactor;
            }

            float x1 = r1 * sin(phi1) * cos(theta);
            float z1 = r1 * sin(phi1) * sin(theta);

            float r2 = radius;
            float y2 = radius * cos(phi2);

            if (y2 > 0)
            {
                y2 = y2 + (y2 * y2) / radius;
                float pinchFactor = 1.0f - (y2 / (radius * 2.0f));
                r2 *= pinchFactor;
            }

            float x2 = r2 * sin(phi2) * cos(theta);
            float z2 = r2 * sin(phi2) * sin(theta);

            glNormal3f(x1, y1, z1);
            glVertex3f(x1, y1, z1);

            glNormal3f(x2, y2, z2);
            glVertex3f(x2, y2, z2);
        }
        glEnd();
    }
}

/**
 * @brief Main rendering callback function
 *
 * This function is called by GLUT whenever the window needs to be redrawn.
 * It clears the buffers, sets up the camera position, applies transformations
 * (translation, rotation, scaling), and renders the teardrop shape.
 * It also draws a smaller cube as a child object to demonstrate hierarchical modeling.
 * Finally, it swaps the buffers to display the rendered frame.
 */
void drawScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(0.0, 0.0, 10.0,
              0.0, 0.0, 0.0,
              0.0, 1.0, 0.0);

    glPushMatrix();
        glTranslatef(transX, transY, transZ);
        glRotatef(angleX, 1.0f, 0.0f, 0.0f);
        glRotatef(angleY, 0.0f, 1.0f, 0.0f);
        glScalef(scaleValue, scaleValue, scaleValue);

        // Draw parent object
        glColor3f(0.0f, 0.8f, 1.0f);
        drawTeardrop(1.5f, 40, 40);

        // Draw child object (smaller cube)
        glPushMatrix();
            glTranslatef(3.0f, 0.0f, 0.0f); 
            glScalef(0.5f, 0.5f, 0.5f);
            glColor3f(1.0f, 0.2f, 0.2f);
            glutSolidTeapot(1.5);
        glPopMatrix();

    glPopMatrix();

    glutSwapBuffers();
}

/**
 * @brief Initializes OpenGL rendering settings
 *
 * Configures the following OpenGL states:
 * - Background color (dark gray)
 * - Depth testing for proper 3D rendering
 * - Lighting system with two directional lights
 * - Color material tracking for vertex colors
 * - Light 0 positioned to the right with ambient, diffuse, and specular components
 * - Light 1 positioned to the left with ambient, diffuse, and specular components
 */
void setUp(void)
{
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);

    glEnable(GL_COLOR_MATERIAL);

    // Light 0 configuration
    GLfloat light0_pos[]     = { 3.0f, 2.0f, 2.0f, 1.0f };      // Right
    GLfloat light0_ambient[]  = { 0.20f, 0.20f, 0.20f, 1.0f };  // ambient
    GLfloat light0_diffuse[]  = { 0.55f, 0.55f, 0.55f, 1.0f };  // diffuse
    GLfloat light0_specular[] = { 0.45f, 0.45f, 0.45f, 1.0f };  // specular

    glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT,  light0_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light0_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);

    // Light 1 configuration
    GLfloat light1_pos[]      = { -3.0f, 1.0f, 2.0f, 1.0f };    // Left
    GLfloat light1_ambient[]  = { 0.15f, 0.15f, 0.15f, 1.0f };  // ambient
    GLfloat light1_diffuse[]  = { 0.40f, 0.40f, 0.40f, 1.0f };  // diffuse
    GLfloat light1_specular[] = { 0.35f, 0.35f, 0.35f, 1.0f };  // specular

    glLightfv(GL_LIGHT1, GL_POSITION, light1_pos);
    glLightfv(GL_LIGHT1, GL_AMBIENT,  light1_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE,  light1_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
}

/**
 * @brief Window resize callback function
 *
 * Called by GLUT when the window is resized. Updates the viewport
 * and recalculates the projection matrix to maintain proper aspect ratio.
 *
 * @param w New window width in pixels
 * @param h New window height in pixels
 */
void resize(int w, int h)
{
    if (h == 0)
        h = 1;
    float ratio = (float)w / h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, ratio, 1.0f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

/**
 * @brief Keyboard input callback for standard keys
 *
 * Handles translation and scaling controls:
 * - W: Move object upward
 * - S: Move object downward
 * - A: Move object left
 * - D: Move object right
 * - +: Increase scale (zoom in)
 * - -: Decrease scale (zoom out, minimum 0.1)
 * - ESC: Terminate the program
 * - Q: Move object closer (along Z-axis)
 * - E: Move object farther (along Z-axis)
 * - 1: Toggle Light 0
 * - 2: Toggle Light 1
 *
 * @param key ASCII code of the pressed key
 * @param x Mouse X position (unused)
 * @param y Mouse Y position (unused)
 */
void keyInput(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'q':
        transZ -= 0.2f; 
        glutPostRedisplay();
        break;
    case 'e':
        transZ += 0.2f; 
        glutPostRedisplay();
        break;
    case 'w':
        transY += 0.2f;
        glutPostRedisplay();
        break;
    case 's':
        transY -= 0.2f;
        glutPostRedisplay();
        break;
    case 'a':
        transX -= 0.2f;
        glutPostRedisplay();
        break;
    case 'd':
        transX += 0.2f;
        glutPostRedisplay();
        break;
    case '+':
        scaleValue += 0.1f;
        glutPostRedisplay();
        break;
    case '-':
        scaleValue -= 0.1f;
        if (scaleValue < 0.1f)
            scaleValue = 0.1f;
        glutPostRedisplay();
        break;
    case '1':
        if (glIsEnabled(GL_LIGHT0))
            glDisable(GL_LIGHT0);
        else
            glEnable(GL_LIGHT0);
        glutPostRedisplay();
        break;
    case '2':
        if (glIsEnabled(GL_LIGHT1))
            glDisable(GL_LIGHT1);
        else
            glEnable(GL_LIGHT1);
        glutPostRedisplay();
        break;
    case 27:
        exit(0);
        break;
    }
}

/**
 * @brief Keyboard input callback for special keys
 *
 * Handles rotation controls using arrow keys:
 * - UP: Rotate around X-axis (tilt backward)
 * - DOWN: Rotate around X-axis (tilt forward)
 * - LEFT: Rotate around Y-axis (turn left)
 * - RIGHT: Rotate around Y-axis (turn right)
 *
 * @param key GLUT special key code
 * @param x Mouse X position (unused)
 * @param y Mouse Y position (unused)
 */
void specialInput(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_UP:
        angleX -= 5.0f;
        break;
    case GLUT_KEY_DOWN:
        angleX += 5.0f;
        break;
    case GLUT_KEY_LEFT:
        angleY -= 5.0f;
        break;
    case GLUT_KEY_RIGHT:
        angleY += 5.0f;
        break;
    }
    glutPostRedisplay();
}

/**
 * @brief Program entry point
 *
 * Initializes the GLUT library, creates the application window,
 * registers callback functions, and starts the main event loop.
 *
 * @param argc Command line argument count
 * @param argv Command line argument values
 * @return Exit status code (0 for success)
 */
int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Project UAS Komgraf (Kelompok 4)");

    glutDisplayFunc(drawScene);
    glutReshapeFunc(resize);
    glutKeyboardFunc(keyInput);
    glutSpecialFunc(specialInput);

    setUp();
    glutMainLoop();
    return 0;
}
