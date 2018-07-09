#pragma once

#include <math.h>

#include "common.h"

#define sqrtf32(x) sqrtf(x)
#define sinf32(x) sinf(x)
#define cosf32(x) cosf(x)

/* Types */

typedef union
{
	struct { f32 x, y; };
	struct { f32 u, v; };
} vec2;

typedef union
{
	struct { f32 x, y, z; };
	f32 v[3];
} vec3;

typedef union
{
	struct { f32 w, x, y, z; };
	f32 q[4];
} quat;

typedef struct
{
	f32 M[9];
} mat3;

typedef struct
{
	f32 M[16];
} mat4;

// enum scale_val
// {
// 	MILLIMETERS=-3,
// 	METERS=1,
// 	KILOMETERS=3
// };

// Linked list is used because the transform hierarchy should be pretty static in most cases.
// typedef struct
// {
// 	vec3 pos;
// 	quat rot;
// 	const CoordTransform *parent;
// } CoordTransform;

// @TODO: Implement a proper hierarchy data type to enable tree traversal for relative transforms
/*struct transform_hierarchy
{
	u32 transform_count = 0;
	CoordTransform *transforms;
	u32 *indices;
	u32 *parents;
	u32 *level;
};*/

/* vec3 */

vec3 vec3_add(vec3 a, vec3 b);
vec3 vec3_sub(vec3 a, vec3 b);
vec3 vec3_scalar_mul(vec3 a, f32 s);
vec3 vec3_scalar_div(vec3 a, f32 s);
f32 vec3_dot(vec3 a, vec3 b);
vec3 vec3_cross(vec3 a, vec3 b);
f32 vec3_mag_squared(vec3 a);
f32 vec3_mag(vec3 a);

/* -- quat -- */

quat *quat_null_rotation(quat *q);
quat quat_conjugate(quat q);
quat quat_normalize(quat q);
quat quat_mul(quat a, quat b);
quat quat_from_axis_angle(vec3 axis, float angle);

/* mat3 */

mat3 mat3_transformation(vec2 pos, f32 angle, vec2 scale);

/* mat4 */

mat4 *mat4_init_to_identity(mat4 *a);
mat4 mat4_rotation_matrix_x(f32 alpha);
mat4 mat4_rotation_matrix_y(f32 beta);
mat4 mat4_rotation_matrix_z(f32 gamma);
mat4 mat4_rotation_from_quat(quat q);

// mat4 mat4_transformation(CoordTransform *transform);

mat4 mat4_mul(const mat4 a, const mat4 b);
mat4 mat4_proj_ortho(f32 width, f32 height, f32 near, f32 far);

/* CoordTransform */

// CoordTransform ct_create_child_transform(const CoordTransform *parent)
// {
// 	CoordTransform result = {};
// 	result.parent = parent;
// 	return result;
// }

// // Note: a must be a direct ancestor of b
// // @TODO: More sophisticated 'cousin' version of this (needs proper tree traversal)
// CoordTransform ct_relative_transform(const CoordTransform *a, const CoordTransform *b)
// {
// 	CoordTransform result = {};
// 	result.parent = NULL;
// 	result.pos = vec3_sub(b->pos, a->pos);
// 	const CoordTransform *current = b;
// 	while (current != a) {
// 		result.rot = quat_mul(quat_mul(quat_conjugate((CoordTransform *)(current->parent)->rot), result.rot), ((CoordTransform *)current->parent)->rot);
// 		current = (const CoordTransform *) &current->parent; // @TODO: Do we need to do this cast?
// 	}
// 	return result;
// }

// CoordTransform ct_absolute_transform(const CoordTransform *t)
// {
// 	CoordTransform result = {};
// 	result.parent = NULL;
// 	quat_null_rotation(&result.rot);
// 	const CoordTransform *current = t;
// 	while (current->parent != NULL) {
// 		result.pos = vec3_add(result.pos, ((CoordTransform *)current->parent)->pos);
// 		result.rot = quat_mul(quat_mul(quat_conjugate((CoordTransform *)(current->parent)->rot), result.rot), (CoordTransform *)(current->parent)->rot);
// 		current = (const CoordTransform *) &current->parent; // @TODO: Do we need to do this cast?
// 	}
// 	return result;
// }