/* Header file for the console routines for VC.
 *
 * Modifications:
 *
 * Jack Cole November 16/03 Initial implementation.
 * jc Dec 4/03 Add cprintf macro. NOTE: If using conio.h,
 *			   MUST include conio.h BEFORE console.h.
 * jc Oct/05   Add Errorp() and Yes()
 * 
 */

#if !defined(_CONSOLE_ROUTINES)

	#define _CONSOLE_ROUTINES		// make sure we do it ONCE

	void InitConsoleIO(void);
	int  PutText( char *szText );
	int  Cprintf(const char *, ...);
	int	 Errorp( const char *szFormat, ... );
	int	 Yes( const char *szFormat, ... );

	// Avoid confusion with the <conio> cprintf(). 
	// Also, preserve compatability with Borland C functions.
	#define cprintf				Cprintf
	#define clrscr				ClrScr
	#define puttext				PutText
	#define textcolor(t)		TextColour(t)
	#define textbackground(t)	TextBackground(t)
	
	// these are defines for the text colours
	// Background colours must be 0-7. Text colours may be any of them.
	#define	BC_BLACK		0
	#define	BC_BLUE			1
	#define	BC_GREEN		2
	#define	BC_CYAN			3
	#define	BC_RED			4
	#define	BC_MAGENTA		5
	#define	BC_BROWN		6
	#define	BC_LIGHTGRAY	7
	#define	BC_DARKGRAY		8
	#define	BC_LIGHTBLUE	9
	#define	BC_LIGHTGREEN	10
	#define	BC_LIGHTCYAN	11
	#define	BC_LIGHTRED	    12
	#define	BC_LIGHTMAGENTA	13
	#define	BC_LIGHTBROWN	14
	#define	BC_WHITE        15

#endif
