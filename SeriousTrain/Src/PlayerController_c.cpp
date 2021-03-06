#include "PlayerController_c.h"
#include "Entity.h"

#include "Walker_c.h"
#include "Transform_c.h"
#include "Camera_c.h"

#include <math.h>


PlayerController_c::PlayerController_c() :
	Component(ComponentType::PLAYERCONTROLLER),
	forward(false), back(false), right(false), left(false),
	forwardKey(OIS::KeyCode::KC_W), backKey(OIS::KeyCode::KC_S), rightKey(OIS::KeyCode::KC_D), leftKey(OIS::KeyCode::KC_A), toggleMouseKey(OIS::KC_LCONTROL),
	lookingAt(btVector3(0,0,0)), walkingTo(btVector3(0,0,0))
{
	InputManager::getInstance()->addKeyListener(this, "tecladoJugador");
	InputManager::getInstance()->addMouseListener(this, "ratonJugador");
	//Change predefined keys if the user changes the configuration
}


void PlayerController_c::start() 
{

	walker = static_cast<Walker_c*>(_myEntity->getComponent(ComponentType::WALKER));
	transform = static_cast<Transform_c*>(_myEntity->getComponent(ComponentType::TRANSFORM));
	cam = static_cast<Camera_c*>(_myEntity->getComponent(ComponentType::CAMERA));
#ifdef _DEBUG
	if (!walker)
		printf("ERROR: La entidad no contiene 'Walker' component!\n");
	if (!transform)
		printf("ERROR: La entidad no contiene 'Transform_c' component!\n");
	if (!cam)
		printf("ERROR: La entidad no contiene 'Camera' component!\n");
#endif // _DEBUG

}

void PlayerController_c::update() 
{
	//get rb position
	btVector3 pos = transform->get().getOrigin();
	//Las colisiones m�s chusqueras del mundo porque bullet no le gustan las kinematics :D
	float x = pos.getX();
	float y = pos.getY();
	float z = pos.getZ();
	if (x > 230) x = 230;
	else if (x < -40) x = -40;
	if (z > 23) z = 23;
	else if (z < -10) z = -10; 
	transform->setPosition(x, y, z);
	//set camera to that position
	cam->setPosition(x, y, z);
}
PlayerController_c::~PlayerController_c()
{
	InputManager::getInstance()->removeKeyListener(this);
	InputManager::getInstance()->removeMouseListener(this);
}

bool PlayerController_c::keyPressed(const OIS::KeyEvent & arg)
{
	bool somethingChange = false;
	

	if (arg.key == forwardKey) {
		forward = true;
		somethingChange = true;
	}
	if (arg.key == backKey) { 
		back = true; 
		somethingChange = true;
	}
	if (arg.key == rightKey) { 
		right = true; 
		somethingChange = true;
	}
	if (arg.key == leftKey) { 
		left = true; 
		somethingChange = true; 
	}

	if(somethingChange)
		updateMovementDirection();
	
	if (arg.key == toggleMouseKey) {

		sendMsg(new Msg::ToggleMouse(_myEntity->getId(), Msg_Base::broadcast));
	}
	return true;
}

bool PlayerController_c::keyReleased(const OIS::KeyEvent & arg)
{
	bool somethingChange = false;

	//No entra
	if (arg.key == forwardKey){	
		forward	= false;
		somethingChange = true;
	}
	if (arg.key == backKey) {
		back = false;
		somethingChange = true;
	}
	if (arg.key == rightKey) {
		right = false;
		somethingChange = true;
	}
	if (arg.key == leftKey) {
		left = false; 
		somethingChange = true;
	}
	if(somethingChange)
		updateMovementDirection();
	
	return true;
}


bool PlayerController_c::mousePressed(const OIS::MouseEvent & arg, OIS::MouseButtonID id)
{
	//Shoot!
	return true;
}

bool PlayerController_c::mouseReleased(const OIS::MouseEvent & arg, OIS::MouseButtonID id)
{
	//Exit 
	return true;
}


void PlayerController_c::listen(Msg_Base * msg)
{
	switch (msg->id)
	{
	case MsgId::LOOKING_TO: 
	{
		Msg::LookAt* p = static_cast<Msg::LookAt*>(msg);
		lookingAt.setValue(p->x, 0, p->z);
		updateMovementDirection();
		break;
	}
	default:
		break;
	}
}

void PlayerController_c::updateMovementDirection()
{
	bool f = forward;
	bool b = back;
	bool r = right;
	bool l = left;

	if (f && b)	f = b = false;
	if (l && r)	r = l = false;

	//If no direction return
	if (!f && !b && !r && !l) {
		walker->setDirection(0, 0);
		return;
	}
	float angle = 0.0f;

	{//Get rotation angle
	/*SIMPLE MOVES*/
		//foward
			 if (f && !r && !l)/*angle = 0*/;		
		//back
		else if (b && !r && !l) angle = 3.141592653589f;
		//right
		else if (!f && !b && r) angle = 3.141592653589f * -0.5f;
		//left
		else if (!f && !b && l)	angle = 3.141592653589f *  0.5f;
	/*COMPOSE MOVES*/
		//foward-right
		else if (f && r)		angle = 3.141592653589f * -0.25f;
		//foward-left
		else if (f && l)		angle = 3.141592653589f *  0.25f;
		//back-right
		else if (b && r)		angle = 3.141592653589f * -0.75f;
		//back-left
		else if (b && l)		angle = 3.141592653589f *  0.75f;
	}

	walkingTo = lookingAt.rotate({ 0,1,0 }, angle);
#ifdef _DEBUG
	printf("*************************************************************\n");
	printf("Camera looking at: %f, %f, %f\n", lookingAt.x(), lookingAt.y(), lookingAt.z());
	printf("Walking to:        %f, %f, %f\n", walkingTo.x(), walkingTo.y(), walkingTo.z());
#endif // _DEBUG
	walker->setDirection(walkingTo.x(), walkingTo.z());
}
