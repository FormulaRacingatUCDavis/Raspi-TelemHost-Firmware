import pygame
import sys
import random
import time

# Initialize pygame
pygame.init()

# Colors
BLACK = (0, 0, 0)
WHITE = (255, 255, 255)
CYAN = (0, 255, 255)
YELLOW = (255, 255, 0)
MAGENTA = (255, 0, 255)
GREEN = (0, 255, 0)
RED = (255, 0, 0)
BLUE = (0, 0, 255)
ORANGE = (255, 165, 0)
GRAY = (128, 128, 128)

# Game dimensions
BOARD_WIDTH = 10
BOARD_HEIGHT = 20
BLOCK_SIZE = 30
BOARD_OFFSET_X = 150  # Shifted right to make room for hold box
BOARD_OFFSET_Y = 50

# Screen dimensions
SCREEN_WIDTH = 750
SCREEN_HEIGHT = 700

# Tetris pieces
TETROMINO_I = [
    [0, 0, 0, 0],
    [1, 1, 1, 1],
    [0, 0, 0, 0],
    [0, 0, 0, 0]
]

TETROMINO_O = [
    [0, 0, 0, 0],
    [0, 1, 1, 0],
    [0, 1, 1, 0],
    [0, 0, 0, 0]
]

TETROMINO_T = [
    [0, 0, 0, 0],
    [0, 1, 0, 0],
    [1, 1, 1, 0],
    [0, 0, 0, 0]
]

TETROMINO_S = [
    [0, 0, 0, 0],
    [0, 1, 1, 0],
    [1, 1, 0, 0],
    [0, 0, 0, 0]
]

TETROMINO_Z = [
    [0, 0, 0, 0],
    [1, 1, 0, 0],
    [0, 1, 1, 0],
    [0, 0, 0, 0]
]

TETROMINO_J = [
    [0, 0, 0, 0],
    [1, 0, 0, 0],
    [1, 1, 1, 0],
    [0, 0, 0, 0]
]

TETROMINO_L = [
    [0, 0, 0, 0],
    [0, 0, 1, 0],
    [1, 1, 1, 0],
    [0, 0, 0, 0]
]

TETROMINOS = [
    TETROMINO_I,
    TETROMINO_O,
    TETROMINO_T,
    TETROMINO_S,
    TETROMINO_Z,
    TETROMINO_J,
    TETROMINO_L
]

TETROMINO_COLORS = [
    CYAN,
    YELLOW,
    MAGENTA,
    GREEN,
    RED,
    BLUE,
    ORANGE
]

class TetrisGame:
    def __init__(self):
        # Initialize screen
        self.screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
        pygame.display.set_caption("Tetris Simulator")
        
        # Initialize game state
        self.reset_game()
        
        # Font for text
        self.font = pygame.font.SysFont('Arial', 24)
        self.small_font = pygame.font.SysFont('Arial', 18)
        
        # Key tracking
        self.r_key_pressed = False
        self.fast_drop_active = False
        
    def reset_game(self):
        # Create empty board
        self.board = [[0 for _ in range(BOARD_WIDTH)] for _ in range(BOARD_HEIGHT)]
        
        # Game state
        self.score = 0
        self.game_over = False
        self.fall_delay = 500  # milliseconds
        self.fast_fall_delay = 50  # milliseconds for fast drop
        self.last_fall_time = pygame.time.get_ticks()
        self.last_button_time = 0
        self.button_delay = 100  # milliseconds
        self.hold_used = False  # Track if hold was used this turn
        self.hold_piece = None  # Piece in hold
        
        # Create piece queue (next 3 pieces)
        self.piece_queue = []
        self.refill_piece_queue()
        
        # Create new piece
        self.create_new_piece()
        
    def refill_piece_queue(self):
        # Fill the piece queue to have at least 3 pieces
        while len(self.piece_queue) < 3:
            self.piece_queue.append(random.randint(0, 6))
        
    def create_new_piece(self):
        # Take the next piece from the queue
        self.current_piece_type = self.piece_queue.pop(0)
        self.refill_piece_queue()  # Make sure queue stays filled
        
        self.current_piece_rotation = 0
        self.current_piece_x = BOARD_WIDTH // 2 - 2
        self.current_piece_y = 0
        
        # Reset hold usage for the new piece
        self.hold_used = False
        
    def hold_piece_action(self):
        # Don't allow holding twice in one turn
        if self.hold_used:
            return
            
        if self.hold_piece is None:
            # Store current piece and get a new one
            self.hold_piece = self.current_piece_type
            self.create_new_piece()
        else:
            # Swap current piece with held piece
            temp = self.current_piece_type
            self.current_piece_type = self.hold_piece
            self.hold_piece = temp
            
            # Reset position for the newly activated piece
            self.current_piece_rotation = 0
            self.current_piece_x = BOARD_WIDTH // 2 - 2
            self.current_piece_y = 0
            
        # Mark hold as used for this turn
        self.hold_used = True
        
    def get_rotated_piece(self, piece_type=None, rotation=None):
        # If no specific piece/rotation provided, use current piece
        if piece_type is None:
            piece_type = self.current_piece_type
        if rotation is None:
            rotation = self.current_piece_rotation
            
        # Make a copy of the original piece
        piece = [row[:] for row in TETROMINOS[piece_type]]
        
        # Apply rotation
        for _ in range(rotation):
            piece = self.rotate_matrix(piece)
            
        return piece
    
    def rotate_matrix(self, matrix):
        # Create a new rotated matrix
        rotated = [[0 for _ in range(4)] for _ in range(4)]
        
        # Rotate 90 degrees clockwise
        for y in range(4):
            for x in range(4):
                rotated[y][x] = matrix[3 - x][y]
                
        return rotated
    
    def check_collision(self, piece_x=None, piece_y=None, piece=None):
        if piece_x is None:
            piece_x = self.current_piece_x
        if piece_y is None:
            piece_y = self.current_piece_y
        if piece is None:
            piece = self.get_rotated_piece()
        
        for y in range(4):
            for x in range(4):
                if piece[y][x]:
                    board_x = piece_x + x
                    board_y = piece_y + y
                    
                    # Check boundaries
                    if (board_x < 0 or board_x >= BOARD_WIDTH or 
                        board_y >= BOARD_HEIGHT):
                        return True
                    
                    # Check collision with placed pieces
                    if board_y >= 0 and self.board[board_y][board_x]:
                        return True
        
        return False
    
    def get_ghost_position(self):
        piece = self.get_rotated_piece()
        ghost_y = self.current_piece_y
        
        # Move the ghost piece down until it collides
        while not self.check_collision(self.current_piece_x, ghost_y + 1, piece):
            ghost_y += 1
            
        return self.current_piece_x, ghost_y
    
    def place_piece(self):
        piece = self.get_rotated_piece()
        
        for y in range(4):
            for x in range(4):
                if piece[y][x]:
                    board_x = self.current_piece_x + x
                    board_y = self.current_piece_y + y
                    
                    if (0 <= board_x < BOARD_WIDTH and 
                        0 <= board_y < BOARD_HEIGHT):
                        self.board[board_y][board_x] = self.current_piece_type + 1
        
        # Add points for placing a piece
        self.score += 10
        
    def rotate_piece(self):
        old_rotation = self.current_piece_rotation
        
        # Try to rotate
        self.current_piece_rotation = (self.current_piece_rotation + 1) % 4
        
        # Check if the rotation causes a collision
        if self.check_collision():
            # Try wall kicks
            for kick in range(-2, 3):
                if kick == 0:
                    continue
                
                old_x = self.current_piece_x
                self.current_piece_x += kick
                
                if not self.check_collision():
                    return
                
                self.current_piece_x = old_x
            
            # If all wall kicks failed, revert rotation
            self.current_piece_rotation = old_rotation
    
    def hard_drop(self):
        """Instantly drop the piece to the bottom"""
        # Find the landing position
        while not self.check_collision(self.current_piece_x, self.current_piece_y + 1):
            self.current_piece_y += 1
        
        # Place the piece and create a new one
        self.place_piece()
        self.clear_lines()
        self.create_new_piece()
        
        # Check for game over
        if self.check_collision():
            self.game_over_state()
    
    def clear_lines(self):
        lines_cleared = 0
        
        y = BOARD_HEIGHT - 1
        while y >= 0:
            line_complete = True
            
            for x in range(BOARD_WIDTH):
                if self.board[y][x] == 0:
                    line_complete = False
                    break
            
            if line_complete:
                lines_cleared += 1
                
                # Move all lines above down
                for move_y in range(y, 0, -1):
                    for x in range(BOARD_WIDTH):
                        self.board[move_y][x] = self.board[move_y - 1][x]
                
                # Clear the top line
                for x in range(BOARD_WIDTH):
                    self.board[0][x] = 0
                    
                # Don't increment y as we need to check the same line again
            else:
                y -= 1
        
        # Award points for cleared lines
        if lines_cleared > 0:
            points = 0
            if lines_cleared == 1:
                points = 100
            elif lines_cleared == 2:
                points = 300
            elif lines_cleared == 3:
                points = 500
            elif lines_cleared == 4:
                points = 800  # Tetris!
            
            self.score += points
            
            # Increase game speed every 1000 points
            if (self.score // 1000) > ((self.score - points) // 1000):
                self.fall_delay = max(100, self.fall_delay - 50)
    
    def draw_piece_preview(self, piece_type, x, y, size=15):
        """Draw a small piece preview that is properly centered"""
        piece = self.get_rotated_piece(piece_type, 0)  # Always show default rotation
        color = TETROMINO_COLORS[piece_type]
        
        # Find active cells in the piece
        active_cells = []
        for py in range(4):
            for px in range(4):
                if piece[py][px]:
                    active_cells.append((px, py))
        
        if not active_cells:
            return
            
        # Calculate bounds
        min_x = min(cell[0] for cell in active_cells)
        max_x = max(cell[0] for cell in active_cells)
        min_y = min(cell[1] for cell in active_cells)
        max_y = max(cell[1] for cell in active_cells)
        
        width = max_x - min_x + 1
        height = max_y - min_y + 1
        
        # Calculate centering offsets
        offset_x = (4 - width) * size // 2
        offset_y = (4 - height) * size // 2
        
        # Draw the piece
        for px, py in active_cells:
            # Center the piece in the preview area
            draw_x = x + (px - min_x) * size + (4 - width) * size // 2
            draw_y = y + (py - min_y) * size + (4 - height) * size // 2
            
            pygame.draw.rect(
                self.screen,
                color,
                (draw_x, draw_y, size, size)
            )
            pygame.draw.rect(
                self.screen,
                BLACK,
                (draw_x, draw_y, size, size),
                1
            )
    
    def draw_board(self):
        # Clear the screen
        self.screen.fill(BLACK)
        
        # Draw board border
        pygame.draw.rect(
            self.screen, 
            WHITE, 
            (BOARD_OFFSET_X - 1, BOARD_OFFSET_Y - 1, 
             BOARD_WIDTH * BLOCK_SIZE + 2, BOARD_HEIGHT * BLOCK_SIZE + 2), 
            1
        )
        
        # Draw placed blocks
        for y in range(BOARD_HEIGHT):
            for x in range(BOARD_WIDTH):
                if self.board[y][x]:
                    color = TETROMINO_COLORS[self.board[y][x] - 1]
                    
                    pygame.draw.rect(
                        self.screen,
                        color,
                        (BOARD_OFFSET_X + x * BLOCK_SIZE,
                         BOARD_OFFSET_Y + y * BLOCK_SIZE,
                         BLOCK_SIZE, BLOCK_SIZE)
                    )
                    
                    pygame.draw.rect(
                        self.screen,
                        BLACK,
                        (BOARD_OFFSET_X + x * BLOCK_SIZE,
                         BOARD_OFFSET_Y + y * BLOCK_SIZE,
                         BLOCK_SIZE, BLOCK_SIZE),
                        1
                    )
        
        # Draw ghost piece (where current piece will land)
        if not self.game_over:
            piece = self.get_rotated_piece()
            ghost_x, ghost_y = self.get_ghost_position()
            
            # Only draw ghost if it's not overlapping with the current piece
            if ghost_y > self.current_piece_y:
                color = TETROMINO_COLORS[self.current_piece_type]
                
                for y in range(4):
                    for x in range(4):
                        if piece[y][x]:
                            board_x = ghost_x + x
                            board_y = ghost_y + y
                            
                            if (0 <= board_x < BOARD_WIDTH and 
                                0 <= board_y < BOARD_HEIGHT):
                                # Draw with outlines
                                pygame.draw.rect(
                                    self.screen,
                                    # Use a slightly darker color
                                    (color[0]//2, color[1]//2, color[2]//2),
                                    (BOARD_OFFSET_X + board_x * BLOCK_SIZE,
                                     BOARD_OFFSET_Y + board_y * BLOCK_SIZE,
                                     BLOCK_SIZE, BLOCK_SIZE),
                                    1  # Just draw the outline with a thickness of 1
                                )
                                
                                # Create a semi-transparent surface
                                s = pygame.Surface((BLOCK_SIZE-2, BLOCK_SIZE-2))
                                s.set_alpha(75)  # Alpha level (0-255)
                                s.fill(color)
                                self.screen.blit(s, (BOARD_OFFSET_X + board_x * BLOCK_SIZE + 1,
                                                   BOARD_OFFSET_Y + board_y * BLOCK_SIZE + 1))
        
        # Draw the current piece
        piece = self.get_rotated_piece()
        color = TETROMINO_COLORS[self.current_piece_type]
        
        for y in range(4):
            for x in range(4):
                if piece[y][x]:
                    board_x = self.current_piece_x + x
                    board_y = self.current_piece_y + y
                    
                    if (0 <= board_x < BOARD_WIDTH and 
                        0 <= board_y < BOARD_HEIGHT):
                        pygame.draw.rect(
                            self.screen,
                            color,
                            (BOARD_OFFSET_X + board_x * BLOCK_SIZE,
                             BOARD_OFFSET_Y + board_y * BLOCK_SIZE,
                             BLOCK_SIZE, BLOCK_SIZE)
                        )
                        
                        pygame.draw.rect(
                            self.screen,
                            BLACK,
                            (BOARD_OFFSET_X + board_x * BLOCK_SIZE,
                             BOARD_OFFSET_Y + board_y * BLOCK_SIZE,
                             BLOCK_SIZE, BLOCK_SIZE),
                            1
                        )
        
        # Draw score
        score_text = self.font.render(f"Score: {self.score}", True, WHITE)
        self.screen.blit(score_text, (BOARD_OFFSET_X, 10))
        
        # Draw quit button
        quit_button = pygame.Rect(SCREEN_WIDTH - 100, 10, 80, 30)
        pygame.draw.rect(self.screen, (255, 0, 0), quit_button, border_radius=5)
        quit_text = self.small_font.render("Quit", True, WHITE)
        quit_text_rect = quit_text.get_rect(center=quit_button.center)
        self.screen.blit(quit_text, quit_text_rect)
        
        # Draw hold piece box
        hold_box_x = 20
        hold_box_y = 50
        hold_box_size = 100
        
        pygame.draw.rect(
            self.screen,
            WHITE,
            (hold_box_x, hold_box_y, hold_box_size, hold_box_size),
            1
        )
        
        hold_text = self.small_font.render("HOLD", True, WHITE)
        self.screen.blit(hold_text, (hold_box_x + (hold_box_size - hold_text.get_width()) // 2, 
                                    hold_box_y - 25))
        
        # Draw the held piece if there is one
        if self.hold_piece is not None:
            # Center the held piece in the box
            self.draw_piece_preview(
                self.hold_piece,
                hold_box_x + 10,
                hold_box_y + 10,
                20
            )
            
            # Indicate if hold is used this turn
            if self.hold_used:
                # Draw a semi-transparent gray overlay
                s = pygame.Surface((hold_box_size-2, hold_box_size-2))
                s.set_alpha(100)
                s.fill(GRAY)
                self.screen.blit(s, (hold_box_x+1, hold_box_y+1))
        
        # Draw next pieces box
        next_box_x = BOARD_OFFSET_X + BOARD_WIDTH * BLOCK_SIZE + 20
        next_box_y = 100  # Moved down to avoid overlapping with score
        next_box_width = 120
        next_box_height = 240
        
        pygame.draw.rect(
            self.screen,
            WHITE,
            (next_box_x, next_box_y, next_box_width, next_box_height),
            1
        )
        
        next_text = self.small_font.render("NEXT", True, WHITE)
        self.screen.blit(next_text, (next_box_x + (next_box_width - next_text.get_width()) // 2, 
                                    next_box_y - 25))
        
        # Draw the next 3 pieces
        for i in range(min(3, len(self.piece_queue))):
            self.draw_piece_preview(
                self.piece_queue[i],
                next_box_x + 20,
                next_box_y + 30 + i * 70,
                20
            )
        
        # Draw controls
        controls = [
            "Controls:",
            "Left/Right: Move",
            "X: Rotate",
            "Up: Hard Drop",
            "Down: Soft Drop",
            "C: Hold piece",
            "R: Restart"
        ]
        
        control_y = next_box_y + next_box_height + 20
        for i, text in enumerate(controls):
            control_text = self.small_font.render(text, True, WHITE)
            self.screen.blit(control_text, (next_box_x, control_y + i * 25))
        
        # Draw title
        title_text = self.font.render("TETRIS", True, WHITE)
        title_x = BOARD_OFFSET_X + (BOARD_WIDTH * BLOCK_SIZE - title_text.get_width()) // 2
        self.screen.blit(title_text, (title_x, 10))
        
        # Draw fast drop indicator
        if self.fast_drop_active:
            speed_text = self.small_font.render("FAST DROP!", True, YELLOW)
            self.screen.blit(speed_text, (hold_box_x, hold_box_y + hold_box_size + 20))
        
        # Draw game over message
        if self.game_over:
            game_over_text = self.font.render("GAME OVER", True, RED)
            restart_text = self.font.render("Press R to restart", True, WHITE)
            
            # Center the text on the board
            text_x = BOARD_OFFSET_X + (BOARD_WIDTH * BLOCK_SIZE - game_over_text.get_width()) // 2
            
            self.screen.blit(game_over_text, (text_x, BOARD_OFFSET_Y + BOARD_HEIGHT * BLOCK_SIZE // 2 - 20))
            self.screen.blit(restart_text, (text_x - 40, BOARD_OFFSET_Y + BOARD_HEIGHT * BLOCK_SIZE // 2 + 20))
    
    def game_over_state(self):
        self.game_over = True
        
    def handle_events(self):
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                return False
            
            if event.type == pygame.MOUSEBUTTONDOWN:
                # Check if quit button was clicked
                mouse_pos = pygame.mouse.get_pos()
                quit_button = pygame.Rect(SCREEN_WIDTH - 100, 10, 80, 30)
                if quit_button.collidepoint(mouse_pos):
                    return False  # Return to main menu
            
            # Handle key press events
            elif event.type == pygame.KEYDOWN:
                # Check for quit shortcuts (P or ESC)
                if event.key in [pygame.K_p, pygame.K_ESCAPE]:
                    return False  # Return to main menu
                    
                if event.key == pygame.K_r and self.game_over:
                    self.reset_game()
                elif event.key == pygame.K_c and not self.game_over:  # Hold piece with 'C' key
                    self.hold_piece_action()
                elif event.key == pygame.K_DOWN and not self.game_over:  # Start soft drop
                    self.fast_drop_active = True
                elif event.key == pygame.K_UP and not self.game_over:  # Hard drop with Up key
                    self.hard_drop()
            
            # Handle key release events
            elif event.type == pygame.KEYUP:
                if event.key == pygame.K_DOWN:  # End soft drop
                    self.fast_drop_active = False
                
        return True
        
    def update(self):
        if self.game_over:
            return
        
        current_time = pygame.time.get_ticks()
        
        # Handle user input
        keys = pygame.key.get_pressed()
        
        if current_time - self.last_button_time > self.button_delay:
            if keys[pygame.K_LEFT]:
                self.last_button_time = current_time
                self.current_piece_x -= 1
                if self.check_collision():
                    self.current_piece_x += 1
            
            elif keys[pygame.K_RIGHT]:
                self.last_button_time = current_time
                self.current_piece_x += 1
                if self.check_collision():
                    self.current_piece_x -= 1
            
            elif keys[pygame.K_x]:  # Changed rotation key from UP to X
                self.last_button_time = current_time
                self.rotate_piece()
        
        # Check if it's time for the piece to fall
        fall_speed = self.fast_fall_delay if self.fast_drop_active else self.fall_delay
        if current_time - self.last_fall_time >= fall_speed:
            self.last_fall_time = current_time
            
            # Move piece down
            self.current_piece_y += 1
            
            # Check for collision
            if self.check_collision():
                self.current_piece_y -= 1
                self.place_piece()
                self.clear_lines()
                self.create_new_piece()
                
                # Check for game over
                if self.check_collision():
                    self.game_over_state()
                    return
    
    def run(self):
        # Game loop
        clock = pygame.time.Clock()
        running = True
        
        while running:
            # Handle events
            running = self.handle_events()
            
            self.update()
            self.draw_board()
            
            pygame.display.flip()
            clock.tick(60)  # 60 FPS

# Run the game
if __name__ == "__main__":
    game = TetrisGame()
    game.run()
    pygame.quit()
    sys.exit()