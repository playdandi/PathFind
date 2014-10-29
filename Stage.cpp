#include "Stage.h"

Scene* Stage::createScene()
{
    auto scene = Scene::create();
    auto layer = Stage::create();
    scene->addChild(layer);
    return scene;
}

bool Stage::init()
{
    if ( !LayerColor::initWithColor(Color4B(255,255,255,255)) )
    {
        return false;
    }
    
    visibleSize = Director::getInstance()->getVisibleSize();
    origin = Director::getInstance()->getVisibleOrigin();
    
    // notification observer
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(Stage::Notification), "Stage", NULL);
    
    // init stages
    InitStages();
    
    touched = false;
    InitTouch();
    
    return true;
}

void Stage::Notification(CCObject* obj)
{
    CCString* param = (CCString*)obj;
    
    if (param->intValue() == 0)
    {
        for (int i = 0 ; i < stages.size() ; i++)
        {
            if (stages[i]->getTag() <= curStage)
                stages[i]->setOpacity(255);
            else
                stages[i]->setOpacity(127);
        }
        /*
        // 터치 활성
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority()+1, true);
        this->setTouchPriority(Depth::GetCurPriority());
        isTouched = false;
        isKeybackTouched = false;
        */
    }
}

void Stage::InitStages()
{
    curStage = UserDefault::getInstance()->getIntegerForKey("stage", 1);
    
    stageLayer = Layer::create();
    stageLayer->setPosition(Point(origin.x+visibleSize.width/2, origin.y+visibleSize.height/2));
    this->addChild(stageLayer);
    
    //UserDefault::getInstance()->setIntegerForKey("stage", 1);
    stages.clear();
    
    char temp[7];
    int number = 1;
    for (int y = 2 ; y >= 0 ; y--)
    {
        for (int x = 0 ; x < 7 ; x++)
        {
            auto sprite = Sprite::create("tile.png");
            Point p = Point((x-3)*(100+20), (y-1)*(100+20));
            sprite->setPosition(p);
            sprite->setTag(number);
            stageLayer->addChild(sprite);
            stages.push_back(sprite);
            
            if (number > curStage)
                sprite->setOpacity(127);
            
            auto label = LabelTTF::create("", "Marker Felt", 36);
            sprintf(temp, "%d", number++);
            label->setColor(Color3B(255,255,255));
            label->setString(temp);
            label->setPosition(p);
            stageLayer->addChild(label);
        }
    }
}


void Stage::InitTouch()
{
    // 디스패처 : 리스너와 오브젝트를 연결해주는 역할
    EventDispatcher* dispatcher = Director::getInstance()->getEventDispatcher();
    
    // 터치 위치를 알려주는 리스너. 단일 터치.
    auto positionListener = EventListenerTouchOneByOne::create();
    
    // zOrder에 따라 밑에 깔린 애도 동작할지 아닐지를 결정한다.
    positionListener->setSwallowTouches(true);
    
    // 콜백 함수 대입
    positionListener->onTouchBegan = CC_CALLBACK_2(Stage::touchBegan, this);
    
    // 디스패처를 이용해 객체와 리스너를 이어준다. 화면 전체를 터치할 수 있게 만들어야 하므로 객체는 this
    dispatcher->addEventListenerWithSceneGraphPriority(positionListener, this);
}

bool Stage::touchBegan(Touch* pTouch, Event* pEvent)
{
    if (touched)
        return true;
    touched = true;
    
    auto target = pEvent->getCurrentTarget();
    Point pos = target->convertToNodeSpace(pTouch->getLocation());
    Rect rect = Rect(0, 0, target->getContentSize().width, target->getContentSize().height);
    
    //
    
    for (int i = 0 ; i < (int)stages.size() ; i++)
    {
        Point p = stages[i]->convertToNodeSpace(pos);
        if (p.x >= 0 && p.x <= 100 && p.y >= 0 && p.y <= 100)
        {
            int stageNumber = stages[i]->getTag();
            if (stageNumber <= curStage)
            {
                Scene* scene = HelloWorld::createScene(stageNumber);
                TransitionMoveInB* trans = TransitionMoveInB::create(1.0f, scene);
                Director::getInstance()->pushScene(trans);
            }
            else
            {
                MessageBox("잠겨 있습니다.", "딩동!");
            }
            break;
        }
    }
    
    touched = false;
    
    return true;
}

