Überlegungen zu den neuen ROM-Pictures

Relevant neben /tmp/CORENAME sind /tmp/CURRENTPATH und /tmp/FULLPATH. In CURRENTPATH steht die gestartete ROM-Datei des Cores und in FULLPATH der Pfad zum gestarteten ROM.

Da viele ROMs dieselbe Fileextension haben wie schon andere Cores/ROMs und Dateinamen der ROMs unzuverlässig sind, scheinen Dateiprüfsummen unausweislich.  
Es gibt für viele/alle Emulatoren ROM-Sets, aus denen man entweder eine bereits vorhandene Prüfsummendatei entnehmen (z.B. MAME) oder ggf. eine eigene generieren könnte. Aufbau der Prüfsummendatei für uns ähnlich wie gehabt:

Dateiname des Bilds immer klein ohne Extension, Unterstrich statt Leerzeichen:
__sha1sum[space]Dateiname_des_Bilds_ohne_Extension__
e4134a3b4a065c856802bc935c12fa7e9868110a battlezone  
2bbc124cead9aa49b364268735dad8cb1eb6594f james_bond_007  

(keine Extension weil **first find fits**, also gsc -> xbm -> text)
