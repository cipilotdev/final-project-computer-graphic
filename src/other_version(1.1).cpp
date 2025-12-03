/**
 * @file combined_main.cpp
 * @brief 3D Teardrop Shape Renderer (Combined Version)
 * @author Group 4
 * @version 1.1
 *
 * This program renders a 3D teardrop shape by modifying a standard sphere.
 * It combines features from the main rendering logic and the animation test suite.
 *
 * @section controls Controls
 * - Arrow Keys: Rotate the object (X and Y axis)
 * - W/A/S/D: Translate the object
 * - Q/E: Translate object Z-axis
 * - +/-: Scale the object (zoom in/out)
 * - ESC: Exit the program
 * - P: Toggle Camera View AND Increase X Rotation Speed (Merged Functionality)
 * - T: Toggle texture on/off
 * - 1/2: Toggle Light 0/1
 *
 * Lighting Controls:
 * - Z/X: Decrease/Increase ambient intensity
 * - C/V: Decrease/Increase diffuse intensity
 * - B/N: Decrease/Increase specular intensity
 * - M/,: Decrease/Increase shininess
 *
 * Animation Controls:
 * - K/L: Toggle Animation X/Y
 * - U/I: Decrease/Increase Speed Y
 * - O: Decrease Speed X
 */

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "include/stb_image.h" // Ensure this file is in your project directory

/** @brief Mathematical constant PI for trigonometric calculations */
const float PI = 3.14159265f;

// --- ANIMATION GLOBALS (From test.cpp) ---
bool animateX = false;
bool animateY = false;

float speedX = 1.0f; // Speed for X rotation
float speedY = 1.0f; // Speed for Y rotation

/**
 * @struct Transform
 * @brief Encapsulates all transformation properties for a 3D object
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

// --- CAMERA GLOBALS (From main.cpp) ---
/** @brief Status boolean for camera position. true = alternate camera. */
bool alternateCameraView = false;

/** @brief OpenGL texture ID for the teardrop surface texture */
GLuint teardropTexture;

/** @brief Flag to toggle texture on/off */
bool textureEnabled = true;

/** @brief Ambient light intensity */
float ambientIntensity = 0.05f;

/** @brief Diffuse light intensity */
float diffuseIntensity = 0.9f;

/** @brief Specular light intensity */
float specularIntensity = 1.0f;

/** @brief Material shininess/specular power */
float shininess = 64.0f;

/**
 * @brief Checks and reports OpenGL errors
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
        // Default to Y-axis up vector if length is near zero
        x = 0.0f;
        y = 1.0f;
        z = 0.0f;
    }
}

/**
 * @brief Renders a 3D teardrop shape using triangle strips
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

/**
 * @brief Loads a texture from an image file
 */
void loadTexture(const char *filename)
{
    int width, height, channels;
    unsigned char *data = stbi_load(filename, &width, &height, &channels, 0);

    if (!data)
    {
        std::cout << "[ERROR] Failed to load texture: " << filename << "\n"
                  << "        Reason: " << stbi_failure_reason() << "\n"
                  << std::endl;
        textureEnabled = false;
        return;
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
    checkGLError("loadTexture");
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

    GLfloat mat_shininess[] = {shininess};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
}

/**
 * @brief Main rendering callback function
 * Combines Camera logic from main.cpp and Animation logic from test.cpp
 */
void drawScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // --- CAMERA LOGIC (From main.cpp) ---
    if (alternateCameraView)
    {
        // Alternate Camera: Top-Down Angled View
        gluLookAt(0.0, 5.0, 5.0,
                  0.0, 0.0, 0.0,
                  0.0, 1.0, 0.0);
    }
    else
    {
        // Default Camera
        gluLookAt(0.0, 0.0, 10.0,
                  0.0, 0.0, 0.0,
                  0.0, 1.0, 0.0);
    }

    updateLighting();

    // Set light positions after camera setup (in eye space)
    GLfloat light0_pos[] = {1.0f, 1.0f, 1.0f, 0.0f}; // Directional
    glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);

    GLfloat light1_pos[] = {-5.0f, -2.0f, 8.0f, 1.0f}; // Positional
    glLightfv(GL_LIGHT1, GL_POSITION, light1_pos);

    glPushMatrix();
    // Apply object transformations
    glTranslatef(objectTransform.translateX,
                 objectTransform.translateY,
                 objectTransform.translateZ);
    glRotatef(objectTransform.rotateX, 1.0f, 0.0f, 0.0f);
    glRotatef(objectTransform.rotateY, 0.0f, 1.0f, 0.0f);
    glScalef(objectTransform.scale,
             objectTransform.scale,
             objectTransform.scale);

    // Draw parent object (Teardrop)
    if (textureEnabled)
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, teardropTexture);
    }
    else
    {
        glDisable(GL_TEXTURE_2D);
    }

    // Optional tint
    glColor3f(0.0f, 0.8f, 1.0f);

    drawTeardrop(1.5f, 40, 40);

    // Draw child object (smaller teapot)
    glPushMatrix();
    glTranslatef(3.0f, 0.0f, 0.0f);
    glScalef(0.5f, 0.5f, 0.5f);
    glColor3f(0.5f, 0.35f, 0.05f);
    glutSolidTeapot(1.5);
    glPopMatrix();

    glPopMatrix();

    glutSwapBuffers();

    // --- ANIMATION LOGIC (From test.cpp) ---
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

    // Light 0: KEY LIGHT (Directional - like sun)
    GLfloat light0_pos[] = {1.0f, 1.0f, 1.0f, 0.0f};
    GLfloat light0_ambient[] = {0.05f, 0.05f, 0.05f, 1.0f};
    GLfloat light0_diffuse[] = {0.9f, 0.9f, 0.85f, 1.0f};
    GLfloat light0_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};

    glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);

    // Light 1: FILL LIGHT (Positional - like lamp)
    GLfloat light1_pos[] = {-5.0f, -2.0f, 8.0f, 1.0f};
    GLfloat light1_ambient[] = {0.02f, 0.02f, 0.02f, 1.0f};
    GLfloat light1_diffuse[] = {0.25f, 0.25f, 0.3f, 1.0f};
    GLfloat light1_specular[] = {0.2f, 0.2f, 0.2f, 1.0f};

    glLightfv(GL_LIGHT1, GL_POSITION, light1_pos);
    glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);

    // Light Model Configuration
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
    GLfloat global_ambient[] = {0.05f, 0.05f, 0.05f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);

    glEnable(GL_TEXTURE_2D);
    loadTexture("../src/resources/dirt.jpg");
    checkGLError("setUp");
}

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

void keyInput(unsigned char key, int x, int y)
{
    const float TRANSLATION_SPEED = 0.2f;
    const float SCALE_STEP = 0.1f;
    const float MIN_SCALE = 0.1f;
    const float INTENSITY_STEP = 0.05f;
    const float SHININESS_STEP = 4.0f;

    switch (key)
    {
    case 27: // ESC key
        exit(0);
        break;
    case 'q':
        objectTransform.translateZ -= TRANSLATION_SPEED;
        glutPostRedisplay();
        break;
    case 'e':
        objectTransform.translateZ += TRANSLATION_SPEED;
        glutPostRedisplay();
        break;
    case 'w':
    case 'W':
        objectTransform.translateY += TRANSLATION_SPEED;
        glutPostRedisplay();
        break;
    case 's':
    case 'S':
        objectTransform.translateY -= TRANSLATION_SPEED;
        glutPostRedisplay();
        break;
    case 'a':
    case 'A':
        objectTransform.translateX -= TRANSLATION_SPEED;
        glutPostRedisplay();
        break;
    case 'd':
    case 'D':
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
    case 't':
    case 'T':
        textureEnabled = !textureEnabled;
        glutPostRedisplay();
        break;

    // --- COMBINED CONFLICT: 'P' ---
    // In main.cpp 'P' toggles camera.
    // In test.cpp 'P' increases speed X.
    // Combining both actions here to preserve logic without manipulation.
    case 'Y':
    case 'y':
        // 1. Camera Logic (From main.cpp)
        alternateCameraView = !alternateCameraView;
        std::cout << "Camera View Toggled. Alternate: " << (alternateCameraView ? "ON" : "OFF") << std::endl;
        glutPostRedisplay();
        break;

    // --- ANIMATION CONTROLS (From test.cpp) ---
    // Y-AXIS SPEED
    case 'u':
    case 'U':
        speedY -= 0.1f;
        std::cout << "Y Speed: " << speedY << std::endl;
        break;
    case 'i':
    case 'I':
        speedY += 0.1f;
        std::cout << "Y Speed: " << speedY << std::endl;
        break;

    // X-AXIS SPEED (Decrease only, Increase is merged with P)
    case 'o':
    case 'O':
        speedX -= 0.1f;
        std::cout << "X Speed: " << speedX << std::endl;
        break;

    case 'p':
    case 'P':
        speedX += 0.1f;
        std::cout << "X Speed: " << speedX << std::endl;
        break;

    // ANIMATION TOGGLES
    case 'k':
    case 'K':
        animateX = !animateX;
        glutPostRedisplay();
        break;
    case 'l':
    case 'L':
        animateY = !animateY;
        glutPostRedisplay();
        break;

    // --- LIGHTING CONTROLS (Common) ---
    case 'z':
    case 'Z':
        ambientIntensity -= INTENSITY_STEP;
        if (ambientIntensity < 0.0f)
            ambientIntensity = 0.0f;
        std::cout << "Ambient Intensity: " << ambientIntensity << std::endl;
        glutPostRedisplay();
        break;
    case 'x':
    case 'X':
        ambientIntensity += INTENSITY_STEP;
        if (ambientIntensity > 1.0f)
            ambientIntensity = 1.0f;
        std::cout << "Ambient Intensity: " << ambientIntensity << std::endl;
        glutPostRedisplay();
        break;
    case 'c':
    case 'C':
        diffuseIntensity -= INTENSITY_STEP;
        if (diffuseIntensity < 0.0f)
            diffuseIntensity = 0.0f;
        std::cout << "Diffuse Intensity: " << diffuseIntensity << std::endl;
        glutPostRedisplay();
        break;
    case 'v':
    case 'V':
        diffuseIntensity += INTENSITY_STEP;
        if (diffuseIntensity > 1.0f)
            diffuseIntensity = 1.0f;
        std::cout << "Diffuse Intensity: " << diffuseIntensity << std::endl;
        glutPostRedisplay();
        break;
    case 'b':
    case 'B':
        specularIntensity -= INTENSITY_STEP;
        if (specularIntensity < 0.0f)
            specularIntensity = 0.0f;
        std::cout << "Specular Intensity: " << specularIntensity << std::endl;
        glutPostRedisplay();
        break;
    case 'n':
    case 'N':
        specularIntensity += INTENSITY_STEP;
        if (specularIntensity > 1.0f)
            specularIntensity = 1.0f;
        std::cout << "Specular Intensity: " << specularIntensity << std::endl;
        glutPostRedisplay();
        break;
    case 'm':
    case 'M':
        shininess -= SHININESS_STEP;
        if (shininess < 1.0f)
            shininess = 1.0f;
        std::cout << "Shininess: " << shininess << std::endl;
        glutPostRedisplay();
        break;
    case ',':
    case '<':
        shininess += SHININESS_STEP;
        if (shininess > 128.0f)
            shininess = 128.0f;
        std::cout << "Shininess: " << shininess << std::endl;
        glutPostRedisplay();
        break;
    }
}

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

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Project UAS Komgraf (Kelompok 4) - Combined");

    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        std::cerr << "Error: " << glewGetString(err) << std::endl;
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