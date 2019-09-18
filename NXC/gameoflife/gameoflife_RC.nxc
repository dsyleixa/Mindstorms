// Game of life
// http://en.wikipedia.org/wiki/Conway's_Game_of_Life
// Code adapted from http://cboard.cprogramming.com/c-programming/128982-simple-life-game-code-near-end.html

// THIS CODE WILL ONLY WORK WITH ROBOTC VERSION 4.26 AND HIGHER.
// Xander Soldaat (xander_at_botbench.com)
// 30 September 2014
// version 1

// The blocks are blockSize * blockSize big
// 4 seems to be a good value for this, 8 works well too
// 2 is too small and don't even bother with 1
const int blockSize = 4;

// Increase the chance of an initially live cell
// by making this number smaller
const int seedChance = 2;

// The size of the EV3 screen
#if defined(EV3)
const int screenWidth = 178;
const int screenHeight = 128;

// The size of the NXT screen
#elif defined(NXT)
const int screenWidth = 100;
const int screenHeight = 64;
#endif

// Make the board +1 on each side to ensure that there's an invisible border of dead cells
const int rows = (screenHeight / blockSize) + 2 ;
const int cols = (screenWidth / blockSize) + 2;

// We need two boards, one for the current generation and one for calculating the next one
// Optimization tip: use bits instead of entire bytes for the cell status
char board[rows][cols];
char tmpboard[rows][cols];

// Count thy neighbours
int countNeighbours(int row, int col)
{
  int count = 0;
  for (int x = -1; x <= +1; x++)
  {
    for (int y = -1; y <= +1; y++)
    {
      if ((board[row + x][col + y] == 1) && (x != 0 || y != 0))
        count++;
    }
  }
  return count;
}

// Calculate the cells that will live and die for the next generation
void calculateGeneration()
{
  int aliveNeighbours = 0;

  // Clear the boardfor the next generation
  memset(tmpboard, 0, sizeof(tmpboard));

  for (int row = 1; row < (rows - 1); row++)
  {
    for (int col = 1; col < (cols - 1); col++)
    {
      aliveNeighbours = countNeighbours(row, col);

      // Rules taken from http://en.wikipedia.org/wiki/Conway's_Game_of_Life

      // Any live cell with fewer than two live neighbours dies, as if caused by under-population.
      if(aliveNeighbours < 2)
        tmpboard[row][col] = 0;

      // Any live cell with two or three live neighbours lives on to the next calculateGeneration
      if (aliveNeighbours >= 2  &&  aliveNeighbours <= 3 )
        tmpboard[row][col] = board[row][col];

      // Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction
      if(aliveNeighbours == 3 && board[row][col]==0)
        tmpboard[row][col] = 1;

      // Any live cell with more than three live neighbours dies, as if by overcrowding
      if(aliveNeighbours > 3)
        tmpboard[row][col] = 0;
    }
  }
  // Copy the new board to the old one
  memcpy(board, tmpboard, sizeof(tmpboard));
}


// Draw all the cells
void drawBoard()
{
  // Wipe the screen
  eraseDisplay();
  for (int row = 0; row < (rows - 2); row++)
  {
    for (int col = 0; col < (cols - 2); col++)
    {
      // Draw all the "live" cells.
      if (board[row + 1][col + 1])
        fillRect(col * blockSize, row * blockSize, (col * blockSize) + blockSize, (row * blockSize) + blockSize);
    }
  }
}

// This adds some random live cells to the board
void initBoard()
{
  for (int row = 1; row < (rows - 1); row++)
  {
    for (int col = 1; col < (cols - 1); col++)
    {
      board[row][col] = !(rand() % seedChance);
    }
  }
}

task main()
{
  initBoard();
  drawBoard();
  sleep(1000);
  while (true)
  {
    // Uncomment the below section if you want to randomly add a live cell to the board
    // It can keep things interesting. Change the "2" to something a little higher or
    // lower, depending on how often you want a new cell to magically come alive.

    //if (!(rand() % 2))
    //{
    //  int row = (abs(rand()) % (rows - 2)) + 1;
    //  int col = (abs(rand()) % (cols - 2)) + 1;
    //  writeDebugStreamLine("random cell: x: %d, y: %d", row, col);
    //  board[row][col] = 1;
    //}

    calculateGeneration();
    drawBoard();
    sleep(200);
  }
}
