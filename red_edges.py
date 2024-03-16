import pygame
import time

# Initialize Pygame
pygame.init()
window_size = (800, 600)
screen = pygame.display.set_mode(window_size)
pygame.display.set_caption("Map Visualization Demo")

# Define colors
BLACK = (0, 0, 0)
WHITE = (255, 255, 255)
RED = (255, 0, 0)

# Define map dimensions and cell size
map_width = 20
map_height = 20
cell_size = 30

# Initialize an empty map
map_data = [[0] * map_width for _ in range(map_height)]

# Set the starting position
current_row = map_height // 2
current_col = map_width // 2

# Demo data (made-up binary values for testing)
demo_data = [
    "1,0,0,1",
    "0,1,1,0",
    "1,1,0,0",
    "0,0,1,1",
    "1,0,1,0",
    "0,1,0,1",
    "1,1,1,0",
    "0,0,0,1"
]

# Main game loop
running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    # Simulate receiving data from Arduino (using demo data)
    for data in demo_data:
        values = data.split(',')
        if len(values) == 4:
            south = int(values[0])
            east = int(values[1])
            west = int(values[2])
            north = int(values[3])

            # Update the map based on the block data
            if current_row < map_height - 1:
                map_data[current_row + 1][current_col] = south
            if current_col < map_width - 1:
                map_data[current_row][current_col + 1] = east
            if current_col > 0:
                map_data[current_row][current_col - 1] = west
            if current_row > 0:
                map_data[current_row - 1][current_col] = north

        # Clear the screen
        screen.fill(WHITE)

        # Draw the map
        for row in range(map_height):
            for col in range(map_width):
                if map_data[row][col] == 1:
                    # Draw white tile with red edges for blocked cells
                    pygame.draw.rect(screen, WHITE, (col * cell_size, row * cell_size, cell_size, cell_size))
                    pygame.draw.rect(screen, RED, (col * cell_size, row * cell_size, cell_size, cell_size), 2)
                else:
                    # Draw white tile for empty cells
                    pygame.draw.rect(screen, WHITE, (col * cell_size, row * cell_size, cell_size, cell_size))
                # Draw black border around each cell
                pygame.draw.rect(screen, BLACK, (col * cell_size, row * cell_size, cell_size, cell_size), 1)

        # Draw the current position
        pygame.draw.rect(screen, (255, 0, 0), (current_col * cell_size, current_row * cell_size, cell_size, cell_size))

        # Update the display
        pygame.display.flip()

        # Delay to simulate receiving data periodically
        time.sleep(1)

# Quit Pygame
pygame.quit()