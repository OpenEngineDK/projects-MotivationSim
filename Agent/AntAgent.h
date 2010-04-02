// Agent
// -------------------------------------------------------------------
// Copyright (C) 2009 Anders Nissen
//
// This program is free software; It is covered by the GNU General
// Public License version 2 or any later version.
// See the GNU General Public License for more details (see LICENSE).
//--------------------------------------------------------------------

#ifndef _ANT_AGENT_H_
#define _ANT_AGENT_H_

#include "Agent.h"

//using namespace OpenEngine::Logic;

namespace Motivation {

	class AntAgent : public Agent {
	public:
	    float energy;
	    float energyCapacity;
	    
        float nutrition;
        float nutritionCapacity;
        
        float lifeExpectancy;
	
    protected:
        Vector<3,float> color;
        virtual RenderNode* CreateRenderNode();

        float metabolism;
        
        float menstruation;
        float menopause;

        float maxSize;
        float size;
        
        bool sleeping;
        
        bool dead;
        int counter;
		
	public:
        AntAgent(Genes* g, std::string id = "ANT");
        virtual ~AntAgent() {}

        Vector<3,float> GetColor();

        virtual void Feed(float nutrition);

        virtual void Move(Vector<3,float> dir);

        virtual void HandleUpdate(float timeDt);
        virtual bool IsDead() { return dead; }
        virtual bool IsReproducable() { return (age >= menstruation && age <= menopause); }
        virtual float GetFitness();

        virtual bool ProposeMating(Agent* agent);

        virtual float GetSize() { return size; }
        
        virtual void Sleep() { sleeping = true; }
        virtual void WakeUp() { sleeping = false; }
	};

}

#endif //_ANT_AGENT_H_
