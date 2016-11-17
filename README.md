# Compiling

Compilation is handled by CMake. I've written a Rakefile to script common CMake actions for convenience.

To compile or re-compile using CMake:

```
$ rake
```

To remove the `build` directory:

```
$ rake clean
```

If you don't have `rake` installed, you can just use `cmake` directly.

```
$ mkdir build
$ cd build
$ cmake ..
$ make
```

If `cmake` is failing because it's missing Freetype (or some other library), you'll have to install it on your machine separate before trying again. Here's a list of external libraries that `cmake` expects to be able to find on the machine. Most machines will have these installed already.

* libpng
* zlib
* Freetype

# Running

There are a few options to specify.

```
-r INTEGER  Resolution in pixels per unit.
-o FILE     Output PNG file name.
```

This program functions by reading a scene description from STDIN. If nothing is found in STDIN, the program will hang until it receives an EOF on STDIN. Bottom line is, make sure you're passing something in through STDIN. The following command will read in the scene description from `myscene.txt`, render it with a resolution of 100 pixels per unit, and save the result to `myimage.png`.

```
$ build/raytracer -o myimage.png -r 100 < myscene.txt
```
