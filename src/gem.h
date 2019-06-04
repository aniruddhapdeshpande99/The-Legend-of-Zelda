#include "main.h"

#ifndef GEM_H
#define GEM_H


class Gem {
public:
    Gem() {}
    Gem(float x, float y, float z, color_t color);
    glm::vec3 position;
    float rotation;
    void draw(glm::mat4 VP);
    void set_position(float x, float y);
    void tick();
    double speed;
private:
    VAO *object;
};

#endif // GEM_H
