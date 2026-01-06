#include "math3d.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

fix g_oneOver[ONEOVERTABLE_SIZE];   // Reciprocal table (for max screen height of 256 in Mode 13)
fix g_oneOver16[ONEOVERTABLE_SIZE]; // Reciprocal table (for max screen height of 256 in Mode 13)
fix g_SineTable[SINETABLE_SIZE];    // SIN table. Offset used for COS.

void SetupTables()
{
  int i;
  // Setup reciprocal table
  for (i = 1; i < ONEOVERTABLE_SIZE; ++i)
  {
    g_oneOver[i] = 256.f / i;
    g_oneOver16[i] = 65536.f / i;
  }

  g_oneOver[0] = 256; // Basically a non-op if dividing by zero
  g_oneOver16[0] = 65536;

  // Setup sine table
  for (i = 0; i < SINETABLE_SIZE; ++i)
  {
    g_SineTable[i] = float2fix(sin((float)i * (2.0f * 3.14159265f / (float)SINETABLE_SIZE)));
  }
}

void SetIdentity(MAT43 *mat)
{
  mat->m11 = 256;
  mat->m12 = 0;
  mat->m13 = 0;
  mat->m21 = 0;
  mat->m22 = 256;
  mat->m23 = 0;
  mat->m31 = 0;
  mat->m32 = 0;
  mat->m33 = 256;
  mat->tx = 0;
  mat->ty = 0;
  mat->tz = 0;
}

void SetScale(MAT43 *mat, fix sx, fix sy, fix sz)
{
  mat->m11 = fixmult(mat->m11, sx);
  mat->m22 = fixmult(mat->m22, sy);
  mat->m33 = fixmult(mat->m33, sz);
}

void SetScaleUniversal(MAT43 *mat, fix s)
{
  SetScale(mat, s, s, s);
}

void EulerToMat(MAT43 *mat, int heading, int pitch, int bank)
{
  fix sh, ch, sp, cp, sb, cb;
  sh = fixsin(heading);
  ch = fixcos(heading);
  sp = fixsin(pitch);
  cp = fixcos(pitch);
  sb = fixsin(bank);
  cb = fixcos(bank);

  mat->m11 = fixmultSML(ch, cb) + fixmultSML(fixmultSML(sh, sp), sb);
  mat->m12 = fixmultSML(-ch, sb) + fixmultSML(fixmultSML(sh, sp), cb);
  mat->m13 = fixmultSML(sh, cp);

  mat->m21 = fixmultSML(sb, cp);
  mat->m22 = fixmultSML(cb, cp);
  mat->m23 = -sp;

  mat->m31 = fixmultSML(-sh, cb) + fixmultSML(fixmultSML(ch, sp), sb);
  mat->m32 = fixmultSML(sb, sh) + fixmultSML(fixmultSML(ch, sp), cb);
  mat->m33 = fixmultSML(ch, cp);

  mat->tx = mat->ty = mat->tz = 0;
}

void RotateAxis(MAT43 *mat, V3D *axis, int angle)
{
  fix s, c, a, ax, ay, az;
  s = fixsin(angle);
  c = fixcos(angle);
  a = 256 - c;
  ax = fixmult(a, axis->x);
  ay = fixmult(a, axis->y);
  az = fixmult(a, axis->z);

  mat->m11 = fixmult(ax, axis->x) + c;
  mat->m12 = fixmult(ax, axis->y) + fixmult(s, axis->z);
  mat->m13 = fixmult(ax, axis->z) - fixmult(s, axis->y);
  mat->m21 = fixmult(ay, axis->x) - fixmult(s, axis->z);
  mat->m22 = fixmult(ay, axis->y) + c;
  mat->m23 = fixmult(ay, axis->z) + fixmult(s, axis->x);
  mat->m31 = fixmult(az, axis->x) + fixmult(s, axis->y);
  mat->m32 = fixmult(az, axis->y) - fixmult(s, axis->x);
  mat->m33 = fixmult(az, axis->z) + c;

  mat->tx = mat->ty = mat->tz = 0;
}

void RotateX(MAT43 *mat, int angle)
{
  fix s, c;
  s = fixsin(angle);
  c = fixcos(angle);

  mat->m11 = 256;
  mat->m12 = 0;
  mat->m13 = 0;
  mat->m21 = 0;
  mat->m22 = c;
  mat->m23 = s;
  mat->m31 = 0;
  mat->m32 = -s;
  mat->m33 = c;
  mat->tx = mat->ty = mat->tz = 0;
}

void RotateY(MAT43 *mat, int angle)
{
  fix s, c;
  s = fixsin(angle);
  c = fixcos(angle);

  mat->m11 = c;
  mat->m12 = 0;
  mat->m13 = -s;
  mat->m21 = 0;
  mat->m22 = 256;
  mat->m23 = 0;
  mat->m31 = s;
  mat->m32 = 0;
  mat->m33 = c;
  mat->tx = mat->ty = mat->tz = 0;
}

void Normal(V3D *a, V3D *b, V3D *c, V3D *n)
{
  V3D v1, v2;
  v1.x = a->x - b->x;
  v1.y = a->y - b->y;
  v1.z = a->z - b->z;
  v2.x = a->x - c->x;
  v2.y = a->y - c->y;
  v2.z = a->z - c->z;

  n->x = fixmult(v1.y, v2.z) - fixmult(v1.z, v2.y);
  n->y = fixmult(v1.z, v2.x) - fixmult(v1.x, v2.z);
  n->z = fixmult(v1.x, v2.y) - fixmult(v1.y, v2.x);
}

void Normalize(V3D *v)
{
  double len, x, y, z;
  x = fix2float(v->x);
  y = fix2float(v->y);
  z = fix2float(v->z);
  len = sqrt(x * x + y * y + z * z);

  v->x = float2fix((float)(x / len));
  v->y = float2fix((float)(y / len));
  v->z = float2fix((float)(z / len));
}

fix DotProduct(const V3D *v1, const V3D *v2)
{
  return fixmultSML(v1->x, v2->x) + fixmultSML(v1->y, v2->y) + fixmultSML(v1->z, v2->z);
}

void MultMatMat(MAT43 *dest, MAT43 *a, MAT43 *b)
{
  MAT43 tmp;
  tmp.m11 = fixmult(a->m11, b->m11) + fixmult(a->m12, b->m21) + fixmult(a->m13, b->m31);
  tmp.m12 = fixmult(a->m11, b->m12) + fixmult(a->m12, b->m22) + fixmult(a->m13, b->m32);
  tmp.m13 = fixmult(a->m11, b->m13) + fixmult(a->m12, b->m23) + fixmult(a->m13, b->m33);

  tmp.m21 = fixmult(a->m21, b->m11) + fixmult(a->m22, b->m21) + fixmult(a->m23, b->m31);
  tmp.m22 = fixmult(a->m21, b->m12) + fixmult(a->m22, b->m22) + fixmult(a->m23, b->m32);
  tmp.m23 = fixmult(a->m21, b->m13) + fixmult(a->m22, b->m23) + fixmult(a->m23, b->m33);

  tmp.m31 = fixmult(a->m31, b->m11) + fixmult(a->m32, b->m21) + fixmult(a->m33, b->m31);
  tmp.m32 = fixmult(a->m31, b->m12) + fixmult(a->m32, b->m22) + fixmult(a->m33, b->m32);
  tmp.m33 = fixmult(a->m31, b->m13) + fixmult(a->m32, b->m23) + fixmult(a->m33, b->m33);

  // Translation
  tmp.tx = fixmult(a->tx, b->m11) + fixmult(a->ty, b->m21) + fixmult(a->tz, b->m31) + b->tx;
  tmp.ty = fixmult(a->tx, b->m12) + fixmult(a->ty, b->m22) + fixmult(a->tz, b->m32) + b->ty;
  tmp.tz = fixmult(a->tx, b->m13) + fixmult(a->ty, b->m23) + fixmult(a->tz, b->m33) + b->tz;

  *dest = tmp;
}

void MultV3DMat(V3D *v, V3D *dest, MAT43 *mat)
{
  dest->x = fixmultSML(v->x, mat->m11) + fixmultSML(v->y, mat->m12) + fixmultSML(v->z, mat->m13) +
            mat->tx;
  dest->y = fixmultSML(v->x, mat->m21) + fixmultSML(v->y, mat->m22) + fixmultSML(v->z, mat->m23) +
            mat->ty;
  dest->z = fixmultSML(v->x, mat->m31) + fixmultSML(v->y, mat->m32) + fixmultSML(v->z, mat->m33) +
            mat->tz;
}

void MultV4DMatC(V4D *v, V4D *dest, MAT43 *mat)
{
  dest->x = fixmultSML(v->x, mat->m11) + fixmultSML(v->y, mat->m12) + fixmultSML(v->z, mat->m13) +
            mat->tx;
  dest->y = fixmultSML(v->x, mat->m21) + fixmultSML(v->y, mat->m22) + fixmultSML(v->z, mat->m23) +
            mat->ty;
  dest->z = fixmultSML(v->x, mat->m31) + fixmultSML(v->y, mat->m32) + fixmultSML(v->z, mat->m33) +
            mat->tz;
}

void MultV4DMatCToScreen(V4D *v, V4D *dest, MAT43 *mat)
{
  dest->x =
      fixmultSML(v->x, mat->m11) + fixmultSML(v->y, mat->m12) + fixmultSML(v->z, mat->m13) + 128;
  dest->x >>= 8;
  dest->y =
      fixmultSML(v->x, mat->m21) + fixmultSML(v->y, mat->m22) + fixmultSML(v->z, mat->m23) + 128;
  dest->y >>= 8;
  dest->z =
      fixmultSML(v->x, mat->m31) + fixmultSML(v->y, mat->m32) + fixmultSML(v->z, mat->m33) + 128;
  dest->z >>= 8;
}

V3D SubV3D(const V3D *a, const V3D *b)
{
  V3D result;
  result.x = a->x - b->x;
  result.y = a->y - b->y;
  result.z = a->z - b->z;
  return result;
}

V3D CrossProductV3D(const V3D *a, const V3D *b)
{
  V3D result;
  result.x = fixmult(a->y, b->z) - fixmult(a->z, b->y);
  result.y = fixmult(a->z, b->x) - fixmult(a->x, b->z);
  result.z = fixmult(a->x, b->y) - fixmult(a->y, b->x);
  return result;
}

void LookAt(const V3D *eyePos, const V3D *forward, MAT43 *mat)
{
  // Calculate the forward vector (direction from eye to target)
  V3D up, right;

  // Define the up vector (world's up)
  up.x = 0;
  up.y = 256;
  up.z = 0;

  // Calculate the right vector (perpendicular to forward and up)
  right = CrossProductV3D(&up, forward);
  up = CrossProductV3D(forward, &right);

  mat->tx = -DotProduct(&right, eyePos);
  mat->ty = -DotProduct(&up, eyePos);
  mat->tz = -DotProduct(forward, eyePos);

  // Fill in the matrix values
  mat->m11 = right.x;
  mat->m12 = right.y;
  mat->m13 = right.z;

  mat->m21 = up.x;
  mat->m22 = up.y;
  mat->m23 = up.z;

  mat->m31 = forward->x;
  mat->m32 = forward->y;
  mat->m33 = forward->z;
}

void PerspectiveProjection(MAT44 *mat, float fov, float aspect, float znear, float zfar)
{
  float yScale = 1.f / tan(fov / 2.f);
  float xScale = yScale * aspect;

  mat->m11 = float2fix(xScale);
  mat->m12 = 0;
  mat->m13 = 0;
  mat->m14 = 0;

  mat->m21 = 0;
  mat->m22 = float2fix(yScale);
  mat->m23 = 0;
  mat->m24 = 0;

  mat->m31 = 0;
  mat->m32 = 0;
  mat->m33 = float2fix(zfar / (zfar - znear));
  mat->m34 = 256;

  mat->m41 = 0;
  mat->m42 = 0;
  mat->m43 = float2fix(znear * zfar / (zfar - znear));
  mat->m44 = 0;
}
