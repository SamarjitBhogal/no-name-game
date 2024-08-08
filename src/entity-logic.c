/**********************************************************************************************
 *
 **   entity-logic.c is responsible for detecting collisions related to general entities.
 *
 *    @authors Marcus Vinicius Santos Lages, Samarjit Bhogal
 *    @version 0.1
 *
 *    @include entity.h
 *
 **********************************************************************************************/

#include "../include/entity.h"

//* ------------------------------------------
//* DEFINITIONS

/**
 * Macro function to get the absolute/module value of a number.
 *
 * @param x Number
 */
#define ABS(x) (x > 0 ? x : x * (-1))

//* ------------------------------------------
//* FUNCTION IMPLEMENTATIONS

void UpdateEntityHitbox(Entity* entity) {
    entity->hitbox.x = entity->pos.x;
    entity->hitbox.y = entity->pos.y + entity->hitbox.height;
}

RayCollision2D EntityRectCollision(Entity entity, Rectangle hitboxTarget) {
    RayCollision2D collision;
    collision.hit = false;

    if(Vector2Equals(entity.direction, Vector2Zero())) return collision;

    UpdateEntityHitbox(&entity);

    collision = HitboxCollision(entity.hitbox, entity.direction, hitboxTarget);
    return collision;
}

RayCollision2D EntitiesCollision(Entity entityIn, Entity entityTarget) {
    RayCollision2D collision;
    collision.hit = false;

    if(Vector2Equals(entityIn.direction, Vector2Zero())) return collision;

    UpdateEntityHitbox(&entityIn);
    UpdateEntityHitbox(&entityTarget);

    collision = HitboxCollision(entityIn.hitbox, entityIn.direction, entityTarget.hitbox);
    return collision;
}

void EntityWorldCollision(Entity* entity) {
    if(collidableTiles == NULL) return;

    CollisionNode* entityCollisionList;
    entityCollisionList = NULL;

    CollisionNode* collidingTile;
    collidingTile = collidableTiles;

    while(collidingTile != NULL) {
        RayCollision2D entityCollision;
        Rectangle tileHitbox =
            (Rectangle){ .x = collidingTile->collidedHitbox.index.x * TILE_WIDTH,
                         .y = collidingTile->collidedHitbox.index.y * TILE_HEIGHT,
                         .width  = TILE_WIDTH,
                         .height = TILE_HEIGHT };

        entityCollision = EntityRectCollision(*entity, tileHitbox);
        if(entityCollision.hit == true && entityCollision.timeHit >= 0) {
            if(entityCollisionList == NULL)
                entityCollisionList = CreateCollisionList(
                    collidingTile->collidedHitbox.index.x,
                    collidingTile->collidedHitbox.index.y, entityCollision.timeHit);
            else
                AddCollisionNode(
                    entityCollisionList, collidingTile->collidedHitbox.index.x,
                    collidingTile->collidedHitbox.index.y, entityCollision.timeHit);
        }
        collidingTile = collidingTile->next;
    }

    if(entityCollisionList != NULL) {
        SortCollisionList(entityCollisionList);

        CollisionNode* resolvingNode = entityCollisionList;
        while(resolvingNode != NULL) {
            RayCollision2D entityCollision;
            Rectangle tileHitbox = (Rectangle){
                .x = world[(int) resolvingNode->collidedHitbox.index.y]
                          [(int) resolvingNode->collidedHitbox.index.x]
                              .x *
                    TILE_WIDTH,
                .y = world[(int) resolvingNode->collidedHitbox.index.y]
                          [(int) resolvingNode->collidedHitbox.index.x]
                              .y *
                    TILE_HEIGHT,
                .width  = TILE_WIDTH,
                .height = TILE_HEIGHT
            };

            entityCollision = EntityRectCollision(*entity, tileHitbox);
            if(entityCollision.hit == true && entityCollision.timeHit >= 0) {
                entity->direction.x += entityCollision.normalVector.x *
                    ABS(entity->direction.x) * (1 - entityCollision.timeHit);
                entity->direction.y += entityCollision.normalVector.y *
                    ABS(entity->direction.y) * (1 - entityCollision.timeHit);
            }
            resolvingNode = resolvingNode->next;
        }

        FreeCollisionList(entityCollisionList);
        entityCollisionList = NULL;
    }
}
