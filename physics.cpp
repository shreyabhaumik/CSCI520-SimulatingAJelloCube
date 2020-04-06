/*

  USC/Viterbi/Computer Science
  "Jello Cube" Assignment 1 starter code

  Your name:
  Shreya Bhaumik
  
*/

#include "jello.h"
#include "physics.h"

// Rest Lengths of different springs
const double rLStruc = 1.0/7.0;
const double rLShearFace = sqrt(2.0) * rLStruc;
const double rLShearMainDiag = sqrt(3.0) * rLStruc;
const double rLBend = 2 * rLStruc;

// To normalize a vector
void normalize(point &v)
{
  double length;
  pLENGTH(v,length);
  v.x /= length;
  v.y /= length;
  v.z /= length;
}

// Computes elastic force on A because of spring between A and B
void computeHooksForce(struct point A, struct point B, double kH, double rL, struct point &F)
{
  // F = (-kH * (|L| - rL)) * (L/|L|)
  pMAKE(0.0,0.0,0.0,F); // initializing F
  point L, normL;
  double lenL, scalarF;
  pDIFFERENCE(A,B,L); // L be the vector pointing from B to A i.e. A-B
  pCPY(L,normL);
  normalize(normL);  // (L/|L|) i.e. unit vector along L
  pLENGTH(L,lenL);  // |L| i.e. length of L
  scalarF = (-kH) * (lenL - rL);  // (-kH * (|L| - rL)) i.e. magnitude of elastic force on A
  pMULTIPLY(normL,scalarF,F); // the final directional elastic force on A
}

// Computes damping force on A for A and B mass points connected with a spring
void computeDampingForce(struct point A, struct point B, struct point vA, struct point vB, double kD, struct point &F)
{
  // F = (-kD * ((vA-vB) dot L) / |L|) * (L/|L|)
  pMAKE(0.0,0.0,0.0,F); // initializing F
  point L, relativeV, normL;
  double lenL, scalarF;
  pDIFFERENCE(A,B,L); // L be the vector pointing from B to A i.e. A-B
  pDIFFERENCE(vA,vB,relativeV); // relativeV be the relative velocity of masses A and B
  pCPY(L,normL);
  normalize(normL);  // (L/|L|) i.e. unit vector along L
  pLENGTH(L,lenL);  // |L| i.e. length of L
  pDOT(relativeV,L,scalarF);
  scalarF *= (-kD) / lenL; // (-kD * ((vA-vB) dot L) / |L|) i.e. magnitude of damping force on A
  pMULTIPLY(normL,scalarF,F); // the final directional damping force on A
}

// Computes structural spring forces on mass point A from structural springs connected with it's neighbor mass points
void computeStrucForce(struct world * jello, int i, int j, int k, struct point &F)
{
  point tempF;
  if(i-1>=0)  // If A has left neighbor, force from spring between A and that neighbor
  {
    computeHooksForce(jello->p[i][j][k], jello->p[i-1][j][k], jello->kElastic, rLStruc, tempF);
    pSUM(F,tempF,F);
    computeDampingForce(jello->p[i][j][k], jello->p[i-1][j][k],jello->v[i][j][k], jello->v[i-1][j][k], jello->dElastic, tempF);
    pSUM(F,tempF,F);
  }
  if(i+1<=7)  // If A has right neighbor, force from spring between A and that neighbor
  {
    computeHooksForce(jello->p[i][j][k], jello->p[i+1][j][k], jello->kElastic, rLStruc, tempF);
    pSUM(F,tempF,F);
    computeDampingForce(jello->p[i][j][k], jello->p[i+1][j][k],jello->v[i][j][k], jello->v[i+1][j][k], jello->dElastic, tempF);
    pSUM(F,tempF,F);
  }
  if(j-1>=0)  // If A has bottom neighbor, force from spring between A and that neighbor
  {
    computeHooksForce(jello->p[i][j][k], jello->p[i][j-1][k], jello->kElastic, rLStruc, tempF);
    pSUM(F,tempF,F);
    computeDampingForce(jello->p[i][j][k], jello->p[i][j-1][k],jello->v[i][j][k], jello->v[i][j-1][k], jello->dElastic, tempF);
    pSUM(F,tempF,F);
  }
  if(j+1<=7)  // If A has top neighbor, force from spring between A and that neighbor
  {
    computeHooksForce(jello->p[i][j][k], jello->p[i][j+1][k], jello->kElastic, rLStruc, tempF);
    pSUM(F,tempF,F);
    computeDampingForce(jello->p[i][j][k], jello->p[i][j+1][k],jello->v[i][j][k], jello->v[i][j+1][k], jello->dElastic, tempF);
    pSUM(F,tempF,F);
  }
  if(k-1>=0)  // If A has back neighbor, force from spring between A and that neighbor
  {
    computeHooksForce(jello->p[i][j][k], jello->p[i][j][k-1], jello->kElastic, rLStruc, tempF);
    pSUM(F,tempF,F);
    computeDampingForce(jello->p[i][j][k], jello->p[i][j][k-1],jello->v[i][j][k], jello->v[i][j][k-1], jello->dElastic, tempF);
    pSUM(F,tempF,F);
  }
  if(k+1<=7)  // If A has front neighbor, force from spring between A and that neighbor
  {
    computeHooksForce(jello->p[i][j][k], jello->p[i][j][k+1], jello->kElastic, rLStruc, tempF);
    pSUM(F,tempF,F);
    computeDampingForce(jello->p[i][j][k], jello->p[i][j][k+1],jello->v[i][j][k], jello->v[i][j][k+1], jello->dElastic, tempF);
    pSUM(F,tempF,F);
  }
}

// Computes shear spring forces on mass point A from shear springs connected with it's neighbor mass points
void computeShearForce(struct world * jello, int i, int j, int k, struct point &F)
{
  point tempF;
  /* Every mass point can have atmost 20 shear springs. Imagine the 8 voxels that can have the mass point as a corner.
  There can be 4 face diagonals along 2D planes on each of the axes - 4*3 = 12 face diagonal springs.
  There can be 8 3D cube main diagonals for each of the 8 voxels the mass point belongs to.
  So, there will be 20 cases to check to compute shear force on a mass point. */
  //For 4 face diagonals along vertical plane i
  if((j-1>=0) && (k-1>=0)) // If A has bottom, back face diagonal neighbor
  {
    computeHooksForce(jello->p[i][j][k], jello->p[i][j-1][k-1], jello->kElastic, rLShearFace, tempF);
    pSUM(F,tempF,F);
    computeDampingForce(jello->p[i][j][k], jello->p[i][j-1][k-1],jello->v[i][j][k], jello->v[i][j-1][k-1], jello->dElastic, tempF);
    pSUM(F,tempF,F);
  }
  if((j-1>=0) && (k+1<=7)) // If A has bottom, front face diagonal neighbor
  {
    computeHooksForce(jello->p[i][j][k], jello->p[i][j-1][k+1], jello->kElastic, rLShearFace, tempF);
    pSUM(F,tempF,F);
    computeDampingForce(jello->p[i][j][k], jello->p[i][j-1][k+1],jello->v[i][j][k], jello->v[i][j-1][k+1], jello->dElastic, tempF);
    pSUM(F,tempF,F);
  }
  if((j+1<=7) && (k-1>=0)) // If A has top, back face diagonal neighbor
  {
    computeHooksForce(jello->p[i][j][k], jello->p[i][j+1][k-1], jello->kElastic, rLShearFace, tempF);
    pSUM(F,tempF,F);
    computeDampingForce(jello->p[i][j][k], jello->p[i][j+1][k-1],jello->v[i][j][k], jello->v[i][j+1][k-1], jello->dElastic, tempF);
    pSUM(F,tempF,F);
  }
  if((j+1<=7) && (k+1<=7)) // If A has top, front face diagonal neighbor
  {
    computeHooksForce(jello->p[i][j][k], jello->p[i][j+1][k+1], jello->kElastic, rLShearFace, tempF);
    pSUM(F,tempF,F);
    computeDampingForce(jello->p[i][j][k], jello->p[i][j+1][k+1],jello->v[i][j][k], jello->v[i][j+1][k+1], jello->dElastic, tempF);
    pSUM(F,tempF,F);
  }
  //For 4 face diagonals along horizontal plane j
  if((i-1>=0) && (k-1>=0)) // If A has left, back face diagonal neighbor
  {
    computeHooksForce(jello->p[i][j][k], jello->p[i-1][j][k-1], jello->kElastic, rLShearFace, tempF);
    pSUM(F,tempF,F);
    computeDampingForce(jello->p[i][j][k], jello->p[i-1][j][k-1],jello->v[i][j][k], jello->v[i-1][j][k-1], jello->dElastic, tempF);
    pSUM(F,tempF,F);
  }
  if((i-1>=0) && (k+1<=7)) // If A has left, front face diagonal neighbor
  {
    computeHooksForce(jello->p[i][j][k], jello->p[i-1][j][k+1], jello->kElastic, rLShearFace, tempF);
    pSUM(F,tempF,F);
    computeDampingForce(jello->p[i][j][k], jello->p[i-1][j][k+1],jello->v[i][j][k], jello->v[i-1][j][k+1], jello->dElastic, tempF);
    pSUM(F,tempF,F);
  }
  if((i+1<=7) && (k-1>=0)) // If A has right, back face diagonal neighbor
  {
    computeHooksForce(jello->p[i][j][k], jello->p[i+1][j][k-1], jello->kElastic, rLShearFace, tempF);
    pSUM(F,tempF,F);
    computeDampingForce(jello->p[i][j][k], jello->p[i+1][j][k-1],jello->v[i][j][k], jello->v[i+1][j][k-1], jello->dElastic, tempF);
    pSUM(F,tempF,F);
  }
  if((i+1<=7) && (k+1<=7)) // If A has right, front face diagonal neighbor
  {
    computeHooksForce(jello->p[i][j][k], jello->p[i+1][j][k+1], jello->kElastic, rLShearFace, tempF);
    pSUM(F,tempF,F);
    computeDampingForce(jello->p[i][j][k], jello->p[i+1][j][k+1],jello->v[i][j][k], jello->v[i+1][j][k+1], jello->dElastic, tempF);
    pSUM(F,tempF,F);
  }
  //For 4 face diagonals along vertical plane k and all 8 3D cube main diagonal
  if((i-1>=0) && (j-1>=0)) // If A has left, bottom face diagonal neighbor
  {
    computeHooksForce(jello->p[i][j][k], jello->p[i-1][j-1][k], jello->kElastic, rLShearFace, tempF);
    pSUM(F,tempF,F);
    computeDampingForce(jello->p[i][j][k], jello->p[i-1][j-1][k],jello->v[i][j][k], jello->v[i-1][j-1][k], jello->dElastic, tempF);
    pSUM(F,tempF,F);
    if(k-1>=0)  // For left, bottom, back 3D cube main diagonal
    {
      computeHooksForce(jello->p[i][j][k], jello->p[i-1][j-1][k-1], jello->kElastic, rLShearMainDiag, tempF);
      pSUM(F,tempF,F);
      computeDampingForce(jello->p[i][j][k], jello->p[i-1][j-1][k-1],jello->v[i][j][k], jello->v[i-1][j-1][k-1], jello->dElastic, tempF);
      pSUM(F,tempF,F);
    }
    if(k+1<=7)  // For left, bottom, front 3D cube main diagonal
    {
      computeHooksForce(jello->p[i][j][k], jello->p[i-1][j-1][k+1], jello->kElastic, rLShearMainDiag, tempF);
      pSUM(F,tempF,F);
      computeDampingForce(jello->p[i][j][k], jello->p[i-1][j-1][k+1],jello->v[i][j][k], jello->v[i-1][j-1][k+1], jello->dElastic, tempF);
      pSUM(F,tempF,F);
    }
  }
  if((i-1>=0) && (j+1<=7)) // If A has left, top face diagonal neighbor
  {
    computeHooksForce(jello->p[i][j][k], jello->p[i-1][j+1][k], jello->kElastic, rLShearFace, tempF);
    pSUM(F,tempF,F);
    computeDampingForce(jello->p[i][j][k], jello->p[i-1][j+1][k],jello->v[i][j][k], jello->v[i-1][j+1][k], jello->dElastic, tempF);
    pSUM(F,tempF,F);
    if(k-1>=0)  // For left, top, back 3D cube main diagonal
    {
      computeHooksForce(jello->p[i][j][k], jello->p[i-1][j+1][k-1], jello->kElastic, rLShearMainDiag, tempF);
      pSUM(F,tempF,F);
      computeDampingForce(jello->p[i][j][k], jello->p[i-1][j+1][k-1],jello->v[i][j][k], jello->v[i-1][j+1][k-1], jello->dElastic, tempF);
      pSUM(F,tempF,F);
    }
    if(k+1<=7)  // For left, top, front 3D cube main diagonal
    {
      computeHooksForce(jello->p[i][j][k], jello->p[i-1][j+1][k+1], jello->kElastic, rLShearMainDiag, tempF);
      pSUM(F,tempF,F);
      computeDampingForce(jello->p[i][j][k], jello->p[i-1][j+1][k+1],jello->v[i][j][k], jello->v[i-1][j+1][k+1], jello->dElastic, tempF);
      pSUM(F,tempF,F);
    }
  }
  if((i+1<=7) && (j-1>=0)) // If A has right, bottom face diagonal neighbor
  {
    computeHooksForce(jello->p[i][j][k], jello->p[i+1][j-1][k], jello->kElastic, rLShearFace, tempF);
    pSUM(F,tempF,F);
    computeDampingForce(jello->p[i][j][k], jello->p[i+1][j-1][k],jello->v[i][j][k], jello->v[i+1][j-1][k], jello->dElastic, tempF);
    pSUM(F,tempF,F);
    if(k-1>=0)  // For right, bottom, back 3D cube main diagonal
    {
      computeHooksForce(jello->p[i][j][k], jello->p[i+1][j-1][k-1], jello->kElastic, rLShearMainDiag, tempF);
      pSUM(F,tempF,F);
      computeDampingForce(jello->p[i][j][k], jello->p[i+1][j-1][k-1],jello->v[i][j][k], jello->v[i+1][j-1][k-1], jello->dElastic, tempF);
      pSUM(F,tempF,F);
    }
    if(k+1<=7)  // For right, bottom, front 3D cube main diagonal
    {
      computeHooksForce(jello->p[i][j][k], jello->p[i+1][j-1][k+1], jello->kElastic, rLShearMainDiag, tempF);
      pSUM(F,tempF,F);
      computeDampingForce(jello->p[i][j][k], jello->p[i+1][j-1][k+1],jello->v[i][j][k], jello->v[i+1][j-1][k+1], jello->dElastic, tempF);
      pSUM(F,tempF,F);
    }
  }
  if((i+1<=7) && (j+1<=7)) // If A has right, top face diagonal neighbor
  {
    computeHooksForce(jello->p[i][j][k], jello->p[i+1][j+1][k], jello->kElastic, rLShearFace, tempF);
    pSUM(F,tempF,F);
    computeDampingForce(jello->p[i][j][k], jello->p[i+1][j+1][k],jello->v[i][j][k], jello->v[i+1][j+1][k], jello->dElastic, tempF);
    pSUM(F,tempF,F);
    if(k-1>=0)  // For right, top, back 3D cube main diagonal
    {
      computeHooksForce(jello->p[i][j][k], jello->p[i+1][j+1][k-1], jello->kElastic, rLShearMainDiag, tempF);
      pSUM(F,tempF,F);
      computeDampingForce(jello->p[i][j][k], jello->p[i+1][j+1][k-1],jello->v[i][j][k], jello->v[i+1][j+1][k-1], jello->dElastic, tempF);
      pSUM(F,tempF,F);
    }
    if(k+1<=7)  // For right, top, front 3D cube main diagonal
    {
      computeHooksForce(jello->p[i][j][k], jello->p[i+1][j+1][k+1], jello->kElastic, rLShearMainDiag, tempF);
      pSUM(F,tempF,F);
      computeDampingForce(jello->p[i][j][k], jello->p[i+1][j+1][k+1],jello->v[i][j][k], jello->v[i+1][j+1][k+1], jello->dElastic, tempF);
      pSUM(F,tempF,F);
    }
  }
}

// Computes bend spring forces on mass point A from bend springs connected with it's neighbor mass points
void computeBendForce(struct world * jello, int i, int j, int k, struct point &F)
{
  point tempF;
  if(i-2>=0)  // If A has left neighbor, force from spring between A and that neighbor
  {
    computeHooksForce(jello->p[i][j][k], jello->p[i-2][j][k], jello->kElastic, rLBend, tempF);
    pSUM(F,tempF,F);
    computeDampingForce(jello->p[i][j][k], jello->p[i-2][j][k],jello->v[i][j][k], jello->v[i-2][j][k], jello->dElastic, tempF);
    pSUM(F,tempF,F);
  }
  if(i+2<=7)  // If A has right neighbor, force from spring between A and that neighbor
  {
    computeHooksForce(jello->p[i][j][k], jello->p[i+2][j][k], jello->kElastic, rLBend, tempF);
    pSUM(F,tempF,F);
    computeDampingForce(jello->p[i][j][k], jello->p[i+2][j][k],jello->v[i][j][k], jello->v[i+2][j][k], jello->dElastic, tempF);
    pSUM(F,tempF,F);
  }
  if(j-2>=0)  // If A has bottom neighbor, force from spring between A and that neighbor
  {
    computeHooksForce(jello->p[i][j][k], jello->p[i][j-2][k], jello->kElastic, rLBend, tempF);
    pSUM(F,tempF,F);
    computeDampingForce(jello->p[i][j][k], jello->p[i][j-2][k],jello->v[i][j][k], jello->v[i][j-2][k], jello->dElastic, tempF);
    pSUM(F,tempF,F);
  }
  if(j+2<=7)  // If A has top neighbor, force from spring between A and that neighbor
  {
    computeHooksForce(jello->p[i][j][k], jello->p[i][j+2][k], jello->kElastic, rLBend, tempF);
    pSUM(F,tempF,F);
    computeDampingForce(jello->p[i][j][k], jello->p[i][j+2][k],jello->v[i][j][k], jello->v[i][j+2][k], jello->dElastic, tempF);
    pSUM(F,tempF,F);
  }
  if(k-2>=0)  // If A has back neighbor, force from spring between A and that neighbor
  {
    computeHooksForce(jello->p[i][j][k], jello->p[i][j][k-2], jello->kElastic, rLBend, tempF);
    pSUM(F,tempF,F);
    computeDampingForce(jello->p[i][j][k], jello->p[i][j][k-2],jello->v[i][j][k], jello->v[i][j][k-2], jello->dElastic, tempF);
    pSUM(F,tempF,F);
  }
  if(k+2<=7)  // If A has front neighbor, force from spring between A and that neighbor
  {
    computeHooksForce(jello->p[i][j][k], jello->p[i][j][k+2], jello->kElastic, rLBend, tempF);
    pSUM(F,tempF,F);
    computeDampingForce(jello->p[i][j][k], jello->p[i][j][k+2],jello->v[i][j][k], jello->v[i][j][k+2], jello->dElastic, tempF);
    pSUM(F,tempF,F);
  }
}

// Computes collision spring forces on mass point A from collision with bounding box wall using Penalty method
// Also force on mass point A from collision with inclined plane
void computeCollisionForce(struct world * jello, int i, int j, int k, struct point &F)
{
  point tempF,vObstacle;
  pMAKE(0.0,0.0,0.0,vObstacle); // Velocity of any obstacle is (0,0,0)

  // For collision with walls
  // Direction will be normal to contact surface, so contact point can be calculated as:
  // for wall at x=2, and A(i,j,k), cp is (2,j,k); similarly for all other walls
  point cp[6];
  double mag; // Magnitude will be proportional to the amount of penetration
  pMAKE(2,jello->p[i][j][k].y,jello->p[i][j][k].z,cp[0]); // initializing contact points for each wall
  pMAKE(-2,jello->p[i][j][k].y,jello->p[i][j][k].z,cp[1]);  
  pMAKE(jello->p[i][j][k].x,2,jello->p[i][j][k].z,cp[2]);
  pMAKE(jello->p[i][j][k].x,-2,jello->p[i][j][k].z,cp[3]);
  pMAKE(jello->p[i][j][k].x,jello->p[i][j][k].y,2,cp[4]);
  pMAKE(jello->p[i][j][k].x,jello->p[i][j][k].y,-2,cp[5]);
  if(jello->p[i][j][k].x>2)
  {
    mag = abs(jello->p[i][j][k].x) - 2;
    computeHooksForce(jello->p[i][j][k],cp[0],(jello->kCollision * mag),0,tempF);
    pSUM(F,tempF,F);
    computeDampingForce(jello->p[i][j][k],cp[0],jello->v[i][j][k],vObstacle,(jello->dCollision * mag),tempF);
    pSUM(F,tempF,F);
  }
  if(jello->p[i][j][k].x<-2)
  {
    mag = abs(jello->p[i][j][k].x) - 2;
    computeHooksForce(jello->p[i][j][k],cp[1],(jello->kCollision * mag),0,tempF);
    pSUM(F,tempF,F);
    computeDampingForce(jello->p[i][j][k],cp[1],jello->v[i][j][k],vObstacle,(jello->dCollision * mag),tempF);
    pSUM(F,tempF,F);
  }
  if(jello->p[i][j][k].y>2)
  {
    mag = abs(jello->p[i][j][k].y) - 2;
    computeHooksForce(jello->p[i][j][k],cp[2],(jello->kCollision * mag),0,tempF);
    pSUM(F,tempF,F);
    computeDampingForce(jello->p[i][j][k],cp[2],jello->v[i][j][k],vObstacle,(jello->dCollision * mag),tempF);
    pSUM(F,tempF,F);
  }
  if(jello->p[i][j][k].y<-2)
  {
    mag = abs(jello->p[i][j][k].y) - 2;
    computeHooksForce(jello->p[i][j][k],cp[3],(jello->kCollision * mag),0,tempF);
    pSUM(F,tempF,F);
    computeDampingForce(jello->p[i][j][k],cp[3],jello->v[i][j][k],vObstacle,(jello->dCollision * mag),tempF);
    pSUM(F,tempF,F);
  }
  if(jello->p[i][j][k].z>2)
  {
    mag = abs(jello->p[i][j][k].z) - 2;
    computeHooksForce(jello->p[i][j][k],cp[4],(jello->kCollision * mag),0,tempF);
    pSUM(F,tempF,F);
    computeDampingForce(jello->p[i][j][k],cp[4],jello->v[i][j][k],vObstacle,(jello->dCollision * mag),tempF);
    pSUM(F,tempF,F);
  }
  if(jello->p[i][j][k].z<-2)
  {
    mag = abs(jello->p[i][j][k].z) - 2;
    computeHooksForce(jello->p[i][j][k],cp[5],(jello->kCollision * mag),0,tempF);
    pSUM(F,tempF,F);
    computeDampingForce(jello->p[i][j][k],cp[5],jello->v[i][j][k],vObstacle,(jello->dCollision * mag),tempF);
    pSUM(F,tempF,F);
  }

  // For collision with inclined plane
  if(jello->incPlanePresent)
  {
    point planeNormal, pOnPlane;
    double checkPoint, sideOfPoint, sideOfNormal, dist;
    // To check if point is on plane and if not then on which side
    checkPoint = jello->a * jello->p[i][j][k].x + jello->b * jello->p[i][j][k].y + jello->c * jello->p[i][j][k].z + jello->d;
    sideOfPoint = copysign(1,checkPoint); // If checkpoint < 0 then -1 and if > 0 then 1
    // To check on which side the normal is facing 
    pMAKE(jello->a,jello->b,jello->c,planeNormal);  // one of the normals on plane
    pLENGTH(planeNormal,dist);
    dist = checkPoint / dist;  // Distance of point from plane
    sideOfNormal = copysign(1,dist); // If dist < 0 then -1 and if > 0 then 1
    dist *= sideOfNormal * sideOfPoint; 
    if(dist<=0) // If both are on same side then no force, if not same side then there has been collision, so force is calculated
    {
      normalize(planeNormal);  // Normalized normal
      pMULTIPLY(planeNormal,dist,planeNormal);  // Normal vector from point to plane
      pDIFFERENCE(jello->p[i][j][k],planeNormal,pOnPlane);  // Point on plane
      computeHooksForce(jello->p[i][j][k],pOnPlane,(jello->kCollision * abs(dist)),0,tempF);
      pSUM(F,tempF,F);
      computeDampingForce(jello->p[i][j][k],pOnPlane,jello->v[i][j][k],vObstacle,(jello->dCollision * abs(dist)),tempF);
      pSUM(F,tempF,F);
    }
  }
}

// Computes force due to external force field on mass point A
void computeExternalForce(struct world * jello, int i, int j, int k, struct point &F)
{
  double ffVoxelLen = 4.0 / (jello->resolution - 1);  // Length of a voxel in the force field
  // Converting world space coordinates (i,j,k) of mass point A to position in force field (X,Y,Z).
  // Origin of force field in world space is at (-2,-2,-2). So the translation matrix would be:
  // 1 0 0 -(-2)
  // 0 1 0 -(-2)
  // 0 0 1 -(-2)
  // 0 0 0 1
  int X = int((jello->p[i][j][k].x + 2) / ffVoxelLen);
  int Y = int((jello->p[i][j][k].y + 2) / ffVoxelLen);
  int Z = int((jello->p[i][j][k].z + 2) / ffVoxelLen);
  // Checking if (X,Y,Z) is within bounding box (0,0,0) to (resolution-1,resolution-1,resolution-1) in force field coordinate system
  if (X >= (jello->resolution - 1))
    X = (jello->resolution - 1)-1;
  else if (X < 0)
    X = 0;
  if (Y >= (jello->resolution - 1))
    Y = (jello->resolution - 1)-1;
  else if (Y < 0)
    Y = 0;
  if (Z >= (jello->resolution - 1))
    Z = (jello->resolution - 1)-1;
  else if (Z < 0)
    Z = 0;
  // Forces at 8 corners of voxel with mass point A
  point F000,F001,F010,F011,F100,F101,F110,F111;
  pCPY(jello->forceField[X * jello->resolution * jello->resolution + Y * jello->resolution + Z],F000);
  pCPY(jello->forceField[X * jello->resolution * jello->resolution + Y * jello->resolution + (Z+1)],F001);
  pCPY(jello->forceField[X * jello->resolution * jello->resolution + (Y+1) * jello->resolution + Z],F010);
  pCPY(jello->forceField[X * jello->resolution * jello->resolution + (Y+1) * jello->resolution + (Z+1)],F011);
  pCPY(jello->forceField[(X+1) * jello->resolution * jello->resolution + Y * jello->resolution + Z],F100);
  pCPY(jello->forceField[(X+1) * jello->resolution * jello->resolution + Y * jello->resolution + (Z+1)],F101);
  pCPY(jello->forceField[(X+1) * jello->resolution * jello->resolution + (Y+1) * jello->resolution + Z],F110);
  pCPY(jello->forceField[(X+1) * jello->resolution * jello->resolution + (Y+1) * jello->resolution + (Z+1)],F111);

  // Trilinear interpolation - 
  // Converting force field (X,Y,Z) position of mass point A back to world space and then computing [(i,j,k) - worldspaceA]/ffVoxelLen
  point A;
  A.x = (jello->p[i][j][k].x - (X * ffVoxelLen - 2))/ffVoxelLen;
  A.y = (jello->p[i][j][k].y - (Y * ffVoxelLen - 2))/ffVoxelLen;
  A.z = (jello->p[i][j][k].z - (Z * ffVoxelLen - 2))/ffVoxelLen;
  // Computing the contribution of force at voxel corners to mass point A using Convex Combination
  pMULTIPLY(F000, ((1 - A.x) * (1 - A.y) * (1 - A.z)), F000);
  pSUM(F000,F,F);
  pMULTIPLY(F001, ((1 - A.x) * (1 - A.y) * A.z), F001);
  pSUM(F001,F,F);
  pMULTIPLY(F010, ((1 - A.x) * A.y * (1 - A.z)), F010);
  pSUM(F010,F,F);
  pMULTIPLY(F011, ((1 - A.x) * A.y * A.z), F011);
  pSUM(F011,F,F);
  pMULTIPLY(F100, (A.x * (1 - A.y) * (1 - A.z)), F100);
  pSUM(F100,F,F);
  pMULTIPLY(F101, (A.x * (1 - A.y) * A.z), F101);
  pSUM(F101,F,F);
  pMULTIPLY(F110, (A.x * A.y * (1 - A.z)), F110);
  pSUM(F110,F,F);
  pMULTIPLY(F111, (A.x * A.y * A.z), F111);
  pSUM(F111,F,F);
}

/* Computes acceleration to every control point of the jello cube, 
   which is in state given by 'jello'.
   Returns result in array 'a'. */
void computeAcceleration(struct world * jello, struct point a[8][8][8])
{
  /* for you to implement ... */
  int i,j,k,dragLen;
  point tempmpdF;
  for (i=0; i<=7; i++)
    for (j=0; j<=7; j++)
      for (k=0; k<=7; k++)
      {
        pMAKE(0.0,0.0,0.0,a[i][j][k]);
        computeStrucForce(jello,i,j,k,a[i][j][k]);
        computeShearForce(jello,i,j,k,a[i][j][k]);
        computeBendForce(jello,i,j,k,a[i][j][k]);
        computeCollisionForce(jello,i,j,k,a[i][j][k]);
        computeExternalForce(jello,i,j,k,a[i][j][k]);
        pCPY(mousedragForce,tempmpdF);
        pLENGTH(tempmpdF,dragLen);
        if(dragLen>0)
        {
          pMULTIPLY(tempmpdF,0.4,tempmpdF);
          pSUM(tempmpdF,a[i][j][k],a[i][j][k]);
        }
        pMULTIPLY(a[i][j][k],(1.0 / jello->mass),a[i][j][k]);

      }
  pMAKE(0.0,0.0,0.0,mousedragForce);
}

/* performs one step of Euler Integration */
/* as a result, updates the jello structure */
void Euler(struct world * jello)
{
  int i,j,k;
  point a[8][8][8];

  computeAcceleration(jello, a);
  
  for (i=0; i<=7; i++)
    for (j=0; j<=7; j++)
      for (k=0; k<=7; k++)
      {
        jello->p[i][j][k].x += jello->dt * jello->v[i][j][k].x;
        jello->p[i][j][k].y += jello->dt * jello->v[i][j][k].y;
        jello->p[i][j][k].z += jello->dt * jello->v[i][j][k].z;
        jello->v[i][j][k].x += jello->dt * a[i][j][k].x;
        jello->v[i][j][k].y += jello->dt * a[i][j][k].y;
        jello->v[i][j][k].z += jello->dt * a[i][j][k].z;

      }
}

/* performs one step of RK4 Integration */
/* as a result, updates the jello structure */
void RK4(struct world * jello)
{
  point F1p[8][8][8], F1v[8][8][8], 
        F2p[8][8][8], F2v[8][8][8],
        F3p[8][8][8], F3v[8][8][8],
        F4p[8][8][8], F4v[8][8][8];

  point a[8][8][8];


  struct world buffer;

  int i,j,k;

  buffer = *jello; // make a copy of jello

  computeAcceleration(jello, a);

  for (i=0; i<=7; i++)
    for (j=0; j<=7; j++)
      for (k=0; k<=7; k++)
      {
         pMULTIPLY(jello->v[i][j][k],jello->dt,F1p[i][j][k]);
         pMULTIPLY(a[i][j][k],jello->dt,F1v[i][j][k]);
         pMULTIPLY(F1p[i][j][k],0.5,buffer.p[i][j][k]);
         pMULTIPLY(F1v[i][j][k],0.5,buffer.v[i][j][k]);
         pSUM(jello->p[i][j][k],buffer.p[i][j][k],buffer.p[i][j][k]);
         pSUM(jello->v[i][j][k],buffer.v[i][j][k],buffer.v[i][j][k]);
      }

  computeAcceleration(&buffer, a);

  for (i=0; i<=7; i++)
    for (j=0; j<=7; j++)
      for (k=0; k<=7; k++)
      {
         // F2p = dt * buffer.v;
         pMULTIPLY(buffer.v[i][j][k],jello->dt,F2p[i][j][k]);
         // F2v = dt * a(buffer.p,buffer.v);     
         pMULTIPLY(a[i][j][k],jello->dt,F2v[i][j][k]);
         pMULTIPLY(F2p[i][j][k],0.5,buffer.p[i][j][k]);
         pMULTIPLY(F2v[i][j][k],0.5,buffer.v[i][j][k]);
         pSUM(jello->p[i][j][k],buffer.p[i][j][k],buffer.p[i][j][k]);
         pSUM(jello->v[i][j][k],buffer.v[i][j][k],buffer.v[i][j][k]);
      }

  computeAcceleration(&buffer, a);

  for (i=0; i<=7; i++)
    for (j=0; j<=7; j++)
      for (k=0; k<=7; k++)
      {
         // F3p = dt * buffer.v;
         pMULTIPLY(buffer.v[i][j][k],jello->dt,F3p[i][j][k]);
         // F3v = dt * a(buffer.p,buffer.v);     
         pMULTIPLY(a[i][j][k],jello->dt,F3v[i][j][k]);
         pMULTIPLY(F3p[i][j][k],1.0,buffer.p[i][j][k]);
         pMULTIPLY(F3v[i][j][k],1.0,buffer.v[i][j][k]);
         pSUM(jello->p[i][j][k],buffer.p[i][j][k],buffer.p[i][j][k]);
         pSUM(jello->v[i][j][k],buffer.v[i][j][k],buffer.v[i][j][k]);
      }
         
  computeAcceleration(&buffer, a);


  for (i=0; i<=7; i++)
    for (j=0; j<=7; j++)
      for (k=0; k<=7; k++)
      {
         // F3p = dt * buffer.v;
         pMULTIPLY(buffer.v[i][j][k],jello->dt,F4p[i][j][k]);
         // F3v = dt * a(buffer.p,buffer.v);     
         pMULTIPLY(a[i][j][k],jello->dt,F4v[i][j][k]);

         pMULTIPLY(F2p[i][j][k],2,buffer.p[i][j][k]);
         pMULTIPLY(F3p[i][j][k],2,buffer.v[i][j][k]);
         pSUM(buffer.p[i][j][k],buffer.v[i][j][k],buffer.p[i][j][k]);
         pSUM(buffer.p[i][j][k],F1p[i][j][k],buffer.p[i][j][k]);
         pSUM(buffer.p[i][j][k],F4p[i][j][k],buffer.p[i][j][k]);
         pMULTIPLY(buffer.p[i][j][k],1.0 / 6,buffer.p[i][j][k]);
         pSUM(buffer.p[i][j][k],jello->p[i][j][k],jello->p[i][j][k]);

         pMULTIPLY(F2v[i][j][k],2,buffer.p[i][j][k]);
         pMULTIPLY(F3v[i][j][k],2,buffer.v[i][j][k]);
         pSUM(buffer.p[i][j][k],buffer.v[i][j][k],buffer.p[i][j][k]);
         pSUM(buffer.p[i][j][k],F1v[i][j][k],buffer.p[i][j][k]);
         pSUM(buffer.p[i][j][k],F4v[i][j][k],buffer.p[i][j][k]);
         pMULTIPLY(buffer.p[i][j][k],1.0 / 6,buffer.p[i][j][k]);
         pSUM(buffer.p[i][j][k],jello->v[i][j][k],jello->v[i][j][k]);
      }

  return;  
}
