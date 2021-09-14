#pragma once

struct VMatrix
{
	float m[ 4 ][ 4 ];

	inline float* operator[]( int i )
	{
		return m[ i ];
	}

	inline const float* operator[]( int i ) const
	{
		return m[ i ];
	}
};