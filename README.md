This project offers a fractal browser.

# Dependencies

- [Qt 4.8.0](http://qt-project.org/downloads/)
  
- [qwt 6.0.1](http://sourceforge.net/projects/qwt/)

# Build process

## Building qwt 6.0.1
   `qmake qwt.pro`   
   `make`   
   `make install`   
   `qmake -set QMAKEFEATURES ...`   
   
## Building fractal-map
   `qmake`   
   `make`
