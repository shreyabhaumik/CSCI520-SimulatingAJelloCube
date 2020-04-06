#if defined(WIN32)  || defined(_WIN32) 
#include <Windows.h>
// compatibility Profile
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
//Core Profile
// #include <GL/glew.h>
// #include <GL/glut.h>
#elif defined(linux) || defined(__linux__)
// compatibility Profile
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
// Core Profile
// #include <GL/glew.h>
// #include <GL/glut.h>
#elif defined(__APPLE__)
/* Defining GL_SILENCE_DEPRECATION to avoid deprecation messages */
#define GL_SILENCE_DEPRECATION
// compatibility Profile
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
// Core Profile
// #include <OpenGL/gl3.h>
// #include <OpenGL/gl3ext.h>
// #include <GLUT/glut.h>
#endif

