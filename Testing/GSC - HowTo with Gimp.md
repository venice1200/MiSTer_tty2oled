------------------------ ENGLISH ------------------------

Have a look for a banner or logo of the desired picture which roughly could
be scaled down to 256\*64 pixels without losing too much details and contrast.
Start Gimp, load or insert the found picture and

- if there is a transparent layer, remove it by /Ebene/Transparenz/Alphakanal
- /Colours/Desaturate/Colour to Grey
- /Colours/Invert
- /Image/Mode/Indexed/Generate optimum palette (16 Colours)
- /Image/Scale Image/256\*64 if possible. It might be needful to resize
  the picture non-proportional. Otherwise resize to 256\*Y or X\*64 and
- use /Image/Canvas Size afterwards to move the whole thing so that it fits
- /File/Export as/ and save your work as GIF
- Start image2lcd, load your GIF ans save the result as \*.C file
  without "head data"
- Edit the created \*.C file and replace the line
  "const unsigned char gImage_x[8192]" by

//  
//  
const unsigned char _bits[8192] PROGMEM = {

Done.
------------------------ ------ ------------------------
![image2lcd settings](https://github.com/venice1200/MiSTer_tty2oled/blob/main/Pictures/image2lcd.png?raw=true)
------------------------ ------ ------------------------

------------------------ GERMAN ------------------------

Ein Banner o.ä. suchen, welches sich Pi\*Daumen auf ca. 256\*64 Pixel skalieren
lässt. Hatt man denn eines gefunden, ist die weitere Vorgehensweise wie folgt:

- Falls eine transparente Ebene vorhanden ist: /Ebene/Transparenz/Alphakanal
  entfernen
- /Farben/Entsättigen/Grau einfärben
- /Farben/Invertieren
- /Bild/Modus/Indiziert/Optimale Palette (16 Farben)
- /Bild/Bild skalieren/256\*64 sofern möglich. Manchmal reicht es auch, einfach
  nicht-proportional zu skalieren. Ansonsten auf 256\*Y oder X\*64 skalieren und
- per /Bild/Leinwandgröße das ganze zurecht rücken, so dass es passt
- /Datei/Exportieren nach/ als BMP oder GIF speichern
- Das exportierte Bild in image2lcd laden und als \*.C wieder speichern
  (ohne "head data")
- Im \*.C Export die Zeile "const unsigned char gImage_x[8192]" entfernen und
  durch ersetzen durch:

//  
//  
const unsigned char _bits[8192] PROGMEM = {

Fertig.
