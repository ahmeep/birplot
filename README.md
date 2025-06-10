# birplot
Simple mathematical function graphing program. Uses OpenGL for rendering and shaders for all mathematical operations.

# Dependencies
- GLFW
- [cglm](https://github.com/recp/cglm)

For downloading **cglm**, look into this [issue](https://github.com/recp/cglm/issues/47) or use [AUR](https://aur.archlinux.org/packages/cglm). Alternatively you can put **cglm** headers into `vendor/` folder.

# Building
This project uses Makefiles. So just run `make` and your executable will be at `build/birplot`.

# Usage
To modify the function go into `assets/shaders/plot.frag` and edit the function `float func(vec2 p)`. The components of `p` are in the interval $\left[ -1, 1 \right]$ birplot supports live reload of shaders, just save the file and the program will detect and reload the shaders.

Also plotting will happen where `float func(vec2 p)` is 0. So your function must be in form of $f(x, y) = 0$. For example
```glsl
float func(vec2 p) {
    return p.x*p.x + p.y*p.y - 1;
}
```
This is going to result as $f(x, y) = x^2 + y^2 - 1 = 0$ which is a circle with radius 1.

# TODO
- [ ] Coordinate axes
- [ ] Keybindings for disabling texts and coordinate axes
- [ ] Support multiple functions to be plotted at the same time
- [ ] Use another method to define mathematical functions instead of modifying the fragment shader

