#ifndef _LIB_MAT_H_
#define _LIB_MAT_H_
#include "lib_3d.h"

void mult_vecteur(t_point3d* dest, double mat[4][4], t_point3d* src); // dest = mat*src
void mult_mat(double dest[4][4], double mat[4][4], double src[4][4]); // dest = mat*src

#endif
