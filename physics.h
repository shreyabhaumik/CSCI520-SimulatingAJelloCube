/*

  USC/Viterbi/Computer Science
  "Jello Cube" Assignment 1 starter code

*/

#ifndef _PHYSICS_H_
#define _PHYSICS_H_

// Computes elastic force on A because of spring between A and B
void computeHooksForce(struct point A, struct point B, double kH, double rL, struct point &F);
// Computes damping force on A for A and B mass points connected with a spring
void computeDampingForce(struct point A, struct point B, struct point vA, struct point vB, double kD, struct point &F);
// Computes structural spring forces on mass point A from structural springs connected with it's neighbor mass points
void computeStrucForce(struct world * jello, int i, int j, int k, struct point &totStrucF);
// Computes shear spring forces on mass point A from shear springs connected with it's neighbor mass points
void computeShearForce(struct world * jello, int i, int j, int k, struct point &totStrucF);
// Computes bend spring forces on mass point A from bend springs connected with it's neighbor mass points
void computeBendForce(struct world * jello, int i, int j, int k, struct point &totStrucF);
// Computes collision spring forces on mass point A from collision with bounding box wall using Penalty method
void computeCollisionForce(struct world * jello, int i, int j, int k, struct point &F);
// Computes force due to external force field on mass point A
void computeExternalForce(struct world * jello, int i, int j, int k, struct point &F);

void computeAcceleration(struct world * jello, struct point a[8][8][8]);

// perform one step of Euler and Runge-Kutta-4th-order integrators
// updates the jello structure accordingly
void Euler(struct world * jello);
void RK4(struct world * jello);

#endif

