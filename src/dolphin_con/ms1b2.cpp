
#include <windows.h>
#include <iostream>
#include <cmath>
#include <cassert>

#include "ms1b.h"

using namespace std;

#if _DEBUG
const unsigned times = 1000000;
#else
const unsigned times = 10000000;
#endif
__int64 freq;

//-----------------------------------------------------------------------------

// compare
unsigned __fastcall NearestPowerOf2(unsigned n)
{
	if (!n) return n;  // (0 == 2^0)


	unsigned x = 1;
	while(x < n) {
		x <<= 1;
	}
	return x;
}

//-----------------------------------------------------------------------------

//  saturation
unsigned __fastcall NearestPowerOf2_1( unsigned x ) 
{
	--x;    
	x |= x >> 1;
	x |= x >> 2;    
	x |= x >> 4;    
	x |= x >> 8;    
	x |= x >> 16;    
	return ++x;
}

//-----------------------------------------------------------------------------

// logarithmic
unsigned __fastcall NearestPowerOf2_2( unsigned n )
{
	return (unsigned) pow( (float) 2, ceil( log( (float) n ) / log( (float) 2 ) ) );
}

//-----------------------------------------------------------------------------

// recursion
unsigned __fastcall NearestPowerOf2_3(unsigned i)
{   
	unsigned x = ((i - 1) & i);   
	return x ? NearestPowerOf2_3(x) : i << 1;
}

//-----------------------------------------------------------------------------

// floating pounsigned representation
unsigned __fastcall NearestPowerOf2_4( unsigned n )
{	
	const unsigned MantissaBitdepth = 23;	
	const unsigned MantissaMask = (1<<MantissaBitdepth) - 1;	

	(*(float*)&n) = (float) n;	

	n  += MantissaMask;
	n >>= 23;
	n  -= 127;

	return 1 << n;
} 

//-----------------------------------------------------------------------------

// always 5 if statements (unrolled compare)
unsigned __fastcall NearestPowerOf2_5( unsigned n )
{
	if (n > 1<<16)
	{
		if (n > 1<<24)
		{
			if (n > 1<<28)
			{
				if (n > 1<<30)
				{
					if (n > 1<<31)
					{
						return 1<<32;
					}
					else
					{
						return 1<<31;
					}
				}
				else
				{
					if (n > 1<<29)
					{
						return 1<<30;
					}
					else
					{
						return 1<<29;
					}
				}
			}
			else
			{
				if (n > 1<<26)
				{
					if (n > 1<<27)
					{
						return 1<<28;
					}
					else
					{
						return 1<<27;
					}
				}
				else
				{
					if (n > 1<<25)
					{
						return 1<<26;
					}
					else
					{
						return 1<<25;
					}
				}
			}
		}
		else
		{
			if (n > 1<<20)
			{
				if (n > 1<<22)
				{
					if (n > 1<<23)
					{
						return 1<<24;
					}
					else
					{
						return 1<<23;
					}
				}
				else
				{
					if (n > 1<<21)
					{
						return 1<<22;
					}
					else
					{
						return 1<<21;
					}
				}
			}
			else
			{
				if (n > 1<<18)
				{
					if (n > 1<<19)
					{
						return 1<<20;
					}
					else
					{
						return 1<<19;
					}
				}
				else
				{
					if (n > 1<<17)
					{
						return 1<<18;
					}
					else
					{
						return 1<<17;
					}
				}
			}
		}
	}
	else
	{
		if (n > 1<<8)
		{
			if (n > 1<<12)
			{
				if (n > 1<<14)
				{
					if (n > 1<<15)
					{
						return 1<<16;
					}
					else
					{
						return 1<<15;
					}
				}
				else
				{
					if (n > 1<<13)
					{
						return 1<<14;
					}
					else
					{
						return 1<<13;
					}
				}
			}
			else
			{
				if (n > 1<<10)
				{
					if (n > 1<<11)
					{
						return 1<<12;
					}
					else
					{
						return 1<<11;
					}
				}
				else
				{
					if (n > 1<<9)
					{
						return 1<<10;
					}
					else
					{
						return 1<<9;
					}
				}
			}
		}
		else
		{
			if (n > 1<<4)
			{
				if (n > 1<<6)
				{
					if (n > 1<<7)
					{
						return 1<<8;
					}
					else
					{
						return 1<<7;
					}
				}
				else
				{
					if (n > 1<<5)
					{
						return 1<<6;
					}
					else
					{
						return 1<<5;
					}
				}
			}
			else
			{
				if (n > 1<<2)
				{
					if (n > 1<<3)
					{
						return 1<<4;
					}
					else
					{
						return 1<<3;
					}
				}
				else
				{
					if (n > 1<<1)
					{
						return 1<<2;
					}
					else
					{
						return 1<<1;
					}
				}
			}
		}
	}
} 

//-----------------------------------------------------------------------------

// asm trickery, assume N = 244 (11110100)
__declspec( naked )
unsigned __fastcall NearestPowerOf2_6( unsigned n ) 
{ 
	// DigitalDelusion version

	__asm
	{
		dec ecx
		mov eax, 2
		bsr ecx, ecx
		rol eax, cl
		ret
	}
/*
	__asm
	{	
		// ecx = n							
		dec ecx			// n-=1;		n= 243 (111100110)
		mov eax,1		// store 1 in eax	
		bsr ecx,ecx		// get the number of the first bit that is set to 1, scanning from MSB to LSB  (eg.  000111100110 would give you 8)
		inc ecx			// ecx = 9
		shl eax, cl		// shift left 1 by 9 gives you 256
		ret
	}
*/
}

//-----------------------------------------------------------------------------

// floating point representation
unsigned __fastcall NearestPowerOf2_7( unsigned n )
{	
	return 0;
} 

//-----------------------------------------------------------------------------

typedef unsigned (__fastcall * fun)(unsigned);

unsigned test(fun f)
{
	__int64 before;
	__int64 after;

	srand(345678);

	QueryPerformanceCounter ((LARGE_INTEGER*) &before);

	for (int i=0; i<times; i++) {
		f(rand());
	}

	QueryPerformanceCounter ((LARGE_INTEGER*) &after);

	__int64 diff = after - before;
	__int64 milliseconds = diff * 1000 / freq;
	return (unsigned) milliseconds;
}

//-----------------------------------------------------------------------------

int ms1b2_main(int argc, char* argv[])
{
	srand(345678);

	QueryPerformanceFrequency((LARGE_INTEGER*) &freq);

	for (int i=0; i<123456; i++) {
		unsigned n = rand();
		assert(! (NearestPowerOf2(n) == NearestPowerOf2_1(n) == NearestPowerOf2_2(n)
            == NearestPowerOf2_3(n) == NearestPowerOf2_4(n) == NearestPowerOf2_5(n) == NearestPowerOf2_6(n) ));
	}

    std::cout << std::endl;

	unsigned k  = test(NearestPowerOf2);
	std::cout << "compare     " << k  << "\tms" << std::endl;
    std::cout.flush();

	unsigned k1 = test(NearestPowerOf2_1);
	std::cout << "saturation  " << k1 << "\tms" << std::endl;
    std::cout.flush();

	unsigned k2 = test(NearestPowerOf2_2);
	std::cout << "fpu log2    " << k2 << "\tms" << std::endl;
    std::cout.flush();

	unsigned k3 = test(NearestPowerOf2_3);
	std::cout << "recursion   " << k3 << "\tms" << std::endl;
    std::cout.flush();

	unsigned k4 = test(NearestPowerOf2_4);
	std::cout << "float trick " << k4 << "\tms" << std::endl;
    std::cout.flush();

	unsigned k5 = test(NearestPowerOf2_5);
	std::cout << "unrolled if " << k5 << "\tms" << std::endl;
    std::cout.flush();

	unsigned k6 = test(NearestPowerOf2_6);
	std::cout << "asm bsr     " << k6 << "\tms" << std::endl;
    std::cout.flush();

//	unsigned k7 = test(NearestPowerOf2_7);
//	std::cout << "            " << k7 << "\tms" << std::endl;

/*
	char c;
	std::cin >> c;
*/
    std::cout << std::endl;
    system("pause");
	return 0;
}
