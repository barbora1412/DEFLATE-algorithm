# DEFLATE Kompresor a Dekompresor

Implementácia zjednodušeného algoritmu DEFLATE, ktorý je kombináciou kódovania LZ77 + Huffman.


## Vypracovanie

Vstup je binárny súbor, ktorý je rozdelený na bloky dĺžky 2^15. Každý blok je zakódovaný najprv kódovaním *LZ77* a  potom *Huffman*. Po spracovaní *LZ77* vzniknú trojice čísel v desiatkovej sústave, ktoré sú následne zakódované Huffman do binárnej podoby. Niekedy sa stane, že po Huddman kódovaní je veľkosť dát väčšia ako pôvodná. V takomto prípade sa dáta označia ako nekomprimované a zapíšu sa v pôvodnom stave. Po kompresii uloží dáta do súboru, na ktorom prebehne dekompresia pre kontrolu. 

## LZ77

LZ77 zakóduje slová do trojíc ```(size, offset, head)```. 
* **Offset** značí, o koľko znakov sa má posunúť dozadu a prečítať **size* znakov.
* **Size** koľko znakov sa má prečítať - dĺžka stringu
* **Head** je znak, pri ktorom sa znaky z minulosti prestanú zhodovať.

Algoritmus rozdelí text do 2 častí: minulosť a budúcnosť. Ak sa v minulosti nenachádza nič (ak sa nič neprečítalo), zapíše sa konkrétny znak v podobe trojice ```(0, 0, *znak*)```. Ak sa string z budúcností nachádza v minulosti, zapíše sa ako referencia na minulosť ```(miesto začiatia stringu; dĺžka; znak, pri ktorom sa prestanú zhodovať)```

Príklad: reťazec "aaaa"

```
(0,0,a)
(0,0,a)
(2,1,a)
(0,0, NULL)
```

Autor kódu LZ77 a šablóny main @Daniel Hládek

## Huffman kódovanie

Každý blok trojíc má hlavičku, kód a pätu.

#### Hlavička (2b)
* ```10``` blok je kódovaný
* ```00``` nie je kódovaný

#### Päta(8b)
```11111111``` - označuje koniec bloku

#### Kódovanie jednej trojice

* *Length* (6b) 
  - Prvý bit je stále ```0```  , slúži ako padding, aby sa vedel rozlíšiť 
	   koniec bloku od ďalšej trojic.
  - Do zvyšných bitov sa zapíše
	   binárna hodnota čísla length

* *Distance* (8/16b)  
  - Ak je distance < 129, Prvý bit je 0 a zapíše sa zvyšných 7 bitov.
  - Ak je distance  >= 129, Prvý bit je 1 a zapíše sa zvyšných 15 bitov.

* *Head* (1b) Zapíše sa koncový znak



