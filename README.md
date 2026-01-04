This project offers a fractal browser.

# Requirements

- [Qt 6.9.3](http://qt-project.org/downloads/)

- [qwt 6.3.0](http://sourceforge.net/projects/qwt/)

# Build process

Building depends on the compiler:
For gcc use `make` for Visual Studio use `nmake` for mingw use `mingw32-make`

## Building Qt

```bash
./configure -qt-xcb -opensource -nomake tests
make
make install
```

## Building qwt

```bash
qmake
make
make install
```

## Building fractal-map

```bash
qmake
make
```
