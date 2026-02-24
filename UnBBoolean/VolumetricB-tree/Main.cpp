#include "BBTree.h"
#include <iostream>
#include <string>

int main() {
	std::vector<Point> vertices;
	std::ifstream file("C:/Users/pepii/Desktop/teapot.obj");
	std::string c;
	
	BBTree<Polygon> tree;

	file >> c;
	while (file) {
		if (c == "v")
		{
			float x;
			float y;
			float z;
			file >> x >> y >> z;
			vertices.push_back({ x, y, z });
		}
		else if (c == "vt")
		{
			while (file && file.peek() != '\n')
			{
				char s;
				s = file.get();
			}
		}
		else if (c == "f")
		{
			std::vector<Point> polygon;
			std::string value;
			while (file && file.peek() != '\n')
			{
				char s;
				s = file.get();
				switch (s) {
				case '\t':
				case ' ':
					value.clear();
					break;
				case '/':
				{
					int idx = std::atoi(value.c_str())-1;
					polygon.push_back(vertices[idx]);
					value.clear();
				}
					break;
				default:
					value += s;
				}
			}
			tree.add(std::shared_ptr<BoundedObject>(new Polygon(polygon)));
		}
		else
		{
			while (file && file.peek() != '\n')
			{
				char s;
				s = file.get();
			}
		}
		file >> c;
	}
	std::map<int, float> levelVolume;
	tree.FillVolume(levelVolume);
	int height = levelVolume.size();

	std::cout << "level : totalVolume" << std::endl;
	std::cout << "0 : " << tree.GetBound().size() << std::endl;
	for (auto& levelAndVolume : levelVolume)
	{
		std::cout << levelAndVolume.first << " : " << levelAndVolume.second << std::endl;
	}

	std::map<int, std::pair<int,std::ofstream>> levelStreams;
	for (int i = 1; i <= height; ++i)
	{
		levelStreams[i].first = 0;
		levelStreams[i].second = std::ofstream("C:/Users/pepii/Desktop/level_"+std::to_string(i)+".obj");
		levelStreams[i].second << "mtllib default.mtl" << std::endl;
	}
	tree.write(levelStreams);

	return 0;
}