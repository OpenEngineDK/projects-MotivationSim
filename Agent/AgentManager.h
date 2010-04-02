//
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS)
//
// This program is free software; It is covered by the GNU General
// Public License version 2 or any later version.
// See the GNU General Public License for more details (see LICENSE).
//--------------------------------------------------------------------

#ifndef _AGENT_MANAGER_H_
#define _AGENT_MANAGER_H_

#include <Core/IListener.h>
#include <Core/EngineEvents.h>
#include <Scene/SceneNode.h>
#include "Agent.h"

#include <list>

using namespace OpenEngine::Core;
using namespace OpenEngine::Scene;
using namespace Motivation;

//namespace OpenEngine {
//namespace Logic {

  /**
   *
   */
  class AgentManager : public IListener<ProcessEventArg> {
  public:
    float timeModifier;
  
  protected:
    std::list<Agent*> agents;
    std::list<Agent*> deadAgents;
    Genes* genePool;
    SceneNode* scene;

  public:
    AgentManager(Genes* genes);
    virtual ~AgentManager();

	void Handle(ProcessEventArg arg);

    virtual void AddAgent(Agent* agent);
    virtual Agent* GetAgent(int i);
    virtual int GetAgentCount();

    virtual void Print();

    virtual SceneNode* GetSceneNode();

    virtual std::list<Agent*> GetAgentsInArea(Vector<3,float> pos, float radius);
  };

//}
//}

#endif // _AGENT_MANAGER_H_
