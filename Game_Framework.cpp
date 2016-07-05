#include <iostream>
#include <ctime>
#include <vector>
#include <chrono>
#include <thread>
#include <iomanip>
#include <ctime>

#include <opencv/cv.h> /// include most of core headers
#include <opencv/highgui.h> /// include GUI-related headers

#include <opencv2\opencv.hpp>

#define FIELD_SIZE_ROWS 11
#define FIELD_SIZE_COLS 11

const int EXIT = -1;
const int FREE_FIELD = 0;

using namespace std::chrono;

std::vector<int> getNextMonsterStep(std::vector<int> monster_current_gamefield_position, int gamefield_matrix[][FIELD_SIZE_COLS])
{
	// for the beginning, just move the monster by x+1. 
	// FUTUREWORK: More sophisticated path finding
	return std::vector<int>({ 
		monster_current_gamefield_position.at(0), 
		monster_current_gamefield_position.at(1) + 1 
	});
}

int main(int ac, char** av)
{
	// y, x
	std::vector<int> monster_current_gamefield_position = { 
		FIELD_SIZE_ROWS / 2, 
		-1 
	};
	int gamefield_matrix[FIELD_SIZE_ROWS][FIELD_SIZE_COLS] = { FREE_FIELD }; //y,x (with the top left corner being (0,0)
	gamefield_matrix[FIELD_SIZE_ROWS / 2][FIELD_SIZE_COLS - 1] = EXIT;
	int i = 0;

	while (true) {
		milliseconds startTime = duration_cast< milliseconds >(
			system_clock::now().time_since_epoch()
			);

		// check if monster has reached the exit. if yes, place it back on the start. if no, move it by one step
		if (monster_current_gamefield_position == std::vector<int>({ FIELD_SIZE_ROWS / 2, FIELD_SIZE_COLS - 1 })){
			std::cout << "EXIT was reached by the monster. Monster will be placed at the start again.\n";
			// put monster on start field on gamefield
			monster_current_gamefield_position = {
				FIELD_SIZE_ROWS / 2,
				0
			};
		}
		else{
			monster_current_gamefield_position = getNextMonsterStep(monster_current_gamefield_position, gamefield_matrix);	
		}

		// TODO: INTEGRATE: render gamefield with monster on it
		// 1. Find gamefield corners on the camera image
		// 2. Get projective tranformation matrix for transformation between camera image and game field
		// 3. Transform the relative coordinate of the monster into the aboslute coordinates on the map
		// 4. Render the monster on the absolute coordinate
		// FUTUREWORK: make an animated transition from the last absolute step coords to the next absolute step coords within 5s (continously get the current camera image, transformation matrix and render the monster with a minimal position change)
		// After the monster has reached the next step (after the 5s), we would continue the outer loop -> the monster does the next step (which itself would be animated to take 5s again)

		// Sleep until 1s have past from the beginning of this iteration. 
		// Why? to maintain a constant step rate
		milliseconds endTime = duration_cast< milliseconds >(
			system_clock::now().time_since_epoch()
			);
		milliseconds elapsed = endTime - startTime;
		std::this_thread::sleep_for(
			milliseconds(1000) - milliseconds(elapsed)
			);

		// print out the current monster position
		std::cout << i << "\tMonster position: \tx: " << monster_current_gamefield_position.at(1) << "\ty: " << monster_current_gamefield_position.at(0) << "\n";
		i++;
	}
	
}
