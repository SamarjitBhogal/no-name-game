/***********************************************************************************************
*
**   ...
*   
*    @authors Marcus Vinicius Santos Lages and Samarjit Bhogal
*    @version 0.1.0
*
*    @include enemy.h
*
***********************************************************************************************/

#include "../include/enemy.h"

//* ------------------------------------------
//* GLOBAL VARIABLES

Entity enemy;

//* ------------------------------------------
//* MODULAR VARIABLES

/** The animation for an idle enemy. */
static Animation idleEnemyAnimation;

/** The animation for the enemy moving. */
static Animation movingEnemyAnimation;

/** The animation for a enemy attack. */
static Animation attackEnemyAnimation;

//* ------------------------------------------
//* FUNCTION IMPLEMENTATIONS

void EnemyStartup() {
    enemy.x = 50;
    enemy.y = 50;
    enemy.speed = 10;
    enemy.health = 100;
    enemy.direction = Vector2Zero();
    enemy.faceValue = 1;
    enemy.state = IDLE;
    enemy.directionFace = RIGHT;

    // Initializing the idle animation
    idleEnemyAnimation = CreateAnimation(
        DEFAULT_IDLE_FPS,
        ENTITY_TILE_WIDTH,
        ENTITY_TILE_HEIGHT,
        TILE_ENEMY_IDLE,
        textures[TILE_ENEMY_IDLE]);

    // Initializing the moving animation
    movingEnemyAnimation = CreateAnimation(
        DEFAULT_MOVING_FPS,
        ENTITY_TILE_WIDTH,
        ENTITY_TILE_HEIGHT,
        TILE_ENEMY_MOVE,
        textures[TILE_ENEMY_MOVE]);

    // Initializing the attacking animation
    attackEnemyAnimation = CreateAnimation(
        DEFAULT_ATTACK_FPS,
        TEMP_ATTACK_WIDTH,
        TEMP_ATTACK_HEIGHT,
        TILE_ENEMY_ATTACK,
        textures[TILE_ENEMY_ATTACK]);

    // Starting timers for both idle and moving animations
    StartTimer(&idleEnemyAnimation.timer, -1.0f);
    StartTimer(&movingEnemyAnimation.timer, -1.0f);
}

void EnemyMovement() {
    // Ensures the enemy cannot move while attacking   
    if (enemy.state == ATTACKING) return; 

    float deltaTime = GetFrameTime();
    enemy.direction = Vector2Zero();

    //enemy.direction.x = player.direction.x;
    //enemy.direction.y = player.direction.y;
   
    // if (enemy.x < player.x) {
    //     enemy.direction.x++;
    // } else if (enemy.x > player.x) {
    //     enemy.direction.x--;
    // }

    // if (enemy.y < player.y) {
    //     enemy.direction.y++;
    // } else if (enemy.y > player.y) {
    //     enemy.direction.y--;
    // }

    // Set the enemy to MOVING if not ATTACKING.
    enemy.state = enemy.state == ATTACKING ? ATTACKING : MOVING;
    
    // Set the enemy to IDLE if not ATTACKING or moving on any direction
    if(enemy.direction.x == 0 && enemy.direction.y == 0 && enemy.state != ATTACKING) {
        enemy.state = IDLE;
        return;
    }     

    enemy.direction = Vector2Normalize(enemy.direction);

    enemy.x += enemy.direction.x * enemy.speed * deltaTime;
    enemy.y += enemy.direction.y * enemy.speed * deltaTime;
}

void EnemyAttack() {
    
}

void EnemyRender() {
    switch (enemy.state) {
        case IDLE:
            EntityRender(&enemy, &idleEnemyAnimation, ENTITY_TILE_WIDTH * enemy.faceValue, 
                ENTITY_TILE_HEIGHT, 0, 0, 0.0f);
            break;
        case MOVING:
            EntityRender(&enemy, &movingEnemyAnimation, ENTITY_TILE_WIDTH * enemy.faceValue, 
                ENTITY_TILE_HEIGHT, 0, 0, 0.0f);
            break;
        case ATTACKING:
            
            break;
        default:
            break;
    }
}

void EnemyUnload() {
    AnimationUnload(&idleEnemyAnimation);
    AnimationUnload(&movingEnemyAnimation);
    AnimationUnload(&attackEnemyAnimation);
}