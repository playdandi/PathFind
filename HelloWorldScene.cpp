#include "HelloWorldScene.h"
#include "pugixml/pugixml.hpp"

USING_NS_CC;

using namespace pugi;

static int stageNumber;

Scene* HelloWorld::createScene(int stage)
{
    stageNumber = stage;
    
    auto scene = Scene::create();
    auto layer = HelloWorld::create();
    scene->addChild(layer);
    return scene;
}

bool HelloWorld::init()
{
    if ( !LayerColor::initWithColor(Color4B(255,255,255,255)) )
    {
        return false;
    }
    
    visibleSize = Director::getInstance()->getVisibleSize();
    origin = Director::getInstance()->getVisibleOrigin();
    
    // notification observer
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(HelloWorld::Notification), "Game", NULL);
    
    tileLayer = Layer::create();
    tileLayer->setPosition(Point(origin.x + visibleSize.width/2, origin.y + visibleSize.height/2));
    this->addChild(tileLayer);
    
    InitStage();
    
    int mapmap = 0;
    for (int i = 0 ; i < (int)map.size() ; i++)
        mapmap += (int)map[i].size();
    
    
    auto goal = LabelTTF::create("goal", "Market Felt", 36);
    goal->setPosition(Common::SetXY(end_p.x, end_p.y));
    goal->setColor(Color3B(255,255,255));
    tileLayer->addChild(goal, 10);
    
    
    // init turn
    isLiving = true;
    curTime = 0;
    curTimeLabel = LabelTTF::create("0", "Marker Felt", 42);
    curTimeLabel->setAnchorPoint(Point(1, 1));
    curTimeLabel->setPosition(Point(origin.x + visibleSize.width - 10, origin.y + visibleSize.height - 10));
    curTimeLabel->setColor(Color3B(0,0,0));
    this->addChild(curTimeLabel, 0);
    
    touched = false;
    obstacle_touched = false;
    
    // init touch
    InitTouch();

    return true;
}

void HelloWorld::Notification(CCObject* obj)
{
    CCString* param = (CCString*)obj;
    
    if (param->intValue() == 0)
    {
        PopScene();
        /*
         // 터치 활성
         CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority()+1, true);
         this->setTouchPriority(Depth::GetCurPriority());
         isTouched = false;
         isKeybackTouched = false;
         */
    }
}


void HelloWorld::InitStage()
{
    char temp[30];
    sprintf(temp, "stage_%d.xml", stageNumber);
    
    std::vector<std::string> searchPaths;
    searchPaths.push_back("stage/");
    FileUtils::getInstance()->setSearchResolutionsOrder(searchPaths);
    
    std::string filepath = FileUtils::getInstance()->fullPathForFilename(temp);
    //CCLOG("filepath = %s", filepath.c_str());
    
    ssize_t size = 0;
    unsigned char* pData;
    pData = FileUtils::getInstance()->getFileData(filepath.c_str(), "rb", &size);
    
    xml_document xmlDoc;
    xml_parse_result result = xmlDoc.load_buffer(pData, size);
    if (!result)
    {
        CCLOG("%s", result.description());
        return;
    }

    xml_node nodeResult = xmlDoc.child("stage");
    
    // map size
    MAP_WIDTH = nodeResult.child("size").attribute("width").as_int();
    MAP_HEIGHT = nodeResult.child("size").attribute("height").as_int();
    
    // init map
    InitMap();
    
    // start, end point
    start_p = Point( nodeResult.child("start").attribute("x").as_int(), nodeResult.child("start").attribute("y").as_int() );
    end_p = Point( nodeResult.child("end").attribute("x").as_int(), nodeResult.child("end").attribute("y").as_int() );
    
    // rating
    rating_three = nodeResult.child("rating").attribute("three").as_int();
    rating_two = nodeResult.child("rating").attribute("two").as_int();
    
    // make player
    auto sp = Sprite::create("goddess.png");
    sp->setScale(0.3f);
    sp->setPosition(Common::SetXY((int)start_p.x, (int)start_p.y));
    tileLayer->addChild(sp, 3);
    player = new Player(sp, (int)start_p.x, (int)start_p.y);
    
    // cannon, nospace 개수
    int cannons = nodeResult.child("data").attribute("cannon").as_int();
    int nospaces = nodeResult.child("data").attribute("nospace").as_int();
    int walls = nodeResult.child("data").attribute("wall").as_int();
    int bombs = nodeResult.child("data").attribute("bomb").as_int();
    int teleports = nodeResult.child("data").attribute("teleport").as_int();
    int switchdoors = nodeResult.child("data").attribute("switchdoor").as_int();
    int obstacles = nodeResult.child("data").attribute("obstacle").as_int();
    int x, y, x1, y1, x2, y2, now, period;
    
    // cannon
    std::string directions;
    xml_node nr;
    for (int i = 0 ; i < cannons ; i++)
    {
        sprintf(temp, "cannon_%d", i+1);
        nr = nodeResult.child(temp);
        x = nr.attribute("x").as_int();
        y = nr.attribute("y").as_int();
        now = nr.attribute("start").as_int();
        period = nr.attribute("period").as_int();

        // 방향 숫자들 컴마(,) split
        directions = nr.attribute("directions").as_string();
        int idx;
        std::vector<int> d;
        while (1)
        {
            idx = directions.find(",");
            if (idx != -1)
            {
                d.push_back( atoi(directions.substr(0, idx).c_str()) );
                directions = directions.substr(idx+1);
            }
            else
            {
                d.push_back( atoi(directions.c_str()) );
                break;
            }
        }
        
        auto arrow = Sprite::create("arrow.png");
        arrow->setScale(0.8f);
        arrow->setPosition(Common::SetXY(x, y));
        arrow->setRotation(d[0]);
        tileLayer->addChild(arrow, 1);
        auto label = LabelTTF::create("", "Marker Felt", 36);
        sprintf(temp, "%d", now);
        label->setPosition(Common::SetXY(x, y));
        label->setString(temp);
        tileLayer->addChild(label, 2);
        
        cannon.push_back( new Cannon(sp, arrow, label, x, y, d, now, period) );
    }
    // no space
    for (int i = 0 ; i < nospaces ; i++)
    {
        sprintf(temp, "nospace_%d", i+1);
        x = nodeResult.child(temp).attribute("x").as_int();
        y = nodeResult.child(temp).attribute("y").as_int();
        map[x][y]->hasTile = false;
        map[x][y]->sp->setVisible(false);
    }
    // wall
    for (int i = 0 ; i < walls ; i++)
    {
        sprintf(temp, "wall_%d", i+1);
        x1 = nodeResult.child(temp).attribute("x1").as_int();
        x2 = nodeResult.child(temp).attribute("x2").as_int();
        y1 = nodeResult.child(temp).attribute("y1").as_int();
        y2 = nodeResult.child(temp).attribute("y2").as_int();
        
        auto sp = Sprite::create("wall.png");
        if (y1 == y2) // 세로벽
        {
            sp->setPosition( Common::SetWallXY(MIN(x1, x2), y1, true) );
        }
        else // 가로벽
        {
            sp->setRotation(90);
            sp->setPosition( Common::SetWallXY(x1, MIN(y1, y2), false) );
        }
        tileLayer->addChild(sp, 2);
        
        wall.push_back( new Wall(sp, x1, y1, x2, y2) );
    }
    // bomb
    for (int i = 0 ; i < bombs ; i++)
    {
        sprintf(temp, "bomb_%d", i+1);
        x = nodeResult.child(temp).attribute("x").as_int();
        y = nodeResult.child(temp).attribute("y").as_int();
        period = nodeResult.child(temp).attribute("period").as_int();
        
        auto sp = Sprite::create("bomb.png");
        sp->setPosition(Common::SetXY(x, y));
        tileLayer->addChild(sp, 1);
        auto label = LabelTTF::create("", "Marker Felt", 36);
        label->setPosition(Common::SetXY(x, y));
        sprintf(temp, "%d", period);
        label->setString(temp);
        label->setColor(Color3B(255,255,255));
        tileLayer->addChild(label, 1);
        
        bomb.push_back( new Bomb(sp, label, x, y, period) );
    }
    // teleport
    for (int i = 0 ; i < teleports ; i++)
    {
        sprintf(temp, "teleport_%d", i+1);
        x1 = nodeResult.child(temp).attribute("x1").as_int();
        x2 = nodeResult.child(temp).attribute("x2").as_int();
        y1 = nodeResult.child(temp).attribute("y1").as_int();
        y2 = nodeResult.child(temp).attribute("y2").as_int();
        
        auto sp1 = Sprite::create("teleport.png");
        sp1->setPosition(Common::SetXY(x1, y1));
        tileLayer->addChild(sp1, 1);
        auto sp2 = Sprite::create("teleport.png");
        sp2->setPosition(Common::SetXY(x2, y2));
        tileLayer->addChild(sp2, 1);
        
        teleport.push_back( new Teleport(sp1, sp2, x1, y1, x2, y2) );
    }
    // switchdoor
    for (int i = 0 ; i < switchdoors ; i++)
    {
        sprintf(temp, "switchdoor_%d", i+1);
        x = nodeResult.child(temp).attribute("sx").as_int();
        y = nodeResult.child(temp).attribute("sy").as_int();
        x1 = nodeResult.child(temp).attribute("dx1").as_int();
        x2 = nodeResult.child(temp).attribute("dx2").as_int();
        y1 = nodeResult.child(temp).attribute("dy1").as_int();
        y2 = nodeResult.child(temp).attribute("dy2").as_int();
        
        auto swi = Sprite::create("switch.png");
        swi->setPosition(Common::SetXY(x, y));
        tileLayer->addChild(swi, 1);
        auto door = Sprite::create("door.png");
        if (y1 == y2) // 세로문
        {
            door->setPosition( Common::SetWallXY(MIN(x1, x2), y1, true) );
        }
        else // 가로문
        {
            door->setRotation(90);
            door->setPosition( Common::SetWallXY(x1, MIN(y1, y2), false) );
        }
        tileLayer->addChild(door, 5);

        switchdoor.push_back( new SwitchDoor(swi, door, x, y, x1, y1, x2, y2) );
    }
    // obstacle
    for (int i = 0 ; i < obstacles ; i++)
    {
        sprintf(temp, "obstacle_%d", i+1);
        x = nodeResult.child(temp).attribute("x").as_int();
        y = nodeResult.child(temp).attribute("y").as_int();
        
        auto sp  = Sprite::create("obstacle.png");
        sp->setPosition(Common::SetXY(x, y));
        tileLayer->addChild(sp, 1);
        
        obstacle.push_back( new Obstacle(sp, x, y) );
    }
    
    // obstacle install
    obstacle_install = nodeResult.child("data").attribute("obstacle_install").as_int();
    obstacle_sp = NULL;
    obstacle_label = NULL;
    if (obstacle_install > 0)
    {
        obstacle_sp = Sprite::create("obstacle.png");
        obstacle_sp->setPosition(Point(origin.x+100, origin.y+visibleSize.height/2));
        obstacle_sp->setTag(TAG_OBSTACLE);
        this->addChild(obstacle_sp, 0);
        
        obstacle_label = LabelTTF::create("", "Marker Felt", 36);
        obstacle_label->setPosition(Point(origin.x+100, origin.y+visibleSize.height/2));
        sprintf(temp, "%d", obstacle_install);
        obstacle_label->setString(temp);
        obstacle_label->setColor(Color3B(0,0,0));
        this->addChild(obstacle_label, 0);
    }
}


void HelloWorld::InitMap()
{
    // 변수 초기화
    for (int i = 0 ; i < (int)map.size() ; i++)
    {
        for (int j = 0 ; j < (int)map[i].size() ; j++)
            delete map[i][j];
        map[i].clear();
    }
    map.clear();
    
    for (int x = 0 ; x < MAP_WIDTH ; x++)
    {
        std::vector<class TileMap*> tiles;
        for (int y = 0 ; y < MAP_HEIGHT ; y++)
        {
            auto sp = Sprite::create("tile.png");
            sp->setPosition(Common::SetXY(x, y));
            tileLayer->addChild(sp, 0);
            tiles.push_back( new TileMap(sp, x, y, true) );
        }
        map.push_back(tiles);
    }
}

void HelloWorld::InitTouch()
{
    // 디스패처 : 리스너와 오브젝트를 연결해주는 역할
    EventDispatcher* dispatcher = Director::getInstance()->getEventDispatcher();
    
    // 터치 위치를 알려주는 리스너. 단일 터치.
    auto positionListener = EventListenerTouchOneByOne::create();
        
    // zOrder에 따라 밑에 깔린 애도 동작할지 아닐지를 결정한다.
    // positionListener->setSwallowTouches(true);
        
    // 콜백 함수 대입
    positionListener->onTouchBegan = CC_CALLBACK_2(HelloWorld::touchBegan, this);
        
    // 디스패처를 이용해 객체와 리스너를 이어준다. 화면 전체를 터치할 수 있게 만들어야 하므로 객체는 this
    dispatcher->addEventListenerWithSceneGraphPriority(positionListener, this);
}

bool HelloWorld::touchBegan(Touch* pTouch, Event* pEvent)
{
    if (touched)
        return true;
    touched = true;
    
    auto target = pEvent->getCurrentTarget();
    Point pos = target->convertToNodeSpace(pTouch->getLocation());
    //CCLOG("pos = %d %d", (int)pos.x, (int)pos.y);
    //Rect rect = Rect(0, 0, target->getContentSize().width, target->getContentSize().height);
    
    // obstacle 터치인가?
    //CCLOG("sp = %.1f , %.1f", obstacle_sp->getPositionX(), obstacle_sp->getPositionY());
    //Point obs_pos = obstacle_sp->convertToNodeSpace(pos);
    if (obstacle_install > 0 && !obstacle_touched && obstacle_sp->boundingBox().containsPoint(pos))
    {
        CCLOG("obs click");
        obstacle_touched = true;
    }
    else
    {
        for (int x = 0 ; x < MAP_WIDTH ; x++)
        {
            for (int y = 0 ; y < MAP_HEIGHT ; y++)
            {
                if (obstacle_touched)
                {
                    Point p = map[x][y]->sp->convertToNodeSpace(pos);
                    if (p.x >= 0 && p.x <= TILE_WIDTH && p.y >= 0 && p.y <= TILE_HEIGHT)
                    {
                        if (Common::IsEmptyTile(x, y)) // 지정된 곳이 빈 타일이면 장애물 설치.
                        {
                            auto sp = Sprite::create("obstacle.png");
                            sp->setPosition(Common::SetXY(x, y));
                            tileLayer->addChild(sp, 5);
                            obstacle.push_back( new Obstacle(sp, x, y) );
                            
                            obstacle_install--;
                            char temp[7];
                            sprintf(temp, "%d", obstacle_install);
                            obstacle_label->setString(temp);
                            
                            obstacle_touched = false;
                            touched = false;
                            return true;
                        }
                    }
                }
                else
                {
                    if (abs(x-player->x)+abs(y-player->y) == 1 && map[x][y]->hasTile)
                    {
                        // wall, obstacle, 막힌 door, 모두 없어야 이동 가능하다.
                        if (!Common::IsWall(x, y, player->x, player->y) && !Common::IsObstacle(x, y) &&
                            !Common::IsDoor(x, y, player->x, player->y))
                        {
                            Point p = map[x][y]->sp->convertToNodeSpace(pos);
                            if (p.x >= 0 && p.x <= TILE_WIDTH && p.y >= 0 && p.y <= TILE_HEIGHT)
                            {
                                int px = x - player->x;
                                int py = y - player->y;
                                player->x = x;
                                player->y = y;
                                CCActionInterval* action = CCSequence::create(MoveBy::create(0.5f, Point(px*100, py*100)), CCCallFuncND::create(this, callfuncND_selector(HelloWorld::AfterMove), this), NULL);
                                player->sp->runAction(action);
                                
                                return true;
                            }
                        }
                    }
                }
            }
        }
    }
    
    touched = false;

    return true;
}

void HelloWorld::AfterMove(Node* sender, void* ptr)
{
    Point teleport_p;
    if ((teleport_p = Common::IsTeleport(player->x, player->y)) != Point(-1, -1))
    {
        CCActionInterval* action = CCSequence::create(FadeOut::create(0.5f), Place::create(Common::SetXY(teleport_p.x, teleport_p.y)), DelayTime::create(0.3f), FadeIn::create(0.5f), CCCallFuncND::create(this, callfuncND_selector(HelloWorld::AfterPreAction), ptr), NULL);
        player->sp->runAction(action);
        player->x = teleport_p.x;
        player->y = teleport_p.y;
        return;
    }

    int switchIdx;
    if ((switchIdx = Common::OpenDoor(player->x, player->y)) != -1)
    {
        CCActionInterval* action = CCSequence::create(FadeOut::create(0.7f), CCCallFuncND::create(this, callfuncND_selector(HelloWorld::AfterPreAction), ptr), NULL);
        switchdoor[switchIdx]->door->runAction(action);
        return;
    }
    
    ((HelloWorld*)ptr)->BombCheck();
    
    if (isLiving)
        ((HelloWorld*)ptr)->ShootFunction(ptr);
    else
        PlayerDead();
}

void HelloWorld::AfterPreAction(cocos2d::Node *sender, void *ptr)
{
    ((HelloWorld*)ptr)->BombCheck();
    
    if (isLiving)
        ((HelloWorld*)ptr)->ShootFunction(ptr);
    else
        PlayerDead();
}

void HelloWorld::BombCheck()
{
    char temp[10];
    
    // 먼저 점화할 것은 점화시키고, 점화된 것은 카운트를 1씩 줄인다.
    for (int i = 0 ; i < (int)bomb.size() ; i++)
    {
        if (!bomb[i]->isIgnited && abs(player->x-bomb[i]->x)+abs(player->y-bomb[i]->y) == 1)
        {
            bomb[i]->isIgnited = true;
            bomb[i]->sp->runAction( RepeatForever::create( CCSequence::create(TintTo::create(0.5f, 255, 0, 0), TintTo::create(0.5f, 255,255,255), NULL)) );
        }
        else if (bomb[i]->isIgnited)
        {
            bomb[i]->period--;
            sprintf(temp, "%d", bomb[i]->period);
            bomb[i]->label->setString(temp);
        }
    }
    
    // 카운트가 0이 된 폭탄들을 터뜨린다.
    bool flag = false;
    while (!flag)
    {
        flag = true;
        for (int i = 0 ; i < (int)bomb.size() ; i++)
        {
            if (bomb[i]->period == 0 && bomb[i]->isAvailable)
            {
                flag = false;
                for (int x = bomb[i]->x-1 ; x <= bomb[i]->x+1 ; x++)
                {
                    for (int y = bomb[i]->y-1 ; y <= bomb[i]->y+1 ; y++)
                    {
                        Detonate(x, y);
                    }
                }
                
                bomb[i]->sp->setVisible(false);
                bomb[i]->label->setVisible(false);
                bomb[i]->isAvailable = false;
            }
        }
    }
}
void HelloWorld::Detonate(int x, int y) // 장애물, 캐넌 제거 및 플레이어 검사
{
    for (int i = 0 ; i < (int)obstacle.size() ; i++)
    {
        if (obstacle[i]->x == x && obstacle[i]->y == y)
        {
            obstacle[i]->sp->setVisible(false);
            obstacle[i]->isAvailable = false;
        }
    }
    for (int i = 0 ; i < (int)cannon.size() ; i++)
    {
        if (cannon[i]->x == x && cannon[i]->y == y)
        {
            cannon[i]->arrow->setVisible(false);
            cannon[i]->label->setVisible(false);
            cannon[i]->isAvailable = false;
        }
    }
    for (int i = 0 ; i < (int)bomb.size() ; i++) // 다른 폭탄이 범위에 들어오면 period = 0 으로 바꿔, 역시 터뜨리도록 한다.
    {
        if (bomb[i]->x == x && bomb[i]->y == y)
        {
            bomb[i]->period = 0;
        }
    }
    if (player->x == x && player->y == y)
    {
        isLiving = false;
    }
}

void HelloWorld::ShootFunction(void* ptr)
{
    bool flag = true;
    
    for (int i = 0 ; i < (int)cannon.size() ; i++)
    {
        if (cannon[i]->isAvailable && cannon[i]->now == 1) // i-th cannon 발사!
        {
            Shoot(i, ptr, flag);
            flag = false;
        }
    }
    
    if (flag) // 발사할 캐넌이 없는 경우
    {
        RenewCannonState();
        touched = false;
        
        if (player->x == (int)end_p.x && player->y == (int)end_p.y)
        {
            StageSuccess();
        }
    }
}


void HelloWorld::Shoot(int cidx, void* ptr, bool flag)
{
    HelloWorld* pThis = (HelloWorld*)ptr;
    
    int x = cannon[cidx]->x;
    int y = cannon[cidx]->y;
    Point pos = map[x][y]->sp->getPosition(); // + Vec2(origin.x+visibleSize.width/2, origin.y+visibleSize.height/2);
    
    ActionInterval* action;
    int xx = x;
    int yy = y;
    switch (cannon[cidx]->direction[cannon[cidx]->idx])
    {
        case 0: // UP
            action = MoveTo::create(1.0f, Point(pos.x, visibleSize.height/2));
            for (int yy = y+1 ; yy < MAP_HEIGHT ; yy++)
            {
                if (Common::IsWall(x, yy-1, x, yy) || Common::IsDoor(x, yy-1, x, yy))
                {
                    action = MoveBy::create(1.0f, Point(0, (yy-y)*TILE_HEIGHT-TILE_HEIGHT/2));
                    break;
                }
                if (Common::IsObstacle(x, yy))
                {
                    action = MoveBy::create(1.0f, Point(0, (yy-y)*TILE_HEIGHT));
                    break;
                }
                if (x == player->x && yy == player->y)
                {
                    action = MoveBy::create(1.0f, Point(0, (player->y-y)*TILE_HEIGHT));
                    isLiving = false;
                    break;
                }
            }
            break;
            
        case 90: // RIGHT
            action = MoveTo::create(1.0f, Point(visibleSize.width/2, pos.y));
            for (int xx = x+1 ; xx < MAP_WIDTH ; xx++)
            {
                if (Common::IsWall(xx-1, y, xx, y) || Common::IsDoor(xx-1, y, xx, y))
                {
                    action = MoveBy::create(1.0f, Point((xx-x)*TILE_WIDTH-TILE_WIDTH/2, 0));
                    break;
                }
                if (Common::IsObstacle(xx, y))
                {
                    action = MoveBy::create(1.0f, Point((xx-x)*TILE_WIDTH, 0));
                    break;
                }
                if (xx == player->x && y == player->y)
                {
                    action = MoveBy::create(1.0f, Point((player->x-x)*TILE_WIDTH, 0));
                    isLiving = false;
                    break;
                }
            }
            break;
            
        case 180: // DOWN
            action = MoveTo::create(1.0f, Point(pos.x, -visibleSize.height/2));
            for (int yy = y-1 ; yy >= 0 ; yy--)
            {
                if (Common::IsWall(x, yy+1, x, yy) || Common::IsDoor(x, yy+1, x, yy))
                {
                    action = MoveBy::create(1.0f, Point(0, (yy-y)*TILE_HEIGHT+TILE_HEIGHT/2));
                    break;
                }
                if (Common::IsObstacle(x, yy))
                {
                    action = MoveBy::create(1.0f, Point(0, (yy-y)*TILE_HEIGHT));
                    break;
                }
                if (x == player->x && yy == player->y)
                {
                    action = MoveBy::create(1.0f, Point(0, (player->y-y)*TILE_HEIGHT));
                    isLiving = false;
                    break;
                }
            }
            break;
            
        case 270: // LEFT
            action = MoveTo::create(1.0f, Point(-visibleSize.width/2, pos.y));
            for (int xx = x-1 ; xx >= 0 ; xx--)
            {
                if (Common::IsWall(xx+1, y, xx, y) || Common::IsDoor(xx+1, y, xx, y))
                {
                    action = MoveBy::create(1.0f, Point((xx-x)*TILE_WIDTH+TILE_WIDTH/2, 0));
                    break;
                }
                if (Common::IsObstacle(xx, y))
                {
                    action = MoveBy::create(1.0f, Point((xx-x)*TILE_WIDTH, 0));
                    break;
                }
                if (xx == player->x && y == player->y)
                {
                    action = MoveBy::create(1.0f, Point((player->x-x)*TILE_WIDTH, 0));
                    isLiving = false;
                    break;
                }
            }
            break;
            
        case 45: // RIGHT-UP
            action = MoveTo::create(1.0f, Point(visibleSize.width/2, visibleSize.height/2));
            while (xx >= 0 && xx < MAP_WIDTH && yy >= 0 && yy < MAP_HEIGHT)
            {
                if (Common::IsObstacle(xx, yy))
                {
                    action = MoveBy::create(1.0f, Point((xx-x)*TILE_WIDTH, (yy-y)*TILE_HEIGHT));
                    break;
                }
                if (xx == player->x && yy == player->y)
                {
                    action = MoveBy::create(1.0f, Point((player->x-x)*TILE_WIDTH, (player->y-y)*TILE_HEIGHT));
                    isLiving = false;
                    break;
                }
                xx++; yy++;
            }
            break;
        case 135: // RIGHT-DOWN
            action = MoveTo::create(1.0f, Point(visibleSize.width/2, -visibleSize.height/2));
            while (xx >= 0 && xx < MAP_WIDTH && yy >= 0 && yy < MAP_HEIGHT)
            {
                if (Common::IsObstacle(xx, yy))
                {
                    action = MoveBy::create(1.0f, Point((xx-x)*TILE_WIDTH, (yy-y)*TILE_HEIGHT));
                    break;
                }
                if (xx == player->x && yy == player->y)
                {
                    action = MoveBy::create(1.0f, Point((player->x-x)*TILE_WIDTH, (player->y-y)*TILE_HEIGHT));
                    isLiving = false;
                    break;
                }
                xx++; yy--;
            }
            break;
            
        case 225: // LEFT-DOWN
            action = MoveTo::create(1.0f, Point(-visibleSize.width/2, -visibleSize.height/2));
            while (xx >= 0 && xx < MAP_WIDTH && yy >= 0 && yy < MAP_HEIGHT)
            {
                if (Common::IsObstacle(xx, yy))
                {
                    action = MoveBy::create(1.0f, Point((xx-x)*TILE_WIDTH, (yy-y)*TILE_HEIGHT));
                    break;
                }
                if (xx == player->x && yy == player->y)
                {
                    action = MoveBy::create(1.0f, Point((player->x-x)*TILE_WIDTH, (player->y-y)*TILE_HEIGHT));
                    isLiving = false;
                    break;
                }
                xx--; yy--;
            }
            break;
            
        case 315: // LEFT-UP
            action = MoveTo::create(1.0f, Point(-visibleSize.width/2, visibleSize.height/2));
            while (xx >= 0 && xx < MAP_WIDTH && yy >= 0 && yy < MAP_HEIGHT)
            {
                if (Common::IsObstacle(xx, yy))
                {
                    action = MoveBy::create(1.0f, Point((xx-x)*TILE_WIDTH, (yy-y)*TILE_HEIGHT));
                    break;
                }
                if (xx == player->x && yy == player->y)
                {
                    action = MoveBy::create(1.0f, Point((player->x-x)*TILE_WIDTH, (player->y-y)*TILE_HEIGHT));
                    isLiving = false;
                    break;
                }
                xx--; yy++;
            }
            break;
    }
    
    auto missile = Sprite::create("missile.png");
    int rotation = cannon[cidx]->direction[cannon[cidx]->idx];
    missile->setPosition(Common::SetXY(x, y));
    missile->setRotation(rotation);
    missile->setTag(flag);
    pThis->tileLayer->addChild(missile, 5);
    
    CCActionInterval* shoot;
    shoot = CCSequence::create(action, CCCallFuncND::create(pThis, callfuncND_selector(HelloWorld::AfterShoot), pThis), NULL);
    missile->runAction(shoot);
}

void HelloWorld::RenewCannonState()
{
    char temp[10];
    
    // cannon들의 상태 갱신
    for (int i = 0 ; i < (int)cannon.size() ; i++)
    {
        cannon[i]->now--;
        if (cannon[i]->now <= 0)
        {
            cannon[i]->now = cannon[i]->period;
            cannon[i]->idx = (cannon[i]->idx+1) % (int)cannon[i]->direction.size();
            
            //cannon[i]->sp->runAction(RotateTo::create(0.5f, cannon[i]->direction[cannon[i]->idx]));
            cannon[i]->arrow->runAction(RotateTo::create(0.5f, cannon[i]->direction[cannon[i]->idx]));
        }
        
        sprintf(temp, "%d", cannon[i]->now);
        cannon[i]->label->setString(temp);
    }
}

void HelloWorld::AfterShoot(Node* sender, void* ptr)
{
    char temp[20];
    HelloWorld* pThis = (HelloWorld*)ptr;
    int tag = sender->getTag();
    
    sender->removeFromParentAndCleanup(true);
    if (!tag)
        return;
    
    RenewCannonState();
    
    /*
    for (int x = 0 ; x < SIZE ; x++)
    {
        for (int y = 0 ; y < SIZE ; y++)
        {
            if (map[x][y]->timeIdx < (int)map[x][y]->time.size())
            {
                if (map[x][y]->time[map[x][y]->timeIdx] == pThis->curTime)
                {
                    map[x][y]->timeIdx++;
                    int idx = map[x][y]->timeIdx;
                    
                    if ((int)map[x][y]->timeIdx >= (int)map[x][y]->time.size()) // 끝났다.
                    {
                        map[x][y]->missile->stopAllActions();
                        map[x][y]->missile->setVisible(false);
                        map[x][y]->arrow->setVisible(false);
                        map[x][y]->timeLabel->setVisible(false);
                    }
                    else // 아직 남아있다.
                    {
                        map[x][y]->missile->stopAllActions();
                        map[x][y]->missile->setPosition(Common::SetXY(x, y));
                        
                        float rotation = 0;
                        switch (map[x][y]->direction[idx])
                        {
                            case UP:    rotation = 0; break;
                            case RIGHT: rotation = 90; break;
                            case DOWN:  rotation = 180; break;
                            case LEFT:  rotation = 270; break;
                        }
                        map[x][y]->arrow->runAction(RotateTo::create(0.5f, rotation));
                        map[x][y]->missile->runAction(RotateTo::create(0.5f, rotation));
                        map[x][y]->obj->runAction(RotateTo::create(0.5f, rotation));
                        
                        char temp[5];
                        sprintf(temp, "%d", map[x][y]->time[map[x][y]->timeIdx]);
                        map[x][y]->timeLabel->setString(temp);
                    }
                }
            }
        }
    }
    */
    
    pThis->curTime++;
    sprintf(temp, "%d", pThis->curTime);
    pThis->curTimeLabel->setString(temp);
    
    pThis->touched = false;
    
    
    // 맞았는지 검사
    if (!isLiving)
    {
        PlayerDead();
    }
    else if (player->x == (int)end_p.x && player->y == (int)end_p.y)
    {
        StageSuccess();
    }
}

void HelloWorld::PlayerDead()
{
    //MessageBox("죽었다!", "");
    
    Scene* scene = Result::createScene(-1, -1);
    this->addChild(scene, 200);
    //TransitionMoveInT* trans = TransitionMoveInT::create(0.5f, scene);
    //Director::getInstance()->pushScene(scene);
}
void HelloWorld::StageSuccess()
{
    //MessageBox("탈출 성공!", "");
    if (stageNumber == curStage)
    {
        curStage++;
        UserDefault::getInstance()->setIntegerForKey("stage", curStage);
        UserDefault::getInstance()->flush();
    }
    
    int stars = 1;
    if (curTime <= rating_three)
        stars = 3;
    else if (curTime <= rating_two)
        stars = 2;

    Scene* scene = Result::createScene(curTime, stars);
    this->addChild(scene, 200);
    //TransitionMoveInT* trans = TransitionMoveInT::create(0.5f, scene);
    //Director::getInstance()->pushScene(trans);
}

void HelloWorld::PopScene()
{
    FreeAllData();
    
    // remove this notification
    CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, "Game");
    
    // post notification
    CCString* param = CCString::create("0");
    CCNotificationCenter::sharedNotificationCenter()->postNotification("Stage", param);
    Director::getInstance()->popScene();
}


void HelloWorld::FreeAllData()
{
    Common::DeleteData();
    
    if (obstacle_sp != NULL)
        obstacle_sp->removeFromParentAndCleanup(true);
    if (obstacle_label != NULL)
        obstacle_label->removeFromParentAndCleanup(true);
    
    curTimeLabel->removeFromParentAndCleanup(true);
    
    tileLayer->removeAllChildren();
    tileLayer->removeFromParentAndCleanup(true);
}