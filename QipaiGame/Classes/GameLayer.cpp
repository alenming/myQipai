#include "GameLayer.h"
#include "GameNetworkNode.h"
#include "GameService.h"

using namespace cocos2d;
using namespace ui;


GameLayer::GameLayer()
{

}

GameLayer::~GameLayer()
{

}

GameLayer* GameLayer::create()
{
	GameLayer *pRet = new GameLayer();
	if (pRet && pRet->init())
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = NULL;
		return NULL;
	}
}

bool GameLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }

	this->initWithColor(Color4B(150, 200, 255, 255));
	initUI();
	
    return true;
}

void GameLayer::initUI()
{

	auto viSize = Director::getInstance()->getVisibleSize();
	Button* m_Button = Button::create("CloseNormal.png","CloseNormal.png","CloseNormal.png",ui::Widget::TextureResType::LOCAL);
	m_Button->setPosition(Size(viSize.width-40, viSize.height-40));
	std::string buttonName = "OUT";
	auto text = Text::create();
	text->setString(buttonName);
	text->setColor(Color3B::BLACK);
	text->setPosition(Vec2(20, 20));
	m_Button->addChild(text);
	m_Button->setScale(1.5);
	m_Button->addTouchEventListener([=](Ref* spender, Widget::TouchEventType type)
	{
		if (type == ui:: Widget::TouchEventType::ENDED)
		{
			//this->removeFromParent();
		}
	});
	this->addChild(m_Button,98);



	Button* m_Button1 = Button::create("CloseNormal.png", "CloseNormal.png", "CloseNormal.png", ui::Widget::TextureResType::LOCAL);
	m_Button1->setPosition(Size(viSize.width - 40, viSize.height - 100));
	buttonName.clear();
	buttonName = "login";
	auto text1 = Text::create();
	text1->setString(buttonName);
	text1->setColor(Color3B::BLACK);
	text1->setPosition(Vec2(20, 20));
	m_Button1->addChild(text1,99,"Text");
	m_Button1->setScale(1.5);
	m_Button1->addTouchEventListener([=](Ref* spender, Widget::TouchEventType type)
	{
		if (type == ui::Widget::TouchEventType::ENDED)
		{
			GameService::CMD_C2S_LOGIN();
		}
	});
	this->addChild(m_Button1, 98);

	Button* m_Button2 = Button::create("CloseNormal.png", "CloseNormal.png", "CloseNormal.png", ui::Widget::TextureResType::LOCAL);
	m_Button2->setPosition(Size(viSize.width - 40, viSize.height - 160));
	buttonName.clear();
	buttonName = "close";
	auto text2 = Text::create();
	text2->setScale(1.5);
	text2->setString(buttonName);
	text2->setColor(Color3B::BLACK);
	text2->setPosition(Vec2(20, 20));
	m_Button2->addChild(text2, 99, "Text");
	m_Button2->setScale(1.5);
	m_Button2->addTouchEventListener([=](Ref* spender, Widget::TouchEventType type)
	{
		if (type == ui::Widget::TouchEventType::ENDED)
		{
			GameNetworkNode::getInstance()->closeConnect();
		}
	});
	this->addChild(m_Button2, 98);

	Button* m_Button3 = Button::create("CloseNormal.png", "CloseNormal.png", "CloseNormal.png", ui::Widget::TextureResType::LOCAL);
	m_Button3->setPosition(Size(viSize.width - 40, viSize.height - 220));
	buttonName.clear();
	buttonName = "re";
	auto text3 = Text::create();
	text3->setScale(1.5);
	text3->setString(buttonName);
	text3->setColor(Color3B::BLACK);
	text3->setPosition(Vec2(20, 20));
	m_Button3->addChild(text3, 99, "Text");
	m_Button3->setScale(1.5);
	m_Button3->addTouchEventListener([=](Ref* spender, Widget::TouchEventType type)
	{
		if (type == ui::Widget::TouchEventType::ENDED)
		{
			GameNetworkNode::getInstance()->reconnectToServer(SERVER_CONN_SESSION);
		}
	});
	this->addChild(m_Button3, 98);

    //手动产生crash, 测试c++ bug上传及代码定位
    Button* m_Button4 = Button::create("CloseNormal.png", "CloseNormal.png", "CloseNormal.png", ui::Widget::TextureResType::LOCAL);
    m_Button4->setPosition(Size(viSize.width - 40, 40));
    buttonName = "Bugly";
    auto text4 = Text::create();
    text4->setScale(1.5);
    text4->setString(buttonName);
    text4->setColor(Color3B::BLACK);
    text4->setPosition(Vec2(20, 20));
    m_Button4->addChild(text4, 99, "Text");
    m_Button4->setScale(1.5);
    m_Button4->addClickEventListener([=](Ref* spender)
    {
		
    });

    this->addChild(m_Button4, 98);


	//cocos2d::Director::getInstance()->getScheduler()->schedule([](float dt)
	//{
	//	GameNetworkNode::getInstance()->update(dt);
	//}, this, 0.05f, false, "FMOD_AUDIO_ENGINE");
}


