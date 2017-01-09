#include "playback.h"

ULONG SIG_PLAYBACK_FINISHED; //Signal flag for when an 8SVX sound is done playing.
APTR PB_SoundObject;

struct EasyStruct askPlayPCM = {
    sizeof(struct EasyStruct),
    0,
    "Not an 8SVX Sample",
    "The selected sample is not an 8SVX sound. Try to play as PCM?",
    "Yes|No"
};

void PB_PlaySound(struct Window *window, char *fileName, LONG frequency){
    //fileName is a fully-qualified path

    //Try using datatypes.library
    //APTR dtSoundObject = NULL;
    struct dtTrigger dtSoundTrigger;
    ULONG dtReturn;

    struct Task *thisTask = FindTask(NULL);
    PB_SoundObject = NewDTObject(fileName,
                                DTA_GroupID,     GID_SOUND,
                                SDTA_Period,     frequency,
                                SDTA_SignalTask, thisTask,
                                SDTA_SignalBit,  SIG_PLAYBACK_FINISHED,
                                TAG_END);

    if(PB_SoundObject == NULL){
        printf("%s isn't a datatypes.library sound file\n", fileName);
        //This isn't an 8SVX sound. Play as raw PCM.
        PB_PlayPCM(fileName, frequency);
    } else {
        //Play the sound with datatypes.library
        printf("playing %s with datatypes.library\n", fileName);
        dtSoundTrigger.MethodID     = DTM_TRIGGER;
        dtSoundTrigger.dtt_GInfo    = NULL;
        dtSoundTrigger.dtt_Function = STM_PLAY;
        dtSoundTrigger.dtt_Data     = NULL;

        dtReturn = DoDTMethodA(PB_SoundObject, NULL, NULL, &dtSoundTrigger);

        //Wait(signalMask);
        //DisposeDTObject(dtSoundObject);
    }
}

void PB_DisposeSound(){
    printf("Disposing sound object\n");
    DisposeDTObject(PB_SoundObject);
}

void PB_PlayPCM(char *fileName, LONG frequency){
    //Open the file and load it into chipmem.
    BPTR audioFile = Open(fileName, MODE_OLDFILE);
    BYTE *pcmData = (BYTE*)AllocMem(16384, MEMF_CHIP|MEMF_CLEAR);
    LONG sampleCount = Read(audioFile, pcmData, 16384);

    if(sampleCount < 0){
        printf("Couldn't load sound %s\n", fileName);
        return;
    }

    printf("Loaded %s as %d samples at %p. Playing as raw 8-bit PCM.\n", fileName, sampleCount, pcmData);

    UBYTE channelPriority[] = { 1, 2, 4, 8 }; //request any available channel
    struct IOAudio *AudioIO; //I/O block for I/O commands
    struct MsgPort *AudioMP; //Message port for replies
    struct Message *AudioMSG;//Reply message ptr

    ULONG device;
    LONG clock = 3579545; //NTSC
    LONG duration;
    LONG sampleCycles = 1; //cycle count

    //Set up AudioIO to access the audio device.
    AudioIO = (struct IOAudio *)AllocMem(sizeof(struct IOAudio), MEMF_PUBLIC|MEMF_CLEAR);
    if(AudioIO == NULL){
        printf("Error initializing AudioIO\n");
        return;
    }

    //Create the reply port
    AudioMP = CreatePort(0, 0);
    if(AudioMP == NULL){
        printf("Error creating AudioMP\n");
        return;
    }

    //Set up the I/O block
    AudioIO->ioa_Request.io_Message.mn_ReplyPort    = AudioMP;
    AudioIO->ioa_Request.io_Message.mn_Node.ln_Pri  = 0;
    AudioIO->ioa_Request.io_Command                 = ADCMD_ALLOCATE;
    AudioIO->ioa_Request.io_Flags                   = ADIOF_NOWAIT;
    AudioIO->ioa_AllocKey                           = 0;
    AudioIO->ioa_Data                               = channelPriority;
    AudioIO->ioa_Length                             = sizeof(channelPriority);
    printf("I/O block initialized. Trying to open the audio device.\n");

    device = OpenDevice(AUDIONAME, 0L, (struct IORequest *)AudioIO, 0L);
    if(device != 0){
        printf("Error opening audio device.\n");
        return;
    }
    printf("Device %s opened. Allocated a channel.\n", AUDIONAME);

    //Set up the Audio I/O block to play our sound
    AudioIO->ioa_Request.io_Message.mn_ReplyPort    = AudioMP;
    AudioIO->ioa_Request.io_Command                 = CMD_WRITE;
    AudioIO->ioa_Request.io_Flags                   = ADIOF_PERVOL;
    AudioIO->ioa_Data                               = pcmData;
    AudioIO->ioa_Length                             = sampleCount;
    AudioIO->ioa_Period                             = clock/(clock/frequency);
    AudioIO->ioa_Volume                             = 64;
    AudioIO->ioa_Cycles                             = 1;

    printf("Playing PCM sound %s\n", fileName);
    BeginIO((struct IORequest *)AudioIO);
    WaitPort(AudioMP);
    AudioMSG = GetMsg(AudioMP);

    printf("Sound finished playing\n");

    if(pcmData != 0) FreeMem(pcmData, sampleCount);
    if(device == 0)  CloseDevice((struct IORequest *)AudioIO);
    if(AudioMP != 0) DeletePort(AudioMP);
    if(AudioIO != 0) FreeMem(AudioIO, sizeof(struct IOAudio));
}
