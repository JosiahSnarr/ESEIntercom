/* console.c: Routines to simulate some of the Borland console
 *			  functionality - cursor addressing, screen clear,
 *			  and text colours. Windowing in not yet supported.
 *
 * Modifications:
 * November 2003, Jack Cole, initial implementation.
 * Nov ??/03 jc Add colour support.
 * Dec  4/03 jc Allow a call to ClrScr() to perform InitConsoleIO()
 *			    automatically, so user doesn't have to.
 *              Change cprintf() to Cprintf() to avoid conflict with conio.
 *				#define cprintf() to Cprintf()
 *				If a Cprintf() or PutText() ENDS in a \n, will go to
 *				row 0 of next line automatically, and hide the \n.
 *				After a PutText() cursor is positioned to end
 * Mar  4/04 jc Rename some functions to more modern standards.
 * Oct   /05 jc Add Errorp() and Yes().
 */

#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <stdio.h>
#include <stdarg.h>

#include "console.h"

static HANDLE hStdout = NULL, hStdin = NULL; 
static COORD  coordCurrent;

// this is the foreground and background colour. Default is white on black.
static WORD wTextAttributes = (FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_RED);

/* InitConsoleIO() must be called before these routines will work.
 */
void InitConsoleIO(void)
{
    hStdin = GetStdHandle(STD_INPUT_HANDLE); 
    hStdout = GetStdHandle(STD_OUTPUT_HANDLE); 
    return;
}


/* GotoXY() move screen cursor to (x,y) (col, row)
 * (0,0) is top left.
 */
void GotoXY( short x, short y )
{
    coordCurrent.X = x;
    coordCurrent.Y = y;
    SetConsoleCursorPosition( hStdout, coordCurrent );
    return;
}


/* PutText() : write a buffer at the present cursor position
 *      `                  and using the present character attribute
 *                         (foregound and background colours)
 *
 * Returns 1 for success.
 */

int PutText( char *szText )
{
    CONSOLE_SCREEN_BUFFER_INFO csbi; 
    BOOL		  bSuccess;
    unsigned long cWritten;
	// QUESTION K When is iLen created, and where? What's it's storage class?
	int			  iLen;
	BOOL		  bHasNewLine;

   // Get the current text attributes.
   if( !GetConsoleScreenBufferInfo( hStdout, &csbi ))
      return(0);

    iLen = cWritten = strlen(szText);
	if ( iLen <= 0 ) return(0);

	// if the line ends in a newline, don't show it.
	bHasNewLine = (	szText[iLen-1] == '\n' );
	if ( bHasNewLine ) --iLen;
	
    // Set the buffer's attributes accordingly.
    if( !FillConsoleOutputAttribute( hStdout, csbi.wAttributes,
         cWritten, coordCurrent, &cWritten ))
        return(0);

    bSuccess = WriteConsoleOutputCharacter( hStdout, (LPWSTR)szText, iLen,
							coordCurrent, &cWritten);
    coordCurrent.X += (short)cWritten;

	// if final character written is a \n, fix next pos.
	if ( bHasNewLine ) {
	    coordCurrent.X = 0;
	    coordCurrent.Y += 1;
	}

	// Dec 4/03 jc move the cursor there
    SetConsoleCursorPosition( hStdout, coordCurrent );

    return(bSuccess);
}

/* ClrScr() : clears the screen. 
 */

void ClrScr( void )
{
    COORD coordScreen = { 0, 0 };    // home for the cursor 
    DWORD cCharsWritten;
    CONSOLE_SCREEN_BUFFER_INFO csbi; 
    DWORD dwConSize;

    if ( hStdout == NULL || hStdin == NULL )
	   InitConsoleIO();

    // Get the number of character cells in the current buffer. 
    if( !GetConsoleScreenBufferInfo( hStdout, &csbi ))
       return;
    dwConSize = csbi.dwSize.X * csbi.dwSize.Y;

    // Fill the entire screen with blanks.
    if( !FillConsoleOutputCharacter( hStdout, (TCHAR) ' ',
                            dwConSize, coordScreen, &cCharsWritten ) )
          return;

   // Get the current text attribute.
   if( !GetConsoleScreenBufferInfo( hStdout, &csbi ))
      return;

   // Set the buffer's attributes accordingly.
   if( !FillConsoleOutputAttribute( hStdout, csbi.wAttributes,
      dwConSize, coordScreen, &cCharsWritten ))
      return;

	// Put the cursor at its home coordinates.
	//   SetConsoleCursorPosition( hStdout, coordScreen );
    GotoXY(0,0);
    return;
}

/* Cprintf() - console print. Like printf. Has a 300 character
 *                        lnie limitation, after which it may go BOOM.
 * returns # of characters.
 *
 * NOTE: could be confused with Visuals cprintf(), which does
 * not follow the cursor positioning done here!
 * May be renamed in future.
 */

int  Cprintf(const char *szFormat, ...)
{
    char		szBuf[300];
    va_list		arg_ptr;
    int         i;

    /* build a buffer with the message */
    va_start( arg_ptr, szFormat );
    i = vsprintf( szBuf, szFormat, arg_ptr );
    va_end( arg_ptr );
    PutText( szBuf );
    return(i);
}

/* Errorp() : Windows style popup error. Called like a printf().
 * First character may be used to select an icon to appear.
 */

int	 Errorp( const char *szFormat, ... )
{
    char		szBuf[300];
	char		*ptr = szBuf;
    va_list		arg_ptr;
    int         i;
	int			iMode = MB_OK|MB_SETFOREGROUND;

    /* build a buffer with the message */
    va_start( arg_ptr, szFormat );
    i = vsprintf( szBuf, szFormat, arg_ptr );
    va_end( arg_ptr );
	// allow the first character to customize the box 
	if ( *ptr == '!' ) {
		iMode |= MB_ICONEXCLAMATION;
		++ptr;
	} else 
	if ( *ptr == '@' ) {
		iMode |= MB_ICONINFORMATION;
		++ptr;
	} else 
	if ( *ptr == '?' ) {
		iMode |= MB_ICONQUESTION;
		++ptr;
	} else 
	if ( *ptr == '#' ) {
		iMode |= MB_ICONSTOP;
		++ptr;
	} 

	// display the box
    MessageBox( NULL, (LPCWSTR)ptr, (LPCWSTR)"NOTE", iMode);
    return(i);
}

/* Yes() : Windows style Yes/No/Cancel box. Called like a printf().
 * Returns IDYES, IDNO, or IDCANCEL.
 */
int	 Yes( const char *szFormat, ... )
{
    char		szBuf[300];
    va_list		arg_ptr;
    int         i;

    /* build a buffer with the message */
    va_start( arg_ptr, szFormat );
    i = vsprintf( szBuf, szFormat, arg_ptr );
    va_end( arg_ptr );
    i = MessageBox( NULL, (LPCWSTR)szBuf, (LPCWSTR)"Question", MB_YESNOCANCEL|MB_SETFOREGROUND);
    return(i);
}

/* Test routine for drawing bitmaps */
int		DrawBMP( const char *szBmpFile, int x, int y )
{
	// get device context for the entire screen (?)
	HDC		hdcMem;
	BITMAP	bm;
	HBITMAP hbmOld;
	HBITMAP hBitMap = NULL;
	HDC		hdc;
	RECT	Rect;

	// sanity check
	if ( !szBmpFile ) return(0);

	// check that we got one. 
	hdc = GetDC(NULL);
	if ( hdc == NULL ) {
		Errorp("Can't get device context." );
		return(0);
	}

	// load the bitmap
    hBitMap = (HBITMAP)LoadImage(NULL, (LPCWSTR)szBmpFile, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE );
 	if (hBitMap == NULL){
		Errorp("Could not load bitmap %s", szBmpFile);
		ReleaseDC( NULL, hdc );
		return(0);
	}

	// make a memory version of the dc
	hdcMem = CreateCompatibleDC(hdc);
	if ( hdcMem == NULL ) {
		Errorp("Can't create memory context." );
		return(0);
	}

	// select the bitmap into the memory context
	hbmOld = (HBITMAP)SelectObject(hdcMem, hBitMap);
	{
        HWND hwnd = FindWindow((LPCWSTR)"ConsoleWindowClass", NULL);
		GetWindowRect(hwnd, &Rect); // NOTWOKRING!!!
	}

	// Get information about the bitmap
	GetObject(hBitMap, sizeof(bm), &bm);

	// display it
	if ( !BitBlt(hdc, Rect.left+x, Rect.top+y, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY) ) {
		Errorp("Drawing of bitmap failed %ld.", GetLastError() );
	}
 
	// clean up
	DeleteObject(hBitMap);
	SelectObject(hdcMem, hbmOld);
	DeleteDC( hdcMem );
	ReleaseDC( NULL, hdc );
	return(1);
}


/* TextColour() - equivalent of Borland TextColor().
 *
 * Parameter is BC_BLACK, BC_BLUE etc.
 */

void TextColour( int iNewTextColour )
{
	// this converts Borland fgrd colours to Visual colours. Probably is
    // an identity mapping, but who can tell anymore?

	// QUESTION J: WHen and how often is bColourMap[] initialized?
    static  char bColourMap[] = 
        { 0, FOREGROUND_BLUE, FOREGROUND_GREEN,            // black, blue green
          FOREGROUND_BLUE|FOREGROUND_GREEN,                // cyan
          FOREGROUND_RED, FOREGROUND_RED|FOREGROUND_BLUE,  // red, magenta
          FOREGROUND_RED|FOREGROUND_GREEN,                 // brown
          FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE, // grey (light?)
          FOREGROUND_INTENSITY,                            // dark grey ?
          FOREGROUND_BLUE|FOREGROUND_INTENSITY,            // bold versions     
          FOREGROUND_GREEN|FOREGROUND_INTENSITY,                           
          FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_INTENSITY,
          FOREGROUND_RED|FOREGROUND_INTENSITY, 
          FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_INTENSITY,  
          FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY,                            
          FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_INTENSITY, 
    };

    // sanity check
    if ( iNewTextColour < 0 || 
		 iNewTextColour >= (sizeof(bColourMap)/sizeof(char) ) ) return;

	// convert to Visual's colours.
    iNewTextColour = (int)bColourMap[iNewTextColour];

    // remove the present foreground colour 
    wTextAttributes &= (BACKGROUND_BLUE|BACKGROUND_GREEN|BACKGROUND_RED);

    // remove any background info from users new colour (just in case)
    iNewTextColour  &= (FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_RED|FOREGROUND_INTENSITY);

    // add new foreground to background.
    wTextAttributes |= iNewTextColour;
    SetConsoleTextAttribute( hStdout, wTextAttributes );
    return;
}

/* TextBackground() : equivalent of Borland TextBackground().
 *
 * Parameter is BC_BLACK, BC_BLUE etc.
 */

void TextBackground( int iNewBGColour )
{
	// this converts Borland bkrd colours to Visual colours. Probably is
    // an identity mapping, but who can tell anymore?
    static  char bColourMap[] = 
        { 0, BACKGROUND_BLUE, BACKGROUND_GREEN,                 // black, blue green
          BACKGROUND_BLUE|BACKGROUND_GREEN,                     // cyan
          BACKGROUND_RED, BACKGROUND_RED|BACKGROUND_BLUE,		// red, magenta
          BACKGROUND_RED|BACKGROUND_GREEN,                      // brown
          BACKGROUND_RED|BACKGROUND_GREEN|BACKGROUND_BLUE		// grey
    };

    // sanity check
    if ( iNewBGColour < 0 || 
		 iNewBGColour >= (sizeof(bColourMap)/sizeof(char) ) ) return;

    // convert to Visual's colours.
    iNewBGColour = (int)bColourMap[iNewBGColour];

    // remove the present background colour 
    wTextAttributes &= (FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_RED|FOREGROUND_INTENSITY);

    // remove any foreground info from users new colour (just in case)
    iNewBGColour  &= (BACKGROUND_BLUE|BACKGROUND_GREEN|BACKGROUND_RED);

	// add new foreground to background.
    wTextAttributes |= iNewBGColour;

    SetConsoleTextAttribute( hStdout, wTextAttributes );

    return;
}
