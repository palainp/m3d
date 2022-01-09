#include "lib_mat.h"

void mult_vecteur(t_point3d* dest, double mat[4][4], t_point3d* src)
{
	for (size_t i=0; i<4; ++i)
	{
		dest->xyzt[i] = 0;
		for (size_t j=0; j<4; ++j)
		{
			dest->xyzt[i] += mat[i][j]*src->xyzt[j];
		}
	}
}

void mult_mat(double dest[4][4], double mat[4][4], double src[4][4])
{
	for (size_t i=0; i<4; ++i)
	{
		for (size_t j=0; j<4; ++j)
		{
			dest[i][j] = 0;
			for (size_t k=0; k<4; ++k)
			{
				dest[i][j] += mat[i][k]*src[k][j];
			}
		}
	}
}
