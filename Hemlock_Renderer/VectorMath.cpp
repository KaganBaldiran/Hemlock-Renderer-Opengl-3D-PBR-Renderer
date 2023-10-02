#include "VectorMath.h"

//Infamous QUACK III inverse square root algorithm
float Q_rsqrt(float number)
{
	long i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = number * 0.5F;
	y = number;
	i = *(long*)&y;                       // evil floating point bit level hacking
	i = 0x5f3759df - (i >> 1);               // what the fuck?
	y = *(float*)&i;
	y = y * (threehalfs - (x2 * y * y));   // 1st iteration
		// y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed
	return y;
}

int GiveRandomNumf(int min, int max, int sizeofarray, bool exclude_on_off, int numtoexclude) {

	static int timesthisfunctioncalled = 0;
	int* randarray = new int[sizeofarray];
	*randarray = NULL;
	int finalresult;

	srand((unsigned)time(NULL));
	if (min < max)
	{
		for (size_t i = 0; i < sizeofarray; i++)
		{
			randarray[i] = rand() % max;

			if (randarray[i] < min)
			{
				i--;
			}
			if (exclude_on_off == true)
			{
				if (randarray[i] == numtoexclude)
				{

					i--;

				}

			}

		}
	}

	timesthisfunctioncalled++;

	if (timesthisfunctioncalled > sizeofarray - 1)
	{
		timesthisfunctioncalled = (timesthisfunctioncalled % (sizeofarray - 1));
	}

	finalresult = randarray[timesthisfunctioncalled];
	delete[] randarray;
	return finalresult;

};
