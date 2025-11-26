/**
 * @file test.cpp
 * @brief Program untuk menggambar bentuk teardrop 3D menggunakan OpenGL
 * 
 * Program ini membuat bentuk teardrop dengan memodifikasi bola standar,
 * bagian atas dibuat lancip seperti tetesan air.
 * Kontrol: Arrow keys untuk rotasi objek.
 */

#include <GL/freeglut.h>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <vector>

const float PI = 3.14159265f;

float angleX = 15.0f;  /**< Sudut rotasi sumbu X */
float angleY = 0.0f;   /**< Sudut rotasi sumbu Y */

/**
 * @brief Menggambar bentuk teardrop 3D
 * 
 * Fungsi ini membuat bentuk teardrop dengan memodifikasi bola standar.
 * Bagian atas (y > 0) diubah menjadi lancip dengan memperkecil radius
 * dan memanjangkan koordinat Y.
 * 
 * @param radius Radius dasar bentuk
 * @param slices Jumlah segmen horizontal (detail keliling)
 * @param stacks Jumlah segmen vertikal (detail tinggi)
 */
void drawTeardrop(float radius, int slices, int stacks)
{
    for (int i = 0; i < stacks; ++i)
    {
        /* phi: sudut dari atas (0) ke bawah (PI) */
        float phi1 = (float)i / stacks * PI;
        float phi2 = (float)(i + 1) / stacks * PI;

        glBegin(GL_TRIANGLE_STRIP);

        for (int j = 0; j <= slices; ++j)
        {
            /* theta: sudut keliling (0 sampai 2*PI) */
            float theta = (float)j / slices * 2.0f * PI;

            /* Vertex 1: tepi atas strip */
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

            /* Vertex 2: tepi bawah strip */
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

            /* Gambar vertex dengan normal untuk lighting */
            glNormal3f(x1, y1, z1);
            glVertex3f(x1, y1, z1);

            glNormal3f(x2, y2, z2);
            glVertex3f(x2, y2, z2);
        }
        glEnd();
    }
}

/**
 * @brief Fungsi callback untuk menggambar scene
 * 
 * Dipanggil setiap kali window perlu digambar ulang.
 * Mengatur kamera, rotasi, dan memanggil drawTeardrop.
 */
void drawScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    /* Posisi kamera: di depan objek */
    gluLookAt(0.0, 0.0, 10.0,
              0.0, 0.0, 0.0,
              0.0, 1.0, 0.0);

    /* Terapkan rotasi berdasarkan input user */
    glRotatef(angleX, 1.0f, 0.0f, 0.0f);
    glRotatef(angleY, 0.0f, 1.0f, 0.0f);

    /* Gambar teardrop dengan warna cyan */
    glColor3f(0.0f, 0.8f, 1.0f);
    drawTeardrop(1.5f, 40, 40);

    glutSwapBuffers();
}

/**
 * @brief Inisialisasi pengaturan OpenGL
 * 
 * Mengatur warna background, depth test, dan lighting.
 */
void setUp(void)
{
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    GLfloat light_pos[] = {1.0f, 1.0f, 1.0f, 0.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
}

/**
 * @brief Callback untuk resize window
 * 
 * Mengatur viewport dan projection matrix saat window di-resize.
 * 
 * @param w Lebar window baru
 * @param h Tinggi window baru
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
 * @brief Callback untuk input keyboard khusus (arrow keys)
 * 
 * Mengontrol rotasi objek dengan arrow keys.
 * - UP/DOWN: rotasi sumbu X
 * - LEFT/RIGHT: rotasi sumbu Y
 * 
 * @param key Kode tombol yang ditekan
 * @param x Posisi x mouse (tidak digunakan)
 * @param y Posisi y mouse (tidak digunakan)
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
 * @brief Fungsi utama program
 * 
 * Inisialisasi GLUT, membuat window, dan memulai main loop.
 */
int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Project UAS Komgraf (Kelompok 4)");

    glutDisplayFunc(drawScene);
    glutReshapeFunc(resize);
    glutSpecialFunc(specialInput);

    setUp();
    glutMainLoop();
    return 0;
}
