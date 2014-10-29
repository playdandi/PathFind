#include "Result.h"

static int turn;
static int star;

Scene* Result::createScene(int turns, int stars)
{
    turn = turns;
    star = stars;
    
    auto scene = Scene::create();
    auto layer = Result::create();
    scene->addChild(layer);
    return scene;
}

void Result::onEnter()
{
    Layer::onEnter();
    
    if (star != -1)
    {
        this->getChildByTag(1)->runAction( Sequence::create(DelayTime::create(0.0f), FadeIn::create(0.5f), NULL) );
        if (star >= 2)
            this->getChildByTag(2)->runAction( Sequence::create(DelayTime::create(0.75f), FadeIn::create(0.5f), NULL) );
        if (star >= 3)
            this->getChildByTag(3)->runAction( Sequence::create(DelayTime::create(1.5f), FadeIn::create(0.5f), NULL) );
    }
}

bool Result::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    visibleSize = Director::getInstance()->getVisibleSize();
    origin = Director::getInstance()->getVisibleOrigin();
    
    auto box = Sprite::create("result_box.png");
    box->setPosition(Point(origin.x+visibleSize.width/2, origin.y+visibleSize.height/2));
    this->addChild(box);
    
    if (star != -1)
    {
        auto sp1 = Sprite::create("result_star.png");
        sp1->setPosition(Point(origin.x+visibleSize.width/2-130, origin.y+visibleSize.height/2+70));
        sp1->setScale(0.33f);
        sp1->setOpacity(50);
        sp1->setTag(1);
        this->addChild(sp1);
        auto sp2 = Sprite::create("result_star.png");
        sp2->setPosition(Point(origin.x+visibleSize.width/2, origin.y+visibleSize.height/2+70));
        sp2->setScale(0.33f);
        sp2->setOpacity(50);
        sp2->setTag(2);
        this->addChild(sp2);
        auto sp3 = Sprite::create("result_star.png");
        sp3->setPosition(Point(origin.x+visibleSize.width/2+130, origin.y+visibleSize.height/2+70));
        sp3->setScale(0.33f);
        sp3->setOpacity(50);
        sp3->setTag(3);
        this->addChild(sp3);
        
        char temp[20];
        sprintf(temp, "%d turns!", turn);
        auto label = LabelTTF::create(temp, "Marker Felt", 56);
        label->setPosition(Point(origin.x+visibleSize.width/2, origin.y+visibleSize.height/2-30));
        label->setColor(Color3B(0,0,0));
        this->addChild(label);
    }
    else
    {
        auto label = LabelTTF::create("Mission Failed...", "Marker Felt", 64);
        label->setPosition(Point(origin.x+visibleSize.width/2, origin.y+visibleSize.height/2+30));
        label->setColor(Color3B(0,0,0));
        this->addChild(label);
    }
    
    auto btn = Sprite::create("btn_close.png");
    btn->setPosition(Point(origin.x+visibleSize.width/2, origin.y+visibleSize.height/2-100));
    btn->setTag(100);
    this->addChild(btn);
    
    InitTouch();
    
    return true;
}


void Result::InitTouch()
{
    // 디스패처 : 리스너와 오브젝트를 연결해주는 역할
    EventDispatcher* dispatcher = Director::getInstance()->getEventDispatcher();
    
    // 터치 위치를 알려주는 리스너. 단일 터치.
    auto positionListener = EventListenerTouchOneByOne::create();
    
    // zOrder에 따라 밑에 깔린 애도 동작할지 아닐지를 결정한다.
    positionListener->setSwallowTouches(true);
    
    // 콜백 함수 대입
    positionListener->onTouchBegan = CC_CALLBACK_2(Result::touchBegan, this);
    
    // 디스패처를 이용해 객체와 리스너를 이어준다. 화면 전체를 터치할 수 있게 만들어야 하므로 객체는 this
    dispatcher->addEventListenerWithSceneGraphPriority(positionListener, this->getChildByTag(100));
}

bool Result::touchBegan(Touch* pTouch, Event* pEvent)
{
    auto target = pEvent->getCurrentTarget();
    Point pos = target->convertToNodeSpace(pTouch->getLocation());
    Rect rect = Rect(0, 0, target->getContentSize().width, target->getContentSize().height);
    
    if (rect.containsPoint(pos))
    {
        PopScene();
    }
    
    return true;
}

void Result::PopScene()
{
    this->removeAllChildren();
    this->removeFromParentAndCleanup(true);
    
    // post notification
    CCString* param = CCString::create("0");
    CCNotificationCenter::sharedNotificationCenter()->postNotification("Game", param);
    //Director::getInstance()->popScene();
}

