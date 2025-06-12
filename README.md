# birplot
Simple mathematical function graphing program. Uses OpenGL for rendering and shaders for all mathematical operations.

# Dependencies
- GLFW
- [cglm](https://github.com/recp/cglm)

For downloading **cglm**, look into this [issue](https://github.com/recp/cglm/issues/47) or use [AUR](https://aur.archlinux.org/packages/cglm). Alternatively you can put **cglm** headers into `vendor/` folder.

# Building
This project uses Makefiles. So just run `make` and your executable will be at `build/birplot`.

# Usage
You can change what is being plotted by modifying `birplot.glsl`. The inputs x and y are between -1 and 1. Time variable contains how many seconds has been passed since the program started. The defined function will be plotted where it's value becomes 0. For example if you are trying to plot $y = sin(x)$ the appropriate code will be
```glsl
float userFunction(vec2 point) {
    return point.y - sin(point.x);
}
```

> [!NOTE]
> Right now some discontinuous functions have some distortion while plotting.

## Keybindings
- `F`: Toggle FPS indicator
- `C`: Toggle coordinate system

# TODO
- [x] Coordinate axes
- [x] Keybindings for disabling texts and coordinate axes
- [x] Support multiple functions to be plotted at the same time
- [ ] Use another method to define mathematical functions instead of modifying the fragment shader
- [ ] Fix distortions when plotting a discontinuous function

