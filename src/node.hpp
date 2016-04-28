/* Copyright: (c) Kayne Ruse 2013-2016
 * 
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * 
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 * 
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 * 
 * 3. This notice may not be removed or altered from any source
 * distribution.
*/
#pragma once

#include "image.hpp"
#include "vector2.hpp"

#include "SDL2/SDL.h"

#include <cmath>
#include <functional>
#include <list>

class Node {
public:
	enum Type {
		LEAF,
		STEM,
		FLOWER
	};

	Node() = default;
	~Node() = default;

	//accessors & mutators
	Type SetType(Type t);
	Type GetType();
	int SetDirection(int i);
	int GetDirection();
	int SetLength(int i);
	int GetLength();

	Vector2 SetOrigin(Vector2 v);
	Vector2 GetOrigin();

	Image* GetSprite();
	std::list<Node*>* GetChildren();

private:
	Type type = Type::LEAF;
	//right = 0, down = 90, left = 180, up = 270
	int direction = 0;
	int length = 0;
	Vector2 origin; //cached position for drawing
	Image sprite;
	std::list<Node*> children;
};

//public functions
Node* addChildNode(Node* parent, int direction, int length);
void drawNodeTree(SDL_Renderer*, Node* root);
void destroyTree(Node* root);

void generateTree(Node* node, int depth, int spread, int sproutChance);
void findLeaves(Node* root, std::list<Node*>* leafList);
void forEachNode(Node* root, std::function<int(Node*)> fn);
int countEachNode(Node* node);
int findDeepestLeaf(Node* node);
