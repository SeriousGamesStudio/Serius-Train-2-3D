#ifndef _H_INPUTMANAGER_H_
#define _H_INPUTMANAGER_H_

class Game;

class InputManager
{
public:
	InputManager(Game* game);
	~InputManager();

	void update();

};
#endif //!_H_INPUTMANAGER_H_

