//
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS)
//
// This program is free software; It is covered by the GNU General
// Public License version 2 or any later version.
// See the GNU General Public License for more details (see LICENSE).
//--------------------------------------------------------------------

#ifndef _AGENT_NODE_H_
#define _AGENT_NODE_H_

#include <Renderers/IRenderer.h>
#include <Scene/RenderNode.h>
#include <Renderers/OpenGL/Renderer.h>
#include <Renderers/OpenGL/RenderingView.h>

#include "Agent.h"

using namespace Motivation;

using namespace OpenEngine::Utils;

using namespace OpenEngine::Core;
using namespace OpenEngine::Resources;
using namespace OpenEngine::Scene;
using namespace OpenEngine::Renderers;
using namespace OpenEngine::Renderers::OpenGL;

class AgentRenderNode : public RenderNode {
    public:
    virtual void Apply(IRenderingView* renderingView) {
        glPointSize(5.0);
        glColor3f(0.2, 0.2, 1.0);
        glBegin(GL_POINTS);
            glVertex3f(0.0, 0.02, 0.0);
        glEnd();
        glPointSize(1.0);
        glEnable(GL_LIGHTING);
    }
};

class AgentPathRenderNode : public RenderNode {
    private:
        Agent* agent;
        Vector<3,float> color;
        float size;

    public:
        AgentPathRenderNode(Agent* agent) {
            this->agent = agent;
            Genes* g = agent->GetGenes();
            this->color = Vector<3,float>(g->GetValue("color1"), g->GetValue("color2"), g->GetValue("color3"));
            this->size = agent->GetGenes()->GetValue("size");
        }

        virtual void Apply(IRenderingView* renderingView) {
            if (agent->IsDead()) return;
            if (!agent->GetPath()) return;

            Vector<3,float> pos = agent->GetPosition();
            
            glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			float alpha = 0.2;

            glDisable(GL_LIGHTING);
            glLineWidth(1.0);
            glColor4f(color[0]+0.2, color[1]+0.2, color[2]+0.2, alpha);
            glBegin(GL_LINE_STRIP);
            glVertex3f(0.0, 0.02 + size/4, 0.0);
            for (unsigned int i = agent->GetWaypointCounter() - 1; i < agent->GetPath()->size(); i++) {
                Vector<3,int> node = (*(agent->GetPath()))[i];
                glVertex3f(node[0]-pos[0], 0.02-node[1]-pos[1] + size/4, node[2]-pos[2]);
            }
            glEnd();
            glLineWidth(1.0);


            glPointSize(3.0);
            glColor4f(color[0], color[1], color[2], alpha);
            glBegin(GL_POINTS);
            for (unsigned int i = agent->GetWaypointCounter() - 1; i < agent->GetPath()->size(); i++) {
                Vector<3,int> node = (*(agent->GetPath()))[i];
                glVertex3f(node[0]-pos[0], 0.02-node[1]-pos[1] + size/4, node[2]-pos[2]);
            }
            glEnd();
            glPointSize(1.0);
            
            
 
            /*
            if (agent->GetId() == "Ant 33") {
                glLineWidth(5.0);
		        glColor4f(1.0, 1.0, 0.0, 1.0);
		        glBegin(GL_LINES);
		        	glVertex3f(0.0, 0.0, 0.0);
		        	glVertex3f(0.0, 2.0, 0.0);
		        glEnd();
		        
		        glPointSize(50.0);
		        glBegin(GL_POINTS);
		        	glVertex3f(0.0, 0.0, 0.0);
		        	if (agent->GetPath() && agent->GetPath()->size() > 0) {
			        	Vector<3,int> end = (*agent->GetPath())[agent->GetPath()->size()-1] - agent->waypointPos;
			        	glColor4f(0.0, 1.0, 1.0, 1.0);
			        	glVertex3f(end[0], end[1], end[2]);
			        }
		        glEnd();
		        
		        glLineWidth(1.0);
            }
            */
            glEnable(GL_LIGHTING);                      
            glDisable(GL_BLEND);
        }
};

#endif // _AGENT_NODE_H_
