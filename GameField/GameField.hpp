#include <vector>

#define FIELD_SIZE_ROWS 100
#define FIELD_SIZE_COLS 141

const int EXIT = -1;
const int FREE_FIELD = 0;

using namespace std;

class GameField
{
public:
	GameField();
	vector<int> getNextMonsterStep(vector<int> monster_current_gamefield_position, int gamefield_matrix[][FIELD_SIZE_COLS]);
	vector<int> getMonsterPosition();
	void setMonsterPosition(vector<int> position);
	void monsterStep();
private: 
	vector<int> monster_current_gamefield_position;
	int gamefield_matrix[FIELD_SIZE_ROWS][FIELD_SIZE_COLS];
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

vector<int> GameField::getNextMonsterStep(vector<int> monster_current_gamefield_position, int gamefield_matrix[][FIELD_SIZE_COLS])
{
	// for the beginning, just move the monster by x+1.
	// FUTUREWORK: More sophisticated path finding
	return std::vector<int>({
		GameField::monster_current_gamefield_position.at(0),
		GameField::monster_current_gamefield_position.at(1) + 1
	});
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
	// for the beginning, just move the monster by x+1.
	// FUTUREWORK: More sophisticated path finding
	GameField::monster_current_gamefield_position = vector<int>({
		monster_current_gamefield_position.at(0),
		monster_current_gamefield_position.at(1) + 1
	});
}