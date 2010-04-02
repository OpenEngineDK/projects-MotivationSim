//
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS)
//
// This program is free software; It is covered by the GNU General
// Public License version 2 or any later version.
// See the GNU General Public License for more details (see LICENSE).
//--------------------------------------------------------------------

#ifndef _AGENT_BEHAVIOR_H_
#define _AGENT_BEHAVIOR_H_


#include <Logic/TimedTask.h>
#include <Logic/CallbackTask.h>
#include <Logic/BehaviorTree.h>
#include <Logic/Sequence.h>

#include "AgentManager.h"
#include "AntAgent.h"
#include "../LevelMap.h"

class GoToAction : public TimedTask {
protected:
    AntAgent* agent;
    LevelMap* level;
    Vector<3,int> destination;
    float speed;
    float acceptanceRadius;
    vector< Vector<3,int> > path;
    int pathResult;
    unsigned int waypointCounter;
    Vector<3,float> positionDiff;
    float turnRate;
    Quaternion<float> oldTurn;

public:
    GoToAction(AntAgent* agent, LevelMap* level, Vector<3,int> dest);
    virtual ~GoToAction();

    virtual void Start();
    virtual void End();
    virtual TaskStatus Execute(double timeDt, double totalTime);

    vector< Vector<3,int> >* GetPath();
    int GetPathResult();
};


class FeedAction : public Task {
    protected:
        AntAgent* agent;
        LevelMap* level;
        Vector<3,int> pos;

    public:
        FeedAction(AntAgent* agent, LevelMap* level, Vector<3,int> pos);
        virtual ~FeedAction() {}

        virtual TaskStatus Execute();
};


class FindFoodAction : public Sequence {
    public:
        FindFoodAction(AntAgent* agent, LevelMap* level);
};


class WaitForMateAction : public Task {
    protected:
        AntAgent* agent;
        AntAgent* mate;
        float accRadius;

    public:
        WaitForMateAction(AntAgent* agent, AntAgent* mate, float accRadius = 1.0);
        virtual ~WaitForMateAction();

        virtual TaskStatus Execute();
};


class BreedAction : public Task {
    protected:
        AntAgent* agent;
        AntAgent* mate;
        AgentManager* agentManager;
        LevelMap* level;

    public:
        BreedAction(AntAgent* agent, AntAgent* mate, AgentManager* agentManager, LevelMap* level);
        virtual ~BreedAction();

        virtual TaskStatus Execute();
};


class ArrangeMatingAction : public Sequence {
    public:
        ArrangeMatingAction(AntAgent* agent, AntAgent* mate, AgentManager* agentManager, LevelMap* level, Vector<3,int> pos, bool breeder);
};


class FindMateAction : public Task {
    protected:
        AntAgent* agent;
        AgentManager* agentManager;
        LevelMap* level;

    public:
        FindMateAction(AntAgent* agent, AgentManager* agentManager, LevelMap* level);
        virtual ~FindMateAction();

        virtual TaskStatus Execute();
};



class FindBedAndSleepAction : public Sequence {
    public:
        FindBedAndSleepAction(AntAgent* agent, LevelMap* level);
};



class SleepAction : public TimedTask {
protected:
    AntAgent* agent;
    float duration;

public:
    SleepAction(AntAgent* agent, float duration);
    virtual ~SleepAction();

    virtual void Start();
    virtual void End();
    virtual TaskStatus Execute(double timeDt, double totalTime);
};


class MotivationTask : public Task {
    protected:
        AntAgent* agent;
        AgentManager* agentManager;
        LevelMap* level;
        Task* task;

    public:
        MotivationTask(AntAgent* agent, AgentManager* agentManager, LevelMap* level);
        virtual ~MotivationTask();

        virtual TaskStatus Execute();
};

#endif // _AGENT_BEHAVIOR_H
