/**
 * @file hehehe.cpp
 * @brief 3D Teardrop Shape Renderer using OpenGL
 * @author Group 4
 * @version 1.1
 *
 * This program renders a 3D teardrop shape by modifying a standard sphere.
 * The upper hemisphere is tapered to create a water droplet effect.
 *
 * @section controls Controls
 * Movement Controls:
 * - Arrow Keys: Rotate the object (X and Y axis)
 * - W/A/S/D: Translate the object (Y/X axis)
 * - Q/E: Translate object along Z-axis (closer/farther)
 * - +/-: Scale the object (zoom in/out)
 * 
 * Camera Controls:
 * - Y: Toggle alternate camera view
 * 
 * Animation Controls:
 * - K: Toggle X-axis rotation animation
 * - L: Toggle Y-axis rotation animation
 * - O/P: Decrease/Increase X-axis rotation speed
 * - U/I: Decrease/Increase Y-axis rotation speed
 * 
 * Lighting Controls:
 * - 1/2: Toggle Light 0/1
 * - B/N: Decrease/Increase specular intensity
 * - C/V: Decrease/Increase diffuse intensity
 * - M/,: Decrease/Increase shininess (specular power)
 * - Z/X: Decrease/Increase ambient intensity
 * 
 * Texture Controls:
 * - T: Toggle texture on/off
 * 
 * Other:
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

const float PI = 3.14159265f;

/** @brief Flag to enable/disable X-axis rotation animation */
bool animateX = false;

/** @brief Flag to enable/disable Y-axis rotation animation */
bool animateY = false;

/** @brief Speed for X-axis rotation animation */
float speedX = 1.0f;

/** @brief Speed for Y-axis rotation animation */
float speedY = 1.0f;

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

/** @brief Status boolean for camera position. true = alternate camera. */
bool alternateCameraView = false;

/**
 * @brief Checks for OpenGL errors and prints them to stderr
 *
 * This function queries the OpenGL error state and outputs any
 * errors found, along with the name of the function where the
 * check was performed.
 *
 * @param functionName Name of the function calling this check
 */
void checkGLError(const char *functionName)
{
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR)
    {
        std::cerr << "OpenGL Error inside " << functionName << ": "
                  << gluErrorString(err) << " (" << err << ")" << std::endl;
    }
}

/**
 * @brief Normalizes a 3D vector to unit length
 *
 * @param x X component of vector (modified in place)
 * @param y Y component of vector (modified in place)
 * @param z Z component of vector (modified in place)
 */
void normalizeVector(float &x, float &y, float &z)
{
    float length = sqrt(x * x + y * y + z * z);
    if (length > 0.0001f)
    {
        x /= length;
        y /= length;
        z /= length;
    }
    else
    {
        x = 0.0f;
        y = 1.0f;
        z = 0.0f;
    }
}

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

            float u = (float)j / slices;
            float v1 = (float)i / stacks;
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

            float nx1 = x1, ny1 = y1, nz1 = z1;
            normalizeVector(nx1, ny1, nz1);

            glNormal3f(nx1, ny1, nz1);
            glTexCoord2f(u, v1);
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

            float nx2 = x2, ny2 = y2, nz2 = z2;
            normalizeVector(nx2, ny2, nz2);

            glNormal3f(nx2, ny2, nz2);
            glTexCoord2f(u, v2);
            glVertex3f(x2, y2, z2);
        }

        glEnd();
    }
}

GLuint teardropTexture;
bool textureEnabled = true;
float ambientIntensity = 0.05f;
float diffuseIntensity = 0.9f;
float specularIntensity = 1.0f;
float shininess = 64.0f;

/**
 * @brief Loads a texture from an image file
 *
 * @param filename Path to the image file to load
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
 * @brief Updates light parameters based on current intensity values
 */
void updateLighting()
{
    GLfloat light0_ambient[] = {ambientIntensity, ambientIntensity, ambientIntensity, 1.0f};
    GLfloat light0_diffuse[] = {diffuseIntensity, diffuseIntensity, diffuseIntensity, 1.0f};
    GLfloat light0_specular[] = {specularIntensity, specularIntensity, specularIntensity, 1.0f};

    glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);

    // Update material shininess
    GLfloat mat_shininess[] = {shininess};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
}

/**
 * @brief Main rendering callback function
 */
void drawScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    if (alternateCameraView)
    {
        gluLookAt(0.0, 5.0, 5.0,
                  0.0, 0.0, 0.0,
                  0.0, 1.0, 0.0);
    }
    else
    {
        gluLookAt(0.0, 0.0, 10.0,
                  0.0, 0.0, 0.0,
                  0.0, 1.0, 0.0);
    }

    updateLighting();

    GLfloat light0_pos[] = {1.0f, 1.0f, 1.0f, 0.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);
    
    GLfloat light1_pos[] = {-5.0f, -2.0f, 8.0f, 1.0f};
    glLightfv(GL_LIGHT1, GL_POSITION, light1_pos);

    glPushMatrix();
    glTranslatef(objectTransform.translateX,
                 objectTransform.translateY,
                 objectTransform.translateZ);
    glRotatef(objectTransform.rotateX, 1.0f, 0.0f, 0.0f);
    glRotatef(objectTransform.rotateY, 0.0f, 1.0f, 0.0f);
    glScalef(objectTransform.scale,
             objectTransform.scale,
             objectTransform.scale);

    if (textureEnabled)
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, teardropTexture);
    }
    else
    {
        glDisable(GL_TEXTURE_2D);
    }

    glColor3f(0.0f, 0.8f, 1.0f);

    drawTeardrop(1.5f, 40, 40);

    glPushMatrix();
    glTranslatef(3.0f, 0.0f, 0.0f);
    glScalef(0.5f, 0.5f, 0.5f);
    glColor3f(0.5f, 0.35f, 0.05f);
    glutSolidTeapot(1.5);
    glPopMatrix();

    glPopMatrix();

    glutSwapBuffers();

    if (animateX)
    {
        objectTransform.rotateX += speedX;
        if (objectTransform.rotateX > 360.0f)
            objectTransform.rotateX -= 360.0f;
        if (objectTransform.rotateX < 0.0f)
            objectTransform.rotateX += 360.0f;
    }

    if (animateY)
    {
        objectTransform.rotateY += speedY;
        if (objectTransform.rotateY > 360.0f)
            objectTransform.rotateY -= 360.0f;
        if (objectTransform.rotateY < 0.0f)
            objectTransform.rotateY += 360.0f;
    }

    if (animateX || animateY)
    {
        glutPostRedisplay();
    }

    checkGLError("drawScene");
}

/**
 * @brief Initializes OpenGL rendering settings
 */
void setUp(void)
{
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);

    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    GLfloat mat_specular[] = {0.8f, 0.8f, 0.8f, 1.0f};
    GLfloat mat_shininess[] = {64.0f};

    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);

    GLfloat light0_pos[] = {1.0f, 1.0f, 1.0f, 0.0f};
    GLfloat light0_ambient[] = {0.05f, 0.05f, 0.05f, 1.0f};
    GLfloat light0_diffuse[] = {0.9f, 0.9f, 0.85f, 1.0f};
    GLfloat light0_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};

    glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);

    GLfloat light1_pos[] = {-5.0f, -2.0f, 8.0f, 1.0f};
    GLfloat light1_ambient[] = {0.02f, 0.02f, 0.02f, 1.0f};
    GLfloat light1_diffuse[] = {0.25f, 0.25f, 0.3f, 1.0f};
    GLfloat light1_specular[] = {0.2f, 0.2f, 0.2f, 1.0f};

    glLightfv(GL_LIGHT1, GL_POSITION, light1_pos);
    glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);

    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);

    GLfloat global_ambient[] = {0.05f, 0.05f, 0.05f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);

    glEnable(GL_TEXTURE_2D);
    loadTexture("../src/resources/dirt.jpg");
}

/**
 * @brief Window resize callback function
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
    const float INTENSITY_STEP = 0.05f;
    const float SHININESS_STEP = 4.0f;

    switch (key)
    {
    case 27:
        exit(0);
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
    case ',':
    case '<':
        shininess += SHININESS_STEP;
        if (shininess > 128.0f) shininess = 128.0f;
        std::cout << "Shininess: " << shininess << std::endl;
        glutPostRedisplay();
        break;
    case 'a':
    case 'A':
        objectTransform.translateX -= TRANSLATION_SPEED;
        glutPostRedisplay();
        break;
    case 'b':
    case 'B':
        specularIntensity -= INTENSITY_STEP;
        if (specularIntensity < 0.0f) specularIntensity = 0.0f;
        std::cout << "Specular Intensity: " << specularIntensity << std::endl;
        glutPostRedisplay();
        break;
    case 'c':
    case 'C':
        diffuseIntensity -= INTENSITY_STEP;
        if (diffuseIntensity < 0.0f) diffuseIntensity = 0.0f;
        std::cout << "Diffuse Intensity: " << diffuseIntensity << std::endl;
        glutPostRedisplay();
        break;
    case 'd':
    case 'D':
        objectTransform.translateX += TRANSLATION_SPEED;
        glutPostRedisplay();
        break;
    case 'e':
    case 'E':
        objectTransform.translateZ += TRANSLATION_SPEED;
        glutPostRedisplay();
        break;
    case 'i':
    case 'I':
        speedY += 0.1f;
        std::cout << "Y-axis Rotation Speed: " << speedY << std::endl;
        break;
    case 'k':
    case 'K':
        animateX = !animateX;
        std::cout << "X-axis Animation: " << (animateX ? "ON" : "OFF") << std::endl;
        glutPostRedisplay();
        break;
    case 'l':
    case 'L':
        animateY = !animateY;
        std::cout << "Y-axis Animation: " << (animateY ? "ON" : "OFF") << std::endl;
        glutPostRedisplay();
        break;
    case 'm':
    case 'M':
        shininess -= SHININESS_STEP;
        if (shininess < 1.0f) shininess = 1.0f;
        std::cout << "Shininess: " << shininess << std::endl;
        glutPostRedisplay();
        break;
    case 'n':
    case 'N':
        specularIntensity += INTENSITY_STEP;
        if (specularIntensity > 1.0f) specularIntensity = 1.0f;
        std::cout << "Specular Intensity: " << specularIntensity << std::endl;
        glutPostRedisplay();
        break;
    case 'o':
    case 'O':
        speedX -= 0.1f;
        std::cout << "X-axis Rotation Speed: " << speedX << std::endl;
        break;
    case 'p':
    case 'P':
        speedX += 0.1f;
        std::cout << "X-axis Rotation Speed: " << speedX << std::endl;
        break;
    case 'q':
    case 'Q':
        objectTransform.translateZ -= TRANSLATION_SPEED;
        glutPostRedisplay();
        break;
    case 's':
    case 'S':
        objectTransform.translateY -= TRANSLATION_SPEED;
        glutPostRedisplay();
        break;
    case 't':
    case 'T':
        textureEnabled = !textureEnabled;
        std::cout << "Texture: " << (textureEnabled ? "ON" : "OFF") << std::endl;
        glutPostRedisplay();
        break;
    case 'u':
    case 'U':
        speedY -= 0.1f;
        std::cout << "Y-axis Rotation Speed: " << speedY << std::endl;
        break;
    case 'v':
    case 'V':
        diffuseIntensity += INTENSITY_STEP;
        if (diffuseIntensity > 1.0f) diffuseIntensity = 1.0f;
        std::cout << "Diffuse Intensity: " << diffuseIntensity << std::endl;
        glutPostRedisplay();
        break;
    case 'w':
    case 'W':
        objectTransform.translateY += TRANSLATION_SPEED;
        glutPostRedisplay();
        break;
    case 'x':
    case 'X':
        ambientIntensity += INTENSITY_STEP;
        if (ambientIntensity > 1.0f) ambientIntensity = 1.0f;
        std::cout << "Ambient Intensity: " << ambientIntensity << std::endl;
        glutPostRedisplay();
        break;
    case 'y':
    case 'Y':
        alternateCameraView = !alternateCameraView;
        std::cout << "Alternate Camera View: " << (alternateCameraView ? "ON" : "OFF") << std::endl;
        glutPostRedisplay();
        break;
    case 'z':
    case 'Z':
        ambientIntensity -= INTENSITY_STEP;
        if (ambientIntensity < 0.0f) ambientIntensity = 0.0f;
        std::cout << "Ambient Intensity: " << ambientIntensity << std::endl;
        glutPostRedisplay();
        break;
    }
}

/**
 * @brief Keyboard input callback for special keys
 *
 * @param key GLUT special key code
 * @param x Mouse X position (unused)
 * @param y Mouse Y position (unused)
 */
void specialKeyInput(int key, int x, int y)
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

    glewExperimental = GL_TRUE;
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
    glutSpecialFunc(specialKeyInput);

    setUp();
    glutMainLoop();
    return 0;
}
