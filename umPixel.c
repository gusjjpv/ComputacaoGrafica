#include <stdio.h>

int main()
{
    // dimensoes da imagem
    int width = 100;
    int height = 100;
    int center_j = width / 2;
    int center_i = height / 2;

    // Configurando o header do formato ppm
    printf("P3\n %d \t %d\n 255\n", width, height);
    for (int i = 0; i < height; i++)
    { // Altura
        for (int j = 0; j < width; j++)
        {
            // Largura
            if (j == center_j && i == center_i)
            {
                printf("%d \t %d \t %d \n", 255, 0, 0);
            }
            else
            {
                printf("%d \t %d \t %d \n", 0, 0, 0);
            }
        }
    }
    return 0;
}

