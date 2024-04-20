#include "raylib.h"
#include "utils.h"
#include "time.h"
#include "stdlib.h"
#include "raymath.h"
#include <stdio.h>

Cell grid[ROWS][COLS];

Texture2D flagSptrite;

GameState state = PLAYING;

const char *youLoose = "You Lose";
const char *pressRToRestart = "Press 'R' to restart";
const char *youWin = "You Win";
int tilesRevealed;
int minesPresent;

int main(int argc, char *argv[])
{
    InitWindow(WIDTH, HEIGHT, TITLE);

    flagSptrite = LoadTexture("resources/flag.png");

    GridInit();

    while (!WindowShouldClose())
    {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            Vector2 mPos = GetMousePosition();
            int index_i = mPos.x / CELL_WIDTH;
            int index_j = mPos.y / CELL_HEIGHT;

            if (state == PLAYING && IndexIsValid(index_i, index_j))
            {
                CellReveal(index_i, index_j);
            }
        }
        else if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
        {
            Vector2 mPos = GetMousePosition();
            int index_i = mPos.x / CELL_WIDTH;
            int index_j = mPos.y / CELL_HEIGHT;
            if (IndexIsValid(index_i, index_j))
            {
                CellFlag(index_i, index_j);
            }
        }

        if (IsKeyPressed(KEY_R))
        {
            GameInit();
        }

        BeginDrawing();

        ClearBackground(RAYWHITE);

        for (int i = 0; i < COLS; i++)
        {
            for (int j = 0; j < ROWS; j++)
            {
                CellDraw(grid[i][j]);
            }
        }

        if (state == LOSE)
        {
            DrawRectangle(0, 0, WIDTH, HEIGHT, Fade(WHITE, 0.8f));
            DrawText(youLoose, WIDTH / 2 - (MeasureText(youLoose, 20) / 2), HEIGHT / 2 - 10, 20, DARKGRAY);
            DrawText(pressRToRestart, WIDTH / 2 - (MeasureText(pressRToRestart, 20) / 2), 300, 20, DARKGRAY);
        }

        if (state == WIN)
        {

            DrawRectangle(0, 0, WIDTH, HEIGHT, Fade(WHITE, 0.8f));
            DrawText(youWin, WIDTH / 2 - (MeasureText(youWin, 20) / 2), HEIGHT / 2 - 10, 20, DARKGRAY);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}

void GridInit()
{
    for (int i = 0; i < COLS; i++)
    {
        for (int j = 0; j < ROWS; j++)
        {
            grid[i][j] = (Cell){
                .i = i,
                .j = j,
                .containsMine = false,
                .revealed = false,
                .flagged = false,
                .nearByMines = -1,
            };
        }
    }

    minesPresent = (int)ROWS * COLS * 0.1f;

    int minesToPlace = minesPresent;
    while (minesToPlace > 0)
    {
        int i = rand() % COLS;
        int j = rand() % ROWS;

        if (!grid[i][j].containsMine)
        {
            grid[i][j].containsMine = true;
            minesToPlace--;
        }
    }

    for (int i = 0; i < COLS; i++)
    {
        for (int j = 0; j < ROWS; j++)
        {
            if (!grid[i][j].containsMine)
            {

                grid[i][j].nearByMines = CellCountMines(i, j);
            }
        }
    }
}

void CellDraw(Cell cell)
{
    if (cell.revealed)
    {
        if (cell.containsMine)
        {
            DrawRectangle(cell.i * CELL_WIDTH, cell.j * CELL_HEIGHT, CELL_WIDTH, CELL_HEIGHT, RED);
        }
        else
        {
            DrawRectangle(cell.i * CELL_WIDTH, cell.j * CELL_HEIGHT, CELL_WIDTH, CELL_HEIGHT, LIGHTGRAY);
            if (cell.nearByMines > 0)
            {
                DrawText(TextFormat("%d", cell.nearByMines), cell.i * CELL_WIDTH + 12, cell.j * CELL_HEIGHT + 4, CELL_HEIGHT - 8, BLACK);
            }
        }
    }
    else if (cell.flagged)
    {
        Rectangle source = {0, 0, flagSptrite.width, flagSptrite.height};
        Rectangle dest = {cell.i * CELL_WIDTH, cell.j * CELL_HEIGHT, CELL_WIDTH, CELL_HEIGHT};
        Vector2 origini = {0, 0};
        DrawTexturePro(flagSptrite, source, dest, origini, 0.0f, Fade(RED, 0.9f));
    }

    DrawRectangleLines(cell.i * CELL_WIDTH, cell.j * CELL_HEIGHT, CELL_WIDTH, CELL_HEIGHT, BLACK);
}

bool IndexIsValid(int index_i, int index_j)
{
    return index_i >= 0 && index_i < COLS && index_j >= 0 && index_j < ROWS;
}

void CellReveal(int i, int j)
{
    if (grid[i][j].flagged)
    {
        return;
    }
    grid[i][j].revealed = true;

    if (grid[i][j].containsMine)
    {
        state = LOSE;
    }
    else
    {
        // play sound
        if (grid[i][j].nearByMines == 0)
        {
            GridFloodClearFrom(i, j);
        }

        tilesRevealed++;
        if (tilesRevealed > ROWS * COLS - minesPresent)
        {
            state = WIN;
        }
    }
}
void CellFlag(int i, int j)
{
    if (grid[i][j].revealed)
    {
        return;
    }
    grid[i][j].flagged = !grid[i][j].flagged;
}

int CellCountMines(int i, int j)
{
    int count = 0;
    for (int io = -1; io <= 1; io++)
    {
        for (int jo = -1; jo <= 1; jo++)
        {
            if (i == 0 && j == 0)
            {
                continue;
            }
            if (!IndexIsValid(i + io, j + jo))
            {
                continue;
            }
            if (grid[i + io][j + jo].containsMine)
            {

                count++;
            }
        }
    }

    return count;
}

void GridFloodClearFrom(int i, int j)
{
    for (int io = -1; io <= 1; io++)
    {
        for (int jo = -1; jo <= 1; jo++)
        {
            if (io == 0 && jo == 0)
            {
                continue;
            }
            if (!IndexIsValid(i + io, j + jo))
            {
                continue;
            }
            if (!grid[i + io][j + jo].revealed)
            {

                CellReveal(i + io, j + jo);
            }
        }
    }
}

void GameInit()
{
    GridInit();
    state = PLAYING;
}
