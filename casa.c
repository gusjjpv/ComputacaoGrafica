#include <stdio.h>

#define h 256
#define w 256

unsigned char img[h][w][3];

void setPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b) {
    img[y][x][0] = r;
    img[y][x][1] = g;
    img[y][x][2] = b;
}

void clearImage() {
    for (int i = 0; i < h; i++) 
        for (int j = 0; j < w; j++) 
            for (int c = 0; c < 3; c++) 
                img[i][j][c] = 0;
}

void saveImage(){
    printf("P3\n%d %d\n255\n", w, h);
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            for (int c = 0; c < 3; c++) {
                printf("%d ", img[i][j][c]);
            }
        }
        printf("\n");
    }
}   

void drawLine(int x1, int y1, int x2, int y2, unsigned char r, unsigned char g, unsigned char b) {
    for (float t = 0.0; t <= 1.0; t += 0.0001) {
        setPixel((int)(x1 + t * (x2 - x1)), (int)(y1 + t * (y2 - y1)), r, g, b);
    }
}

int main() {
    clearImage();

    // Teto da casa (triângulo)
    drawLine(0, 64, 64, 0, 255, 0, 0);   // Linha esquerda do teto
    drawLine(64, 0, 128, 64, 255, 0, 0); // Linha direita do teto
    drawLine(0, 64, 128, 64, 0, 0, 255); // Base do teto (opcional)

    // Paredes da casa (retângulo)
    drawLine(0, 64, 0, 128, 0, 255, 0);     // Parede esquerda
    drawLine(128, 64, 128, 128, 0, 255, 0); // Parede direita

    // Base da casa (corrigida!)
    drawLine(0, 128, 128, 128, 0, 0, 255); // Base horizontal

    // Porta
    drawLine(48, 128, 48, 98, 255, 255, 0); // Lado esquerdo da porta
    drawLine(80, 128, 80, 98, 255, 255, 0); // Lado direito da porta
    drawLine(48, 98, 80, 98, 255, 255, 0);

    saveImage(); // Salvar a imagem no formato PPM

    return 0;
}