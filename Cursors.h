#include <Devices/SDLInput.h>
#include <Display/Viewport.h>
#include <Display/Frustum.h>
#include <Display/ViewingVolume.h>
#include <Display/InterpolatedViewingVolume.h>
#include <Display/SDLFrame.h>
#include <Logging/Logger.h>
#include <Renderers/OpenGL/Renderer.h>
#include <Renderers/OpenGL/RenderingView.h>
#include <Resources/ResourceManager.h>
#include <Resources/File.h>
#include <Resources/OBJResource.h>
#include <Resources/TGAResource.h>
#include <Scene/GeometryNode.h>
#include <Scene/TransformationNode.h>
#include <Scene/RenderNode.h>
#include <Meta/OpenGL.h>
#include <Display/FollowCamera.h>
#include <Devices/Symbols.h>

#include <string>
#include <fstream>

#include "LevelMap.h"
#include "Agent/AgentManager.h"

// Additional namespaces (others are in the header).
using namespace OpenEngine::Display;
using namespace OpenEngine::Devices;
using namespace OpenEngine::Renderers::OpenGL;
using namespace OpenEngine::Scene;
using namespace OpenEngine::Resources;
using namespace OpenEngine::Utils;
using namespace OpenEngine::Logic;

class CursorHandler : public IListener<KeyboardEventArg> {
private:
	Vector<3,int> startPos;
	int x;
	int y;
    int z;
	TransformationNode* cursorTrans;
	LevelMap* level;
    FollowCamera* camera;
    AgentManager* agentManager;
	
public:
	CursorHandler(LevelMap* level, FollowCamera* cam, TransformationNode* cursorTrans, Vector<3,int> pos, AgentManager* agentManager) {
		this->level = level;
		this->cursorTrans = cursorTrans;
        this->camera = cam;
        startPos = pos;
		this->x = pos[0];
		this->y = pos[1];
        this->z = pos[2];
		cursorTrans->Move(x, -y, z);
		
		this->agentManager = agentManager;
	}

    void Handle(KeyboardEventArg arg) {
    	if (arg.type != EVENT_PRESS) return;
    
		int diffX = 0;
    	int diffY = 0;
        int diffZ = 0;

    	switch (arg.sym) {
    	    case KEY_UP:       diffZ--; break;
            case KEY_DOWN:     diffZ++; break;
            case KEY_LEFT:     diffX--; break;
    	    case KEY_RIGHT:    diffX++; break;
            case KEY_PAGEUP:   diffY--; break;
            case KEY_PAGEDOWN: diffY++; break;
    	    case KEY_r:        diffX = startPos[0]-x; diffY = startPos[1]-y; diffZ = startPos[2]-z;	break;
    	    case KEY_c:        	camera->SetPosition(Vector<3,float>(0.0, 20.0, 0.0));
								camera->LookAt(Vector<3,float>(0.0, 0.0, 0.0)); break;
            case KEY_SPACE:
            	level->ChangeTile(x, y, z, (level->GetTile(x,y,z)->IsPassable() ? (Tile*)new Stone() : (Tile*)new Grass())); 
            	break;
            case KEY_RETURN:   camera->Follow(agentManager->GetAgent(0)->GetAgentNode()); break;
            case KEY_PLUS:     agentManager->timeModifier += 1.0; printf("Time modifier: %f\n", agentManager->timeModifier); break;
            case KEY_MINUS:    agentManager->timeModifier -= 1.0; if (agentManager->timeModifier < 0.0) agentManager->timeModifier = 0.0; 
                               printf("Time modifier: %f\n", agentManager->timeModifier); break;
    	    default: 
    	    	break;
    	}

    	if (arg.mod & KEY_MOD_SHIFT) {
    		diffX *= 4;
    		diffY *= 4;
    		diffZ *= 4;
    	}
    	
        if (x + diffX < 0 || x + diffX >= level->GetWidth())  { diffX = 0; }
        if (z + diffZ < 0 || z + diffZ >= level->GetHeight()) { diffZ = 0; }
        if (y + diffY < 0 || y + diffY >= level->GetDepth())  { diffY = 0; }
        
    	cursorTrans->Move(diffX, -diffY, diffZ);
        x += diffX;
        y += diffY;
        z += diffZ;
        if (diffY != 0) {
            level->SetActiveDepth(y);
        }
    }
};

class CursorRenderNode : public RenderNode {
public:
	CursorRenderNode() {}
	
	virtual void Apply(IRenderingView* renderingView) {
	    IRenderer* renderer = renderingView->GetRenderer();

	    Vector<3,float> color = Vector<3,float>(1.0, 0.0, 0.0);
	    float width = 15;
	    
	    renderer->DrawPoint(Vector<3,float>(0.0, -0.5, 0.0), color, width/2);
	    renderer->DrawLine(Line(Vector<3,float>(0.0, 1.5, 0.0), Vector<3,float>(0.0, -0.5, 0.0)), color, 2);
	    renderer->DrawPoint(Vector<3,float>(0.0, 1.5, 0.0), color, width);
	}
};

