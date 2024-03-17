import pygame
import serial

# Initialize Pygame
pygame.init()
window_size = (1200, 600)  # Increased window width to accommodate two maps
screen = pygame.display.set_mode(window_size)
pygame.display.set_caption("Map Visualization")

# Set up serial communication
ser = serial.Serial('/dev/cu.usbserial-DN05JRN6', 9600)

# Define colors
BLACK = (0, 0, 0)
WHITE = (255, 255, 255)
RED = (255, 0, 0)

# Define map dimensions and cell size
map_width = 20
map_height = 20
cell_size = 30

# Initialize empty maps
map_data_1 = [[0] * map_width for _ in range(map_height)]
map_data_2 = [[0] * map_width for _ in range(map_height)]

# Set the starting positions
current_row_1 = map_height // 2
current_col_1 = map_width // 2
current_row_2 = map_height // 2
current_col_2 = map_width // 2

# Main game loop
running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    # Read data from Arduino
    if ser.in_waiting > 0:
        data = ser.readline().decode('utf-8').rstrip()
        values = data.split(',')  # Assuming comma-separated values
        if len(values) == 5:
            map_id = int(values[0])
            south = int(values[1])
            east = int(values[2])
            north = int(values[3])
            west = int(values[4])

            if map_id == 1:
                # Update map 1 based on the received data
                if current_row_1 < map_height - 1:
                    map_data_1[current_row_1 + 1][current_col_1] = south
                if current_col_1 < map_width - 1:
                    map_data_1[current_row_1][current_col_1 + 1] = east
                if current_row_1 > 0:
                    map_data_1[current_row_1 - 1][current_col_1] = north
                if current_col_1 > 0:
                    map_data_1[current_row_1][current_col_1 - 1] = west
            elif map_id == 2:
                # Update map 2 based on the received data
                if current_row_2 < map_height - 1:
                    map_data_2[current_row_2 + 1][current_col_2] = south
                if current_col_2 < map_width - 1:
                    map_data_2[current_row_2][current_col_2 + 1] = east
                if current_row_2 > 0:
                    map_data_2[current_row_2 - 1][current_col_2] = north
                if current_col_2 > 0:
                    map_data_2[current_row_2][current_col_2 - 1] = west

    # Clear the screen
    screen.fill(WHITE)

    # Draw map 1
    for row in range(map_height):
        for col in range(map_width):
            if map_data_1[row][col] == 1:
                # Draw white tile with red edges for blocked cells
                pygame.draw.rect(screen, WHITE, (col * cell_size, row * cell_size, cell_size, cell_size))
                pygame.draw.rect(screen, RED, (col * cell_size, row * cell_size, cell_size, cell_size), 2)
            else:
                # Draw white tile for empty cells
                pygame.draw.rect(screen, WHITE, (col * cell_size, row * cell_size, cell_size, cell_size))
            # Draw black border around each cell
            pygame.draw.rect(screen, BLACK, (col * cell_size, row * cell_size, cell_size, cell_size), 1)

    # Draw map 2
    for row in range(map_height):
        for col in range(map_width):
            if map_data_2[row][col] == 1:
                # Draw white tile with red edges for blocked cells
                pygame.draw.rect(screen, WHITE, (col * cell_size + 600, row * cell_size, cell_size, cell_size))
                pygame.draw.rect(screen, RED, (col * cell_size + 600, row * cell_size, cell_size, cell_size), 2)
            else:
                # Draw white tile for empty cells
                pygame.draw.rect(screen, WHITE, (col * cell_size + 600, row * cell_size, cell_size, cell_size))
            # Draw black border around each cell
            pygame.draw.rect(screen, BLACK, (col * cell_size + 600, row * cell_size, cell_size, cell_size), 1)

    # Update the display
    pygame.display.flip()

# Quit Pygame
pygame.quit()