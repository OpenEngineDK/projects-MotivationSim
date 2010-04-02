// Agent
// -------------------------------------------------------------------
// Copyright (C) 2009 Anders Nissen
//
// This program is free software; It is covered by the GNU General
// Public License version 2 or any later version.
// See the GNU General Public License for more details (see LICENSE).
//--------------------------------------------------------------------

#ifndef _AGENT_H_
#define _AGENT_H_

#include "Genes.h"
#include <vector>
#include <Logic/BehaviorTree.h>
#include <Logic/CompositeTask.h>
#include <Utils/Timer.h>
#include <Math/Vector.h>
#include <Scene/TransformationNode.h>

using namespace OpenEngine::Logic;

using namespace OpenEngine::Core;
using namespace OpenEngine::Scene;

using std::vector;

namespace Motivation {

	class Agent {
	public:
		float timeModifier;
		Vector<3,int> waypointPos;
		
		float  age;
	
	protected:
		Genes* genes;
        Vector<3,float> position;

        vector< Vector<3,int> >* path;
        int waypointCounter;

        TransformationNode* agentNode;
        TransformationNode* agentRotationNode;

        CompositeTask* selector;
        BehaviorTree* behavior;
        Task* oneTimeBehavior;

        bool   timing;
        double lastTime;       

        std::string id;

        

        virtual RenderNode* CreateRenderNode();

	public:
		bool offspring;
	
        Agent(Genes* g, std::string id = "AGENT");
        virtual ~Agent() { delete genes; delete selector; delete behavior; Speak("Agent object destroyed\n"); };

        virtual void SetupScene(Vector<3,float> pos);

        void Update(float timeModifier);
        virtual void HandleUpdate(float timeDt);

        virtual void Feed(float nutrition) = 0;

        virtual void Move(Vector<3,float> dir);
        void SetPosition(Vector<3,float> pos);
        Vector<3,float> GetPosition();

        TransformationNode* GetAgentNode();
        TransformationNode* GetRotationNode();

        CompositeTask* GetBehavior();

        //float GetGeneValue(string id);

        void Speak(std::string what) { printf("%s says '%s'\n", id.c_str(), what.c_str()); }

        void Print();

        Genes* GetGenes();
        
        string GetId() { return id; }

        void SetOneTimeBehavior(Task* t);

        Task::TaskStatus Execute();

        virtual bool IsDead() = 0;

        virtual bool ProposeMating(Agent* agent) = 0;
        /*
        virtual void ArrangeMating(Agent* agent, Vector<3,float> pos, bool breeder) {
            Speak("Mating arranged! -> TODO: Handle this. Being able to switch to this new task.");
            // GoToAction pos
            // Wait for mate to arrive
            // Spawn new agent
            // Resume behavior
        }
        */
        virtual float GetFitness() = 0;

        virtual bool IsReproducable() = 0;

        vector< Vector<3,int> >* GetPath() { return path; }
        void SetPath(vector< Vector<3,int> >* path) { this->path = path; }

        void SetWaypointCounter(int counter) { waypointCounter = counter; }
        int GetWaypointCounter() { return waypointCounter; }
	};

}

#endif //_AGENT_H_
