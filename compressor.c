/**
 *Author kodovanie lz77+sablona: Daniel Hladek
 *Algoritmus: inspiracia DEFLATE: lz77+huffman
 */

#define _GNU_SOURCE
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "compressor.h"

#define BUFSIZE 32768 //Okno na prechadzanie "minulosti" pre lz77
#define BSIZE 31 //Okno na prechadzanie "buducnosti" pre lz77
#define HEADER_BITS 2 //Dlzka bitov v hlavicke
#define END_BLOCK_L 8 //Dlzka bitov pre oznacenie konca bloku

//Struktura na ulozenie trojice pre lz77
struct node {
  int offset; //kolko sa treba posunut do minulosti
  int size; //kolko znakov zapisat
  char head; //koncovy znak
};

void delNode(struct node * nodeArray, int j);
int mycompress(char * buff, int size, char * outbuf);
void lz77Compress(char * buff, int sizesize, char * outbuf, struct node ** codeWords, int * count);
void intToBin(char * binaryCodes, int length, int number);
int binToInt(char * binaryCodes, int length);
int codeTriples(struct node * codeWords, int count, int buffSize, char * outbuff, char * inbuff);
int checkEndBlock(int * pointer, char * buffer);

/**
 *Uvolni pamat alokovanu pre pole nodeArray
 *@arg pole so strukturami v ktorych su ulozene trojice lz77
 *@arg velkost pola
 */
void delNode(struct node * nodeArray, int j) {

  for (int i = 0; i < (j); i++) {
    free( & nodeArray[i]);
  }
  free(nodeArray);
}

/**
 *Zavola kodovanie lz77, huffmanovo kodovanie
 *@arg vstupne pole, ktore sa ma zakodovat
 *@arg kolko znakov je v bufferi
 *@arg buffer, do ktoreho sa ma zapisat vystup
 *returns: pocet znakov, ktore sa maju zapisat do suboru
 */
int mycompress(char * buff, int size, char * outbuf) {

  struct node * codeWords = NULL; //Pole na ulozenie trojic pre lz77
  int count = 0; //celkovy pocet trojic v poli
  lz77Compress(buff, size, outbuf, & codeWords, & count); //lz77 kodovanie
  int out = codeTriples(codeWords, count, size, outbuf, buff); //huffman kodovanie

  free(codeWords);

  return out;
}
/**
 *Vlozi do pola struktur trojice zakodovane pomocou lz77
 *@arg vstupne pole znakov
 *@arg pocet znakov vo vstupnom poli
 *@arg vystupne pole
 *@arg pole struktur pre trojice
 *@arg pocet struktur v poli
 */
void lz77Compress(char * buff, int sizesize, char * outbuf, struct node ** codeWords, int * count) {
  int head = 0;
  int osz = 0;
  int dsz = sizesize; //kolko pismen je v bufferi

  while (head < dsz && buff[head] > 0) {
    // Search buffer is before head
    // Search buffer size
    int bsz = BUFSIZE;
    if (head < bsz) {
      bsz = head;
    }
    // Search buffer start
    // Search buffer contains data from the past
    char * buffer = buff + head - bsz;
    // Lookahead buffer start
    // Lookahead buffer contains data in the future
    char * window = buff + head;
    // Lookahead buffer
    int wsz = BSIZE;
    if ((dsz - head) < wsz) {
      wsz = BSIZE - head;
    }
    // We search in search buffer
    // for the longest string in lookahead that starts with head
    // search and lookahead buffers can have different sizes
    // Max largest match size is the minimum
    int msz = wsz;
    if (bsz < wsz) {
      msz = bsz;
    }
    // Offset of the match in the past
    int offset = -1;
    // Size of the match in the past
    int size = -1;
    for (int i = 1; i < msz; i++) {
      // Searches window,i in buffer,bsz
      char * r = memmem(buffer, bsz, window, i);
      if (r > 0) {
        offset = bsz - (r - buffer);
        size = i + 1;
      } else {
        break;
      }
    }

    * count += 1;

    if ( * count - 1 == 0) {
      * codeWords = malloc( * count * sizeof(struct node));

    } else {
      struct node * tmp = realloc( * codeWords, ( * count) * sizeof(struct node));
      if (tmp == NULL) {

        delNode( * codeWords, ( * count) - 1);
        assert(tmp);
      } else {
        * codeWords = tmp;
      }
    }
    struct node * tmpNode = calloc(1, sizeof(struct node));

    if (size > 0) {
      tmpNode -> head = buff[head + size - 1];
      tmpNode -> size = size - 1;
      tmpNode -> offset = offset;

      head += size;

    } else {
      tmpNode -> head = buff[head];
      tmpNode -> size = 0;
      tmpNode -> offset = 0;

      head += 1;
    }

    memcpy( & (( * codeWords)[( * count) - 1]), tmpNode, sizeof(struct node));
    free(tmpNode);

    osz += 1;
  }
  printf("%d characters into %d codewords\n", dsz, osz);

}

/**
* Premeni int na cislo v binarnom tvare, vysledok zapise do pola
*@arg pole do ktore sa zapise vysledok
*@arg dlzka pola, v ktorom bude vysledok
*/
void intToBin(char * binaryCodes, int length, int number) {

  for (int i = length - 1; i >= 0; i--) {
    binaryCodes[i] = number % 2;

    if (number % 2 == 1) {
      binaryCodes[i] = '1';
      number = (number - 1) / 2;
    } else {
      binaryCodes[i] = '0';
      number = number / 2;
    }
  }

}

/**
* Premeni cislo v binarnom tvare do int
*@arg pole v ktorom sa nachadza cislo v binarnom tvare
*@arg dlzka pola, v ktorom je binarne cislo
*returns: cislo v desiatkovej sustave
*/
int binToInt(char * binaryCodes, int length) {
  int out = 0;
  for (int i = 1; i < length + 1; i++) {
    if (binaryCodes[length - i] == '1') {
      out += pow(2, i - 1);
      //printf("%f ", pow(2,i));
    }

  }
  return out;

}

void printArray(char * array, int length) {
  for (int i = 0; i < length; i++) {
    printf("%c", array[i]);
  }
  printf("\n");
}
/**
* Zakoduje trojice huffmanovym kodom: inspiracia DEFLATE
* Header: dva bity: 10 znacia kompresiu
* Trojica:
*         Length: 1+5 bitov, 1b hodnoty 0 pre padding aby sa vedelo odlisit, kde konci blok
*         Distance: 1+7/15 bitov.
*                    Ak je Distance > 129 tak 1.bit je 0 a zvysnych 7 je cislo v binarnom tvare
*                                  <= 129 tak 1.bit je 1 a zvysnych 15 je cislo v binarnom tvare
*          Head: 1b - znak ktory sa zapise
*Koniec bloku: znaceny 8b same jednotky 11111111
*@arg pole v ktorom sa nachadzaju trojice
*@arg pocet znakov v pociatocnom bufferi
*@arg buffer do ktoreho sa zapisuje vysledok
@arg vstupny buffer
@arg pocet trojic v poli
*returns kolko znakov sa ma zapisat do suboru
*/
int codeDynamic(struct node * codeWords, int buffSize, char * outbuff, char * inbuff, int count) {
  /* Hlavicka */
  int outBufSize = HEADER_BITS;
  outbuff[0] = '1';
  outbuff[1] = '0';

  for (int i = 0; i < count; i++) {
     /* Zapisanie length ako 1+5b */
    char length[5];
    intToBin(length, 5, (codeWords[i]).size);

    outBufSize += 1; //padding pre length
    outbuff[outBufSize - 1] = '0';

    for (int j = 1; j < 6; j++) {
      outbuff[outBufSize + j - 1] = length[j - 1];

    }
    outBufSize += 5;
    /* Zapisanie Distance ako 1+7/15b */
    int lDistance = 0;
    if ((codeWords[i]).offset < 129) {
      lDistance = 7;
      outbuff[outBufSize] = '0';
    } else {
      lDistance = 15;

      outbuff[outBufSize] = '1';
    }
    outBufSize += 1;
    char distance[lDistance];
    intToBin(distance, lDistance, (codeWords[i]).offset);
    //printArray(distance, lDistance);
    for (int j = 1; j < lDistance + 1; j++) {

      outbuff[outBufSize + j - 1] = distance[j - 1];
    }
    outBufSize += lDistance;
    /* zapisanie Head*/
    outBufSize += 1;
    outbuff[outBufSize - 1] = (codeWords[i]).head;

  }
    /*Zapis konca bloku*/
  for (int i = 1; i < 9; i++) {
    outbuff[outBufSize + i - 1] = '1';
  }
  outBufSize += 8;

  return outBufSize;

}
/*
* Zavola vhodnu funkciu na kodovanie trojic
*@arg pole v ktorom sa nachadzaju trojice
*@arg pocet znakov v pociatocnom bufferi
*@arg buffer do ktoreho sa zapisuje vysledok
@arg vstupny buffer
@arg pocet trojic v poli
*returns: kolko znakov sa ma zapisat do suboru
*/
int codeTriples(struct node * codeWords, int count, int buffSize, char * outbuff, char * inbuff) {
  int outBufSize = 0;
  outBufSize = codeDynamic(codeWords, buffSize, outbuff, inbuff, count);

  return outBufSize;

}

void compress(FILE * infile, FILE * outfile) {
  char buffer[BUFSIZE];
  memset(buffer, 0, BUFSIZE);
  while (1) {
    int insize = fread(buffer, sizeof(char), BUFSIZE, infile);
    if (insize == 0) {
      if (feof(infile)) {
        // end of file
        break;
      }
      assert(!ferror(infile));
    }
    char outbuf[BUFSIZE * 2];
    memset(outbuf, 0, BUFSIZE * 2);
    // Doplnte implementaciu kompresie.
    int outsize = mycompress(buffer, insize, outbuf);
    if (outsize > 0) {
      fwrite(outbuf, sizeof(char), outsize, outfile);
    }
  }
}
/*
* Skontroluje, ci sa pri dekomprimacii za trojicou lz77 nachadza koniec bloku
* alebo nasleduje dalsia trojica lz77.
* Ak najde 0, znamena to ze narazil na padding pre Length a nasleduje trojica
*@arg Pocet precitanych znakov v bufferi
*@arg Buffer, z ktoreho sa cita
*returns: 0 ak nie je koniec bloku
*         1 ak je koniec bloku
*/
int checkEndBlock(int * pointer, char * buffer) {

  for (int i = 1; i < 9; i++) {

    if (buffer[ * pointer - 1 + i] == '0') {
      return 0;
    }

  }
  * pointer += 8;
  return 1;
}
/*
* Dekoduje lz77 trojice
*@arg kolko sa ma zapisat z minulosti
*@arg o kolko sa posunut do minulosti
*@arg koncovy znak
*@arg kolko uz je zapisanych znakov vo vyslednom bufferi
*@arg vysledny buffer
*@arg subor, do ktoreho sa zapisuje vystup
*/
void decodeLZ77(int length, int distance, char head, int * out, char * outbuf, FILE * outfile) {
  int size = 0;
  /* trojica 0 0 +znak */
  printf("l d h %d %d %c\n", length, distance, head);
  if (distance == 0 && length == 0) {
    size += 1;
    fputc(head, outfile);

  }
  /* trojica kde treba pozerat do minulosti */
  else {
    char tmpBuff[length];
    memset(tmpBuff, 0, length);

    fseek(outfile, * out - distance, SEEK_SET);

    int t = fread(tmpBuff, sizeof(char), length, outfile);
    printArray(tmpBuff, length);
    fseek(outfile, * out, SEEK_SET);

    printf("%d\n", t);

    for (int i = 1; i < length + 1; i++) {

      fputc(tmpBuff[i - 1], outfile);

    }

    size += length;
    size += 1;
    fputc(head, outfile);

  }

  * out += size;
  printf("out, size %d %d\n", * out, size);
}
/*
*Dekoduje 1 blok zakodovany lz77+huffman
*@arg pole vstupnych znakov
*@arg pocet znakov v stupnom poli
*@arg pole vystupnych znakov
*@arg pocet precitanych znakov zo vstupneho suboru
*@arg pocet znakov na zapisanie
*@arg subor na zapisanie
*/
void mydecompress(char * buffer, int insize, char * outbuf, int * pointer, int * out, FILE * outfile) {

  if (buffer[0] == '1' && buffer[1] == '0') {
    * pointer += 2;
    while (1) {
      char length[5];
      * pointer += 1;
      //printf("char %d\n", *pointer);
      for (int i = 1; i < 6; i++) {
        length[i - 1] = buffer[ * pointer - 1 + i];
      }
      //printArray(length, 5);
      * pointer += 5;

      int decLength = binToInt(length, 5);
      int lDistance = 0;
      * pointer += 1;
      if (buffer[ * pointer - 1] == '0') {
        lDistance = 7;
      } else {
        lDistance = 15;
      }

      char distance[lDistance];
      for (int i = 1; i < lDistance + 1; i++) {
        distance[i - 1] = buffer[ * pointer - 1 + i];

      }
      //printArray(distance,lDistance);

      * pointer += lDistance;
      int decDistance = binToInt(distance, lDistance);
      * pointer += 1;
      char head = buffer[ * pointer - 1];
      //printf("%d\n", *pointer);
      decodeLZ77(decLength, decDistance, head, out, outbuf, outfile);
      //ak je konec bloku
      int endofBlock = checkEndBlock(pointer, buffer);
      if (endofBlock) {

        return;
      }
    }
  }

  return;
}

void decompress(FILE * infile, FILE * outfile) {

  char buffer[BUFSIZE * 2]; //buffer na nacitanie znakov zo suboru
  int pPointer = 0; // pozicia spracovanych bitov v subore
  int pointer = 0; //pozicia bitu ktory sa spracovava v bufferi
  /* Zisti velkost suboru */
  fseek(infile, 0, SEEK_END);
  int sz = ftell(infile);
  fseek(infile, 0, SEEK_SET);
  int out = 0; //kolko znakov sa zapisalo v outfile
  while (1) {
    memset(buffer, 0, BUFSIZE * 2);

    int insize = fread(buffer, sizeof(char), 2 * BUFSIZE, infile);

    char outbuf[BUFSIZE * 2];
    memset(outbuf, 0, BUFSIZE * 2);

    mydecompress(buffer, insize, outbuf, & pointer, & out, outfile);
    pPointer += pointer;
    pointer = 0;

    if (sz == pPointer) break; //ak sa precitali vsetky znaky zo suboru skonci
    fseek(infile, pPointer, SEEK_SET); //ak nie, tak sa posun na poziciu v subore, kde bol koniec bloku

  }
}