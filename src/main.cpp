#include <vector>
#include <string>
#include <cstdlib>
#include <random>
#include <algorithm>
#include <iostream>
#include <sstream>
#include "raylib.h"
#include "resource_dir.h"	// utility header for SearchAndSetResourceDir
#include "Node.h"
#include "Button.h"

enum Status {NONE, MAX, MIN, NEWTREE, BINARYSEARCH, LINEARSEARCH};

Status status = NONE;
Node* selectedNode = nullptr;
Node* root;
bool setupMode = false;

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
	if (node == nullptr) return;
	if (node->left != nullptr) deleteTree(node->left);
	if (node->right != nullptr) deleteTree(node->right);

	delete node;
}

void findMin(Button* self) {
	std::cout << "Finding the minimum..." << std::endl;
	status = MIN;
	selectedNode = nullptr;
}

void findMax(Button* self) {
	std::cout << "Finding the maximum.." << std::endl;
	status = MAX;
	selectedNode = nullptr;
}

void reset(Button* self) {
	std::cout << "Resetting..." << std::endl;
	status = NONE;
	selectedNode = nullptr;
}

void newTree(Button* self) {
	std::cout << "Creating new tree..." << std::endl;
	status = NEWTREE;
	selectedNode = nullptr;
}

void switchMode(Button* self) {
	std::cout << "Switching mode..." << std::endl;
	const std::string BT = "Binary Tree";
	const std::string BS = "Binary Search";
	const std::string LS = "Linear Search";
	std::cout << self->name << std::endl;
	if (self->name == BT) {
		self->name = BS;
		status = BINARYSEARCH;
		setupMode = true;
	}
	else if (self->name == BS) {
		self->name = LS;
		status = LINEARSEARCH;
		setupMode = true;
	}
	else if (self->name == LS) {
		self->name = BT;
		status = NEWTREE;
		setupMode = true;
	}
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
	const std::string BS_HELPER = "Red = Left Bound\nBlue = Right Bound\nPurple = R+L/2 (Midpoint)\nGrey = Target";

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

	std::vector<Button*> buttons;

	Button* minButton = new Button{(int)(WIDTH*0.8), (int)(HEIGHT*0.8), (int)(0.2 * WIDTH), (int)(0.1 * HEIGHT), "Minimum", *findMin};
	Button* maxButton = new Button{(int)(WIDTH*0.8), (int)(HEIGHT*0.7), (int)(0.2 * WIDTH), (int)(0.1 * HEIGHT), "Maximum", *findMax};
	Button* resetButton = new Button{(int)(WIDTH*0.8), (int)(HEIGHT*0.6), (int)(0.2 * WIDTH), (int)(0.1 * HEIGHT), "Reset", *reset};
	Button* newTreeButton = new Button{(int)(WIDTH*0.8), (int)(HEIGHT*0.5), (int)(0.2 * WIDTH), (int)(0.1 * HEIGHT), "New Tree", *newTree};
	Button* modeButton = new Button{(int)(WIDTH*0.8), (int)(HEIGHT*0.4), (int)(0.2 * WIDTH), (int)(0.1 * HEIGHT), "Binary Tree", *switchMode};

	std::vector<int> searchList;
	int leftBound;
	int rightBound;
	int midPoint;
	int target;

	int count = 0;
	while (!WindowShouldClose()) {
		if (status != BINARYSEARCH && status != LINEARSEARCH) {
			buttons = {minButton, maxButton, resetButton, newTreeButton, modeButton};
		}
		else {
			buttons = {modeButton};
		}

		if (status == NEWTREE) {
			deleteTree(root);
			nums.clear();
			searchList.clear();

			for (int i = 0; i < 50; i++) {
				nums.push_back(dist(gen));
			}

			median = nums.at(nums.size() / 2);
			nums.erase(nums.begin() + nums.size()/2);

			root = new Node{median};

			status = NONE;
		}

		if (status == BINARYSEARCH && setupMode) {
			deleteTree(root);
			for (int i = 0; i < 50; ++i) {
				searchList.push_back(i);
			}
			nums.clear();
			root = nullptr;
			leftBound = 0;
			rightBound = searchList.size() - 1;
			midPoint = (leftBound + rightBound) / 2;
			target = searchList.at(dist(gen) % searchList.size());
			setupMode = false;
		}

		if (status == LINEARSEARCH && setupMode) {
			deleteTree(root);
			for (int i = 0; i < 50; ++i) {
				searchList.push_back(i);
			}
			nums.clear();
			root = nullptr;
			leftBound = -1;
			rightBound = -1;
			midPoint = 0;
			target = searchList.at(dist(gen) % searchList.size());
			setupMode = false;
		}

		if (status == NONE && root == nullptr) {
			status = NEWTREE;
			searchList.clear();
		}

		if ((status == MIN || status == MAX) && selectedNode == nullptr) {
			selectedNode = root;
		}
		else if (count % 120 == 0) {
			if (status == BINARYSEARCH) {
				if (searchList.at(midPoint) < target) {
					leftBound = midPoint + 1;
					midPoint = (leftBound + rightBound)/2;
				}
				if (searchList.at(midPoint) > target) {
					rightBound = midPoint - 1;
					midPoint = (leftBound + rightBound)/2;
				}
			}
		}
		else if (count % 30 == 0) {
			if (status == MIN && selectedNode->left != nullptr) selectedNode = selectedNode->left;
			if (status == MAX && selectedNode->right != nullptr) selectedNode = selectedNode->right;
			if (status == LINEARSEARCH && searchList.at(midPoint) != target) midPoint++;
		}

		BeginDrawing();

		ClearBackground(BLACK);

		//If we have more numbers to place, place them every 30 frames
		if (nums.size() > 0 && count % 30 == 0) {
			int i =  dist(gen) % nums.size();
			placeValue(nums.at(i), root);
			nums.erase(nums.begin() + i);
		}

		//Draw tree
		if (root != nullptr) {
			traverseAndDraw(root, WIDTH/2, 20, 400);
		}

		//Draw list
		for (int i = 0; i < searchList.size(); ++i) {
			int x = WIDTH/20 + (i * 30);
			DrawRectangle(x - 2.5, HEIGHT/3 - 2.5, 35, 35, BEIGE);
			if (searchList.at(i) == target) {
				DrawRectangle(x - 2.5, HEIGHT/3 - 2.5, 35, 35, BLACK);
			} else {
				DrawRectangle(x - 2.5, HEIGHT/3 - 2.5, 35, 35, BEIGE);
			}
			DrawRectangle(x, HEIGHT/3, 30, 30, WHITE);
			DrawText(std::to_string(searchList.at(i)).c_str(), x + 2, HEIGHT/3 + 10, 20, BLACK);
			//Draw bound circles
			if (status == BINARYSEARCH || status == LINEARSEARCH) {
				if (leftBound == i) {
					DrawCircle(x + 15, HEIGHT/3 - 20, 10, RED);
				}
				if (rightBound == i) {
					DrawCircle(x + 15, HEIGHT/3 - 20, 10, BLUE);
				}
				if (midPoint == i) {
					DrawCircle(x + 15, HEIGHT/3 - 20, 10, PURPLE);
				}
			}
		}

		//Binary search helper text
		if (status == BINARYSEARCH) {
			DrawText(BS_HELPER.c_str(), 50, 50, 20, WHITE);
		}
		//BST result text
		if ((status == MIN || status == MAX) && selectedNode != nullptr) {
			std::ostringstream text;
			text << "Current ";
			if (status == MIN) {
				text << "min";
			}
			else {
				text << "max";
			}
			text << ": " << selectedNode->val;
			DrawText(text.str().c_str(), (int)(WIDTH * 0.8), HEIGHT/20, 20, WHITE);
		}

		for (Button* b : buttons) {
			if (isOver(b->x, b->y, b->xwidth, b->ywidth)) {
				DrawRectangle(b->x, b->y, b->xwidth, b->ywidth, BEIGE);
			}
			else {
				DrawRectangle(b->x, b->y, b->xwidth, b->ywidth, WHITE);
			}
			DrawText(b->name.c_str(), b->x + b->xwidth/3, b->y+b->ywidth/3, 20, BLACK);
			if (IsMouseButtonPressed(0) && isOver(b->x, b->y, b->xwidth, b->ywidth)) {
				(*b->mousePressed)(b);
			}
		}
		count++;
		EndDrawing();
	}

	deleteTree(root);
	for (Button* b : buttons) {
		delete b;
	}

	CloseWindow();
	return 0;
}
