#ifndef _H_SCENE_H_
#define _H_SCENE_H_
#pragma once

#include <list>
#include <deque>
#include <vector>
#include <unordered_map>


#include "Component.h"
#include "Notifications.h"
#include "Entity.h"
#include "Components.h"
#include "MeshRenderer_c.h"

class SceneManager;
class Game;
class GameManager_c;

class Scene
{
public:

	enum Tipo {
		MENU,
		LEVEL,
		size
	};
	
	Scene(Tipo tipo);
	virtual ~Scene();

	void tick();

	void addListiner(MsgId id, Component* component);
	void removeListener(MsgId id, Component* component);


	void reciveMsg(Msg_Base* newMsg);
	//Entity management
	Entity* whoIs(EntityId id);
	Entity* whoIs(std::string name);

	void setGameManager();
	void restaEnemigo();
	
	void endGame() { endLevel_ = true; }
	


	inline GameManager_c* getGameManager() { return (GameManager_c*)_gameManager->getComponent(ComponentType::GAMEMANAGER); }


	std::string const getNameOf(EntityId id);
	EntityId const getIdOf(std::string name);

private:
	std::list <Entity*> entities;
	std::list <Entity*> graveland;
	std::deque<Msg_Base*> messages;
	std::vector<Msg_Base*> messagesBuffer;
	std::unordered_map<MsgId, std::vector<Component*>> listeners;

	Entity * _gameManager;
	Tipo t_;
	int counter_;
	bool start_;
	int numEnemigos_;
	std::string enRes_;
	std::string level;
	bool endLevel_;
	

	bool isSendingMessages;
	void _msgDeliver();
	void _dumpMessages();
	void destroyEntity(Entity* entity);

};


#endif //!_H_SCENE_H_