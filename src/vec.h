#ifndef VEC_H
#define VEC_H

struct Vec2i {
	int x,y;
};

struct Vec2f
{
	float x,y;
};

bool operator==(Vec2f a, Vec2f b);
Vec2f operator+(Vec2f a, Vec2f b);
Vec2f operator+=(Vec2f &a, Vec2f b);
Vec2f operator-(Vec2f v);
Vec2f operator-(Vec2f a, Vec2f b);
Vec2f operator-=(Vec2f &a, Vec2f b);
Vec2f operator*(float c, Vec2f a);
Vec2f operator*(Vec2f a, Vec2f b);
Vec2f Normalize(Vec2f v);
float Length(Vec2f v);
float Distance(Vec2f a, Vec2f b);

struct Vec3f
{
	float x,y,z;
};

Vec3f operator+(Vec3f a, Vec3f b);
Vec3f operator+=(Vec3f &a, Vec3f b);
Vec3f operator-(Vec3f v);
Vec3f operator-(Vec3f a, Vec3f b);
Vec3f operator-=(Vec3f &a, Vec3f b);
Vec3f operator*(float c, Vec3f a);

Vec3f OrthonormalVector(Vec3f v);
Vec3f Cross(Vec3f a, Vec3f b);
Vec3f Normalize(Vec3f v);
float Length(Vec3f v);
float Distance(Vec3f a, Vec3f b);

struct Vec4f {
	float x,y,z,w;
};

Vec4f operator*(float c, Vec4f a);

struct Mat3f {
	float xx, xy, xz;
	float yx, yy, yz;
	float zx, zy, zz;
};

Mat3f operator*(Mat3f a, Mat3f b);
Vec3f operator*(Mat3f mat, Vec3f vec);

struct Mat4f {
	float xx, xy, xz, xw;
	float yx, yy, yz, yw;
	float zx, zy, zz, zw;
	float wx, wy, wz, ww;
};

Vec4f operator*(Mat4f mat, Vec4f vec);
Mat4f operator*(Mat4f a, Mat4f b);
Mat4f transpose(Mat4f a);

struct Rect
{
	Vec2f pos;
	Vec2f size;
};

bool PointInRect(Rect rect, Vec2f point);

#endif