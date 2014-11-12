#ifndef __GALAXY_WAR_SCENE_H__
#define __GALAXY_WAR_SCENE_H__

#include "cocos2d.h"
#include "GalaxyWarConstants.h"

class GalaxyWar : public cocos2d::Layer
{
public:
	GalaxyWar();
	~GalaxyWar();
	virtual bool init();
	static cocos2d::Scene* createScene();
	CREATE_FUNC(GalaxyWar);
	void alienMovementUpdate(float delta);
	virtual void update(float dt);
	bool onContactBegin(cocos2d::PhysicsContact &contact);
private:
	cocos2d::PhysicsWorld *_world;
	int _alienCount;
	MovementDirection _currentDirection;
	MovementDirection _tempDirection;
	cocos2d::Vector<Node*> _aliens;
	bool _isEndGame;
	bool _isWin;
	int _score;
private:
	void setPhysicWorld(cocos2d::PhysicsWorld *world){ _world = world;};
	void setBackground();
	void setShip();
	void setAlien();
	void setHud();
	cocos2d::Sprite* makeAlienByType(const AlienType type);
	void moveAliensByDirection();
	void determineAlienMovement();
	void fireFromShip();
	void fireFromAlien();
	void makeBulletByType(BulletType type, const cocos2d::Point &from, const cocos2d::Point &to,const char* soundFileName);
	void removeNode(Node* node);
	void moveShipByDirection(MovementDirection direction);
	void checkAlien();
};

#endif