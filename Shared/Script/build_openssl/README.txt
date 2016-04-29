***********************
*   PREREQUISITI:     *
***********************
- Microsoft Visual Studio 2012;
- ActivePerl (scaricare qui: http://www.activestate.com/activeperl/downloads );
- Nasm per compilazione a 32 bit (scaricare qui: http://www.nasm.us/pub/nasm/releasebuilds/2.11.08/win32/ );
- aggiungere alla variable d'ambiente PATH il percorso: NASM_HOME (default NASM_HOME = C:\Program Files (x86)\nasm).
- Curl for windows (scaricare da qui: http://curl.haxx.se/download.html#Win32 );
- copiare tutto il contenuto dell'archivio curl in C:\Windows;
- 7-zip (scaricare da qui: http://www.7-zip.org/ );
- aggiungere alla variable d'ambiente PATH il percorso: 7Z_HOME (default 7Z_HOME = C:\Program Files\7-Zip).

****************************
*   COMPILAZIONE WIN:      *
****************************

lanciare lo script di compilazione: "build_ssl_win.bat" (lista parametri: build_ssl_win.bat --help)

il risultato si trova in C:\OpenSSL_$ARCH_$VERSION (es: C:\OpenSSL_x64_1.0.2g)