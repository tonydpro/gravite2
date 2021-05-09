#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <SDL/SDL.h>
//#include <SDL/SDL_image.h>

#define NB_IMG      2
#define ecran       images[0]
#define pi          M_PI
#define ON          1
#define OFF         0

int LARGEUR, HAUTEUR;

SDL_Surface* init(int x,int y,const char* titre)
{
    SDL_Surface* screen = NULL;

    if (SDL_Init(SDL_INIT_VIDEO) == -1)
        exit(EXIT_FAILURE);

    if ((screen =
         SDL_SetVideoMode(x,y,32,SDL_DOUBLEBUF | SDL_HWSURFACE | SDL_FULLSCREEN)) == NULL)
        exit(EXIT_FAILURE);

    SDL_WM_SetCaption(titre,NULL);
    SDL_EnableUNICODE(1);
    SDL_EnableKeyRepeat(500,10);

    return screen;
}

void cleanup(SDL_Surface** images)
{
    int img;
    for (img = 0 ; img < NB_IMG ; img++)
        SDL_FreeSurface(images[img]);
    SDL_Quit();
}

FILE* s_fopen(const char* cfile,const char* mode)
{
    FILE* file = NULL;

    if ((file = fopen(cfile,mode)) == NULL)
        exit(EXIT_FAILURE);

    return file;
}

void* s_malloc(size_t size)
{
    void* ptr = NULL;

    if ((ptr = malloc(size)) == NULL)
        exit(EXIT_FAILURE);

    return ptr;
}
/*
SDL_Surface* s_IMG_Load(const char* cimage)
{
    SDL_Surface* surface = NULL;

    if ((surface = IMG_Load(cimage)) == NULL)
    {
        fprintf(stderr,"Impossible de charger l'image \'%s\' !",cimage);
        exit(EXIT_FAILURE);
    }

    return surface;
}*/

size_t sizeofFile(const char* cfile)
{
    size_t size;
    FILE* file = s_fopen(cfile,"rb");
    fseek(file,0,SEEK_END);
    size = ftell(file);
    rewind(file);
    fclose(file);
    return size;
}

void definirPixel(SDL_Surface *surface,int x,int y,Uint32 pixel)
{
    int nbOctetsParPixel = surface->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * nbOctetsParPixel;

    switch(nbOctetsParPixel)
    {

        case 1:
            *p = pixel;
            break;

        case 2:
            *(Uint16 *)p = pixel;
            break;

        case 3:
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            {
                p[0] = (pixel >> 16) & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = pixel & 0xff;
            }
            else
            {
                p[0] = pixel & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = (pixel >> 16) & 0xff;
            }
            break;

        case 4:
            *(Uint32 *)p = pixel;
            break;

		default:
			*p = 0;
			break;
    }
}

Uint32 obtenirPixel(SDL_Surface *surface,int x,int y)
{

    int nbOctetsParPixel = surface->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * nbOctetsParPixel;

    switch(nbOctetsParPixel)
    {
        case 1:
            return *p;
			break;

        case 2:
            return *(Uint16 *)p;
			break;

        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
                return p[0] << 16 | p[1] << 8 | p[2];
            else
                return p[0] | p[1] << 8 | p[2] << 16;
			break;

        case 4:
            return *(Uint32 *)p;
			break;

        default:
            return 0;
			break;
    }
}


int entAleat(int a,int b)
{
    return rand()%(b-a+1)+a;
}

#endif // MAIN_H_INCLUDED
