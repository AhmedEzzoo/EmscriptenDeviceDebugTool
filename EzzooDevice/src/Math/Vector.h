#pragma once

#include "glm/glm.hpp"
#include <ostream>

namespace Ezzoo {

struct Vector2f {
  union {
    float x = 0.0f;
    float r;
  };

  union {
    float y = 0.0f;
    float g;
  };

  Vector2f() {}

  Vector2f(float _x, float _y) {
    x = _x;
    y = _y;
  }

  Vector2f(const Vector2f &vector) {
    x = vector.x;
    y = vector.y;
  }

  float Length() {
    float length = sqrtf(x * x + y * y);

    return length;
  }

  Vector2f &Normalize(const Vector2f &vec) {
    float len = Length();

    assert(len != 0);

    x /= len;
    y /= len;

    return *this;
  }

  float Distance(const Vector2f &vec) {
    float delta_x = x - vec.x;
    float delta_y = y - vec.y;

    float dis = sqrtf(delta_x * delta_x + delta_y * delta_y);

    return dis;
  }

  Vector2f &operator*=(float factor) {
    x *= factor;
    y *= factor;

    return *this;
  }

  Vector2f &operator*=(const Vector2f &vector) {

    x *= vector.x;
    y *= vector.y;

    return *this;
  }

  Vector2f &operator+=(const Vector2f &vector) {

    x += vector.x;
    y += vector.y;

    return *this;
  }

  Vector2f &operator-=(const Vector2f &vector) {
    x -= vector.x;
    y -= vector.y;

    return *this;
  }
};

// std::ostream &operator<<(std::ostream &os, const Vector2f &vector) {
//   return os << "[" << vector.X << ", " << vector.Y << "]";
// }

//////////////////////
/////////Vector3//////
//////////////////////
struct Vector4f;
struct Vector3f {
  union {
    float x = 0.0f;
    float r;
  };

  union {
    float y = 0.0f;
    float g;
  };

  union {
    float z = 0.0f;
    float b;
    /* data */
  };

  Vector3f(){};

  Vector3f(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

  Vector3f(float val) : Vector3f(val, val, val) {}

  Vector3f(const Vector3f &vector) : x(vector.x), y(vector.y), z(vector.z) {}

  Vector3f(const Vector4f &v);

  Vector3f(const float *f) {

    assert(f != nullptr);
    x = f[0];
    y = f[1];
    z = f[2];
  }

  // Vector3f &operator+(const Vector3f &other) {
  //   x = x + other.x;
  //   y = y + other.y;
  //   z = z + other.z;
  //
  //  return *this;
  //}
  // Vector3f &operator-(const Vector3f &other) {
  //   x = x - other.x;
  //   y = y - other.x;
  //   z = z - other.z;

  //   return *this;
  // }
  // Vector3f &operator*(const Vector3f &other) {
  //  x = x * other.x;
  //  y = y * other.y;
  //  z = z * other.z;
  //  return *this;
  //}
  Vector3f &operator=(const Vector3f &other) {
    x = other.x;
    y = other.y;
    z = other.z;

    return *this;
  }

  Vector3f &operator+=(const Vector3f &other) {
    x += other.x;
    y += other.y;
    z += other.z;

    return *this;
  }

  Vector3f &operator-=(const Vector3f &other) {
    x -= other.x;
    y -= other.y;
    z -= other.z;

    return *this;
  }

  Vector3f &operator*=(const Vector3f &other) {
    x *= other.x;
    y *= other.y;
    z *= other.z;

    return *this;
  }

  Vector3f &operator*=(float factor) {
    x *= factor;
    y *= factor;
    z *= factor;

    return *this;
  }

  bool operator==(const Vector3f &vec) {
    return (x == vec.x && y == vec.y && z == vec.z);
  }

  float Length() {
    float len = sqrtf(x * x + y * y + z * z);

    return len;
  }

  float Dot() {
    float ret = x * x + y * y + z * z;
    return ret;
  }

  Vector3f &Normalize() {
    float len = Length();
    assert(len != 0.0f);

    x /= len;
    y /= len;
    z /= len;

    return *this;
  }

  Vector3f Cross(const Vector3f &v) const {
    const float _x = y * v.z - z * v.y;
    const float _y = z * v.x - x * v.z;
    const float _z = x * v.y - y * v.x;

    return Vector3f(_x, _y, _z);
  }

  float Distance(const Vector3f &vec) {
    float delta_x = x - vec.x;
    float delta_y = y - vec.y;
    float delta_z = z - vec.z;

    float dis =
        sqrtf(delta_x * delta_x + delta_y * delta_y + delta_z * delta_z);

    return dis;
  }

  void Set(float f) { x = y = z = f; }

  bool Iszero() {
    bool ret = ((x + y + z) == 0.0f);

    return ret;
  }

  float *Data() { return &x; }
  void operator=(const glm::vec3 &vec) {
    x = vec.x;
    y = vec.y;
    z = vec.z;
  }

  Vector3f Negate() const {
    Vector3f ret(-x, -y, -z);
    return ret;
  }

  void Rotate(float Angle, const Vector3f &V);

  glm::vec3 ToGLM() { return glm::vec3(x, y, z); }

  glm::vec3 ToGLM() const { return glm::vec3(x, y, z); }

  operator float *() { return &x; }
  operator const float *() const { return &(x); }
};

inline std::ostream &operator<<(std::ostream &os, const Vector3f &vec) {

  return os << vec.x << ", " << vec.y << ", " << vec.z;
}
//////////////////////
/////////Vector4//////
//////////////////////

struct Vector4f {
  union {
    float x = 0.0f;
    float r;
  };

  union {
    float y = 0.0f;
    float g;
  };

  union {
    float z = 0.0f;
    float b;
  };

  union {
    float w = 0.0f;
    float a;
  };

  Vector4f() {}

  Vector4f(float _x, float _y, float _z, float _w) {
    x = _x;
    y = _y;
    z = _z;
    w = _w;
  }

  Vector4f(const Vector3f &v, float _w) {
    x = v.x;
    y = v.y;
    z = v.z;
    w = _w;
  }

  Vector4f(float f) { x = y = z = w = f; }

  void Print(bool endl = true) const {
    printf("(%f, %f, %f, %f)", x, y, z, w);

    if (endl) {
      printf("\n");
    }
  }

  Vector3f to3f() const {
    Vector3f v(x, y, z);
    return v;
  }

  float Length() const {
    float len = sqrtf(x * x + y * y + z * z + w * w);
    return len;
  }

  Vector4f &Normalize() {
    float len = Length();

    assert(len != 0);

    x /= len;
    y /= len;
    z /= len;

    return *this;
  }

  float Dot(const Vector4f &v) const {
    float ret = x * v.x + y * v.y + z * v.z + w * v.w;
    return ret;
  }

  glm::vec4 ToGLM() const { return glm::vec4(x, y, z, w); }

  glm::vec3 ToGLM() { return glm::vec3(x, y, z); }

  bool operator==(const Vector4f &r) {
    return ((x == r.x) && (y == r.y) && (z == r.z) && (w == r.w));
  }

  bool operator!=(const Vector4f &r) { return !(*this == r); }
};

inline Vector3f operator+(const Vector3f &l, const Vector3f &r) {
  Vector3f Ret(l.x + r.x, l.y + r.y, l.z + r.z);

  return Ret;
}

inline Vector3f operator-(const Vector3f &l, const Vector3f &r) {
  Vector3f Ret(l.x - r.x, l.y - r.y, l.z - r.z);

  return Ret;
}

inline Vector3f operator*(const Vector3f &l, float f) {
  Vector3f Ret(l.x * f, l.y * f, l.z * f);

  return Ret;
}

inline Vector3f operator/(const Vector3f &l, float f) {
  Vector3f Ret(l.x / f, l.y / f, l.z / f);

  return Ret;
}

inline Vector3f::Vector3f(const Vector4f &v) {
  x = v.x;
  y = v.y;
  z = v.z;
}

inline Vector4f operator+(const Vector4f &l, const Vector4f &r) {
  Vector4f Ret(l.x + r.x, l.y + r.y, l.z + r.z, l.w + r.w);

  return Ret;
}

inline Vector4f operator-(const Vector4f &l, const Vector4f &r) {
  Vector4f Ret(l.x - r.x, l.y - r.y, l.z - r.z, l.w - r.w);

  return Ret;
}

inline Vector4f operator/(const Vector4f &l, float f) {
  Vector4f Ret(l.x / f, l.y / f, l.z / f, l.w / f);

  return Ret;
}

inline Vector4f operator*(const Vector4f &l, float f) {
  Vector4f Ret(l.x * f, l.y * f, l.z * f, l.w * f);

  return Ret;
}

inline Vector4f operator*(float f, const Vector4f &l) {
  Vector4f Ret(l.x * f, l.y * f, l.z * f, l.w * f);

  return Ret;
}

// std::ostream &operator<<(std::ostream &os, const Vector4f &vector) {
//   return os << "[" << vector.X << ", " << vector.Y << ", " << vector.Z <<
//   "]";
//}

} // namespace Ezzoo