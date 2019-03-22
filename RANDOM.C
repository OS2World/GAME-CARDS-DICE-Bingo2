//#define GetRandom( min, max ) ((rand() % (int)(((max)+1) - (min))) + (min))
#include <stdlib.h>
#include <time.h>

void InitRandom( void );
long GetRandom( long min, long max );

void InitRandom( void )
{
  srand( (unsigned)time( NULL ) );
  return;
}

long GetRandom( long min, long max )
{
  return ((rand() % (int)(((max)+1) - (min))) + (min));
}
