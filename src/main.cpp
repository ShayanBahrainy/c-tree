#include <vector>
#include <string>
#include <cstdlib>
#include <random>
#include <algorithm>
#include <iostream>

#include "raylib.h"
#include "resource_dir.h"	// utility header for SearchAndSetResourceDir
#include "Node.h"
#include "Button.h"

enum Status {NONE, MAX, MIN, NEWTREE};

Status status = NONE;
Node* selectedNode = nullptr;
Node* root;

void traverseAndDraw(Node* node, int xOffset, int yOffset, int startingSpread=240) {
	if (selectedNode == node) {
		DrawCircle(xOffset, yOffset, 20, PURPLE);
	}
	DrawCircle(xOffset, yOffset, 15, WHITE);
	DrawText(std::to_string(node->val).c_str(), xOffset - 5, yOffset - 5, 5, BLACK);
	if (node->left != nullptr) {
		DrawLine(xOffset, yOffset, xOffset - startingSpread, yOffset + 60, BLUE);
		traverseAndDraw(node->left, xOffset - startingSpread, yOffset + 60, startingSpread/2);
	}
	if (node->right != nullptr) {
		DrawLine(xOffset, yOffset, xOffset + startingSpread, yOffset + 60, RED);
		traverseAndDraw(node->right, xOffset + startingSpread, yOffset + 60, startingSpread/2);
	}
}

Node* generateTree(int count, std::mt19937 &gen, std::uniform_int_distribution<> &dist) {
	if (count == -1) return nullptr;

	Node* node = new Node;

	node->left = generateTree(count - 1, gen, dist);

	node->val = dist(gen);

	node->right = generateTree(count - 1, gen, dist);

	return node;
}

void placeValue(int val, Node* node) {
	Node* prev;
	Node* curr = node;

	while (curr != nullptr) {
		prev = curr;
		if (val < curr->val) {
			curr = curr->left;
		}
		else {
			curr = curr->right;
		}
	}

	if (val < prev->val) {
		prev->left = new Node{val};
	}
	else {
		prev->right = new Node{val};
	}
}

void deleteTree(Node* node) {
	if (node->left != nullptr) deleteTree(node->left);
	if (node->right != nullptr) deleteTree(node->right);

	delete node;
}

void findMin() {
	std::cout << "Finding the minimum..." << std::endl;
	status = MIN;
	selectedNode = nullptr;
}

void findMax() {
	std::cout << "Finding the maximum.." << std::endl;
	status = MAX;
	selectedNode = nullptr;
}

void reset() {
	std::cout << "Resetting..." << std::endl;
	status = NONE;
	selectedNode = nullptr;
}

void newTree() {
	std::cout << "Creating new tree..." << std::endl;
	status = NEWTREE;
	selectedNode = nullptr;
}

bool isOver(int x, int y, int xwidth, int ywidth) {
	return GetMouseX() > x && GetMouseX() < x + xwidth && GetMouseY() > y && GetMouseY() < y + ywidth;
}

int main () {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 500);

	const int HEIGHT = 10 * 100;
	const int WIDTH = 16 * 100;

	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	InitWindow(WIDTH, HEIGHT, "Is that my christmas tree?");

	SearchAndSetResourceDir("resources");

	std::vector<int> nums;

	for (int i = 0; i < 50; ++i) {
		nums.push_back(dist(gen));
	}

	std::sort(nums.begin(), nums.end());

	int median = nums.at(nums.size() / 2);

	root = new Node{median};

	nums.erase(nums.begin() + nums.size()/2);

	std::vector<Button> buttons;

	Button minButton = Button{(int)(WIDTH*0.8), (int)(HEIGHT*0.8), (int)(0.2 * WIDTH), (int)(0.1 * HEIGHT), "Minimum", *findMin};
	Button maxButton = Button{(int)(WIDTH*0.8), (int)(HEIGHT*0.7), (int)(0.2 * WIDTH), (int)(0.1 * HEIGHT), "Maximum", *findMax};
	Button resetButton = Button{(int)(WIDTH*0.8), (int)(HEIGHT*0.6), (int)(0.2 * WIDTH), (int)(0.1 * HEIGHT), "Reset", *reset};
	Button newTreeButton = Button{(int)(WIDTH*0.8), (int)(HEIGHT*0.5), (int)(0.2 * WIDTH), (int)(0.1 * HEIGHT), "New Tree", *newTree};

	buttons.push_back(minButton);
	buttons.push_back(maxButton);
	buttons.push_back(resetButton);
	buttons.push_back(newTreeButton);

	int count = 0;
	while (!WindowShouldClose()) {
		if (status == NEWTREE) {
			deleteTree(root);
			nums.clear();
			for (int i = 0; i < 50; i++) {
				nums.push_back(dist(gen));
			}

			median = nums.at(nums.size() / 2);
			nums.erase(nums.begin() + nums.size()/2);

			root = new Node{median};

			status = NONE;
		}


		if (status != NONE && selectedNode == nullptr) {
			selectedNode = root;
		}
		else if (count % 30 == 0) {
			if (status == MIN && selectedNode->left != nullptr) selectedNode = selectedNode->left;
			if (status == MAX && selectedNode->right != nullptr) selectedNode = selectedNode->right;
		}

		BeginDrawing();

		ClearBackground(BLACK);

		DrawText("Hello Pointers!", (int)(WIDTH*0.85), (int)(HEIGHT*0.4),20,WHITE);

		if (nums.size() > 0 && count % 30 == 0) {
			int i =  dist(gen) % nums.size();
			placeValue(nums.at(i), root);
			nums.erase(nums.begin() + i);
		}

		traverseAndDraw(root, WIDTH/2, 20, 400);

		for (Button b : buttons) {
			if (isOver(b.x, b.y, b.xwidth, b.ywidth)) {
				DrawRectangle(b.x, b.y, b.xwidth, b.ywidth, BEIGE);
			}
			else {
				DrawRectangle(b.x, b.y, b.xwidth, b.ywidth, WHITE);
			}
			DrawText(b.name.c_str(), b.x + b.xwidth/3, b.y+b.ywidth/3, 20, BLACK);
			if (IsMouseButtonPressed(0) && isOver(b.x, b.y, b.xwidth, b.ywidth)) {
				(*b.mousePressed)();
			}
		}

		count++;
		EndDrawing();
	}

	deleteTree(root);

	CloseWindow();
	return 0;
}
