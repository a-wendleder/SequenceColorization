#include <iostream>
#include <string>

using namespace std;

bool pixel_at(int width, int x, int y, bool *ptr) {
	return (bool)ptr[y*width + x];
}

int maitn() {
	int w = 200;
	int h = 200;
	bool * ptr = (bool*)malloc(sizeof(bool)* w * h);

	
	for (int i = 0; i < w; i++)
	{
		for (int j = 0; j < h; j++)
		{
			bool b = pixel_at(w, i, j, ptr);
			cout << b; cout << " ";
		}
	}




	string a;

	cin >> a;


	return 0;
	return 0;
}