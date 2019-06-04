# Graphics - Assignment 2
# Submitted by:
  * Aniruddha Deshpande - 20161058

# Controls

1. Up Arrow Key : Move Front.
2. Down Arrow Key : Move Backwards.
3. Left Arrow Key : Turn Left.
4. Right Arrow Key : Turn Right.
5. Space : Jump.
6. Mouse : Aim Cannon.
7. F : Shoot Fireball.
8. C : Toggle Camera Angle.
9. Left Mouse Button Click and Drag : Change Look Angle.
10. Mouse Scroll up : Move Camera Closer to the scene.
11. Mouse Scroll down : Move Camera Farther from the scene.

# Instructions

1. Cannon can be aimed only in the following angles :
   * Boat View (First Person View).
   * Top View.
   * Follow-Cam view (Third Person View).

2. Wind in a random direction with a random intensity will be set off every 5 seconds, which lasts for 5 seconds/
3. Collision to a rock costs the player '1' health point.
4. Collision to a monster costs the player '5' health points.
5. Collision to a boss costs the player '15' health points.
6. Boss spawns after all pawn monsters are eliminated.
7. Boss Qualities :
   * Bigger Alert Radius.
   * Higher Damage.
   * Faster.
   * 5 times more health than normal monster.
8. Monsters (Including Boss) follow and attack you if within alert radius.
9. Look Angle and Distance to target of the camera can be changed only in the Helicopter View.
10. Game is Over if player runs out of health points.
11. Gems spread around the ocean can be collected by jumping over the Gem Barrels.
12. Gem Types :
    * Purple Gem : Point gems (Point gifts).
    * Green Gem : Health gems.
    * Red Gem : Booster gems.
13. Point Distribution :
    * Stone Destroyed with fireball : 1 point.
    * Monster eliminated with fireball : 5 points.
    * Boss hit with fireball : 10 points.
    * Boss eliminated with fireball : 100 points.

# Bonus Implementation

1. Background music :
   * "Undertale OST: 016 - Nyeh Heh Heh!" : [https://www.youtube.com/watch?v=FKdtstAo6iU]

2. Sail and Ocean blended. (Translucency added).
