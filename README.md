# IN204-Project
Ray tracing with c++

# Usage
For more scenarios than the demo given by `main.cpp`, edit this `construct` function in source code with the following instructions:

Construct a list for all objects to be rendered : `HittableList world`

Define a texture with : `auto texture = std::make_shared<TEXTURE>(...)`

Define a material with : `auto material = std::make_shared<MATERIAL>(texture)`

Define a geometry with : `auto geometry = std::make_shared<GEOMETRY>(Point3d(X,Y,Z), RADIUS, material)`

Add this geometry to world : `world.add(geometry)`

**(Arguments can be slightly differed from method to method, check the header file for accurate usage.)**

Then just build and run the ray tracing program!

Currently supported options for TEXTURE are:
 - CheckerTexture: Squares distributed like checker

Currently supported options for MATERIAL are:
 - Lambertian : Material to simulate objects that cause diffusion reflection
 - Metal : Material to simulate metal objects
 - Dielectric : Material to simulate glass-like objects
 - Diffuse Light : Under construction yet

Currently supported options for GEOMETRY are:
 - Sphere
 - Box

# Build and run

## For Linux Users

Run the following command in your terminal:

```bash
bash run.sh
```

## For Windows Users

Run the following command in your Command Prompt:

```bash
.\run.bat
```

## Configuration Options

Both scripts allow you to change variables for different work modes. Here are the options you can configure:

Change the variables in script for different work mode

- -n_samples : Number of samples per pixel during rendering
- -max_depth : Maximum recursion depth for one ray
- -image_width : Width of image rendered
- -n_threads : Threads used in parallel mode
- -p : parallel mode on
- -a : anti-alias mode on

# Log

`result/log.txt` : you can find the corresponding parameters and elapsed time appended in this file after every run.

# Caracteristics to be implemented

- Texture (mapping from 2d to 3d)
- OOP on color.h
- Geometry like triangle
- Light source
- Motion of objects
- Fix the noisy points
- CUDA support
- Optimisation on design(Matrix operation, Search for object hits)