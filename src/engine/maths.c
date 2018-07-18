#include "maths.h"

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

/* -- quat -- */

quat *quat_null_rotation(quat *q)
{
	q->w = 1.0f;
	q->x = 0.0f;
	q->y = 0.0f;
	q->z = 0.0f;
	return q;
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

mat4 mat4_rotation_matrix_x(f32 alpha)
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

mat4 mat4_rotation_matrix_y(f32 beta)
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

mat4 mat4_rotation_matrix_z(f32 gamma)
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

mat4 mat4_rotation_from_quat(quat q) // @TODO: Missing
{
	mat4 result = {};
	return result;
}

// mat4 mat4_transformation(CoordTransform *transform)
// {
// 	mat4 result = mat4_rotation_from_quat(transform->rot);
// 	result.M[3 * 4 + 0] = transform->pos.x;
// 	result.M[3 * 4 + 1] = transform->pos.y;
// 	result.M[3 * 4 + 2] = transform->pos.z;
// 	return result;
// }

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

mat4 mat4_proj_ortho(f32 width, f32 height, f32 near, f32 far)
{
	mat4 result = {
		2.0f/width, 0, 0, 0,
		0, 2.0f/height, 0, 0,
		0, 0, -2.0f/(far-near), -(far+near)/(far-near),
		0, 0, 0, 1
	};
	return result;
}