#include <vector>

#define FIELD_SIZE_ROWS 100
#define FIELD_SIZE_COLS 141

const int EXIT = -1;
const int FREE_FIELD = 0;
const int WALL = 1000;

using namespace std;

class GameField
{
public:
	GameField();
	vector<int> getNextMonsterStep(vector<int> monster_current_gamefield_position, int gamefield_matrix[][FIELD_SIZE_COLS]);
	vector<int> getMonsterPosition();
	void setMonsterPosition(vector<int> position);
	void monsterStep();
	void readCameraForWalls(Mat thresholdedGrid);
private: 
	vector<int> monster_current_gamefield_position;
	int gamefield_matrix[FIELD_SIZE_ROWS][FIELD_SIZE_COLS];
	bool isWallOrOut(int y, int x);
};

GameField::GameField()
{
	// y, x
	GameField::monster_current_gamefield_position = {
		FIELD_SIZE_ROWS / 2,
		-1
	};
	GameField::gamefield_matrix[FIELD_SIZE_ROWS][FIELD_SIZE_COLS] = { FREE_FIELD }; //y,x (with the top left corner being (0,0)
	GameField::gamefield_matrix[FIELD_SIZE_ROWS / 2][FIELD_SIZE_COLS - 1] = EXIT;
}

vector<int> GameField::getMonsterPosition()
{
	return GameField::monster_current_gamefield_position;
}

void GameField::setMonsterPosition(vector<int> position)
{
	GameField::monster_current_gamefield_position = position;
}

void GameField::monsterStep()
{
	// FUTUREWORK: More sophisticated path finding

	if (GameField::isWallOrOut(
		monster_current_gamefield_position.at(0), monster_current_gamefield_position.at(1) + 1))
	{
		if (GameField::isWallOrOut(
			monster_current_gamefield_position.at(0) + 1, monster_current_gamefield_position.at(1)))
		{
			// Back to start
			GameField::monster_current_gamefield_position = vector<int>({
				FIELD_SIZE_ROWS / 2,
				0
			});
		}
		else {
			GameField::monster_current_gamefield_position = vector<int>({
				monster_current_gamefield_position.at(0) + 1,
				monster_current_gamefield_position.at(1)
			});
		}
	}
	else {
		GameField::monster_current_gamefield_position = vector<int>({
			monster_current_gamefield_position.at(0),
			monster_current_gamefield_position.at(1) + 1
		});
	}

}

void GameField::readCameraForWalls(Mat thresholdedGrid)
{
	for (int i = 0; i < FIELD_SIZE_ROWS; i++)
	{
		for (int j = 0; j < FIELD_SIZE_COLS; j++)
		{
			if (thresholdedGrid.at<int>(i, j) == 0)
			{
				GameField::gamefield_matrix[i][j] = WALL;
			}
			else
			{
				GameField::gamefield_matrix[i][j] = FREE_FIELD;
			}
		}
	}
}

bool GameField::isWallOrOut(int y, int x)
{
	if (x < 0 || x >= FIELD_SIZE_COLS) return true;
	if (y < 0 || y >= FIELD_SIZE_ROWS) return true;

	if (GameField::gamefield_matrix[y][x] == WALL) return true;

	return false;
}