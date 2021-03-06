#ifndef _H_MESHRENDERER_H_
#define _H_MESHRENDERER_H_
#include "Component.h"
#include "Components.h"

#include <string>
#include <Ogre.h>
class GraphicsManager;


class MeshRenderer_c :
	public Component
{
public:
	MeshRenderer_c(std::string meshName, float scale);
	~MeshRenderer_c();
	
	void start();

	virtual void listen(Msg_Base* msg);
	inline Ogre::SceneNode* getSceneNode() { return scnNode_; }
	

private:
	std::string meshPath;
	float scale;
	Ogre::SceneNode* scnNode_;

	Ogre::MaterialPtr mat;
	Ogre::MaterialPtr mat2;
	

};
#endif // !_H_MESHRENDERER_H_

