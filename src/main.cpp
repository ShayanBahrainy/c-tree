#include <vector>
#include <string>
#include <cstdlib>
#include <random>

#include "raylib.h"
#include "resource_dir.h"	// utility header for SearchAndSetResourceDir
#include "Node.h"


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

int main () {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 99);

	const int HEIGHT = 10 * 100;
	const int WIDTH = 16 * 100;

	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	InitWindow(WIDTH, HEIGHT, "That's my christmas tree!");

	SearchAndSetResourceDir("resources");

	Node* root = generateTree(5, gen, dist);

	while (!WindowShouldClose()) {
		BeginDrawing();

		ClearBackground(BLACK);

		DrawText("Hello World!", 0,0,20,WHITE);

		traverseAndDraw(root, WIDTH/2, 20, 240);

		EndDrawing();
	}
	CloseWindow();
	return 0;
}
