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

#include <cmath>

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
		delete it;
	}

	root->GetChildren()->clear();
	delete root;
}
