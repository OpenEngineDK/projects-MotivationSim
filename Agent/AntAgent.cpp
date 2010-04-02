// Ant Agent
// -------------------------------------------------------------------
// Copyright (C) 2009 Anders Nissen
//
// This program is free software; It is covered by the GNU General
// Public License version 2 or any later version.
// See the GNU General Public License for more details (see LICENSE).
//--------------------------------------------------------------------

#include "AntAgent.h"
#include "AntAgentNode.h"

namespace Motivation {

    AntAgent::AntAgent(Genes* g, std::string id) : Agent(g, id) {
        color             = Vector<3,float>(genes->GetValue("color1"), genes->GetValue("color2"), genes->GetValue("color3"));
        lifeExpectancy    = 40.0 + genes->GetValue("life_expectancy") * 250.0;
        metabolism        = 0.5 + genes->GetValue("metabolism") * 0.7;
        energyCapacity    = 20.0 + genes->GetValue("energy_capacity") * 200.0;
        energy            = energyCapacity;
        menstruation      = 0.3 * lifeExpectancy + (genes->GetInverseValue("menstruation")/3.0) * lifeExpectancy;
        menopause         = lifeExpectancy - (genes->GetInverseValue("menopause")/3.0) * lifeExpectancy;
        maxSize           = 0.5 + genes->GetInverseValue("size")/2.0;
        size              = 0.1 + maxSize / 10.0;
        nutritionCapacity = 10.0 + genes->GetValue("nutrition_capacity") * 200.0;
        nutrition         = nutritionCapacity;
        
        offspring = false;
        sleeping = false;
        dead = false;  
        counter = 0;     
    }

    void AntAgent::HandleUpdate(float timeDt) {
    	if (timeDt < 0.01) return;
    	if (dead) return;
    	
    	if (age > lifeExpectancy) {
        	//Speak("I died of old age");
        	//if (id == "Ant 3") genes->Print();
        	dead = true;
        	return;
        }
        
        nutrition -= metabolism * 1.5 * timeDt * (sleeping ? 0.5 : 1.0);
    	if (nutrition < 0.0) {
        	//Speak("I died of starvation");
        	//if (id == "Ant 3") genes->Print();
        	dead = true;
        	return;
        }
    	
        if (sleeping) {
        	energy += metabolism * timeDt * 5.0;
        	if (energy > energyCapacity) energy = energyCapacity;
        } else {
        	energy -= metabolism * timeDt;
        	if (energy < 0.0) {
	        	//Speak("I died of lack of energy");
	        	//if (id == "Ant 3") genes->Print();
	        	dead = true;
	        	return;
	        }
        }
        
        /*
        if (id == "Ant 3") {
        	counter++;
        	if (counter % 20 == 0) printf("[Ant 2:] Nutrition: %f,\t Energy: %f, \t Age: %f\n", nutrition/nutritionCapacity, energy/energyCapacity, age/lifeExpectancy);
        }
        */
        
        if (size < maxSize) size += ((maxSize-size) * timeDt) / 10.0;
    }

    void AntAgent::Feed(float nutrition) {
        this->nutrition += metabolism * nutrition;
        if (this->nutrition > nutritionCapacity) this->nutrition = nutritionCapacity;
    }

    void AntAgent::Move(Vector<3,float> dir) {
        Agent::Move(dir);
        energy -= dir.GetLength() / 10.0;
    }

    RenderNode* AntAgent::CreateRenderNode() {
        return new AntAgentRenderNode(color, this);
    }

    float AntAgent::GetFitness() {
        return genes->GetAverage();
    }

    Vector<3,float> AntAgent::GetColor() {
        return color;
    }

    bool AntAgent::ProposeMating(Agent* agent) {
        if (oneTimeBehavior) { return false; } // If we are already busy
        float breedingRequirement = genes->GetValue("breeding_requirement");
        // Only accept mating if the mate is fit enough
        return (fabs(GetFitness() - agent->GetFitness()) < breedingRequirement);
    }
}
