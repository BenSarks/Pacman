// ============================================
// Created by Ben Serkis
// ============================================

#include <cstdio>
#include <iostream>
#include <string>
#include <windows.h>
#include "Constants.h"
#include "Game.h"
#include <stb_image.h>
#include <GL/freeglut.h>

namespace
{
	Game* game = nullptr;
	GLuint mazeTexture = 0;
	bool paused = false;
	bool showRoutes = true;
	int simulationSpeed = 1;

	// Created by Ben Serkis
	std::string ExecutableDirectory()
	{
		char buffer[MAX_PATH];
		DWORD length = GetModuleFileNameA(nullptr, buffer, MAX_PATH);
		std::string path(buffer, length);
		size_t slash = path.find_last_of("\\/");
		return slash == std::string::npos ? std::string() : path.substr(0, slash + 1);
	}

	// Created by Ben Serkis
	bool LoadMazeTexture()
	{
		const std::string candidates[] = {
			ExecutableDirectory() + "assets/pacman-maze.png",
			"assets/pacman-maze.png",
		};
		for (const std::string& path : candidates)
		{
			int width, height, channels;
			unsigned char* pixels = stbi_load(path.c_str(), &width, &height, &channels, 3);
			if (!pixels)
				continue;

			glGenTextures(1, &mazeTexture);
			glBindTexture(GL_TEXTURE_2D, mazeTexture);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			stbi_image_free(pixels);
			return true;
		}
		std::cerr << "Could not load the maze image (" << stbi_failure_reason()
			<< "); drawing plain walls instead.\n";
		return false;
	}

	// Created by Ben Serkis
	void DrawMaze()
	{
		if (mazeTexture != 0)
		{
			glColor3f(1.0f, 1.0f, 1.0f);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, mazeTexture);
			glBegin(GL_QUADS);
			glTexCoord2f(0, 1); glVertex2f(0, 0);
			glTexCoord2f(1, 1); glVertex2f(MAZE_WIDTH_PX, 0);
			glTexCoord2f(1, 0); glVertex2f(MAZE_WIDTH_PX, MAZE_HEIGHT_PX);
			glTexCoord2f(0, 0); glVertex2f(0, MAZE_HEIGHT_PX);
			glEnd();
			glDisable(GL_TEXTURE_2D);
			return;
		}

		// no image - draw walls as blocks
		glColor3f(0.0f, 0.05f, 0.45f);
		for (int r = 0; r < MAZE_ROWS; r++)
		{
			for (int c = 0; c < MAZE_COLS; c++)
			{
				if (!game->GetMaze().IsWalkable({ r, c }))
					glRectf(c * CELL_WIDTH, r * CELL_HEIGHT, (c + 1) * CELL_WIDTH, (r + 1) * CELL_HEIGHT);
			}
		}
	}

	// Created by Ben Serkis
	void DrawText(float x, float y, const std::string& text, void* font = GLUT_BITMAP_HELVETICA_18)
	{
		glRasterPos2f(x, y);
		glutBitmapString(font, reinterpret_cast<const unsigned char*>(text.c_str()));
	}

	// Created by Ben Serkis
	void DrawHud()
	{
		const Pacman& pacman = game->GetPacman();
		const Maze& maze = game->GetMaze();
		const float top = static_cast<float>(MAZE_HEIGHT_PX);

		glColor3f(0.08f, 0.08f, 0.12f);
		glRectf(0, top, WINDOW_WIDTH, WINDOW_HEIGHT);

		char line[160];
		std::snprintf(line, sizeof(line), "Score %d    Coins %d/%d    Ghosts %d/%d    %ds",
			pacman.Score(), maze.TotalCoins() - maze.CoinsLeft(), maze.TotalCoins(),
			game->GhostsRemaining(), static_cast<int>(game->Ghosts().size()),
			static_cast<int>(game->ElapsedSeconds()));
		glColor3f(1.0f, 1.0f, 1.0f);
		DrawText(10, top + 24, line);

		std::string status = std::string("Pac-Man: ") + pacman.StateName();
		int statusWidth = glutBitmapLength(GLUT_BITMAP_HELVETICA_18, reinterpret_cast<const unsigned char*>(status.c_str()));
		glColor3f(1.0f, 0.9f, 0.2f);
		DrawText(static_cast<float>(WINDOW_WIDTH - statusWidth - 10), top + 24, status);

		std::snprintf(line, sizeof(line), "[P] pause  [R] restart  [V] routes %s  [+/-] speed x%d  [Esc] quit%s",
			showRoutes ? "on" : "off", simulationSpeed, paused ? "   - PAUSED -" : "");
		glColor3f(0.65f, 0.65f, 0.7f);
		DrawText(10, top + 7, line, GLUT_BITMAP_HELVETICA_12);
	}

	// Created by Ben Serkis
	void DrawGameOver()
	{
		if (game->Result() == GameResult::Running)
			return;

		const bool pacmanWon = game->Result() == GameResult::PacmanWins;
		const float cx = MAZE_WIDTH_PX / 2.0f;
		const float cy = MAZE_HEIGHT_PX / 2.0f;

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(0.0f, 0.0f, 0.0f, 0.8f);
		glRectf(cx - 230, cy - 55, cx + 230, cy + 55);
		glDisable(GL_BLEND);

		if (pacmanWon)
			glColor3f(1.0f, 1.0f, 0.0f);
		else
			glColor3f(1.0f, 0.3f, 0.3f);
		DrawText(cx - 75, cy + 20, pacmanWon ? "PAC-MAN WINS!" : "GHOSTS WIN!", GLUT_BITMAP_TIMES_ROMAN_24);

		glColor3f(0.9f, 0.9f, 0.9f);
		DrawText(cx - 210, cy - 10, game->LastEvent(), GLUT_BITMAP_HELVETICA_12);
		DrawText(cx - 210, cy - 35, "Press R to play again", GLUT_BITMAP_HELVETICA_12);
	}

	// Created by Ben Serkis
	void Display()
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		DrawMaze();
		game->Draw(showRoutes);
		DrawHud();
		DrawGameOver();

		glutSwapBuffers();
	}

	// Created by Ben Serkis
	void Reshape(int width, int height)
	{
		glViewport(0, 0, width, height);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT, -1, 1);
	}

	// fixed timestep
	// Created by Ben Serkis
	void Tick(int)
	{
		if (!paused)
		{
			for (int i = 0; i < simulationSpeed; i++)
				game->Update(TICK_SECONDS);
		}
		glutPostRedisplay();
		glutTimerFunc(TICK_MS, Tick, 0);
	}

	// Created by Ben Serkis
	void Keyboard(unsigned char key, int, int)
	{
		switch (key)
		{
		case 'p': case 'P': case ' ':
			paused = !paused;
			break;
		case 'r': case 'R':
			game->Reset();
			paused = false;
			break;
		case 'v': case 'V':
			showRoutes = !showRoutes;
			break;
		case '+': case '=':
			simulationSpeed = simulationSpeed < 8 ? simulationSpeed * 2 : 8;
			break;
		case '-': case '_':
			simulationSpeed = simulationSpeed > 1 ? simulationSpeed / 2 : 1;
			break;
		case 27: // Esc
			glutLeaveMainLoop();
			break;
		}
	}
}

// Created by Ben Serkis
int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition(200, 50);
	glutCreateWindow("AI-Powered Pac-Man");
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	LoadMazeTexture();

	Game theGame;
	game = &theGame;

	std::cout << "AI-Powered Pac-Man\n"
		<< "  P / Space  pause\n  R          restart\n  V          toggle AI route overlay\n"
		<< "  + / -      simulation speed\n  Esc        quit\n\n";

	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutTimerFunc(TICK_MS, Tick, 0);
	glutMainLoop();

	game = nullptr;
	return 0;
}
