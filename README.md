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
-r INTEGER   Resolution in pixels per unit. 
             (e.g. if the projection plane of the camera is 2.5 by 3.0 in world
             space and the resolution is set to 100, then the output image will
             be 250 by 300 pixels.) (Default: 40)
-o FILE      Output PNG file name. Make sure the file extension is '.png'.
-aa INTEGER  Side length of jittered grid of rays per pixel.
             (e.g. if set to 3, the total number of rays per pixel will be 9)
             (Default: 1 // no anti-aliasing)
```

This program functions by reading a scene description from STDIN. If nothing is found in STDIN, the program will hang until it receives an EOF on STDIN. Bottom line is, make sure you're passing something in through STDIN. The following command will read in the scene description from `myscene.txt`, render it with a resolution of 100 pixels per unit, and save the result to `myimage.png`.

```
$ build/raytracer -o myimage.png -r 100 < myscene.txt
```

# Testing

To run tests, switch to the prt directory and run `rake test`. By default, the output of the raytracer program wil be suppressed.
If you wish for the full output to be shown, run `rake test[-v]`.
For each test, the script will output a PASS or a FAIL, as well as the magnitude of the difference if there is a failure.
The tests currently do not cover anti aliasing, as it injects a bit of randomness into the generation of output.
