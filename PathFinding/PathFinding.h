using namespace std;
#include <vector>;
#include <tuple>;
#include <algorithm>;
#include <iostream>;

#define FIELD_SIZE_ROWS 11
#define FIELD_SIZE_COLS 11

template<typename T>
inline void remove(std::vector<T> & v, const T & item);

template< typename T, size_t N, size_t M >
void printArray(T(&theArray)[N][M]);

void copyMatrix(int matrixA[][FIELD_SIZE_COLS], int matrixB[][FIELD_SIZE_COLS]);

std::vector<int> findExitPoint(int matrix[][FIELD_SIZE_COLS]);

void calculateOptimalPaths(int matrix[][FIELD_SIZE_COLS], int pathMatrix[][FIELD_SIZE_COLS]);

void initGameField(int matrix[][FIELD_SIZE_COLS]);

std::vector<int> getNextStep(int pathMatrix[][FIELD_SIZE_COLS], int y, int x);