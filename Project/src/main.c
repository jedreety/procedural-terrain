/*
    Projet de Génération de Carte 2D
    Auteur : Mehdi Mazouz
    Date : 2024

    Ce projet génère des cartes d'altitude réalistes en utilisant du bruit de Perlin.
    Il inclut la génération de terrain, et une exportation au format PPM.

	Aucun tutoriel malheursement, mais de la documention sur plein de différent aspect disponible.
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Configuration des constantes
enum { LARGEUR = 256, LONGUEUR = 256, TAILLE_PERMUTATION = 256 };

// Déclaration des fonctions
void initialiser_perlin(int permutation[]);
double lissage(double x);
double interpolation_lineaire(double t, double a, double b);
double gradient(int hash, double x, double y, double z);
double bruit_perlin(double x, double y, double z, const int permutation[]);
void sauvegarder_ppm(double terrain[LARGEUR][LONGUEUR], double hauteur_max, double hauteur_min);

// Tableau de permutation original pour le bruit de Perlin
static const int PERMUTATION[TAILLE_PERMUTATION] = {
    151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,
    8,99,37,240,21,10,23,190,6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,
    35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168,68,175,74,165,71,
    134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,
    55,46,245,40,244,102,143,54,65,25,63,161,1,216,80,73,209,76,132,187,208,89,18,
    169,200,196,135,130,116,188,159,86,164,100,109,198,173,186,3,64,52,217,226,250,
    124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,
    28,42,223,183,170,213,119,248,152,2,44,154,163,70,221,153,101,155,167,43,172,9,
    129,22,39,253,19,98,108,110,79,113,224,232,178,185,112,104,218,246,97,228,251,
    34,242,193,238,210,144,12,191,179,162,241,81,51,145,235,249,14,239,107,49,192,
    214,31,181,199,106,157,184,84,204,176,115,121,50,45,127,4,150,254,138,236,205,
    93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
};

int main() {
    int permutation[TAILLE_PERMUTATION * 2];
    initialiser_perlin(permutation);

    double terrain[LARGEUR][LONGUEUR];

    // Paramètres ajustables (variables)
    double echelle = 50.0;
    int octaves = 7;
    double persistance = 0.8;

    double hauteur_min = INFINITY;
    double hauteur_max = -INFINITY;

    for (int x = 0; x < LARGEUR; x++) {
        for (int y = 0; y < LONGUEUR; y++) {
            double amplitude = 1.0;
            double frequence = 1.0;
            double hauteur = 0.0;

            for (int o = 0; o < octaves; o++) {
                double echantillon_x = x / echelle * frequence;
                double echantillon_y = y / echelle * frequence;

                double bruit = bruit_perlin(echantillon_x, 0.0, echantillon_y, permutation) * 2 - 1;
                hauteur += bruit * amplitude;

                amplitude *= persistance;
                frequence *= 2.0;
            }

            terrain[x][y] = hauteur;
            hauteur_min = fmin(hauteur_min, hauteur);
            hauteur_max = fmax(hauteur_max, hauteur);
        }
    }

    sauvegarder_ppm(terrain, hauteur_max, hauteur_min);
    return 0;
}

void initialiser_perlin(int permutation[]) {
    for (int i = 0; i < TAILLE_PERMUTATION; i++) {
        permutation[i] = PERMUTATION[i];
        permutation[i + TAILLE_PERMUTATION] = PERMUTATION[i];
    }
}

double lissage(double x) {
    return x * x * x * (x * (x * 6 - 15) + 10);
}

double interpolation_lineaire(double t, double a, double b) {
    return a + t * (b - a);
}

double gradient(int hash, double x, double y, double z) {
    int h = hash & 15;
    double u = h < 8 ? x : y;
    double v = h < 4 ? y : (h == 12 || h == 14 ? x : z);
    return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
}

double bruit_perlin(double x, double y, double z, const int permutation[]) {
    int X = (int)floor(x) & 255;
    int Y = (int)floor(y) & 255;
    int Z = (int)floor(z) & 255;

    x -= floor(x);
    y -= floor(y);
    z -= floor(z);

    double u = lissage(x);
    double v = lissage(y);
    double w = lissage(z);

    int A = permutation[X] + Y;
    int AA = permutation[A] + Z;
    int AB = permutation[A + 1] + Z;
    int B = permutation[X + 1] + Y;
    int BA = permutation[B] + Z;
    int BB = permutation[B + 1] + Z;

    double lerp1 = interpolation_lineaire(u, gradient(permutation[AA], x, y, z), gradient(permutation[BA], x - 1, y, z));
    double lerp2 = interpolation_lineaire(u, gradient(permutation[AB], x, y - 1, z), gradient(permutation[BB], x - 1, y - 1, z));
    double lerp3 = interpolation_lineaire(u, gradient(permutation[AA + 1], x, y, z - 1), gradient(permutation[BA + 1], x - 1, y, z - 1));
    double lerp4 = interpolation_lineaire(u, gradient(permutation[AB + 1], x, y - 1, z - 1), gradient(permutation[BB + 1], x - 1, y - 1, z - 1));

    return (interpolation_lineaire(w,
        interpolation_lineaire(v, lerp1, lerp2),
        interpolation_lineaire(v, lerp3, lerp4)) + 1) / 2;
}

void sauvegarder_ppm(double terrain[LARGEUR][LONGUEUR], double hauteur_max, double hauteur_min) {
    FILE* fichier = fopen("../generatedImage/terrain.ppm", "w");
    if (!fichier) {
        perror("Erreur de création du fichier");
        return;
    }

    fprintf(fichier, "P3\n%d %d\n255\n", LARGEUR, LONGUEUR);

    const char gradient[] = { '~', '.', '-', '^', 'A' };
    const int nb_couleurs = sizeof(gradient);

    for (int y = 0; y < LONGUEUR; y++) {
        for (int x = 0; x < LARGEUR; x++) {
            double valeur = (terrain[x][y] - hauteur_min) / (hauteur_max - hauteur_min);
            int r, g, b;

            if (valeur < 0.3) { // Eau
                b = 128 + (int)(valeur * 127 / 0.3);
                g = r = 0;
            }
            else if (valeur < 0.4) { // Sable
                r = 240; g = 230; b = 140;
            }
            else if (valeur < 0.7) { // Végétation
                double t = (valeur - 0.4) / 0.3;

                r = (int)((1 - t) * 40 + t * 25);
                g = (int)((1 - t) * 130 + t * 80);
                b = (int)((1 - t) * 40 + t * 15);
            }
            else { // Montagne
                int gris = 150 + (int)((valeur - 0.7) * 105 / 0.3);
                r = g = b = gris;
            }

            fprintf(fichier, "%d %d %d ", r, g, b);
        }
        fprintf(fichier, "\n");
    }

    fclose(fichier);

    printf("Carte generee avec succes!\nElle est disponible dans le dossier /generatedImage.\n");

}