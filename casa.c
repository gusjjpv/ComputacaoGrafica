#define _USE_MATH_DEFINES
#include <stdio.h>
#include <math.h>
#include <string.h>

#define IMAGE_WIDTH 256
#define IMAGE_HEIGHT 256
#define CENTER_X 64
#define CENTER_Y 64
#define NUM_HOUSE_LINES 9

unsigned char img[IMAGE_HEIGHT][IMAGE_WIDTH][3];

typedef struct
{
    int x;
    int y;
} Point;

typedef struct
{
    Point start;
    Point end;
    unsigned char r, g, b;
} Line;

// Protótipos de todas as funções
void initializeHouse(Line house[NUM_HOUSE_LINES]);
void clampPoint(Point *p);
void transformHouse(Line house[NUM_HOUSE_LINES], float scale, float angle);
void drawHouse(Line house[NUM_HOUSE_LINES]);
void scalePoint(Point *p, Point center, float scale);
void clearImage();
void saveImage();
void setPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b);
void drawLine(Point start, Point end, unsigned char r, unsigned char g, unsigned char b);
void reflectPoint(Point *p, int reflectX, int reflectY, Point center);
void reflectHouse(Line house[NUM_HOUSE_LINES], int reflectX, int reflectY);
void espelharCasas(Line original[NUM_HOUSE_LINES], int refletirX, int refletirY, int ambos);
void translatePoint(Point *p, int dx, int dy);
void translateHouse(Line house[NUM_HOUSE_LINES], int dx, int dy);
void shearPoint(Point *p, float shearX, float shearY);
void shearHouse(Line house[NUM_HOUSE_LINES], float shearX, float shearY);

int main()
{
    const float scale_factor = 1;
    const float rotation_angle = 0.0;

    const int dx = 0; // deslocamento no eixo X
    const int dy = 0; // deslocamento no eixo Y

    const float shearX = 0.0f; // Shear horizontal (em X)
    const float shearY = 0.0f; // Shear vertical (em Y)

    Line house[NUM_HOUSE_LINES];
    initializeHouse(house);

    transformHouse(house, scale_factor, rotation_angle);
    shearHouse(house, shearX, shearY);
    translateHouse(house, dx, dy);

    clearImage();
    drawHouse(house);

    // Personalize os parâmetros abaixo:
    // refletirX = 1 ativa espelhamento no eixo X
    // refletirY = 1 ativa espelhamento no eixo Y
    // ambos = 1 ativa espelhamento nos dois eixos
    espelharCasas(house, 0, 0, 0); // exemplo: desativa espelhamento

    saveImage();
    return 0;
}

void clampPoint(Point *p)
{
    if (p->x < 0)
        p->x = 0;
    if (p->x >= IMAGE_WIDTH)
        p->x = IMAGE_WIDTH - 1;
    if (p->y < 0)
        p->y = 0;
    if (p->y >= IMAGE_HEIGHT)
        p->y = IMAGE_HEIGHT - 1;
}

void scalePoint(Point *p, Point center, float scale)
{
    float max_scale_x = fminf(
        (center.x) / fabsf(p->x - center.x),
        (IMAGE_WIDTH - center.x) / fabsf(p->x - center.x));
    float max_scale_y = fminf(
        (center.y) / fabsf(p->y - center.y),
        (IMAGE_HEIGHT - center.y) / fabsf(p->y - center.y));
    float safe_scale = fminf(fminf(max_scale_x, max_scale_y), scale);

    float translatedX = p->x - center.x;
    float translatedY = p->y - center.y;

    translatedX *= safe_scale;
    translatedY *= safe_scale;

    p->x = round(translatedX + center.x);
    p->y = round(translatedY + center.y);

    clampPoint(p);
}

void rotatePoint(Point *p, Point center, float angle_degrees)
{
    float angle_rad = angle_degrees * M_PI / 180.0f;
    float cos_theta = cosf(angle_rad);
    float sin_theta = sinf(angle_rad);

    int translatedX = p->x - center.x;
    int translatedY = p->y - center.y;

    p->x = round(translatedX * cos_theta - translatedY * sin_theta) + center.x;
    p->y = round(translatedX * sin_theta + translatedY * cos_theta) + center.y;

    clampPoint(p);
}

void transformHouse(Line house[NUM_HOUSE_LINES], float scale, float angle)
{
    const Point center = {CENTER_X, CENTER_Y};

    for (int i = 0; i < NUM_HOUSE_LINES; i++)
    {
        rotatePoint(&house[i].start, center, angle);
        rotatePoint(&house[i].end, center, angle);

        scalePoint(&house[i].start, center, scale);
        scalePoint(&house[i].end, center, scale);
    }
}

void reflectPoint(Point *p, int reflectX, int reflectY, Point center)
{
    int x = p->x - center.x;
    int y = p->y - center.y;

    if (reflectX)
        x = -x;
    if (reflectY)
        y = -y;

    p->x = x + center.x;
    p->y = y + center.y;

    clampPoint(p);
}

void reflectHouse(Line house[NUM_HOUSE_LINES], int reflectX, int reflectY)
{
    const Point center = {IMAGE_WIDTH / 2, IMAGE_HEIGHT / 2};
    for (int i = 0; i < NUM_HOUSE_LINES; i++)
    {
        reflectPoint(&house[i].start, reflectX, reflectY, center);
        reflectPoint(&house[i].end, reflectX, reflectY, center);
    }
}

void espelharCasas(Line original[NUM_HOUSE_LINES], int refletirX, int refletirY, int ambos)
{
    if (refletirX)
    {
        Line casaX[NUM_HOUSE_LINES];
        memcpy(casaX, original, sizeof(Line) * NUM_HOUSE_LINES);
        reflectHouse(casaX, 1, 0);
        drawHouse(casaX);
    }

    if (refletirY)
    {
        Line casaY[NUM_HOUSE_LINES];
        memcpy(casaY, original, sizeof(Line) * NUM_HOUSE_LINES);
        reflectHouse(casaY, 0, 1);
        drawHouse(casaY);
    }

    if (ambos)
    {
        Line casaXY[NUM_HOUSE_LINES];
        memcpy(casaXY, original, sizeof(Line) * NUM_HOUSE_LINES);
        reflectHouse(casaXY, 1, 1);
        drawHouse(casaXY);
    }
}

void translatePoint(Point *p, int dx, int dy)
{
    p->x += dx;
    p->y += dy;
    clampPoint(p); // Garante que o ponto fique dentro da imagem
}

void translateHouse(Line house[NUM_HOUSE_LINES], int dx, int dy)
{
    for (int i = 0; i < NUM_HOUSE_LINES; i++)
    {
        translatePoint(&house[i].start, dx, dy);
        translatePoint(&house[i].end, dx, dy);
    }
}

void shearPoint(Point *p, float shearX, float shearY)
{
    int newX = round(p->x + shearX * p->y);
    int newY = round(p->y + shearY * p->x);

    p->x = newX;
    p->y = newY;

    clampPoint(p); // Mantém dentro da imagem
}

void shearHouse(Line house[NUM_HOUSE_LINES], float shearX, float shearY)
{
    for (int i = 0; i < NUM_HOUSE_LINES; i++)
    {
        shearPoint(&house[i].start, shearX, shearY);
        shearPoint(&house[i].end, shearX, shearY);
    }
}

void initializeHouse(Line house[NUM_HOUSE_LINES])
{
    house[0] = (Line){{0, 64}, {64, 0}, 255, 0, 0};
    house[1] = (Line){{64, 0}, {128, 64}, 255, 0, 0};
    house[2] = (Line){{0, 64}, {128, 64}, 0, 0, 255};
    house[3] = (Line){{0, 64}, {0, 128}, 0, 255, 0};
    house[4] = (Line){{128, 64}, {128, 128}, 0, 255, 0};
    house[5] = (Line){{0, 128}, {128, 128}, 0, 0, 255};
    house[6] = (Line){{48, 128}, {48, 98}, 255, 255, 0};
    house[7] = (Line){{80, 128}, {80, 98}, 255, 255, 0};
    house[8] = (Line){{48, 98}, {80, 98}, 255, 255, 0};

    int door_width = 32;
    int door_height = 30;
    int base_y = 128;

    house[6] = (Line){{(128 - door_width) / 2, base_y}, {(128 - door_width) / 2, base_y - door_height}, 255, 255, 0};
    house[7] = (Line){{(128 + door_width) / 2, base_y}, {(128 + door_width) / 2, base_y - door_height}, 255, 255, 0};
    house[8] = (Line){{(128 - door_width) / 2, base_y - door_height}, {(128 + door_width) / 2, base_y - door_height}, 255, 255, 0};
}

void setPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b)
{
    if (x >= 0 && x < IMAGE_WIDTH && y >= 0 && y < IMAGE_HEIGHT)
    {
        img[y][x][0] = r;
        img[y][x][1] = g;
        img[y][x][2] = b;
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

void drawHouse(Line house[NUM_HOUSE_LINES])
{
    for (int i = 0; i < NUM_HOUSE_LINES; i++)
    {
        drawLine(house[i].start, house[i].end, house[i].r, house[i].g, house[i].b);
    }
}

void clearImage()
{
    for (int i = 0; i < IMAGE_HEIGHT; i++)
        for (int j = 0; j < IMAGE_WIDTH; j++)
            for (int c = 0; c < 3; c++)
                img[i][j][c] = 0;
}

void saveImage()
{
    printf("P3\n%d %d\n255\n", IMAGE_WIDTH, IMAGE_HEIGHT);
    for (int i = 0; i < IMAGE_HEIGHT; i++)
    {
        for (int j = 0; j < IMAGE_WIDTH; j++)
        {
            for (int c = 0; c < 3; c++)
            {
                printf("%d ", img[i][j][c]);
            }
        }
        printf("\n");
    }
}
