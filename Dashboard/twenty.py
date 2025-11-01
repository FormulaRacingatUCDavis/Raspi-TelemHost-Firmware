import pygame
import random
import sys

# Initialize Pygame
pygame.init()

# Constants
WINDOW_SIZE = 500
GRID_SIZE = 4
CELL_SIZE = WINDOW_SIZE // GRID_SIZE
PADDING = 10
ANIMATION_SPEED = 15
ANIMATION_FRAMES = 10  # Number of frames for animations

# Colors
BACKGROUND_COLOR = (187, 173, 160)
GRID_COLOR = (205, 193, 180)
EMPTY_CELL_COLOR = (205, 193, 180)
CELL_COLOR = (205, 193, 180)
TEXT_COLOR = (119, 110, 101)
OVERLAY_COLOR = (238, 228, 218, 0.73)  # Semi-transparent white
GAME_OVER_COLOR = (119, 110, 101)
TILE_COLORS = {
    2: (238, 228, 218),
    4: (237, 224, 200),
    8: (242, 177, 121),
    16: (245, 149, 99),
    32: (246, 124, 95),
    64: (246, 94, 59),
    128: (237, 207, 114),
    256: (237, 204, 97),
    512: (237, 200, 80),
    1024: (237, 197, 63),
    2048: (237, 194, 46),
    4096: (237, 190, 30),
    8192: (237, 187, 13)
}

class Animation:
    def __init__(self, start_pos, end_pos, value, is_merge=False):
        self.start_pos = start_pos
        self.end_pos = end_pos
        self.value = value
        self.is_merge = is_merge
        self.progress = 0
        self.completed = False

    def update(self):
        self.progress += 1 / ANIMATION_FRAMES
        if self.progress >= 1:
            self.progress = 1
            self.completed = True

    def get_current_pos(self):
        if self.is_merge:
            # For merges, we want a pop effect
            scale = 1 + 0.1 * (1 - abs(2 * self.progress - 1))
            return self.end_pos, scale
        else:
            # For movements, we want smooth easing
            # Using easeOutQuad for smoother movement
            t = self.progress
            t = 1 - (1 - t) * (1 - t)  # easeOutQuad
            x = self.start_pos[0] + (self.end_pos[0] - self.start_pos[0]) * t
            y = self.start_pos[1] + (self.end_pos[1] - self.start_pos[1]) * t
            return (x, y), 1

class Game2048:
    def __init__(self):
        self.screen = pygame.display.set_mode((WINDOW_SIZE, WINDOW_SIZE))
        pygame.display.set_caption("2048")
        self.clock = pygame.time.Clock()
        self.font = pygame.font.Font(None, 55)
        self.small_font = pygame.font.Font(None, 35)
        self.big_font = pygame.font.Font(None, 72)
        self.animations = []
        self.animation_in_progress = False
        self.game_over = False
        self.reset_game()

    def reset_game(self):
        self.grid = [[0] * GRID_SIZE for _ in range(GRID_SIZE)]
        self.score = 0
        self.moved = False
        self.animations = []
        self.animation_in_progress = False
        self.game_over = False
        self.add_new_tile()
        self.add_new_tile()

    def add_new_tile(self):
        empty_cells = [(i, j) for i in range(GRID_SIZE) for j in range(GRID_SIZE) if self.grid[i][j] == 0]
        if empty_cells:
            i, j = random.choice(empty_cells)
            self.grid[i][j] = 2 if random.random() < 0.9 else 4
            # Add pop-in animation for new tile
            self.animations.append(Animation((j, i), (j, i), self.grid[i][j], True))

    def move(self, direction):
        if self.animation_in_progress:
            return

        # Store the old grid for comparison
        old_grid = [row[:] for row in self.grid]
        
        self.moved = False
        self.animations = []
        self.animation_in_progress = True
        
        if direction in ['UP', 'DOWN']:
            for j in range(GRID_SIZE):
                # Get the column and track original positions
                column = []
                original_positions = []
                for i in range(GRID_SIZE):
                    if self.grid[i][j] != 0:
                        column.append(self.grid[i][j])
                        original_positions.append(i)
                
                if direction == 'DOWN':
                    column.reverse()
                    original_positions = [GRID_SIZE - 1 - pos for pos in original_positions]
                
                # Merge tiles
                new_column = []
                merged = [False] * len(column)
                
                i = 0
                while i < len(column):
                    if i + 1 < len(column) and column[i] == column[i + 1] and not merged[i]:
                        new_column.append(column[i] * 2)
                        self.score += column[i] * 2  # Add score for merge
                        merged[i] = True
                        i += 2
                    else:
                        new_column.append(column[i])
                        i += 1
                
                # Pad with zeros
                new_column = new_column + [0] * (GRID_SIZE - len(new_column))
                
                if direction == 'DOWN':
                    new_column.reverse()
                
                # Check if the column actually changed
                column_changed = False
                for i in range(GRID_SIZE):
                    if self.grid[i][j] != new_column[i]:
                        column_changed = True
                        break
                
                if column_changed:
                    self.moved = True
                    # Create animations for each tile
                    current_pos = 0
                    for i in range(GRID_SIZE):
                        if new_column[i] != 0:
                            # Find the original position
                            start_i = original_positions[current_pos] if current_pos < len(original_positions) else i
                            if direction == 'DOWN':
                                start_i = GRID_SIZE - 1 - start_i
                            
                            # Create animation
                            self.animations.append(Animation(
                                (j, start_i), (j, i), new_column[i],
                                new_column[i] != old_grid[start_i][j]
                            ))
                            current_pos += 1
                
                # Update the grid
                for i in range(GRID_SIZE):
                    self.grid[i][j] = new_column[i]
        
        elif direction in ['LEFT', 'RIGHT']:
            for i in range(GRID_SIZE):
                # Get the row and track original positions
                row = []
                original_positions = []
                for j in range(GRID_SIZE):
                    if self.grid[i][j] != 0:
                        row.append(self.grid[i][j])
                        original_positions.append(j)
                
                if direction == 'RIGHT':
                    row.reverse()
                    original_positions = [GRID_SIZE - 1 - pos for pos in original_positions]
                
                # Merge tiles
                new_row = []
                merged = [False] * len(row)
                
                j = 0
                while j < len(row):
                    if j + 1 < len(row) and row[j] == row[j + 1] and not merged[j]:
                        new_row.append(row[j] * 2)
                        self.score += row[j] * 2  # Add score for merge
                        merged[j] = True
                        j += 2
                    else:
                        new_row.append(row[j])
                        j += 1
                
                # Pad with zeros
                new_row = new_row + [0] * (GRID_SIZE - len(new_row))
                
                if direction == 'RIGHT':
                    new_row.reverse()
                
                # Check if the row actually changed
                row_changed = False
                for j in range(GRID_SIZE):
                    if self.grid[i][j] != new_row[j]:
                        row_changed = True
                        break
                
                if row_changed:
                    self.moved = True
                    # Create animations for each tile
                    current_pos = 0
                    for j in range(GRID_SIZE):
                        if new_row[j] != 0:
                            # Find the original position
                            start_j = original_positions[current_pos] if current_pos < len(original_positions) else j
                            if direction == 'RIGHT':
                                start_j = GRID_SIZE - 1 - start_j
                            
                            # Create animation
                            self.animations.append(Animation(
                                (start_j, i), (j, i), new_row[j],
                                new_row[j] != old_grid[i][start_j]
                            ))
                            current_pos += 1
                
                # Update the grid
                self.grid[i] = new_row

    def merge_tiles(self, line):
        score = 0
        animations = []
        
        # First compact the line by removing zeros
        compact_line = [x for x in line if x != 0]
        
        # Then merge adjacent tiles
        merged_line = []
        i = 0
        while i < len(compact_line):
            if i + 1 < len(compact_line) and compact_line[i] == compact_line[i + 1]:
                merged_value = compact_line[i] * 2
                score += merged_value
                merged_line.append(merged_value)
                animations.append((len(merged_line) - 1, len(merged_line) - 1, merged_value))
                i += 2
            else:
                merged_line.append(compact_line[i])
                animations.append((len(merged_line) - 1, len(merged_line) - 1, compact_line[i]))
                i += 1
        
        # Pad with zeros to maintain grid size
        new_line = merged_line + [0] * (GRID_SIZE - len(merged_line))
        
        return new_line, score, animations

    def is_game_over(self):
        # Check for empty cells
        if any(0 in row for row in self.grid):
            return False
            
        # Check for possible merges
        for i in range(GRID_SIZE):
            for j in range(GRID_SIZE):
                current = self.grid[i][j]
                # Check right neighbor
                if j < GRID_SIZE - 1 and current == self.grid[i][j + 1]:
                    return False
                # Check bottom neighbor
                if i < GRID_SIZE - 1 and current == self.grid[i + 1][j]:
                    return False
                # Check left neighbor
                if j > 0 and current == self.grid[i][j - 1]:
                    return False
                # Check top neighbor
                if i > 0 and current == self.grid[i - 1][j]:
                    return False
        return True

    def has_valid_move(self, direction):
        # Create a copy of the grid to test the move
        test_grid = [row[:] for row in self.grid]
        
        if direction in ['UP', 'DOWN']:
            for j in range(GRID_SIZE):
                # Get the column
                column = [test_grid[i][j] for i in range(GRID_SIZE)]
                if direction == 'DOWN':
                    column.reverse()
                
                # Check if there are any non-zero tiles
                non_zero = [x for x in column if x != 0]
                if not non_zero:
                    continue
                
                # Check if tiles can move (there are zeros before them)
                if direction == 'UP':
                    if column[0] == 0:
                        return True
                else:  # DOWN
                    if column[-1] == 0:
                        return True
                
                # Check for possible merges
                for i in range(len(non_zero) - 1):
                    if non_zero[i] == non_zero[i + 1]:
                        return True
        
        elif direction in ['LEFT', 'RIGHT']:
            for i in range(GRID_SIZE):
                # Get the row
                row = test_grid[i].copy()
                if direction == 'RIGHT':
                    row.reverse()
                
                # Check if there are any non-zero tiles
                non_zero = [x for x in row if x != 0]
                if not non_zero:
                    continue
                
                # Check if tiles can move (there are zeros before them)
                if direction == 'LEFT':
                    if row[0] == 0:
                        return True
                else:  # RIGHT
                    if row[-1] == 0:
                        return True
                
                # Check for possible merges
                for j in range(len(non_zero) - 1):
                    if non_zero[j] == non_zero[j + 1]:
                        return True
        
        return False

    def check_game_over(self):
        # First check for empty cells
        if any(0 in row for row in self.grid):
            return False
        
        # Check each direction independently
        for direction in ['UP', 'DOWN', 'LEFT', 'RIGHT']:
            if self.has_valid_move(direction):
                return False
        
        return True

    def draw_game_over(self):
        # Create a semi-transparent overlay
        overlay = pygame.Surface((WINDOW_SIZE, WINDOW_SIZE), pygame.SRCALPHA)
        overlay.fill(OVERLAY_COLOR)
        self.screen.blit(overlay, (0, 0))
        
        # Draw "Game Over" text
        game_over_text = self.big_font.render("Game Over!", True, GAME_OVER_COLOR)
        text_rect = game_over_text.get_rect(center=(WINDOW_SIZE/2, WINDOW_SIZE/2 - 50))
        self.screen.blit(game_over_text, text_rect)
        
        # Draw "You Lose" text
        lose_text = self.font.render("You Lose!", True, GAME_OVER_COLOR)
        lose_rect = lose_text.get_rect(center=(WINDOW_SIZE/2, WINDOW_SIZE/2))
        self.screen.blit(lose_text, lose_rect)
        
        # Draw "Press R to restart" text
        restart_text = self.font.render("Press R to restart", True, GAME_OVER_COLOR)
        restart_rect = restart_text.get_rect(center=(WINDOW_SIZE/2, WINDOW_SIZE/2 + 50))
        self.screen.blit(restart_text, restart_rect)

    def draw(self):
        self.screen.fill(BACKGROUND_COLOR)
        
        # Draw the grid
        for i in range(GRID_SIZE):
            for j in range(GRID_SIZE):
                x = j * CELL_SIZE + PADDING
                y = i * CELL_SIZE + PADDING
                
                # Draw cell background
                pygame.draw.rect(
                    self.screen,
                    CELL_COLOR,
                    (x, y, CELL_SIZE - PADDING, CELL_SIZE - PADDING),
                    border_radius=5
                )
                
                # Draw tile if it exists
                value = self.grid[i][j]
                if value != 0:
                    # Draw tile background
                    color = TILE_COLORS.get(value, TILE_COLORS[2048])
                    pygame.draw.rect(
                        self.screen,
                        color,
                        (x, y, CELL_SIZE - PADDING, CELL_SIZE - PADDING),
                        border_radius=5
                    )
                    
                    # Draw tile value
                    if value < 100:
                        font = self.font
                    else:
                        font = self.small_font
                    
                    text = font.render(str(value), True, TEXT_COLOR)
                    text_rect = text.get_rect(center=(x + (CELL_SIZE - PADDING) // 2,
                                                     y + (CELL_SIZE - PADDING) // 2))
                    self.screen.blit(text, text_rect)
        
        # Draw score
        score_text = self.font.render(f"Score: {self.score}", True, TEXT_COLOR)
        self.screen.blit(score_text, (PADDING, WINDOW_SIZE - 50))
        
        # Draw quit button
        quit_button = pygame.Rect(WINDOW_SIZE - 100, WINDOW_SIZE - 50, 80, 30)
        pygame.draw.rect(self.screen, (255, 0, 0), quit_button, border_radius=5)
        quit_text = self.small_font.render("Quit", True, TEXT_COLOR)
        quit_text_rect = quit_text.get_rect(center=quit_button.center)
        self.screen.blit(quit_text, quit_text_rect)
        
        # Draw game over screen if game is over
        if self.game_over:
            self.draw_game_over()
        
        pygame.display.flip()

    def run(self):
        while True:
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    pygame.quit()
                    sys.exit()
                
                if event.type == pygame.MOUSEBUTTONDOWN:
                    # Check if quit button was clicked
                    mouse_pos = pygame.mouse.get_pos()
                    quit_button = pygame.Rect(WINDOW_SIZE - 100, WINDOW_SIZE - 50, 80, 30)
                    if quit_button.collidepoint(mouse_pos):
                        return  # Return to main menu
                
                if event.type == pygame.KEYDOWN:
                    # Check for quit shortcuts (P or ESC)
                    if event.key in [pygame.K_p, pygame.K_ESCAPE]:
                        return  # Return to main menu
                        
                    if not self.game_over:
                        if event.key == pygame.K_UP:
                            self.move('UP')
                        elif event.key == pygame.K_DOWN:
                            self.move('DOWN')
                        elif event.key == pygame.K_LEFT:
                            self.move('LEFT')
                        elif event.key == pygame.K_RIGHT:
                            self.move('RIGHT')
                    
                    if event.key == pygame.K_r:
                        self.reset_game()
            
            # Update animations
            if self.animations:
                all_completed = True
                for animation in self.animations:
                    animation.update()
                    if not animation.completed:
                        all_completed = False
                
                if all_completed:
                    self.animations = []
                    self.animation_in_progress = False
                    if self.moved:
                        self.add_new_tile()
                        self.moved = False
                        if self.is_game_over():
                            self.game_over = True
            else:
                # Always ensure animation_in_progress is False when no animations are present
                self.animation_in_progress = False
            
            self.draw()
            self.clock.tick(60)

if __name__ == "__main__":
    game = Game2048()
    game.run()