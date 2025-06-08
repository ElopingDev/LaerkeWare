#pragma once
#include <cstdint>
#include "geom/geom.h"

class Player
{
public:
	void* N0000004F; //0x0000
	//class N0000018F N00000050; //0x0004
	char pad_0008[228]; //0x0008
	int32_t Health; //0x00EC
	char pad_00F0[848]; //0x00F0

	
}; //Size: 0x0440

class Entity
{
public:
	char pad_0000[56]; //0x0000
	Vector3 pos; //0x0038
	char pad_0044[160]; //0x0044
	int32_t N0000023E; //0x00E4
	int32_t maxHealth; //0x00E8
	int32_t Health; //0x00EC
	uint32_t Flags; //0x00F0
	char pad_00F4[792]; //0x00F4

	std::string getEntityName();

}; //Size: 0x040C
