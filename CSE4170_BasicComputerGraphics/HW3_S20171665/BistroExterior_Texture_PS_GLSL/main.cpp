//
//  main.cpp
//
//  Written for CSE4170
//  Department of Computer Science and Engineering
//  Copyright © 2023 Sogang University. All rights reserved.
//

#define _CRT_SECURE_NO_WARNINGS

#include "LoadScene.h"
#include "DrawScene.h"

SCENE scene;

int main(int argc, char* argv[]) {
	
	read3DSceneFromFile(&scene);
	drawScene(argc, argv);
	freeData(&scene);

	return 1;
}
