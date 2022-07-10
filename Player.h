#pragma once
// Generated using ReClass 2016

class Player;

class Player
{
public:
	char pad_0x0000[0x4]; //0x0000
	unsigned char KeyPressedUpDown; //0x0004 
	unsigned char KeyPressedLeftRight; //0x0005 
	__int16 camAngle; //0x0006 
	char padding[2]; //0x0008 
	unsigned char isSpinning; //0x000A 
	unsigned char isJumping; //0x000B 
	unsigned char isPaused; //0x000C 
	char pad_0x000D[0xF]; //0x000D
	__int32 yPos; //0x001C 
	char pad_0x0020[0x1C]; //0x0020
	__int32 rings; //0x003C 
	char pad_0x0040[0x54]; //0x0040
	__int32 isJumping2; //0x0094 
	unsigned char isMoving; //0x0098 
	char pad_0x0099[0x5F]; //0x0099
	__int32 Velocity; //0x00F8 
	char pad_0x00FC[0x28]; //0x00FC
	__int32 relativeVel1; //0x0124 
	__int32 relativeVel2; //0x0128 
	char pad_0x012C[0x4]; //0x012C
	__int32 time; //0x0130 

}; //Size=0x0440
