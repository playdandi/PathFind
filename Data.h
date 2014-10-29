#ifndef __PathFind__Data__
#define __PathFind__Data__

#include<algorithm>
#include<vector>
#include "cocos2d.h"

#define TILE_WIDTH 100
#define TILE_HEIGHT 100

#define NONE -1
#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

#define TAG_OBSTACLE 0

using namespace cocos2d;

class Player
{
public:
    Player(Sprite* sp, int x, int y);
    
    Sprite* sp;
    int x;
    int y;
};

class TileMap
{
public:
    TileMap(Sprite* sp, int x, int y, bool hasTile);
    
    Sprite* sp;
    int x;
    int y;
    bool hasTile;
};

class Cannon
{
public:
    Cannon(Sprite* sp, Sprite* arrow, LabelTTF* label, int x, int y, std::vector<int> direction, int now, int period);
    
    Sprite* sp;
    Sprite* arrow;
    LabelTTF* label;
    int x;
    int y;
    std::vector<int> direction;
    int now;
    int period;
    int idx;
    bool isAvailable;
};

class Wall
{
public:
    Wall(Sprite* sp, int x1, int y1, int x2, int y2);
    
    Sprite* sp;
    int x1, y1, x2, y2;
};

class Obstacle
{
public:
    Obstacle(Sprite* sp, int x, int y);
    
    Sprite* sp;
    int x, y;
    bool isAvailable;
};

class Bomb
{
public:
    Bomb(Sprite* sp, LabelTTF* label, int x, int y, int period);
    
    Sprite* sp;
    int x;
    int y;
    int period;
    LabelTTF* label;
    bool isIgnited;
    bool isAvailable;
};

class Teleport
{
public:
    Teleport(Sprite* sp1, Sprite* sp2, int x1, int y1, int x2, int y2);
    
    Sprite* sp1;
    Sprite* sp2;
    int x1, y1, x2, y2;
};

class SwitchDoor
{
public:
    SwitchDoor(Sprite* swi, Sprite* door, int sx, int sy, int dx1, int dy1, int dx2, int dy2);
    
    Sprite* swi;
    Sprite* door;
    int sx, sy;
    int dx1, dy1, dx2, dy2;
    bool isOpened;
};


class Common
{
public:
    static Point SetXY(int x, int y);
    static Point SetWallXY(int x, int y, bool isVertical);
    
    static bool IsWall(int x1, int y1, int x2, int y2);
    static bool IsObstacle(int x, int y);
    static bool IsDoor(int x1, int y1, int x2, int y2);
    static Point IsTeleport(int x, int y);
    
    static int OpenDoor(int x, int y);
    
    static bool IsEmptyTile(int x, int y);
    
    static void DeleteData();
};


extern Player* player;
extern std::vector< std::vector<class TileMap*> > map;

extern std::vector<class Cannon*> cannon;
extern std::vector<class Wall*> wall;
extern std::vector<class Bomb*> bomb;
extern std::vector<class Teleport*> teleport;
extern std::vector<class SwitchDoor*> switchdoor;
extern std::vector<class Obstacle*> obstacle;


extern int MAP_WIDTH;
extern int MAP_HEIGHT;
extern int curStage;

#endif /* defined(__PathFind__Data__) */
