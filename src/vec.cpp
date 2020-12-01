#include "vec.h"
#include "game.h"

#include "util.h"
#include "math.h"

// Vec2f
bool
operator==(Vec2f a, Vec2f b)
{
    return(a.x==b.x && a.y==b.y);
}

Vec2f
operator+(Vec2f a, Vec2f b)
{
    return {a.x+b.x,a.y+b.y};
}

Vec2f
operator+=(Vec2f &a, Vec2f b)
{
    a = a+b;
    return a;
}

Vec2f
operator-(Vec2f v)
{
    return {-v.x,-v.y};
}

Vec2f
operator-(Vec2f a, Vec2f b)
{
    return {a.x-b.x,a.y-b.y};
}

Vec2f
operator-=(Vec2f &a, Vec2f b)
{
    a = a-b;
    return a;
}

Vec2f
operator*(float c, Vec2f a)
{
    return {c*a.x,c*a.y};
}

Vec2f
operator/(Vec2f a, float c)
{
    return {a.x/c,a.y/c};
}


// Element-wise multiplication
Vec2f
operator*(Vec2f a, Vec2f b)
{
    return {a.x*b.x, a.y*b.y};
}

Vec2f
operator*=(Vec2f &v, float c)
{
    v = c*v;
    return v;
}

Vec2f
Round(Vec2f v)
{
    return {float(int(v.x+0.5f)), float(int(v.y+0.5f))};
}

float
Length(Vec2f v)
{
    return m::Sqrt(v.x*v.x+v.y*v.y);
}

Vec2f
Normalize(Vec2f v)
{
    return (1/Length(v))*v;
}

float
Distance(Vec2f a, Vec2f b)
{
    return Length(a-b);
}

Vec2f
Rotate(Vec2f v, float degrees)
{
    float rad = degrees * (c::pi / 180.f);

    return {
        .x = v.x * m::Cos(rad) - v.y * m::Sin(rad),
        .y = v.x * m::Sin(rad) + v.y * m::Cos(rad)
    };
}

// Vec3f
Vec3f
operator+(Vec3f a, Vec3f b)
{
    return {a.x+b.x, a.y+b.y, a.z+b.z};
}

Vec3f
operator+=(Vec3f &a, Vec3f b)
{
    a = a + b;
    return a;
}

Vec3f
operator-(Vec3f v)
{
    return {-v.x,-v.y,-v.z};
}

Vec3f
operator-(Vec3f a, Vec3f b)
{
    return {a.x-b.x, a.y-b.y, a.z-b.z};
}

Vec3f
operator-=(Vec3f &a, Vec3f b)
{
    a = a - b;
    return a;
}

Vec3f
operator*(float c, Vec3f a)
{
    return {c*a.x, c*a.y, c*a.z};
}

Vec3f
operator*=(Vec3f v, float c)
{
    return c*v;
}

float
Dot(Vec3f a, Vec3f b)
{
    return a.x*b.x+a.y*b.y+a.z*b.z;
}

Vec3f
Cross(Vec3f a, Vec3f b) {
    return {
        a.y*b.z - a.z*b.y,
        a.z*b.x - a.x*b.z,
        a.x*b.y - a.y*b.x
    };
}

float
Length(Vec3f v)
{
    return m::Sqrt(v.x*v.x+v.y*v.y+v.z*v.z);
}

float
Distance(Vec3f a, Vec3f b)
{
    return Length(a-b);
}

Vec3f Normalize(Vec3f v)
{
    return (1/Length(v))*v;
}

Vec3f
OrthonormalVector(Vec3f v)
{
    Vec3f ortho;

    if(v.x<=v.y and v.x<=v.z)
    { // X is smallest (abs) normal vector component
        ortho = {0.f, -v.z, v.y};
    }
    else if(v.y<=v.x and v.y<=v.z)
    { // Y is smallest (abs) normal vector component
        ortho = {-v.z, 0.f, v.x};
    }
    else
    { // Z is smallest (abs) normal vector component
        ortho = {-v.y, v.x, 0.f};
    }

    return Normalize(ortho);
}

Vec4f
operator*(float c, Vec4f a)
{
    return {c*a.x, c*a.y, c*a.z, c*a.w};
}

Mat3f
operator*(Mat3f a, Mat3f b)
{
    return {
        a.xx*b.xx + a.xy*b.yx + a.xz*b.zx,
        a.xx*b.xy + a.xy*b.yy + a.xz*b.zy,
        a.xx*b.xz + a.xy*b.yz + a.xz*b.zz,
        a.yx*b.xx + a.yy*b.yx + a.yz*b.zx,
        a.yx*b.xy + a.yy*b.yy + a.yz*b.zy,
        a.yx*b.xz + a.yy*b.yz + a.yz*b.zz,
        a.zx*b.xx + a.zy*b.yx + a.zz*b.zx,
        a.zx*b.xy + a.zy*b.yy + a.zz*b.zy,
        a.zx*b.xz + a.zy*b.yz + a.zz*b.zz,
    };
}

Vec3f
operator*(Mat3f mat, Vec3f vec)
{
    return {
        mat.xx*vec.x + mat.xy*vec.y + mat.xz*vec.z,
        mat.yx*vec.x + mat.yy*vec.y + mat.yz*vec.z,
        mat.zx*vec.x + mat.zy*vec.y + mat.zz*vec.z
    };
}

Vec4f
operator*(Mat4f mat, Vec4f vec)
{
    return {
        mat.xx*vec.x + mat.xy*vec.y + mat.xz*vec.z + mat.xw*vec.w,
        mat.yx*vec.x + mat.yy*vec.y + mat.yz*vec.z + mat.yw*vec.w,
        mat.zx*vec.x + mat.zy*vec.y + mat.zz*vec.z + mat.zw*vec.w,
        mat.wx*vec.x + mat.wy*vec.y + mat.wz*vec.z + mat.ww*vec.w
    };
}

Mat4f
operator*(Mat4f a, Mat4f b)
{
    return {
        a.xx*b.xx + a.xy*b.yx + a.xz*b.zx + a.xw*b.wx,
        a.xx*b.xy + a.xy*b.yy + a.xz*b.zy + a.xw*b.wy,
        a.xx*b.xz + a.xy*b.yz + a.xz*b.zz + a.xw*b.wz,
        a.xx*b.xw + a.xy*b.yw + a.xz*b.zw + a.xw*b.ww,

        a.yx*b.xx + a.yy*b.yx + a.yz*b.zx + a.yw*b.wx,
        a.yx*b.xy + a.yy*b.yy + a.yz*b.zy + a.yw*b.wy,
        a.yx*b.xz + a.yy*b.yz + a.yz*b.zz + a.yw*b.wz,
        a.yx*b.xw + a.yy*b.yw + a.yz*b.zw + a.yw*b.ww,

        a.zx*b.xx + a.zy*b.yx + a.zz*b.zx + a.zw*b.wx,
        a.zx*b.xy + a.zy*b.yy + a.zz*b.zy + a.zw*b.wy,
        a.zx*b.xz + a.zy*b.yz + a.zz*b.zz + a.zw*b.wz,
        a.zx*b.xw + a.zy*b.yw + a.zz*b.zw + a.zw*b.ww,

        a.wx*b.xx + a.wy*b.yx + a.wz*b.zx + a.ww*b.wx,
        a.wx*b.xy + a.wy*b.yy + a.wz*b.zy + a.ww*b.wy,
        a.wx*b.xz + a.wy*b.yz + a.wz*b.zz + a.ww*b.wz,
        a.wx*b.xw + a.wy*b.yw + a.wz*b.zw + a.ww*b.ww
    };
}

Mat4f
transpose(Mat4f a)
{
    return {
        a.xx, a.yx, a.zx, a.wx,
        a.xy, a.yy, a.zy, a.wy,
        a.xz, a.yz, a.zz, a.wz,
        a.xw, a.yw, a.zw, a.ww
    };
}

Mat4f
FacingMatrix(Vec3f f)
{
    Vec3f r = OrthonormalVector(f);
    Vec3f u = Cross(r,f);
    return {
        f.x, u.x, r.x, 0.f,
        f.y, u.y, r.y, 0.f,
        f.z, u.z, r.z, 0.f,
        0.f, 0.f, 0.f, 1.f
    };
}

bool PointInRect(Rect rect, Vec2f point)
{
    return( point.x >= rect.pos.x and
            point.x < rect.pos.x + rect.size.x and
            point.y >= rect.pos.y and
            point.y < rect.pos.y + rect.size.y);
}

float
RectLeft(Rect rect)
{
    return rect.pos.x;
}

float
RectRight(Rect rect)
{
    return rect.pos.x + rect.size.x;
}

float
RectTop(Rect rect)
{
    return rect.pos.y;
}

float
RectBottom(Rect rect)
{
    return rect.pos.y + rect.size.y;
}

Vec2f
RectTopLeft(Rect rect)
{
    return rect.pos;
}

Vec2f
RectTopCenter(Rect rect)
{
    return rect.pos + Vec2f{0.5f*rect.size.x, 0.f};
}

Vec2f
RectTopRight(Rect rect)
{
    return rect.pos + Vec2f{rect.size.x, 0.f};
}

Vec2f
RectCenter(Rect rect)
{
    return rect.pos + 0.5f*rect.size;
}

Vec2f
RectRightCenter(Rect rect)
{
    return rect.pos + Vec2f{1.f*rect.size.x, 0.5f*rect.size.y};
}

Vec2f
RectBottomLeft(Rect rect)
{
    return rect.pos + Vec2f{0.f, rect.size.y};
}

Vec2f
RectBottomRight(Rect rect)
{
    return rect.pos + rect.size;
}

Vec2f
RectBottomCenter(Rect rect)
{
    return rect.pos + Vec2f{0.5f*rect.size.x, 1.f*rect.size.y};
}

Rect
RectLerp(Rect initial, Rect final, float t)
{
    Rect lerped_rect = {};
    Vec2f top_left     = (1.f - t)*RectTopLeft(initial)     + (t)*RectTopLeft(final);
    Vec2f bottom_right = (1.f - t)*RectBottomRight(initial) + (t)*RectBottomRight(final);

    lerped_rect.pos = top_left;
    lerped_rect.size = bottom_right - top_left;

    return lerped_rect;
}

Vec2f
ScreenPointToWorldPoint(Camera camera, Vec2f screen_pt)
{
    // AR_screen = AspectRatio(game->window_size);
    // AR_camera = AspectRatio(game->camera_view);
    // float scale = AR_camera / AR_screen;

    // x_ratio = game->camera_view.x / game->window_size.x;
    // y_ratio = game->camera_view.y / game->window_size.y;

    Vec2f world_pt = Vec2f{
        .x = (camera.view.x / game->window_size.x) * screen_pt.x + (camera.pos.x - 0.5f*camera.view.x),
        .y = (camera.view.y / game->window_size.y) * screen_pt.y + (camera.pos.y - 0.5f*camera.view.y)
    };

    return world_pt;
}

Vec2f
WorldPointToScreenPoint(Camera camera, Vec2f world_pt)
{
    Vec2f screen_pt = {
        .x = (game->window_size.x / camera.view.x) * (world_pt.x - camera.pos.x + 0.5f*camera.view.x),
        .y = (game->window_size.y / camera.view.y) * (world_pt.y - camera.pos.y + 0.5f*camera.view.y),
    };
    return screen_pt;
}

Vec2f
Lerp(Vec2f start, Vec2f end, float t)
{
    Vec2f lerped = Vec2f{m::Lerp(start.x, end.x, t),
                         m::Lerp(start.y, end.y, t)};

    return lerped;
}

float
AspectRatio(Vec2f v)
{
    return v.x / v.y;
}

float
AspectRatio(Rect rect)
{
    return AspectRatio(rect.size);
}

Vec2f
PadToAspectRatio(Vec2f vec, float target_AR)
{
    float AR = AspectRatio(vec);
    if(AR == target_AR)
    {
        return vec;
    }
    if(AR >= target_AR)
    { // vec is too large in the x direction, so pad in the y divecion.
        Vec2f adjusted_vec = vec;
        adjusted_vec.y = (AR/target_AR)*vec.y;
        return adjusted_vec;
    }
    else
    { // vec is too large in the y direction, so pad in the x divecion.
        Vec2f adjusted_vec = vec;
        adjusted_vec.x = (target_AR/AR)*vec.x;
        return adjusted_vec;
    }
}