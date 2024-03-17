#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>


RF24 radio(7, 8); // CE, CSN

const byte address[6] = "CA100820";

// Define the size of the grid
const int GRID_SIZE = 5;

// Define a struct to represent a position
struct Position {
    int x;
    int y;

     // Overload the equality operator
    bool operator==(const Position& other) const {
        return x == other.x && y == other.y;
    }
};

struct Cell {
    bool northWall = false;
    bool southWall = false;
    bool eastWall = false;
    bool westWall = false;
    int visited = 0; // 0 = not explored, 1 = explored robot one, 2 = explored robot 2
};


struct Robot {
    Position pos; // Current position
    Position posExplored[GRID_SIZE * GRID_SIZE];
    int exploredCount = 0;
    Position goal;
    // Add more attributes such as direction, status, etc.
};


// Define the grid as a 2D array (since std::vector isn't supported in standard Arduino)
Cell grid[GRID_SIZE][GRID_SIZE];
Robot robotOne;
Robot robotTwo;

bool exploring = true;
bool navToGoal = false;

// Create the grid
void initializeGrid() {
    // Set all cells in the grid to 0 (empty)
    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            grid[y][x].visited = 0;
        }
    }
}

void setStartPosition(Position robotOnePos, Position robotTwoPos) {
  robotOne.pos = robotOnePos;
  robotTwo.pos = robotTwoPos;
  grid[robotOne.pos.y][robotOne.pos.x].visited = 1;
  grid[robotTwo.pos.y][robotTwo.pos.x].visited = 2;
  robotOne.posExplored[0] = robotOnePos;
  robotTwo.posExplored[0] = robotTwoPos;
  robotOne.exploredCount++;
  robotTwo.exploredCount++;
}

//Debugging
void printGrid() {
  Serial.println(); // Newline after each row

    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
          Position currentPos = {y, x};
          Serial.print(grid[y][x].visited); 
          
          Serial.print(" ");
        }
        Serial.println(); // Newline after each row
    }
}

void checkNeighbours(Position currentPos)
{
  Cell cell = grid[currentPos.y][currentPos.x];
  //set walls if there are some based on data received from robot
  if (radio.available()) {
    int data[4];
    radio.read(&data, sizeof(data));
    grid[currentPos.y][currentPos.x].northWall = data[0];
    grid[currentPos.y][currentPos.x].eastWall = data[1];
    grid[currentPos.y][currentPos.x].southWall = data[2];
    grid[currentPos.y][currentPos.x].westWall = data[3];
  }
}

//Set adjacent cell wall accordingly 
void setNeighbourWall(Position pos) {
    // Check and set the east wall's mirror (west wall of the cell to the right)
    if(pos.x + 1 < GRID_SIZE && grid[pos.y][pos.x].eastWall) {
        grid[pos.y][pos.x + 1].westWall = true;
    }

    // Check and set the south wall's mirror (north wall of the cell below)
    if(pos.y + 1 < GRID_SIZE && grid[pos.y][pos.x].southWall) {
        grid[pos.y + 1][pos.x].northWall = true;
    }

    // Check and set the west wall's mirror (east wall of the cell to the left)
    if(pos.x - 1 >= 0 && grid[pos.y][pos.x].westWall) {
        grid[pos.y][pos.x - 1].eastWall = true;
    }

    // Check and set the north wall's mirror (south wall of the cell above)
    if(pos.y - 1 >= 0 && grid[pos.y][pos.x].northWall) {
        grid[pos.y - 1][pos.x].southWall = true;
    }
}


//Send commands on where the robot should move
void sendCommand(Robot robot, Position nextPos)
{
  //Send commands of next position
   // Define an integer array
  int data[4] = {robot.pos.x, robot.pos.y, nextPos.x, nextPos.y};

  // Send the integer array
  radio.write(&data, sizeof(data));

  delay(100);

  //Set as visited
  if(robot.pos == robotOne.pos) grid[robot.pos.y][robot.pos.x].visited = 1;
  else grid[robot.pos.y][robot.pos.x].visited = 2;
}

// Placeholder function to check if a position is within bounds and not visited
bool inBoundary(Position pos) {
  return pos.x >= 0 && pos.x < GRID_SIZE && pos.y >= 0 && pos.y < GRID_SIZE;
}

bool wallCheck(Position pos, int direction) {
  // Check current position for walls in the specified direction
  if(direction == 0) return grid[pos.y][pos.x].northWall; // North
  if(direction == 1) return grid[pos.y][pos.x].eastWall;  // East
  if(direction == 2) return grid[pos.y][pos.x].southWall; // South
  if(direction == 3) return grid[pos.y][pos.x].westWall;  // West

  return true; // If direction is invalid, assume movement is blocked
}

Position GetNextPosition(Robot &robot) {
    Position currentPos = robot.pos;
    Cell currentCell = grid[currentPos.y][currentPos.x];

    // Try moving North first, then East, South, and West last.
    // This order can be changed based on exploration preference or strategy.
    // Directions are based on (y, x)
    Position nextPositions[4] = {
        {currentPos.x, currentPos.y - 1}, // North: Decrease y
        {currentPos.x + 1, currentPos.y}, // East: Increase x
        {currentPos.x, currentPos.y + 1}, // South: Increase y
        {currentPos.x - 1, currentPos.y}  // West: Decrease x
    };

    //Check all neighboring postions to see if available
    for (int i = 0; i < 4; i++) {
        Position nextPos = nextPositions[i];
        // Check if the move is valid: within bounds, not visited, and no wall blocking the path.
        if (inBoundary(nextPos) && !wallCheck(currentPos, i)) {
          //Move to unvisited
          if(grid[nextPos.y][nextPos.x].visited == 0)
          {
            robot.exploredCount += 1;
            robot.posExplored[robot.exploredCount] = currentPos;
            // Additional checks for walls based on the direction can be added here
            return nextPos; // Move in the first available direction
          }
        }
    }

  // If no valid move is found, backtrack based on previous stored positions (SIMPLE APPROACH)
  if(robot.exploredCount > 0){
    currentPos = robot.posExplored[--robot.exploredCount]; // Decrement first, then access
  }

  return currentPos;
}


bool checkIfMapped()
{
  for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
          if(grid[y][x].visited == 0)
          {
            return false;
          }
        }
    }

  return true;
}

int heuristic(Position a, Position b) {
    return abs(a.x - b.x) + abs(a.y - b.y);
}

Position findNearestUnexplored(Position start) {
    // Initialize to start; this will hold our nearest unexplored cell
    Position nearestUnexplored = start; 
    int nearestDistance = 100;

    // Iterate over the entire grid to find the nearest unexplored cell
    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            if (grid[y][x].visited == 0) { // Unexplored cell
                Position potentialTarget = {x, y};
                int distance = heuristic(start, potentialTarget);
                if (distance < nearestDistance) {
                    nearestDistance = distance;
                    nearestUnexplored = potentialTarget;
                }
            }
        }
    }
    // Return the nearest unexplored cell's position
    return nearestUnexplored;
}

//Debugging
void checkCurrentPositions()
{
  Serial.println();
  Serial.print("Robot One Position: X= " + String(robotOne.pos.x) + " Y= " + String(robotOne.pos.y));
  Serial.println();
  Serial.print("Robot Two Position: X= " + String(robotTwo.pos.x) + " Y= " + String(robotTwo.pos.y));
}

void setup() {

  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
  radio.openWritingPipe(address);
  radio.stopListening();

  // Initialize the grid
  initializeGrid();

  // Set the starting positions of the robots
  setStartPosition({0, 0}, {GRID_SIZE - 1, GRID_SIZE - 1});
}

void loop() {
  if(exploring)
  {
    // Print the grid to the Serial Monitor
    printGrid();

    //Debugging current position of robots
    checkCurrentPositions();

    // Check For neighbourings for walls and update map
    checkNeighbours(robotOne.pos);
    checkNeighbours(robotTwo.pos);

    // Decide and send next command to robots on where to move
    sendCommand(robotOne, GetNextPosition(robotOne));
    robotOne.pos = GetNextPosition(robotOne);

    sendCommand(robotTwo, GetNextPosition(robotTwo));
    robotTwo.pos = GetNextPosition(robotTwo);

    // Add a short delay to wait for robots to move
    delay(1000);

    if(checkIfMapped())
    {
      //Send data to map visualization
      
      // Print the grid to the Serial Monitor
      printGrid();

      //Debugging current position of robots
      checkCurrentPositions();
      //exit
      Serial.println();
      Serial.print("Explored Map");
      exploring = false;
    }
  }
}
