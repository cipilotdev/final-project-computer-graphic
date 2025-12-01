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

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <cstdlib>
#include <cmath>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "include/stb_image.h"
#include <vector>

/** @brief Mathematical constant PI for trigonometric calculations */
const float PI = 3.14159265f;

/**
 * @struct Transform
 * @brief Encapsulates all transformation properties for a 3D object
 *
 * This structure groups translation, rotation, and scale parameters
 * to provide a clean interface for object transformations.
 */
struct Transform
{
    float translateX = 0.0f; /**< Translation offset along X-axis */
    float translateY = 0.0f; /**< Translation offset along Y-axis */
    float translateZ = 0.0f; /**< Translation offset along Z-axis */
    float rotateX = 15.0f;   /**< Rotation angle around X-axis (degrees) */
    float rotateY = 0.0f;    /**< Rotation angle around Y-axis (degrees) */
    float scale = 1.0f;      /**< Uniform scale factor */
};

/** @brief Transform state for the main teardrop object */
Transform objectTransform;

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

            /** @brief Horizontal texture coordinate (0.0 to 1.0) based on slice position */
            float u = (float)j / slices;
            /** @brief Vertical texture coordinate for first vertex based on stack position */
            float v1 = (float)i / stacks;
            /** @brief Vertical texture coordinate for second vertex based on next stack position */
            float v2 = (float)(i + 1) / stacks;

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

            glNormal3f(x1, y1, z1);
            glTexCoord2f(u, v1); // Set texture coordinate for vertex 1
            glVertex3f(x1, y1, z1);

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

            glNormal3f(x2, y2, z2);
            glTexCoord2f(u, v2); // Set texture coordinate for vertex 2
            glVertex3f(x2, y2, z2);
        }

        glEnd();
    }
}

/** @brief OpenGL texture ID for the teardrop surface texture */
GLuint teardropTexture;

/**
 * @brief Loads a texture from an image file and configures OpenGL texture parameters
 *
 * This function uses stb_image library to load an image file, then creates
 * an OpenGL texture with linear filtering and mipmapping for smooth rendering.
 * The texture is set to repeat in both S and T directions.
 *
 * @param filename Path to the image file to load (supports JPG, PNG, etc.)
 * @note Program will exit with code 1 if texture loading fails
 */
void loadTexture(const char *filename)
{
    int width, height, channels;
    unsigned char *data = stbi_load(filename, &width, &height, &channels, 0);

    if (!data)
    {
        std::cout << "Failed to load texture: " << filename << std::endl;
        exit(1);
    }

    glGenTextures(1, &teardropTexture);
    glBindTexture(GL_TEXTURE_2D, teardropTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height,
                 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
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
    // Apply object transformations from Transform struct
    glTranslatef(objectTransform.translateX,
                 objectTransform.translateY,
                 objectTransform.translateZ);
    glRotatef(objectTransform.rotateX, 1.0f, 0.0f, 0.0f);
    glRotatef(objectTransform.rotateY, 0.0f, 1.0f, 0.0f);
    glScalef(objectTransform.scale,
             objectTransform.scale,
             objectTransform.scale);

    // Draw parent object
    glBindTexture(GL_TEXTURE_2D, teardropTexture);

    // optional: tint
    glColor3f(1.0f, 1.0f, 1.0f);

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
    GLfloat light0_pos[] = {3.0f, 2.0f, 2.0f, 1.0f};         // Right
    GLfloat light0_ambient[] = {0.20f, 0.20f, 0.20f, 1.0f};  // ambient
    GLfloat light0_diffuse[] = {0.55f, 0.55f, 0.55f, 1.0f};  // diffuse
    GLfloat light0_specular[] = {0.45f, 0.45f, 0.45f, 1.0f}; // specular

    glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);

    // Light 1 configuration
    GLfloat light1_pos[] = {-3.0f, 1.0f, 2.0f, 1.0f};        // Left
    GLfloat light1_ambient[] = {0.15f, 0.15f, 0.15f, 1.0f};  // ambient
    GLfloat light1_diffuse[] = {0.40f, 0.40f, 0.40f, 1.0f};  // diffuse
    GLfloat light1_specular[] = {0.35f, 0.35f, 0.35f, 1.0f}; // specular

    glLightfv(GL_LIGHT1, GL_POSITION, light1_pos);
    glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);

    glEnable(GL_TEXTURE_2D);
    loadTexture("../src/resources/dirt.jpg");
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
    // Movement speed constants
    const float TRANSLATION_SPEED = 0.2f;
    const float SCALE_STEP = 0.1f;
    const float MIN_SCALE = 0.1f;

    switch (key)
    {
    case 'q':
        objectTransform.translateZ -= TRANSLATION_SPEED;
        glutPostRedisplay();
        break;
    case 'e':
        objectTransform.translateZ += TRANSLATION_SPEED;
        glutPostRedisplay();
        break;
    case 'w':
        objectTransform.translateY += TRANSLATION_SPEED;
        glutPostRedisplay();
        break;
    case 's':
        objectTransform.translateY -= TRANSLATION_SPEED;
        glutPostRedisplay();
        break;
    case 'a':
        objectTransform.translateX -= TRANSLATION_SPEED;
        glutPostRedisplay();
        break;
    case 'd':
        objectTransform.translateX += TRANSLATION_SPEED;
        glutPostRedisplay();
        break;
    case '+':
        objectTransform.scale += SCALE_STEP;
        glutPostRedisplay();
        break;
    case '-':
        objectTransform.scale -= SCALE_STEP;
        if (objectTransform.scale < MIN_SCALE)
            objectTransform.scale = MIN_SCALE;
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
    const float ROTATION_SPEED = 5.0f;

    switch (key)
    {
    case GLUT_KEY_UP:
        objectTransform.rotateX -= ROTATION_SPEED;
        break;
    case GLUT_KEY_DOWN:
        objectTransform.rotateX += ROTATION_SPEED;
        break;
    case GLUT_KEY_LEFT:
        objectTransform.rotateY -= ROTATION_SPEED;
        break;
    case GLUT_KEY_RIGHT:
        objectTransform.rotateY += ROTATION_SPEED;
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

    glewExperimental = GL_TRUE; // Needed for some drivers
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
        return 1;
    }
    std::cout << "GLEW Initialized successfully." << std::endl;

    glutDisplayFunc(drawScene);
    glutReshapeFunc(resize);
    glutKeyboardFunc(keyInput);
    glutSpecialFunc(specialInput);

    setUp();
    glutMainLoop();
    return 0;
}
