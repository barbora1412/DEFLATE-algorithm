Riešenie úlohy je implementácia zjednodušeného algorimu DEFLATE.
DEFLATE je kombináciou LZ77 + Huffmanovo kódovanie. 

-------------------------------------------------------------------
LZ77 zakóduje slová do trojíc (size, offset, head). 
Offset značí o koľko znakov sa má posunúť dozadu a prečítať siza znakov.
Head je znak, pri ktorom sa znaky z minulosti prestanú zhodovať.
Text je rozdelený na bloky dĺžky 2^15

-------------------------------------------------------------------
Každá trojica je zakódovaná pomocou Huffmanovho kódovania. 
Do hlavičky bloku sa zapíše reťazec 10 dĺžky 2bity, čo značí, že 
blok je kódovaný. Na konci bloku sa zapíše reťazec 11111111 (8b)

Trojice sú kódované následovne:

Length(6b) Prvý bit je stále 0 - slúži ako padding, aby sa vedel rozlíšiť 
	   koniec bloku od ďalšej trojice. Do zvyšných bitov sa zapíše
	   binárna hodnota čísla length

Distance(8/16b)  Ak je distance < 129, Prvý bit je 0 a zapíše sa 8 bitov.
		 Ak je distance  >= 129, Prvý bit je 1 a zapíše sa 16 bitov.

Head(1b) Zapíše sa koncový znak

------------------------------------------------------------------

Komprimácia funguje len na špecifické súbory. Veľkosť po kompresii
 je niekedy väčšia ako pôvodná. 
Po dekompresii veľkosť sedí s pôvodným súborom, ale zle sa vypisujú znaky.

-----------------------------------------------------------------
Kódovanie lz77 + šablóna: Daniel Hládek


