/* playback.h
 */

#ifndef PLAYBACK_H
#define PLAYBACK_H

#ifdef __cplusplus
extern "C"{
#endif

int InitializeRecording(void);
int InitializePlayback(void);
int InitializeNotes(void);
int PlayNote(int iNote, int iDuration );
void ClosePlayback(void);
int WaitForNotePlayback(int iNote);
int PlayBuffer( short *piBuf, long lSamples );

#ifdef __cplusplus
}
#endif

#endif
