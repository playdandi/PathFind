#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "Data.h"
#include "Result.h"
#include <iostream>
#include <sstream>
#include <iterator>

using namespace cocos2d;

class HelloWorld : public LayerColor
{
public:
    static Scene* createScene(int stage);
    virtual bool init();
    
    void Notification(CCObject* obj);
    
    void InitStage();
    void InitMap();
    
    void InitTouch();
    bool touchBegan(Touch* pTouch, Event* pEvent);

    void AfterMove(Node* sender, void* ptr);
    void AfterPreAction(Node* sender, void* ptr);
    
    void BombCheck();
    void Detonate(int x, int y);
    
    void ShootFunction(void* ptr);
    void Shoot(int cidx, void* ptr, bool flag);
    void AfterShoot(Node* sender, void* ptr);
    
    void RenewCannonState();
    
    void PlayerDead();
    void StageSuccess();

    void PopScene();
    void FreeAllData();

    CREATE_FUNC(HelloWorld);
    
private:
    bool touched;
    bool obstacle_touched;
    Layer* tileLayer;
    
    int obstacle_install;
    Sprite* obstacle_sp;
    LabelTTF* obstacle_label;
    
    int curTime;
    LabelTTF* curTimeLabel;
    
    bool isLiving;
    
    Size visibleSize;
    Vec2 origin;
    
    Point start_p;
    Point end_p;
    
    int rating_three;
    int rating_two;
};

#endif // __HELLOWORLD_SCENE_H__
