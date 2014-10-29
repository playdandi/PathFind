#ifndef __PathFind__Stage__
#define __PathFind__Stage__

#include "cocos2d.h"
#include "Data.h"
#include "HelloWorldScene.h"

using namespace cocos2d;

class Stage : public LayerColor
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    
    void Notification(CCObject* obj);
    
    void InitStages();
    
    void InitTouch();
    bool touchBegan(Touch* pTouch, Event* pEvent);
    
    CREATE_FUNC(Stage);
    
private:
    Layer* stageLayer;
    
    bool touched;
    
    Size visibleSize;
    Vec2 origin;
    
    std::vector<Sprite*> stages;
};



#endif /* defined(__PathFind__Stage__) */
