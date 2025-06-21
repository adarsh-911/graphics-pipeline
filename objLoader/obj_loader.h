#pragma once

#include <bits/stdc++.h>
#include <vector>
#include <string>

struct Vec3 { 
  float x, y, z;
  Vec3() : x(0), y(0), z(0) {}
  Vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

  Vec3 operator*(float scalar) const {return Vec3(x * scalar, y * scalar, z * scalar);}
  Vec3 operator+(const Vec3& other) const {return Vec3(x + other.x, y + other.y, z + other.z);}
};

struct Vec2uv { 
  float u, v;
  Vec2uv() = default;
  Vec2uv(float _u, float _v) : u(_u), v(_v) {}
};

struct Vec4 { 
  float x, y, z, w;
  Vec4() = default;
  Vec4(float x_, float y_, float z_, float w_ = 1.0f)
      : x(x_), y(y_), z(z_), w(w_) {}
};

struct Ind {
  int v0, v1, v2;
};

class OBJLoader {
  public:
    bool load(const std::string& filename);

    std::vector<Vec4>& getVertices();
    std::vector<Ind>& getTriangleInd();
    std::vector<Vec2uv>& getTexCords();
    std::vector<Ind>& getTexInd();
    std::vector<Vec3>& getNormals();
    std::vector<Ind>& getNormalInd();

  private:
    std::vector<Ind> triangleInd;
    std::vector<Vec4> vertices;
    std::vector<Vec2uv> texcoords;
    std::vector<Ind> texInd;
    std::vector<Vec3> normals;
    std::vector<Ind> normalInd;
};
