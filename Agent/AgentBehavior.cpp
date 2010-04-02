//
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS)
//
// This program is free software; It is covered by the GNU General
// Public License version 2 or any later version.
// See the GNU General Public License for more details (see LICENSE).
//--------------------------------------------------------------------


#include "AgentBehavior.h"

#include <Logic/WaitAction.h>
#include <Math/Math.h>

using namespace OpenEngine::Math;
/*
using namespace OpenEngine::Utils;
using namespace Motivation;
using namespace OpenEngine::Logic;
*/


GoToAction::GoToAction(AntAgent* agent, LevelMap* level, Vector<3,int> dest) : TimedTask() {
    this->agent = agent;
    this->level = level;
    destination = dest;
    speed = 0.0005 + (0.002 * agent->GetGenes()->GetValue("speed")) * 0.01;
    acceptanceRadius = 0.4;
    pathResult = MicroPather::NO_SOLUTION;
    turnRate = 0.0;

    waypointCounter = 0;
}

GoToAction::~GoToAction() {
    path.clear();
    agent->SetPath(NULL);
}

void GoToAction::Start() {
    waypointCounter = 0;
    path.clear();
    float totalCost = 0;
    Vector<3,int> pos = agent->waypointPos; //agent->GetPosition().ToInt();
    pathResult = level->GetPath(pos, destination, &path, &totalCost);
    oldTurn = agent->GetRotationNode()->GetRotation();

    agent->SetPath(&path);
    agent->SetWaypointCounter(0);
}

void GoToAction::End() {
    //printf("Done...\n");
}

Task::TaskStatus GoToAction::Execute(double timeDt, double totalTime) {
    if (pathResult != MicroPather::SOLVED) {
        path.clear();
        return TASK_FAIL;
    }
    if (waypointCounter == path.size()) {
        path.clear();
        return TASK_SUCCESS;
    }

    Vector<3,float> diff = level->GetPosition(path[waypointCounter]) - agent->GetPosition();
    float tileAcceptanceRadius = ((waypointCounter == path.size() - 1) ? 0.01 * agent->timeModifier : acceptanceRadius); // decrease acceptance radius on the dest. tile
    if (diff.GetLength() <= tileAcceptanceRadius) {
        //agent->SetPosition(path[waypointCounter].ToFloat());
        agent->waypointPos = path[waypointCounter];
        positionDiff = Vector<3,float>(0.0, 0.0, 0.0);
        waypointCounter++;
        turnRate = 0.0;
        oldTurn = agent->GetRotationNode()->GetRotation();
        agent->SetWaypointCounter(waypointCounter+1);
        return TASK_RUNNING;
    }

    diff.Normalize();

    if (turnRate <= 1.00) {
        float rotation = atan2(-diff[0], diff[2]);

        turnRate += 0.002 * timeDt * agent->timeModifier;
        if (turnRate > 1.0) turnRate = 1.0;

        Vector<3,float> upVector(0.0, -1.0, 0.0);
        Quaternion<float> rotationQ(rotation - PI/2, upVector);
        Quaternion<float> slerpQ(oldTurn, rotationQ, turnRate);

        agent->GetRotationNode()->SetRotation(slerpQ);
    }

    agent->Move(diff * speed * timeDt  * agent->timeModifier);

    return TASK_RUNNING;
}

vector< Vector<3,int> >* GoToAction::GetPath() {
    return &path;
}

int GoToAction::GetPathResult() {
    return pathResult;
}




FeedAction::FeedAction(AntAgent* agent, LevelMap* level, Vector<3,int> pos) : Task() {
    this->agent = agent;
    this->level = level;
    this->pos   = pos;
}

Task::TaskStatus FeedAction::Execute() {
    Tile* tile = level->GetTile(pos);
    float foodAtLocation = tile->GetFoodAmount();

    if (foodAtLocation < 0.1) return TASK_FAIL;

    float ateFood = 0.2;
    if (foodAtLocation < ateFood) ateFood = foodAtLocation;
    tile->SetFoodAmount(foodAtLocation - ateFood);
    agent->Feed(ateFood * 3.0 * agent->nutritionCapacity); // Eat for 60 % of nutritionCapacity

    return TASK_SUCCESS;
}





FindFoodAction::FindFoodAction(AntAgent* agent, LevelMap* level) : Sequence() {
    Vector<3,int> pos = level->FindPassableTileInArea(agent->GetPosition().ToInt(), 2);
    AppendTask(new GoToAction(agent, level, pos));
    AppendTask(new FeedAction(agent, level, pos));
    AppendTask(new WaitAction(5.0/agent->timeModifier));
}



WaitForMateAction::WaitForMateAction(AntAgent* agent, AntAgent* mate, float accRadius) : Task() {
    this->agent        = agent;
    this->mate         = mate;
    this->accRadius    = accRadius;
}

WaitForMateAction::~WaitForMateAction() {}

Task::TaskStatus WaitForMateAction::Execute() {
    if (agent->GetPosition().Distance(mate->GetPosition()) < accRadius) { /* agent->Speak("Mate arrived!"); */ return TASK_SUCCESS; }
    if (mate->IsDead()) { /* agent->Speak("Mate dead :("); */ return TASK_FAIL; }
    //agent->Speak("Waiting for mate...");
    return TASK_RUNNING;
}





BreedAction::BreedAction(AntAgent* agent, AntAgent* mate, AgentManager* agentManager, LevelMap* level) : Task() {
    this->agent        = agent;
    this->mate         = mate;
    this->agentManager = agentManager;
    this->level        = level;
}

BreedAction::~BreedAction() {}

Task::TaskStatus BreedAction::Execute() {
    // Wait a while?
    //agent->Speak("Breeding!");

    if  (!agent->IsReproducable()) { agent->Speak("I cannot reproduce :|"); return TASK_FAIL; }
    if  (!mate->IsReproducable()) { agent->Speak("My mate cannot reproduce :|"); return TASK_FAIL; }

    int litterSize = 1 + (int)((agent->GetGenes()->GetValue("litter_size")) * 3.5);
    if  (litterSize == 0) { agent->Speak("Breeding didn't produced offspring :|"); return TASK_FAIL; }

	std::stringstream ss;
	ss << "Gave birth to " << litterSize << " beautiful " << (litterSize == 1 ? "baby" : "babies");
	agent->Speak(ss.str());
	
    for (int i = 0; i < litterSize; i++) {
        Genes* genes = agent->GetGenes()->Combine(mate->GetGenes());
        AntAgent* antAgent = new AntAgent(genes);
        antAgent->SetupScene(agent->GetPosition());

        Task* task = new MotivationTask(antAgent, agentManager, level);
        antAgent->GetBehavior()->AppendTask( task );
        antAgent->offspring = true;

        agentManager->AddAgent(antAgent);
    }

    return TASK_SUCCESS;
}





ArrangeMatingAction::ArrangeMatingAction(AntAgent* agent, AntAgent* mate, AgentManager* agentManager, LevelMap* level, Vector<3,int> pos, bool breeder) : Sequence() {
    AppendTask(new GoToAction(agent, level, pos));
    AppendTask(new WaitForMateAction(agent, mate, 1.0));
    AppendTask(new WaitAction(3.0 / agent->timeModifier));
    if (breeder) { AppendTask(new BreedAction(agent, mate, agentManager, level)); } // Only the "female" breeds offspring
}







FindMateAction::FindMateAction(AntAgent* agent, AgentManager* agentManager, LevelMap* level) : Task() {
    this->agent        = agent;
    this->agentManager = agentManager;
    this->level        = level;
}

FindMateAction::~FindMateAction() {}

Task::TaskStatus FindMateAction::Execute() {
    if (!agent->IsReproducable()) return TASK_FAIL;

    // find a suitable mate within distance
    float mateFindingRadius = 10.0;

    std::list<Agent*> areaAgents = agentManager->GetAgentsInArea(agent->GetPosition(), mateFindingRadius);

    std::list<Agent*>::iterator agentIter;
    for (agentIter = areaAgents.begin(); agentIter != areaAgents.end(); agentIter++) {
        Agent* possibleMate = (*agentIter);
        if (possibleMate == agent) continue;

        if (!possibleMate->IsReproducable()) continue;

        if (!agent->ProposeMating(possibleMate)) continue;
        if (!possibleMate->ProposeMating(agent)) continue;

        Vector<3,int> matingPos;

        if (agent->GetPosition().Distance(possibleMate->GetPosition()) <= 1.0) {
            matingPos = possibleMate->GetPosition().ToInt();
        } else {
            //Vector<3,float> displacement(0.4, 0.4, 0.4);
            //waypointPos
            GoToAction matePath(agent, level, possibleMate->GetPosition().ToInt());
            matePath.Start();

            if (matePath.GetPathResult() != MicroPather::SOLVED) {
                //agent->Speak("Cannot reach my lover!");
                //("Me: %p %s, lover: %p %s, path size: %i\n", agent, agent->GetPosition().ToInt().ToString().c_str(), possibleMate, possibleMate->GetPosition().ToInt().ToString().c_str(), matePath.GetPath()->size());
                continue;
            }

            int pathLength = matePath.GetPath()->size();
            matingPos = matePath.GetPath()->at((int)(pathLength/2));
        }

        AntAgent* mate = (AntAgent*)possibleMate;

        agent->SetOneTimeBehavior(new ArrangeMatingAction(agent, mate, agentManager, level, matingPos, false));
        mate->SetOneTimeBehavior(new ArrangeMatingAction(mate, agent, agentManager, level, matingPos, true));

        return TASK_SUCCESS;
    }

    //agent->Speak("No possible mates within radius!");

    return TASK_FAIL;
}




SleepAction::SleepAction(AntAgent* agent, float duration) {
	this->agent = agent;
	this->duration = duration;
}

SleepAction::~SleepAction() {}

void SleepAction::Start() {
	agent->Sleep();
}

void SleepAction::End() {
	agent->WakeUp();
}

Task::TaskStatus SleepAction::Execute(double timeDt, double totalTime) {
	if (totalTime >= (duration  / agent->timeModifier) * 1000) return TASK_SUCCESS;
	return TASK_RUNNING;
}



FindBedAndSleepAction::FindBedAndSleepAction(AntAgent* agent, LevelMap* level) {
	AppendTask(new GoToAction(agent, level, level->GetBedTile()));
	AppendTask(new SleepAction(agent, 10.0));
}



MotivationTask::MotivationTask(AntAgent* agent, AgentManager* agentManager, LevelMap* level) : Task() {
    this->agent        = agent;
    this->agentManager = agentManager;
    this->level        = level;
    this->task         = NULL;
}

MotivationTask::~MotivationTask() {}

Task::TaskStatus MotivationTask::Execute() {
    // motivation for
    // - sleeping
    // - eating
    // - breeding

    //sense
    if (!task) {
    	//task = new FindBedAndSleepAction(agent, level);
    	//task = new GoToAction(agent, level, level->FindPassableTile());
    	/*
        float randValue = ((float)rand()/RAND_MAX);
        if (randValue < 0.1) {
            task = new GoToAction(agent, level, level->FindPassableTile());
        } else if (randValue < 0.4)  {
            task = new FindMateAction(agent, agentManager, level);
        } else if (randValue < 0.7) {
            task = new FindFoodAction(agent, level);
        } else if (randValue < 1.0) {
            task = new FindBedAndSleepAction(agent, level);   
        } else {
            task = new WaitAction(1.0*agent->timeModifier);
        }
        */
        
        Genes* g = agent->GetGenes();
        
        float nutritionPercentage = agent->nutrition / agent->nutritionCapacity;      
        float energyPercentage    = agent->energy    / agent->energyCapacity;
        
        float feedingMotivation = 0.3 + g->GetValue("feeding_motivation")/3.0;
        float restingMotivation = 0.4 + g->GetValue("resting_motivation")/3.0;
        float breedingMotivation = 0.3 + g->GetValue("breeding_motivation")/3.0;
        
        if (feedingMotivation > nutritionPercentage) {
        	//if (agent->GetId() == "Ant 3") agent->Speak("+++ Finding food");
			task = new FindFoodAction(agent, level);
		} else if (restingMotivation > energyPercentage) {
			//if (agent->GetId() == "Ant 3") agent->Speak("+++ Finding rest");
			task = new FindBedAndSleepAction(agent, level);
		} else if (agent->IsReproducable() && breedingMotivation  > ((float)rand()/RAND_MAX)) {
			//if (agent->GetId() == "Ant 3") agent->Speak("+++ Finding mate");
			task = new FindMateAction(agent, agentManager, level);
		} else {
			float randValue = ((float)rand()/RAND_MAX);
			if (randValue < 0.8) {
				//if (agent->GetId() == "Ant 3") agent->Speak("+++ Finding idle");
				task = new GoToAction(agent, level, level->FindPassableTileInArea(agent->GetPosition().ToInt(), 3));
			} else {
				//if (agent->GetId() == "Ant 3") agent->Speak("+++ Finding wait");
				task = new WaitAction(1.0/agent->timeModifier);
			}
		}
        
    }

    Task::TaskStatus status = task->Execute();
    if (status != TASK_RUNNING) {
        delete task;
        task = NULL;
    }

    return TASK_RUNNING;
}


