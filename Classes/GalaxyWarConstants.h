#ifndef __GALAXY_WAR_CONSTANTS_H__
#define __GALAXY_WAR_CONSTANTS_H__

typedef enum AlienType
{
	AlienTypeA,
	AlienTypeB,
	AlienTypeC
};

typedef enum MovementDirection{
	kDirectionLeft,
	kDirectionRight,
	kDirectionDownThenLeft,
	kDirectionDownThenRight,
	kDirectionNone
};

typedef enum BulletType{
	kBulletAlien,
	kBulletShip,
	kBulletNone
};

#define WIN_SIZE Director::getInstance()->getVisibleSize()

#define kAlienSize Size(50, 30)
#define kAlienName "alien"
#define kAlienGridSpacing 8
#define kAlienRow 6
#define kAlienCol 6

#define kShipSize Size(60, 20)
#define kShipName "ship"

#define kShipBulletName "shipBullet"
#define kAlienBulletName "alienBullet"
#define kBulletSize Size(4,8)

#define kHudHealth "health"
#define kHudScore "score"

#endif