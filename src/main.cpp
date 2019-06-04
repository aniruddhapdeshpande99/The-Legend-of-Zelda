#include "main.h"
#include "string.h"
#include "timer.h"
#include "ball.h"
#include "water.h"
#include "block.h"
#include "sail.h"
#include "barrel.h"
#include "cannon.h"
#include "gem.h"
#include "monster.h"
#include "unistd.h"
// #include "signal.h"
#define PI 3.14285714286

using namespace std;

GLMatrices Matrices;
GLuint     programID;
GLFWwindow *window;

/**************************
* Customizable functions *
**************************/

float randomPos(float min, float max)
{
  return ((float(rand()) / float(RAND_MAX)) * (max-min)) + min;
}

//To read cursor position
static void cursorPositionCallback(GLFWwindow *window, double xPos, double yPos);
//To check for cursor position only within the window
void cursorEnterCallback(GLFWwindow *window, int entered);
//To check for mouse click
void mouseButtonCallBack(GLFWwindow *window, int button, int action, int mods);
//To check for scroll on mouse
void scrollCallback(GLFWwindow *window, double xOffset, double yOffset);

Block ocean; //Declaring Ocean
Block boat; //Declaring Boat
Cannon cannon; //Declaring cannon
Block stones[18]; //Declaring stones
Sail sail; //Declaring sail
Barrel gift_barrels[15], med_barrels[7], boost_barrels[7]; //Declaring Gift point barrels, Health Point Barrels and Booster Barrels respectively
Gem gifts[15], meds[7], boosts[7]; //Declaring Gift point gems, Health gems and booster gems respectively
Monster monsters[5]; //Declaring monsters
Block fireball; //Declaring fireball
Monster boss; //Declaring boss
int spawn_boss = 0, mons_count = 0, spawned = 0;

float target_x = 0.0, target_y = 0.0, target_z = 0.0;
float cam_x = 0.0, cam_y = 0.0 , cam_z = 0.0;

float follow_x  = 3.0*cos((PI/180)*boat.rotation), follow_y = 1.0, follow_z = -3.0*sin((PI/180)*boat.rotation);

float top_x = 0.01*cos((PI/180)*boat.rotation), top_y = 8.0, top_z = -0.01*sin((PI/180)*boat.rotation);

float tower_x = 15.0, tower_y = 7.0, tower_z = 0.0;

float helicopter_x = 10.0, helicopter_y = 10.0, helicopter_z = 10.0;

float boatview_x = boat.position.x, boatview_y = boat.position.y, boatview_z = boat.position.z;

int curr_angle = 0;

float screen_zoom = 1, screen_center_x = 0, screen_center_y = 0;
float camera_rotation_angle = 0;

float wobble = 0.0, surface = -0.75;
int cam_press = 0, jump_press = 0, in_air = 0, fire_press = 0;
int wind_duration = 300, wind_exist = 0, pause_duration = 300;
float wind_angle = 0.0, wind_intensity = 0.0;

int health = 100, points = 0, level = 1;

float shootposx = 0.0, shootposz = 0.0, shootangle = 0.0;

double cur_xPos = 0, cur_yPos = 0, prev_xPos = 0.0, prev_yPos = 0.0;
int frame_count = 0;
int lmb_press = 0;
double heli_hor = 0.0, heli_ver = 0.0;

int dead[5] = {0,0,0,0,0};

Timer t60(1.0 / 60);

/* Render the scene with openGL */
/* Edit this function according to your assignment */
void draw() {
    // clear the color and depth in the frame buffer
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // use the loaded shader program
    // Don't change unless you know what you are doing
    glUseProgram (programID);

    // Eye - Location of camera. Don't change unless you are sure!!
    glm::vec3 eye (cam_x, cam_y, cam_z);
    // Target - Where is the camera looking at.  Don't change unless you are sure!!
    glm::vec3 target (target_x, target_y, target_z);
    // Up - Up vector defines tilt of camera.  Don't change unless you are sure!!
    glm::vec3 up (0,1,0);

    // Compute Camera matrix (view)
    Matrices.view = glm::lookAt( eye, target, up ); // Rotating Camera for 3D
    // Don't change unless you are sure!!
    // Matrices.view = glm::lookAt(glm::vec3(0, 0, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)); // Fixed camera for 2D (ortho) in XY plane

    // Compute ViewProject matrix as view/camera might not be changed for this frame (basic scenario)
    // Don't change unless you are sure!!
    glm::mat4 VP = Matrices.projection * Matrices.view;

    // Send our transformation to the currently bound shader, in the "MVP" uniform
    // For each model you render, since the MVP will be different (at least the M part)
    // Don't change unless you are sure!!
    glm::mat4 MVP;  // MVP = Projection * View * Model

    // Scene render

    cannon.draw(VP);

    boat.draw(VP);

    for (int i = 0; i < 15; i++) {
      gift_barrels[i].draw(VP);
      gifts[i].draw(VP);
    }

    for (int i = 0; i < 7; i++) {
      boost_barrels[i].draw(VP);
      boosts[i].draw(VP);
    }

    for (int i = 0; i < 7; i++) {
      med_barrels[i].draw(VP);
      meds[i].draw(VP);
    }

    fireball.draw(VP);
    for (int i = 0; i < 18; i++) {
      stones[i].draw(VP);
    }

    for (int i = 0; i < 5; i++) {
      monsters[i].draw(VP);
    }

    boss.draw(VP);

    glUniform1f(Matrices.tr, 0.5);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    ocean.draw(VP);
    glDisable(GL_BLEND);


    if (curr_angle != 3) {
      glUniform1f(Matrices.tr, 0.75);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      sail.draw(VP);
      glDisable(GL_BLEND);
    }

}

void tick_input(GLFWwindow *window) {
    int left  = glfwGetKey(window, GLFW_KEY_LEFT);
    int right = glfwGetKey(window, GLFW_KEY_RIGHT);
    int up = glfwGetKey(window, GLFW_KEY_UP);
    int down = glfwGetKey(window, GLFW_KEY_DOWN);
    int cam_toggle = glfwGetKey(window, GLFW_KEY_C);
    int space = glfwGetKey(window, GLFW_KEY_SPACE);
    int fire = glfwGetKey(window, GLFW_KEY_F);

    //Initiates camera toggle
    if (cam_toggle == GLFW_PRESS) {
      cam_press = 1;
    }

    //Toggled on release
    if (cam_toggle == GLFW_RELEASE && cam_press) {
      curr_angle = (curr_angle+1)%5;
      cam_press = 0;
    }

    //Move front
    if (up) {
      boat.position.x -= (0.1+wind_intensity)*cos((boat.rotation*PI)/180);
      boat.position.z += (0.1+wind_intensity)*sin((boat.rotation*PI)/180);

      sail.position.x -= (0.1+wind_intensity)*cos((boat.rotation*PI)/180);
      sail.position.z += (0.1+wind_intensity)*sin((boat.rotation*PI)/180);

      cannon.position.x -= (0.1+wind_intensity)*cos((boat.rotation*PI)/180);
      cannon.position.z += (0.1+wind_intensity)*sin((boat.rotation*PI)/180);

      if(curr_angle == 3) {

        target_x = boat.position.x - 1.5*cos((PI/180)*boat.rotation);
        target_y = boat.position.y + 0.5;
        target_z = boat.position.z + 1.5*sin((PI/180)*boat.rotation);

        cam_x = boat.position.x - 0.5*cos((PI/180)*boat.rotation);
        cam_y = boat.position.y + 0.5;
        cam_z = boat.position.z + 0.5*sin((PI/180)*boat.rotation);
      }
    }

    //Move backwards
    if (down) {

      boat.position.x += (0.1+wind_intensity)*cos((boat.rotation*PI)/180);
      boat.position.z -= (0.1+wind_intensity)*sin((boat.rotation*PI)/180);

      sail.position.x += (0.1+wind_intensity)*cos((boat.rotation*PI)/180);
      sail.position.z -= (0.1+wind_intensity)*sin((boat.rotation*PI)/180);

      cannon.position.x += (0.1+wind_intensity)*cos((boat.rotation*PI)/180);
      cannon.position.z -= (0.1+wind_intensity)*sin((boat.rotation*PI)/180);

    }

    //Turn Left
    if (left) {
        boat.rotation +=  0.5f;
        sail.rotation += 0.5f;
        cannon.rotationy += 0.5f;
    }

    //Turn right
    if (right) {
      boat.rotation -= 0.5f;
      sail.rotation -= 0.5f;
      cannon.rotationy -= 0.5f;
    }

    //Trigger Jump
    if (space && in_air == 0) {
      jump_press = 1;
    }

    //Jump after release
    if (space == GLFW_RELEASE && jump_press) {
      boat.speedy = 0.25;
      sail.speedy = 0.25;
      cannon.speedy = 0.25;
      jump_press = 0;
      in_air = 1;
    }


    //Load Fireball
    if (fire == GLFW_PRESS) {
      fire_press = 1;
      fireball.speedx = 0;
      fireball.speedz = 0;
      fireball.position.x = cannon.position.x;
      fireball.position.y = cannon.position.y;
      fireball.position.z = cannon.position.z;

      shootposx = cannon.position.x;
      shootposz = cannon.position.z;
      shootangle = cannon.rotationy;
    }

    //Shoot on release
    if (fire == GLFW_RELEASE && fire_press) {
      fireball.speedx -= (0.1)*cos((cannon.rotationy*PI)/180);
      fireball.speedz += (0.1)*sin((cannon.rotationy*PI)/180);
      fire_press = 0;
    }
}

void tick_elements() {

    char cam[5][100] = {"Follow Cam", "Top View Cam", "Tower View Cam", "Boat Cam", "Helicopter Cam"};

    //Boat wobble animation
    wobble += PI/30;
    boat.position.y += sin(wobble)*0.015;
    sail.position.y += sin(wobble)*0.015;
    cannon.position.y += sin(wobble)*0.015;

    //Spawn Boss after monsters dead
    for (mons_count = 0; mons_count < 5; mons_count++) {
      if (dead[mons_count] != 1) {
        break;
      }
    }

    if (mons_count == 5) {
      spawn_boss = 1;
    }

    if (spawn_boss == 1 && spawned != 1) {
      boss.position.x = randomPos(-70.0,-20.0);
      boss.position.y =  0;
      boss.position.z = randomPos(-50.0,50.0);
      spawned = 1;
    }

    //Wobble boss
    boss.position.y += sin(wobble)*0.015;

    //Wobble Gems and Gem Barrels
    for (int i = 0; i < 15; i++) {
      gift_barrels[i].position.y +=sin(wobble)*0.01;
      gifts[i].position.y += sin(wobble)*0.01;
    }

    for (int i = 0; i < 7; i++) {
      boost_barrels[i].position.y +=sin(wobble)*0.01;
      boosts[i].position.y += sin(wobble)*0.01;

      med_barrels[i].position.y +=sin(wobble)*0.01;
      meds[i].position.y += sin(wobble)*0.01;
    }



    //Wobble monsters
    for (int i = 0; i < 5; i++) {
      monsters[i].position.y += sin(wobble)*0.015;
    }




    //Wind Mechanics
    if (pause_duration != 0 && wind_exist == 0) {
      pause_duration--;
    }

    if (pause_duration == 0) {
      pause_duration = 300;
      wind_exist = 1;
      wind_angle = randomPos(0.0, 360.0);
      wind_intensity = randomPos(0.025, 0.05);
    }

    if (wind_duration != 0 && wind_exist == 1) {
      wind_duration --;
    }

    if (wind_duration == 0) {
      wind_duration = 300;
      wind_exist = 0;
      wind_intensity = 0;
      wind_angle = 0;
    }


    if (wind_exist == 1) {
      boat.position.x -= wind_intensity*cos(wind_angle*(PI/180));
      sail.position.x -= wind_intensity*cos(wind_angle*(PI/180));
      cannon.position.x -= wind_intensity*cos(wind_angle*(PI/180));

      boat.position.z += wind_intensity*sin(wind_angle*(PI/180));
      sail.position.z += wind_intensity*sin(wind_angle*(PI/180));
      cannon.position.z += wind_intensity*sin(wind_angle*(PI/180));

      sail.rotation = wind_angle;
    }

    if (wind_exist == 0) {
      sail.rotation = boat.rotation;
    }




    //Camera Mechanics
    //Follow Boat Angle ie 3rd Person
    if (curr_angle == 0)  {
      target_x = boat.position.x;
      target_z = boat.position.z;
      target_y = 0.0;

      follow_x = 3.0*cos((PI/180)*boat.rotation);
      follow_z = -3.0*sin((PI/180)*boat.rotation);

      cam_x = follow_x + target_x;
      cam_y = follow_y + target_y;
      cam_z = follow_z + target_z;

      if (cur_xPos > 0 && cur_xPos < 450.0) {
        cannon.rotationy = boat.rotation + ((cur_xPos - 0.0)/450.0)*(-45.0) + 45.0;
      }

      else if (cur_xPos >= 450.0 && cur_xPos < 900.0) {
        cannon.rotationy = boat.rotation + ((cur_xPos - 450.0)/450.0)*(-45.0);
      }

    }

    //Top view Angle
    else if(curr_angle == 1) {
      target_x = boat.position.x;
      target_z = boat.position.z;
      target_y = 0.0;

      top_x = 0.01*cos((PI/180)*boat.rotation);
      top_z = -0.01*sin((PI/180)*boat.rotation);

      cam_x = top_x + target_x;
      cam_y = top_y + target_y;
      cam_z = top_z + target_z;

      if (cur_xPos > 0 && cur_xPos < 450.0) {
        cannon.rotationy = boat.rotation + ((cur_xPos - 0.0)/450.0)*(-45.0) + 45.0;
      }

      else if (cur_xPos >= 450.0 && cur_xPos < 900.0) {
        cannon.rotationy = boat.rotation + ((cur_xPos - 450.0)/450.0)*(-45.0);
      }
    }

    //Tower view Angle
    else if(curr_angle == 2) {
      target_x = boat.position.x;
      target_z = boat.position.z;
      target_y = 0.0;

      cam_x = tower_x;
      cam_y = tower_y;
      cam_z = tower_z;

      //No Cannon option on this angle
    }

    //Boat View Angle ie First person
    else if(curr_angle == 3) {
      target_x = boat.position.x - 1.5*cos((PI/180)*boat.rotation);
      target_y = boat.position.y + 0.5;
      target_z = boat.position.z + 1.5*sin((PI/180)*boat.rotation);

      cam_x = boat.position.x - 0.5*cos((PI/180)*boat.rotation);
      cam_y = boat.position.y + 0.5;
      cam_z = boat.position.z + 0.5*sin((PI/180)*boat.rotation);

      if (cur_xPos > 0 && cur_xPos < 450.0) {
        cannon.rotationy =  boat.rotation + ((cur_xPos - 0.0)/450.0)*(-45.0) + 45.0;
      }

      else if (cur_xPos >= 450.0 && cur_xPos < 900.0) {
        cannon.rotationy = boat.rotation + ((cur_xPos - 450.0)/450.0)*(-45.0);
      }

    }

    //Helicopter Cam
    else if (curr_angle == 4) {
      //Cant aim in Helicopter Cam
      double xpos, ypos;
      cam_x = helicopter_x;
      cam_y = helicopter_y;
      cam_z = helicopter_z;
      glfwGetCursorPos(window, &xpos, &ypos);
      //Change Look Angle
      if (lmb_press == 1) {
          heli_hor += (xpos - prev_xPos)*360.f*0.0001f;
          heli_ver += (ypos - prev_yPos)*360.f*0.0001f;

          double pos_diff_x = abs(cam_x - target_x), pos_diff_z = abs(cam_z - target_z), pos_diff_y = abs(cam_y - target_y);

          double pos_diff_xz = sqrt(pow(pos_diff_x,2) + pow(pos_diff_z,2));
          double pos_diff_xy = sqrt(pow(pos_diff_x,2) + pow(pos_diff_y,2));

          target_z = pos_diff_xz*sin(heli_hor);
          target_x = pos_diff_xz*cos(heli_hor);
          target_y = -pos_diff_xy*sin(heli_ver);

          prev_xPos = xpos;
          prev_yPos = ypos;
      }
    }




    //Jump Gravity Mechanics
    //Gravity deceleration
    if (in_air) {
      boat.speedy -= 0.01;
      sail.speedy -= 0.01;
      cannon.speedy -= 0.01;
    }

    //To avoid the boat to go below the surface
    if (boat.position.y <= surface) {
      in_air = 0;
      boat.speedy = 0;
      sail.speedy = 0;
      cannon.speedy = 0;
      boat.position.y = -0.75;
      sail.position.y = -0.25;
      cannon.position.y = -0.75;
    }





    //Gift Gems implementation
    for (int i = 0; i < 15; i++) {
      if (detect_collision1(boat.bounding_box(), gift_barrels[i].bounding_box())) {
        float bPosX = 0.0, bPosZ = 0;
        bPosX = randomPos(-100.0,-10.0);
        bPosZ = randomPos(-100.0,100.0);

        gift_barrels[i].position.x = bPosX;
        gift_barrels[i].position.z = bPosZ;

        gifts[i].position.x = bPosX;
        gifts[i].position.z = bPosZ;

        points++;
      }
    }

    //Health Gems implementation
    for (int i = 0; i < 7; i++) {
      if (detect_collision1(boat.bounding_box(), med_barrels[i].bounding_box()) && health < 100) {
        float bPosX = 0.0, bPosZ = 0;
        bPosX = randomPos(-100.0,-10.0);
        bPosZ = randomPos(-100.0,100.0);

        med_barrels[i].position.x = bPosX;
        med_barrels[i].position.z = bPosZ;

        meds[i].position.x = bPosX;
        meds[i].position.z = bPosZ;

        health+=5;

        if (health > 100) {
          health = 100;
        }
      }
    }

    //Boost Gems implementation
    //Note: Forward Boost only
    for (int i = 0; i < 7; i++) {
      if (detect_collision1(boat.bounding_box(), boost_barrels[i].bounding_box())) {
        float bPosX = 0.0, bPosZ = 0;
        bPosX = randomPos(-100.0,-10.0);
        bPosZ = randomPos(-100.0,100.0);

        boost_barrels[i].position.x = bPosX;
        boost_barrels[i].position.z = bPosZ;

        boosts[i].position.x = bPosX;
        boosts[i].position.z = bPosZ;

        boat.speedx = -0.5*cos(boat.rotation*(PI/180));
        boat.speedz = 0.5*sin(boat.rotation*(PI/180));

        sail.speedx = -0.5*cos(boat.rotation*(PI/180));
        sail.speedz = 0.5*sin(boat.rotation*(PI/180));

        cannon.speedx = -0.5*cos(boat.rotation*(PI/180));
        cannon.speedz = 0.5*sin(boat.rotation*(PI/180));

      }
    }




    //Collision with Gift barrels
    for (int i = 0; i < 15; i++) {
      if (detect_collision(boat.bounding_box(), gift_barrels[i].bounding_box())) {

        if (abs(boat.position.x - gift_barrels[i].position.x) < 0.1) {

          if (abs(boat.position.z - gift_barrels[i].position.z) < 0.1) {
            boat.speedz = 0;
            sail.speedz = 0;
            cannon.speedz = 0;
          }

          else if (boat.position.z > gift_barrels[i].position.z) {
            boat.speedz = 0.25;
            sail.speedz = 0.25;
            cannon.speedz = 0.25;
          }

          else if (boat.position.z < gift_barrels[i].position.z) {
            boat.speedz = -0.25;
            sail.speedz = -0.25;
            cannon.speedz = -0.25;
          }

          boat.speedx = 0;
          sail.speedx = 0;
          cannon.speedx = 0;

        }

        else if (boat.position.x > gift_barrels[i].position.x) {

          if (abs(boat.position.z - gift_barrels[i].position.z) < 0.1) {
            boat.speedz = 0;
            sail.speedz = 0;
            cannon.speedz = 0;
          }

          else if (boat.position.z > gift_barrels[i].position.z) {
            boat.speedz = 0.25;
            sail.speedz = 0.25;
            cannon.speedz = 0.25;
          }

          else if (boat.position.z < gift_barrels[i].position.z) {
            boat.speedz = -0.25;
            sail.speedz = -0.25;
            cannon.speedz = -0.25;
          }

          boat.speedx = 0.25;
          sail.speedx = 0.25;
          cannon.speedx = 0.25;

        }

        else if (boat.position.x < gift_barrels[i].position.x) {

          if (abs(boat.position.z - gift_barrels[i].position.z) < 0.1) {
            boat.speedz = 0;
            sail.speedz = 0;
            cannon.speedz = 0;
          }

          else if (boat.position.z > gift_barrels[i].position.z) {
            boat.speedz = 0.25;
            sail.speedz = 0.25;
            cannon.speedz = 0.25;
          }
          else if (boat.position.z < gift_barrels[i].position.z) {
            boat.speedz = -0.25;
            sail.speedz = -0.25;
            cannon.speedz = -0.25;
          }

          boat.speedx = -0.25;
          sail.speedx = -0.25;
          cannon.speedx = -0.25;

        }
      }
    }

    //Collision with Booster barrels
    for (int i = 0; i < 7; i++) {
      if (detect_collision(boat.bounding_box(), boost_barrels[i].bounding_box())) {

        if (abs(boat.position.x - boost_barrels[i].position.x) < 0.1) {

          if (abs(boat.position.z - boost_barrels[i].position.z) < 0.1) {
            boat.speedz = 0;
            sail.speedz = 0;
            cannon.speedz = 0;
          }

          else if (boat.position.z > boost_barrels[i].position.z) {
            boat.speedz = 0.25;
            sail.speedz = 0.25;
            cannon.speedz = 0.25;
          }

          else if (boat.position.z < boost_barrels[i].position.z) {
            boat.speedz = -0.25;
            sail.speedz = -0.25;
            cannon.speedz = -0.25;
          }

          boat.speedx = 0;
          sail.speedx = 0;
          cannon.speedx = 0;

        }

        else if (boat.position.x > boost_barrels[i].position.x) {

          if (abs(boat.position.z - boost_barrels[i].position.z) < 0.1) {
            boat.speedz = 0;
            sail.speedz = 0;
            cannon.speedz = 0;
          }

          else if (boat.position.z > boost_barrels[i].position.z) {
            boat.speedz = 0.25;
            sail.speedz = 0.25;
            cannon.speedz = 0.25;
          }

          else if (boat.position.z < boost_barrels[i].position.z) {
            boat.speedz = -0.25;
            sail.speedz = -0.25;
            cannon.speedz = -0.25;
          }

          boat.speedx = 0.25;
          sail.speedx = 0.25;
          cannon.speedx = 0.25;

        }

        else if (boat.position.x < boost_barrels[i].position.x) {

          if (abs(boat.position.z - boost_barrels[i].position.z) < 0.1) {
            boat.speedz = 0;
            sail.speedz = 0;
            cannon.speedz = 0;
          }

          else if (boat.position.z > boost_barrels[i].position.z) {
            boat.speedz = 0.25;
            sail.speedz = 0.25;
            cannon.speedz = 0.25;
          }
          else if (boat.position.z < boost_barrels[i].position.z) {
            boat.speedz = -0.25;
            sail.speedz = -0.25;
            cannon.speedz = -0.25;
          }

          boat.speedx = -0.25;
          sail.speedx = -0.25;
          cannon.speedx = -0.25;

        }
      }
    }

    //Collision with Meds barrels
    for (int i = 0; i < 7; i++) {
      if (detect_collision(boat.bounding_box(), med_barrels[i].bounding_box())) {

        if (abs(boat.position.x - med_barrels[i].position.x) < 0.1) {

          if (abs(boat.position.z - med_barrels[i].position.z) < 0.1) {
            boat.speedz = 0;
            sail.speedz = 0;
            cannon.speedz = 0;
          }

          else if (boat.position.z > med_barrels[i].position.z) {
            boat.speedz = 0.25;
            sail.speedz = 0.25;
            cannon.speedz = 0.25;
          }

          else if (boat.position.z < med_barrels[i].position.z) {
            boat.speedz = -0.25;
            sail.speedz = -0.25;
            cannon.speedz = -0.25;
          }

          boat.speedx = 0;
          sail.speedx = 0;
          cannon.speedx = 0;

        }

        else if (boat.position.x > med_barrels[i].position.x) {

          if (abs(boat.position.z - med_barrels[i].position.z) < 0.1) {
            boat.speedz = 0;
            sail.speedz = 0;
            cannon.speedz = 0;
          }

          else if (boat.position.z > med_barrels[i].position.z) {
            boat.speedz = 0.25;
            sail.speedz = 0.25;
            cannon.speedz = 0.25;
          }

          else if (boat.position.z < med_barrels[i].position.z) {
            boat.speedz = -0.25;
            sail.speedz = -0.25;
            cannon.speedz = -0.25;
          }

          boat.speedx = 0.25;
          sail.speedx = 0.25;
          cannon.speedx = 0.25;

        }

        else if (boat.position.x < med_barrels[i].position.x) {

          if (abs(boat.position.z - med_barrels[i].position.z) < 0.1) {
            boat.speedz = 0;
            sail.speedz = 0;
            cannon.speedz = 0;
          }

          else if (boat.position.z > med_barrels[i].position.z) {
            boat.speedz = 0.25;
            sail.speedz = 0.25;
            cannon.speedz = 0.25;
          }
          else if (boat.position.z < med_barrels[i].position.z) {
            boat.speedz = -0.25;
            sail.speedz = -0.25;
            cannon.speedz = -0.25;
          }

          boat.speedx = -0.25;
          sail.speedx = -0.25;
          cannon.speedx = -0.25;

        }
      }
    }


    //Collision with rocks
    for (int i = 0; i < 18; i++) {
      if (detect_collision(boat.bounding_box(), stones[i].bounding_box())) {
        health --;
        if (abs(boat.position.x - stones[i].position.x) < 0.1) {

          if (abs(boat.position.z - stones[i].position.z) < 0.1) {
            boat.speedz = 0;
            sail.speedz = 0;
            cannon.speedz = 0;
          }

          else if (boat.position.z > stones[i].position.z) {
            boat.speedz = 0.25;
            sail.speedz = 0.25;
            cannon.speedz = 0.25;
          }

          else if (boat.position.z < stones[i].position.z) {
            boat.speedz = -0.25;
            sail.speedz = -0.25;
            cannon.speedz = -0.25;
          }

          boat.speedx = 0;
          sail.speedx = 0;
          cannon.speedx = 0;

        }

        else if (boat.position.x > stones[i].position.x) {

          if (abs(boat.position.z - stones[i].position.z) < 0.1) {
            boat.speedz = 0;
            sail.speedz = 0;
            cannon.speedz = 0;
          }

          else if (boat.position.z > stones[i].position.z) {
            boat.speedz = 0.25;
            sail.speedz = 0.25;
            cannon.speedz = 0.25;
          }

          else if (boat.position.z < stones[i].position.z) {
            boat.speedz = -0.25;
            sail.speedz = -0.25;
            cannon.speedz = -0.25;
          }

          boat.speedx = 0.25;
          sail.speedx = 0.25;
          cannon.speedx = 0.25;

        }

        else if (boat.position.x < stones[i].position.x) {

          if (abs(boat.position.z - stones[i].position.z) < 0.1) {
            boat.speedz = 0;
            sail.speedz = 0;
            cannon.speedz = 0;
          }

          else if (boat.position.z > stones[i].position.z) {
            boat.speedz = 0.25;
            sail.speedz = 0.25;
            cannon.speedz = 0.25;
          }
          else if (boat.position.z < stones[i].position.z) {
            boat.speedz = -0.25;
            sail.speedz = -0.25;
            cannon.speedz = -0.25;
          }

          boat.speedx = -0.25;
          sail.speedx = -0.25;
          cannon.speedx = -0.25;

        }
      }
  }


  //Collision with Monsters
  for (int i = 0; i < 5; i++) {
    if (detect_collision(boat.bounding_box(), monsters[i].bounding_box())) {
      health -= 5;
      if (abs(boat.position.x - monsters[i].position.x) < 0.1) {

        if (abs(boat.position.z - monsters[i].position.z) < 0.1) {
          boat.speedz = 0;
          sail.speedz = 0;
          cannon.speedz = 0;
        }

        else if (boat.position.z > monsters[i].position.z) {
          boat.speedz = 0.25;
          sail.speedz = 0.25;
          cannon.speedz = 0.25;
        }

        else if (boat.position.z < monsters[i].position.z) {
          boat.speedz = -0.25;
          sail.speedz = -0.25;
          cannon.speedz = -0.25;
        }

        boat.speedx = 0;
        sail.speedx = 0;
        cannon.speedx = 0;

      }

      else if (boat.position.x > monsters[i].position.x) {

        if (abs(boat.position.z - monsters[i].position.z) < 0.1) {
          boat.speedz = 0;
          sail.speedz = 0;
          cannon.speedz = 0;
        }

        else if (boat.position.z > monsters[i].position.z) {
          boat.speedz = 0.25;
          sail.speedz = 0.25;
          cannon.speedz = 0.25;
        }

        else if (boat.position.z < monsters[i].position.z) {
          boat.speedz = -0.25;
          sail.speedz = -0.25;
          cannon.speedz = -0.25;
        }

        boat.speedx = 0.25;
        sail.speedx = 0.25;
        cannon.speedx = 0.25;

      }

      else if (boat.position.x < monsters[i].position.x) {

        if (abs(boat.position.z - monsters[i].position.z) < 0.1) {
          boat.speedz = 0;
          sail.speedz = 0;
          cannon.speedz = 0;
        }

        else if (boat.position.z > monsters[i].position.z) {
          boat.speedz = 0.25;
          sail.speedz = 0.25;
          cannon.speedz = 0.25;
        }
        else if (boat.position.z < monsters[i].position.z) {
          boat.speedz = -0.25;
          sail.speedz = -0.25;
          cannon.speedz = -0.25;
        }

        boat.speedx = -0.25;
        sail.speedx = -0.25;
        cannon.speedx = -0.25;

      }
    }
  }


  //Collision with Boss
  if (detect_collision(boat.bounding_box(), boss.bounding_box())) {
    health -= 15;
    if (abs(boat.position.x - boss.position.x) < 0.1) {

      if (abs(boat.position.z - boss.position.z) < 0.1) {
        boat.speedz = 0;
        sail.speedz = 0;
        cannon.speedz = 0;
      }

      else if (boat.position.z > boss.position.z) {
        boat.speedz = 0.25;
        sail.speedz = 0.25;
        cannon.speedz = 0.25;
      }

      else if (boat.position.z < boss.position.z) {
        boat.speedz = -0.25;
        sail.speedz = -0.25;
        cannon.speedz = -0.25;
      }

      boat.speedx = 0;
      sail.speedx = 0;
      cannon.speedx = 0;

    }

    else if (boat.position.x > boss.position.x) {

      if (abs(boat.position.z - boss.position.z) < 0.1) {
        boat.speedz = 0;
        sail.speedz = 0;
        cannon.speedz = 0;
      }

      else if (boat.position.z > boss.position.z) {
        boat.speedz = 0.25;
        sail.speedz = 0.25;
        cannon.speedz = 0.25;
      }

      else if (boat.position.z < boss.position.z) {
        boat.speedz = -0.25;
        sail.speedz = -0.25;
        cannon.speedz = -0.25;
      }

      boat.speedx = 0.25;
      sail.speedx = 0.25;
      cannon.speedx = 0.25;

    }

    else if (boat.position.x < boss.position.x) {

      if (abs(boat.position.z - boss.position.z) < 0.1) {
        boat.speedz = 0;
        sail.speedz = 0;
        cannon.speedz = 0;
      }

      else if (boat.position.z > boss.position.z) {
        boat.speedz = 0.25;
        sail.speedz = 0.25;
        cannon.speedz = 0.25;
      }
      else if (boat.position.z < boss.position.z) {
        boat.speedz = -0.25;
        sail.speedz = -0.25;
        cannon.speedz = -0.25;
      }

      boat.speedx = -0.25;
      sail.speedx = -0.25;
      cannon.speedx = -0.25;

    }
  }




    //Post collision deceleration
    if (boat.speedx > 0) {
      boat.speedx -= 0.005;
      sail.speedx -= 0.005;
      cannon.speedx -= 0.005;

      if (boat.speedx < 0) {
        boat.speedx = 0;
        sail.speedx = 0;
        cannon.speedx = 0;
      }

    }

    if (boat.speedz > 0) {
      boat.speedz -= 0.005;
      sail.speedz -= 0.005;
      cannon.speedz -= 0.005;

      if (boat.speedz < 0) {
        boat.speedz = 0;
        sail.speedz = 0;
        cannon.speedz = 0;
      }

    }

    if (boat.speedx < 0) {
      boat.speedx += 0.005;
      sail.speedx += 0.005;
      cannon.speedx += 0.005;

      if (boat.speedx > 0) {
        boat.speedx = 0;
        sail.speedx = 0;
        cannon.speedx = 0;
      }

    }

    if (boat.speedz < 0) {
      boat.speedz += 0.005;
      sail.speedz += 0.005;
      cannon.speedz += 0.005;

      if (boat.speedz > 0) {
        boat.speedz = 0;
        sail.speedz = 0;
        cannon.speedz = 0;
      }

    }




    //Monster Attack Movement
    for (int i = 0; i < 5; i++) {

      if (abs(boat.position.x-monsters[i].position.x) < 5 && abs(boat.position.z - monsters[i].position.z) < 5) {

        if (abs(boat.position.x -  monsters[i].position.x) > 0.2) {
          if (boat.position.x > monsters[i].position.x) {
            monsters[i].position.x += 0.05;
          }
          else if (boat.position.x < monsters[i].position.x) {
            monsters[i].position.x -= 0.05;
          }
        }

        if (abs(boat.position.z - monsters[i].position.z) > 0.2) {
          if (boat.position.z > monsters[i].position.z) {
            monsters[i].position.z += 0.05;
          }
          else if (boat.position.z < monsters[i].position.z) {
            monsters[i].position.z -= 0.05;
          }
        }

      }

    }




    //Boss Attack Movement
    if (abs(boat.position.x-boss.position.x) < 10 && abs(boat.position.z - boss.position.z) < 10) {

      if (abs(boat.position.x -  boss.position.x) > 0.2) {
        if (boat.position.x > boss.position.x) {
          boss.position.x += 0.125;
        }
        else if (boat.position.x < boss.position.x) {
          boss.position.x -= 0.125;
        }
      }

      if (abs(boat.position.z - boss.position.z) > 0.2) {
        if (boat.position.z > boss.position.z) {
          boss.position.z += 0.125;
        }
        else if (boat.position.z < boss.position.z) {
          boss.position.z -= 0.125;
        }
      }

    }





    //Fireball collision with stones
    for (int i = 0; i < 18; i++) {
      if (!detect_collision(boat.bounding_box(), stones[i].bounding_box()) && detect_collision2(fireball.bounding_box(), stones[i].bounding_box())){
        stones[i].position.x = -1000;
        stones[i].position.z = -1000;
        stones[i].position.y = -15;

        fireball.speedx = 0;
        fireball.speedz = 0;

        fireball.position.x = -1000;
        fireball.position.y = -1000;
        fireball.position.z = -1000;

        points++;
      }
    }



    //Fireball collision with monsters
    for (int i = 0; i < 5; i++) {
      if (!detect_collision(boat.bounding_box(), monsters[i].bounding_box()) && detect_collision2(fireball.bounding_box(), monsters[i].bounding_box())) {
        monsters[i].position.x = -1000;
        monsters[i].position.z = -1000;
        monsters[i].position.y = -15;

        fireball.speedx = 0;
        fireball.speedz = 0;

        fireball.position.x = -1000;
        fireball.position.y = -1000;
        fireball.position.z = -1000;

        points+=5;

        dead[i] = 1;
      }
    }



    //Fireball colliding with boss
    if (!detect_collision(boat.bounding_box(), boss.bounding_box()) && detect_collision2(fireball.bounding_box(), boss.bounding_box())){

      if (boss.life > 0) {
        boss.life--;
      }
      if (boss.life == 0) {
        boss.position.x = -1000;
        boss.position.z = -1000;
        boss.position.y = -15;
      }

      fireball.speedx = 0;
      fireball.speedz = 0;

      fireball.position.x = -1000;
      fireball.position.y = -1000;
      fireball.position.z = -1000;

      points+=10;

      if (boss.life == 0) {
        points += 100;
      }
    }



    //Collided with nothing
    if (abs(fireball.position.x - shootposx) > 10*cos((shootangle*PI)/180)) {

      fireball.speedx = 0;
      fireball.speedz = 0;

      fireball.position.x = -1000;
      fireball.position.y = -1000;
      fireball.position.z = -1000;
    }


    ocean.tick();
    boat.tick();
    sail.tick();
    cannon.tick();

    for (int i = 0; i < 5; i++) {
      monsters[i].tick();
    }

    for (int i = 0; i < 15; i++) {
      gift_barrels[i].tick();
      gifts[i].tick();
    }

    for (int i = 0; i < 7; i++) {
      boost_barrels[i].tick();
      boosts[i].tick();
    }

    for (int i = 0; i < 7; i++) {
      med_barrels[i].tick();
      meds[i].tick();
    }

    fireball.tick();

    for (int i = 0; i < 18; i++) {
      stones[i].tick();
    }

    boss.tick();


    char s[1000];
    sprintf(s, "| Health : %d | Score: %d | Wind Intensity : %.2f | Wind Angle : %.2f | Cam : %s |", health, points, wind_intensity, wind_angle, cam[curr_angle]);
    glfwSetWindowTitle(window, s);
    //Print Game Status


    //Quit when no health
    if (health < 0) {
      std::cout<<"GAME OVER : Your final score is : "<<points<<"\n";
      quit(window);
    }

}

/* Initialize the OpenGL rendering properties */
/* Add all the models to be created here */
void initGL(GLFWwindow *window, int width, int height) {
    /* Objects should be created before any other gl function and shaders */
    // Create the models
    ocean = Block(0,-2,0, 1000, 1, 1000, COLOR_BLUE);
    boat = Block(0,-0.75,0,0.5,0.5,0.5,COLOR_BROWN);
    sail = Sail(0,-0.25,0, COLOR_WHITE);
    cannon = Cannon(0,-0.75,0,COLOR_BLACK);
    boss = Monster(-500, -500,-500, 3,3,3,5, COLOR_GREEN);

    for (int i = 0; i < 5; i++) {
      monsters[i] = Monster(randomPos(-70.0,-20.0), 0,randomPos(-50.0,50.0),0.5, 0.5,0.5, 1, COLOR_RED);
    }

    float bPosX = 0.0, bPosZ = 0.0;

    for (int i = 0; i < 15; i++) {
      bPosX = randomPos(-100.0,-10.0);
      bPosZ = randomPos(-100.0,100.0);
      gift_barrels[i] = Barrel(bPosX,-0.5,bPosZ, COLOR_CHOC_BROWN);
      gifts[i] = Gem(bPosX, 0.25, bPosZ, COLOR_FUCHSIA);
    }

    for (int i = 0; i < 7; i++) {
      bPosX = randomPos(-100.0,-10.0);
      bPosZ = randomPos(-100.0,100.0);
      boost_barrels[i] = Barrel(bPosX,-0.5,bPosZ, COLOR_CHOC_BROWN);
      boosts[i] = Gem(bPosX, 0.25, bPosZ, COLOR_RED);
    }

    for (int i = 0; i < 7; i++) {
      bPosX = randomPos(-100.0,-10.0);
      bPosZ = randomPos(-100.0,100.0);
      med_barrels[i] = Barrel(bPosX,-0.5,bPosZ, COLOR_CHOC_BROWN);
      meds[i] = Gem(bPosX, 0.25, bPosZ, COLOR_GREEN);
    }

    fireball = Block(-1000, -3, -1000, 0.05,0.05,0.05, COLOR_ORANGE);

    for (int i = 0; i < 18; i++) {
      stones[i] = Block(randomPos(-50,-10), 0, randomPos(-50,50),0.5,0.5,0.5, COLOR_BLACK);
    }

    //Default Camera Angle : Follow Cam ie. 3rd Person
    cam_x = follow_x + target_x;
    cam_y = follow_y + target_y;
    cam_z = follow_z + target_z;

    // Create and compile our GLSL program from the shaders
    programID = LoadShaders("Sample_GL.vert", "Sample_GL.frag");
    // Get a handle for our "MVP" uniform
    Matrices.MatrixID = glGetUniformLocation(programID, "MVP");
    Matrices.tr = glGetUniformLocation(programID, "tr");


    reshapeWindow (window, width, height);

    // Background color of the scene
    glClearColor (COLOR_BACKGROUND.r / 256.0, COLOR_BACKGROUND.g / 256.0, COLOR_BACKGROUND.b / 256.0, 0.0f); // R, G, B, A
    glClearDepth (1.0f);

    glEnable (GL_DEPTH_TEST);
    glDepthFunc (GL_LEQUAL);

    cout << "VENDOR: " << glGetString(GL_VENDOR) << endl;
    cout << "RENDERER: " << glGetString(GL_RENDERER) << endl;
    cout << "VERSION: " << glGetString(GL_VERSION) << endl;
    cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
}


int main(int argc, char **argv) {
    srand(time(0));
    int width  = 900;
    int height = 900;

    window = initGLFW(width, height);

    initGL (window, width, height);
    audio_init();

    glfwSetCursorPosCallback(window, cursorPositionCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallBack);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetScrollCallback(window, scrollCallback);

    GLFWcursor* cursor = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
    glfwSetCursor(window, cursor);

    /* Draw in loop */
    while (!glfwWindowShouldClose(window)) {
        audio_play();
        // Process timers
        // glfwGetCursorPos(window, &cur_xPos, &cur_yPos);
        if (t60.processTick()) {
            // 60 fps
            // OpenGL Draw commands
            draw();
            // Swap Frame Buffer in double buffering
            glfwSwapBuffers(window);

            tick_elements();
            tick_input(window);
        }

        // Poll for Keyboard and mouse events
        glfwPollEvents();
    }

    quit(window);
    // kill(0, SIGKILL);
}

bool detect_collision(bounding_box_t a, bounding_box_t b) {
    return (abs(a.x - b.x) * 2 < (a.length + b.length)) &&
           (abs(a.y - b.y) * 2 < (a.width + b.width)) &&
           (abs(a.z - b.z) * 2 < (a.height + b.height));
}

bool detect_collision1(bounding_box_t a, bounding_box_t b) {
    return (abs(a.x - b.x) * 2 < (a.length + b.length)) &&
         (abs(a.y - (b.y+1)) * 2 < (a.width + b.width)) &&
         (abs(a.z - b.z) * 2 < (a.height + b.height));
}

bool detect_collision2(bounding_box_t a, bounding_box_t b) {
  return (abs(a.x - b.x) * 2 < (1 + b.length)) &&
         (abs(a.y - b.y) * 2 < (1 + b.width)) &&
         (abs(a.z - b.z) * 2 < (1 + b.height));
}


void reset_screen() {
    // float top    = screen_center_y + 4 / screen_zoom;
    // float bottom = screen_center_y - 4 / screen_zoom;
    // float left   = screen_center_x - 4 / screen_zoom;
    // float right  = screen_center_x + 4 / screen_zoom;
    Matrices.projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 100.0f);
}

static void cursorPositionCallback(GLFWwindow *window, double xPos, double yPos){
  cur_xPos = xPos;
  cur_yPos = yPos;
}

void cursorEnterCallback(GLFWwindow *window, int entered) {
  // if (entered) {
  //   std::cout<<"Entered\n";
  // }
}

void mouseButtonCallBack(GLFWwindow *window, int button, int action, int mods){
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
      lmb_press = 1;
      double xpos, ypos;
      glfwGetCursorPos(window, &xpos, &ypos);
      prev_xPos = xpos;
      prev_yPos = ypos;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
      lmb_press = 0;
    }
}

void scrollCallback(GLFWwindow *window, double xOffset, double yOffset){

  //Condition to Work only on helicopter cam
  //To change Distance to scene / target
  if (curr_angle == 4) {
    if (yOffset < 0) {

      if (target_x < 0) {
        helicopter_x += 0.5;
      }
      else{
        helicopter_x -= 0.5;
      }

      if (target_y < 0) {
        helicopter_y += 0.5;
      }
      else{
        helicopter_y -= 0.5;
      }

      if (target_z < 0) {
        helicopter_z += 0.5;
      }
      else{
        helicopter_z -= 0.5;
      }

    }
    else if(yOffset > 0){

      if (target_x < 0) {
        helicopter_x -= 0.5;
      }
      else{
        helicopter_x += 0.5;
      }


      if (target_y < 0) {
        helicopter_y -= 0.5;
      }
      else{
        helicopter_y += 0.5;
      }

      if (target_z < 0) {
        helicopter_z -= 0.5;
      }
      else{
        helicopter_z += 0.5;
      }

    }
  }

}
