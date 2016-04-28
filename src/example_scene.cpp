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
#include "example_scene.hpp"

//-------------------------
//tree management
//-------------------------

//auto-grow the tree, (customize for different species)
void growCherryBlossom(Node* root) {
	//defaults
	int depth = 0;
	int spread = 50;
	int sproutChance = 10;

	//maximum plant size
	std::list<Node*> leafList;
	findLeaves(root, &leafList);

//	if (findDeepestLeaf(root) >= 60) {
	if (leafList.size() >= 800) {
		return;
	}

	//set all non-flowers to stems
	forEachNode(root, [](Node* node) -> int {
		if (node->GetType() != Node::Type::FLOWER) {
			node->SetType(Node::Type::STEM);
			return 0;
		}
		return 0;
	});

	//shape the trunk
	int deepestLeaf = findDeepestLeaf(root);
	if (deepestLeaf < 6) {
		spread = 20;
		sproutChance = 99;
	}
	if (deepestLeaf == 6) {
		sproutChance = 0;
	}

	//grow all non-flower leaves
	for (auto& it : leafList) {
		if (it->GetType() != Node::Type::FLOWER) {
			generateTree(it, depth, spread, sproutChance);
		}
	}

	//grow some flowers
	for (auto& it : leafList) {
		//trunk
		if (deepestLeaf < 10) {
			break;
		}

		if (it->GetType() == Node::Type::FLOWER) {
			continue;
		}

		if (rand() % 10 == 0) {
			Node* child = addChildNode(it, rand() % (spread*2) + it->GetDirection() - spread, it->GetLength());
			child->SetType(Node::Type::FLOWER);
		}
	}

	//re-mark all non-flower leaves
	leafList.clear();
	findLeaves(root, &leafList);
	for (auto& it : leafList) {
		if (it->GetType() != Node::Type::FLOWER) {
			it->SetType(Node::Type::LEAF);
		}
	}
}

//-------------------------
//Scene
//-------------------------

ExampleScene::ExampleScene() {
	//setup
	srand(time(nullptr));
	textureLoader.Load(GetRenderer(), "rsc/", "pot.png");
	textureLoader.Load(GetRenderer(), "rsc/", "stem.png");
	textureLoader.Load(GetRenderer(), "rsc/", "leaf.png");
	textureLoader.Load(GetRenderer(), "rsc/", "flower.png");

	//setup the rootload
	rootNode = new Node();
	rootNode->GetSprite()->SetTexture(textureLoader.Find("stem.png"));
	rootNode->SetOrigin({400, 500});
	rootNode->SetDirection(270);

	//put the pot under the plant
	potImage.SetTexture(textureLoader.Find("pot.png"));
	potX = rootNode->GetOrigin().x - potImage.GetClipW() / 2;
	potY = rootNode->GetOrigin().y;

	std::list<Node*> leafList;

	findLeaves(rootNode, &leafList);

	std::cout << "Leaves: " << leafList.size() << std::endl;

	for (auto& it : leafList) {
		it->GetSprite()->SetTexture(textureLoader.Find("leaf.png"));
	}
}

ExampleScene::~ExampleScene() {
	destroyTree(rootNode);
}

//-------------------------
//frame phases
//-------------------------

void ExampleScene::FrameStart() {
	//
}

void ExampleScene::Update() {
	//
}

void ExampleScene::FrameEnd() {
	//
}

void ExampleScene::RenderFrame(SDL_Renderer* renderer) {
	drawNodeTree(renderer, rootNode);
	potImage.DrawTo(renderer, potX, potY);
}

//-------------------------
//input events
//-------------------------

void ExampleScene::MouseMotion(SDL_MouseMotionEvent const& event) {
	//
}

void ExampleScene::MouseButtonDown(SDL_MouseButtonEvent const& event) {
	switch(event.button) {
		case SDL_BUTTON_LEFT: {
			//find the selected node
			Node* selected = nullptr;
			Vector2 mouse(event.x, event.y);
			forEachNode(rootNode, [&](Node* node) -> int {
				if ((mouse - node->GetOrigin()).Length() <= 8) {
					selected = node;
				}
				return 0;
			});

			//delete the selected node & it's children
			if (selected != nullptr && selected != rootNode) {
				destroyTree(selected);
			}

			//BUGFIX: find and fix the invalidated pointer
			forEachNode(rootNode, [&](Node* node) -> int {
				for (std::list<Node*>::iterator it = node->GetChildren()->begin(); it != node->GetChildren()->end(); it++) {
					if (*it == selected) {
						node->GetChildren()->erase(it);
						return 0;
					}
				}
				return 0;
			});
		}
		break;
	}
}

void ExampleScene::MouseButtonUp(SDL_MouseButtonEvent const& event) {
	//
}

void ExampleScene::MouseWheel(SDL_MouseWheelEvent const& event) {
	//
}

void ExampleScene::KeyDown(SDL_KeyboardEvent const& event) {
	//preference as a default
	switch(event.keysym.sym) {
		case SDLK_ESCAPE:
			QuitEvent();
		break;

		case SDLK_SPACE: {
			growCherryBlossom(rootNode);
			std::list<Node*> leafList;
			findLeaves(rootNode, &leafList);
			std::cout << "Leaves: " << leafList.size() << "\tTotal Nodes: " << countEachNode(rootNode) << std::endl;
			CorrectSprites();
		}
		break;

		case SDLK_TAB:
			for (auto& it : *rootNode->GetChildren()) {
				destroyTree(it);
			}
			rootNode->GetChildren()->clear();
			CorrectSprites();
	}
}

void ExampleScene::KeyUp(SDL_KeyboardEvent const& event) {
	//
}

void ExampleScene::CorrectSprites() {
	forEachNode(rootNode, [&](Node* node) -> int {
		switch(node->GetType()) {
			case Node::Type::LEAF:
				node->GetSprite()->SetTexture(textureLoader.Find("leaf.png"));
			break;

			case Node::Type::STEM:
				node->GetSprite()->SetTexture(textureLoader.Find("stem.png"));
			break;

			case Node::Type::FLOWER:
				node->GetSprite()->SetTexture(textureLoader.Find("flower.png"));
			break;
		}
		return 0;
	});
}