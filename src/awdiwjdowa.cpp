/**
 * @file awdiwjdowa.cpp
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
 * - Q/E: Move object along Z-axis (closer/farther)
 * - +/-: Scale the object (zoom in/out)
 * - 1/2: Toggle Light 0/Light 1
 * - ESC: Exit the program
 */

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <vector>
#include <cstring>

// ============================================================================
// CONSTANTS
// ============================================================================

/** @brief Mathematical constant PI for trigonometric calculations */
const float PI = 3.14159265f;

/** @brief Default teardrop radius at widest point */
const float TEARDROP_RADIUS = 1.5f;

/** @brief Number of longitudinal divisions for teardrop mesh */
const int TEARDROP_SLICES = 40;

/** @brief Number of latitudinal divisions for teardrop mesh */
const int TEARDROP_STACKS = 40;

/** @brief Child object offset from parent on X-axis */
const float CHILD_OFFSET_X = 3.0f;

/** @brief Child object uniform scale factor */
const float CHILD_SCALE = 0.5f;

/** @brief Child teapot size */
const float CHILD_TEAPOT_SIZE = 1.5f;

/** @brief Camera distance from origin */
const float CAMERA_DISTANCE = 10.0f;

// ============================================================================
// DATA STRUCTURES
// ============================================================================

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

/**
 * @struct LightConfig
 * @brief Configuration parameters for a single OpenGL light source
 *
 * Encapsulates position and color properties for ambient, diffuse,
 * and specular light components.
 */
struct LightConfig
{
    GLfloat position[4];  /**< Light position (x, y, z, w). w=1 for positional */
    GLfloat ambient[4];   /**< Ambient color component (RGBA) */
    GLfloat diffuse[4];   /**< Diffuse color component (RGBA) */
    GLfloat specular[4];  /**< Specular color component (RGBA) */
};

/**
 * @struct Color
 * @brief RGB color representation
 */
struct Color
{
    float r; /**< Red component (0.0 - 1.0) */
    float g; /**< Green component (0.0 - 1.0) */
    float b; /**< Blue component (0.0 - 1.0) */
};

// ============================================================================
// GLOBAL STATE
// ============================================================================

/** @brief Transform state for the main teardrop object */
Transform objectTransform;

/** @brief Parent object color (cyan/water blue) */
const Color PARENT_COLOR = {0.0f, 0.8f, 1.0f};

/** @brief Child object color (red) */
const Color CHILD_COLOR = {1.0f, 0.2f, 0.2f};

// ============================================================================
// SHADER SOURCES (GLSL)
// ============================================================================

/**
 * @brief Vertex shader source code for Phong lighting
 *
 * Transforms vertices to clip space and passes position/normal
 * to fragment shader for per-pixel lighting calculation.
 */
const char* vertexShaderSource = R"glsl(
#version 120
attribute vec3 aPos;
attribute vec3 aNormal;

varying vec3 FragPos;
varying vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = normalMatrix * aNormal;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)glsl";

/**
 * @brief Fragment shader source code for Phong lighting
 *
 * Implements Phong lighting model with two light sources,
 * calculating ambient, diffuse, and specular components per-pixel.
 */
const char* fragmentShaderSource = R"glsl(
#version 120

varying vec3 FragPos;
varying vec3 Normal;

// Light properties
uniform vec3 light0Pos;
uniform vec3 light0Ambient;
uniform vec3 light0Diffuse;
uniform vec3 light0Specular;
uniform bool light0Enabled;

uniform vec3 light1Pos;
uniform vec3 light1Ambient;
uniform vec3 light1Diffuse;
uniform vec3 light1Specular;
uniform bool light1Enabled;

// Material/Object properties
uniform vec3 objectColor;
uniform vec3 viewPos;

// Specular shininess
uniform float shininess;

vec3 calculateLight(vec3 lightPos, vec3 ambient, vec3 diffuse, vec3 specular, 
                    vec3 normal, vec3 fragPos, vec3 viewDir)
{
    // Ambient
    vec3 ambientResult = ambient * objectColor;
    
    // Diffuse
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuseResult = diffuse * diff * objectColor;
    
    // Specular (Blinn-Phong)
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
    vec3 specularResult = specular * spec;
    
    return ambientResult + diffuseResult + specularResult;
}

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    vec3 result = vec3(0.0);
    
    // Add contribution from Light 0
    if (light0Enabled) {
        result += calculateLight(light0Pos, light0Ambient, light0Diffuse, 
                                  light0Specular, norm, FragPos, viewDir);
    }
    
    // Add contribution from Light 1
    if (light1Enabled) {
        result += calculateLight(light1Pos, light1Ambient, light1Diffuse, 
                                  light1Specular, norm, FragPos, viewDir);
    }
    
    // If no lights enabled, show ambient only
    if (!light0Enabled && !light1Enabled) {
        result = objectColor * 0.2;
    }
    
    gl_FragColor = vec4(result, 1.0);
}
)glsl";

// ============================================================================
// GLOBAL SHADER STATE
// ============================================================================

/** @brief Shader program ID */
GLuint shaderProgram = 0;

/** @brief Vertex Array Object for teardrop */
GLuint teardropVAO = 0;

/** @brief Vertex Buffer Object for teardrop */
GLuint teardropVBO = 0;

/** @brief Element Buffer Object for teardrop */
GLuint teardropEBO = 0;

/** @brief Number of indices for teardrop mesh */
GLsizei teardropIndexCount = 0;

/** @brief Light 0 enabled state */
bool light0Enabled = true;

/** @brief Light 1 enabled state */
bool light1Enabled = true;

// ============================================================================
// RENDERING FUNCTIONS
// ============================================================================

/**
 * @brief Calculates vertex position and applies teardrop deformation
 * @param phi Latitude angle in radians
 * @param theta Longitude angle in radians
 * @param radius Base radius of the shape
 * @param[out] x Output X coordinate
 * @param[out] y Output Y coordinate
 * @param[out] z Output Z coordinate
 *
 * For vertices above the equator (y > 0), applies pinching and stretching
 * to create the pointed tip characteristic of a teardrop.
 */
void calculateTeardropVertex(float phi, float theta, float radius,
                              float &x, float &y, float &z)
{
    float r = radius;
    y = radius * cos(phi);

    // Apply teardrop deformation for upper hemisphere
    if (y > 0)
    {
        // Stretch y upward quadratically
        y = y + (y * y) / radius;
        // Pinch radius inward as we approach the tip
        float pinchFactor = 1.0f - (y / (radius * 2.0f));
        r *= pinchFactor;
    }

    x = r * sin(phi) * cos(theta);
    z = r * sin(phi) * sin(theta);
}

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

            // Calculate first vertex of the strip
            float x1, y1, z1;
            calculateTeardropVertex(phi1, theta, radius, x1, y1, z1);

            // Calculate second vertex of the strip
            float x2, y2, z2;
            calculateTeardropVertex(phi2, theta, radius, x2, y2, z2);

            // Emit vertices with normals for lighting
            glNormal3f(x1, y1, z1);
            glVertex3f(x1, y1, z1);

            glNormal3f(x2, y2, z2);
            glVertex3f(x2, y2, z2);
        }
        glEnd();
    }
}

/**
 * @brief Renders the child object (teapot) relative to parent
 *
 * Draws a smaller teapot as a demonstration of hierarchical modeling.
 * The child inherits parent transformations and applies its own offset.
 */
void drawChildObject()
{
    glPushMatrix();
    
    // Position child relative to parent
    glTranslatef(CHILD_OFFSET_X, 0.0f, 0.0f);
    glScalef(CHILD_SCALE, CHILD_SCALE, CHILD_SCALE);
    
    // Set child color and render
    glColor3f(CHILD_COLOR.r, CHILD_COLOR.g, CHILD_COLOR.b);
    glutSolidTeapot(CHILD_TEAPOT_SIZE);
    
    glPopMatrix();
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

    // Set up camera looking at origin
    gluLookAt(0.0, 0.0, CAMERA_DISTANCE,
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

    // Draw parent object (teardrop)
    glColor3f(PARENT_COLOR.r, PARENT_COLOR.g, PARENT_COLOR.b);
    drawTeardrop(TEARDROP_RADIUS, TEARDROP_SLICES, TEARDROP_STACKS);

    // Draw child object (teapot) - inherits parent transformations
    drawChildObject();

    glPopMatrix();

    glutSwapBuffers();
}

// ============================================================================
// SHADER FUNCTIONS (GLEW)
// ============================================================================

/**
 * @brief Compiles a shader from source code
 * @param type Shader type (GL_VERTEX_SHADER or GL_FRAGMENT_SHADER)
 * @param source GLSL source code string
 * @return Compiled shader ID, or 0 on failure
 */
GLuint compileShader(GLenum type, const char* source)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    
    // Check compilation status
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "Shader Compilation Error (" 
                  << (type == GL_VERTEX_SHADER ? "Vertex" : "Fragment") 
                  << "):\n" << infoLog << std::endl;
        return 0;
    }
    
    return shader;
}

/**
 * @brief Creates and links a shader program from vertex and fragment shaders
 * @param vertexSource Vertex shader GLSL source
 * @param fragmentSource Fragment shader GLSL source
 * @return Linked program ID, or 0 on failure
 */
GLuint createShaderProgram(const char* vertexSource, const char* fragmentSource)
{
    // Compile individual shaders
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);
    
    if (vertexShader == 0 || fragmentShader == 0)
    {
        return 0;
    }
    
    // Create and link program
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    
    // Check link status
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cerr << "Shader Program Linking Error:\n" << infoLog << std::endl;
        return 0;
    }
    
    // Clean up individual shaders (no longer needed after linking)
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return program;
}

/**
 * @brief Sets a mat4 uniform in the shader program
 * @param program Shader program ID
 * @param name Uniform variable name
 * @param matrix Pointer to 16-float matrix data (column-major)
 */
void setMat4(GLuint program, const char* name, const float* matrix)
{
    GLint loc = glGetUniformLocation(program, name);
    glUniformMatrix4fv(loc, 1, GL_FALSE, matrix);
}

/**
 * @brief Sets a mat3 uniform in the shader program
 * @param program Shader program ID
 * @param name Uniform variable name
 * @param matrix Pointer to 9-float matrix data (column-major)
 */
void setMat3(GLuint program, const char* name, const float* matrix)
{
    GLint loc = glGetUniformLocation(program, name);
    glUniformMatrix3fv(loc, 1, GL_FALSE, matrix);
}

/**
 * @brief Sets a vec3 uniform in the shader program
 * @param program Shader program ID
 * @param name Uniform variable name
 * @param x X component
 * @param y Y component
 * @param z Z component
 */
void setVec3(GLuint program, const char* name, float x, float y, float z)
{
    GLint loc = glGetUniformLocation(program, name);
    glUniform3f(loc, x, y, z);
}

/**
 * @brief Sets a float uniform in the shader program
 * @param program Shader program ID
 * @param name Uniform variable name
 * @param value Float value
 */
void setFloat(GLuint program, const char* name, float value)
{
    GLint loc = glGetUniformLocation(program, name);
    glUniform1f(loc, value);
}

/**
 * @brief Sets a bool uniform in the shader program
 * @param program Shader program ID
 * @param name Uniform variable name
 * @param value Boolean value
 */
void setBool(GLuint program, const char* name, bool value)
{
    GLint loc = glGetUniformLocation(program, name);
    glUniform1i(loc, value ? 1 : 0);
}

// ============================================================================
// INITIALIZATION FUNCTIONS
// ============================================================================

/**
 * @brief Configures a single OpenGL light source (legacy, kept for compatibility)
 * @param lightId OpenGL light identifier (GL_LIGHT0, GL_LIGHT1, etc.)
 * @param config Light configuration parameters
 */
void configureLight(GLenum lightId, const LightConfig &config)
{
    glLightfv(lightId, GL_POSITION, config.position);
    glLightfv(lightId, GL_AMBIENT, config.ambient);
    glLightfv(lightId, GL_DIFFUSE, config.diffuse);
    glLightfv(lightId, GL_SPECULAR, config.specular);
}

/**
 * @brief Initializes OpenGL and GLEW rendering settings with shader-based lighting
 *
 * Configures the following OpenGL states using modern GLEW pipeline:
 * - Background color (dark gray)
 * - Depth testing for proper 3D rendering
 * - Compiles and links Phong lighting shaders
 * - Sets up two light sources with ambient, diffuse, and specular components
 * - Initializes shader uniforms for lighting parameters
 *
 * This function uses GLEW for modern OpenGL shader-based lighting
 * instead of the deprecated fixed-function pipeline.
 */
void setUp(void)
{
    // Configure background and depth testing
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    
    // Create shader program using GLEW
    shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);
    if (shaderProgram == 0)
    {
        std::cerr << "Failed to create shader program. Falling back to fixed-function." << std::endl;
        
        // Fallback to legacy OpenGL lighting
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glEnable(GL_LIGHT1);
        glEnable(GL_COLOR_MATERIAL);

        LightConfig light0 = {
            {3.0f, 2.0f, 2.0f, 1.0f},
            {0.20f, 0.20f, 0.20f, 1.0f},
            {0.55f, 0.55f, 0.55f, 1.0f},
            {0.45f, 0.45f, 0.45f, 1.0f}
        };
        configureLight(GL_LIGHT0, light0);

        LightConfig light1 = {
            {-3.0f, 1.0f, 2.0f, 1.0f},
            {0.15f, 0.15f, 0.15f, 1.0f},
            {0.40f, 0.40f, 0.40f, 1.0f},
            {0.35f, 0.35f, 0.35f, 1.0f}
        };
        configureLight(GL_LIGHT1, light1);
        return;
    }
    
    std::cout << "GLEW Shader-based lighting initialized successfully!" << std::endl;
    
    // Use the shader program and set initial light uniforms
    glUseProgram(shaderProgram);
    
    // Light 0: Primary light from the right (GLEW shader uniforms)
    setVec3(shaderProgram, "light0Pos", 3.0f, 2.0f, 2.0f);
    setVec3(shaderProgram, "light0Ambient", 0.20f, 0.20f, 0.20f);
    setVec3(shaderProgram, "light0Diffuse", 0.55f, 0.55f, 0.55f);
    setVec3(shaderProgram, "light0Specular", 0.45f, 0.45f, 0.45f);
    setBool(shaderProgram, "light0Enabled", true);
    
    // Light 1: Fill light from the left (GLEW shader uniforms)
    setVec3(shaderProgram, "light1Pos", -3.0f, 1.0f, 2.0f);
    setVec3(shaderProgram, "light1Ambient", 0.15f, 0.15f, 0.15f);
    setVec3(shaderProgram, "light1Diffuse", 0.40f, 0.40f, 0.40f);
    setVec3(shaderProgram, "light1Specular", 0.35f, 0.35f, 0.35f);
    setBool(shaderProgram, "light1Enabled", true);
    
    // Set material shininess for specular highlights
    setFloat(shaderProgram, "shininess", 32.0f);
    
    // Also enable legacy lighting as fallback for glutSolidTeapot
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_COLOR_MATERIAL);

    LightConfig light0Legacy = {
        {3.0f, 2.0f, 2.0f, 1.0f},
        {0.20f, 0.20f, 0.20f, 1.0f},
        {0.55f, 0.55f, 0.55f, 1.0f},
        {0.45f, 0.45f, 0.45f, 1.0f}
    };
    configureLight(GL_LIGHT0, light0Legacy);

    LightConfig light1Legacy = {
        {-3.0f, 1.0f, 2.0f, 1.0f},
        {0.15f, 0.15f, 0.15f, 1.0f},
        {0.40f, 0.40f, 0.40f, 1.0f},
        {0.35f, 0.35f, 0.35f, 1.0f}
    };
    configureLight(GL_LIGHT1, light1Legacy);
    
    glUseProgram(0); // Reset to fixed-function for compatibility
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
    // Prevent division by zero
    if (h == 0)
        h = 1;
    
    float aspectRatio = (float)w / h;
    
    // Update viewport to match new window size
    glViewport(0, 0, w, h);
    
    // Reconfigure projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, aspectRatio, 1.0f, 100.0f);
    
    // Return to modelview for rendering
    glMatrixMode(GL_MODELVIEW);
}

// ============================================================================
// INPUT HANDLING
// ============================================================================

/** @brief Movement speed for translation controls */
const float TRANSLATION_SPEED = 0.2f;

/** @brief Increment step for scale adjustments */
const float SCALE_STEP = 0.1f;

/** @brief Minimum allowed scale value */
const float MIN_SCALE = 0.1f;

/** @brief Rotation speed in degrees per key press */
const float ROTATION_SPEED = 5.0f;

/**
 * @brief Toggles the specified OpenGL light source (both legacy and GLEW shader)
 * @param lightId OpenGL light identifier (GL_LIGHT0, GL_LIGHT1, etc.)
 *
 * Updates both the legacy fixed-function pipeline and GLEW shader uniforms
 * to ensure consistent lighting behavior.
 */
void toggleLight(GLenum lightId)
{
    // Toggle legacy OpenGL light
    if (glIsEnabled(lightId))
        glDisable(lightId);
    else
        glEnable(lightId);
    
    // Update GLEW shader uniform if shader program is active
    if (shaderProgram != 0)
    {
        glUseProgram(shaderProgram);
        
        if (lightId == GL_LIGHT0)
        {
            light0Enabled = !light0Enabled;
            setBool(shaderProgram, "light0Enabled", light0Enabled);
        }
        else if (lightId == GL_LIGHT1)
        {
            light1Enabled = !light1Enabled;
            setBool(shaderProgram, "light1Enabled", light1Enabled);
        }
        
        glUseProgram(0); // Reset to fixed-function
    }
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
    (void)x; // Suppress unused parameter warning
    (void)y; // Suppress unused parameter warning

    switch (key)
    {
    // Z-axis translation
    case 'q':
        objectTransform.translateZ -= TRANSLATION_SPEED;
        break;
    case 'e':
        objectTransform.translateZ += TRANSLATION_SPEED;
        break;
    
    // Y-axis translation
    case 'w':
        objectTransform.translateY += TRANSLATION_SPEED;
        break;
    case 's':
        objectTransform.translateY -= TRANSLATION_SPEED;
        break;
    
    // X-axis translation
    case 'a':
        objectTransform.translateX -= TRANSLATION_SPEED;
        break;
    case 'd':
        objectTransform.translateX += TRANSLATION_SPEED;
        break;
    
    // Scaling
    case '+':
        objectTransform.scale += SCALE_STEP;
        break;
    case '-':
        objectTransform.scale -= SCALE_STEP;
        if (objectTransform.scale < MIN_SCALE)
            objectTransform.scale = MIN_SCALE;
        break;
    
    // Light toggles
    case '1':
        toggleLight(GL_LIGHT0);
        break;
    case '2':
        toggleLight(GL_LIGHT1);
        break;
    
    // Exit
    case 27: // ESC key
        exit(0);
        break;
    
    default:
        return; // Unknown key, don't redisplay
    }
    
    glutPostRedisplay();
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
    (void)x; // Suppress unused parameter warning
    (void)y; // Suppress unused parameter warning

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
    default:
        return; // Unknown key, don't redisplay
    }
    
    glutPostRedisplay();
}

// ============================================================================
// MAIN ENTRY POINT
// ============================================================================

/** @brief Window title displayed in title bar */
const char* WINDOW_TITLE = "Project UAS Komgraf (Kelompok 4)";

/** @brief Initial window width in pixels */
const int WINDOW_WIDTH = 800;

/** @brief Initial window height in pixels */
const int WINDOW_HEIGHT = 600;

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
    // Initialize GLUT library
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow(WINDOW_TITLE);

    // Initialize GLEW for OpenGL extension loading
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        std::cerr << "GLEW Error: " << glewGetErrorString(err) << std::endl;
        return 1;
    }

    // Register callback functions
    glutDisplayFunc(drawScene);
    glutReshapeFunc(resize);
    glutKeyboardFunc(keyInput);
    glutSpecialFunc(specialInput);

    // Initialize OpenGL state
    setUp();
    
    // Enter main event loop (never returns)
    glutMainLoop();
    
    return 0;
}
