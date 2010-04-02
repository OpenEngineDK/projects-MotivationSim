//
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS)
//
// This program is free software; It is covered by the GNU General
// Public License version 2 or any later version.
// See the GNU General Public License for more details (see LICENSE).
//--------------------------------------------------------------------

#ifndef _ANT_AGENT_NODE_H_
#define _ANT_AGENT_NODE_H_

#include <Renderers/IRenderer.h>
#include <Scene/RenderNode.h>
#include <Renderers/OpenGL/Renderer.h>
#include <Renderers/OpenGL/RenderingView.h>

#include "AntAgent.h"

using namespace Motivation;

using namespace OpenEngine::Utils;

using namespace OpenEngine::Core;
using namespace OpenEngine::Resources;
using namespace OpenEngine::Scene;
using namespace OpenEngine::Renderers;
using namespace OpenEngine::Renderers::OpenGL;

class AntAgentRenderNode : public RenderNode {
private:
    AntAgent* agent;
    Vector<3,float> color;
    //float size;

public:
    AntAgentRenderNode(Vector<3,float> color, AntAgent* agent) {
        this->agent = agent;
        this->color = color*0.8;
        //this->size = 0.4 + size*0.2;
    }

    virtual void Apply(IRenderingView* renderingView) {
        if (agent->IsDead()) return;

		//RenderMatrix(); return;

        float size = agent->GetSize() * 0.6;

        Vector<3,float> pos(-size/2, 0.01, 0.0);

        float* height = new float[2];
        float* width  = new float[2];
        
        height[0] = (0.1 + color[0]) * size;
        width[0] = (0.1 + color[1]) * size;

        height[1] = (0.1 + color[2]) * size;
        width[1] = (0.1 + color[0]) * size;

        RenderShape(pos, width, height, size/2, 0);
        
		RenderHorns(pos, height);
                
        
		int joins = 1+(int)(agent->GetGenes()->GetValue("joins")*8.0);
		for (int i = 0; i < joins; i++) {
	        pos[0] += size/4;

		    height[0] = height[1];
		    width[0]  = width[1];
		    
		    stringstream ss;
		    ss << "color" << (i+2);
		    float shape = agent->GetGenes()->GetValue(ss.str());

		    height[1] = (0.1 + shape) * size  - size * i * 0.1;
		    width[1]  = (0.1 + shape) * size  - size * i * 0.1;
		    
		    if (height[1] < height[0]/2) height[1] = height[0]/2;
		    if (width[1] < width[0]/2) width[1] = width[0]/2;

		    RenderShape(pos, width, height, size/4, i);
		}

		glDisable(GL_LIGHTING);
		float adulthood = agent->lifeExpectancy / 5.0;
       if (agent->offspring && agent->age < adulthood) {
	        glLineWidth(3.0);
	        glColor4f(1.0, 0.0, 0.5, 1.0);
	        glBegin(GL_LINES);
	        	glVertex3f(0.0, 0.0, 0.0);
	        	glVertex3f(0.0, 2.5 - 2.5 * (1.0 - ((adulthood - agent->age) / adulthood)), 0.0);
	        glEnd();
	        glLineWidth(1.0);
        }
        glEnable(GL_LIGHTING);
    }

    void RenderShape(Vector<3,float> pos, float width[2], float height[2], float size, int iteration) {
        float x = pos[0];
        float y = pos[1];
        float z = pos[2];

		Vector<3,float> normColor   = color - color * iteration * 0.1;
		Vector<3,float> darkerColor = Vector<3,float>(normColor[2]-0.2, normColor[1]-0.2, normColor[0]-0.2);

        glDisable(GL_LIGHTING);
        glBegin(GL_QUADS);
        for (int i = 0; i < 2; i++) {
            float theSize = size;
            // Draw the top and buttom sides
			glColor3f(normColor[0], normColor[1], normColor[2]);
            glVertex3f(x-theSize/2, y+i*height[0], z+width[0]/2);
            glNormal3f(0.0, 1.0, 0.0);
            glVertex3f(x+theSize/2, y+i*height[1], z+width[1]/2);
            glNormal3f(0.0, 1.0, 0.0);
            glVertex3f(x+theSize/2, y+i*height[1], z-width[1]/2);
            glNormal3f(0.0, 1.0, 0.0);
            glVertex3f(x-theSize/2, y+i*height[0], z-width[0]/2);
            glNormal3f(0.0, 1.0, 0.0);

            // Draw the left and right sides
            glColor3f(darkerColor[0], darkerColor[1], darkerColor[2]);
            glVertex3f(x+i*theSize-theSize/2, y,         z+width[i]/2);
            glVertex3f(x+i*theSize-theSize/2, y,         z-width[i]/2);  glColor3f(normColor[0], normColor[1], normColor[2]);
            glVertex3f(x+i*theSize-theSize/2, y+height[i], z-width[i]/2);
            glVertex3f(x+i*theSize-theSize/2, y+height[i], z+width[i]/2);

            // Draw the front and back sides
            glColor3f(darkerColor[0], darkerColor[1], darkerColor[2]);
            int flip = -1 + i * 2; // -1 at i = 0 and 1 at i = 1
            glVertex3f(x-theSize/2, y, z+flip*width[0]/2);
            glVertex3f(x+theSize/2, y, z+flip*width[1]/2);glColor3f(normColor[0], normColor[1], normColor[2]);
            glVertex3f(x+theSize/2, y+height[1],         z+flip*width[1]/2);	 
            glVertex3f(x-theSize/2, y+height[0],         z+flip*width[0]/2);
        }
        glEnd();
        glEnable(GL_LIGHTING);
    }
    
    void RenderHorns(Vector<3,float> pos, float height[2]) {
        int horns = 2;
        glDisable(GL_LIGHTING);
        glLineWidth(5.0);
		glColor3f(1.0, 0.0, 0.0);
		Vector<3,float> hornColor = color - Vector<3,float>(0.2, 0.2, 0.2);
		Vector<3,float> antennaColor = hornColor - Vector<3,float>(0.2, 0.2, 0.2);
		
		glColor3f(hornColor[0], hornColor[1], hornColor[2]);
        glBegin(GL_LINES);
        	for (int i = 0; i < horns; i++) {
	            glVertex3f(pos[0], pos[1] + height[0]/2, pos[2]);
            	glVertex3f(pos[0]-0.2, pos[1] + height[0] * 1.2, pos[2] + (-1+2*i) * 0.1);
            }
        glEnd();
        glLineWidth(1.0);
        
        glPointSize(8.0);
        glColor3f(antennaColor[0], antennaColor[1], antennaColor[2]);
        glBegin(GL_POINTS);
        	for (int i = 0; i < horns; i++) {
            	glVertex3f(pos[0]-0.2, pos[1] + height[0] * 1.2, pos[2] + (-1+2*i) * 0.1);
            }
        glEnd();
        glPointSize(1.0);
        glEnable(GL_LIGHTING);
    }

    /*
    void RenderMatrix() {
    	glColor3f(color[0], color[1], color[2]);
        glDisable(GL_LIGHTING);
        glBegin(GL_TRIANGLE_STRIP);
//        float shape[3][2] = {{1.0, 1.0}, {1.0, 1.0}, {1.0, 1.0}};
		Vector<3,float> point[2][2][2] = {
										   {
											{Vector<3,float>(0.0), Vector<3,float>(0.2)}, 
											{Vector<3,float>(0.0), Vector<3,float>(0.0)},
										   }, 
										   {
											{Vector<3,float>(0.0), Vector<3,float>(0.2)}, 
											{Vector<3,float>(0.0), Vector<3,float>(0.0)},
										   },
									     };
        for (int x = 0; x < 2; x++) {
        	for (int y = 0; y < 2; y++) {
        		for (int z = 0; z < 2; z++) {
        			Vector<3,float> value = point[x][y][z];
		        	glVertex3f(x + value[0], y + value[1], z + value[2]);
		        }
        	}
        }
        glEnd();
        glEnable(GL_LIGHTING);
    }
    */
};

#endif // _ANT_AGENT_NODE_H_
