#include "Data.h"


Player::Player(Sprite* sp, int x, int y)
{
    this->x = x;
    this->y = y;
    this->sp = sp;
}


TileMap::TileMap(Sprite* sp, int x, int y, bool hasTile)
{
    this->sp = sp;
    this->x = x;
    this->y = y;
    this->hasTile = hasTile;
}

Cannon::Cannon(Sprite* sp, Sprite* arrow, LabelTTF* label, int x, int y, std::vector<int> direction, int now, int period)
{
    this->sp = sp;
    this->arrow = arrow;
    this->label = label;
    this->x = x;
    this->y = y;
    this->direction = direction;
    this->now = now;
    this->period = period;

    this->idx = 0;
    this->isAvailable = true;
}

Wall::Wall(Sprite* sp, int x1, int y1, int x2, int y2)
{
    this->sp = sp;
    this->x1 = x1;
    this->y1 = y1;
    this->x2 = x2;
    this->y2 = y2;
}

Bomb::Bomb(Sprite* sp, LabelTTF* label, int x, int y, int period)
{
    this->sp = sp;
    this->x = x;
    this->y = y;
    this->period = period;
    this->label = label;
    this->isAvailable = true;
    this->isIgnited = false;
}

Teleport::Teleport(Sprite* sp1, Sprite* sp2, int x1, int y1, int x2, int y2)
{
    this->sp1 = sp1;
    this->sp2 = sp2;
    this->x1 = x1;
    this->y1 = y1;
    this->x2 = x2;
    this->y2 = y2;
}

SwitchDoor::SwitchDoor(Sprite* swi, Sprite* door, int sx, int sy, int dx1, int dy1, int dx2, int dy2)
{
    this->swi = swi;
    this->door = door;
    this->sx = sx;
    this->sy = sy;
    this->dx1 = dx1;
    this->dy1 = dy1;
    this->dx2 = dx2;
    this->dy2 = dy2;
    this->isOpened = false;
}

Obstacle::Obstacle(Sprite* sp, int x, int y)
{
    this->sp = sp;
    this->x = x;
    this->y = y;
    this->isAvailable = true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////

Point Common::SetXY(int x, int y)
{
    int px = (x - MAP_WIDTH/2) * TILE_WIDTH;
    int py = (y - MAP_HEIGHT/2) * TILE_HEIGHT;
    if (MAP_WIDTH % 2 == 0)
        px += TILE_WIDTH/2;
    if (MAP_HEIGHT % 2 == 0)
        py += TILE_HEIGHT/2;
    return Point(px, py);
}
Point Common::SetWallXY(int x, int y, bool isVertical)
{
    Point p = Common::SetXY(x, y);
    
    if (isVertical)
        return Point(p.x+TILE_WIDTH/2, p.y);
    return Point(p.x, p.y+TILE_HEIGHT/2);
}

bool Common::IsWall(int x1, int y1, int x2, int y2)
{
    for (int i = 0 ; i < (int)wall.size() ; i++)
    {
        if ((wall[i]->x1 == x1 && wall[i]->y1 == y1 && wall[i]->x2 == x2 && wall[i]->y2 == y2) ||
            (wall[i]->x1 == x2 && wall[i]->y1 == y2 && wall[i]->x2 == x1 && wall[i]->y2 == y1))
            return true;
    }
    return false;
}
bool Common::IsObstacle(int x, int y)
{
    for (int i = 0 ; i < (int)obstacle.size() ; i++)
    {
        if (obstacle[i]->isAvailable && obstacle[i]->x == x && obstacle[i]->y == y)
            return true;
    }
    return false;
}
bool Common::IsDoor(int x1, int y1, int x2, int y2)
{
    for (int i = 0 ; i < (int)switchdoor.size() ; i++)
    {
        if ((switchdoor[i]->dx1 == x1 && switchdoor[i]->dy1 == y1 && switchdoor[i]->dx2 == x2 && switchdoor[i]->dy2 == y2) ||
            (switchdoor[i]->dx1 == x2 && switchdoor[i]->dy1 == y2 && switchdoor[i]->dx2 == x1 && switchdoor[i]->dy2 == y1))
        {
            if (!switchdoor[i]->isOpened)
                return true;
        }
    }
    return false;
}
Point Common::IsTeleport(int x, int y)
{
    for (int i = 0 ; i < (int)teleport.size() ; i++)
    {
        if (teleport[i]->x1 == x && teleport[i]->y1 == y)
            return Point(teleport[i]->x2, teleport[i]->y2);
        else if (teleport[i]->x2 == x && teleport[i]->y2 == y)
            return Point(teleport[i]->x1, teleport[i]->y1);
    }
    
    return Point(-1, -1);
}
int Common::OpenDoor(int x, int y)
{
    for (int i = 0 ; i < (int)switchdoor.size() ; i++)
    {
        if (switchdoor[i]->sx == x && switchdoor[i]->sy == y && !switchdoor[i]->isOpened)
        {
            switchdoor[i]->isOpened = true;
            return i;
        }
    }
    return -1;
}

void Common::DeleteData()
{
    player->sp->removeFromParentAndCleanup(true);
    delete player;
    
    for (int i = 0 ; i < (int)cannon.size() ; i++)
    {
        cannon[i]->direction.clear();
        //cannon[i]->sp->removeFromParentAndCleanup(true);
        cannon[i]->arrow->removeFromParentAndCleanup(true);
        cannon[i]->label->removeFromParentAndCleanup(true);
        delete cannon[i];
    }
    cannon.clear();
    
    for (int i = 0 ; i < (int)wall.size() ; i++)
    {
        wall[i]->sp->removeFromParentAndCleanup(true);
        delete wall[i];
    }
    wall.clear();
    
    for (int i = 0 ; i < (int)obstacle.size() ; i++)
    {
        obstacle[i]->sp->removeFromParentAndCleanup(true);
        delete obstacle[i];
    }
    obstacle.clear();
    
    for (int i = 0 ; i < (int)bomb.size() ; i++)
    {
        bomb[i]->sp->removeFromParentAndCleanup(true);
        delete bomb[i];
    }
    bomb.clear();
    
    for (int i = 0 ; i < (int)teleport.size() ; i++)
    {
        teleport[i]->sp1->removeFromParentAndCleanup(true);
        teleport[i]->sp2->removeFromParentAndCleanup(true);
        delete teleport[i];
    }
    teleport.clear();
    
    for (int i = 0 ; i < (int)switchdoor.size() ; i++)
    {
        switchdoor[i]->swi->removeFromParentAndCleanup(true);
        switchdoor[i]->door->removeFromParentAndCleanup(true);
        delete switchdoor[i];
    }
    switchdoor.clear();
    
    for (int i = 0 ; i < (int)map.size() ; i++)
    {
        for (int j = 0 ; j < (int)map[i].size() ; j++)
        {
            map[i][j]->sp->removeFromParentAndCleanup(true);
            delete map[i][j];
        }
        map[i].clear();
    }
    map.clear();
}

bool Common::IsEmptyTile(int x, int y)
{
    for (int i = 0 ; i < (int)obstacle.size() ; i++)
    {
        if (obstacle[i]->isAvailable && obstacle[i]->x == x && obstacle[i]->y == y)
            return false;
    }
    for (int i = 0 ; i < (int)bomb.size() ; i++)
    {
        if (bomb[i]->isAvailable && bomb[i]->x == x && bomb[i]->y == y)
            return false;
    }
    for (int i = 0 ; i < (int)cannon.size() ; i++)
    {
        if (cannon[i]->isAvailable && cannon[i]->x == x && cannon[i]->y == y)
            return false;
    }
    for (int i = 0 ; i < (int)teleport.size() ; i++)
    {
        if ((teleport[i]->x1 == x && teleport[i]->y1 == y) || (teleport[i]->x2 == x && teleport[i]->y2 == y))
            return false;
    }
    for (int i = 0 ; i < (int)switchdoor.size() ; i++)
    {
        if (switchdoor[i]->sx == x && switchdoor[i]->sy == y)
            return false;
    }
    
    return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////



Player* player;
std::vector< std::vector<class TileMap*> > map;

std::vector<class Cannon*> cannon;
std::vector<class Wall*> wall;
std::vector<class Obstacle*> obstacle;
std::vector<class Bomb*> bomb;
std::vector<class Teleport*> teleport;
std::vector<class SwitchDoor*> switchdoor;


int MAP_WIDTH;
int MAP_HEIGHT;
int curStage;

