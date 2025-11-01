import pygame
import sys
import os
from twenty import Game2048
from tetris import TetrisGame
from pacman import Pacman

# Simplified Dashboard class without Tkinter
class SimpleDashboard:
    def __init__(self):
        self.vcu_state = 'DRIVE'  # Default state
        self.bms_state = None
        self.motor_temp = -1
        self.mc_temp = -1
        self.pack_temp = -1
        self.speed_MPH = None
        self.glv_voltage = None
        self.soc = None
        self.knob1_percentage = 0
        self.knob2_percentage = 0

# Initialize Pygame
pygame.init()

# Constants
WINDOW_SIZE = 800
MENU_ITEM_HEIGHT = 100
PADDING = 20
ANIMATION_SPEED = 0.1

# Voltage thresholds (in volts)
MIN_CAPACITOR_VOLTAGE = 300  # 30.0V (since capacitor_volt_x10 is multiplied by 10)
MIN_GLV_VOLTAGE = 100  # 10.0V
MIN_PACK_VOLTAGE = 300  # 300V

# Colors
BACKGROUND_COLOR = (20, 20, 20)  # Dark background
TEXT_COLOR = (255, 255, 255)
HOVER_COLOR = (255, 50, 50)  # Racing red
SELECTED_COLOR = (255, 100, 50)  # Orange-red
TITLE_COLOR = (255, 50, 50)  # Racing red
GRID_COLOR = (40, 40, 40)  # Slightly lighter than background
ACCENT_COLOR = (255, 200, 0)  # Gold accent
ERROR_COLOR = (255, 0, 0)  # Red for errors

class MainMenu:
    def __init__(self):
        self.screen = pygame.display.set_mode((WINDOW_SIZE, WINDOW_SIZE))
        pygame.display.set_caption("Dashboard Games")
        self.clock = pygame.time.Clock()
        
        # Initialize dashboard
        self.dashboard = SimpleDashboard()
        
        # Load fonts
        self.title_font = pygame.font.Font(None, 72)
        self.menu_font = pygame.font.Font(None, 48)
        self.error_font = pygame.font.Font(None, 36)
        
        # Menu items
        self.menu_items = [
            {"name": "2048", "game": Game2048},
            {"name": "Tetris", "game": TetrisGame},
            {"name": "Pacman", "game": Pacman}
        ]
        
        # Animation properties
        self.hover_index = -1
        self.hover_progress = 0
        self.selected_index = -1
        self.selected_progress = 0
        
        # Game instance
        self.current_game = None
        
        # Voltage error state
        self.voltage_error = None

    def check_voltage_status(self):
        """Check if the dashboard is in LV state"""
        try:
            # Always allow games regardless of state
            self.voltage_error = None
            return True
            
        except Exception as e:
            self.voltage_error = f"Error checking vehicle state: {str(e)}"
            return False

    def draw_voltage_error(self):
        """Draw the voltage error message if one exists"""
        if self.voltage_error:
            # Create a semi-transparent overlay
            overlay = pygame.Surface((WINDOW_SIZE, WINDOW_SIZE), pygame.SRCALPHA)
            overlay.fill((0, 0, 0, 180))  # Semi-transparent black
            self.screen.blit(overlay, (0, 0))
            
            # Draw error message
            error_text = self.error_font.render(self.voltage_error, True, ERROR_COLOR)
            text_rect = error_text.get_rect(center=(WINDOW_SIZE // 2, WINDOW_SIZE // 2))
            self.screen.blit(error_text, text_rect)
            
            # Draw "Press any key to continue" message
            continue_text = self.error_font.render("Press any key to continue", True, TEXT_COLOR)
            continue_rect = continue_text.get_rect(center=(WINDOW_SIZE // 2, WINDOW_SIZE // 2 + 50))
            self.screen.blit(continue_text, continue_rect)

    def reinitialize_display(self):
        """Reinitialize the display surface after returning from a game"""
        self.screen = pygame.display.set_mode((WINDOW_SIZE, WINDOW_SIZE))
        pygame.display.set_caption("Dashboard Games")

    def draw_menu_item(self, text, y, is_hovered, is_selected):
        # Calculate colors based on state
        if is_selected:
            color = SELECTED_COLOR
            scale = 1.1 + 0.1 * self.selected_progress
        elif is_hovered:
            color = HOVER_COLOR
            scale = 1.0 + 0.1 * self.hover_progress
        else:
            color = TEXT_COLOR
            scale = 1.0

        # Draw menu item background
        item_rect = pygame.Rect(
            WINDOW_SIZE // 4,
            y,
            WINDOW_SIZE // 2,
            MENU_ITEM_HEIGHT
        )
        
        # Scale the rectangle
        scaled_width = int(item_rect.width * scale)
        scaled_height = int(item_rect.height * scale)
        scaled_x = item_rect.centerx - scaled_width // 2
        scaled_y = item_rect.centery - scaled_height // 2
        
        # Draw background with gradient effect
        for i in range(scaled_height):
            alpha = 255 - (i * 2)  # Gradient from top to bottom
            if alpha < 0:
                alpha = 0
            s = pygame.Surface((scaled_width, 1))
            s.set_alpha(alpha)
            s.fill(color)
            self.screen.blit(s, (scaled_x, scaled_y + i))
        
        # Draw border
        pygame.draw.rect(
            self.screen,
            ACCENT_COLOR,
            (scaled_x, scaled_y, scaled_width, scaled_height),
            2,
            border_radius=10
        )
        
        # Draw text with shadow
        text_surface = self.menu_font.render(text, True, (0, 0, 0))
        shadow_rect = text_surface.get_rect(center=(item_rect.centerx + 2, item_rect.centery + 2))
        self.screen.blit(text_surface, shadow_rect)
        
        text_surface = self.menu_font.render(text, True, color)
        text_rect = text_surface.get_rect(center=item_rect.center)
        self.screen.blit(text_surface, text_rect)

    def draw(self):
        self.screen.fill(BACKGROUND_COLOR)
        
        # Draw grid pattern
        grid_spacing = 40
        for x in range(0, WINDOW_SIZE, grid_spacing):
            pygame.draw.line(self.screen, GRID_COLOR, (x, 0), (x, WINDOW_SIZE))
        for y in range(0, WINDOW_SIZE, grid_spacing):
            pygame.draw.line(self.screen, GRID_COLOR, (0, y), (WINDOW_SIZE, y))
        
        # Draw title with shadow
        title_text = self.title_font.render("DASHBOARD GAMES", True, (0, 0, 0))
        title_rect = title_text.get_rect(center=(WINDOW_SIZE // 2 + 2, 100 + 2))
        self.screen.blit(title_text, title_rect)
        
        title_text = self.title_font.render("DASHBOARD GAMES", True, TITLE_COLOR)
        title_rect = title_text.get_rect(center=(WINDOW_SIZE // 2, 100))
        self.screen.blit(title_text, title_rect)
        
        # Draw decorative lines
        line_length = 200
        line_y = 150
        pygame.draw.line(self.screen, ACCENT_COLOR, 
                        (WINDOW_SIZE // 2 - line_length, line_y),
                        (WINDOW_SIZE // 2 + line_length, line_y), 2)
        
        # Draw menu items
        for i, item in enumerate(self.menu_items):
            y = 250 + i * (MENU_ITEM_HEIGHT + PADDING)
            is_hovered = i == self.hover_index
            is_selected = i == self.selected_index
            self.draw_menu_item(item["name"], y, is_hovered, is_selected)
        
        # Draw voltage error if present
        self.draw_voltage_error()
        
        pygame.display.flip()

    def handle_events(self):
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                sys.exit()
            
            if event.type == pygame.KEYDOWN:
                # Check for quit shortcuts (P or ESC)
                if event.key in [pygame.K_p, pygame.K_ESCAPE]:
                    pygame.quit()
                    sys.exit()
                
                # If there's a voltage error, any key will dismiss it
                if self.voltage_error:
                    self.voltage_error = None
                    return
            
            if event.type == pygame.MOUSEMOTION:
                # Check if mouse is over any menu item
                mouse_y = event.pos[1]
                for i, item in enumerate(self.menu_items):
                    y = 250 + i * (MENU_ITEM_HEIGHT + PADDING)
                    if y <= mouse_y <= y + MENU_ITEM_HEIGHT:
                        self.hover_index = i
                        break
                else:
                    self.hover_index = -1
            
            if event.type == pygame.MOUSEBUTTONDOWN:
                if event.button == 1:  # Left click
                    mouse_y = event.pos[1]
                    for i, item in enumerate(self.menu_items):
                        y = 250 + i * (MENU_ITEM_HEIGHT + PADDING)
                        if y <= mouse_y <= y + MENU_ITEM_HEIGHT:
                            self.selected_index = i
                            break

    def update(self):
        # Check voltage status
        if not self.check_voltage_status():
            # If not in LV state, reset any selection and prevent game start
            self.selected_index = -1
            self.selected_progress = 0
            return
        
        # Update hover animation
        if self.hover_index != -1:
            self.hover_progress = min(1.0, self.hover_progress + ANIMATION_SPEED)
        else:
            self.hover_progress = max(0.0, self.hover_progress - ANIMATION_SPEED)
        
        # Update selection animation
        if self.selected_index != -1:
            self.selected_progress = min(1.0, self.selected_progress + ANIMATION_SPEED)
            if self.selected_progress >= 1.0:
                # Start the selected game
                selected_game = self.menu_items[self.selected_index]["game"]
                self.current_game = selected_game()
                self.current_game.run()
                # Reset selection after game ends
                self.selected_index = -1
                self.selected_progress = 0
                # Reinitialize display after returning from game
                self.reinitialize_display()
        else:
            self.selected_progress = max(0.0, self.selected_progress - ANIMATION_SPEED)

    def run(self):
        while True:
            self.handle_events()
            self.update()
            self.draw()
            self.clock.tick(60)

if __name__ == "__main__":
    menu = MainMenu()
    menu.run() 