/* Playback.cpp
*
* Functions for playback of audio data, to be adapted for S2, Project 2.
* This version creates buffers for all notes from C0 to D#8.
*
* A testing main() is included that can be used in the following ways:
* playback #1 #2  : play a half second of note number #1 then #2 etc (up to 10), 1 - NFREQUENCIES
* playback A-Ga-g : play a half second of the notes A-G or a-g (up to 10)
* playback r : record 6 seconds and then play it back.
*
* Future: Follow the note with a duration as in ABC notation. Same with recording time.  Let note 0 or Z mean silence.
*/

#define TESTING


#include "playback.h"
#include "globals.h"

#define	DEFAULT_NSAMPLES	4000	// 8000 is one second
#define MIN_BUFSIZE			1000

static float fFrequencies[] = {
	/* C0 */  16.35f, 17.32f, 18.35f, 19.45f, 20.6f, 21.83f, 23.12f, 24.5f, 25.96f, 27.5f, 29.14f, 30.87f,			// 0 - 11
	/* C1 */  32.7f, 34.65f, 36.71f, 38.89f, 41.2f, 43.65f, 46.25f, 49.0f, 51.91f, 55.0f, 58.27f, 61.74f,			// 12-23
	/* C2 */  65.41f, 69.3, 73.42, 77.78, 82.41, 87.31, 92.5, 98.0, 103.83, 110.0f, 116.54f, 123.47f,			// 24-35
	/* C3 */ 130.81f, 138.59f, 146.83f, 155.56f, 164.81f, 174.61f, 185.0f, 196.0f, 207.65f, 220.0f, 233.08f, 246.94f,			// 36-47
	/* C4 */ 261.63f, 277.18f, 293.66f, 311.13f, 329.63f, 349.23f, 369.99f, 392.0f, 415.30f, 440.0f, 466.16f, 493.88f,			// 48-59
	/* C5 */ 523.25f, 554.37f, 587.33f, 622.25f, 659.26f, 698.46f, 739.99f, 783.99f, 830.6f, 880.0f, 932.33f, 987.77f,			// 60-71
	/* C6 */1046.5f, 1108.73f, 1174.66f, 1244.51f, 1318.51f, 1396.91f, 1479.98f, 1567.98f, 1661.22f, 1760.0f, 1864.66f, 1975.53f,
	/* C7 */2093.f, 2217.46f, 2349.32f, 2489.02f, 2637.02f, 2793.83f, 2959.96f, 3135.96f, 3322.44f, 3520.0f, 3729.31f, 3951.07f
};

#define	NFREQUENCIES (sizeof(fFrequencies)/sizeof(float))
#define RECORD_TIME		6		// seconds
#define SAMPLES_SEC		8000	// samples per second

// Buffers for the notes, plus 1 big buffer for recording.
static short *pFreqBufs[NFREQUENCIES] = { NULL };
static short nSamples[NFREQUENCIES] = { 0 };
static short iBigBuf[SAMPLES_SEC * RECORD_TIME];
static long	 lBigBufSize = SAMPLES_SEC * RECORD_TIME;	// in samples

// output and input channel parameters
static	int		g_nSamplesPerSec = SAMPLES_SEC;
static	int		g_nBitsPerSample = 16;
static	HWAVEOUT	HWaveOut;				/* Handle of opened WAVE Out and In device */
static  HWAVEIN		HWaveIn;
static	WAVEFORMATEX WaveFormat;			/* WAVEFORMATEX structure for reading in the WAVE fmt chunk */
static  WAVEHDR	WaveHeader[NFREQUENCIES];	/* WAVEHDR structures - 1 per buffer */
static  WAVEHDR	WaveHeaderSilence;
static  WAVEHDR WaveHeaderIn;

static int WaitOnHeader(WAVEHDR *wh, char cDit);

#define Errorp printf

/* SetupFormat() initializes a WAVEFORMATEX strucutre to the required
*				 parameters (sample rate, bits per sample, etc)
*/
static void SetupFormat(WAVEFORMATEX *wf)
{
/*
	wf->wFormatTag = WAVE_FORMAT_PCM;
	wf->nChannels = 1;
	wf->nSamplesPerSec = g_nSamplesPerSec;
	wf->wBitsPerSample = g_nBitsPerSample;
	wf->nBlockAlign = wf->nChannels * (wf->wBitsPerSample / 8);
	wf->nAvgBytesPerSec = wf->nSamplesPerSec * wf->nBlockAlign;
	wf->cbSize = 0;
*/
    wf->wFormatTag = WAVE_FORMAT_PCM;
	wf->nChannels = nChannels;
	wf->nSamplesPerSec = nSamplesPerSec;
	wf->wBitsPerSample = nBitsPerSample;
	wf->nBlockAlign = wf->nChannels * (wf->wBitsPerSample / 8);
	wf->nAvgBytesPerSec = wf->nSamplesPerSec * wf->nBlockAlign;
	wf->cbSize = cbSize;
	return;
}

/* FreeNoteBuffers() releases the buffers holding the set of all notes.
*/
static void FreeNoteBuffers(void)
{
	unsigned int i;
	for (i = 0; i < NFREQUENCIES; ++i) {
		if (pFreqBufs[i]) free(pFreqBufs[i]);
	}
	return;
}

/* CreateSound: This routine fills a buffer with a tone at a specified
*				frequency.
*				TODO: add some harmonics to make it sound nice?
*				TODO: use different volumes?
*
* Returns the index of the last 0 in the buffer, This can be used by the caller
* to ensure that the buffer ends on a 0 crossing, handy for splicing. TODO: Should probably
* make sure it's a 0 crossing that complete a cycle.
*/
static int CreateSound(short *pBuf, int nSamples, float fFreq)
{
	int		i, iLastZero;
	double	lfTime, lfTimeStep, lfValue, lfAmp, lfOmega;

	lfTime = 0.0;
	lfTimeStep = (double)1.0 / (float)g_nSamplesPerSec;

	// calculate 2*PI*f
	lfOmega = 2.0 * 3.14159 * fFreq;

	// make the maximum signal 75% of the range
	switch (g_nBitsPerSample) {
	case 16: lfAmp = 65536 * .75; break;
	case 8: lfAmp = 256 * .75; break;
	default:
		Errorp("Bad playback sample size. Using 8 bits.");
		lfAmp = 256.0 * .75;
	}
	lfAmp /= 2.0;	// making it signed so half range. 

	iLastZero = 0;
	for (i = 0; i < nSamples; ++i) {
		// TODO for every point in the curve....init to first curve, then add rest. 
		lfValue = (float)(lfAmp*sin(lfOmega*lfTime));
		if (lfValue >= 0.0)
			*pBuf = (short)(lfValue + 0.5);
		else
			*pBuf = (short)(lfValue - 0.5);
		if (labs(*pBuf) < 320) iLastZero = i;	// close enough to 0
		++pBuf;
		lfTime += lfTimeStep;
	}
	return(iLastZero);
}

/* InitializePlayback()
*/

int	InitializePlayback(void)
{
	int		rc;

	// set up the format structure, needed for playback (and recording)
	SetupFormat(&WaveFormat);

	// open the playback device
	rc = waveOutOpen(&HWaveOut, WAVE_MAPPER, &WaveFormat, (DWORD)NULL, 0, CALLBACK_NULL);
	if (rc) {
		Errorp("Unable to open Output sound Device! Error %x.", rc);
		return(0);
	}

	return(1);
}

/* Initializ es() : create buffers for however many notes
* in the score.
*/
int InitializeNotes(void)
{
	int		nFreqs, i;
	int		iLastZero;
	MMRESULT rc;

	// determine the number of buffers required (one per frequency)
	nFreqs = NFREQUENCIES;

	// for each buffer, determine space and allocate it, then fill it
	for (i = 0; i < nFreqs; ++i) {
		// how many samples make up 1 minimum note duration? 
		nSamples[i] = DEFAULT_NSAMPLES;

		pFreqBufs[i] = (short *)malloc(nSamples[i] * sizeof(short));
		if (pFreqBufs[i] == NULL) {
			// out of space
			Errorp("Unable to allocate space for note %d %d. Playback disabled.",
				i, nSamples[i]);
			FreeNoteBuffers();
			return(0);
		}

		// fill the buffer with a note of chosen frequency
		iLastZero = CreateSound(pFreqBufs[i], nSamples[i], fFrequencies[i]);

#ifdef NEVER
		// shorten the buffer so that it ends on a zero crossing, to make it 
		// sound better when played. But not too short.
		if (iLastZero > MIN_BUFSIZE) {
			nSamples[i] = iLastZero + 1;
			// TODO - free old buffer and create new. realloc() not efficient. 
			// realloc(pFreqBufs[i], nSamples[i]*sizeof(short) );
		}
#endif
	}

	if (HWaveOut == NULL)
		InitializePlayback();

	// prepare the buffer headrs for use later on
	for (i = 0; i < nFreqs; ++i) {
		WaveHeader[i].lpData = (char *)pFreqBufs[i];
		WaveHeader[i].dwBufferLength = nSamples[i] * sizeof(short);
		rc = waveOutPrepareHeader(HWaveOut, &WaveHeader[i], sizeof(WAVEHDR));
		if (rc) {
			Errorp("Failed preparing WAVEHDR, error 0x%x.", rc);
			return(0);
		}
	}

#ifdef NEVER
	// Prepare a bit of silence. TODO check for errors. 
	WaveHeaderSilence.lpData = (char *)calloc(DEFAULT_NSAMPLES * sizeof(short), 1);
	WaveHeaderSilence.dwBufferLength = DEFAULT_NSAMPLES*sizeof(short);
	WaveHeaderSilence.dwFlags &= ~(WHDR_BEGINLOOP | WHDR_ENDLOOP);
	rc = waveOutPrepareHeader(HWaveOut, &WaveHeaderSilence, sizeof(WAVEHDR));
	if (rc) {
		Errorp("Failed preparing WAVEHDR for silence, error 0x%x.", rc);
		return(0);
	}
#endif

	return(1);
}


/* PlayBuffer()
*/
int PlayBuffer(short *piBuf, long lSamples)
{
	static	WAVEFORMATEX WaveFormat;	/* WAVEFORMATEX structure for reading in the WAVE fmt chunk */
	static  WAVEHDR	WaveHeader;			/* WAVEHDR structure for this buffer */
	MMRESULT	mmErr;
	int		rc;

	// stop previous note (just in case)
	waveOutReset(HWaveOut);   // is this good?

	// get the header ready for playback
	WaveHeader.lpData = (char *)piBuf;
	WaveHeader.dwBufferLength = lSamples*sizeof(short);
	rc = waveOutPrepareHeader(HWaveOut, &WaveHeader, sizeof(WAVEHDR));
	if (rc) {
		Errorp("Failed preparing WAVEHDR, error 0x%x.", rc);
		return(0);
	}
	WaveHeader.dwFlags &= ~(WHDR_BEGINLOOP | WHDR_ENDLOOP);

	// play the buffer. This is NON-blocking.
	mmErr = waveOutWrite(HWaveOut, &WaveHeader, sizeof(WAVEHDR));
	if (mmErr != MMSYSERR_NOERROR) {
		char	szErrBuf[150];
        waveOutGetErrorText(mmErr, (LPWSTR)szErrBuf, sizeof(szErrBuf));
		Errorp("Error 0x%x writing to playback device. %s", mmErr, szErrBuf);
		return(0);
	}

	// wait for completion
	rc = WaitOnHeader(&WaveHeader, 0);

	// give back resources
	waveOutUnprepareHeader(HWaveOut, &WaveHeader, sizeof(WAVEHDR));

	return(rc);
}

/* PlayNote() : the specified note is played for the specified duration.
*
* Buffers of all expected frequencies already exist.
* If less than a buffer is required, the length is shortened.
* If more than a buffer is required, the buffer is repeated.
* PROBLEM: Only want to issue one write. so stuck with a multiple.
*/
int PlayNote(int iNote, int iDuration)	// duration in mS
{
	MMRESULT	mmErr;
	int			iBuffers, iBufSamples;
	long		lSamples;

	if (iNote < 0 || iNote >= NFREQUENCIES) return(0);

	// figure out how much to play. For now only 2 choices - part of 1 buffer 
	// or repetitions of full buffer. 
	lSamples = (int)(((double)iDuration * (double)g_nSamplesPerSec) / 1000. + .9);
	if (lSamples > nSamples[iNote]) {
		// need to repeat the buffer. Figure out how many times, then shorten the buffer 
		// make the total length <= desired. 
		WaveHeader[iNote].dwLoops = iBuffers = (lSamples + nSamples[iNote] - 1) / nSamples[iNote];
		iBufSamples = lSamples / iBuffers;
		WaveHeader[iNote].dwBufferLength = iBufSamples * sizeof(short);
		WaveHeader[iNote].dwFlags |= (WHDR_BEGINLOOP | WHDR_ENDLOOP);
	}
	else {
		// need to play just part of the buffer
		WaveHeader[iNote].dwBufferLength = lSamples * sizeof(short);
		WaveHeader[iNote].dwFlags &= ~(WHDR_BEGINLOOP | WHDR_ENDLOOP);
	}
	//	iBufTime = (nSamples[i]*1000) / g_nSamplesPerSec;
	//	iBuffers = iDuration / iBufTime; 

	// stop previous note (just in case)
	// waveOutReset(HWaveOut);   // is this good?

	// play the note. This is NON-blocking.
	mmErr = waveOutWrite(HWaveOut, &WaveHeader[iNote], sizeof(WAVEHDR));
	if (mmErr != MMSYSERR_NOERROR) {
		char	szErrBuf[150];
        waveOutGetErrorText(mmErr, (LPWSTR)szErrBuf, sizeof(szErrBuf));
		Errorp("Error 0x%x writing to playback device. %s", mmErr, szErrBuf);
		return(-1);
	}

#ifdef NEVER
	// play some silence after the note to eiminate "clicks"
	mmErr = waveOutWrite(HWaveOut, &WaveHeaderSilence, sizeof(WAVEHDR));
	if (mmErr != MMSYSERR_NOERROR) {
		char	szErrBuf[150];
		waveOutGetErrorText(mmErr, szErrBuf, sizeof(szErrBuf));
		Errorp("Error 0x%x writing silence to playback device. %s", mmErr, szErrBuf);
		return(-1);
	}
#endif

	return(0);
}


static int WaitOnHeader(WAVEHDR *wh, char cDit)
{
	long	lTime = 0;

	// wait for whatever is being played, to finish. Quit after 10 seconds.
	for (;;) {
		if (wh->dwFlags & WHDR_DONE) return(1);
		// idle for a bit so as to free CPU
		Sleep(100L);
		lTime += 100;
		if (lTime >= timeout) return(0);  // timeout
		if (cDit) printf("%c", cDit);
	}
}

/* WaitForNotePlayback() : wait for the playback of specified note to finish.
* If note is playing more than once, this will return the current one - bit of a hole.
*/
int WaitForNotePlayback(int iNote)
{
	// wait for whatever is being played, to finish. Quit after 10 seconds.
	return (WaitOnHeader(&WaveHeader[iNote], 0));
}

void ClosePlayback(void)
{
	int		i;

	for (i = 0; i < NFREQUENCIES; ++i) {
		waveOutUnprepareHeader(HWaveOut, &WaveHeader[i], sizeof(WAVEHDR));
	}

	// close the playback device
	waveOutClose(HWaveOut);

	return;
}

/* RECORDING ********************************************************************/

int InitializeRecording(void)
{
	MMRESULT rc;

	// set up the format structure, needed for recording.
	SetupFormat(&WaveFormat);

	// open the recording device
	rc = waveInOpen(&HWaveIn, WAVE_MAPPER, &WaveFormat, (DWORD)NULL, 0, CALLBACK_NULL);
	if (rc) {
        Errorp("Unable to open Input sound Device! Error %x.\n", rc);

        switch(rc){

        case MMSYSERR_ALLOCATED:
            Errorp("already allocated\n");
            break;

        case MMSYSERR_BADDEVICEID:
            Errorp("Bad device\n");

            break;

        case MMSYSERR_NODRIVER:
            Errorp("no driver\n");
            break;

        case WAVERR_BADFORMAT:
            Errorp("bad format\n");
            break;

        case MMSYSERR_NOMEM:
            Errorp("no mem\n");
            break;

        }

        return(0);
	}

	// prepare the buffer header for use later on
	WaveHeaderIn.lpData = (char *)iBigBuf;
	WaveHeaderIn.dwBufferLength = lBigBufSize*sizeof(short);
	rc = waveInPrepareHeader(HWaveIn, &WaveHeaderIn, sizeof(WAVEHDR));
	if (rc) {
		Errorp("Failed preparing input WAVEHDR, error 0x%x.", rc);
		return(0);
	}

	return(1);

}

/* RecordBuffer() : FiIl in the buffer from input device.
* Returns 0 for failure.
*/
int	RecordBuffer(short *piBuf, long lBufSize)
{
	static	WAVEFORMATEX WaveFormat;	/* WAVEFORMATEX structure for reading in the WAVE fmt chunk */
	static  WAVEHDR	WaveHeader;			/* WAVEHDR structure for this buffer */
	MMRESULT	mmErr;
	int		rc;

	// stop previous recording (just in case)
	waveInReset(HWaveIn);   // is this good?

	// get the header ready for recording.  This should not be needed here AND in init.
	WaveHeader.lpData = (char *)piBuf;
	WaveHeader.dwBufferLength = lBufSize*sizeof(short);
	rc = waveInPrepareHeader(HWaveIn, &WaveHeader, sizeof(WAVEHDR));
	if (rc) {
		Errorp("Failed preparing WAVEHDR, error 0x%x.", rc);
		return(0);
	}
	WaveHeader.dwFlags &= ~(WHDR_BEGINLOOP | WHDR_ENDLOOP);

	// Give the buffer to the recording device to fill.
	mmErr = waveInAddBuffer(HWaveIn, &WaveHeader, sizeof(WAVEHDR));
	if (mmErr != MMSYSERR_NOERROR) {
		char	szErrBuf[150];
        waveOutGetErrorText(mmErr, (LPWSTR)szErrBuf, sizeof(szErrBuf));
		Errorp("Error 0x%x writing to playback device. %s", mmErr, szErrBuf);
		return(0);
	}

	// Record the buffer. This is NON-blocking.
	mmErr = waveInStart(HWaveIn);
	if (mmErr != MMSYSERR_NOERROR) {
		char	szErrBuf[150];
        waveOutGetErrorText(mmErr, (LPWSTR)szErrBuf, sizeof(szErrBuf));
		Errorp("Error 0x%x writing to playback device. %s", mmErr, szErrBuf);
		return(0);
	}

	// wait for completion
	rc = WaitOnHeader(&WaveHeader, '.');

	printf("Done\n");

	// probably not necessary
	waveInStop(HWaveIn);

	return(rc);
}

void CloseRecording(void)
{
	waveInUnprepareHeader(HWaveIn, &WaveHeaderIn, sizeof(WAVEHDR));

	// close the playback device
	waveInClose(HWaveIn);

	return;
}


#ifdef TESTING  // a testing mainline

static int LetterToIndex(char *pLetter)
{
	switch (*pLetter) {
	case 'A': return(46);
	case 'B': return(48);
	case 'C': return(49);
	case 'D': return(51);
	case 'E': return(53);
	case 'F': return(54);
	case 'G': return(56);
	case 'a': return(58);
	case 'b': return(60);
	case 'c': return(61);
	case 'd': return(63);
	case 'e': return(65);
	case 'f': return(66);
	case 'g': return(68);
	default: return(0);
	}
}

#define MAX_NOTES	16

#define RECORDING
#ifdef TESTING_MAINLINE

int	main(int argc, char *argv[])
{
#ifndef RECORDING
	short	iNotes[MAX_NOTES + 1] = { -1 };
	int		i, nNotes;
	int		iTestPlayRecord = 0;

	// default is play a single note and exit
	if (argc == 1) {
		iNotes[0] = NFREQUENCIES / 2;
		iNotes[1] = -1;
	}
	else {
		if (toupper(*argv[1]) == 'R') {
			iTestPlayRecord = 1;
		}
		else {
			// play notes 
			for (nNotes = 0; nNotes < MAX_NOTES && nNotes < (argc - 1); ++nNotes) {
				if (isalpha(*(argv[nNotes + 1]))) i = LetterToIndex(argv[nNotes + 1]) + 1;    // +1 to counteract the -1 below
				else i = atoi(argv[nNotes + 1]);
				if (i <= 0 || i > NFREQUENCIES) {
					fprintf(stderr, "Invalid usage. Argument should be 1 - %d.\n", NFREQUENCIES);
					return(0);
				}
				iNotes[nNotes] = i - 1;
			}
			iNotes[nNotes] = -1;
		}
	}

	InitializePlayback();

	if (iTestPlayRecord) {
		// Record the special buffer
		InitializeRecording();
		RecordBuffer(iBigBuf, lBigBufSize);
		CloseRecording();
		// play the special buffer
		printf("Playing special %d..\n", 60);
		PlayBuffer(iBigBuf, lBigBufSize);
	}
	else {
		InitializeNotes();
		// test individual notes
		for (i = 0; iNotes[i] >= 0; ++i) {
			PlayNote(iNotes[i], 500);
			printf("Playing %d..\n", iNotes[i] + 1);
			WaitForNotePlayback(iNotes[i]);
			// Sleep(500L);
		}
		FreeNoteBuffers();
	}

	ClosePlayback();
#endif

#ifdef RECORDING

	long len = 100 * 1024 * sizeof(short);
	short *audioBuff = (short *)malloc(len);

	InitializeRecording();
	RecordBuffer(audioBuff, len);
	CloseRecording();

	InitializePlayback();
	PlayBuffer(audioBuff, len);
	ClosePlayback();

#endif

	return(0);
}
#endif

#endif


