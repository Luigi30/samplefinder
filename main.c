/* -----------------------------------------------------------------------------

  <insert project description here>

*/

#include "main.h"

/* macros */

//OS4 list functions missing from OS3
#define NewList(list) ((list)->lh_Head = (struct Node *)&(list)->lh_Tail,(list)->lh_TailPred = (struct Node *)&(list)->lh_Head,(list)->lh_Tail = NULL)
#define GetHead(list) ((list) && (list)->lh_Head && (list)->lh_Head->ln_Succ ? (list)->lh_Head : NULL)
#define GetTail(list) ((list) && (list)->lh_TailPred && (list)->lh_TailPred->ln_Pred ? (list)->lh_TailPred : NULL)
#define GetSucc(node) ((node) && (node)->ln_Succ->ln_Succ ? (node)->ln_Succ : NULL)
#define GetPred(node) ((node) && (node)->ln_Pred->ln_Pred ? (node)->ln_Pred : NULL)

#define M_GAD_GETSTRING(gadgetId) ((struct StringInfo *)gadgets[gadgetId]->SpecialInfo)->Buffer

/* globals */

struct Gadget *gadList, *gadTemp, *gadgets[16]; //gadList is internal, gadgets is mine
struct Screen *pubScreen;
struct Window *mainWindow;

struct SearchResult *selectedItem;

struct List queryResultsList;

char searchDirectory[256];

/* gadtools */
APTR vInfo;

/* program */

char *strlwr(char *s){
    char *tmp = s;

    for(;*tmp;++tmp){
        *tmp = tolower((unsigned char)*tmp);
    }

    return s;
}

struct Node *GetListIndex(struct List *list, int index){
    struct Node *node = GetHead(list);

    for(int i=0;i<index;i++){
        node = GetSucc(node);
    }

    return node;
}

void EmptyList(struct List *list){
    while(!IsListEmpty(list)){
        struct SearchResult *result = (struct SearchResult *)GetTail(list);
        RemTail(list);
        FreeMem(result->sr_Node.ln_Name, 128);
        FreeVec(result);
    }
}

void SetupGadgets(struct Window *window){
    vInfo = GetVisualInfo(pubScreen, TAG_DONE);
    if(!(gadTemp = CreateContext(&gadList))) {
        printf("Failed to create GadTools context!\n");
    }

    //querySearchPathGadget
    querySearchPathGadget.ng_VisualInfo = vInfo;
    gadgets[GAD_QUERYSEARCHPATH] = gadTemp = CreateGadget(STRING_KIND, gadTemp, &querySearchPathGadget, GTST_MaxChars, 256, TAG_DONE);
    //end

    //querySearchBrowseGadget
    querySearchBrowseGadget.ng_VisualInfo = vInfo;
    gadgets[GAD_QUERYSEARCHBROWSE] = gadTemp = CreateGadget(BUTTON_KIND, gadTemp, &querySearchBrowseGadget, TAG_DONE);
    //end

    //queryInputGadget
    queryInputGadget.ng_VisualInfo = vInfo;
    gadgets[GAD_QUERYINPUT] = gadTemp = CreateGadget(STRING_KIND, gadTemp, &queryInputGadget, TAG_DONE);
    //end

    //queryDoSearchGadget
    queryDoSearchGadget.ng_VisualInfo = vInfo;
    gadgets[GAD_QUERYDOSEARCH] = gadTemp = CreateGadget(BUTTON_KIND, gadTemp, &queryDoSearchGadget, TAG_DONE);

    //queryResultsListViewGadget
    queryResultsListViewGadget.ng_VisualInfo = vInfo;
    gadgets[GAD_QUERYRESULTSLISTVIEW] = gadTemp = CreateGadget(LISTVIEW_KIND, gadTemp, &queryResultsListViewGadget, GTLV_Labels, &queryResultsList, GTLV_ShowSelected, NULL, TAG_DONE);
    //end

    //btnPreviewSound
    btnPreviewSoundGadget.ng_VisualInfo = vInfo;
    gadgets[GAD_BTN_PREVIEWSOUND] = gadTemp = CreateGadget(BUTTON_KIND, gadTemp, &btnPreviewSoundGadget, TAG_DONE);
    //end

    //gad_btn_PreviewC1
    gad_btn_PreviewC1.ng_VisualInfo = vInfo;
    gadgets[GAD_BTN_PREVIEWC1] = gadTemp = CreateGadget(BUTTON_KIND, gadTemp, &gad_btn_PreviewC1, TAG_DONE);
    //end

    //gad_btn_PreviewC2
    gad_btn_PreviewC2.ng_VisualInfo = vInfo;
    gadgets[GAD_BTN_PREVIEWC2] = gadTemp = CreateGadget(BUTTON_KIND, gadTemp, &gad_btn_PreviewC2, TAG_DONE);
    //end

    //gad_btn_PreviewC3
    gad_btn_PreviewC3.ng_VisualInfo = vInfo;
    gadgets[GAD_BTN_PREVIEWC3] = gadTemp = CreateGadget(BUTTON_KIND, gadTemp, &gad_btn_PreviewC3, TAG_DONE);
    //end

    //gad_lbl_FileSize
    gad_lbl_FileSize.ng_VisualInfo = vInfo;
    gadgets[GAD_LBL_FILESIZE] = gadTemp = CreateGadget(TEXT_KIND, gadTemp, &gad_lbl_FileSize, GTTX_Text, (STRPTR)"12345", TAG_DONE);
    //end

    //gad_lbl_FileFormat
    gad_lbl_FileFormat.ng_VisualInfo = vInfo;
    gadgets[GAD_LBL_FILEFORMAT] = gadTemp = CreateGadget(TEXT_KIND, gadTemp, &gad_lbl_FileFormat, GTTX_Text, (STRPTR)"8SVX", TAG_DONE);
    //end
}

void DoSampleSearch(struct List *resultsList, UBYTE *dirName, UBYTE *searchString){
    printf("Searching for samples containing '%s' in the directory '%s'\n", searchString, dirName);

    BPTR dirLock = Lock((STRPTR)dirName, ACCESS_READ);
    if(BADDR(dirLock) == NULL){
        printf("Couldn't obtain read lock on %s!\n", dirName);
        return;
    }

    struct FileInfoBlock *fib = AllocDosObject(DOS_FIB, TAG_DONE);

    if(Examine(dirLock, fib)){
        printf("first examined object is '%s'. directory flag is %x\n", fib->fib_FileName, fib->fib_DirEntryType);

        while(ExNext(dirLock, fib)){
            //printf("Searching for the string '%s' in the string '%s'\n", strlwr(searchString), strlwr(fib->fib_FileName));
            if(strstr(strlwr(fib->fib_FileName), strlwr(searchString)) != NULL){
                char *fullPath = AllocMem(128, MEMF_PUBLIC|MEMF_CLEAR);
                strcat(fullPath, dirName);
                strcat(fullPath, "/");
                strcat(fullPath, fib->fib_FileName);

                struct SearchResult *result = AllocVec(sizeof(struct SearchResult), MEMF_CLEAR);
                result->sr_Node.ln_Name = fullPath;
                result->sr_Size = fib->fib_Size;

                //We can't open from the lock and re-use it in ExNext. :(
                BPTR file = Open(fullPath, MODE_OLDFILE);
                BYTE header[4];
                Read(file, header, 4);

                if(header[0] == 'F' && header[1] == 'O' && header[2] == 'R' && header[3] == 'M')
                    strcpy(&result->sr_Format, "8SVX");
                    //result->sr_Format = (ULONG)"8SVX";
                else
                    strcpy(&result->sr_Format, "PCM ");
                    //result->sr_Format = (ULONG)"PCM ";
                Close(file);

                //ignore the warning here, AddHead accepts any struct that starts with a Node
                AddHead(resultsList, result);
                printf("Added '%s' to resultsList\n", fullPath);
            }
        }
    }

    if(BADDR(dirLock) != NULL) UnLock(dirLock); //Done, so release our read lock
    if(fib != NULL) FreeDosObject(DOS_FIB, fib);
}

//void processGadgetUp(struct Gadget *gadget){
void processGadgetUp(struct IntuiMessage *msg) {
    struct FileRequester *requester;
    //struct SearchResult *selectedItem = (struct SearchResult *)GetListIndex(&queryResultsList, msg->Code);
    BOOL result;
    char sizeStr[] = "12345";
    char formatStr[5] = "????\0";
    printf("selected sound is %s\n", selectedItem->sr_Node.ln_Name);

    struct Gadget *gadget = (struct Gadget *)msg->IAddress;
    printf("IDCMP_GADGETUP: Gadget ID is %d\n", gadget->GadgetID);

    switch(gadget->GadgetID){
        case GAD_QUERYSEARCHBROWSE:
            //open a directory requester
            requester = AllocAslRequest(ASL_FileRequest, NULL);
            if(AslRequestTags(requester,
                                    ASLFR_Window, mainWindow,
                                    ASLFR_TitleText, (STRPTR)"Select Sample Folder",
                                    ASLFR_DrawersOnly, TRUE,
                                    ASLFR_InitialShowVolumes, TRUE,
                                    ASLFO_InitialWidth, 300,
                                    ASLFO_InitialHeight, 400,
                                    TAG_DONE))
            {
                printf("Selected directory is %s\n", requester->rf_Dir);
                GT_SetGadgetAttrs(gadgets[GAD_QUERYSEARCHPATH], mainWindow, NULL, GTST_String, requester->rf_Dir, TAG_DONE);
            }
            else
            {
                printf("Requester returned false. IOErr is %x\n", IoErr());
            }
            break;
        case GAD_QUERYDOSEARCH:
            //find every file in the folder and add them to the listview
            //detach the list before we update it
            GT_SetGadgetAttrs(gadgets[GAD_QUERYRESULTSLISTVIEW], mainWindow, NULL, GTLV_Labels, ~0, TAG_DONE);

            EmptyList(&queryResultsList);
            DoSampleSearch(&queryResultsList, M_GAD_GETSTRING(GAD_QUERYSEARCHPATH), M_GAD_GETSTRING(GAD_QUERYINPUT));

            //reattach the list
            GT_SetGadgetAttrs(gadgets[GAD_QUERYRESULTSLISTVIEW], mainWindow, NULL, GTLV_Labels, &queryResultsList, TAG_DONE);
            break;
        case GAD_BTN_PREVIEWC1:
            PB_PlaySound(mainWindow, selectedItem->sr_Node.ln_Name, NOTE_C1);
            break;
        case GAD_BTN_PREVIEWC2:
            PB_PlaySound(mainWindow, selectedItem->sr_Node.ln_Name, NOTE_C2);
            break;
        case GAD_BTN_PREVIEWC3:
            PB_PlaySound(mainWindow, selectedItem->sr_Node.ln_Name, NOTE_C3);
            break;
        case GAD_QUERYRESULTSLISTVIEW:
            selectedItem = (struct SearchResult *)GetListIndex(&queryResultsList, msg->Code);
            sprintf(sizeStr, "%d", selectedItem->sr_Size);
            sprintf(formatStr, "%s", selectedItem->sr_Format);
            GT_SetGadgetAttrs(gadgets[GAD_LBL_FILESIZE], mainWindow, NULL, GTTX_Text, sizeStr, TAG_DONE);
            GT_SetGadgetAttrs(gadgets[GAD_LBL_FILEFORMAT], mainWindow, NULL, GTTX_Text, formatStr, TAG_DONE);
            //printf("selected item %d, length is %d and format is %x\n", msg->Code, selectedItem->sr_Size, selectedItem->sr_Format);
            break;
        default:
            break;
    }
}

void MainWindowEventLoop(struct Window *window){
    struct IntuiMessage *msg;
    ULONG msgClass;

    ULONG windowSignal = (1L << window->UserPort->mp_SigBit);
    ULONG waitSignals = windowSignal | SIG_PLAYBACK_FINISHED;

    int CLOSE_WINDOW = FALSE;
    while(CLOSE_WINDOW == FALSE){
        ULONG signal = Wait(waitSignals); //Wait for an event.

        if(signal & windowSignal) {
            //Process the window event.
            msg = GT_GetIMsg(mainWindow->UserPort);
            msgClass = msg->Class;
            GT_ReplyIMsg(msg); //Reply to the message.

            switch(msgClass) {
                case IDCMP_CLOSEWINDOW:
                    CloseWindow(window);
                    CLOSE_WINDOW = TRUE;
                    break;
                case IDCMP_GADGETUP: //A button was clicked.
                    //processGadgetUp((struct Gadget *)msg->IAddress);
                    processGadgetUp(msg);
                    break;
                default:
                    //printf("Unhandled msgClass: %lx\n", msgClass);
                    break;
            }
        }
        if(signal & SIG_PLAYBACK_FINISHED){
            printf("Got SIG_PLAYBACK_FINISHED\n");
            PB_DisposeSound();
        }
    }
}

int main(int argc, char **argv)
{
    NewList(&queryResultsList); //initialize the list for later

    pubScreen = LockPubScreen(NULL);
    SetupGadgets(mainWindow);

    BYTE signalBit = AllocSignal(-1); //grab an available signal flag
    SIG_PLAYBACK_FINISHED = 1UL << signalBit; //convert bit to mask for tag

    mainWindow = OpenWindowTags(NULL,
        WA_Left,        20,
        WA_Top,         20,
        WA_Width,       500,
        WA_Height,      300,
        WA_IDCMP,       IDCMP_CLOSEWINDOW | IDCMP_GADGETUP | IDCMP_GADGETDOWN | IDCMP_MOUSEMOVE | ARROWIDCMP,
        WA_Flags,       WFLG_SIZEGADGET | WFLG_DRAGBAR | WFLG_DEPTHGADGET | WFLG_CLOSEGADGET,
        WA_Gadgets,     gadList,
        WA_Title,       "Sample Finder",
        WA_PubScreenName,   "Workbench",
        TAG_DONE);

    GT_RefreshWindow(mainWindow, NULL);

    MainWindowEventLoop(mainWindow);

    if(pubScreen) UnlockPubScreen(NULL, pubScreen);
    if(vInfo) FreeVisualInfo(vInfo);
    if(gadList) FreeGadgets(gadList);

    return(0);
}                                       
