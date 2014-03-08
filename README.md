This project offers a fractal browser.

# Dependencies

- [Qt 5.2.1](http://qt-project.org/downloads/)
  
- [qwt 6.1.0](http://sourceforge.net/projects/qwt/)

# Build process

## Building qwt 6.1.0
   `qmake qwt.pro`   
   `make` or `nmake`  
   `make install` or `nmake install`   
   `qmake -set QMAKEFEATURES ...`   
   
## Building fractal-map
   `qmake`   
   `make` or `nmake`
