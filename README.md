# PIWO_server
Code for server side of TCP connection in PIWO Lite
Main idea of PIWO Lite is to make screen out of a whole building. This idea was originally made by MOS, a group of people from Wroclaw University of Technology, you can learn more about it here:
http://www.mos.pwr.edu.pl/projekt-p-i-w-o/

All communication to display a frame flows like this:
1. Prepare all pixels in hex value like "#ffffff".
2. Wrap all of them to one long string without spaces, just "#000000#00ff00#ff00ff ...". Do it in order like you read, this is from left to right, then up to down. No x and y, just one long string.
4. Send long string to main ESP (server), through TCP or UART. It will split it to all pixels and send each one to proper module (client).
5. Modules will take care of rest and display proper color.
