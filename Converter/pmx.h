#pragma once
#include "stdfx.h"

using byte = unsigned char;

// -- Pmxƒwƒbƒ_ --
struct PmxHead
{
	float ver;


};

// -- ’¸“_ --
struct float3
{
	float x, y, z;
};

struct float2
{
	float x, y;
};

struct Vertex
{
	float3 position;//12byte
	float3 normal;	//12byte
	float2 uv;		//8byte
};