#pragma once
#include <Windows.h>
#include <iostream>
#include <math.h>
#include<vector>
#define PI 3.14159265
class Entity {
public:
	float xpos, ypos, zpos, aimx, aimy;
	int health;
	
};
float CalculateDistance(Entity a, Entity b) {
	float dis;
	dis = (float)sqrt(pow((a.xpos - b.xpos), 2) + pow((a.ypos - b.ypos), 2) + pow((a.zpos - b.zpos), 2));
	return dis;
}
Entity FindNearestAliveEntity(Entity main, std::vector<Entity> Entities) {
	unsigned int closest=0;
	float dis = CalculateDistance(main, Entities[0]);
	for (unsigned int i = 0; i < Entities.size(); i++)
	{
		if (CalculateDistance(main, Entities[i]) < dis&& Entities[i].health>0 && Entities[i].health <= 100) {
			dis = CalculateDistance(main, Entities[i]);
			closest = i;
		}
	}
	return Entities[closest];


}



