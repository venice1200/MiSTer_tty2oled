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
