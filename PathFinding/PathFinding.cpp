using namespace std;
#include <vector>;
#include <tuple>;
#include <algorithm>;
#include <iostream>;

typedef vector< tuple<int, int, int> > tuple_list;
const int FREE_FIELD = 0;
const int START = 1000;
const int EXIT = -1;
const int WALL = 999;

#define FIELD_SIZE_ROWS 11
#define FIELD_SIZE_COLS 11

template<typename T>
inline void remove(std::vector<T> & v, const T & item)
{
	v.erase(std::remove(v.begin(), v.end(), item), v.end());
	// v.clear();
}

template< typename T, size_t N, size_t M >
void printArray(T(&theArray)[N][M]) {
	for (int x = 0; x < N; x++) {
		for (int y = 0; y < M; y++) {
			cout << theArray[x][y] << "\t ";
		}
		cout << "\n";
	}
}

void copyMatrix(int matrixA[][FIELD_SIZE_COLS], int matrixB[][FIELD_SIZE_COLS]) {
	for (int x = 0; x < FIELD_SIZE_ROWS; x++) {
		for (int y = 0; y < FIELD_SIZE_COLS; y++) {
			matrixB[x][y] = matrixA[x][y];
		}
	}
}

std::vector<int> findExitPoint(int matrix[][FIELD_SIZE_COLS]) {
	for (int x = 0; x < FIELD_SIZE_ROWS; x++) {
		for (int y = 0; y < FIELD_SIZE_COLS; y++) {
			if (matrix[x][y] == EXIT)
			{
				std::vector<int> result = { x, y };
				return result;
			}
		}
	}
	// return null;
}

/*
	FREE_FIELD: 0
	CURRENT_POSITION: 1000
	EXIT: -300
	WALL = -999
*/
void calculateOptimalPaths(int matrix[][FIELD_SIZE_COLS], int pathMatrix [][FIELD_SIZE_COLS])
{
	std::vector<int> exitPoint = findExitPoint(matrix);

	tuple_list main_list;
	main_list.push_back(tuple<int, int, int>(exitPoint[0], exitPoint[1], 0));

	int k = 0;

	while (k < main_list.size())
	{
		tuple<int, int, int>t = main_list.at(k);
		k++;

		// Create a list of the four adjacent cells, with a counter variable of the current element's counter variable + 1
		tuple_list adjacent_list;

		// top adjacent coord
		tuple<int, int, int> top = tuple<int, int, int>(
			get<0>(t) -1,
			get<1>(t),
			get<2>(t) +1);
		if (get<0>(top) >= 0 && get<1>(top) >= 0 && get<0>(top) < FIELD_SIZE_ROWS && get<1>(top) < FIELD_SIZE_COLS)
		{
			adjacent_list.push_back(top);
		}

		// right adjacent coord
		tuple<int, int, int> right = tuple<int, int, int>(
			get<0>(t),
			get<1>(t) +1,
			get<2>(t) +1);
		if (get<0>(right) >= 0 && get<1>(right) >= 0 && get<0>(right) < FIELD_SIZE_ROWS && get<1>(right) < FIELD_SIZE_COLS)
		{
			adjacent_list.push_back(right);
		}

		// bottom adjacent coord
		tuple<int, int, int> bottom = tuple<int, int, int>(
			get<0>(t) +1,
			get<1>(t),
			get<2>(t) +1);
		if (get<0>(bottom) >= 0 && get<1>(bottom) >= 0 && get<0>(bottom) < FIELD_SIZE_ROWS && get<1>(bottom) < FIELD_SIZE_COLS)
		{
			adjacent_list.push_back(bottom);
		}

		// left adjacent coord
		tuple<int, int, int> left = tuple<int, int, int>(
			get<0>(t),
			get<1>(t) -1,
			get<2>(t) +1);
		if (get<0>(left) >= 0 && get<1>(left) >= 0 && get<0>(left) < FIELD_SIZE_ROWS && get<1>(left) < FIELD_SIZE_COLS)
		{
			adjacent_list.push_back(left);
		}

		tuple_list new_adjacent_list;
		for (int i = 0; i < adjacent_list.size(); i++)
		{
			tuple<int, int, int>a = adjacent_list.at(i);
			//cout << matrix[6][10] << " \n";
			//cout << get<0>(a) << " " << get<1>(a) << "\n";

			if (matrix[get<0>(a)][get<1>(a)] != WALL)
			{
				new_adjacent_list.push_back(a);
			}
		}
		adjacent_list = new_adjacent_list;

		/*tuple_list new_main_list;
		for (int i = 0; i < main_list.size(); i++)
		{
		tuple<int, int, int>a = main_list.at(i);

		if (matrix[get<0>(a)][get<1>(a)] == WALL)
		{
		remove(main_list, a);
		}
		}*/

		new_adjacent_list.clear();
		for (int i = 0; i < adjacent_list.size(); i++)
		{
			tuple<int, int, int>a = adjacent_list.at(i);

			bool exists_already_or_better = false;

			for (int j = 0; j < main_list.size(); j++)
			{
				tuple<int, int, int>b = main_list.at(j);

				if (get<0>(a) == get<0>(b)
					&& get<1>(a) == get<1>(b))
				{
					if (get<2>(a) > get<2>(b))
					{
						exists_already_or_better = true;
					}
				}
			}

			if (!exists_already_or_better)
			{
				new_adjacent_list.push_back(a);
			}
		}
		adjacent_list = new_adjacent_list;

		for (int i = 0; i < adjacent_list.size(); i++)
		{
			tuple<int, int, int>a = adjacent_list.at(i);

			main_list.push_back(a);
		}
	}

	copyMatrix(matrix, pathMatrix);

	for (int i = 0; i < main_list.size(); i++)
	{
		tuple<int, int, int>a = main_list.at(i);
		// cout << get<0>(a) << " " << get<1>(a) << "\n";

		pathMatrix[get<0>(a)][get<1>(a)] = get<2>(a);
	}
}

void initGameField(int matrix [][FIELD_SIZE_COLS])
{
	int rows = FIELD_SIZE_ROWS;
	int cols = FIELD_SIZE_COLS;

	// create outer wall
	for (int i = 0; i < rows; i++)
	{
		matrix[i][0] = WALL;
		matrix[i][cols-1] = WALL;
	}

	for (int i = 0; i < rows; i++)
	{
		matrix[0][i] = WALL;
		matrix[rows-1][i] = WALL;
	}

	matrix[rows/2][0] = START;
	matrix[rows/2][cols-1] = EXIT;

	matrix[5][5] = WALL;
}

std::vector<int> getNextStep(int pathMatrix[][FIELD_SIZE_COLS], int y, int x)
{
	int top = pathMatrix[y - 1][x];
	int right = pathMatrix[y][x + 1];
	int bottom = pathMatrix[y + 1][x];
	int left = pathMatrix[y][x - 1];

	int minimum = std::min({ top, right, bottom, left });


	if (minimum == right)
	{
		std::vector<int> result = { y, x + 1 };
		return result;
	}

	if (minimum == top)
	{
		std::vector<int> result = { y - 1, x };
		return result;
	}

	if (minimum == bottom)
	{
		std::vector<int> result = { y + 1, x };
		return result;
	}

	if (minimum == left)
	{
		std::vector<int> result = { y, x - 1 };
		return result;
	}

}

void main()
{
	// matrix has 11x11 size, with 9x9 inner size (because of outer wall)
	int matrix[FIELD_SIZE_ROWS][FIELD_SIZE_COLS] = { 0 }; //y,x (with the top left corner being (0,0)
	int pathMatrix[FIELD_SIZE_ROWS][FIELD_SIZE_COLS] = { 0 };

	initGameField(matrix);

	calculateOptimalPaths(matrix, pathMatrix);

	printArray(pathMatrix);

	std::vector<int> nextStep = getNextStep(pathMatrix, 5, 4);
	cout << nextStep[0] << " " << nextStep[1] << "\n";

	nextStep = getNextStep(pathMatrix, nextStep[0], nextStep[1]);
	cout << nextStep[0] << " " << nextStep[1] << "\n";


	system("pause");
}