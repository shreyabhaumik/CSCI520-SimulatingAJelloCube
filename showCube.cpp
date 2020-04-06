/*

  USC/Viterbi/Computer Science
  "Jello Cube" Assignment 1 starter code

  Your name:
  Shreya Bhaumik

*/

#include "jello.h"
#include "showCube.h"
#include <iostream>

int pointMap(int side, int i, int j)
{
  int r;

  switch (side)
  {
  case 1: //[i][j][0] bottom face
    r = 64 * i + 8 * j;
    break;
  case 6: //[i][j][7] top face
    r = 64 * i + 8 * j + 7;
    break;
  case 2: //[i][0][j] front face
    r = 64 * i + j;
    break;
  case 5: //[i][7][j] back face
    r = 64 * i + 56 + j;
    break;
  case 3: //[0][i][j] left face
    r = 8 * i + j;
    break;
  case 4: //[7][i][j] right face
    r = 448 + 8 * i + j;
    break;
  }

  return r;
}

void showCube(struct world * jello)
{
  int i,j,k,ip,jp,kp;
  point r1,r2,r3; // aux variables
  
  /* normals buffer and counter for Gourad shading*/
  struct point normal[8][8];
  int counter[8][8];

  int face;
  double faceFactor, length;

  if (fabs(jello->p[0][0][0].x) > 10)
  {
    printf ("Your cube somehow escaped way out of the box.\n");
    exit(0);
  }

  
  #define NODE(face,i,j) (*((struct point * )(jello->p) + pointMap((face),(i),(j))))

  
  #define PROCESS_NEIGHBOUR(di,dj,dk) \
    ip=i+(di);\
    jp=j+(dj);\
    kp=k+(dk);\
    if\
    (!( (ip>7) || (ip<0) ||\
      (jp>7) || (jp<0) ||\
    (kp>7) || (kp<0) ) && ((i==0) || (i==7) || (j==0) || (j==7) || (k==0) || (k==7))\
       && ((ip==0) || (ip==7) || (jp==0) || (jp==7) || (kp==0) || (kp==7))) \
    {\
      glVertex3f(jello->p[i][j][k].x,jello->p[i][j][k].y,jello->p[i][j][k].z);\
      glVertex3f(jello->p[ip][jp][kp].x,jello->p[ip][jp][kp].y,jello->p[ip][jp][kp].z);\
    }\

 
  if (viewingMode==0) // render wireframe
  {
    glLineWidth(1);
    glPointSize(5);
    glDisable(GL_LIGHTING);
    for (i=0; i<=7; i++)
      for (j=0; j<=7; j++)
        for (k=0; k<=7; k++)
        {
          if (i*j*k*(7-i)*(7-j)*(7-k) != 0) // not surface point
            continue;

          glBegin(GL_POINTS); // draw point
            glColor4f(0,0,0,0);  
            glVertex3f(jello->p[i][j][k].x,jello->p[i][j][k].y,jello->p[i][j][k].z);        
          glEnd();

          //
          //if ((i!=7) || (j!=7) || (k!=7))
          //  continue;

          glBegin(GL_LINES);      
          // structural
          if (structural == 1)
          {
            glColor4f(0,0.6,1,1);
            PROCESS_NEIGHBOUR(1,0,0);
            PROCESS_NEIGHBOUR(0,1,0);
            PROCESS_NEIGHBOUR(0,0,1);
            PROCESS_NEIGHBOUR(-1,0,0);
            PROCESS_NEIGHBOUR(0,-1,0);
            PROCESS_NEIGHBOUR(0,0,-1);
          }
          
          // shear
          if (shear == 1)
          {
            glColor4f(0,1,0,1);
            PROCESS_NEIGHBOUR(1,1,0);
            PROCESS_NEIGHBOUR(-1,1,0);
            PROCESS_NEIGHBOUR(-1,-1,0);
            PROCESS_NEIGHBOUR(1,-1,0);
            PROCESS_NEIGHBOUR(0,1,1);
            PROCESS_NEIGHBOUR(0,-1,1);
            PROCESS_NEIGHBOUR(0,-1,-1);
            PROCESS_NEIGHBOUR(0,1,-1);
            PROCESS_NEIGHBOUR(1,0,1);
            PROCESS_NEIGHBOUR(-1,0,1);
            PROCESS_NEIGHBOUR(-1,0,-1);
            PROCESS_NEIGHBOUR(1,0,-1);

            PROCESS_NEIGHBOUR(1,1,1)
            PROCESS_NEIGHBOUR(-1,1,1)
            PROCESS_NEIGHBOUR(-1,-1,1)
            PROCESS_NEIGHBOUR(1,-1,1)
            PROCESS_NEIGHBOUR(1,1,-1)
            PROCESS_NEIGHBOUR(-1,1,-1)
            PROCESS_NEIGHBOUR(-1,-1,-1)
            PROCESS_NEIGHBOUR(1,-1,-1)
          }
          
          // bend
          if (bend == 1)
          {
            glColor4f(1,0,0,1);
            PROCESS_NEIGHBOUR(2,0,0);
            PROCESS_NEIGHBOUR(0,2,0);
            PROCESS_NEIGHBOUR(0,0,2);
            PROCESS_NEIGHBOUR(-2,0,0);
            PROCESS_NEIGHBOUR(0,-2,0);
            PROCESS_NEIGHBOUR(0,0,-2);
          }           
          glEnd();
        }
    glEnable(GL_LIGHTING);
  }
  
  else
  {
    glPolygonMode(GL_FRONT, GL_FILL); 
    
    for (face=1; face <= 6; face++) 
      // face == face of a cube
      // 1 = bottom, 2 = front, 3 = left, 4 = right, 5 = far, 6 = top
    {
      
      if ((face==1) || (face==3) || (face==5))
        faceFactor=-1; // flip orientation
      else
        faceFactor=1;
      

      for (i=0; i <= 7; i++) // reset buffers
        for (j=0; j <= 7; j++)
        {
          normal[i][j].x=0;normal[i][j].y=0;normal[i][j].z=0;
          counter[i][j]=0;
        }

      /* process triangles, accumulate normals for Gourad shading */
  
      for (i=0; i <= 6; i++)
        for (j=0; j <= 6; j++) // process block (i,j)
        {
          pDIFFERENCE(NODE(face,i+1,j),NODE(face,i,j),r1); // first triangle
          pDIFFERENCE(NODE(face,i,j+1),NODE(face,i,j),r2);
          CROSSPRODUCTp(r1,r2,r3); pMULTIPLY(r3,faceFactor,r3);
          pNORMALIZE(r3);
          pSUM(normal[i+1][j],r3,normal[i+1][j]);
          counter[i+1][j]++;
          pSUM(normal[i][j+1],r3,normal[i][j+1]);
          counter[i][j+1]++;
          pSUM(normal[i][j],r3,normal[i][j]);
          counter[i][j]++;

          pDIFFERENCE(NODE(face,i,j+1),NODE(face,i+1,j+1),r1); // second triangle
          pDIFFERENCE(NODE(face,i+1,j),NODE(face,i+1,j+1),r2);
          CROSSPRODUCTp(r1,r2,r3); pMULTIPLY(r3,faceFactor,r3);
          pNORMALIZE(r3);
          pSUM(normal[i+1][j],r3,normal[i+1][j]);
          counter[i+1][j]++;
          pSUM(normal[i][j+1],r3,normal[i][j+1]);
          counter[i][j+1]++;
          pSUM(normal[i+1][j+1],r3,normal[i+1][j+1]);
          counter[i+1][j+1]++;
        }

      
        /* the actual rendering */
        for (j=1; j<=7; j++) 
        {

          if (faceFactor > 0)
            glFrontFace(GL_CCW); // the usual definition of front face
          else
            glFrontFace(GL_CW); // flip definition of orientation
         
          glBegin(GL_TRIANGLE_STRIP);
          for (i=0; i<=7; i++)
          {
            glNormal3f(normal[i][j].x / counter[i][j],normal[i][j].y / counter[i][j],
              normal[i][j].z / counter[i][j]);
            glVertex3f(NODE(face,i,j).x, NODE(face,i,j).y, NODE(face,i,j).z);
            glNormal3f(normal[i][j-1].x / counter[i][j-1],normal[i][j-1].y/ counter[i][j-1],
              normal[i][j-1].z / counter[i][j-1]);
            glVertex3f(NODE(face,i,j-1).x, NODE(face,i,j-1).y, NODE(face,i,j-1).z);
          }
          glEnd();
        }
        
        
    }  
  } // end for loop over faces
  glFrontFace(GL_CCW);
}

// To show inclined plane
// by computing intersection between a plane and an AABB (axis-aligned bounding box)
// Ref: https://www.asawicki.info/news_1428_finding_polygon_of_plane-aabb_intersection
// The polygon formed by the plane in the cube can have 3 to 6 vertices
point origin, planeNormal;
void rayToPlane(const point min, const point max, const double a, const double b, const double c, const double d, int &count, point* const &vertices)
{
  // Computing the intersection points
  point RayOrig, RayDir, intersectionPt;
  pMAKE(min.x,min.y,min.z,RayOrig);
  pMAKE(max.x,max.y,max.z,RayDir);
  pDIFFERENCE(RayDir,RayOrig,RayDir);
  bool hit;
  double OutVD,T;
  OutVD = a * RayDir.x + b * RayDir.y + c * RayDir.z;
  if(OutVD == 0.0f) // OutVD > 0 means that ray is back-facing the plane
  {
    hit=false;
  }
  else
  {
    T = -(a * RayOrig.x + b * RayOrig.y + c * RayOrig.z + d) / OutVD;
    hit=true;
  }
  if(hit && (T >= 0.f) && (T <= 1.f))
  {
    pMAKE((RayOrig.x + RayDir.x * T),(RayOrig.y + RayDir.y * T),(RayOrig.z + RayDir.z * T),intersectionPt);
    pCPY(intersectionPt,vertices[count]);
    count++;
  }
}

// To compare vertices while sorting them
int compareVertices(const void *lhs, const void *rhs)
{
  point v,LHS,RHS;
  double res;
  pDIFFERENCE(*(point*)lhs,origin,LHS);
  pDIFFERENCE(*(point*)rhs,origin,RHS);
  CROSSPRODUCTp(LHS,RHS,v);
  pDOT(v,planeNormal,res);
  return (res < 0);
}

// For texture vertices
struct tex 
{
   double x;
   double y;
};
tex origin1;
// To compare vertices while sorting them
int compareTexVertices(const void *lhs, const void *rhs)
{
  point v,LHS,RHS,normal1;
  pMAKE(0.0,0.0,1.0,normal1);
  double res;
  LHS.x = (*(tex*)lhs).x - origin1.x;
  LHS.y = (*(tex*)lhs).y - origin1.y;
  LHS.z = 1.0;
  RHS.x = (*(tex*)rhs).x - origin1.x;
  RHS.y = (*(tex*)rhs).y - origin1.y;
  RHS.z = 1.0;
  CROSSPRODUCTp(LHS,RHS,v);
  pDOT(v,normal1,res);
  return (res < 0);
}

void showInclinedPlane(struct world *jello)
{
  point vertices[9], min, max;
  int count = 0;
  if(jello->incPlanePresent)
  {
    // To test edges along X-axis, pointing right
    pMAKE(-2,-2,-2,min); pMAKE(2,-2,-2,max);
    rayToPlane(min,max,jello->a,jello->b,jello->c,jello->d,count,vertices);
    pMAKE(-2,-2,2,min); pMAKE(2,-2,2,max);
    rayToPlane(min,max,jello->a,jello->b,jello->c,jello->d,count,vertices);
    pMAKE(-2,2,-2,min); pMAKE(2,2,-2,max);
    rayToPlane(min,max,jello->a,jello->b,jello->c,jello->d,count,vertices);
    pMAKE(-2,2,2,min); pMAKE(2,2,2,max);
    rayToPlane(min,max,jello->a,jello->b,jello->c,jello->d,count,vertices);
    // To test edges along Y-axis, pointing up
    pMAKE(-2,-2,-2,min); pMAKE(-2,2,-2,max);
    rayToPlane(min,max,jello->a,jello->b,jello->c,jello->d,count,vertices);
    pMAKE(-2,-2,2,min); pMAKE(-2,2,2,max);
    rayToPlane(min,max,jello->a,jello->b,jello->c,jello->d,count,vertices);
    pMAKE(2,-2,-2,min); pMAKE(2,2,-2,max);
    rayToPlane(min,max,jello->a,jello->b,jello->c,jello->d,count,vertices);
    pMAKE(2,-2,2,min); pMAKE(2,2,2,max);
    rayToPlane(min,max,jello->a,jello->b,jello->c,jello->d,count,vertices);
    // To test edges along Z-axis, pointing forward
    pMAKE(-2,-2,-2,min); pMAKE(-2,-2,2,max);
    rayToPlane(min,max,jello->a,jello->b,jello->c,jello->d,count,vertices);
    pMAKE(-2,2,-2,min); pMAKE(-2,2,2,max);
    rayToPlane(min,max,jello->a,jello->b,jello->c,jello->d,count,vertices);
    pMAKE(2,-2,-2,min); pMAKE(2,-2,2,max);
    rayToPlane(min,max,jello->a,jello->b,jello->c,jello->d,count,vertices);
    pMAKE(2,2,-2,min); pMAKE(2,2,2,max);
    rayToPlane(min,max,jello->a,jello->b,jello->c,jello->d,count,vertices);

    if(!count)
      return;

    pMAKE(jello->a,jello->b,jello->c,planeNormal);
    pCPY(vertices[0],origin);
    qsort(vertices,count,sizeof(point),compareVertices);  // Now we have all the vertices sorted

    if(incPlaneTex) // If texture file is provided
    {
      // Add texture to inclined plane
      GLuint myTextureID;
      Pic* texImage = loadTextureImage(texImagePath);
      glGenTextures( 1, &myTextureID ); // allocate a texture name
      glBindTexture( GL_TEXTURE_2D, myTextureID );  // select our current texture
      glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST); // for small texture are, bilinear filtering and finding the closest mipmap
      glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR ); // for large texture area, bilinear filtering the original image
      glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT ); // wraping texture over the edges - repeat
      glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
      gluBuild2DMipmaps( GL_TEXTURE_2D, 3, texImage->nx, texImage->ny, GL_RGB, GL_UNSIGNED_BYTE, texImage->pix ); // texture mipmaps
      
      glColor4f(1.0, 1.0, 1.0, 0.1);
      glDisable(GL_CULL_FACE);
      glEnable(GL_TEXTURE_2D);
      glBegin(GL_POLYGON);

      // To find texture coordinates
      point texVertices[9], screenNormal,axisOfRotation;
      tex texVertices1[9];
      pMAKE(0.0,0.0,1.0,screenNormal);
      for (int i = 0; i < count; i++)
      {
        texVertices[i].x = vertices[i].x - jello->d;
        texVertices[i].y = vertices[i].y - jello->d;
      }
      double dotprod,plen,costheta,sintheta;
      pDOT(planeNormal,screenNormal,dotprod);
      pLENGTH(planeNormal,plen);
      if(plen)
      {
        costheta = dotprod/plen;
        sintheta = sqrt(1-costheta*costheta);
      }
      CROSSPRODUCTp(planeNormal,screenNormal,axisOfRotation);
      // normalize(axisOfRotation);
      double length;
      pLENGTH(axisOfRotation,length);
      axisOfRotation.x /= length;
      axisOfRotation.y /= length;
      axisOfRotation.z /= length;
      double xsqr = axisOfRotation.x * axisOfRotation.x;
      double ysqr = axisOfRotation.y * axisOfRotation.y;
      for (int i = 0; i < count; i++)
      {
        // 3d rotation about a 3d axis
        texVertices1[i].x = (xsqr + (1-xsqr)*costheta) * texVertices[i].x + 
                           (axisOfRotation.x * axisOfRotation.y * (1-costheta) - axisOfRotation.z*sintheta) * texVertices[i].y +
                           (axisOfRotation.x * axisOfRotation.z * (1-costheta) + axisOfRotation.y*sintheta) * texVertices[i].z;
        texVertices1[i].y = (axisOfRotation.x * axisOfRotation.y * (1-costheta) + axisOfRotation.z*sintheta) * texVertices[i].x +
                           (ysqr + (1-ysqr)*costheta) * texVertices[i].y +
                           (axisOfRotation.x * axisOfRotation.z * (1-costheta) - axisOfRotation.x*sintheta) * texVertices[i].z;
        // std::cout<<texVertices[i].x<<' '<<texVertices[i].y<<'\n';
      }
      origin1.x = texVertices1[0].x;
      origin1.y = texVertices1[0].y;
      qsort(texVertices1,count,sizeof(tex),compareTexVertices);  // Now we have all the texture vertices sorted
      // for (int i = 0; i < count; i++)
      // {
      //   std::cout<<texVertices1[i].x<<' '<<texVertices1[i].y<<'\n';
      // }

      for (int i = 0; i < count; i++)
      {
        glTexCoord2f(texVertices1[i].x,texVertices1[i].y);
        glVertex3f(vertices[i].x, vertices[i].y, vertices[i].z);
        // std::cout<<vertices[i].x<<' '<<vertices[i].y<<' '<<vertices[i].z<<'\n';
      }
      glEnd();
      glDisable(GL_TEXTURE_2D);
      glEnable(GL_CULL_FACE);
      pic_free(texImage);
    }
    else
    {
      glColor4f(0.15, 0.35, 0.3, 0);
      glDisable(GL_CULL_FACE);
      glBegin(GL_POLYGON);
      for (int i = 0; i < count; i++)
        glVertex3f(vertices[i].x, vertices[i].y, vertices[i].z);
      glEnd();
      glEnable(GL_CULL_FACE);
    }
  }
}

// To show coordinates
void showCoordinates()
{
  // if(coordinates)
  // {
  // X-axis
  glColor3f(0.2,0.4,1.0); // blue
  glBegin(GL_LINES);
  glVertex3f(-0.5, 0.0, 0.0);
  glVertex3f(0.5, 0.0, 0.0);
  // arrow head
  glVertex3f(0.5, 0.0, 0.0);
  glVertex3f(0.4, 0.1, 0.0);
  glVertex3f(0.5, 0.0, 0.0);
  glVertex3f(0.4, -0.1, 0.0);
  glEnd();

  // Y-axis
  glColor3f(1.0,0.0,0.0); // red
  glBegin(GL_LINES);
  glVertex3f(0.0, -0.5, 0.0);
  glVertex3f(0.0, 0.5, 0.0);
  // arrow head
  glVertex3f(0.0, 0.5, 0.0);
  glVertex3f(0.1, 0.4, 0.0);
  glVertex3f(0.0, 0.5, 0.0);
  glVertex3f(-0.1, 0.4, 0.0);
  glEnd();

  // Z-axis
  glColor3f(0.0,1.0,0.0); // green
  glBegin(GL_LINES);
  glVertex3f(0.0, 0.0, -0.5);
  glVertex3f(0.0, 0.0, 0.5);
  // arrow head
  glVertex3f(0.0, 0.0, 0.5);
  glVertex3f(0.0, 0.1, 0.4);
  glVertex3f(0.0, 0.0, 0.5);
  glVertex3f(0.0, -0.1, 0.4);
  glEnd();

  GLfloat a,b,c;
  point todrawF;
  glColor3f(1.0,1.0,1.0); // white
  glBegin(GL_LINES);
  a = 0.0; b = 0.0; c = 0.0;
  glVertex3f(a,b,c);
  pCPY(mousedragForce,todrawF);
  // normalize(todrawF);
  double length;
  pLENGTH(todrawF,length);
  todrawF.x /= length;
  todrawF.y /= length;
  todrawF.z /= length;
  std::cout<<todrawF.x<<'\n';
  a = todrawF.x*100; b = todrawF.y*100; c = todrawF.z*100;
  glVertex3f(a,b,c);
  glEnd();
  // }
}

// To print my details as a text rotating in space
void showMyDetails()
{
  int len=0,i=0,j;
  char lines[3][40];

  glPushMatrix();
  if(!skyworld)  // If skybox is red
    glColor4f(0.6,0.75,1.0,0);
  else  // If skybox is blue
    glColor4f(1.0,0.4,0.5,0);
  glTranslated(-50.0, 0.0, -10.0);
  glRotated(90, 0.0, 1.0, 0.0);
  glRotated(90, 0.0, 0.0, 1.0);
  glScalef(0.02, 0.02, 0.02);
  glRotatef(textAngle,0.0,1.0,0.0);

  strcpy(lines[0],"Computer Animation & Simulation");
  strcpy(lines[1],"Jello Cube");
  strcpy(lines[2],"Shreya Bhaumik");

  while(i<3)
  {
    glPushMatrix();
    len = (int)strlen(lines[i]);
    glTranslated(-(len * 35), (i*120), 0.0);
    for (j=0; j < len; j++)
      glutStrokeCharacter(GLUT_STROKE_ROMAN, lines[i][j]);
    i++;
    glPopMatrix();
  }
  glPopMatrix();
  glFlush();
}

// To show skybox
void showSkybox()
{
  int s = 20;
  glColor4f(1.0, 1.0, 1.0, 0.1);
  bool isOn = glIsEnabled(GL_TEXTURE_2D); // To leave the textures on even if it was already on
  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);
  //bottom face
  glBindTexture(GL_TEXTURE_2D,skybox[SKY_NEGZ]);
  glBegin(GL_QUADS); 
  glTexCoord2f(1,0);
  glVertex3f(-s/2,s/2,-s/2);
  glTexCoord2f(0,0);
  glVertex3f(-s/2,-s/2,-s/2);
  glTexCoord2f(0,1);
  glVertex3f(s/2,-s/2,-s/2);
  glTexCoord2f(1,1);
  glVertex3f(s/2,s/2,-s/2);
  glEnd();
  // back face
  glBindTexture(GL_TEXTURE_2D,skybox[SKY_NEGX]);
  glBegin(GL_QUADS);     
  glTexCoord2f(1,0);
  glVertex3f(-s/2,s/2,s/2);
  glTexCoord2f(0,0);
  glVertex3f(-s/2,-s/2,s/2);
  glTexCoord2f(0,1);
  glVertex3f(-s/2,-s/2,-s/2);
  glTexCoord2f(1,1);
  glVertex3f(-s/2,s/2,-s/2);
  glEnd();
  //top face
  glBindTexture(GL_TEXTURE_2D,skybox[SKY_POSZ]);
  glBegin(GL_QUADS);     
  glTexCoord2f(1,0);
  glVertex3f(s/2,s/2,s/2);
  glTexCoord2f(0,0);
  glVertex3f(s/2,-s/2,s/2);
  glTexCoord2f(0,1);
  glVertex3f(-s/2,-s/2,s/2);
  glTexCoord2f(1,1);
  glVertex3f(-s/2,s/2,s/2);
  glEnd();
  //front face
  glBindTexture(GL_TEXTURE_2D,skybox[SKY_POSX]);
  glBegin(GL_QUADS);     
  glTexCoord2f(1,0);
  glVertex3f(s/2,-s/2,s/2);
  glTexCoord2f(0,0);
  glVertex3f(s/2,s/2,s/2);
  glTexCoord2f(0,1);
  glVertex3f(s/2,s/2,-s/2);
  glTexCoord2f(1,1);
  glVertex3f(s/2,-s/2,-s/2);
  glEnd();
  // right face
  glBindTexture(GL_TEXTURE_2D,skybox[SKY_POSY]);     
  glBegin(GL_QUADS);
  glTexCoord2f(1,0);
  glVertex3f(s/2,s/2,s/2);
  glTexCoord2f(0,0);
  glVertex3f(-s/2,s/2,s/2);
  glTexCoord2f(0,1);
  glVertex3f(-s/2,s/2,-s/2);
  glTexCoord2f(1,1);
  glVertex3f(s/2,s/2,-s/2);
  glEnd();
  //left face
  glBindTexture(GL_TEXTURE_2D,skybox[SKY_NEGY]);              
  glBegin(GL_QUADS);     
  glTexCoord2f(1,0);
  glVertex3f(-s/2,-s/2,s/2);
  glTexCoord2f(0,0);
  glVertex3f(s/2,-s/2,s/2);
  glTexCoord2f(0,1);
  glVertex3f(s/2,-s/2,-s/2);
  glTexCoord2f(1,1);
  glVertex3f(-s/2,-s/2,-s/2);
  glEnd();
  glEnable(GL_LIGHTING);
  glEnable(GL_DEPTH_TEST);
  if(!isOn)
    glDisable(GL_TEXTURE_2D);
}

void showBoundingBox()
{
  int i,j;

  if(!skyworld)  // If skybox is red
    glColor4f(0.6,0.75,1.0,0);
  else  // If skybox is blue
    glColor4f(1.0,0.4,0.5,0);

  glBegin(GL_LINES);

  // front face
  for(i=-2; i<=2; i++)
  {
    glVertex3f(i,-2,-2);
    glVertex3f(i,-2,2);
  }
  for(j=-2; j<=2; j++)
  {
    glVertex3f(-2,-2,j);
    glVertex3f(2,-2,j);
  }

  // back face
  for(i=-2; i<=2; i++)
  {
    glVertex3f(i,2,-2);
    glVertex3f(i,2,2);
  }
  for(j=-2; j<=2; j++)
  {
    glVertex3f(-2,2,j);
    glVertex3f(2,2,j);
  }

  // left face
  for(i=-2; i<=2; i++)
  {
    glVertex3f(-2,i,-2);
    glVertex3f(-2,i,2);
  }
  for(j=-2; j<=2; j++)
  {
    glVertex3f(-2,-2,j);
    glVertex3f(-2,2,j);
  }

  // right face
  for(i=-2; i<=2; i++)
  {
    glVertex3f(2,i,-2);
    glVertex3f(2,i,2);
  }
  for(j=-2; j<=2; j++)
  {
    glVertex3f(2,-2,j);
    glVertex3f(2,2,j);
  }
  
  glEnd();

  return;
}

