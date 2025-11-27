# Project UAS Komputer Grafik - Kelompok 4

> 3D Teardrop Shape Renderer menggunakan OpenGL dan FreeGLUT

## 📋 Daftar Anggota Kelompok

| No | Nama | NIM |
|----|------|-----|
| 1  | -    | -   |
| 2  | -    | -   |
| 3  | -    | -   |
| 4  | -    | -   |

---

## ✅ Checklist Implementasi

### Functionality (50 pts)

#### 2 Objects Present and Rendered Correctly (18 pts)
- [x] **Object 1: Teardrop (Procedural)** - Custom parametric shape dibuat dengan modifikasi sphere (9 pts)
- [x] **Object 2: Teapot (Library)** - Menggunakan `glutSolidTeapot()` dari FreeGLUT (9 pts)

#### Transformations & Hierarchical Modeling (10 pts)
- [x] **Translation** - Objek dapat dipindahkan dengan WASD dan Q/E (sumbu X, Y, Z)
- [x] **Rotation** - Objek dapat diputar dengan Arrow Keys (sumbu X dan Y)
- [x] **Scaling** - Objek dapat di-zoom dengan +/- keys
- [x] **Hierarchical Modeling** - Teapot sebagai child object dari Teardrop (transformasi parent mempengaruhi child)

#### Lighting with at least 2 Lights and Toggles (10 pts)
- [x] **Light 0 (Right)** - Posisi (3, 2, 2) dengan ambient, diffuse, specular
- [x] **Light 1 (Left)** - Posisi (-3, 1, 2) dengan ambient, diffuse, specular
- [x] **Toggle Light 0** - Tekan tombol `1` untuk on/off
- [x] **Toggle Light 1** - Tekan tombol `2` untuk on/off

#### Texture Mapping (8 pts)
- [ ] Texture mapping pada minimal 1 objek
- [ ] Proper UV coordinates

#### Camera & User Controls (4 pts)
- [x] **Camera** - Menggunakan `gluLookAt()` dengan posisi tetap
- [x] **Toggle Lights** - Tombol 1 dan 2
- [ ] Toggle Texture
- [ ] Toggle Animation

---

### Code Quality & Build (30 pts)

#### Builds and Runs as Described (15 pts)
- [x] Program dapat di-compile dengan g++ dan library OpenGL
- [x] Program berjalan tanpa error/crash
- [x] Build task tersedia di `.vscode/tasks.json`

#### Code Structure, Comments, Readability (8 pts)
- [x] **Doxygen-style comments** - Semua fungsi memiliki dokumentasi lengkap
- [x] **Organized code structure** - Menggunakan `struct Transform` untuk grouping data
- [x] **Named constants** - `TRANSLATION_SPEED`, `ROTATION_SPEED`, `SCALE_STEP`, dll.
- [x] **Section comments** - Kode diorganisir dengan section headers
- [x] **Meaningful variable names** - `objectTransform.translateX` bukan `tx`

#### Proper Error Checking (7 pts)
- [ ] Shader compile error checking
- [ ] File load error checking
- [x] Division by zero prevention (pada `resize()` function)

---

### Report and Demo (20 pts)

#### Short Report (max 2 pages)
- [ ] Deskripsi project dan fitur
- [ ] Penjelasan teknis implementasi
- [ ] Problems encountered dan solusinya

#### Screencast/GIF Demo (1-2 minutes)
- [ ] Video/GIF demonstrasi program berjalan

---

## 🎮 Kontrol

| Key | Action |
|-----|--------|
| `W` | Gerak ke atas (translate Y+) |
| `S` | Gerak ke bawah (translate Y-) |
| `A` | Gerak ke kiri (translate X-) |
| `D` | Gerak ke kanan (translate X+) |
| `Q` | Gerak mendekat (translate Z-) |
| `E` | Gerak menjauh (translate Z+) |
| `↑` | Rotasi ke atas (rotate X-) |
| `↓` | Rotasi ke bawah (rotate X+) |
| `←` | Rotasi ke kiri (rotate Y-) |
| `→` | Rotasi ke kanan (rotate Y+) |
| `+` | Zoom in (scale+) |
| `-` | Zoom out (scale-) |
| `1` | Toggle Light 0 |
| `2` | Toggle Light 1 |
| `ESC` | Keluar program |

---

## 🛠️ Build & Run

### Prerequisites
- MinGW with g++
- FreeGLUT library
- OpenGL32 and GLU32

### Compile
```bash
g++ -IC:/MinGW/include -LC:/MinGW/lib src/test.cpp -o build/test.exe -lfreeglut -lopengl32 -lglu32
```

### Run
```bash
./build/test.exe
```

---

## 📁 Struktur Project

```
project_uas_komgraf/
├── README.md
├── src/
│   ├── test.cpp          # Main application
│   ├── main.cpp          # Alternative main
│   └── controls/
│       └── controls.cpp  # Control utilities
└── build/
    └── *.exe             # Compiled executables
```

---

## 📝 Catatan Teknis

### Teardrop Shape Algorithm
Bentuk teardrop dibuat dengan memodifikasi parametric sphere:
1. Untuk vertices di upper hemisphere (y > 0):
   - Y-coordinate di-stretch ke atas: `y = y + (y²/radius)`
   - Radius di-pinch ke dalam: `r *= (1 - y/(2*radius))`
2. Hasil: bentuk seperti tetesan air dengan ujung runcing di atas

### Hierarchical Modeling
```cpp
glPushMatrix();
    // Parent transformations (Teardrop)
    glTranslatef(...);
    glRotatef(...);
    glScalef(...);
    drawTeardrop(...);
    
    // Child object (Teapot) - inherits parent transforms
    glPushMatrix();
        glTranslatef(3.0f, 0.0f, 0.0f);  // Local offset
        glutSolidTeapot(1.5);
    glPopMatrix();
glPopMatrix();
```

---

## 🔧 TODO

- [ ] Implementasi texture mapping
- [ ] Tambah animasi otomatis
- [ ] Error checking untuk shader
- [ ] Buat laporan (max 2 halaman)
- [ ] Rekam video demo (1-2 menit)
