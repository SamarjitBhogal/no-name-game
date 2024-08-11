/**********************************************************************************************
 *
 **   dungeon.c is responsible for dealing and assemblying all functionalities related to the
 **   dungeon game screen.
 *
 *    @authors Marcus Vinicius Santos Lages, Samarjit Bhogal
 *    @version 0.1
 *
 *    @include screen.h, player.h
 *
 **********************************************************************************************/

#include "../include/enemy.h"
#include "../include/player.h"
#include "../include/screen.h"
#include "../include/tile.h"

//* ------------------------------------------
//* GLOBAL VARIABLES

/** 2D array of type Tile for the world level (texture.h) */
Tile** world;

/** Framebuffer (white canvas) for displaying the map. */
RenderTexture2D worldCanvas;

/** Linked list with indexes to all the possible collidable tiles. */
CollisionNode* collidableTiles;

/** A reference to the game's tilemap (texture.h) */
Texture2D* textures;

//* ------------------------------------------
//* FUNCTION PROTOTYPES

/**
 * Starts the 2D camera by initializing it.
 */
static void StartCamera();

/**
 * Loads all of the textures required for the dungeon by populating the array of textures.
 */
static void LoadTextures();

/**
 * Allocates all the tiles into the world array and the collidableTiles list.
 */
static void InitializeTiles();

//* ------------------------------------------
//* FUNCTION IMPLEMENTATIONS

void DungeonStartup() {
    // Sets the current screen
    currentScreen = DUNGEON;

    // Allocate memory for the game's textures as Texture2D
    textures = (Texture2D*) malloc(MAX_TEXTURES * sizeof(Texture2D));

    // Allocate memory for the word as a 2D array of Tile
    world = (Tile**) malloc(WORLD_WIDTH * sizeof(Tile*));

    // Assign initial null value value for the collidableTiles list
    collidableTiles = NULL;

    // Populating textures array with the texture images
    LoadTextures();

    // Allocating tiles of type Tile into 2D array
    InitializeTiles();

    StartCamera();
    EnemyStartup();
    PlayerStartup();
}

void DungeonUpdate() {
    PlayerMovement();
    EnemyMovement();
    EnemyAttack();
    PlayerAttack();

    // Update camera to follow the player
    camera.target = (Vector2){ (int) player.pos.x + 8, (int) player.pos.y + 16 };
}

void DungeonRender() {
    // Render the world canvas
    // ? Note: height is negative because OpenGL orientation is inverted from Raylib
    DrawTextureRec(
        worldCanvas.texture,
        (Rectangle){ 0, 0, worldCanvas.texture.width, -worldCanvas.texture.height },
        (Vector2){ 0.0f, 0.0f }, WHITE
    );

    // Draw player on the screen
    EnemyRender();
    PlayerRender();
}

void DungeonUnload() {
    // Unloads the player sprites and animations.
    PlayerUnload();

    // Unloads the enemy sprites and animations.
    EnemyUnload();

    // Unloads all tiles
    for(int j = 0; j < WORLD_HEIGHT; j++) {
        free(world[j]);
        world[j] = NULL;
    }
    free(world);
    world = NULL;

    // Unloads collidableTiles list
    FreeCollisionList(collidableTiles);
    collidableTiles = NULL;

    // Unloads texture array
    for(int i = 0; i < MAX_TEXTURES; i++) {
        UnloadTexture(textures[i]);
    }
    free(textures);
    textures = NULL;

    UnloadRenderTexture(worldCanvas);
}

static void StartCamera() {
    camera.target   = (Vector2){ player.pos.x + 8, player.pos.y + 16 };
    camera.offset   = (Vector2){ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
    camera.rotation = 0.0f;
    camera.zoom     = 4.0f;
}

static void LoadTextures() {
    // TODO: Take out the tilemap image from textures array
    Image img          = LoadImage("resources/tilemap.png");
    textures[TILE_MAP] = LoadTextureFromImage(img);
    UnloadImage(img);

    img                        = LoadImage("resources/player-idle.png");
    textures[TILE_PLAYER_IDLE] = LoadTextureFromImage(img);
    UnloadImage(img);

    img                        = LoadImage("resources/player-movement.png");
    textures[TILE_PLAYER_MOVE] = LoadTextureFromImage(img);
    UnloadImage(img);

    img                          = LoadImage("resources/player-attack.png");
    textures[TILE_PLAYER_ATTACK] = LoadTextureFromImage(img);
    UnloadImage(img);

    img                       = LoadImage("resources/enemy-1-idle.png");
    textures[TILE_ENEMY_IDLE] = LoadTextureFromImage(img);
    UnloadImage(img);

    img                       = LoadImage("resources/enemy-1-movement.png");
    textures[TILE_ENEMY_MOVE] = LoadTextureFromImage(img);
    UnloadImage(img);

    //! NOTE: player attack is used here temporailly. New asset must be found.
    img                         = LoadImage("resources/player-attack.png");
    textures[TILE_ENEMY_ATTACK] = LoadTextureFromImage(img);
    UnloadImage(img);
}

static void InitializeTiles() {
    tmx_map* mapTmx;
    mapTmx = TmxMapFrameBufStartup(&worldCanvas, "resources/map/map.tmx");
    TmxMapFrameBufRender(&worldCanvas, mapTmx);

    // Frees the map.tmx
    // ! ATTENTION: Pay attention if there will be any problem with the
    // ! UnloadMapTexture() from tile.c
    tmx_map_free(mapTmx);
    mapTmx = NULL;

    // Delete tiles and world
    for(int j = 0; j < WORLD_HEIGHT; j++) {
        Tile* tiles = (Tile*) malloc(WORLD_HEIGHT * sizeof(Tile));
        for(int i = 0; i < WORLD_WIDTH; i++) {
            Tile tile = { .x = i, .y = j, .isCollidable = (j != 3) ? false : true };
            tiles[i] = tile;
        }
        world[j] = tiles;
    }
}

void DrawTile(int xPos, int yPos, int textureTileX, int textureTileY, TextureFile tileTexture) {
    Rectangle source = { (float) (textureTileX * TILE_WIDTH),
                         (float) (textureTileY * TILE_HEIGHT), (float) TILE_WIDTH,
                         (float) (tileTexture > TILE_MAP ? ENTITY_TILE_HEIGHT : TILE_HEIGHT) };
    Rectangle dest   = { (float) xPos, (float) yPos, (float) TILE_WIDTH,
                         (float) (tileTexture > TILE_MAP ? ENTITY_TILE_HEIGHT : TILE_HEIGHT) };
    Vector2 origin   = { 0, 0 };
    DrawTexturePro(textures[tileTexture], source, dest, origin, 0.0f, WHITE);
}