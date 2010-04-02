//
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS)
//
// This program is free software; It is covered by the GNU General
// Public License version 2 or any later version.
// See the GNU General Public License for more details (see LICENSE).
//--------------------------------------------------------------------

#include "AgentManager.h"


//namespace OpenEngine {
//namespace Logic {

  /**
   *
   */

AgentManager::AgentManager(Genes* genes) {
  genePool = genes;
  scene = new SceneNode();
  timeModifier = 1.0;
}

AgentManager::~AgentManager() {
  agents.~list();
  deadAgents.~list();
}

void AgentManager::Handle(ProcessEventArg arg) {
std::list<Agent*>::iterator agentIter;
for (agentIter = agents.begin(); agentIter != agents.end(); ) {
    Agent* agent = (*agentIter);
    agent->Update(timeModifier);
    if (agent->IsDead()) {
        //deadAgents.push_back(*agentIter);
        agentIter = agents.erase(agentIter);
        if (agents.size() == 0) break;
    } else {
        agent->Execute();
        agentIter++;
    }
}
}

void AgentManager::AddAgent(Agent* agent) {
    agents.push_back(agent);
    scene->AddNode(agent->GetAgentNode());
}

int AgentManager::GetAgentCount() {
    return agents.size();
}

Agent* AgentManager::GetAgent(int i) {
    return agents.back();
}

void AgentManager::Print() {
    printf("[AgentManager:\n");
    std::list<Agent*>::iterator agentIter;
    for (agentIter = agents.begin(); agentIter != agents.end(); agentIter++) {
        Agent* agent = (*agentIter);
        agent->Print();
        printf("\n");
    }
    printf("]\n");
}

SceneNode* AgentManager::GetSceneNode() {
    return scene;
}

std::list<Agent*> AgentManager::GetAgentsInArea(Vector<3,float> pos, float radius) {
    std::list<Agent*> areaAgents;
    std::list<Agent*>::iterator agentIter;
    for (agentIter = agents.begin(); agentIter != agents.end(); agentIter++) {
        Agent* agent = (*agentIter);
        if (agent->GetPosition().Distance(pos) <= radius) {
            areaAgents.push_back(agent);
        }
    }
    return areaAgents;
}


//}
//}


