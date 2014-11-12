#include "GalaxyWarScene.h"
#include "GameOverScene.h"

USING_NS_CC;

static const unsigned int bulletCategory	= 0x1 << 0;
static const unsigned int shipCategory		= 0x1 << 1;
static const unsigned int alienCategory		= 0x1 << 2;
static const unsigned int spaceCategory		= 0x1 << 3;

GalaxyWar::GalaxyWar():_alienCount(0),
	_currentDirection(MovementDirection::kDirectionRight),
	_tempDirection(MovementDirection::kDirectionNone){}
GalaxyWar::~GalaxyWar()
{
}

Scene* GalaxyWar::createScene()
{
	Scene *scene = Scene::createWithPhysics();
	scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_NONE);
	GalaxyWar *layer = GalaxyWar::create();
	layer->setPhysicWorld(scene->getPhysicsWorld());
	scene->addChild(layer);
	return scene;
}

bool GalaxyWar::init()
{
	if(!Layer::init()) return false;

	_isEndGame = false;
	_isWin = false;
	_score = 0;

	//Create Background
	this->setBackground();

	//Create Ship
	this->setShip();

	//Setup Labels
	this->setHud();

	//Setup Aliens
	this->setAlien();
	this->schedule(schedule_selector(GalaxyWar::alienMovementUpdate), 0.25);
	this->scheduleUpdate();

	auto *keyListener = EventListenerKeyboard::create();
	keyListener->onKeyPressed = [this](EventKeyboard::KeyCode key, Event *e)
	{
		switch (key)
		{
		case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
			this->moveShipByDirection(MovementDirection::kDirectionLeft);
			break;
		case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
			this->moveShipByDirection(MovementDirection::kDirectionRight);
			break;
		case cocos2d::EventKeyboard::KeyCode::KEY_SPACE:
			this->fireFromShip();
			break;
		default:
			break;
		}
	};
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyListener, this);

	auto contactListener = EventListenerPhysicsContact::create();
	contactListener->onContactBegin = CC_CALLBACK_1(GalaxyWar::onContactBegin, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);

	return true;
}

bool GalaxyWar::onContactBegin(PhysicsContact &contact)
{
 	Node* contactA = contact.getShapeA()->getBody()->getNode();
	Node* contactB = contact.getShapeB()->getBody()->getNode();

	if((contactA->getName() == kShipBulletName && contactB->getName() == kAlienName) || 
		(contactA->getName() == kAlienName && contactB->getName() == kShipBulletName))
	{
  		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("InvaderHit.wav");
		contactA->removeFromParent();
		contactB->removeFromParent();
		_alienCount--;
		_score += 270;

		char strScore[25] = {0};
		sprintf(strScore, "Score: %04d", _score);

		LabelTTF *scoreLabel = (LabelTTF*)this->getChildByName(kHudScore);
		scoreLabel->setString(strScore);

		if(_alienCount <= 0) {
			_isWin = true;
			_isEndGame = true;
		}
	}
	else if((contactA->getName() == kAlienBulletName && contactB->getName() == kShipName) || 
		(contactA->getName() == kShipName && contactB->getName() == kAlienBulletName))
	{
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("ShipHit.wav");
		
		_isWin = false;
		_isEndGame = true;
	}

	return true;
}

void GalaxyWar::update(float dt)
{
	if(_isEndGame)
	{
		char *strTitle = _isWin ? "You Win!":"You Lose!";
		auto scene = GameOver::createScene(strTitle, _score);
		Director::getInstance()->replaceScene(TransitionFade::create(0.5, scene));
	}
	this->checkAlien();
	this->fireFromAlien();
}

void GalaxyWar::checkAlien()
{
	this->enumerateChildren(kAlienName, [this](Node* node){
		if(node->getPosition().y <= kShipSize.height){
			_isWin = false;
			_isEndGame = true;
			return true;
		}

		return false;
	});
}

void GalaxyWar::alienMovementUpdate(float delta)
{		
	this->determineAlienMovement();
	this->moveAliensByDirection();
}

void GalaxyWar::setBackground()
{
	auto edgeBound = PhysicsBody::createEdgeBox(WIN_SIZE, PHYSICSBODY_MATERIAL_DEFAULT, 2.0f);
	Sprite *background = Sprite::create();
	background->setPhysicsBody(edgeBound);
	background->setPosition(WIN_SIZE.width / 2, WIN_SIZE.height / 2);
	this->addChild(background);
}

void GalaxyWar::setShip()
{
	auto shipBody = PhysicsBody::createBox(kShipSize);
	shipBody->setDynamic(TRUE);
	shipBody->setMass(0.02);
	shipBody->setCategoryBitmask(shipCategory);
	shipBody->setContactTestBitmask(bulletCategory);
	shipBody->setGravityEnable(FALSE);

	Sprite* ship = Sprite::create("Ship.png");
	ship->setName(kShipName);
	ship->setColor(Color3B::YELLOW);
	ship->setPosition(WIN_SIZE.width / 2, (kShipSize.height / 2) + 2);
	ship->setPhysicsBody(shipBody);
	ship->setZOrder(9);
	this->addChild(ship);
}

void GalaxyWar::fireFromShip()
{
	Node* existingBullet = this->getChildByName(kShipBulletName);

	if(!existingBullet){
		Sprite *ship = (Sprite*)this->getChildByName(kShipName);

		Point from(ship->getPosition().x, 0);
		Point to(ship->getPosition().x, WIN_SIZE.height);

		this->makeBulletByType(BulletType::kBulletShip, from, to, "ShipBullet.wav");
	}
}

void GalaxyWar::fireFromAlien()
{
	Node *existingBullet = this->getChildByName(kAlienBulletName);

	if(!existingBullet){
		_aliens.clear();

		this->enumerateChildren(kAlienName, [this](Node* node){
			_aliens.pushBack(node);
			return false;
		});

		if(_aliens.size() > 0){
			int _alienIndex = CCRANDOM_0_1() * _aliens.size();
			
			Node* alien = _aliens.at(_alienIndex);
			Point from(alien->getPosition().x, (alien->getPosition().y - kAlienSize.height / 2));
			Point to(alien->getPosition().x, 0);

			this->makeBulletByType(BulletType::kBulletAlien, from, to, "InvaderBullet.wav");
		}
	}
}

void GalaxyWar::makeBulletByType(BulletType type, const Point &from, const Point &to,const char* filename)
{
	Sprite* bullet;

	PhysicsBody *bulletBody = PhysicsBody::createBox(kBulletSize);
	bulletBody->setDynamic(TRUE);
	bulletBody->setGravityEnable(FALSE);
	bulletBody->setCategoryBitmask(bulletCategory);

	switch(type)
	{
	case BulletType::kBulletAlien:
		bulletBody->setContactTestBitmask(shipCategory);
		bullet = Sprite::create("AlienBullet.png");
		bullet->setName(kAlienBulletName);
		break;
	case BulletType::kBulletShip:
		bulletBody->setContactTestBitmask(alienCategory);
		bullet = Sprite::create("ShipBullet.png");
		bullet->setName(kShipBulletName);
		break;
	default:
		bullet = nullptr;
		break;
	}

	if(bullet)
	{
		bullet->setContentSize(kBulletSize);
		bullet->setPhysicsBody(bulletBody);
		bullet->setZOrder(0);
		bullet->setPosition(from);
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(filename);
		Action *fireAction = Sequence::create(
			MoveTo::create(2.0f, to), 
			CallFuncN::create( CC_CALLBACK_1(GalaxyWar::removeNode, this)),
			NULL
			);
		bullet->runAction(fireAction);
		this->addChild(bullet);
	}
}

void GalaxyWar::removeNode(Node* node)
{
	node->removeFromParent();
}

void GalaxyWar::setAlien()
{
	_alienCount = 0;
	AlienType type;
	float x = (kAlienSize.width / 2);
	float y = WIN_SIZE.height - ((kAlienSize.height + kAlienGridSpacing) * kAlienRow) - 20;

	for(int r = 0; r < kAlienRow; r++)
	{
		if(r % 3 == 0) type = AlienType::AlienTypeA;
		else if(r % 3 == 1) type = AlienType::AlienTypeB;
		else type = AlienType::AlienTypeC;

		for(int c = 0; c < kAlienCol; c++)
		{
			auto alienBody = PhysicsBody::createBox(kAlienSize);
			alienBody->setDynamic(TRUE);
			alienBody->setGravityEnable(FALSE);

			alienBody->setCategoryBitmask(alienCategory);
			alienBody->setContactTestBitmask(bulletCategory);

			Sprite* alien = this->makeAlienByType(type);
			alien->setPosition(x, y);
			alien->setPhysicsBody(alienBody);

			this->addChild(alien);

			_alienCount++;

			x += kAlienGridSpacing + kAlienSize.width;
		}

		x = kAlienSize.width / 2;
		y += kAlienGridSpacing + kAlienSize.height;
	}
}

Sprite* GalaxyWar::makeAlienByType(const AlienType type)
{
	char* alienName;
	Color3B color(Color3B::WHITE);

	switch (type){
		case AlienType::AlienTypeA:
			alienName = "InvaderA";
			color = Color3B(0, 200,255);
			break;
		case AlienType::AlienTypeB:
			alienName = "InvaderB";
			color = Color3B(255, 100, 0);
			break;
		case AlienType::AlienTypeC:
			alienName = "InvaderC";
			color = Color3B(0, 255, 200);
			break;
	}
	
	auto animation = Animation::create();

	for(int i = 0; i < 2; i++)
	{
		char str[50] = {0};
		sprintf(str, "%s_%02d.png", alienName, i);
		animation->addSpriteFrameWithFile(str);
	}

	animation->setDelayPerUnit(0.25);
	auto moveAction = RepeatForever::create(Animate::create(animation));

	Sprite *alien = Sprite::create(StringUtils::format("%s_00.png", alienName));
	alien->setColor(color);
	alien->setName(kAlienName);
	alien->runAction(moveAction);

	return alien;
}

void GalaxyWar::moveShipByDirection(MovementDirection direction)
{
	Sprite *ship = (Sprite*)this->getChildByName(kShipName);

	switch(direction){
	case MovementDirection::kDirectionLeft:
		if(!(ship->getPosition().x - (ship->getContentSize().width / 2) < 3.0f))
			ship->setPosition(ship->getPosition().x - 20, ship->getPosition().y);
		break;
	case MovementDirection::kDirectionRight:
		if(!(ship->getPosition().x + (ship->getContentSize().width / 2) > WIN_SIZE.width - 3.0f))
			ship->setPosition(ship->getPosition().x + 20, ship->getPosition().y);
		break;
	default:
		CCLog("Wrong Movement");
		break;
	}
}

void GalaxyWar::determineAlienMovement()
{
	_tempDirection = _currentDirection;
	this->enumerateChildren(kAlienName, [this](Node* node){
		switch (_tempDirection)
		{
		case MovementDirection::kDirectionRight:
			if((node->getPosition().x + node->getContentSize().width / 2) > WIN_SIZE.width - 3.0f)
			{
				_tempDirection = MovementDirection::kDirectionDownThenLeft;
				return TRUE;
			}
			break;
		case MovementDirection::kDirectionLeft:
			if((node->getPosition().x - node->getContentSize().width / 2) < 3.0f){
				_tempDirection = MovementDirection::kDirectionDownThenRight;
				return TRUE;
			}
			break;
		case MovementDirection::kDirectionDownThenLeft:
			_tempDirection = MovementDirection::kDirectionLeft;
			return TRUE;
		case MovementDirection::kDirectionDownThenRight:
			_tempDirection = MovementDirection::kDirectionRight;
			return TRUE;
		default:
			CCLog("Wrong move");
			break;
		}

		return FALSE;
	});

	if(_tempDirection != _currentDirection){
		_currentDirection = _tempDirection;
	}
}

void GalaxyWar::moveAliensByDirection()
{
	this->enumerateChildren(kAlienName, [this](Node* node){

		switch (_currentDirection)
		{
		case MovementDirection::kDirectionRight:
			node->setPosition(node->getPosition().x + 20, node->getPosition().y);
			break;
		case MovementDirection::kDirectionLeft:
			node->setPosition(node->getPosition().x - 20, node->getPosition().y);
			break;
		case MovementDirection::kDirectionDownThenLeft:
		case MovementDirection::kDirectionDownThenRight:
			node->setPosition(node->getPosition().x, node->getPosition().y - 10);
		default:
			CCLog("Node don't move");
			break;
		}

		return false;
	});
}

void GalaxyWar::setHud()
{
	LabelTTF *labelScore = LabelTTF::create("Score: 00000", "fonts/Marker Felt.ttf", 25);
	labelScore->setName(kHudScore);
	labelScore->setColor(Color3B(0,255,0));
	labelScore->setPosition(labelScore->getContentSize().width / 2 + 3, 
		WIN_SIZE.height - (labelScore->getContentSize().height / 2));
	this->addChild(labelScore);
	/*
	LabelTTF *labelHealth = LabelTTF::create("Health: 100", "fonts/Marker Felt.ttf", 19);
	labelHealth->setName(kHudHealth);
	labelHealth->setColor(Color3B::RED);
	labelHealth->setPosition(WIN_SIZE.width - (labelScore->getContentSize().width / 2) - 5,
		WIN_SIZE.height - (labelScore->getContentSize().height / 2));
	this->addChild(labelHealth);*/
}