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

typedef struct{
    int x;
    int y;
} Point;

typedef struct{
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

int main(){
    const float scale_factor = 1;
    const float rotation_angle = 0.0;

    Line house[NUM_HOUSE_LINES];
    initializeHouse(house);

    transformHouse(house, scale_factor, rotation_angle);

    clearImage();
    drawHouse(house);

    Line reflectedX[NUM_HOUSE_LINES];
    Line reflectedY[NUM_HOUSE_LINES];
    Line reflectedXY[NUM_HOUSE_LINES];

    memcpy(reflectedX, house, sizeof(house));
    memcpy(reflectedY, house, sizeof(house));
    memcpy(reflectedXY, house, sizeof(house));

    reflectHouse(reflectedX, 1, 0);  // Reflexão no eixo X (abaixo)
    reflectHouse(reflectedY, 0, 1);  // Reflexão no eixo Y (esquerda)
    reflectHouse(reflectedXY, 1, 1); // Reflexão nos dois eixos (diagonal)

    drawHouse(reflectedX);
    drawHouse(reflectedY);
    drawHouse(reflectedXY);

    saveImage();

    return 0;
}

void clampPoint(Point *p){
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
    // Calcula a escala máxima permitida para caber na imagem
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

void rotatePoint(Point *p, Point center, float angle_degrees){
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
        // Aplica rotação primeiro
        rotatePoint(&house[i].start, center, angle);
        rotatePoint(&house[i].end, center, angle);

        // Depois escala
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
    const Point center = {IMAGE_WIDTH / 2, IMAGE_HEIGHT / 2}; // <- centro da imagem
    for (int i = 0; i < NUM_HOUSE_LINES; i++)
    {
        reflectPoint(&house[i].start, reflectX, reflectY, center);
        reflectPoint(&house[i].end, reflectX, reflectY, center);
    }
}

void initializeHouse(Line house[NUM_HOUSE_LINES]){
    house[0] = (Line){{0, 64}, {64, 0}, 255, 0, 0};
    house[1] = (Line){{64, 0}, {128, 64}, 255, 0, 0};
    house[2] = (Line){{0, 64}, {128, 64}, 0, 0, 255};
    house[3] = (Line){{0, 64}, {0, 128}, 0, 255, 0};
    house[4] = (Line){{128, 64}, {128, 128}, 0, 255, 0};
    house[5] = (Line){{0, 128}, {128, 128}, 0, 0, 255};
    house[6] = (Line){{48, 128}, {48, 98}, 255, 255, 0};
    house[7] = (Line){{80, 128}, {80, 98}, 255, 255, 0};
    house[8] = (Line){{48, 98}, {80, 98}, 255, 255, 0};

    int door_width = 32;  // Largura da porta
    int door_height = 30; // Altura da porta
    int base_y = 128;     // Base da casa

    // Pontos da porta
    house[6] = (Line){{(128 - door_width) / 2, base_y}, {(128 - door_width) / 2, base_y - door_height}, 255, 255, 0}; // Esquerda
    house[7] = (Line){{(128 + door_width) / 2, base_y}, {(128 + door_width) / 2, base_y - door_height}, 255, 255, 0}; // Direita
    house[8] = (Line){{(128 - door_width) / 2, base_y - door_height}, {(128 + door_width) / 2, base_y - door_height}, 255, 255, 0};
}

void setPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b){
    if (x >= 0 && x < IMAGE_WIDTH && y >= 0 && y < IMAGE_HEIGHT)
    {
        img[y][x][0] = r;
        img[y][x][1] = g;
        img[y][x][2] = b;
    }
}

void drawLine(Point start, Point end, unsigned char r, unsigned char g, unsigned char b){
    for (float t = 0.0; t <= 1.0; t += 0.0001)
    {
        int x = start.x + t * (end.x - start.x);
        int y = start.y + t * (end.y - start.y);
        setPixel(x, y, r, g, b);
    }
}

void drawHouse(Line house[NUM_HOUSE_LINES]){
    for (int i = 0; i < NUM_HOUSE_LINES; i++)
    {
        drawLine(house[i].start, house[i].end, house[i].r, house[i].g, house[i].b);
    }
}

void clearImage(){
    for (int i = 0; i < IMAGE_HEIGHT; i++)
        for (int j = 0; j < IMAGE_WIDTH; j++)
            for (int c = 0; c < 3; c++)
                img[i][j][c] = 0;
}

void saveImage(){
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