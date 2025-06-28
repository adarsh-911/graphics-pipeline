#pragma once

#include "../objLoader/objLoader.hpp"

inline void serializeString(std::ostream& os, const std::string& s) {
    size_t len = s.size();
    os.write(reinterpret_cast<const char*>(&len), sizeof(len));
    os.write(s.data(), len);
}

inline void deserializeString(std::istream& is, std::string& s) {
    size_t len;
    is.read(reinterpret_cast<char*>(&len), sizeof(len));
    s.resize(len);
    if (len > 0) {
        is.read(&s[0], len);
    }
}

template<typename T>
void serializeVectorTrivial(std::ostream& os, const std::vector<T>& v) {
    size_t size = v.size();
    os.write(reinterpret_cast<const char*>(&size), sizeof(size));
    if (size > 0) {
        os.write(reinterpret_cast<const char*>(v.data()), size * sizeof(T));
    }
}

template<typename T>
void deserializeVectorTrivial(std::istream& is, std::vector<T>& v) {
    size_t size;
    is.read(reinterpret_cast<char*>(&size), sizeof(size));
    v.resize(size);
    if (size > 0) {
        is.read(reinterpret_cast<char*>(v.data()), size * sizeof(T));
    }
}

inline void serializeVecVecColor(std::ostream& os, const std::vector<std::vector<Color>>& pixels) {
    size_t outerSize = pixels.size();
    os.write(reinterpret_cast<const char*>(&outerSize), sizeof(outerSize));
    for (const auto& inner : pixels) {
        serializeVectorTrivial(os, inner);
    }
}

inline void deserializeVecVecColor(std::istream& is, std::vector<std::vector<Color>>& pixels) {
    size_t outerSize;
    is.read(reinterpret_cast<char*>(&outerSize), sizeof(outerSize));
    pixels.resize(outerSize);
    for (auto& inner : pixels) {
        deserializeVectorTrivial(is, inner);
    }
}

class Model {
  public:
    std::string name;
    bool light;
    std::vector<int> texWidth, texHeight;

    glm::vec3 position;
    glm::vec3 axis;
    float angle;

    float scaleFactor;
    Color color;

    bool load(const std::string& filename);
    void loadTexture (const char* filename, int& width, int& height);

    const std::vector<glm::vec3>& getVertices() const;
    const std::vector<glm::vec2>& getTexCoords() const;
    std::vector<glm::vec3>& getNormals();

    const std::vector<idx>& getIndices(int which) const;
    const std::vector<Color>& getTexColors(int x) const;
    const int getTexID(int x) const;
    std::vector<glm::vec3> normals;

    // Serialization methods
    void serialize(std::ostream& os) const {
        serializeString(os, name);
        os.write(reinterpret_cast<const char*>(&light), sizeof(light));

        serializeVectorTrivial(os, texWidth);
        serializeVectorTrivial(os, texHeight);

        os.write(reinterpret_cast<const char*>(&position), sizeof(position));
        os.write(reinterpret_cast<const char*>(&axis), sizeof(axis));
        os.write(reinterpret_cast<const char*>(&angle), sizeof(angle));
        os.write(reinterpret_cast<const char*>(&scaleFactor), sizeof(scaleFactor));
        os.write(reinterpret_cast<const char*>(&color), sizeof(color));

        serializeVectorTrivial(os, vertices);
        serializeVectorTrivial(os, texCoords);
        serializeVectorTrivial(os, pIdx);
        serializeVectorTrivial(os, tIdx);
        serializeVectorTrivial(os, nIdx);
        serializeVectorTrivial(os, texID);
        serializeVectorTrivial(os, normals);

        serializeVecVecColor(os, pixels);
    }

    bool deserialize(std::istream& is) {
        deserializeString(is, name);
        is.read(reinterpret_cast<char*>(&light), sizeof(light));

        deserializeVectorTrivial(is, texWidth);
        deserializeVectorTrivial(is, texHeight);

        is.read(reinterpret_cast<char*>(&position), sizeof(position));
        is.read(reinterpret_cast<char*>(&axis), sizeof(axis));
        is.read(reinterpret_cast<char*>(&angle), sizeof(angle));
        is.read(reinterpret_cast<char*>(&scaleFactor), sizeof(scaleFactor));
        is.read(reinterpret_cast<char*>(&color), sizeof(color));

        deserializeVectorTrivial(is, vertices);
        deserializeVectorTrivial(is, texCoords);
        deserializeVectorTrivial(is, pIdx);
        deserializeVectorTrivial(is, tIdx);
        deserializeVectorTrivial(is, nIdx);
        deserializeVectorTrivial(is, texID);
        deserializeVectorTrivial(is, normals);

        deserializeVecVecColor(is, pixels);

        return !is.fail();
    }
  
  private:
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> texCoords;

    std::vector<idx> pIdx;
    std::vector<idx> tIdx;
    std::vector<idx> nIdx;
    std::vector<int> texID;

    std::vector<std::vector<Color>> pixels;
};