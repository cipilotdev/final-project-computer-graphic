# Project UAS Komputer Grafik - Kelompok 4

> 3D Teardrop Shape Renderer menggunakan OpenGL dan FreeGLUT

## 📋 Daftar Anggota Kelompok

| No  | Nama                            | NIM       |
| --- | ------------------------------- | --------- |
| 1   | Davidson Rafael Krisman Nugroho | 412024030 |
| 2   | Yedija Teofilus Yonathan        | 412024    |
| 3   | Michael Tandeas                 | 412024    |
| 4   | Janisha Jaya                    | 412024    |

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

- [x] **Light 0 (Key Light)** - Directional light (1, 1, 1) dengan ambient, diffuse, specular
- [x] **Light 1 (Fill Light)** - Positional light (-5, -2, 8) dengan ambient, diffuse, specular
- [x] **Toggle Light 0** - Tekan tombol `1` untuk on/off
- [x] **Toggle Light 1** - Tekan tombol `2` untuk on/off
- [x] **Real-time Lighting Adjustment** - Kontrol intensity (ambient, diffuse, specular) dan shininess

#### Texture Mapping (8 pts)

- [x] **Texture mapping** pada teardrop object dengan dirt.jpg
- [x] **Proper UV coordinates** menggunakan spherical mapping
- [x] **Toggle texture** dengan tombol T
- [x] **Mipmapping** untuk smooth rendering

#### Camera & User Controls (4 pts)

- [x] **Camera** - Menggunakan `gluLookAt()` dengan posisi tetap
- [x] **Toggle Lights** - Tombol 1 dan 2
- [x] **Toggle Texture** - Tombol T
- [x] **Toggle Animation** - Tombol K (X-axis) dan L (Y-axis)
- [x] **Alternate Camera View** - Tombol Y

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

- [ ] Shader compile error checking (N/A - not using shaders)
- [x] **File load error checking** - Texture loading dengan fallback mechanism
- [x] **Division by zero prevention** - Pada `resize()` function
- [x] **OpenGL error checking** - `checkGLError()` function untuk debugging

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

### Movement Controls

| Key | Action                        |
| --- | ----------------------------- |
| `W` | Gerak ke atas (translate Y+)  |
| `S` | Gerak ke bawah (translate Y-) |
| `A` | Gerak ke kiri (translate X-)  |
| `D` | Gerak ke kanan (translate X+) |
| `Q` | Gerak mendekat (translate Z-) |
| `E` | Gerak menjauh (translate Z+)  |
| `↑` | Rotasi ke atas (rotate X-)    |
| `↓` | Rotasi ke bawah (rotate X+)   |
| `←` | Rotasi ke kiri (rotate Y-)    |
| `→` | Rotasi ke kanan (rotate Y+)   |
| `+` | Zoom in (scale+)              |
| `-` | Zoom out (scale-)             |

### Camera Controls

| Key | Action                       |
| --- | ---------------------------- |
| `Y` | Toggle alternate camera view |

### Animation Controls

| Key | Action                           |
| --- | -------------------------------- |
| `K` | Toggle X-axis rotation animation |
| `L` | Toggle Y-axis rotation animation |
| `O` | Decrease X-axis rotation speed   |
| `P` | Increase X-axis rotation speed   |
| `U` | Decrease Y-axis rotation speed   |
| `I` | Increase Y-axis rotation speed   |

### Lighting Controls

| Key | Action                              |
| --- | ----------------------------------- |
| `1` | Toggle Light 0                      |
| `2` | Toggle Light 1                      |
| `Z` | Decrease ambient intensity          |
| `X` | Increase ambient intensity          |
| `C` | Decrease diffuse intensity          |
| `V` | Increase diffuse intensity          |
| `B` | Decrease specular intensity         |
| `N` | Increase specular intensity         |
| `M` | Decrease shininess (specular power) |
| `,` | Increase shininess (specular power) |

### Texture Controls

| Key | Action                |
| --- | --------------------- |
| `T` | Toggle texture on/off |

### Other

| Key   | Action         |
| ----- | -------------- |
| `ESC` | Keluar program |

---

## 🛠️ Build & Run

### Prerequisites

- MinGW with g++
- FreeGLUT library
- OpenGL32 and GLU32

### Compile

```bash
g++ -IC:/MinGW/include -LC:/MinGW/lib src/hehehe.cpp -o build/hehehe.exe -lglew32 -lfreeglut -lopengl32 -lglu32
```

### Run

```bash
./build/hehehe.exe
```

Atau gunakan VS Code task (Ctrl+Shift+B)

---

## 📁 Struktur Project

```
project_uas_komgraf/
├── README.md
├── src/
│   ├── hehehe.cpp        # Main application (v1.1)
│   ├── lol.cpp           # Development version
│   ├── main.cpp          # Legacy version
│   ├── minecraft.cpp     # Test version
│   ├── test.cpp          # Test version
│   ├── include/
│   │   └── stb_image.h   # Image loading library
│   └── resources/
│       └── dirt.jpg      # Texture file
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

- [x] Implementasi texture mapping
- [x] Tambah animasi otomatis (K/L keys)
- [x] Real-time lighting adjustment
- [x] Multiple camera views
- [ ] Error checking untuk file loading
- [ ] Buat laporan (max 2 halaman)
- [ ] Rekam video demo (1-2 menit)

---

## 🎨 Fitur Tambahan

- ✅ **Advanced Lighting System** - 2 lights dengan realistic key + fill light setup
- ✅ **Real-time Lighting Control** - Adjust ambient, diffuse, specular intensity dan shininess
- ✅ **Animation System** - Independent X/Y axis rotation dengan adjustable speed
- ✅ **Multiple Camera Views** - Toggle antara default dan alternate view
- ✅ **Texture Mapping** - Dengan mipmapping dan UV coordinates
- ✅ **Clean Code** - Doxygen documentation, organized structure, alphabetical keybinds
