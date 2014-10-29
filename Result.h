#ifndef __PathFind__Result__
#define __PathFind__Result__

#include "cocos2d.h"
#include "Data.h"

using namespace cocos2d;

class Result : public Layer
{
public:
    static Scene* createScene(int turns, int stars);
    virtual bool init();
    
    virtual void onEnter();
    
    void InitTouch();
    bool touchBegan(Touch* pTouch, Event* pEvent);
    
    void PopScene();
    
    CREATE_FUNC(Result);
    
private:
    Size visibleSize;
    Vec2 origin;
};



#endif /* defined(__PathFind__Result__) */
