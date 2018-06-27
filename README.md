# PIWO_server
## EN:
##### Server side in PIWO Lite
Main idea of PIWO Lite is to make screen out of a whole building. This idea was originally made by MOS, a group of people from Wroclaw University of Technology, you can learn more about it here:
http://www.mos.pwr.edu.pl/projekt-p-i-w-o/

To dispal a frame, just send (through TCP or UART) server long, solid string of hex color values, without any spacers. Count pixels like you read, that is from left to right, then up to down. For example, if your dispay is 2x2 pxl, and you want to light up up-right and down-left, send "#000000#ffffff#FFFFFF#000000" (upper-lower case doesn't matter).
Then, server will split it, and send to all proper pixels (clients).

## PL:
#### Serwer w PIWO Lite
Zamysłem projektu PIWO Lite jest zrobienie ekranu z całego budynku. Oryginalnymi autorami tego pomysłu jest koło naukowe MOS z Politechniki Wrocławskiej, więcej o ich projekcie znajdziesz tutaj:
http://www.mos.pwr.edu.pl/projekt-p-i-w-o/

Aby wyświetlić jedną klatke, wystarczy wysłać (poprzez TCP lub UART) jeden zbity tekst zawierający wszystkie piksele w postaci koloru hex, bez odstępów czy innych symboli pomiędzy pikselami. Kolejność pikseli jest taka, jak przy czytaniu, od lewej do prawej, następnie od góry do dołu. Na przykład, klatka 2x2 piksele w której prawy-górny i lewy-dolny róg są zapalone będzie wyglądać tak: "#000000#ffffff#FFFFFF#000000" (wielkość znaków nie ma znaczenia). Serwer samodzielnie rozdzieli tekst i wyśle każdemu modułowy (pikselowi) odpowiednią wiadomość.
