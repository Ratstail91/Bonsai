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
#include "node.hpp"

//-------------------------
//accessors & mutators
//-------------------------

Node::Type Node::SetType(Type t) {
	return type = t;
}

Node::Type Node::GetType() {
	return type;
}

int Node::SetDirection(int i) {
	return direction = i;
}

int Node::GetDirection() {
	return direction;
}

int Node::SetLength(int i) {
	return length = i;
}

int Node::GetLength() {
	return length;
}

Vector2 Node::SetOrigin(Vector2 v) {
	return origin = v;
}

Vector2 Node::GetOrigin() {
	return origin;
}

Image* Node::GetSprite() {
	return &sprite;
}

std::list<Node*>* Node::GetChildren() {
	return &children;
}

//-------------------------
//public functions
//-------------------------

Node* addChildNode(Node* parent, int direction, int length) {
	//make, push & setup
	Node* child = new Node();
	parent->GetChildren()->push_back(child);
	child->GetSprite()->SetTexture(parent->GetSprite()->GetTexture());
	child->SetDirection(direction);
	child->SetLength(length);

	//cos & sin return radians
	Vector2 unitVector;
	unitVector.x = cos(child->GetDirection() * M_PI / 180.0) * 180.0/M_PI;
	unitVector.y = sin(child->GetDirection() * M_PI / 180.0) * 180.0/M_PI;

	unitVector.Normalize();

	Vector2 v = parent->GetOrigin() + unitVector * child->GetLength();
	child->SetOrigin(v);

	return child;
}

void drawNodeTree(SDL_Renderer* renderer, Node* root) {
	if (root == nullptr) {
		return;
	}

	root->GetSprite()->DrawTo(renderer, root->GetOrigin().x, root->GetOrigin().y);

	for (auto& it : *root->GetChildren()) {
		drawNodeTree(renderer, it);
	}
}

void destroyTree(Node* root) {
	for (auto& it : *root->GetChildren()) {
		destroyTree(it);
	}

	root->GetChildren()->clear();
	delete root;
}

//this forces the creation of more nodes
void generateTree(Node* node, int depth, int spread, int sproutChance) {
	if (depth < 0) {
		return;
	}
	addChildNode(node, rand() % spread + node->GetDirection() - (spread/2), 10);

	if ((sproutChance == 0 || rand() % sproutChance == 0) && sproutChance != 99) {
		//wider spread for new shoots
		addChildNode(node, rand() % (spread*2) + node->GetDirection() - spread, 10);
	}

	for (auto& it : *node->GetChildren()) {
		generateTree(it, depth - 1, spread, sproutChance);
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
		return 0;
	});
	return count;
}


int findDeepestLeaf(Node* node) {
	if (node->GetChildren()->size() == 0) {
		return 1;
	}

	std::list<int> depthList;

	for (auto& it : *node->GetChildren()) {
		depthList.push_back(findDeepestLeaf(it));
	}

	int deepest = 0;

	for (auto& it : depthList) {
		if (it > deepest) {
			deepest = it;
		}
	}

	return deepest + 1;
}
