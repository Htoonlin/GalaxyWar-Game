#include "GameOverScene.h"
#include "GalaxyWarScene.h"
#include "GalaxyWarConstants.h"

Scene* GameOver::createScene(const char* title, int score)
{
	Scene *scene = Scene::create();
	GameOver *layer = GameOver::create();
	layer->setTitile(title);
	layer->setScore(score);
	layer->initWithData();
	scene->addChild(layer);
	return scene;
}

void GameOver::initWithData()
{
	LabelTTF *_titleLabel = LabelTTF::create(_title, "fonts/Marker Felt.ttf", 80);
	_titleLabel->setColor(Color3B::RED);
	_titleLabel->setPosition(WIN_SIZE.width / 2, (WIN_SIZE.height / 2) + 120);
	this->addChild(_titleLabel);

	char strScore[25] = {0};
	sprintf(strScore, "Your Score : %04d", _score);
	LabelTTF *_scoreLabel = LabelTTF::create(strScore, "fonts/Marker Felt.ttf", 50);
	_scoreLabel->setColor(Color3B::YELLOW);
	_scoreLabel->setPosition(WIN_SIZE.width / 2, WIN_SIZE.height / 2);
	this->addChild(_scoreLabel);

	MenuItemFont *restartMenu = MenuItemFont::create("(Play Again)", CC_CALLBACK_0(GameOver::restartGame, this));
	restartMenu->setPosition(WIN_SIZE.width / 2, (WIN_SIZE.height / 2) - 100);
	Menu *menu = Menu::create(restartMenu, NULL);
	menu->setPosition(Point::ZERO);
	this->addChild(menu);
}

void GameOver::restartGame()
{
	Scene* scene = GalaxyWar::createScene();
	Director::getInstance()->replaceScene(TransitionFade::create(0.5, scene));
}