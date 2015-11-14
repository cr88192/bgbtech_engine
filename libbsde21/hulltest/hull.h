#ifndef HULL_H
#define HULL_H

typedef struct Hull_Face_s Hull_Face;
typedef struct Solid_Hull_s Solid_Hull;

struct Hull_Face_s {
Hull_Face *next;

float norm[4];

int nvecs;
float *vecs;
float *evecs;
};

struct Solid_Hull_s {
Solid_Hull *next;

float org[3];
float vel[3];
float avel[3];

float tvel[3];
float rot[9];

float ivel[3];	//impulse velocity
float itvel[3];	//impulse torque

float mass;	//Hull Mass
float moment;	//Moment of Interia (scalar)

float mins[3];	//bbox of hull
float maxs[3];

Hull_Face *face;
};

#endif
