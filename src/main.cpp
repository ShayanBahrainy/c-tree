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


void traverseAndDraw(Node* root, int xOffset, int yOffset, int startingSpread=240) {
	DrawCircle(xOffset, yOffset, 10, WHITE);
	DrawText(std::to_string(root->val).c_str(), xOffset - 5, yOffset - 5, 5, BLACK);
	if (root->left != nullptr) {
		DrawLine(xOffset, yOffset, xOffset - startingSpread, yOffset + 60, GREEN);
		traverseAndDraw(root->left, xOffset - startingSpread, yOffset + 60, startingSpread/2);
	}
	if (root->right != nullptr) {
		DrawLine(xOffset, yOffset, xOffset + startingSpread, yOffset + 60, PURPLE);
		traverseAndDraw(root->right, xOffset + startingSpread, yOffset + 60, startingSpread/2);
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

void placeValue(int val, Node* root) {
	Node* prev;
	Node* curr = root;

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
}

int main () {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 69);

	const int HEIGHT = 10 * 100;
	const int WIDTH = 16 * 100;

	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	InitWindow(WIDTH, HEIGHT, "That's my christmas tree!");

	SearchAndSetResourceDir("resources");

	std::vector<int> nums;

	for (int i = 0; i < 50; ++i) {
		nums.push_back(dist(gen));
	}

	std::sort(nums.begin(), nums.end());

	int median = nums.at(nums.size() / 2);

	Node* root = new Node{median};

	nums.erase(nums.begin() + nums.size()/2);

	std::vector<Button> buttons;

	buttons.push_back(Button{WIDTH/2, HEIGHT/2, 300, 300, "Nice One!", *niceOne});

	int count = 0;
	while (!WindowShouldClose()) {
		BeginDrawing();

		ClearBackground(BLACK);

		DrawText("Hello World!", 0,0,20,WHITE);

		if (nums.size() > 0 && count % 30 == 0) {
			int i =  dist(gen) % nums.size();
			placeValue(nums.at(i), root);
			nums.erase(nums.begin() + i);
		}

		traverseAndDraw(root, WIDTH/2, 20, 240);

		for (Button b : buttons) {
			DrawRectangle(b.x, b.y, b.xwidth, b.ywidth, WHITE);
			DrawText(b.name.c_str(), b.x + b.xwidth/2, b.y+b.ywidth/2, 50, BLACK);
			if (IsMouseButtonPressed(0) && GetMouseX() > b.x && GetMouseX() < b.x + b.xwidth && GetMouseY() > b.y && GetMouseY() < b.y + b.ywidth) {
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
