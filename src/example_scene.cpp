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

//this forces the creation of more nodes
void generateTree(Node* node, int depth, int spread) {
	if (depth < 0) {
		return;
	}
	addChildNode(node, rand() % spread + node->GetDirection() - (spread/2), 20);

	if (rand() % 10 == 0) {
		addChildNode(node, rand() % spread + node->GetDirection() - (spread/2), 20);
	}

	for (auto& it : *node->GetChildren()) {
		generateTree(it, depth - 1, spread);
	}
}

//this finds the end points of the tree, ignoring types
void findLeaves(Node* root, std::list<Node*>* leafList) {
	if (root->GetChildren()->size() == 0) {
		leafList->push_back(root);
	}
	else {
		for (auto& it : *root->GetChildren()) {
			findLeaves(it, leafList);
		}
	}
}

//apply the given function to all nodes
void forEachNode(Node* root, std::function<int(Node*)> fn) {
	fn(root);
	for (auto& it : *root->GetChildren()) {
		forEachNode(it, fn);
	}
}

int countEachNode(Node* node) {
	int count = 0;
	forEachNode(node, [&count](Node* node) -> int {
		count++;
	});
	return count;
}

//auto-grow the tree, (customize for different species)
void growTree(Node* root, int depth, int spread) {
	//maximum plant size
	std::list<Node*> leafList;
	findLeaves(root, &leafList);

	if (leafList.size() >= 50 || countEachNode(root) >= 300) {
		return;
	}

	//set all non-flowers to stems
	forEachNode(root, [](Node* node) -> int {
		if (node->GetType() != Node::Type::FLOWER) {
			node->SetType(Node::Type::STEM);
		}
		return 0;
	});

	//grow all non-flower leaves
	for (auto& it : leafList) {
		if (it->GetType() != Node::Type::FLOWER) {
			generateTree(it, depth, spread);
		}
	}

	//grow some flowers
	for (auto& it : leafList) {
		if (it->GetType() == Node::Type::FLOWER) {
			continue;
		}

		if (rand() % 15 == 0) {
			Node* child = addChildNode(it, rand() % spread + it->GetDirection() - (spread/2), it->GetLength());
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

	//make sure there is at least one leaf type left
//	for (auto& it : leafList) {
//		if (it->GetType() == Node::Type::LEAF) {
//			return;
//		}
//	}

//	leafList.back()->SetType(Node::Type::LEAF);

	//NOTE: fix the graphics elsewhere
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

//	generateTree(rootNode, 20, 50);

	//put the pot under the plant
	potImage.SetTexture(textureLoader.Find("pot.png"));
	potX = rootNode->GetOrigin().x - (potImage.GetClipW() - rootNode->GetSprite()->GetClipW()) / 2;
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
	//
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
			growTree(rootNode, 0, 50);
			std::list<Node*> leafList;
			findLeaves(rootNode, &leafList);
			std::cout << "Leaf Count: " << leafList.size() << "\tNode Count: " << countEachNode(rootNode) << std::endl;
			CorrectSprites();
		}
		break;
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
	});
}