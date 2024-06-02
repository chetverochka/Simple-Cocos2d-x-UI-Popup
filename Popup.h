#pragma once
#include "cocos2d.h"
#include "DefaultLayer.h"
#include "TextButton.h"
#include "ui/UIScale9Sprite.h"

class Popup : public DefaultLayer { // только реализация! для создание своего окна надо наследоваться от этого класса и создать свою функцию ::create();
protected:
	std::string titleText;
	std::string text;
	cocos2d::Vector<TextButton*> buttons;

	cocos2d::ui::Scale9Sprite* bg;
	cocos2d::Label* textLabel;
	cocos2d::Label* titleLabel;

	// параметры дистанций
	float distanceBetweenLabels;
	float distanceBetweenLabelAndButton;
	float buttonDistance;
	float titleDistance;

	float defaultButtonHeight;
	float distanceBetweenButtons;

	float maxButtonHeight; // не задаётся вручную!!! значение устанавливается автоматически в цикле функции initBG()


	// установка параметров по умолчанию
	// не вызывается в init и при наследовании можно задать отдельно
	void initValues() {
		distanceBetweenLabels = 20.f;
		distanceBetweenLabelAndButton = 30.f;

		// расстояние от нижнего и верхнего края кнопки и титла соответственно
		buttonDistance = 10.f;
		titleDistance = 10.f;
		
		defaultButtonHeight = 70.f;
		distanceBetweenButtons = 5.f;
	}
	void initLabels() {
		textLabel = cocos2d::Label::createWithTTF(text, "fonts/arial.ttf", 27);
		titleLabel = cocos2d::Label::createWithBMFont("fonts/Unnamed.fnt", titleText);
		if (titleLabel) {
			titleLabel->setScale(45 / titleLabel->getBoundingBox().size.height);
			titleLabel->setColor(cocos2d::Color3B::YELLOW);
		}
	}

	void initBG() {
		
		cocos2d::Size textSize;
		cocos2d::Size titleSize;

		textSize = titleSize = cocos2d::Size(0, 0);

		// инициализация параметров [высота, ширина]
		if (textLabel) {
			textSize = textLabel->getBoundingBox().size;
		}
		
		if (titleLabel) {
			titleSize = titleLabel->getBoundingBox().size;
		}

		cocos2d::Size size;
		
		// ширина окна
		if (textSize.width >= titleSize.width) {
			size.width = textSize.width;
		}
		else {
			size.width = titleSize.width;
		}

		maxButtonHeight = 0.f;
		float buttonsSize = (distanceBetweenButtons * (buttons.size() - 1)); // ширина всех кнопок сразу
		for (auto button : buttons) {
			if (button->getContentSize().height > maxButtonHeight) {
				maxButtonHeight = button->getContentSize().height;
			}

			buttonsSize += button->getContentSize().width;
		}

		if (buttonsSize > size.width) {
			size.width = buttonsSize;
		}

		// то что выше это вычисление минимального размера окна чтобы все элементы влезали в него по ширине.
		// теперь его надо увеличить для красоты, поэтому я домножаю его на процент
		float widthFactor = 1.3f;
		size.width *= widthFactor;


		//высота окна
		size.height = textSize.height + titleSize.height + distanceBetweenLabels + distanceBetweenLabelAndButton + buttonDistance + titleDistance + maxButtonHeight;

		bg = cocos2d::ui::Scale9Sprite::create("Popup.png");
		if (!bg)  return; 
		bg->setCapInsets(cocos2d::Rect(30, 30, 70, 70));
		bg->setContentSize(size);
		bg->setPosition(this->getContentSize() / 2);
		bg->setScale(0);
		bg->runAction(cocos2d::EaseBackOut::create(cocos2d::ScaleTo::create(fadingDuration * 1.5f, 1)));
		this->addChild(bg);
	}

	void setupLabels() {
		if (!bg) {
			return;
		}

		if (titleLabel) {
			titleLabel->setPositionX(bg->getContentSize().width / 2);
			
			float posY = bg->getContentSize().height - titleLabel->getBoundingBox().size.height / 2 - titleDistance;
			titleLabel->setPositionY(posY);
			bg->addChild(titleLabel);
		}

		if (textLabel) {
			textLabel->setPositionX(bg->getContentSize().width / 2);
			textLabel->setAlignment(cocos2d::TextHAlignment::CENTER);
			if (titleLabel) {
				float posY = titleLabel->getPosition().y - (titleLabel->getBoundingBox().size.height / 2) - (textLabel->getBoundingBox().size.height / 2) - distanceBetweenLabels;
				textLabel->setPositionY(posY);
				bg->addChild(textLabel);
			}
		}
	}

	void setupButtons() {
		cocos2d::Size nodeSize;
		nodeSize.width = (distanceBetweenButtons * (buttons.size() - 1));
		for (auto button : buttons) { // у некоторых кнопок разные размеры и приходится через цикл прибавлять размер
			nodeSize.width += button->getContentSize().width;
		}
		nodeSize.height = maxButtonHeight;

		cocos2d::Node* container = cocos2d::Node::create();
		container->setContentSize(nodeSize);
		container->setAnchorPoint(cocos2d::Vec2(0.5, 0.5));
		container->setPosition(cocos2d::Vec2(bg->getContentSize().width / 2, nodeSize.height / 2 + buttonDistance));

		bg->addChild(container, 2);

		float posY = maxButtonHeight / 2;
		for (int i = 0; i < buttons.size(); i++) {
			TextButton* btn = buttons.at(i);
			if (!btn) {
				continue;
			}
			
			float posX;

			if (i == 0) {
				posX = btn->getContentSize().width / 2;
			}
			else {
				TextButton* prevBtn = buttons.at(i - 1);
				posX = prevBtn->getPosition().x + prevBtn->getContentSize().width / 2 + btn->getContentSize().width / 2 + distanceBetweenButtons;
			}
			btn->setPosition(cocos2d::Vec2(posX, posY));
			container->addChild(btn);
		}
	}

	bool init() {
		if (!DefaultLayer::init()) {
			return false;
		}

		initLabels();
		initBG();
		setupLabels();
		setupButtons();
		//formatLabel(textLabel);
		return true;
	}
public:
	static void formatLabel(cocos2d::Label* label) {
		// ... доделать
	}
};

class SimplePopup : public Popup  {
	bool init(std::string title, std::string text) {
		Popup::initValues();

		titleText = title;
		this->text = text;

		TextButton* button = TextButton::create("OK", cocos2d::Size(140, defaultButtonHeight));
		button->setCallback([this](TextButton* ref) {
			deleteLayer();
			});
		buttons.pushBack(button);
		

		if (!Popup::init()) {
			return false;
		}
		
		return true;
	}
public:
	static SimplePopup* create(std::string title, std::string text) {
		SimplePopup* popup = new (std::nothrow) SimplePopup();

		if (popup && popup->init(title, text)) {
			popup->autorelease();
			return popup;
		}
		else {
			CC_SAFE_DELETE(popup);
			return nullptr;
		}
	}
};

class ConfirmPopup : public Popup {
	std::function<void()> yesFunc;

	bool init(std::string title, std::string text, std::function<void()> yesFunc) {
		Popup::initValues();

		this->yesFunc = yesFunc;
		titleText = title;
		this->text = text;

		TextButton* button1 = TextButton::create("Cancel", cocos2d::Size(160, defaultButtonHeight));
		button1->setCallback([this](TextButton* ref) {
			deleteLayer();
			});
		buttons.pushBack(button1);

		TextButton* button2 = TextButton::create("OK", cocos2d::Size(120, defaultButtonHeight));
		button2->setCallback(CC_CALLBACK_1(ConfirmPopup::callback, this));
		buttons.pushBack(button2);


		if (!Popup::init()) {
			return false;
		}

		return true;
	}
	void callback(TextButton* ref) {
		if (yesFunc) {
			yesFunc();
		}
		deleteLayer();
	}
public:
	static ConfirmPopup* create(std::string title, std::string text, std::function<void()> yesFunc) {
		ConfirmPopup* popup = new (std::nothrow) ConfirmPopup();

		if (popup && popup->init(title, text, yesFunc)) {
			popup->autorelease();
			return popup;
		}
		else {
			CC_SAFE_DELETE(popup);
			return nullptr;
		}
	}
};