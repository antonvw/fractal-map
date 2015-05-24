This project offers a fractal browser.

# Dependencies

- [Qt 5.4.1](http://qt-project.org/downloads/)
  
- [qwt 6.1.2](http://sourceforge.net/projects/qwt/)

# Build process
Building depends on the compiler:  
For gcc use `make` for Visual Studio use `nmake` for mingw use `mingw32-make`   

## Building Qt
   `./configure -qt-xcb -opensource -nomake tests`   
   `make`    
   `make install` 

## Building qwt
   `qmake`   
   `make`   
   `make install`  
   `qmake -set QMAKEFEATURES ...`   
   
## Building fractal-map
   `qmake`   
   `make` 
