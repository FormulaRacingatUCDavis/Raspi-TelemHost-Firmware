import pygame
import sys
import math

# Initialize Pygame
pygame.init()

# Colors
BLACK = (0, 0, 0)
WHITE = (255, 255, 255)
BLUE = (0, 0, 255)
YELLOW = (255, 255, 0)
RED = (255, 0, 0)
PINK = (255, 192, 203)
ORANGE = (255, 165, 0)
CYAN = (0, 255, 255)
GREEN = (0, 255, 0)
GRAY = (128, 128, 128)

# Game dimensions
CELL_SIZE = 20
MAZE_WIDTH = 19
MAZE_HEIGHT = 21
SCREEN_WIDTH = MAZE_WIDTH * CELL_SIZE
SCREEN_HEIGHT = MAZE_HEIGHT * CELL_SIZE

# Create the screen
screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
pygame.display.set_caption("Pacman")

# Clock for controlling frame rate
clock = pygame.time.Clock()

# Classic Pacman maze layout (1 = wall, 0 = empty space, 2 = dot, 3 = power pellet)
MAZE_LAYOUT = [
    [1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1],
    [1,2,2,2,2,2,2,2,2,1,2,2,2,2,2,2,2,2,1],
    [1,2,1,1,2,1,1,1,2,1,2,1,1,1,2,1,1,2,1],
    [1,3,1,1,2,1,1,1,2,1,2,1,1,1,2,1,1,3,1],
    [1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1],
    [1,2,1,1,2,1,2,1,1,1,1,1,2,1,2,1,1,2,1],
    [1,2,2,2,2,1,2,2,2,1,2,2,2,1,2,2,2,2,1],
    [1,1,1,1,2,1,1,1,0,1,0,1,1,1,2,1,1,1,1],
    [0,0,0,1,2,1,0,0,0,0,0,0,0,1,2,1,0,0,0],
    [1,1,1,1,2,1,0,1,1,0,1,1,0,1,2,1,1,1,1],
    [0,0,0,0,2,0,0,1,0,0,0,1,0,0,2,0,0,0,0],
    [1,1,1,1,2,1,0,1,1,1,1,1,0,1,2,1,1,1,1],
    [0,0,0,1,2,1,0,0,0,0,0,0,0,1,2,1,0,0,0],
    [1,1,1,1,2,1,0,1,1,1,1,1,0,1,2,1,1,1,1],
    [1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1],
    [1,2,1,1,2,1,1,1,2,1,2,1,1,1,2,1,1,2,1],
    [1,2,2,2,2,1,2,2,2,1,2,2,2,1,2,2,2,2,1],
    [1,1,2,1,1,1,2,1,1,1,1,1,2,1,1,1,2,1,1],
    [1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1],
    [1,3,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,3,1],
    [1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1]
]

class Pacman:
    def __init__(self, x, y):
        self.x = x
        self.y = y
        self.direction = 0  # 0=right, 1=down, 2=left, 3=up
        self.next_direction = 0
        self.speed = 2
        
    def update(self):
        # Check for direction change
        keys = pygame.key.get_pressed()
        if keys[pygame.K_RIGHT]:
            self.next_direction = 0
        elif keys[pygame.K_DOWN]:
            self.next_direction = 1
        elif keys[pygame.K_LEFT]:
            self.next_direction = 2
        elif keys[pygame.K_UP]:
            self.next_direction = 3
            
        # Rule 1: Prevent turning in opposite direction
        # Opposite directions: 0↔2 (right↔left), 1↔3 (down↔up)
        opposite_directions = {0: 2, 2: 0, 1: 3, 3: 1}
        if self.next_direction != opposite_directions.get(self.direction, -1):
            # Rule 2: Only change direction if the new direction is valid (not into a wall)
            if self.can_move(self.next_direction):
                self.direction = self.next_direction
            
        # Move in current direction
        if self.can_move(self.direction):
            if self.direction == 0:  # Right
                self.x += self.speed
            elif self.direction == 1:  # Down
                self.y += self.speed
            elif self.direction == 2:  # Left
                self.x -= self.speed
            elif self.direction == 3:  # Up
                self.y -= self.speed
                
        # Wrap around screen edges
        if self.x < 0:
            self.x = SCREEN_WIDTH - CELL_SIZE
        elif self.x >= SCREEN_WIDTH:
            self.x = 0
            
    def can_move(self, direction):
        # Predict new position
        next_x = self.x
        next_y = self.y
        if direction == 0:
            next_x += self.speed
        elif direction == 1:
            next_y += self.speed
        elif direction == 2:
            next_x -= self.speed
        elif direction == 3:
            next_y -= self.speed

        # Define Pacman's bounding box (tight collision detection to prevent wall squeezing)
        margin = 1  # minimal margin to prevent edge cases
        left = next_x + margin
        right = next_x + CELL_SIZE - margin
        top = next_y + margin
        bottom = next_y + CELL_SIZE - margin

        # Convert all 4 corners to grid coordinates
        corners = [
            (int(left // CELL_SIZE), int(top // CELL_SIZE)),
            (int(right // CELL_SIZE), int(top // CELL_SIZE)),
            (int(left // CELL_SIZE), int(bottom // CELL_SIZE)),
            (int(right // CELL_SIZE), int(bottom // CELL_SIZE)),
        ]

        # Check if any corner hits a wall
        for gx, gy in corners:
            if gx < 0 or gx >= MAZE_WIDTH or gy < 0 or gy >= MAZE_HEIGHT:
                return True  # allow wrapping
            if MAZE_LAYOUT[gy][gx] == 1:
                return False

        return True


    def draw(self):
        # Draw Pacman as a yellow circle with mouth
        center_x = self.x + CELL_SIZE // 2
        center_y = self.y + CELL_SIZE // 2
        radius = CELL_SIZE // 2 - 2
        
        # Calculate mouth angle based on direction
        mouth_angle = 45  # degrees
        if self.direction == 0:  # Right
            start_angle = 0
        elif self.direction == 1:  # Down
            start_angle = 90
        elif self.direction == 2:  # Left
            start_angle = 180
        else:  # Up
            start_angle = 270
            
        # Draw Pacman
        pygame.draw.circle(screen, YELLOW, (center_x, center_y), radius)
        # Draw mouth (black triangle)
        mouth_points = [
            (center_x, center_y),
            (center_x + radius * math.cos(math.radians(start_angle)), 
             center_y + radius * math.sin(math.radians(start_angle))),
            (center_x + radius * math.cos(math.radians(start_angle + mouth_angle)), 
             center_y + radius * math.sin(math.radians(start_angle + mouth_angle)))
        ]
        pygame.draw.polygon(screen, BLACK, mouth_points)

def draw_maze():
    for y in range(MAZE_HEIGHT):
        for x in range(MAZE_WIDTH):
            cell_x = x * CELL_SIZE
            cell_y = y * CELL_SIZE
            
            if MAZE_LAYOUT[y][x] == 1:  # Wall
                pygame.draw.rect(screen, BLUE, (cell_x, cell_y, CELL_SIZE, CELL_SIZE))
            elif MAZE_LAYOUT[y][x] == 2:  # Dot
                center_x = cell_x + CELL_SIZE // 2
                center_y = cell_y + CELL_SIZE // 2
                pygame.draw.circle(screen, WHITE, (center_x, center_y), 2)
            elif MAZE_LAYOUT[y][x] == 3:  # Power pellet
                center_x = cell_x + CELL_SIZE // 2
                center_y = cell_y + CELL_SIZE // 2
                pygame.draw.circle(screen, WHITE, (center_x, center_y), 6)

def main():
    # Create Pacman at starting position
    pacman = Pacman(CELL_SIZE, CELL_SIZE)
    
    running = True
    while running:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
                
        # Update game objects
        pacman.update()
        
        # Draw everything
        screen.fill(BLACK)
        draw_maze()
        pacman.draw()
        
        pygame.display.flip()
        clock.tick(60)
        
    pygame.quit()
    sys.exit()

if __name__ == "__main__":
    main()