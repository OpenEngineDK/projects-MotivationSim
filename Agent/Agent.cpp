// Agent
// -------------------------------------------------------------------
// Copyright (C) 2009 Anders Nissen
//
// This program is free software; It is covered by the GNU General
// Public License version 2 or any later version.
// See the GNU General Public License for more details (see LICENSE).
//--------------------------------------------------------------------

#include <stdio.h>

#include <Scene/TransformationNode.h>
#include "Agent.h"
#include "AgentNode.h"

#include <Logic/Selector.h>

namespace Motivation {

    Agent::Agent(Genes* g, std::string id) {
        genes = g;

        selector = new Selector();
        behavior = new BehaviorTree(selector);

        oneTimeBehavior = NULL;

        path = NULL;
        waypointCounter = 0;

        this->id = id;

        timing = false;
        age = 0.0;
        lastTime = 0.0;
        
        timeModifier = 1.0;
    }

    void Agent::SetupScene(Vector<3,float> pos) {


        agentRotationNode = new TransformationNode();
        agentRotationNode->AddNode(CreateRenderNode());

        agentNode = new TransformationNode();
        agentNode->AddNode(agentRotationNode);
        agentNode->AddNode(new AgentPathRenderNode(this));

		waypointPos = pos.ToInt();
        Move(pos);
    }

    void Agent::Update(float timeModifier) {
    	this->timeModifier = timeModifier;
    	
        if (!timing) {
            lastTime   = Timer::GetTime().AsInt64() / 1000.0 / 1000.0;
            timing = true;
            return;
        }

        double time   = Timer::GetTime().AsInt64() / 1000.0 / 1000.0;
        double timeDt = (time - lastTime) * timeModifier;
        age          += timeDt;
        lastTime      = time;

        HandleUpdate(timeDt);
    }

    void Agent::HandleUpdate(float timeDt) {

    }

    void Agent::SetOneTimeBehavior(Task* t) {
        oneTimeBehavior = t;
    }

    RenderNode* Agent::CreateRenderNode() {
        return new AgentRenderNode();
    }

    void Agent::Print() {
        printf("[Agent, ");
        genes->Print();
        printf("]");
    }

    void Agent::Move(Vector<3,float> dir) {
        position += dir;
        agentNode->Move(dir[0], dir[1], dir[2]);
    }

    void Agent::SetPosition(Vector<3,float> pos) {
        position = pos;
        agentNode->SetPosition(pos);
    }

    Vector<3,float> Agent::GetPosition() {
        return position;
    }

    TransformationNode* Agent::GetAgentNode() {
        return agentNode;
    }

    TransformationNode* Agent::GetRotationNode() {
        return agentRotationNode;
    }

    CompositeTask* Agent::GetBehavior() {
        return selector;
    }

    Task::TaskStatus Agent::Execute() {
        if (!oneTimeBehavior) {
            return selector->Execute();
        }

        Task::TaskStatus status = oneTimeBehavior->Execute();
        if (status != Task::TASK_RUNNING) {
            delete oneTimeBehavior;
            oneTimeBehavior = NULL;
        }
        return status;
    }

    Genes* Agent::GetGenes() {
        return genes;
    }

}
