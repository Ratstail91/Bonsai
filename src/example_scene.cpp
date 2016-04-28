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

void generateTree(Node* node, int depth, int spread) {
	if (depth < 0) {
		return;
	}
	node = addChildNode(node, rand() % spread + node->GetDirection() - (spread/2), 20);

	generateTree(node, depth - 1, spread);

	if (rand() % 10 == 0) {
		generateTree(node, depth - 1, spread);
	}
}

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

void forEachNode(Node* root, std::function<int(Node*)> fn) {
	fn(root);
	for (auto& it : *root->GetChildren()) {
		forEachNode(it, fn);
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

	//setup the rootload
	rootNode = new Node();
	rootNode->GetSprite()->SetTexture(textureLoader.Find("stem.png"));
	rootNode->SetOrigin({400, 500});
	rootNode->SetDirection(270);

	generateTree(rootNode, 20, 50);

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
			Node* leafNode = rootNode;
			while(leafNode->GetChildren()->size() > 0) {
				leafNode = leafNode->GetChildren()->front();
			}
			addChildNode(leafNode, 360, 20);
		}
		break;
	}
}

void ExampleScene::KeyUp(SDL_KeyboardEvent const& event) {
	//
}
