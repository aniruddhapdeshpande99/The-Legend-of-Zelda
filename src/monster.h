#include "main.h"

#ifndef MONSTER_H
#define MONSTER_H


class Monster {
public:
    Monster() {}
    Monster(float x, float y, float z, float l, float w, float h, int health, color_t color);
    glm::vec3 position;
    float rotation;
    void draw(glm::mat4 VP);
    void set_position(float x, float y);
    void tick();
    double speedx;
    double speedy;
    double speedz;
    double length;
    double width;
    double height;
    int life;
    bounding_box_t bounding_box();
private:
    VAO *object;
    VAO *object1;
};

#endif // MONSTER_H
