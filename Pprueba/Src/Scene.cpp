#include "Scene.h"
#include "SceneManager.h"
#include "ObjectsFactory.h"
#include "Game.h"
#include "DataManager.h"
#include "GameManager_c.h"

#include <algorithm>

#include "btBulletCollisionCommon.h"
void startGame() {
	Scene * initial = new Scene(Scene::Tipo::LEVEL);
	SceneManager::getInstance()->changeScene(initial);
	initial->setGameManager();

}
Scene::Scene(Tipo tipo):
	isSendingMessages(false), _gameManager(0), t_(tipo), counter_(0)
{
	switch (t_)
	{
	case MENU:
	{
		start_ = true;
	}
	break;
	case LEVEL:
	{
		start_ = false;
		std::string sceneDataPath = "..\\Data\\Levels\\Escena_1.xml";//Esto queda por ver c�mo darle valor y tal leyendo de fichero
		SceneData* sceneData = DataManager::getInstance()->loadScene(sceneDataPath);
		for (auto entityData : *sceneData)
		{
			Entity* newEntity = ObjectsFactory::getInstance()->create(*entityData, this);
			entities.push_back(newEntity);
		}
		delete sceneData;

	}
		break;
	default:
		break;
	}
	
}

Scene::~Scene()
{
	delete _gameManager;
	for (Entity* e : entities) {
		delete e;
		e = nullptr;
	}
}

///////////////////////////////TICK///////////////////////////////////////
void Scene::tick()
{
	if (!graveland.empty()) {
		for (auto e : graveland)
			destroyEntity(e);
		graveland.clear();
	}
	if (!messages.empty())
		_msgDeliver();


	for (Entity* e : entities) {
		e->tick();
		if (!e->isAlive())
			graveland.push_back(e);
	}

	if (start_ && counter_ >= 50) {
		start_ = false;
		startGame();
	}
	else{

		counter_++;
	}
	
}
///////////////////////////////TICK///////////////////////////////////////

void Scene::addListiner(MsgId id, Component* component)
{
	//Si este mensaje es nuevo, dadlo de alta junto con el componente que lo llamo
	if (!listeners.count(id)) {
		std::vector<Component*> nv;
		nv.push_back(component);
		listeners.emplace(id, nv);
	}
	//Si ya existe, simplemente a�ade el componente a la lista
	else
		listeners[id].push_back(component);
}

void Scene::removeListener(MsgId id, Component* component)
{
	//Si hay alguien suscrito a este mensaje
	if (listeners.count(id)) {
		auto it = listeners[id].begin();
		auto end = listeners[id].end();
		//encuentra el componente que se quieres desuscribir
		while (component != *it && it != end) it++;
		//si se ha encontrado, elim�nalo
		if (it != end) listeners[id].erase(it);
	}
}

void Scene::reciveMsg(Msg_Base* newMsg)
{
	if (isSendingMessages)
		messagesBuffer.push_back(newMsg);
	else
		messages.push_back(newMsg);
}

Entity* Scene::whoIs(EntityId id)
{
	auto entityIt = std::lower_bound(entities.begin(), entities.end(), id, 
		[&](Entity* entity, EntityId id) { return entity->getId() < id; }
	);
	if (entityIt != entities.end())
		return *entityIt;
	return nullptr;
}

Entity* Scene::whoIs(std::string name)
{
	for (Entity* entity : entities)
		if (entity->getName() == name)
			return entity;
	return nullptr;
}

void Scene::setGameManager()
{
	if (!_gameManager) {
		_gameManager = new Entity(this, 0, "GameManager");
		_gameManager->addComponent(new GameManager_c());
		_gameManager->init();
	}

}

std::string const Scene::getNameOf(EntityId id)
{
	return whoIs(id)->getName();
}

EntityId const Scene::getIdOf(std::string name)
{
	return whoIs(name)->getId();
}

void Scene::_msgDeliver()
{
	while (!messages.empty()) {
		isSendingMessages = true;
		Msg_Base* msg = messages.front();
		for (Component* listener : listeners[msg->id]) {
			if (msg->reciver != Msg_Base::broadcast) {
				if (msg->reciver == listener->getEntity()->getId())
					listener->listen(msg);
			}
			else
				listener->listen(msg);
		}
		delete msg;
		messages.pop_front();
	}
	isSendingMessages = false;
}

void Scene::_dumpMessages()
{
	if (!isSendingMessages) {
		for (auto m : messagesBuffer)
			messages.push_back(m);
		messagesBuffer.clear();
	}
}

void Scene::destroyEntity(Entity* entity)
{
	entities.remove(entity);
	delete entity;
}
