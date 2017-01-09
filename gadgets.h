#ifndef GADGETS_H
#define GADGETS_H

//#define DEF_GADGET(LeftEdge, TopEdge, Width, Height, GadgetText, TextAttr, GadgetID, Flags, VisualInfo, UserData)

struct TextAttr topaz8 = { (STRPTR)"topaz.font", 8, 0, 1 };

enum GADGET_INDEX { GAD_QUERYINPUT,
    GAD_QUERYRESULTSLISTVIEW,
    GAD_QUERYSEARCHPATH,
    GAD_QUERYSEARCHBROWSE,
    GAD_QUERYDOSEARCH,
    GAD_BTN_PREVIEWSOUND,
    GAD_BTN_PREVIEWC1,
    GAD_BTN_PREVIEWC2,
    GAD_BTN_PREVIEWC3,
    GAD_LBL_FILESIZE,
    GAD_LBL_FILEFORMAT };

/* /// gadgets */
struct NewGadget queryInputGadget = {
    110, 15,    //LeftEdge, TopEdge
    200, 14,    //Width, Height
    (UBYTE*)"Sample Name", //GadgetText
    &topaz8,    //TextAttr
    GAD_QUERYINPUT, //GadgetID (user field)
    PLACETEXT_LEFT, //Flags
    NULL,       //VisualInfo ptr
    NULL        //APTR UserData (user field)
};

struct NewGadget queryDoSearchGadget = {
    320, 15,
    100, 14,
    (UBYTE*)"Search",
    &topaz8,
    GAD_QUERYDOSEARCH,
    PLACETEXT_IN,
    NULL,
    NULL
};

struct NewGadget queryResultsListViewGadget = {
    10, 80,
    300, 200,
    (UBYTE*)"Samples Found",
    &topaz8,
    GAD_QUERYRESULTSLISTVIEW,
    PLACETEXT_ABOVE,
    NULL,
    NULL
};

struct NewGadget querySearchPathGadget = {
    110, 35,
    200, 14,
    (UBYTE*)"Search Path",
    &topaz8,
    GAD_QUERYSEARCHPATH,
    PLACETEXT_LEFT,
    NULL,
    NULL
};

struct NewGadget querySearchBrowseGadget = {
    320, 35,
    100, 14,
    (UBYTE*)"Browse...",
    &topaz8,
    GAD_QUERYSEARCHBROWSE,
    PLACETEXT_IN,
    NULL,
    NULL
};

struct NewGadget btnPreviewSoundGadget = {
    320, 180,
    100, 14,
    (UBYTE*)"Preview",
    &topaz8,
    GAD_BTN_PREVIEWSOUND,
    PLACETEXT_IN,
    NULL,
    NULL
};

struct NewGadget gad_btn_PreviewC1 = {
    320, 80,
    30, 14,
    (UBYTE*)"C-1",
    &topaz8,
    GAD_BTN_PREVIEWC1,
    PLACETEXT_IN,
    NULL,
    NULL
};

struct NewGadget gad_btn_PreviewC2 = {
    353, 80,
    31, 14,
    (UBYTE*)"C-2",
    &topaz8,
    GAD_BTN_PREVIEWC2,
    PLACETEXT_IN,
    NULL,
    NULL
};

struct NewGadget gad_btn_PreviewC3 = {
    387, 80,
    30, 14,
    (UBYTE*)"C-3",
    &topaz8,
    GAD_BTN_PREVIEWC3,
    PLACETEXT_IN,
    NULL,
    NULL
};

struct NewGadget gad_lbl_FileSize = {
    390, 110,
    100, 14,
    (UBYTE*)"Length:",
    &topaz8,
    GAD_LBL_FILESIZE,
    PLACETEXT_LEFT,
    NULL,
    NULL
};

struct NewGadget gad_lbl_FileFormat = {
    390, 130,
    100, 14,
    (UBYTE*)"Format:",
    &topaz8,
    GAD_LBL_FILEFORMAT,
    PLACETEXT_LEFT,
    NULL,
    NULL
};
/* /// */

#endif
