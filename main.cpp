// Main setup for the OpenEngine Racer project.
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS)
//
// This program is free software; It is covered by the GNU General
// Public License version 2 or any later version.
// See the GNU General Public License for more details (see LICENSE).
//--------------------------------------------------------------------

// Simple setup of a rendering engine
#include <Utils/SimpleSetup.h>

// Display structures
#include <Display/FollowCamera.h>

// Rendering structures
#include <Scene/RenderNode.h>
#include <Renderers/OpenGL/RenderingView.h>

// Resources
#include <Resources/IModelResource.h>
#include <Resources/ResourceManager.h>

// Scene structures
#include <Scene/SceneNode.h>
#include <Scene/GeometryNode.h>
#include <Scene/TransformationNode.h>
#include <Scene/VertexArrayTransformer.h>
#include <Scene/DisplayListTransformer.h>
#include <Scene/PointLightNode.h>
#include <Scene/DirectionalLightNode.h>
#include <Renderers/OpenGL/LightRenderer.h>
#include <Renderers/TextureLoader.h>

#include <Utils/MoveHandler.h>
#include <Utils/Statistics.h>

// Project files
#include <Logic/BehaviorTree.h>
#include <Logic/Selector.h>
#include <Logic/Sequence.h>
#include <Logic/Task.h>
#include <Logic/ProbabilitySelector.h>
#include <Logic/RandomSelector.h>
#include <Logic/DebugDecorator.h>
#include <Logic/LoopOnSuccessDecorator.h>
#include <Logic/WaitAction.h>
#include <Logic/PrintAction.h>
#include <Logic/Parallel.h>
#include <Logic/FixedStatusTask.h>


//#include "LevelMap.h"
//#include "UnitManager.h"
//#include "Agent/AgentManager.h"
#include "Cursors.h"
//#include "Logic/UnitTasks.h"
#include "Agent/AgentBehavior.h"

#include "Agent/Genes.h"
#include "Agent/AntAgent.h"

//#include <Display/AntTweakBarModule.h>
//#include "Plist/PlistBar.h"

// Additional namespaces
using namespace OpenEngine::Core;
using namespace OpenEngine::Logging;
using namespace OpenEngine::Devices;
using namespace OpenEngine::Renderers;
using namespace OpenEngine::Resources;
using namespace OpenEngine::Utils;
using namespace OpenEngine::Scene;
using namespace OpenEngine::Display;
using namespace OpenEngine::Renderers::OpenGL;

// Configuration structure to pass around to the setup methods
struct Config {
    SimpleSetup           setup;
    FollowCamera*         camera;
    ISceneNode*           scene;
    Config()
        : setup(SimpleSetup("<<Motivation Sim>>"))
        , camera(NULL)
        , scene(NULL)
    {

    }
};

// Forward declaration of the setup methods
void SetupResources(Config&);
void SetupDisplay(Config&);
void SetupScene(Config&);
void SetupRendering(Config&);
void SetupDevices(Config&);
void SetupDebugging(Config&);

TransformationNode* cursorTrans;

int main(int argc, char** argv) {
    Config config;

    // Print usage info.
    logger.info << "========= Running The Motivation Sim Project =========" << logger.end;
    logger.info << logger.end;

    // Setup the engine
    SetupResources(config);
    SetupDisplay(config);
    SetupScene(config);
    SetupRendering(config);
    SetupDevices(config);

    // Start up the engine.
    config.setup.GetEngine().Start();

    // Return when the engine stops.
    return EXIT_SUCCESS;
}

void SetupResources(Config& config) {
    config.setup.AddDataDirectory("projects/MotivationSim/data/");

    // load the resource plug-ins
    ResourceManager<ITextureResource>::AddPlugin(new TGAPlugin());
    ResourceManager<IModelResource>::AddPlugin(new OBJPlugin());

}

void SetupDisplay(Config& config) {  
    config.camera  = new FollowCamera( *(new InterpolatedViewingVolume(*config.setup.GetCamera() )));
    config.setup.SetCamera(*config.camera);
}

void SetupRendering(Config& config) {
    // Add rendering initialization tasks
    TextureLoader* textureLoader = new TextureLoader( config.setup.GetRenderer() );
    config.setup.GetRenderer().PreProcessEvent().Attach( *textureLoader );

    // Supply the scene to the renderer
    delete config.setup.GetScene();
    config.setup.SetScene(*config.scene);

    config.setup.GetRenderer().PreProcessEvent().Attach( *(new LightRenderer(*config.camera)) );

    config.setup.GetEngine().InitializeEvent().Attach(config.setup.GetRenderer());
    config.setup.GetEngine().ProcessEvent().Attach(config.setup.GetRenderer());
    config.setup.GetEngine().DeinitializeEvent().Attach(config.setup.GetRenderer());
}

void SetupDevices(Config& config) {

    // Register movement handler to be able to move the camera
    MoveHandler* move_h = new MoveHandler(*config.camera, config.setup.GetMouse());
    config.setup.GetKeyboard().KeyEvent().Attach(*move_h);

    config.setup.GetEngine().InitializeEvent().Attach(*move_h);
    config.setup.GetEngine().ProcessEvent().Attach(*move_h);
    config.setup.GetEngine().DeinitializeEvent().Attach(*move_h);
    
}
/*
Vector<3,int> FindRandNeighbourPos(LevelMap* map, Vector<3,int> pos) {
    bool goodPos = false;
    Vector<3,int> foundPos;
    while (!goodPos) {
        int rand1 = pos[0] - 1 + ((float)rand()/RAND_MAX) * 3;
        int rand2 = pos[1] - 1 + ((float)rand()/RAND_MAX) * 3;
        int rand3 = pos[2] - 1 + ((float)rand()/RAND_MAX) * 3;
        foundPos = Vector<3,int>(rand1, rand2, rand3);

        Tile* tile = map->GetTile(foundPos);
        if (tile == NULL) continue;
        if (tile->IsPassable()) {
            goodPos = true;
            tile->SetFoodAmount(tile->GetFoodAmount() + 1.0);
        }
    }
    return foundPos;
}
*/
void SetupScene(Config& config) {
    if (config.scene != NULL)
        throw Exception("Setup scene dependencies are not satisfied.");

    // Create scene nodes
    config.scene = new SceneNode();

    RenderStateNode* renderStateNode = new RenderStateNode();
    renderStateNode->EnableOption(RenderStateNode::LIGHTING);
    config.scene = renderStateNode;

    LevelMap* map = new LevelMap("./projects/MotivationSim/data/Levels/test.lvl");

   	IModelResourcePtr skyboxRes = ResourceManager<IModelResource>::Create("Skybox/Skybox.obj");
	skyboxRes->Load();
	TransformationNode* skyboxTrans = new TransformationNode();
	skyboxTrans->AddNode(skyboxRes->GetSceneNode());
	skyboxRes->Unload();

	//skyboxTrans->Scale(100.0, 100.0, 100.0);
    skyboxTrans->Scale(0.1, 0.1, 0.1);
    skyboxTrans->Move(map->GetWidth() / 2, map->GetHeight() / 2, 0.0);

	config.scene->AddNode(skyboxTrans);


    IModelResourcePtr groundRes = ResourceManager<IModelResource>::Create("Ground/Ground.obj");
    groundRes->Load();
    TransformationNode* groundTrans = new TransformationNode();
    groundTrans->AddNode(groundRes->GetSceneNode());
    groundTrans->Scale(0.1, 0.1, 0.1);
    groundTrans->Move(map->GetWidth() / 2, -0.1, map->GetWidth() / 2);
    groundRes->Unload();

    config.scene->AddNode(groundTrans);


    TransformationNode* mapTrans = new TransformationNode();
    RenderNode* mapRenderNode = map->GetRenderNode();
    mapTrans->AddNode(mapRenderNode);

    config.scene->AddNode(mapTrans);


	Vector<3,int> cursorPos(10, 0, 10);
	
    cursorTrans = new TransformationNode();
    cursorTrans->AddNode(new CursorRenderNode());
    cursorTrans->Move(0.0, 0.5, 0.0);
    config.scene->AddNode(cursorTrans);
    
    config.camera->SetPosition(cursorTrans->GetPosition() + Vector<3,float>(0.0, 20.0, 0.0));
    config.camera->LookAt(cursorTrans->GetPosition());
    config.camera->Follow(cursorTrans);  
    
    Motivation::Genes g;
    g.SetValue("speed", 0.5);
    g.SetValue("size", 0.5);
    g.SetValue("metabolism", 0.2);
    g.SetValue("life_expectancy", 0.8);
    g.SetValue("menstruation", 0.6);
    g.SetValue("menopause", 0.4);
    g.SetValue("metabolism", 0.8);
    g.SetValue("energy_capacity", 0.8);
    g.SetValue("nutrition_capacity", 0.8);
    g.SetValue("feeding_motivation", 0.5);
    g.SetValue("resting_motivation", 0.5);
    g.SetValue("breeding_motivation", 0.5);
    g.SetValue("litter_size", 0.5);
    g.SetValue("breeding_requirement", 0.5);
    g.SetValue("joins", 1.0);
    
    g.SetValue("color1", 1.0);
    g.SetValue("color2", 1.0);
    g.SetValue("color3", 1.0);
    g.SetValue("color4", 1.0);
    g.SetValue("color5", 1.0);
    g.SetValue("color6", 1.0);
    g.SetValue("color7", 1.0);
    g.SetValue("color8", 1.0);
    g.SetValue("color9", 1.0);
    AgentManager* agentManager = new AgentManager(&g);

    CursorHandler* cursor = new CursorHandler(map, config.camera, cursorTrans, cursorPos, agentManager);
    config.setup.GetKeyboard().KeyEvent().Attach(*cursor);


    DirectionalLightNode* light1 = new DirectionalLightNode();
    light1->ambient = Vector<4,float>(1.0, 1.0, 1.0, 1.0);
    light1->diffuse = Vector<4,float>(1.0, 1.0, 1.0, 1.0);
    config.scene->AddNode(light1);

    PointLightNode* light2 = new PointLightNode();
    light2->ambient = Vector<4,float>(1.0, 0.0, 1.0, 1.0);
    light2->diffuse = Vector<4,float>(0.0, 1.0, 1.0, 1.0);
    cursorTrans->AddNode(light2);

    IModelResourcePtr dollRes = ResourceManager<IModelResource>::Create("Ragdoll/Ragdoll.obj");
	dollRes->Load();
   

    
    int populationSize = 200;

    for (int i = 0; i < populationSize; i++) {
        bool goodPos = false;
        Vector<3,float> pos;
        while (!goodPos) {
            int rand1 = ((float)rand()/RAND_MAX) * map->GetWidth();
            int rand2 = ((float)rand()/RAND_MAX) * map->GetDepth();
            int rand3 = ((float)rand()/RAND_MAX) * map->GetHeight();
            if (map->GetTile(Vector<3,int>(rand1, rand2, rand3))->IsPassable()) {
                goodPos = true;
                pos     = Vector<3,float>((float)rand1, (float)rand2 + 0.5, (float)rand3);
            }
        }

        std::stringstream ss;
        ss << "Ant " << i;
        
        AntAgent* agent = new AntAgent(g.GetRandomGenes(), ss.str());
        agent->SetupScene(pos);
        

        /*
        ProbabilitySelector* randomSelector = new ProbabilitySelector();

        RandomSelector* feedSelector = new RandomSelector();
        feedSelector->AppendTask(new GoToAction(agent, map, map->FindPassableTileInArea(Vector<3,int>(15,0,8),2)));
        feedSelector->AppendTask(new GoToAction(agent, map, map->FindPassableTileInArea(Vector<3,int>(17,0,17),2)));
        feedSelector->AppendTask(new GoToAction(agent, map, map->FindPassableTileInArea(Vector<3,int>(3,0,3),2)));
        feedSelector->AppendTask(new GoToAction(agent, map, map->FindPassableTileInArea(Vector<3,int>(5,0,16),2)));
        feedSelector->AppendTask(new GoToAction(agent, map, map->FindPassableTileInArea( Vector<3,int>(17,0,3),2)));

        Sequence* restSequence = new Sequence();
        restSequence->AppendTask(new GoToAction(agent, map, map->FindPassableTileInArea(Vector<3,int>(23,0,16),2)));
        //restSequence->AppendTask(new PrintAction("Resting"));
        restSequence->AppendTask(new WaitAction(5));

        Sequence* breedSequence = new Sequence();
        breedSequence->AppendTask(new FindMateAction(agent, agentManager, map));
        //breedSequence->AppendTask(new GoToAction(agent, map, FindRandNeighbourPos(map, Vector<3,int>(13,0,10))));
        //breedSequence->AppendTask(new PrintAction("Breeding"));
        breedSequence->AppendTask(new WaitAction(2));

        randomSelector->AddTask(feedSelector,  (int)(agent->GetGenes()->GetValue("feeding_motivation")  * 100.0));
        randomSelector->AddTask(restSequence,  (int)(agent->GetGenes()->GetValue("resting_motivation")  * 100.0));
        randomSelector->AddTask(breedSequence, (int)(agent->GetGenes()->GetValue("breeding_motivation") * 100.0));
        */
        Task* task = new MotivationTask(agent, agentManager, map);

        agent->GetBehavior()->AppendTask(task);

        agentManager->AddAgent(agent);
    }

    config.scene->AddNode(agentManager->GetSceneNode());

    config.setup.GetEngine().ProcessEvent().Attach(*agentManager);
    
    /*
    PropertyList* plist = new PropertyList("data/meta.txt");
    
    int WINDOW_WIDTH  = 800;
    int WINDOW_HEIGHT = 600;
    
	AntTweakBarModule *tw = new AntTweakBarModule(WINDOW_WIDTH, WINDOW_HEIGHT);
    PlistBar* bar = new PlistBar(*plist);
    

    tw->AddBar(bar);
    tw->Initialize();

    RenderNode* rn = tw->RenderNode();
    config.scene->AddNode(rn);
    */
}



