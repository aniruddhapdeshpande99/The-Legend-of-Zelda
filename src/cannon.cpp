#include "cannon.h"
#include "main.h"

Cannon::Cannon(float x, float y, float z, color_t color) {
    this->position = glm::vec3(x, y, z);
    this->rotationx = 0;
    this->rotationy = 0;
    this->rotationz = 0;
    speedx = 0;
    speedy = 0;
    speedz = 0;
    // Our vertices. Three consecutive floats give a 3D vertex; Three consecutive vertices give a triangle.
    // A cube has 6 faces with 2 triangles each, so this makes 6*2=12 triangles, and 12*3 vertices
    const GLfloat vertex_buffer_data[] = {
      -1.0,-0.05,-0.05,
      -1.0,-0.05, 0.05,
      -1.0, 0.05, 0.05,
      1.0, 0.05,-0.05,
      -1.0,-0.05,-0.05,
      -1.0, 0.05,-0.05,
      1.0,-0.05, 0.05,
      -1.0,-0.05,-0.05,
      1.0,-0.05,-0.05,
      1.0, 0.05,-0.05,
      1.0,-0.05,-0.05,
      -1.0,-0.05,-0.05,
      -1.0,-0.05,-0.05,
      -1.0, 0.05, 0.05,
      -1.0, 0.05,-0.05,
      1.0,-0.05, 0.05,
      -1.0,-0.05, 0.05,
      -1.0,-0.05,-0.05,
      -1.0, 0.05, 0.05,
      -1.0,-0.05, 0.05,
      1.0,-0.05, 0.05,
      1.0, 0.05, 0.05,
      1.0,-0.05,-0.05,
      1.0, 0.05,-0.05,
      1.0,-0.05,-0.05,
      1.0, 0.05, 0.05,
      1.0,-0.05, 0.05,
      1.0, 0.05, 0.05,
      1.0, 0.05,-0.05,
      -1.0, 0.05,-0.05,
      1.0, 0.05, 0.05,
      -1.0, 0.05,-0.05,
      -1.0, 0.05, 0.05,
      1.0, 0.05, 0.05,
      -1.0, 0.05, 0.05,
      1.0,-0.05, 0.05
    };

    this->object = create3DObject(GL_TRIANGLES, 12*3, vertex_buffer_data, color, GL_FILL);
}

void Cannon::draw(glm::mat4 VP) {
    Matrices.model = glm::mat4(1.0f);
    glm::mat4 translate = glm::translate (this->position);    // glTranslatef
    glm::mat4 rotatex    = glm::rotate((float) (this->rotationx * M_PI / 180.0f), glm::vec3(1, 0, 0));
    glm::mat4 rotatey    = glm::rotate((float) (this->rotationy * M_PI / 180.0f), glm::vec3(0, 1, 0));
    glm::mat4 rotatez    = glm::rotate((float) (this->rotationz * M_PI / 180.0f), glm::vec3(0, 0, 1));
    // No need as coords centered at 0, 0, 0 of cube arouund which we waant to rotate
    // rotate          = rotate * glm::translate(glm::vec3(0, -0.6, 0));
    Matrices.model *= (translate * rotatex * rotatey * rotatez);
    glm::mat4 MVP = VP * Matrices.model;
    glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
    draw3DObject(this->object);
}

void Cannon::set_position(float x, float y) {
    this->position = glm::vec3(x, y, 0);
}

void Cannon::tick() {
    // this->rotation += speed;
    this->position.x += speedx;
    this->position.y += speedy;
    this->position.z += speedz;
}

bounding_box_t Cannon::bounding_box() {
    float x = this->position.x, y = this->position.y, z = this->position.z;
    float l = 2*1, w = 2*0.05, h = 2*0.05;
    bounding_box_t bbox = { x, y, z, l, w, h};
    return bbox;
}
