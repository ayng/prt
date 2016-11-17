# High level overview

`src/main.cpp` is the entry point of the code. It parses command line options, initializes the scene with a scene description, calls the `scene.render()`, and writes the output from that into a PNG using pngwriter.

`Scene::render()` is where the bulk of the calculation happens. You can find the definition of this function in `Scene.cpp`.

`Scene::render()` contains a double for-loop that iterates over every pixel. For each pixel, it shoots a ray (or multiple jittered rays, if anti-aliasing is enabled) into the scene by calling `Scene::trace()`, passing in a `Ray` as the parameter.

`Scene::trace()` checks for intersection with all `Geometry` in the `Scene`. Based on the lighting and geometry, it determines a color and returns it. `trace()` calls itself recursively for reflections. The maximum number of bounces is capped.

# Areas for improvement

* A robust test suite is needed to ensure optimizations do not break functionality.
* So far, the only implemented benchmark measures the time it takes to render. Benchmarks for FLOPS and arithmetic intensity are needed. See how `src/Profiler.cpp` is used for a place to start.
* Algorithmic improvements can be made to reduce the time complexity of ray intersection (e.g. BVH).
* Calls to `Scene::trace()` can be multi-threaded, as they are entirely independent.
* All linear algebra operations occur in `Vector` and `Matrix` classes. These operations are not vectorized or parallelized. These classes can be swapped out for an optimized library, or can be improved by batching calculations.
* The GPU can be utilized to parallelize intersection tests, vector calculations, etc.
