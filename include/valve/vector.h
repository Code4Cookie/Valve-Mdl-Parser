//========= Copyright Valve Corporation, All rights reserved. ============//

#pragma once

typedef float vec_t;


class Quaternion				// same data-layout as engine's vec4_t,
{								//		which is a vec_t[4]
public:
	vec_t x, y, z, w;
};

class Vector
{
public:
	Vector() {}

	Vector(vec_t X, vec_t Y, vec_t Z)
	{
		x = X; y = Y; z = Z;
	}

	Vector(vec_t XYZ)
	{
		x = y = z = XYZ;
	}


	vec_t x, y, z;
};



class RadianEuler
{
public:
	vec_t x, y, z;
};

struct matrix3x4_t
{
	matrix3x4_t() {}
	matrix3x4_t(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23)
	{
		m_flMatVal[0][0] = m00;	m_flMatVal[0][1] = m01; m_flMatVal[0][2] = m02; m_flMatVal[0][3] = m03;
		m_flMatVal[1][0] = m10;	m_flMatVal[1][1] = m11; m_flMatVal[1][2] = m12; m_flMatVal[1][3] = m13;
		m_flMatVal[2][0] = m20;	m_flMatVal[2][1] = m21; m_flMatVal[2][2] = m22; m_flMatVal[2][3] = m23;
	}

	float m_flMatVal[3][4];
};