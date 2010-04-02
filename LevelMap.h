// Map
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS)
//
// This program is free software; It is covered by the GNU General
// Public License version 2 or any later version.
// See the GNU General Public License for more details (see LICENSE).
//--------------------------------------------------------------------

#ifndef _LEVEL_MAP_H_
#define _LEVEL_MAP_H_

#include <iostream>
#include <fstream>
using std::ifstream;

#include <Resources/File.h>

#include <Meta/OpenGL.h>
//#include <Logic/CompositeTaskDecorator.h>
// Game engine interface
#include <Scene/ISceneNode.h>
#include <Scene/TransformationNode.h>

// For the rendering of the properties of the dwarf
#include <Renderers/IRenderer.h>
#include <Scene/RenderNode.h>

#include <vector>
using std::vector;

#include "micropather/micropather.h"
using namespace micropather;

// Namespaces
using namespace OpenEngine::Core;
using namespace OpenEngine::Resources;
using namespace OpenEngine::Renderers;
using namespace OpenEngine::Scene;
//using namespace OpenEngine::OpenGL;

  class ITileRenderNode {
  public:
  	virtual void Render(IRenderer* renderer, float x, float y, float z) = 0;
  };

    class Tile : public ITileRenderNode {
    private:
        float food;

    public:
        Tile(float food = 0.0f) { SetFoodAmount(food); }
        virtual ~Tile() {}
        virtual bool IsPassable() = 0;

        float GetFoodAmount() { return food; }
        void SetFoodAmount(float f) { food = f; }
        bool HasFood() { return (food > 0.0f); }
    };

  class Stone : public Tile {
  public:
  	Stone() : Tile() {

  	}
  	virtual ~Stone() {}

  	virtual bool IsPassable() { return false; }

  	virtual void Render(IRenderer* renderer, float x, float y, float z) {
        glColor3f(0.5, 0.3, 0.1);
	   float tileSize = 0.85;
	   glBegin(GL_QUADS);
	   for (int i = 0; i < 2; i++) {
           // Draw the top and buttom sides
           glColor3f(0.5, 0.3, 0.1);
	       glVertex3f(x-tileSize/2, y+i*tileSize, z+tileSize/2);
	       glNormal3f(0.0, 1.0, 0.0);
	       glVertex3f(x+tileSize/2, y+i*tileSize, z+tileSize/2);
	       glNormal3f(0.0, 1.0, 0.0);
	       glVertex3f(x+tileSize/2, y+i*tileSize, z-tileSize/2);
	       glNormal3f(0.0, 1.0, 0.0);
	       glVertex3f(x-tileSize/2, y+i*tileSize, z-tileSize/2);
	       glNormal3f(0.0, 1.0, 0.0);

           // Draw the left and right sides
           glColor3f(0.7, 0.3, 0.1);
	       glVertex3f(x+i*tileSize-tileSize/2, y,   z+tileSize/2);
	       glVertex3f(x+i*tileSize-tileSize/2, y,   z-tileSize+tileSize/2);
           glColor3f(0.5, 0.3, 0.1);
	       glVertex3f(x+i*tileSize-tileSize/2, y+tileSize, z-tileSize+tileSize/2);
	       glVertex3f(x+i*tileSize-tileSize/2, y+tileSize, z+tileSize/2);

           // Draw the front and back sides
           glColor3f(0.7, 0.3, 0.1);
	       glVertex3f(x - tileSize/2, y,   z-i*tileSize+tileSize/2);
	       glVertex3f(x+tileSize-tileSize/2, y,   z-i*tileSize+tileSize/2);
           glColor3f(0.5, 0.3, 0.1);
	       glVertex3f(x+tileSize-tileSize/2, y+tileSize, z-i*tileSize+tileSize/2);
	       glVertex3f(x - tileSize/2, y+tileSize, z-i*tileSize+tileSize/2);
	   }
	   glEnd();
  	}
  };

  class Grass : public Tile {
      protected:
          Vector<3,float> color;

      public:
          Grass() : Tile( ((((float)rand()/RAND_MAX) < 0.5) ? ((float)rand()/RAND_MAX) : 0.0) ) {
              color = Vector<3,float>(((float)rand()/RAND_MAX) * 0.05, 0.3 + ((float)rand()/RAND_MAX) * 0.05, ((float)rand()/RAND_MAX) * 0.05);
          }
          virtual ~Grass() {}

          virtual bool IsPassable() { return true; }

          virtual void Render(IRenderer* renderer, float x, float y, float z) {
              glColor3f(color[0], color[1], color[2]);
              float tileSize = 1.0;
              glBegin(GL_QUADS);
                glVertex3f(x-tileSize/2, y, z+tileSize/2);
                glNormal3f(0.0, 1.0, 0.0);
                glVertex3f(x+tileSize/2, y, z+tileSize/2);
                glNormal3f(0.0, 1.0, 0.0);
                glVertex3f(x+tileSize/2, y, z-tileSize/2);
                glNormal3f(0.0, 1.0, 0.0);
                glVertex3f(x-tileSize/2, y, z-tileSize/2);
                glNormal3f(0.0, 1.0, 0.0);
              glEnd();

			if (!HasFood()) return;

            glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			float alpha = 0.5;

              // Draw grass
              glColor4f(0.0, 0.6, 0.0, alpha);
              glLineWidth(4.0);
              glBegin(GL_LINES);
              int foodValue = (int)(GetFoodAmount() * 10);
              for (int zpos = 0; zpos < foodValue; zpos++) {
                  for (int xpos = 0; xpos < foodValue; xpos++) {
                      float xposi = x-tileSize/2 + (float)xpos / foodValue * tileSize;
                      float zposi = z-tileSize/2 + (float)zpos / foodValue * tileSize;
                      glVertex3f(xposi, y, zposi);
                      glVertex3f(xposi, y + 0.4 * GetFoodAmount(), zposi);
                  }
              }
              glEnd();
              glLineWidth(1.0);

              glColor4f(0.0, 0.3, 0.0, alpha);
              glPointSize(6.0f);
              glBegin(GL_POINTS);
              //int foodValue = (int)(GetFoodAmount() * 10);
              for (int zpos = 0; zpos < foodValue; zpos++) {
                  for (int xpos = 0; xpos < foodValue; xpos++) {
                      float xposi = x-tileSize/2 + (float)xpos / foodValue * tileSize;
                      float zposi = z-tileSize/2 + (float)zpos / foodValue * tileSize;
                      glVertex3f(xposi, y + 0.4 * GetFoodAmount(), zposi);
                  }
              }
              glEnd();
              glPointSize(1.0f);
              
              glDisable(GL_BLEND);

            /*
            if (GetFoodAmount() > 0.0) {
                // Draw food
                glColor3f(0.5, 0.0, 0.2);
                glPointSize(GetFoodAmount()*2.0);
                glBegin(GL_POINTS);
                    glVertex3f(x, y, z);
                glEnd();
                glPointSize(1.0f);
            }
            */
          }
  };

  class Floor : public Tile {
  public:
  	Floor() : Tile() {

  	}
  	virtual ~Floor() {}

  	virtual bool IsPassable() { return true; }

  	virtual void Render(IRenderer* renderer, float x, float y, float z) {
		glColor3f(0.1, 0.1, 0.1);
		float tileSize = 1.0;
		glBegin(GL_QUADS);
			glVertex3f(x-tileSize/2, y, z+tileSize/2);
			glNormal3f(0.0, 1.0, 0.0);
			glVertex3f(x+tileSize/2, y, z+tileSize/2);
			glNormal3f(0.0, 1.0, 0.0);
			glVertex3f(x+tileSize/2, y, z-tileSize/2);
			glNormal3f(0.0, 1.0, 0.0);
			glVertex3f(x-tileSize/2, y, z-tileSize/2);
			glNormal3f(0.0, 1.0, 0.0);
		glEnd();

            /*
	   Vector<3,float> position = Vector<3,float>(x, z, y);
	   Vector<3,float> color = Vector<3,float>(0.2, 0.1, 0.6);
	   float width = 5;
	   renderer->DrawPoint(position, color, width);
            */
  	}
  };
  
  class Bed : public Grass {
  public:
  	Bed() : Grass() {
	  	SetFoodAmount(0.0);
	  	color = Vector<3,float>(((float)rand()/RAND_MAX) * 0.08, 0.2 + ((float)rand()/RAND_MAX) * 0.08, 0.5 + ((float)rand()/RAND_MAX) * 0.08);  	
  	}
  };

  struct TileInfo {
  	Vector<3,int> position;
  };

  class LevelMap : public Graph {
  private:
  	Tile** tiles;
  	int height;
  	int width;
    int depth;
    int activeDepth;
  	RenderNode* renderNode;
  	MicroPather* pather;
    //vector<Tile*> passableTiles;
    vector< Vector<3,int> > bedTiles;

  	public:
 	vector<TileInfo> selectedTiles;
    std::vector< Vector<3,int> > path;

  	private:

	class LevelMapRenderNode : public RenderNode {
	private:
		LevelMap* level;

	public:
		LevelMapRenderNode(LevelMap* level) : level(level) {}

		virtual void Apply(IRenderingView* renderingView) {
		    IRenderer* renderer = renderingView->GetRenderer();

		    int height = level->GetHeight();
		    int width  = level->GetWidth();
            int depth  = level->GetDepth();

		    glDisable(GL_LIGHTING);
			glColor3f(0.3, 0.3, 0.3);
			float tileSize = 1.0;
			float mapWidth = width * tileSize;
			float mapHeight = height * tileSize;
			float mapStartX = -0.5 * tileSize;
			float mapEndX   = mapWidth - 0.5 * tileSize;
			float mapStartZ = -0.5 * tileSize;
			float mapEndZ   = mapHeight - 0.5 * tileSize;


			// Render the individual tiles
			int activeDepth = level->GetActiveDepth();
            //for (int y = 0; y < depth; y++) {
			for (int z = 0; z < height; z++) {
			  for (int x = 0; x < width; x++) {
			    Tile* tile = level->GetTile(x,activeDepth,z);
			    if (activeDepth == depth - 1 || !(tile->IsPassable() && level->GetTile(x,activeDepth+1,z)->IsPassable())) {
			      tile->Render(renderer, x * tileSize, -activeDepth * tileSize, z * tileSize);
			    }
			  }
			}
            //}


			glLineWidth(1.0);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glBegin(GL_LINES);

            // Draw the map 2D grid
            int h = activeDepth;
            glColor4f(0.1, 0.3, 0.1, 1.0);

            for (int z = 0; z < height+1; z++) {
                glVertex3f(mapStartX, 0.01 - h * tileSize, mapStartZ + z * tileSize);
                glVertex3f(mapEndX,   0.01 - h * tileSize, mapStartZ + z * tileSize);
            }
            for (int x = 0; x < width+1; x++) {
                glVertex3f(mapStartX + x * tileSize, 0.01 - h * tileSize, mapStartZ);
                glVertex3f(mapStartX + x * tileSize, 0.01 - h * tileSize, mapEndZ);
            }

            /*
            // Draw the map 3D grid
            for (int h = activeDepth - 1; h <= activeDepth; h++) {
                if (h == activeDepth) {
                    glColor4f(0.1, 0.1, 0.3, 1.0);
                } else {
                    glColor4f(0.1, 0.1, 0.3, 0.2);
                }

				for (int z = 0; z < height+1; z++) {
		            glVertex3f(mapStartX, 0.01 - h * tileSize, mapStartZ + z * tileSize);
		            glVertex3f(mapEndX,   0.01 - h * tileSize, mapStartZ + z * tileSize);
				}
				for (int x = 0; x < width+1; x++) {
                    glVertex3f(mapStartX + x * tileSize, 0.01 - h * tileSize, mapStartZ);
                    glVertex3f(mapStartX + x * tileSize, 0.01 - h * tileSize, mapEndZ);
				}
            }

            glColor4f(0.1, 0.1, 0.3, 0.2);
            for (int z = 0; z < height+1; z++) {
                for (int x = 0; x < width+1; x++) {
                    glVertex3f(mapStartX + x * tileSize, -activeDepth * tileSize, mapStartZ + z * tileSize);
                    glVertex3f(mapStartX + x * tileSize, 1-activeDepth * tileSize, mapStartZ + z * tileSize);
                }
            }
            */
			glEnd();
            glDisable(GL_BLEND);
			glLineWidth(1.0);


			vector<TileInfo>::iterator selectedIter;
			for (selectedIter = level->selectedTiles.begin(); selectedIter != level->selectedTiles.end(); selectedIter++) {
				TileInfo tileInfo = *(selectedIter);
				Vector<3,int> selectedTile = tileInfo.position;
                renderer->DrawPoint(Vector<3,float>(selectedTile[0] * tileSize, -selectedTile[1] * tileSize, selectedTile[2] * tileSize), Vector<3,float>(0.0, 0.0, 1.0), 10);
			}

            glLineWidth(2.0);
            glColor3f(0.0, 1.0, 0.0);
            glBegin(GL_LINE_STRIP);
            //int x;
            //int y;
            //int z;
            vector< Vector<3,int> >::iterator pathIter;
            for (pathIter = level->path.begin(); pathIter != level->path.end(); pathIter++) {
                Vector<3,int> node = *(pathIter);
                //level->NodeToXYZ(node, &x, &y, &z);
                glVertex3f(node[0], 0.02-node[1], node[2]);
            }
            glEnd();
            glLineWidth(1.0);

			glEnable(GL_LIGHTING);
		}
    };

  public:
	LevelMap(int width, int height, int depth = 1) : height(height), width(width), depth(depth) {
		tiles = new Tile*[height*width*depth];
		for (int y = 0; y < depth; y++) {
		    for (int z = 0; z < height; z++) {
		        for (int x = 0; x < width; x++) {
		            Tile* tile = NULL;
		            if (z == 0) {
		                tile = new Grass();
		            } else {
		                tile = new Stone();
		            }
		            SetTile(x, y, z, tile);
		        }
		    }
		}
		PostInit();
	}

        LevelMap(string file) {
            printf("Loading map file: %s\n", file.c_str());

             // open the map file
            ifstream* in = File::Open(file);

            // set up working variables
            char buffer[255];
            bool loadingMap = false;
            int widthCounter = -1;
            int heightCounter = -1;
            int depthCounter = -1;
            int line = 0;

            // for each line in the map file...
            while (!in->eof()) {
                line++;
                in->getline(buffer,255);

                // ignored stuff
                if (in->gcount() <= 2 || // short line
                    buffer[0] == ' ') continue; // empty lines

                if (string(buffer,5) == "size:") {
                    if (sscanf(buffer, "size: %i x %i x %i", &width, &height, &depth) != 3) {
                        printf("Invalid size declaration in line %i\n", line);
                    } else {
                        tiles = new Tile*[height*width*depth];
                    }
                } else if (string(buffer,6) == "depth:") {
                    depthCounter++;
                    loadingMap = true;
                    int tempDepth;
                    if (sscanf(buffer, "depth: %i", &tempDepth) != 1) {
                        printf("Invalid depth declarationin line %i\n", line);
                    } else {
                        if (tempDepth != depthCounter) {
                            printf("%i is an invalid depth index in this context - expected %i (line %i)!\n", tempDepth, depthCounter, line);
                        }
                    }
                } else if (loadingMap) {
                    char tmp[255];
                    if (sscanf(buffer, "%s", tmp) != 1) {
                        printf("Invalid map declaration in line %i\n", line);
                    } else {
                        widthCounter = 0;
                        heightCounter++;
                        Tile* tile;
                        while (tmp[widthCounter] != 0 || tmp[widthCounter] == ' ' || tmp[widthCounter] == '\n') {
                            switch (tmp[widthCounter]) {
                                case '"': tile = new Grass(); break;
                                case '|': tile = new Stone(); break;
                                case '_': tile = new Floor(); break;
                               	case '#': tile = new Bed();   bedTiles.push_back( Vector<3,int>(widthCounter, depthCounter, heightCounter) ); break;
                                default: tile = NULL; break;
                            }
                            SetTile(widthCounter, depthCounter, heightCounter, tile);
                            widthCounter++;
                        }
                        printf("\n");
                        if (heightCounter == height - 1) {
                            heightCounter = -1;
                            loadingMap = false;
                        }
                    }
                }
            }

            // close the file
            in->close();

            PostInit();
        }

	virtual ~LevelMap() {
		delete pather;
	}
	
	Vector<3,int> GetBedTile() {
		int index = ((float)rand()/RAND_MAX) * (bedTiles.size() - 1);
		return bedTiles[index];
	}

    void PostInit() {
        activeDepth = 0;
        renderNode = new LevelMapRenderNode(this);
        pather = new MicroPather( this, height*width );
    }

    virtual Tile* GetTile(Vector<3,int> vec) {
        return GetTile(vec[0], vec[1], vec[2]);
    }

	virtual Tile* GetTile(int x, int y, int z) {
        if (x < 0 || y < 0 || z < 0 || x >= width || y >= depth || z >= height) return NULL;
		return tiles[y*height*width + z*width + x];
	}

	virtual void SetTile(int x, int y, int z, Tile* tile) {
	   tiles[y*height*width + z*width + x] = tile;
	}
	
	virtual bool IsValidTile(Vector<3,int> pos) {
		return (GetTile(pos) != NULL);
	}

    virtual void ChangeTile(int x, int y, int z, Tile* tile) {
        Tile* oldTile = GetTile(x,y,z);
        SetTile(x,y,z,tile);
        if (tile->IsPassable() != oldTile->IsPassable()) {
            /*
            if (tile->IsPassable()) {
                passableTiles.push(tile)
            } else {
                passableTiles
            }
            */
            pather->Reset();
        }
    }

    virtual Vector<3,float> GetPosition(Vector<3,int> pos) {
        float tileSize = 1.0;
        float mapStartX = 0.0; //-0.5 * tileSize;
        float mapStartZ = 0.0; //-0.5 * tileSize;
        return Vector<3,float>(mapStartX + pos[0] * tileSize, -pos[1] * tileSize, mapStartZ + pos[2] * tileSize);
        }

	virtual void SelectTile(int x, int y, int z) {
		TileInfo tileInfo;
		tileInfo.position = Vector<3,int>(x,y,z);
		selectedTiles.push_back(tileInfo);

		if (selectedTiles.size() > 1) {
            path.clear();
			float totalCost;

			TileInfo startState = selectedTiles[selectedTiles.size()-2];
			TileInfo endState = selectedTiles[selectedTiles.size()-1];

            int result = GetPath(startState.position, endState.position, &path, &totalCost);

            printf("Result: %s.\n",
                   (result == MicroPather::SOLVED ? "Solved" :
                   (result == MicroPather::NO_SOLUTION ? "No solution" :
                   "Same start and end")));
			printf("Total cost: %f\n", totalCost);
            printf("Nodes in path: %i\n", path.size());

            selectedTiles.clear();
		}
	}

    virtual int GetPath(Vector<3,int> start, Vector<3,int> end, std::vector< Vector<3,int> >* path, float* totalCost) {
        bool invalidPath = false;
        //printf("Started a path from %i, %i, %i to %i, %i, %i\n", start[0], start[1], start[2], end[0], end[1], end[2]);
        if (!IsValidTile(start) || !GetTile(start)->IsPassable()) {
            printf("[LevelMap::GetPath] Starting tile %i, %i, %i is not passable!\n", start[0], start[1], start[2]);
            start = FindPassableTileInArea(start, 1);
            //invalidPath = true;
        }
        if (!IsValidTile(end) || !GetTile(end)->IsPassable()) {
            //printf("[LevelMap::GetPath] Destination %i, %i, %i is not passable!\n", end[0], end[1], end[2]);
            invalidPath = true;
        }
        
        if (start == end) {
        	invalidPath = true;
        }

        if (invalidPath) {
            *path = std::vector< Vector<3,int> >();
            *totalCost = 0;
            return MicroPather::NO_SOLUTION;
        }

        pather->Reset(); // HACK HACK HACK HACK HACK HACK HACK HACK

        std::vector< void* > thePath;
        int result = pather->Solve(PositionToNode(start), PositionToNode(end), &thePath, totalCost);

        std::vector< void* >::iterator pathIter;
        for (pathIter = thePath.begin(); pathIter != thePath.end(); pathIter++) {
            Vector<3,int> node = NodeToPosition(*pathIter);
            path->push_back(node);
        }

        return result;
    }

	virtual void PrintMap() {
		printf("Map:\n");
        for (int y = 0; y < depth; y++) {
            printf("Depth %i\n", y);
            for (int z = 0; z < height; z++) {
                for (int x = 0; x < width; x++) {
                	printf("%s", (GetTile(x,y,z)->IsPassable() ? " " : "X"));
                }
                printf("\n");
            }
            printf("\n");
        }
	}

	int GetHeight() {
		return height;
	}

	int GetWidth() {
		return width;
	}

    int GetDepth() {
        return depth;
    }

    void SetActiveDepth(int activeDepth) {
        this->activeDepth = activeDepth;
    }

    int GetActiveDepth() {
        return activeDepth;
    }

	virtual RenderNode* GetRenderNode() {
  		return renderNode;
  	}

    void* PositionToNode(Vector<3,int> pos) {
        return XYZToNode(pos[0], pos[1], pos[2]);
    }

    Vector<3,int> NodeToPosition(void* node) {
       Vector<3,int> temp;
       NodeToXYZ(node, &temp[0], &temp[1], &temp[2]);
       return temp;
    }

    void NodeToXYZ( void* node, int* x, int* y, int* z ) {
        int index = (int)node;
        int tilesPrY = width*height;
        int tilesPrZ = width;

        *y = index / tilesPrY;
        *z = (index - *y * tilesPrY) / tilesPrZ;
        *x = index - (*y * tilesPrY + *z * tilesPrZ);
    }

    void* XYZToNode( int x, int y, int z ) {
        return (void*) ( y*height*width + z*width + x );
    }


    virtual float LeastCostEstimate( void* nodeStart, void* nodeEnd ) {
        Vector<3,int> start = NodeToPosition(nodeStart);
        Vector<3,int> end   = NodeToPosition(nodeEnd);
        Vector<3,int> diff  = start - end;

        // Compute the minimum path cost using distance measurement. It is possible
        // to compute the exact minimum path using the fact that you can move only
        // on a straight line or on a diagonal, and this will yield a better result.
        return diff.GetLength();
    }

    virtual void AdjacentCost( void* node, std::vector< StateCost > *neighbors ) {
        int x, y, z;

        const int dx[10] = { 1, 1, 0, -1, -1, -1,  0,  1,  0, 0 };
        const int dz[10] = { 0, 1, 1,  1,  0, -1, -1, -1,  0, 0 };
        const int dy[10] = { 0, 0, 0,  0,  0,  0,  0,  0, -1, 1};
        const float cost[10] = { 1.0f, 1.41f, 1.0f, 1.41f, 1.0f, 1.41f, 1.0f, 1.41f, 2.0f, 2.0f };

        NodeToXYZ( node, &x, &y, &z );

        for (int i = 0; i < 10; ++i ) {
            int nx = x + dx[i];
            int ny = y + dy[i];
            int nz = z + dz[i];

            // the neighbor is outside the map
            if (nx < 0 || ny < 0 || nz < 0 || nx >= width || nz >= height || ny >= depth) {
                StateCost nodeCost = { XYZToNode( nx, ny, nz ), FLT_MAX };
                neighbors->push_back( nodeCost );
                continue;
            }

            Tile* currentTile = GetTile(nx, ny, nz);
            bool passable = currentTile->IsPassable();
            if (passable) {
                // if we don't go directional or if we're out in the open, it okay to go directional
                if ((nx == x || nz == z) || (GetTile(nx, ny, z)->IsPassable() && GetTile(x, ny, nz)->IsPassable())) {
                    StateCost nodeCost = { XYZToNode( nx, ny, nz ), cost[i] };
                    neighbors->push_back( nodeCost );
                } else { // we cannot squeeze through two inpassable blocks
                    StateCost nodeCost = { XYZToNode( nx, ny, nz ), FLT_MAX };
                    neighbors->push_back( nodeCost );
                }
            } else {
                StateCost nodeCost = { XYZToNode( nx, ny, nz ), FLT_MAX };
                neighbors->push_back( nodeCost );
            }
        }
    }

    virtual void PrintStateInfo( void* node ) {
        int x, y, z;
        NodeToXYZ( node, &x, &y, &z );
        printf( "(%d,%d,%d)", x, y, z );
    }

    virtual Vector<3,int> FindPassableTileInArea(Vector<3,int> pos, int radius) {
        int tries = 0;
        int maxTries = (radius * 2 + 1)^2;
        while (tries < maxTries) {
            tries++;
            int rand1 = pos[0] - radius + ((float)rand()/RAND_MAX) * (radius * 2 + 1);
            int rand2 = pos[1] - radius + ((float)rand()/RAND_MAX) * (radius * 2 + 1);
            int rand3 = pos[2] - radius + ((float)rand()/RAND_MAX) * (radius * 2 + 1);
            Vector<3,int> foundPos = Vector<3,int>(rand1, rand2, rand3);

            Tile* tile = GetTile(foundPos);
            if (tile == NULL) continue;
            if (tile->IsPassable()) {
                return foundPos;
            }
        }
        return pos;
    }

    virtual Vector<3,int> FindPassableTile() {
        return FindPassableTileInArea(Vector<3,int>(width/2,0,height/2), min(width/2,height/2)-1);
    }

  };

#endif // _LEVEL_MAP_H_
