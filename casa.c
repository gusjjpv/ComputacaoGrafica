#define _USE_MATH_DEFINES
#include <stdio.h>
#include <math.h>

#define h 256
#define w 256
#define NUM_LINES 9 // Número total de linhas da casa

unsigned char img[h][w][3];

// Estrutura para armazenar pontos
typedef struct
{
    int x;
    int y;
} Point;

// Vetor com todas as linhas da casa (ponto inicial e final + cor)
typedef struct
{
    Point start;
    Point end;
    unsigned char r, g, b;
} HouseLine;

// Função para rotação de um ponto
void rotatePoint(Point *p, Point center, float angle_degrees)
{
    float angle_rad = angle_degrees * M_PI / 180.0;
    float cos_theta = cos(angle_rad);
    float sin_theta = sin(angle_rad);

    // Transladar para origem
    int translatedX = p->x - center.x;
    int translatedY = p->y - center.y;

    // Aplicar rotação
    p->x = round(translatedX * cos_theta - translatedY * sin_theta) + center.x;
    p->y = round(translatedX * sin_theta + translatedY * cos_theta) + center.y;
}

void setPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b)
{
    if (x >= 0 && x < w && y >= 0 && y < h)
    {
        img[y][x][0] = r;
        img[y][x][1] = g;
        img[y][x][2] = b;
    }
}

void clearImage()
{
    for (int i = 0; i < h; i++)
        for (int j = 0; j < w; j++)
            for (int c = 0; c < 3; c++)
                img[i][j][c] = 0;
}

void saveImage()
{
    printf("P3\n%d %d\n255\n", w, h);
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            for (int c = 0; c < 3; c++)
            {
                printf("%d ", img[i][j][c]);
            }
        }
        printf("\n");
    }
}

void drawLine(Point start, Point end, unsigned char r, unsigned char g, unsigned char b)
{
    for (float t = 0.0; t <= 1.0; t += 0.0001)
    {
        int x = start.x + t * (end.x - start.x);
        int y = start.y + t * (end.y - start.y);
        setPixel(x, y, r, g, b);
    }
}

int main()
{
    clearImage();

    // Centro de rotação
    Point center = {64, 64};
    float rotation_angle = 45.0; // Ângulo em graus

    // Definir todas as linhas da casa (coordenadas originais)
    HouseLine house[NUM_LINES] = {
        // Teto
        {{0, 64}, {64, 0}, 255, 0, 0},   // Esquerdo
        {{64, 0}, {128, 64}, 255, 0, 0}, // Direito
        {{0, 64}, {128, 64}, 0, 0, 255}, // Base do teto

        // Paredes
        {{0, 64}, {0, 128}, 0, 255, 0},     // Esquerda
        {{128, 64}, {128, 128}, 0, 255, 0}, // Direita

        // Base
        {{0, 128}, {128, 128}, 0, 0, 255}, // Chão

        // Porta
        {{48, 128}, {48, 98}, 255, 255, 0}, // Esquerda
        {{80, 128}, {80, 98}, 255, 255, 0}, // Direita
        {{48, 98}, {80, 98}, 255, 255, 0}   // Topo
    };

    // Aplicar rotação a todos os pontos
    for (int i = 0; i < NUM_LINES; i++)
    {
        rotatePoint(&house[i].start, center, rotation_angle);
        rotatePoint(&house[i].end, center, rotation_angle);
    }

    // Desenhar todas as linhas
    for (int i = 0; i < NUM_LINES; i++)
    {
        drawLine(
            house[i].start,
            house[i].end,
            house[i].r,
            house[i].g,
            house[i].b);
    }

    saveImage();
    return 0;
}