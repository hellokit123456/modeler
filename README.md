
##  Team Members

* **Tanishk Nath Goswami** (Roll No. 24b2165)
* ** Heet Patel** (Roll No. 24b0020)

---

##  Declaration

We affirm that:

* The assignment code was written by us.
* We used online tutorials, documentation, and chatbots **only for reference/clarification particularly for concepts we didn't already know**.
* External references:

  * [LearnOpenGL](https://learnopengl.com)
  * [GLM Documentation](https://github.com/g-truc/glm)
  * [GLFW Docs](https://www.glfw.org/docs/latest/)
  * ChatGPT (for conceptual understanding)
  * CS 675 Youtube videos

---

##  Project Overview

This project is a **3D Modeller and Inspector tool** built in **C++ with OpenGL**. It allows users to:

* Construct models from **basic geometric primitives** (sphere, cylinder, box, cone).
* **Transform** (rotate, translate, scale) these shapes.
* **Change colors** interactively.
* **Save** models into `.mod` files (custom text-based format).
* **Load and inspect** models, with auto camera centering.

The system is divided into three major components:

1. **Shape Classes** – Encapsulate geometry and tessellation logic.
2. **Model Hierarchy** – Tree-based structure that combines multiple shapes.
3. **User Interface** – Input handling for modelling and inspection modes.

---

##  Design Details

### 1. `shape_t` (Abstract Parent Class)

* **Members:**

  * `std::vector<glm::vec4> vertices` – stores vertex positions.
  * `std::vector<glm::vec4> colors` – stores color per vertex.
  * `ShapeType` enum { SPHERE\_SHAPE, CYLINDER\_SHAPE, BOX\_SHAPE, CONE\_SHAPE }.
  * `unsigned int level` – tessellation level (0–4).
* **Methods:**

  * Pure virtual `draw()` – overridden in derived classes.
  * Pure virtual constructor – enforces tessellation-level parameter.

### 2. Derived Classes (`sphere_t`, `cylinder_t`, `box_t`, `cone_t`)

* Generate geometry based on tessellation level.
* Fill `vertices` and `colors` containers.
* Override `draw()` for rendering.
* Example: Sphere tessellation increases by subdividing latitude/longitude.

### 3. `model_t` (Hierarchical Model)

* Inspired by **HNode** structure.
* **Each node contains:**

  * A `shape_t` object.
  * Transformation matrices: `glm::mat4 translation`, `rotation`, `scale`.
  * Links to child nodes (tree structure).
* Enables **hierarchical composition**: e.g., robot model where head rotates independently of body.

---

##  Repository Structure

```
graphics-assignment/
│── src/
│   ├── shape.h / shape.cpp
│   ├── sphere.h / sphere.cpp
│   ├── box.h / box.cpp
│   ├── cylinder.h / cylinder.cpp
│   ├── cone.h / cone.cpp
│   ├── model.h / model.cpp
│   ├── main.cpp
│
│── assets/models/
│   ├── toy_robot.mod
│   ├── table_lamp.mod
│
│── Makefile
│── README.md
```

---

##  Build & Run

1. Install dependencies:

   ```bash
   sudo apt-get update
   sudo apt-get install build-essential cmake git
   sudo apt-get install libglfw3-dev libglew-dev libglm-dev
   ```

2. Clone and build:

   ```bash
   git clone <repo_url>
   cd graphics-assignment
   make
   ```

3. Run:

   ```bash
   ./modeller
   ```

---

##  Controls & Keymap

###  Mode Switching

* `M` → Modelling Mode
* `I` → Inspection Mode
* Mode printed to terminal when changed.

### 🛠 Modelling Mode

* **Shape Creation**:

  * `1` → Add Sphere
  * `2` → Add Cylinder
  * `3` → Add Box
  * `4` → Add Cone
  * `5` → Remove last shape

* **Transformations**:

  * `R` → Rotation mode
  * `T` → Translation mode
  * `G` → Scaling mode
  * Then:

    * `X`, `Y`, `Z` → Choose axis
    * `+`, `-` → Apply increment/decrement

* **Colors**:

  * `C` → Input RGB (0–1 floats) via terminal → Updates current shape

* **Saving**:

  * `S` → Save to `.mod` file (asks for filename)

###  Inspection Mode

* **Load Model**: `L` → enter `.mod` filename in terminal
* **Rotate Entire Model**:

  * `R` → rotation mode
  * `X/Y/Z` + `+/-` → rotate around chosen axis
* Camera centers on model centroid automatically.

###  Global

* `Esc` → Exit program (frees memory)

---

##  Conclusion

This project demonstrates:

* Use of **abstract classes** and **inheritance** for shape management.
* **Hierarchical modeling** with transformation matrices.
* **Interactive input handling** for transformations & editing.
* **File I/O with custom format** for model persistence.
* Practical application of **OpenGL graphics pipeline**.

