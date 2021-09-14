#pragma once

#define QUICK_MATH
#define PI			3.14159265358979323846

class QAngle;
class Vector;
struct matrix3x4_t;
class math : public singleton<math>
{
public:
	static constexpr auto pitch_bounds = 178.f;
	static constexpr auto yaw_bounds = 360.f;
	static constexpr auto roll_bounds = 100.f;

	static void vector_transform( const Vector & in1, const matrix3x4_t & in2, Vector & out );
	QAngle calc_angle( const Vector src, const Vector dst ) const;
	Vector calc_angle_v(const Vector& vecSource, const Vector& vecDestination);
	void vector_angles( const Vector & forward, QAngle & angles ) const;
	void vector_angles_2(const Vector& vecForward, Vector& vecAngles);
	void vector_angles_3(const Vector& forward, QAngle& angles, Vector* up = nullptr);
	void angle_vectors( const QAngle & angles, Vector * forward, Vector *right = nullptr, Vector *up = nullptr ) const;
	void angle_vectors_2(const QAngle& angles, Vector& forward);

	void angle_matrix( const QAngle &angles, const Vector &position, matrix3x4_t& matrix ) const;
	void angle_matrix( const QAngle &angles, matrix3x4_t& matrix ) const;
	void martrix_angles(const matrix3x4_t& matrix, QAngle& angles);
	static void matrix_set_column( const Vector &in, int column, matrix3x4_t& out );
	static void matrix_get_column( const matrix3x4_t & in, int column, Vector & out );
	static void matrix_set_origin( Vector pos, matrix3x4_t & matrix );
	static Vector matrix_get_origin( const matrix3x4_t & src );
	static void matrix_copy( const matrix3x4_t & in, matrix3x4_t & out );

	static void vector_scale( const Vector& in, float scale, Vector& out );
	static void vector_scale( const float * in, float scale, float * out );

	template < typename t >
	static t lerp( const t& t1, const t& t2, float progress )
	{
		return t1 + ( t2 - t1 ) * progress;
	}

	inline static float simple_spline(float value)
	{
		float valueSquared = value * value;

		// Nice little ease-in, ease-out spline-like curve
		return (3 * valueSquared - 2 * valueSquared * value);
	}

	inline static float simple_spline_remap_val_clamped(float val, float A, float B, float C, float D)
	{
		if (A == B)
			return val >= B ? D : C;
		float cVal = (val - A) / (B - A);
		std::clamp(cVal, 0.0f, 1.0f);
		return C + (D - C) * simple_spline(cVal);
	}

	static void vector_ma( const Vector & start, float scale, const Vector & direction, Vector & dest );
	static void vector_i_transform( const Vector& in1, const matrix3x4_t & in2, Vector& out );
	static void vector_i_rotate(const Vector& in1, const matrix3x4_t& in2, Vector& out);
	static bool intersect_line_with_bb(Vector& start, Vector& end, Vector& min, Vector& max);
	Vector vector_rotate(const Vector& in1, const matrix3x4_t& in2);
	Vector vector_rotate(const Vector& in1, const QAngle& in2);
	static float segment_to_segment(const Vector s1, const Vector s2, const Vector k1, const Vector k2);
	static float dot_product( const Vector v1, const Vector v2 );
	static void cross_product( const Vector v1, const Vector v2, Vector & result );

	static bool screen_transform( const Vector & in, Vector & out );
	static bool world_to_screen( const Vector& in, Vector& out );

	void concat_transforms(const matrix3x4_t& in1, const matrix3x4_t& in2, matrix3x4_t& out);

	float get_fov( const QAngle& angles, const QAngle& target ) const;

	inline void sin_cos( float radians, float *sine, float *cosine ) const;
	static inline void fast_sqrt( float * __restrict p_out, float * __restrict p_in );
	float fast_asin( float x ) const;
	float fast_atan2( float y, float x ) const;
	float fast_atan( float x ) const;
	float fast_cos( float x ) const;
	float fast_acos( float x ) const;
	static void fast_rsqrt( float a, float * out );
	static float fast_vec_normalize( Vector & vec );
	float NormalizeFloat(float value);
 	static void normalize(QAngle& angle);
	float fast_sin( float x ) const;

	static float normalize_float( float angle )
	{
		auto revolutions = angle / 360.f;
		if ( angle > 180.f || angle < -180.f )
		{
			revolutions = round( abs( revolutions ) );
			if ( angle < 0.f )
				angle = ( angle + 360.f * revolutions );
			else
				angle = ( angle - 360.f * revolutions );
			return angle;
		}
		return angle;
	}

	// degrees to radians.
	__forceinline constexpr float deg_to_rad(float val)
	{
		return val * (3.14159265358979323846f / 180.f);
	}

	// radians to degrees.
	__forceinline constexpr float rad_to_deg(float val)
	{
		return val * (180.f / 3.14159265358979323846f);
	}

	__forceinline float NormalizedAngle(float angle) {
		normalize_float(angle);
		return angle;
	}

	template < typename t >
	__forceinline void clamp(t& n, const t& lower, const t& upper) {
		n = std::max(lower, std::min(n, upper));
	}
};