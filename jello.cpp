/*

  USC/Viterbi/Computer Science
  "Jello Cube" Assignment 1 starter code

  Your name:
  Shreya Bhaumik

*/

#include "jello.h"
#include "showCube.h"
#include "input.h"
#include "physics.h"
#include "performanceCounter.h"
#include <iostream>

// camera parameters
double Theta = pi / 6;
double Phi = pi / 6;
double R = 6;

// mouse control
int g_iMenuId;
int g_vMousePos[2];
int g_iLeftMouseButton,g_iMiddleMouseButton,g_iRightMouseButton;

// number of images saved to disk so far
int sprite=0;

// these variables control what is displayed on screen
int shear=0, bend=0, structural=1, pause=0, viewingMode=1, saveScreenToFile=0;
int skyworld = 0;

struct world jello;

int windowWidth, windowHeight;

// For mousedrag force
GLdouble mvMatrix[16], projMatrix[16];
int dragOrigin[2];
point mousedragForce = {0.0,0.0,0.0}, initialPos = {0.0,0.0,0.0}, finalPos = {0.0,0.0,0.0};

// For inclined plane texture
int incPlaneTex = 0;
char* texImagePath = NULL;

// For skybox
unsigned int skybox[6];

// For text
PerformanceCounter pc;  // To get number of frames per sec
GLfloat textAngle = 0;
double timeElapsed;

void myinit()
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(90.0,1.0,0.01,1000.0);

  // set background color to grey
  glClearColor(0.5, 0.5, 0.5, 0.0);

  glCullFace(GL_BACK);
  glEnable(GL_CULL_FACE);

  glShadeModel(GL_SMOOTH);
  glEnable(GL_POLYGON_SMOOTH);
  glEnable(GL_LINE_SMOOTH);

  pc.StartCounter();  // Starting to count first time

  return; 
}

void reshape(int w, int h) 
{
  // Prevent a divide by zero, when h is zero.
  // You can't make a window of zero height.
  if(h == 0)
    h = 1;

  glViewport(0, 0, w, h);

  // Reset the coordinate system before modifying
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  // Set the perspective
  double aspectRatio = 1.0 * w / h;
  gluPerspective(60.0f, aspectRatio, 0.01f, 1000.0f);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity(); 

  windowWidth = w;
  windowHeight = h;

  glutPostRedisplay();
}

void display()
{
  // glClearColor(0.17, 0.22, 0.27, 1.0);  // Background color
  glClearColor(0.18, 0.21, 0.27, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // For mousedrag force
  glGetDoublev(GL_MODELVIEW_MATRIX, mvMatrix);
  glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);

  // camera parameters are Phi, Theta, R
  gluLookAt(R * cos(Phi) * cos (Theta), R * sin(Phi) * cos (Theta), R * sin (Theta),
	        0.0,0.0,0.0, 0.0,0.0,1.0);

  /* Lighting */
  /* You are encouraged to change lighting parameters or make improvements/modifications
     to the lighting model . 
     This way, you will personalize your assignment and your assignment will stick out. 
  */

  // global ambient light
  GLfloat aGa[] = { 0.05, 0.05, 0.05, 0.0 };
  
  // light 's ambient, diffuse, specular
  // GLfloat lKa0[] = { 0.0, 0.0, 0.0, 1.0 };
  // GLfloat lKd0[] = { 1.0, 1.0, 1.0, 1.0 };
  // GLfloat lKs0[] = { 1.0, 1.0, 1.0, 1.0 };

  // GLfloat lKa1[] = { 0.0, 0.0, 0.0, 1.0 };
  // GLfloat lKd1[] = { 1.0, 0.0, 0.0, 1.0 };
  // GLfloat lKs1[] = { 1.0, 0.0, 0.0, 1.0 };

  // GLfloat lKa2[] = { 0.0, 0.0, 0.0, 1.0 };
  // GLfloat lKd2[] = { 1.0, 1.0, 0.0, 1.0 };
  // GLfloat lKs2[] = { 1.0, 1.0, 0.0, 1.0 };

  // GLfloat lKa3[] = { 0.0, 0.0, 0.0, 1.0 };
  // GLfloat lKd3[] = { 0.0, 1.0, 1.0, 1.0 };
  // GLfloat lKs3[] = { 0.0, 1.0, 1.0, 1.0 };

  // GLfloat lKa4[] = { 0.0, 0.0, 0.0, 1.0 };
  // GLfloat lKd4[] = { 0.0, 0.0, 1.0, 1.0 };
  // GLfloat lKs4[] = { 0.0, 0.0, 1.0, 1.0 };

  // GLfloat lKa5[] = { 0.0, 0.0, 0.0, 1.0 };
  // GLfloat lKd5[] = { 1.0, 0.0, 1.0, 1.0 };
  // GLfloat lKs5[] = { 1.0, 0.0, 1.0, 1.0 };

  // GLfloat lKa6[] = { 0.0, 0.0, 0.0, 1.0 };
  // GLfloat lKd6[] = { 1.0, 1.0, 1.0, 1.0 };
  // GLfloat lKs6[] = { 1.0, 1.0, 1.0, 1.0 };

  // GLfloat lKa7[] = { 0.0, 0.0, 0.0, 1.0 };
  // GLfloat lKd7[] = { 0.0, 1.0, 1.0, 1.0 };
  // GLfloat lKs7[] = { 0.0, 1.0, 1.0, 1.0 };

  GLfloat lKa0[] = { 0.0, 0.0, 0.0, 1.0 };
  GLfloat lKd0[] = { 3.0, 1.5, 0.5, 1.0 };
  GLfloat lKs0[] = { 3.0, 1.5, 0.5, 1.0 };

  GLfloat lKa1[] = { 0.0, 0.0, 0.0, 1.0 };
  GLfloat lKd1[] = { 6.0, 3.0, 0.0, 1.0 };
  GLfloat lKs1[] = { 6.0, 3.0, 0.0, 1.0 };

  GLfloat lKa2[] = { 0.0, 0.0, 0.0, 1.0 };
  GLfloat lKd2[] = { 6.5, 3.0, 0.0, 1.0 };
  GLfloat lKs2[] = { 6.5, 3.0, 0.0, 1.0 };

  GLfloat lKa3[] = { 0.0, 0.0, 0.0, 1.0 };
  GLfloat lKd3[] = { 5.0, 3.0, 0.0, 1.0 };
  GLfloat lKs3[] = { 5.0, 3.0, 0.0, 1.0 };

  GLfloat lKa4[] = { 0.0, 0.0, 0.0, 1.0 };
  GLfloat lKd4[] = { 5.0, 3.0, 0.0, 1.0 };
  GLfloat lKs4[] = { 5.0, 3.0, 0.0, 1.0 };

  GLfloat lKa5[] = { 0.0, 0.0, 0.0, 1.0 };
  GLfloat lKd5[] = { 6.5, 3.0, 0.0, 1.0 };
  GLfloat lKs5[] = { 6.5, 3.0, 0.0, 1.0 };

  GLfloat lKa6[] = { 0.0, 0.0, 0.0, 1.0 };
  GLfloat lKd6[] = { 6.0, 3.0, 0.0, 1.0 };
  GLfloat lKs6[] = { 6.0, 3.0, 0.0, 1.0 };

  GLfloat lKa7[] = { 0.0, 0.0, 0.0, 1.0 };
  GLfloat lKd7[] = { 3.0, 1.5, 0.5, 1.0 };
  GLfloat lKs7[] = { 3.0, 1.5, 0.5, 1.0 };

  // light positions and directions
  // GLfloat lP0[] = { -1.999, -1.999, -1.999, 1.0 };
  // GLfloat lP1[] = { 1.999, -1.999, -1.999, 1.0 };
  // GLfloat lP2[] = { 1.999, 1.999, -1.999, 1.0 };
  // GLfloat lP3[] = { -1.999, 1.999, -1.999, 1.0 };
  // GLfloat lP4[] = { -1.999, -1.999, 1.999, 1.0 };
  // GLfloat lP5[] = { 1.999, -1.999, 1.999, 1.0 };
  // GLfloat lP6[] = { 1.999, 1.999, 1.999, 1.0 };
  // GLfloat lP7[] = { -1.999, 1.999, 1.999, 1.0 };
  GLfloat lP0[] = { -5.0, -5.0, -5.0, 1.0 };
  GLfloat lP1[] = { -5.0, -5.0, 5.0, 1.0 };
  GLfloat lP2[] = { -5.0, 5.0, -5.0, 1.0 };
  GLfloat lP3[] = { -5.0, 5.0, 5.0, 1.0 } ;
  GLfloat lP4[] = { 5.0, -5.0, -5.0, 1.0 };
  GLfloat lP5[] = { 5.0, -5.0, 5.0, 1.0 };
  GLfloat lP6[] = { 5.0, 5.0, -5.0, 1.0 };
  GLfloat lP7[] = { 5.0, 5.0, 5.0, 1.0 };
  
  // jelly material color
  // GLfloat mKa[] = { 0.0, 0.0, 0.0, 1.0 };
  // GLfloat mKd[] = { 0.3, 0.3, 0.3, 1.0 };
  // GLfloat mKs[] = { 1.0, 1.0, 1.0, 1.0 };
  // GLfloat mKe[] = { 0.0, 0.0, 0.0, 1.0 };
  GLfloat mKa[] = { 0.0, 0.0, 0.0, 0.6 };
  GLfloat mKd[] = { 0.3, 0.15, 0.0, 0.6 };
  GLfloat mKs[] = { 0.9, 0.45, 0.0, 0.6 };
  GLfloat mKe[] = { 0.0, 0.0, 0.0, 1.0 };

  /* set up lighting */
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, aGa);
  glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
  glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);

  // set up cube color
  glMaterialfv(GL_FRONT, GL_AMBIENT, mKa);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, mKd);
  glMaterialfv(GL_FRONT, GL_SPECULAR, mKs);
  glMaterialfv(GL_FRONT, GL_EMISSION, mKe);
  // glMaterialf(GL_FRONT, GL_SHININESS, 120);
  glMaterialf(GL_FRONT, GL_SHININESS, 90);
    
  // macro to set up light i
  #define LIGHTSETUP(i)\
  glLightfv(GL_LIGHT##i, GL_POSITION, lP##i);\
  glLightfv(GL_LIGHT##i, GL_AMBIENT, lKa##i);\
  glLightfv(GL_LIGHT##i, GL_DIFFUSE, lKd##i);\
  glLightfv(GL_LIGHT##i, GL_SPECULAR, lKs##i);\
  glEnable(GL_LIGHT##i)
  
  LIGHTSETUP (0);
  LIGHTSETUP (1);
  LIGHTSETUP (2);
  LIGHTSETUP (3);
  LIGHTSETUP (4);
  LIGHTSETUP (5);
  LIGHTSETUP (6);
  LIGHTSETUP (7);

  // enable lighting
  glEnable(GL_LIGHTING);    
  glEnable(GL_DEPTH_TEST);

  initSkybox(); // Loading texture for skybox
  showSkybox();

  glDisable(GL_LIGHTING);
  showBoundingBox(); // show the bounding box
  showInclinedPlane(&jello);  // show the inclined plane
  showMyDetails();  // Show text
  glEnable(GL_LIGHTING); 

  // show the cube
  glDisable(GL_CULL_FACE);
  glEnable(GL_BLEND);  // To bring transparency
  glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  //To bring trasparency
  showCube(&jello);
  glDisable(GL_BLEND);  // To bring transparency
  glEnable(GL_CULL_FACE);

  glDisable(GL_LIGHTING);

  // show coordinates
  showCoordinates();
 
  glutSwapBuffers();

  freeMemSkybox();
}

void doIdle()
{
  char s[20]="picxxxx.ppm";
  int i;

  // Steps: Stop the counter at the beginning of idle handler, query its value, and restart it.
  pc.StopCounter();
  timeElapsed = pc.GetElapsedTime();
  std::cout<<(1/timeElapsed)<<'\n';
  pc.StartCounter();
  
  // save screen to file
  s[3] = 48 + (sprite / 1000);
  s[4] = 48 + (sprite % 1000) / 100;
  s[5] = 48 + (sprite % 100 ) / 10;
  s[6] = 48 + sprite % 10;

  if (saveScreenToFile==1)
  {
    saveScreenshot(windowWidth, windowHeight, s);
    //saveScreenToFile=0; // save only once, change this if you want continuos image generation (i.e. animation)
    sprite++;
  }

  if (sprite >= 300) // allow only 300 snapshots
  {
    exit(0);	
  }

  if (pause == 0)
  {
    // insert code which appropriately performs one step of the cube simulation:
    if (strcmp(jello.integrator,"Euler") == 0)
      Euler(&jello);
    else if (strcmp(jello.integrator,"RK4") == 0)
      RK4(&jello);
    else exit(0);
  }

  // Angle for text rotation calculation
  if (textAngle < 180)
    textAngle -= 20*timeElapsed;
  else
    textAngle = 0;

  glutPostRedisplay();
}

// To load pic for texturing
Pic* loadTextureImage(char* texFile)
{
  int nx, ny;
  if(ppm_get_size(texFile, &nx, &ny))
  {
     Pic* image = ppm_read(texFile,pic_alloc(nx,ny,3,NULL));
     return image;
  }
  return NULL;
}

// To load a cubemap for a skybox
unsigned int loadCubemapForSkybox(char* cubemapFile)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    Pic* texImage = loadTextureImage(cubemapFile);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texImage->nx, texImage->ny, 0, GL_RGB, GL_UNSIGNED_BYTE, texImage->pix);
    pic_free(texImage);
    return textureID;
}  

// To initialize skybox
void initSkybox()
{
  if(!skyworld)
  {
    skybox[SKY_POSX] = loadCubemapForSkybox("skyboxes/redskybox/posx.ppm");
    skybox[SKY_NEGX] = loadCubemapForSkybox("skyboxes/redskybox/negx.ppm");
    skybox[SKY_POSY] = loadCubemapForSkybox("skyboxes/redskybox/posy.ppm");
    skybox[SKY_NEGY] = loadCubemapForSkybox("skyboxes/redskybox/negy.ppm");
    skybox[SKY_POSZ] = loadCubemapForSkybox("skyboxes/redskybox/posz.ppm");
    skybox[SKY_NEGZ] = loadCubemapForSkybox("skyboxes/redskybox/negz.ppm");
  }
  else
  {
    skybox[SKY_POSX] = loadCubemapForSkybox("skyboxes/blueskybox/posx.ppm");
    skybox[SKY_NEGX] = loadCubemapForSkybox("skyboxes/blueskybox/negx.ppm");
    skybox[SKY_POSY] = loadCubemapForSkybox("skyboxes/blueskybox/posy.ppm");
    skybox[SKY_NEGY] = loadCubemapForSkybox("skyboxes/blueskybox/negy.ppm");
    skybox[SKY_POSZ] = loadCubemapForSkybox("skyboxes/blueskybox/posz.ppm");
    skybox[SKY_NEGZ] = loadCubemapForSkybox("skyboxes/blueskybox/negz.ppm");
  }
}

void freeMemSkybox()
{
        glDeleteTextures(6,&skybox[0]);
}

int main (int argc, char ** argv)
{
  if (argc<2)
  {  
    printf ("Oops! You didn't say the jello world file!\n");
    printf ("Usage: %s [worldfile]\n", argv[0]);
    exit(0);
  }
  if (argc == 3) // For inclined plane texture file path
  {
    incPlaneTex = 1;
    texImagePath = argv[2];
  }

  readWorld(argv[1],&jello);

  glutInit(&argc,argv);
  
  /* double buffered window, use depth testing, 640x480 */
  glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  
  windowWidth = 640;
  windowHeight = 480;
  glutInitWindowSize (windowWidth, windowHeight);
  glutInitWindowPosition (0,0);
  glutCreateWindow ("Jello cube");
  glutReshapeWindow(windowWidth-1, windowHeight-1);

  /* tells glut to use a particular display function to redraw */
  glutDisplayFunc(display);

  /* replace with any animate code */
  glutIdleFunc(doIdle);

  /* callback for mouse drags */
  glutMotionFunc(mouseMotionDrag);

  /* callback for window size changes */
  glutReshapeFunc(reshape);

  /* callback for mouse movement */
  glutPassiveMotionFunc(mouseMotion);

  /* callback for mouse button changes */
  glutMouseFunc(mouseButton);

  /* register for keyboard events */
  glutKeyboardFunc(keyboardFunc);

  /* do initialization */
  myinit();

  /* forever sink in the black hole */
  glutMainLoop();

  return(0);
}

