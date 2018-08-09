#include "maths.h"

vec3 vec3_new(f32 x, f32 y, f32 z)
{
	vec3 result = {x, y, z};
	return result;
}

vec3 vec3_add(vec3 a, vec3 b)
{
	vec3 result = {
		a.x + b.x,
		a.y + b.y,
		a.z - b.z
	};
	return result;
}

vec3 vec3_sub(vec3 a, vec3 b)
{
	vec3 result = {
		a.x - b.x,
		a.y - b.y,
		a.z - b.z
	};
	return result;
}

vec3 vec3_scalar_mul(vec3 a, f32 s)
{
	vec3 result = {
		a.x * s,
		a.y * s,
		a.z * s
	};
	return result;
}

vec3 vec3_scalar_div(vec3 a, f32 s)
{
	vec3 result = {
		a.x / s,
		a.y / s,
		a.z / s
	};
	return result;
}

f32 vec3_dot(vec3 a, vec3 b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

vec3 vec3_cross(vec3 a, vec3 b)
{
	vec3 result = {
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
	};
	return result;
}

f32 vec3_mag_squared(vec3 a)
{
	return vec3_dot(a, a);
}

f32 vec3_mag(vec3 a)
{
	return sqrtf32(vec3_mag_squared(a));
}

vec4 vec4_zero()
{
	vec4 result = {0, 0, 0, 0};
	return result;
}

/* -- quat -- */

quat quat_null_rotation()
{
	quat result = {1, 0, 0, 0};
	return result;
}

quat quat_conjugate(quat q)
{
	quat result = {};
	result.w = q.w;
	result.x = -q.x;
	result.y = -q.y;
	result.z = -q.z;
	return result;
}

quat quat_normalize(quat q)
{
	quat result;
	f32 mag = sqrtf32(q.w * q.w * q.x * q.x + q.y + q.y + q.z * q.z); 
	result.w = q.w / mag;
	result.x = q.x / mag;
	result.y = q.y / mag;
	result.z = q.z / mag;
	return result;
}

quat quat_mul(quat a, quat b)
{
	quat result = {};
	result.w = a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z;
	result.x = a.x * b.w + a.y * b.z - a.z * b.y + a.w * b.x;
	result.y = a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x;
	result.z = a.x * b.y - a.y * b.x + a.z * b.w + a.w * b.z;
	return result;
}

vec3 quat_rotate(quat q, vec3 v)
{
	quat q_v = {
		-q.x * v.x - q.y * v.y - q.z * v.z,
		q.w * v.x + q.y * v.z - q.z * v.y,
		q.w * v.y + q.z * v.x - q.x * v.z,
		q.w * v.z + q.x * v.y - q.y * v.x
	};

	quat q_v_qstar = {
		q_v.w * q.w + q_v.x * q.x + q_v.y * q.y + q_v.z * q.z,
		q_v.x * q.w - q_v.w * q.x - q_v.y * q.z + q_v.z * q.y,
		q_v.y * q.w - q_v.w * q.y - q_v.z * q.x + q_v.x * q.z,
		q_v.z * q.w - q_v.w * q.z - q_v.x * q.y + q_v.y * q.x
	};

	vec3 result = {q_v_qstar.x, q_v_qstar.y, q_v_qstar.z};
	return result;
}

quat quat_from_axis_angle(vec3 axis, float angle)
{
	quat result = {};

	float sin_half_angle = sinf32(0.5f * angle);
	float cos_half_angle = cosf32(0.5f * angle);

	result.w = cos_half_angle;
	result.x = axis.x * sin_half_angle;
	result.y = axis.y * sin_half_angle;
	result.z = axis.z * sin_half_angle;

	return result;
}

/* mat3 */

mat3 mat3_transformation(vec2 pos, f32 angle, vec2 scale)
{
	f32 s = sinf32(angle);
	f32 c = cosf32(angle);
	mat3 result = {
		  c * scale.x, s,           pos.x,
		 -s,           c * scale.y, pos.y,
		  0,           0,           1
	};
	return result;
}

/* mat4 */

mat4 mat4_identity()
{
	mat4 result =  {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	return result;
}

mat4 *mat4_init_to_identity(mat4 *a)
{
	for (u32 i = 0; i < 16; i++) {
		a->M[i] = i % 4 == 0 ? 1 : 0;
	}
	return a;
}

mat4 mat4_translation(vec3 pos)
{
	mat4 result = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		pos.x, pos.y, pos.z, 1
	};
	return result;
}

mat4 mat4_rotation_x(f32 alpha)
{
	f32 s = sinf32(alpha);
	f32 c = cosf32(alpha);
	mat4 result =  {
		1,  0, 0, 0,
		0,  c, s, 0,
		0, -s, c, 0,
		0,  0, 0, 1
	};
	return result;
}

mat4 mat4_rotation_y(f32 beta)
{
	f32 s = sinf32(beta);
	f32 c = cosf32(beta);
	mat4 result =  {
		c, 0, -s, 0,
		0, 1,  0, 0,
		s, 0,  c, 0,
		0, 0,  0, 1
	};
	return result;
}

mat4 mat4_rotation_z(f32 gamma)
{
	f32 s = sinf32(gamma);
	f32 c = cosf32(gamma);
	mat4 result = {
		 c, s, 0, 0,
		-s, c, 0, 0,
		 0, 0, 1, 0,
		 0, 0, 0, 1
	};
	return result;
}

mat4 mat4_rotation_from_quat(quat q)
{
	static const vec3 z = {0, 0, 1};
	static const vec3 y = {0, 1, 0};
	static const vec3 x = {1, 0, 0};

	const vec3 f = quat_rotate(q, z);
	const vec3 u = quat_rotate(q, y);
	const vec3 r = quat_rotate(q, x);

	mat4 result = {
		r.x, r.y, r.z, 0,
		u.x, u.y, u.z, 0,
		f.x, f.y, f.z, 0,
		0, 0, 0, 1
	};

	return result;
}

mat4 mat4_scale(vec3 scale)
{
	mat4 result = {
		scale.x, 0, 0, 0,
		0, scale.y, 0, 0,
		0, 0, scale.z, 0,
		0, 0, 0, 1
	};
	return result;
}

mat4 mat4_mul(const mat4 a, const mat4 b)
{
	mat4 result = {};
	for (u32 j = 0; j < 4; j++) {
		for (u32 i = 0; i < 4; i++) {
			result.M[i + j * 4] = a.M[0 + j * 4] * b.M[i + 0 * 4]
								+ a.M[1 + j * 4] * b.M[i + 1 * 4]
								+ a.M[2 + j * 4] * b.M[i + 2 * 4]
								+ a.M[3 + j * 4] * b.M[i + 3 * 4];
		}
	}	
	return result;
}

mat4 mat4_ortho(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far)
{
	mat4 result = {
		2.0f / (right - left), 0, 0, 0,
		0, 2.0f / (top - bottom), 0, 0,
		0, 0, 2.0f / (near - far), 0,
		(left + right) / (left - right), (bottom + top) / (bottom - top), (far + near) / (near - far), 1
	};
	return result;
}

mat4 mat4_perspective(f32 fov, f32 aspect_ratio, f32 near, f32 far)
{
	const f32 q = 1.0f / tanf(MATH_PI / 180.0f * (0.5f * fov));
	const f32 a = q / aspect_ratio;

	const f32 b = (near + far) / (near - far);
	const f32 c = (2.0f * near * far) / (near - far);

	mat4 result = {
		a, 0, 0, 0,
		0, q, 0, 0,
		0, 0, b, -1,
		0, c, 0, 1
	};

	// result.m[0 + 0 * 4] = a;
	// result.m[1 + 1 * 4] = q;
	// result.m[2 + 2 * 4] = b;
	// result.m[3 + 2 * 4] = (T)-1;
	// result.m[2 + 3 * 4] = c;
		// (*this)[0][0] = 1.0f/(tan_half_fov * aspect_ratio); (*this)[1][0] = T(0);   (*this)[2][0] = T(0);            (*this)[3][0] = T(0);
		// (*this)[0][1] = T(0);                   (*this)[1][1] = T(1)/tan_half_fov; (*this)[2][1] = T(0);            (*this)[3][1] = T(0);
		// (*this)[0][2] = T(0);                   (*this)[1][2] = T(0);            (*this)[2][2] = (-near - far)/z_range ; (*this)[3][2] = T(2)*far*near/z_range;
		// (*this)[0][3] = T(0);                   (*this)[1][3] = T(0);            (*this)[2][3] = T(1);            (*this)[3][3] = T(0); 
	return result;
}

mat4 mat4_transformation(const Transform *transform)
{
	mat4 result = mat4_mul(mat4_mul(mat4_rotation_from_quat(transform->rot),
									mat4_translation(transform->pos)),
									mat4_scale(transform->scale));
	return result;
}