
#define WIDTH 700
#define HEIGHT 700
#define TITLE "Game" 

#define ROWS 20
#define COLS 20

#define CELL_WIDTH (WIDTH/COLS)
#define CELL_HEIGHT (HEIGHT/ROWS)

typedef struct Cell
{
    int i;
    int j;
    bool containsMine;
    bool revealed;
    int nearByMines;
    bool flagged;
} Cell;


typedef enum GameState
{
    PLAYING,
    LOSE,
    WIN,
} GameState;

void CellDraw(Cell);
bool IndexIsValid(int, int);
void CellReveal(int, int);
int CellCountMines(int, int);
void CellFlag(int, int);
void GridInit(void);
void GridFloodClearFrom(int, int);
void GameInit(void);

