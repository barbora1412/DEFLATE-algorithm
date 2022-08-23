#ifndef _COMPRESSORH
#define _COMPRESSORH
#include <stdio.h>

/**
 * Skomprimuje súbor in a zapíše do súboru out.
 * @arg in smerník na otvorený vstupný súbor (na čítanie)
 * @arg out smerník na otvorený výstupný súbor (na zápis)
 */
void compress(FILE* in, FILE* out);
/**
 * Dekomprimuje súbor in a zapíše do súboru out.
 * @arg in smerník na otvorený vstupný súbor (na čítanie)
 * @arg out smerník na otvorený výstupný súbor (na zápis)
 */
void decompress(FILE* in, FILE* out);

#endif
