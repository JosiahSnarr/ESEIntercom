/* playback.h
*/

#ifndef PLAYBACK_H
#define PLAYBACK_H

#include <stdio.h>
#include <windows.h>
#include <mmsystem.h>
#include <math.h>

#include "console.h"

#undef TESTING_MAINLINE

#ifdef __cplusplus
extern "C"{
#endif

	//void SetupFormat(WAVEFORMATEX *wf);

	int InitializeRecording(void);
	int	RecordBuffer(short *piBuf, long lBufSize);
	void CloseRecording(void);
	int InitializePlayback(void);
	int InitializeNotes(void);
	int PlayNote(int iNote, int iDuration);
	void ClosePlayback(void);
	int WaitForNotePlayback(int iNote);
	int PlayBuffer(short *piBuf, long lSamples);

#ifdef __cplusplus
}
#endif

#endif
