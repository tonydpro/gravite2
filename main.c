#include "main.h"

double min(double a, double b)
{
    if (a < b)
        return a;
    return b;
}

double max(double a, double b)
{
    if (a > b)
        return a;
    return b;
}

double valeur_absolue(double n)
{
    if (n < 0) return -1*n;
    else return n;
}

double carre(double a)
{
    return a*a;
}

void pause(void)
{
    int continuer = 1;
    SDL_Event event;

    while (continuer)
    {
        SDL_PollEvent(&event);
        switch(event.type)
        {
            case SDL_QUIT:
                continuer = 0;
                break;
            case SDL_KEYDOWN :
                if(event.key.keysym.sym == SDLK_RETURN)
                    continuer = 0;
                break;
        }
    }
}

typedef struct VIT VIT;
struct VIT
{
    double vecteur,angle;
};

typedef struct ATOME ATOME;
struct ATOME
{
    VIT vitesse,inertie;
    double x,y,v,w,masse;
    int active;
};

typedef struct RESULTANTE RESULTANTE;
struct RESULTANTE
{
    double x,y;
};

ATOME init_atome(double vecteur,double angle,double x,double y,double masse,int active)
{
    ATOME atome;
    atome.x = x;
    atome.y = y;
    atome.vitesse.vecteur = 0;
    atome.vitesse.angle = 0;
    atome.inertie.vecteur = vecteur;
    atome.inertie.angle = angle;
    atome.masse = masse;
    atome.active = active;
    return atome;
}

int NB_ATOME = 0;

int main(int argc, char** argv)
{
    int trace = 0,ralentissement = 0,lu = 0;
    double force_gravitatonnelle = 0;

    FILE* param = s_fopen("parametres.txt","r");

    lu = fscanf(param,
                "nombre d'atomes = %d\n   \
                traces (ON = 1 ; OFF = 0) = %d\n    \
                ralentissement (entre 1 et 5) = %d\n   \
                gravite = %lf",
                &NB_ATOME,&trace,&ralentissement,&force_gravitatonnelle);



    fclose(param);


    srand(time(NULL));
    //freopen("CON","w",stdout);

    printf("%d %d %d %lf",NB_ATOME,trace,ralentissement,force_gravitatonnelle);

    if (lu != 4)
    {
        printf("Erreur lecture des parametres : %d sur 4\n",lu);
        return -1;
    }

    if ((NB_ATOME < 0) || NB_ATOME > 1000)
    {
        printf("Mauvais nombre d'atomes\n");
        return -1;
    }

    if ((ralentissement > 5) || (ralentissement < 1))
    {
        printf("Mauvaise valeur pour ralentissement (ms)\n");
        return -1;
    }

    int continuer = 1,p;
    SDL_Surface* images[NB_IMG] = {NULL,NULL};
    SDL_Event event;

    SDL_Surface* image = SDL_SetVideoMode(0,0,32,SDL_FULLSCREEN);
    LARGEUR = SDL_GetVideoInfo()->current_w;
    HAUTEUR = SDL_GetVideoInfo()->current_h;
    SDL_FreeSurface(image);

    images[0] = init(LARGEUR,HAUTEUR,"Gravite");
    images[1] = SDL_LoadBMP("vide.bmp");

    ATOME atome[NB_ATOME];
    int i;

    for (i = 0 ; i < NB_ATOME ; i++)
    {
        atome[i] = init_atome(0,0,entAleat(20,LARGEUR-20),entAleat(20,HAUTEUR-20),force_gravitatonnelle,ON);
    }


    RESULTANTE R[NB_ATOME];

    while(continuer)
    {
            memset(R,0,sizeof(RESULTANTE)*NB_ATOME);

            for (p = 0 ; p < NB_ATOME ; p++)
            {
                if (atome[p].active == OFF)
                    continue;

                SDL_PollEvent(&event);

               if (!trace && (atome[p].v >= 0) && (valeur_absolue(atome[p].v) < LARGEUR) && (atome[p].w >= 0) && (valeur_absolue(atome[p].w) < HAUTEUR))
                    definirPixel(ecran,atome[p].v,atome[p].w,0x000000);

                if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
					continuer = 0;

                atome[p].v = atome[p].x; atome[p].w = atome[p].y;

                int a;
                double gravite;

                for (a = 0 ; a < NB_ATOME ; a++)
                {
                    if (a != p)
                    {
                        gravite = (atome[a].masse * atome[p].masse)
                            / max(1,carre(atome[p].x - atome[a].x) + carre(atome[p].y - atome[a].y));

                        atome[p].vitesse.vecteur = gravite;
                        atome[p].vitesse.angle = atan((atome[p].y - atome[a].y)/(atome[p].x - atome[a].x));
                        if (atome[p].x > atome[a].x) atome[p].vitesse.angle += pi;

                        R[p].x = R[p].x + atome[p].vitesse.vecteur*cos(atome[p].vitesse.angle);
                        R[p].y = R[p].y + atome[p].vitesse.vecteur*sin(atome[p].vitesse.angle);
                    }
                }
            }

            for (p = 0 ; p < NB_ATOME ; p++)
            {
                atome[p].x += R[p].x + atome[p].inertie.vecteur*cos(atome[p].inertie.angle);
                atome[p].y += R[p].y + atome[p].inertie.vecteur*sin(atome[p].inertie.angle);

                if ((atome[p].x >= 0) && (valeur_absolue(atome[p].x) < LARGEUR) && (atome[p].y >= 0) && (valeur_absolue(atome[p].y) < HAUTEUR))
                    definirPixel(ecran,atome[p].x,atome[p].y,0xffffff);

                if (atome[p].x == atome[p].v) atome[p].x++;
					atome[p].inertie.angle = atan((atome[p].y - atome[p].w) / (atome[p].x - atome[p].v));


                if (atome[p].x == atome[p].v+1) atome[p].x--;
                if (atome[p].x < atome[p].v) atome[p].inertie.angle += pi;

               atome[p].inertie.vecteur = sqrt(carre(atome[p].w - atome[p].y) + carre(atome[p].v - atome[p].x));

            }

        SDL_Flip(ecran);


        switch(ralentissement)
        {
            case 1:
                break;
            case 2:
                SDL_Delay(1);
                break;
            case 3:
                SDL_Delay(10);
                break;
            case 4:
                SDL_Delay(25);
                break;
            case 5:
                SDL_Delay(100);
                break;
            default:
                break;
        }

    }

    cleanup(images);
    SDL_Quit();
    return EXIT_SUCCESS;
}
