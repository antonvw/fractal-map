This project offers a fractal browser.

# Dependencies

- [Qt 5.2.1](http://qt-project.org/downloads/)
  
- [qwt 6.1.0](http://sourceforge.net/projects/qwt/)

# Build process
    Building depends on the compiler:    
    For gcc use `make` for Visual Studio use `nmake` for mingw use `mingw32-make`   

## Building Qt 5.2.1
    `./configure -qt-xcb -opensource -nomake tests`   
   `make`    
   `make install` 

## Building qwt 6.1.0
   `qmake`   
   `make`   
   `make install`  
   `qmake -set QMAKEFEATURES ...`   
   
## Building fractal-map
   `qmake`   
   `make` 
