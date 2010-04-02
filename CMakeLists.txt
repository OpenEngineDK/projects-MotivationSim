# Project name. Should be the same as the project directory name and
# will become the name of the binary target.
SET( PROJECT_NAME "MotivationSim")

# Project source code list
SET( PROJECT_SOURCES
  # Add all the cpp source files here
  main.cpp
  LevelMap.h
  micropather/micropather.h
  micropather/micropather.cpp
  Cursors.h
  Agent/Genes.h
  Agent/Genes.cpp
  Agent/Agent.h
  Agent/Agent.cpp
  Agent/AntAgent.h
  Agent/AntAgent.cpp
  Agent/AgentManager.h
  Agent/AgentManager.cpp
  Agent/AgentNode.h
  Agent/AntAgentNode.h
  Agent/AgentBehavior.h
  Agent/AgentBehavior.cpp
  #Plist/PlistBar.h
  #Plist/PlistBar.cpp
)

# Project executable
ADD_EXECUTABLE(${PROJECT_NAME}
  ${PROJECT_SOURCES}
)

# Include needed to use SDL under Mac OS X
IF(APPLE)
  SET(PROJECT_SOURCES ${PROJECT_SOURCES} ../../conf/mac/SDLMain.m)
ENDIF(APPLE)

# Project dependencies
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
  OpenEngine_Core
  OpenEngine_Logging
  OpenEngine_Display
  OpenEngine_Devices
  OpenEngine_Scene
  OpenEngine_Resources
  # Extensions
  Extensions_SetupHelpers
  Extensions_SDL
  Extensions_GenericHandlers
  Extensions_TGAResource
  Extensions_OBJResource
  Extensions_BehaviorTree
  Extensions_GenerelBehavior
  Extensions_OpenGLRenderer
  #Extensions_AntTweakBar
  #Extensions_PropertyList
)
