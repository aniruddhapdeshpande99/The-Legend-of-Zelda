#include "main.h"

#ifndef CANNONF_H
#define CANNON_H


class Cannon {
public:
    Cannon() {}
    Cannon(float x, float y, float z, color_t color);
    glm::vec3 position;
    float rotationx;
    float rotationy;
    float rotationz;
    void draw(glm::mat4 VP);
    void set_position(float x, float y);
    void tick();
    double speedx;
    double speedy;
    double speedz;
    bounding_box_t bounding_box();
private:
    VAO *object;
};

#endif // CANNON_H
