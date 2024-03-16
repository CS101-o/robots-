import pygame
import serial

# Initialize Pygame
pygame.init()
window_size = (800, 600)
screen = pygame.display.set_mode(window_size)
pygame.display.set_caption("Map Visualization")

# Set up serial communication
ser = serial.Serial('/dev/cu.usbserial-DN05JRN6', 9600)

# Define colors
BLACK = (0, 0, 0)
WHITE = (255, 255, 255)

# Define map dimensions and cell size
map_width = 20
map_height = 20
cell_size = 30

# Initialize an empty map
map_data = [[0] * map_width for _ in range(map_height)]

# Main game loop
running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    # Read map data from Arduino
    if ser.in_waiting > 0:
        data = ser.readline().decode('utf-8').rstrip()
        values = data.split(',')  # Assuming comma-separated values
        if len(values) == 2:
            row = int(values[0])
            col = int(values[1])
            if 0 <= row < map_height and 0 <= col < map_width:
                map_data[row][col] = 1

    # Clear the screen
    screen.fill(WHITE)

    # Draw the map
    for row in range(map_height):
        for col in range(map_width):
            if map_data[row][col] == 1:
                pygame.draw.rect(screen, BLACK, (col * cell_size, row * cell_size, cell_size, cell_size))

    # Update the display
    pygame.display.flip()

# Quit Pygame
pygame.quit()