/***********************************************************     
* �ļ�����: UILogLayer         
* �� �� �ߣ�Mr.Alen     
* �������ڣ�2016/09/18         
/***********************************************************/
#ifndef __UILOG_LAYER_H__
#define __UILOG_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"



class GameLayer : public cocos2d::LayerColor
{
public:
	GameLayer();
	~GameLayer();
	static GameLayer* create();

    virtual bool init();

	void initUI();

};


#endif // __UILOG_LAYER_H__
