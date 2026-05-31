#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <math.h>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>

#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4

const int GRID_SIZE = 10;
const int MAX_X = 100;
const int MAX_Y = 100;

std::vector<std::pair<int, int>> snake;

int foodX, foodY;
int direction = RIGHT;

int score = 0;
int level = 1;
int currentSpeed = 100;

bool gameOver = false;
bool isPaused = false;

int playTimeSeconds = 0;
int timeAccumulator = 0;

void Draw();
void Set_Transformations();
void Initialize(int argc, char *argv[]);
void Timer(int extra);
void SpecialKeyboard(int key, int x, int y);
void NormalKeyboard(unsigned char key, int x, int y);
void Draw_Square(int x, int y, float r, float g, float b);
void SpawnFood();
void DrawText(std::string text, float x, float y);

int main(int argc, char *argv[])
{
    Initialize(argc, argv);
    return 0;
}

void SpawnFood()
{
    foodX = (rand() % ((MAX_X * 2) / GRID_SIZE)) * GRID_SIZE - MAX_X;
    foodY = (rand() % ((MAX_Y * 2) / GRID_SIZE)) * GRID_SIZE - MAX_Y;

    if(foodX >= MAX_X)
        foodX = MAX_X - GRID_SIZE;

    if(foodY >= MAX_Y)
        foodY = MAX_Y - GRID_SIZE;
}

void Initialize(int argc, char *argv[])
{
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

    glutInitWindowPosition(100, 100);
    glutInitWindowSize(600, 600);

    glutCreateWindow("OpenGL Snake Game");

    srand(time(0));

    Set_Transformations();

    snake.push_back({0, 0});
    snake.push_back({-GRID_SIZE, 0});
    snake.push_back({-GRID_SIZE * 2, 0});

    SpawnFood();

    glutDisplayFunc(Draw);
    glutSpecialFunc(SpecialKeyboard);
    glutKeyboardFunc(NormalKeyboard);

    glutTimerFunc(currentSpeed, Timer, 0);

    glutMainLoop();
}

void Timer(int extra)
{
    if (!gameOver && !isPaused)
    {
        timeAccumulator += currentSpeed;

        if (timeAccumulator >= 1000)
        {
            playTimeSeconds++;
            timeAccumulator -= 1000;
        }

        int nextX = snake[0].first;
        int nextY = snake[0].second;

        if (direction == UP)
            nextY += GRID_SIZE;

        else if (direction == DOWN)
            nextY -= GRID_SIZE;

        else if (direction == LEFT)
            nextX -= GRID_SIZE;

        else if (direction == RIGHT)
            nextX += GRID_SIZE;

        if (nextX >= MAX_X || nextX < -MAX_X ||
            nextY >= MAX_Y || nextY < -MAX_Y)
        {
            gameOver = true;
        }

        for (size_t i = 0; i < snake.size(); i++)
        {
            if (snake[i].first == nextX &&
                snake[i].second == nextY)
            {
                gameOver = true;
            }
        }

        if (!gameOver)
        {
            snake.insert(snake.begin(), {nextX, nextY});

            if (nextX == foodX && nextY == foodY)
            {
                score += 10;

                level = (score / 50) + 1;

                currentSpeed = 100 - ((level - 1) * 10);

                if (currentSpeed < 30)
                    currentSpeed = 30;

                SpawnFood();
            }
            else
            {
                snake.pop_back();
            }
        }
    }

    glutPostRedisplay();

    glutTimerFunc(currentSpeed, Timer, 0);
}

void SpecialKeyboard(int key, int x, int y)
{
    if (isPaused || gameOver)
        return;

    if (key == GLUT_KEY_UP && direction != DOWN)
        direction = UP;

    if (key == GLUT_KEY_DOWN && direction != UP)
        direction = DOWN;

    if (key == GLUT_KEY_LEFT && direction != RIGHT)
        direction = LEFT;

    if (key == GLUT_KEY_RIGHT && direction != LEFT)
        direction = RIGHT;
}

void NormalKeyboard(unsigned char key, int x, int y)
{
    if ((key == 'p' || key == 'P' || key == ' ') && !gameOver)
    {
        isPaused = !isPaused;
    }
}

void Set_Transformations()
{
    glClearColor(0.9f, 0.9f, 0.9f, 1.0f);

    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();

    gluOrtho2D(-100, 100, -100, 100);

    glMatrixMode(GL_MODELVIEW);
}

void Draw_Square(int x, int y, float r, float g, float b)
{
    glColor3f(r, g, b);

    glBegin(GL_QUADS);

    glVertex2f(x, y);
    glVertex2f(x + GRID_SIZE, y);
    glVertex2f(x + GRID_SIZE, y + GRID_SIZE);
    glVertex2f(x, y + GRID_SIZE);

    glEnd();

    glColor3f(0.0f, 0.0f, 0.0f);

    glBegin(GL_LINE_LOOP);

    glVertex2f(x, y);
    glVertex2f(x + GRID_SIZE, y);
    glVertex2f(x + GRID_SIZE, y + GRID_SIZE);
    glVertex2f(x, y + GRID_SIZE);

    glEnd();
}

void DrawText(std::string text, float x, float y)
{
    glColor3f(0.0f, 0.0f, 0.0f);

    glRasterPos2f(x, y);

    for (char c : text)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
}

void Draw()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glLoadIdentity();

    Draw_Square(foodX, foodY, 1.0f, 0.0f, 0.0f);

    for (size_t i = 0; i < snake.size(); i++)
    {
        if (i == 0)
        {
            Draw_Square(
                snake[i].first,
                snake[i].second,
                0.0f,
                0.6f,
                0.0f
            );
        }
        else
        {
            Draw_Square(
                snake[i].first,
                snake[i].second,
                0.2f,
                0.9f,
                0.2f
            );
        }
    }

    std::string scoreText =
        "Points: " + std::to_string(score);

    std::string timeText =
        "Time: " + std::to_string(playTimeSeconds) + "s";

    std::string levelText =
        "Level: " + std::to_string(level);

    DrawText(scoreText, -95.0f, 90.0f);
    DrawText(timeText, -15.0f, 90.0f);
    DrawText(levelText, 70.0f, 90.0f);

    if (gameOver)
    {
        DrawText("GAME OVER!", -18.0f, 0.0f);
    }
    else if (isPaused)
    {
        DrawText("PAUSED", -12.0f, 0.0f);
    }

    glutSwapBuffers();
}
