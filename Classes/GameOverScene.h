#ifndef __GAME_OVER_SCENE_H__
#define __GAME_OVER_SCENE_H__

#include "cocos2d.h"
USING_NS_CC;

class GameOver : public Layer
{
public:
	void initWithData();
	static Scene* createScene(const char* title, int score);
	CREATE_FUNC(GameOver);
	void setTitile(const char* title){_title = title;};
	void setScore(int score){ _score = score;};
private:
	void restartGame();
	const char* _title;
	int _score;
};


#endif