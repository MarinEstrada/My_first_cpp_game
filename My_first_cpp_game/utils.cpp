typedef char char8;
typedef unsigned char uchar8;
typedef short short16;
typedef unsigned short ushort16;
typedef int int32;
typedef unsigned int uint32;
typedef long long long64;
typedef unsigned long long ulong64;

#define global_var static
#define internal_var static


inline int clamp(int min, int val, int max) {
	if (val < min) return min;
	if (val > max) return max;
	return val;
}
