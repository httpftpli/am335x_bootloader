
#include "string.h"
#include <stdio.h>
#include "hw_control_AM335x.h"
#include "hw_types.h"
#include "bl.h"
#include "bl_copy.h"
#include "pf_platform.h"
#include "pf_bootloader.h"
#include "time.h"
#include "misc.h"
#include "bl_platform.h"
#include <time.h>
#include "pf_lcd.h"


#define MAX_BUTTON_CAPTION   30
#define MAX_LABEL_CAPTION    100

#define BTR_VERSION  "V1.2"
const char Ver[] = { "SoftVer-A.MWYT.SW-A.01.20.00&"__DATE__ };

#if  defined(LCD_RES_800_600)
#define PAGELABEL_H  560
#define STATLABEL_Y  580
#define STATLABELTIME_Y 580
#define FRAME_H    482
#define BUTTON_Y   150
#define BUTTON_I   60
#define BUTTON2_Y  540
#define INAND_UDISK_BUTTON_COUNT  11

#elif defined(LCD_RES_800_480)
#define PAGELABEL_H  440
#define STATLABEL_Y  460
#define STATLABELTIME_Y 460
#define FRAME_H    362
#define BUTTON_Y   120
#define BUTTON_I 50
#define BUTTON2_Y  420
#define INAND_UDISK_BUTTON_COUNT  9
#else
#error "must define lcd res"
#endif


extern GUI_FONT GUI_Fontascii_16,GUI_Fontascii_20;

extern BOOL idiskFdisk();
extern BOOL idiskFormat();



MYFILEINFO udiskfileinfolist[200], idiskfileinfolist[20];
unsigned int udiskFileCount, idiskFileCount;
unsigned int ufileindex = -1, ufilegroupindex = 0;
unsigned int ifileindex = -1, ifilegroupindex = 0;
unsigned int ufilestatchagned = 0, ifilestatchagned = 1;



typedef void BUTTON_CLICK_HANDLER(void *button, unsigned int stat);

typedef struct {
    unsigned short x,y,width,height;
    unsigned char pushed:1;
    unsigned char statChanged:1;
    unsigned char checkable:1;
    unsigned char haveFrame:1;
    unsigned char enable:1;
    unsigned char show:1;
    unsigned char colorIndex:2;
    unsigned char shortkey:8;
    unsigned short tabId;
    unsigned int   group;
    void *parent;
    TEXTCHAR *caption;
    BUTTON_CLICK_HANDLER *handler;
}BUTTON;


typedef struct {
    unsigned short x,y,width,height;
    bool val;
    unsigned char statChanged:1;
    unsigned char enable:1;
    unsigned char show:1;
    unsigned char colorIndex:2;
    unsigned char shortkey:8;
    unsigned short tabId;
    unsigned int   group;
    void *parent;
    TEXTCHAR *oncaption[10];
    TEXTCHAR *offcaption[10];
    BUTTON_CLICK_HANDLER *handler;
}SWITCHBUTTON;


typedef struct __label {
    unsigned short x,y,width,height;
    unsigned int haveFrame:1;
    unsigned int statChanged:1;
    unsigned int focus:1;
    unsigned int colorIndex :2;
    TEXTCHAR *caption;
}LABEL;




BUTTON *buttonList[50];
unsigned int buttonindext = 0;
LABEL *labelList[20];
unsigned int labelindex = 0;

BUTTON *buttonGroup1[50];
BUTTON *buttonGroup2[50];


void registButton(BUTTON *button, void *parent) {
    buttonList[buttonindext++] = button;
}

void buttonRegistHandler(BUTTON *button, BUTTON_CLICK_HANDLER handler) {
    button->handler = handler;
}


void registLabel(LABEL *label) {
    labelList[labelindex++] = label;
}


LABEL titleLabel = {
    .x = 0, .y = 0, .width = 800, .height = 20, .haveFrame = 1, .caption = "mingde bootloader "  BTR_VERSION " (build "__DATE__ ")",
};

LABEL pageLabel = {
    .x = 0, .y = 20, .width = 800, .height = PAGELABEL_H, .haveFrame = 1,
};

LABEL statLabel = {
    .x = 0, .y = STATLABEL_Y, .width = 60, .height = 20, .haveFrame = 1, .haveFrame = 1, .caption = "status"
};

LABEL statLabel1 = {
    .x = 60, .y = STATLABEL_Y, .width = 580, .height = 20, .haveFrame = 1,
};
LABEL statLabelTime = {
    .x = 640, .y = STATLABEL_Y, .width = 160, .height = 20, .haveFrame = 1,
};


LABEL frametitles[] = {
    [0] = { .x = 230, .y = 35, .width = 220, .height = 20, .haveFrame = 1, .caption = NULL },
    [1] = { .x = 450, .y = 35, .width = 220, .height = 20, .haveFrame = 1, .caption = "udisk" },
};

LABEL tsTestLabel = {
    .x = 20, .y = 35, .width = 190, .height = 20, .haveFrame = 1, .caption = "touchpad test"
};

LABEL tsTestLabel1 = {
    .x = 20, .y = 55, .width = 190, .height = FRAME_H, .haveFrame = 1, .caption = NULL
};

LABEL frames[] = {
    [0] = { .x = 230, .y = 55, .width = 220, .height = FRAME_H, .haveFrame = 1, },
    [1] = { .x = 450, .y = 55, .width = 220, .height = FRAME_H, .haveFrame = 1, .focus = 1 },
};

#ifdef INNERBOOT
#define BUTTONSHOW  1
#else
#define BUTTONSHOW  0
#endif


BUTTON burnpagebuttons[] = {
    [0] = { .x = 680, .y = BUTTON_Y-80, .width = 110, .height = 40, .show = 1, .enable = 1, .tabId = 0, .caption = "ts cal", .shortkey = KEY_F1 },
    [1] = { .x = 680, .y = BUTTON_Y, .width = 110, .height = 40, .show = 1, .enable = 1, .tabId = 1, .caption = "iformat", .shortkey = KEY_F2 },
[2] = { .x = 680, .y = BUTTON_Y+BUTTON_I*1, .width = 110, .height = 40, .show = 1, .enable = 0, .tabId = 2, .caption = "burn app", .shortkey = KEY_F3 },
[3] = { .x = 680, .y = BUTTON_Y+BUTTON_I*2, .width = 110, .height = 40, .show = 0, .enable = 0, .tabId = 3, .caption = "burn font", .shortkey = KEY_F4 },
[4] = { .x = 680, .y = BUTTON_Y+BUTTON_I*5, .width = 110, .height = 40, .show = 1, .enable = 1, .tabId = 6, .caption = "run app", .shortkey = KEY_OK },
[5] = { .x = 680, .y = BUTTON_Y+BUTTON_I*3, .width = 110, .height = 40, .show = BUTTONSHOW, .enable = 0, .tabId = 4, .caption = "burn boot", .shortkey = KEY_F5 },
[6] = { .x = 680, .y = BUTTON_Y+BUTTON_I*4, .width = 110, .height = 40, .show = BUTTONSHOW, .enable = 0, .tabId = 5, .caption = "burn auto", .colorIndex = 1, .shortkey = KEY_F6 },
};

BUTTON pagebuttons[2][2] = {
    { { .x = 240, .y = BUTTON2_Y, .width = 80, .height = 35, .enable = 1, .tabId = -1, .caption = "up", .shortkey = KEY_PU },
        { .x = 350, .y = BUTTON2_Y, .width = 80, .height = 35, .enable = 1, .tabId = -1, .caption = "down", .shortkey = KEY_PD }, },
    { { .x = 470, .y = BUTTON2_Y, .width = 80, .height = 35, .enable = 1, .tabId = -1, .caption = "up", .shortkey = KEY_PU },
        { .x = 580, .y = BUTTON2_Y, .width = 80, .height = 35, .enable = 1, .tabId = -1, .caption = "down", .shortkey = KEY_PD }, },
};


BUTTON tscTestButton = {
    .x = 70, .y = BUTTON2_Y, .width = 80, .height = 35, .show = 1, .enable = 1, .haveFrame = 1, .caption = "clear", .shortkey = KEY_NO, .statChanged = 1
};


BUTTON inandButtons[INAND_UDISK_BUTTON_COUNT];
BUTTON udiskButtons[INAND_UDISK_BUTTON_COUNT];


static const char* key2keycaption(unsigned int key) {
    const char *cap;
    switch (key) {
    case KEY_0 :
        cap =  "0";
        break;
    case KEY_1 :
        cap =   "1";
        break;
    case KEY_2 :
        cap =   "2";
        break;
    case KEY_3 :
        cap =   "3";
        break;
    case KEY_4  :
        cap =   "4";
        break;
    case KEY_5  :
        cap =   "5";
        break;
    case KEY_6  :
        cap =   "6";
        break;
    case KEY_7 :
        cap =   "7";
        break;
    case KEY_8 :
        cap =   "8";
        break;
    case KEY_9 :
        cap =   "9";
        break;
    case KEY_POINT:
        cap =   ".";
        break;
    case KEY_ZF :
        cap =   "+/-";
        break;
    case KEY_A  :
        cap =   "A";
        break;
    case KEY_B :
        cap =   "B";
        break;
    case KEY_C  :
        cap =   "C";
        break;
    case KEY_D :
        cap =   "D";
        break;
    case KEY_E :
        cap =   "E";
        break;
    case KEY_F :
        cap =  "F";
        break;
    case KEY_F1:
        cap =   "F1";
        break;
    case KEY_F2 :
        cap =   "F2";
        break;
    case KEY_F3 :
        cap =   "F3";
        break;
    case KEY_F4 :
        cap =  "F4";
        break;
    case KEY_F5 :
        cap = "F5";
        break;
    case KEY_F6:
        cap = "F6";
        break;
    case KEY_RIGHT:
        cap =   "<-";
        break;
    case KEY_LEFT :
        cap =   "->";
        break;
    case KEY_UP :
        cap = "UP";
        break;
    case KEY_DOWN:
        cap =   "DWN";
        break;
    case KEY_QUK :
        cap =   ">>";
        break;
    case KEY_ESC :
        cap =   "ESC";
        break;
    case KEY_OK:
        cap =   "EN";
        break;
    case KEY_PU :
        cap =   "PU";
        break;
    case KEY_PD :
        cap =   "PD";
        break;
    case KEY_USB:
        cap =   "USB";
        break;
    case KEY_CE :
        cap =   "CE";
        break;
    case KEY_MEM :
        cap =   "MEM";
        break;
    case KEY_POP :
        cap =   "POP";
        break;
    default:
        cap =   NULL;
        break;
    }
    return cap;
}


void buttonRedraw(BUTTON *button, unsigned int force) {
    unsigned int forcetemp = force || button->statChanged;
    if (!forcetemp) return;
    button->statChanged = 0;
    if (!button->show) {
        drawRectFillEx(button->x, button->y, button->width, button->height, C_BLUE);
        return;
    }
    COLOR color = (button->colorIndex == 0) ? C_BLUE : C_RED;
    COLOR colorfill, colorframe;
    if (!button->enable) {
        colorfill =  C_GRAY;
        colorframe = C_WHITE;
    } else {
        if (!button->pushed) {
            colorfill =  color;
            colorframe = C_WHITE;
        } else {
            colorfill =  C_WHITE;
            colorframe = color;
        }
    }
    if (button->haveFrame == 1) {
        drawHLineEx(button->x, button->y, button->width, colorframe);
        drawHLineEx(button->x, button->y + button->height - 1, button->width, colorframe);
        drawVLineEx(button->x, button->y, button->height, colorframe);
        drawVLineEx(button->x + button->width - 1, button->y, button->height, colorframe);
    }
    drawRectFillEx(button->x + 1, button->y + 1, button->width - 2, button->height - 2, colorfill);
    if (button->caption != NULL) {
        char shortcut[20];
        const char *shortkeycap = key2keycaption(button->shortkey);
        unsigned int shortcutwidth = 0;
        if (NULL != shortkeycap) {
            sprintf(shortcut, "%s%s%s", "[", key2keycaption(button->shortkey), "]");
            drawStringAlignEx(shortcut, ALIGN_RIGHT_MIDDLE, button->x, button->y, button->width, button->height, &GUI_Fontascii_16, colorframe, C_TRANSPARENT);
            shortcutwidth = getStringMetricWidth(shortcut);
        }
        drawStringAlignEx(button->caption, ALIGN_MIDDLE_MIDDLE, button->x, button->y, button->width - shortcutwidth, button->height, &GUI_Fontascii_16, colorframe, C_TRANSPARENT);
    }
}





void buttonShow(BUTTON *button) {
    button->show = 1;
    button->statChanged = 1;
}

void buttonHidde(BUTTON *button) {
    button->show = 0;
    button->statChanged = 1;
}

void buttonEnable(BUTTON *button) {
    button->enable = 1;
    button->statChanged = 1;
}


void buttonDisable(BUTTON *button) {
    button->enable = 0;
    button->statChanged = 1;
}



void buttonSetCaption(BUTTON *button, TEXTCHAR *caption) {
    button->caption = caption;
    button->statChanged = 1;
}

void buttonSetStat(BUTTON *button, int stat) {
    button->pushed = stat;
    button->statChanged = 1;
}


void labelSetCaption(LABEL *label, TEXTCHAR *caption) {
    label->caption = caption;
    label->statChanged = 1;
}

void labelRedraw(LABEL *label, unsigned int force);
void statBarPrint(unsigned int error, TEXTCHAR *buf) {
    static TEXTCHAR caption[120];
    if (NULL == buf) {
        statLabel1.caption = NULL;
    } else {
        strcpy(caption, buf);
        statLabel1.caption = caption;
    }
    statLabel1.colorIndex = !!error;
    labelRedraw(&statLabel1, 1);
}


void labelCapRedraw(LABEL *label);
void statLabelShowTime() {
    static char buf[30];
    everydiffdo(time_t, t, time(NULL)) {
        struct tm *now = gmtime(&t);
        sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d", now->tm_year + 1900, now->tm_mon + 1,
                now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
        statLabelTime.caption = buf;
        labelCapRedraw(&statLabelTime);
    }
}


void labelRedraw(LABEL *label, unsigned int force) {
    unsigned int forcetemp = force || label->statChanged;
    COLOR color = (label->colorIndex == 0) ? C_BLUE : C_RED;
    if (forcetemp) {
        drawRectFillEx(label->x, label->y, label->width, label->height, color);
        if (label->haveFrame) {
            drawHLineEx(label->x, label->y, label->width, C_WHITE);
            drawHLineEx(label->x, label->y + label->height - 1, label->width, C_WHITE);
            drawVLineEx(label->x, label->y, label->height, C_WHITE);
            drawVLineEx(label->x + label->width - 1, label->y, label->height, C_WHITE);
            if (label->focus) {
                drawHLineEx(label->x + 1, label->y + 1, label->width - 2, C_WHITE);
                drawHLineEx(label->x - 1, label->y + label->height - 2, label->width - 2, C_WHITE);
                drawVLineEx(label->x + 1, label->y + 1, label->height - 2, C_WHITE);
                drawVLineEx(label->x + label->width - 2, label->y - 1, label->height - 2, C_WHITE);
            }
        }
        if (label->caption != NULL) {
            drawStringAlignEx(label->caption, ALIGN_MIDDLE_MIDDLE, label->x, label->y, label->width, label->height, &GUI_Fontascii_16, C_WHITE, C_TRANSPARENT);
        }
        label->statChanged = 0;
    }
}

void labelCapRedraw(LABEL *label) {
    COLOR color = (label->colorIndex == 0) ? C_BLUE : C_RED;
    if (label->caption != NULL) {
        drawStringAlignEx(label->caption, ALIGN_MIDDLE_MIDDLE, label->x, label->y, label->width, label->height, &GUI_Fontascii_16, C_WHITE, color);
    }
}


void labelSetFocus(LABEL *label, BOOL focus) {
    if ((bool)(label->focus) == focus) {
        return;
    }
    label->focus = focus;
    COLOR color;
    if (focus) {
        color = C_WHITE;
    } else {
        color = (label->colorIndex == 0) ? C_BLUE : C_RED;
    }
    drawHLineEx(label->x + 1, label->y + 1, label->width - 2, color);
    drawHLineEx(label->x - 1, label->y + label->height - 2, label->width - 2, color);
    drawVLineEx(label->x + 1, label->y + 1, label->height - 2, color);
    drawVLineEx(label->x + label->width - 2, label->y - 1, label->height - 2, color);
}


void guiRedraw(void) {
    for (int i = 0; (i < NARRAY(labelList)) && (labelList[i] != NULL); i++) {
        labelRedraw(labelList[i], 0);
        labelList[i]->statChanged = 0;
    }
    for (int i = 0; (i < NARRAY(buttonList)) && (buttonList[i] != NULL); i++) {
        buttonRedraw(buttonList[i], 0);
        buttonList[i]->statChanged = 0;
    }
}

static unsigned int touch2buttonindex() {
    for (int i = 0; (i < NARRAY(buttonList)) && (buttonList[i] != NULL); i++) {
        if ((g_ts.x > buttonList[i]->x) && (g_ts.x < buttonList[i]->x + buttonList[i]->width) &&
            (g_ts.y > buttonList[i]->y) && (g_ts.y < buttonList[i]->y + buttonList[i]->height)
            && (buttonList[i]->enable == 1)) {
            return i;
        }
    }
    return -1;
}

void (*touchedhandler)(POINT_16 *point) = NULL;

void regestTouchedHandler(void (*handler)(POINT_16 *point)) {
    touchedhandler = handler;
}

void touchedHandler(POINT_16 *point) {
    if ((point->x > tsTestLabel1.x) && (point->x < (tsTestLabel1.x + tsTestLabel1.width)) &&
        (point->y > tsTestLabel1.y) && (point->y < (tsTestLabel1.y + tsTestLabel1.height))) {
        drawPix(point->x, point->y, C_WHITE);
    }
}


void guiExec(void) {
#define STAT_NO         0
#define STAT_PUSHDOWN   1
#define STAT_FROZE      2
    static int stat = STAT_NO;
    static int timemark;
    static unsigned int button;
    unsigned int buttonnow;
    switch (stat) {
    case STAT_NO:
        if (atomicTestClear(&g_touched)) {
            if (NULL != touchedhandler) {
                POINT_16 point;
                point.x = g_ts.x;
                point.y = g_ts.y;
                touchedHandler(&point);
            }
            buttonnow = touch2buttonindex();
            if ((buttonnow != -1) && (buttonList[buttonnow]->enable == 1)
                && buttonList[buttonnow]->show == 1) {
                stat = STAT_PUSHDOWN;
                timemark = TimerTickGet();
                if (0 == buttonList[buttonnow]->checkable) {
                    buttonList[buttonnow]->pushed = 1;
                    buttonList[buttonnow]->statChanged = 1;
                    button = buttonnow;
                } else {
                    buttonList[buttonnow]->pushed = !buttonList[buttonnow]->pushed;
                    buttonList[buttonnow]->statChanged = 1;
                    button = buttonnow;
                }
                guiRedraw();
                if (buttonList[buttonnow]->handler != NULL) buttonList[buttonnow]->handler(buttonList[buttonnow], buttonList[buttonnow]->pushed);
                //return buttonList[buttonnow]->tabId;
            }
        }
        break;
    case STAT_PUSHDOWN:
        if (!atomicTestClear(&g_touched)) {
            if (TimerTickGet() - timemark > 150) {
                if (0 == buttonList[button]->checkable) {
                    buttonList[button]->pushed = 0;
                    buttonList[button]->statChanged = 1;
                }
                stat = STAT_NO;
            }
        } else {
            timemark = TimerTickGet();
        }
        break;
    default:
        break;
    }
    guiRedraw();
    //return 0;
}


void labelShow(LABEL *label) {
    labelRedraw(label, 1);
};


TEXTCHAR buff[100];



extern BOOL isIDok;
void hmishow() {
    labelShow(&titleLabel);
    labelShow(&pageLabel);
    statLabel.colorIndex  = (isIDok == 1) ? 0 : 1;
    labelShow(&statLabel);
    labelShow(&statLabel1);
    labelShow(&statLabelTime);
    labelShow(&tsTestLabel);
    labelShow(&tsTestLabel1);
    for (int i = 0; i < NARRAY(frametitles); i++) {
        labelShow(frametitles + i);
    }
    for (int i = 0; i < NARRAY(frames); i++) {
        labelShow(frames + i);
    }
    for (int i = 0; i < NARRAY(burnpagebuttons); i++) {
        //buttonShow(&burnpagebuttons[i]);
        burnpagebuttons[i].statChanged = 1;
    }

    for (int i = 0; i < NARRAY(pagebuttons); i++) {
        for (int j = 0; j < NARRAY(pagebuttons[0]); j++) {
            buttonShow(&pagebuttons[i][j]);
        }
    }
    buttonShow(&tscTestButton);

    ufilestatchagned = 1;
    ifilestatchagned = 1;
}



void tscalhandler(void *button, unsigned int stat) {
    if (1 == stat) {
        TouchCalibrate(1);
    }
    hmishow();
}


static void idiskbuttonhandler(void *button, unsigned int stat) {
    BUTTON *b = (BUTTON *)button;
    BUTTON **grop;
    unsigned int index;
    char strbuf[100];
    if (stat == 1) {
        grop = (BUTTON **)(b->group);
        for (int i = 0;; i++) {
            if (grop[i] == NULL) {
                break;
            }
            if (grop[i] != b) {
                if (grop[i]->pushed == 1) {
                    grop[i]->pushed = 0;
                    grop[i]->statChanged = 1;
                }
            } else {
                index = i;
            }
        }
        ifileindex = index;
    } else {
        ifileindex = -1;
    }
    if (ifileindex != -1) {
        time_t time = idiskfileinfolist[ifilegroupindex * INAND_UDISK_BUTTON_COUNT + ifileindex].modtime;
        struct tm *tm_time = gmtime(&time);
        sprintf(strbuf, "%s     %04d-%02d-%02d %02d:%02d:%02d",
                idiskfileinfolist[ifilegroupindex * INAND_UDISK_BUTTON_COUNT + ifileindex].filename,
                tm_time->tm_year + 1900, tm_time->tm_mon + 1,
                tm_time->tm_mday, tm_time->tm_hour, tm_time->tm_min, tm_time->tm_sec);
        statBarPrint(0, strbuf);
    } else {
        statBarPrint(0, NULL);
    }
}



extern FRESULT scan_files(TCHAR *path, MYFILEINFO *fileinfolist, unsigned int *nfiles);
void probUdisk() {
    static int probed = 0;
    if ((g_usbMscState == USBMSC_DEVICE_READY) && (probed == 0)) {
        udiskFileCount = NARRAY(udiskfileinfolist);
        scan_files("2:/", udiskfileinfolist, &udiskFileCount);
        probed = 1;
        buttonEnable(&pagebuttons[1][0]);
        buttonEnable(&pagebuttons[1][1]);
        buttonEnable(&burnpagebuttons[2]);
        buttonEnable(&burnpagebuttons[3]);
        buttonEnable(&burnpagebuttons[5]);
        buttonEnable(&burnpagebuttons[6]);
        ufilegroupindex = 0;
        ufileindex  = -1;
        ufilestatchagned = 1;
    } else if ((g_usbMscState == USBMSC_NO_DEVICE) && (probed == 1)) {
        probed = 0;
        buttonDisable(&pagebuttons[1][0]);
        buttonDisable(&pagebuttons[1][1]);
        buttonDisable(&burnpagebuttons[2]);
        buttonDisable(&burnpagebuttons[3]);
        buttonDisable(&burnpagebuttons[5]);
        buttonDisable(&burnpagebuttons[6]);
        ufilegroupindex = 0;
        if (ufileindex != -1) {
            buttonSetStat(&udiskButtons[ufileindex], 0);
        }
        ufileindex = -1;
        memset(udiskfileinfolist, 0, sizeof udiskfileinfolist);
        udiskFileCount = 0;
        ufilestatchagned = 1;
    }
}

void displayUdisk() {
    if (ufilestatchagned == 1) {
        ufilestatchagned = 0;
        for (unsigned int i = 0; i < INAND_UDISK_BUTTON_COUNT; i++) {
            if ((ufilegroupindex * INAND_UDISK_BUTTON_COUNT + i) >= udiskFileCount) {
                buttonSetCaption(&udiskButtons[i], NULL);
                buttonHidde(&udiskButtons[i]);
            } else {
                buttonSetCaption(&udiskButtons[i], udiskfileinfolist[ufilegroupindex * INAND_UDISK_BUTTON_COUNT + i].filename);
                buttonShow(&udiskButtons[i]);
            }
        }
    }
}


extern unsigned int long long  getpartitionfree(const TCHAR *driverpath);
void probIdisk_display() {
    static TEXTCHAR labelchar[48];
    if (ifilestatchagned == 1) {
        idiskFileCount = NARRAY(idiskfileinfolist);
        scan_files("0:/", idiskfileinfolist, &idiskFileCount);
        ifilestatchagned = 0;
        for (unsigned int i = 0; i < INAND_UDISK_BUTTON_COUNT; i++) {
            if (((ifilegroupindex * INAND_UDISK_BUTTON_COUNT + i) >= idiskFileCount)) {
                buttonSetCaption(&inandButtons[i], NULL);
                buttonHidde(&inandButtons[i]);
            } else {
                buttonSetCaption(&inandButtons[i], idiskfileinfolist[ifilegroupindex * INAND_UDISK_BUTTON_COUNT + i].filename);
                buttonShow(&inandButtons[i]);
            }
        }
        unsigned int free =  getpartitionfree("0:/") / 1024;
        sprintf(labelchar, "idisk(%dKB free)", free);
        labelSetCaption(&frametitles[0], labelchar);
    }
}


static void idiskformathandler(void *button, unsigned int stat) {
    BOOL r;
#ifndef INNERBOOT
    if (!isIDok) return;
#endif
    if (1 == stat) {
        r = idiskFormat();
        if (r) {
            eraseRunApp();
            statBarPrint(0, "idisk format success");
        } else {
            statBarPrint(1, "idisk format fail");
        }
    }
    ifilestatchagned = 1;
}

static void udiskbuttonhandler(void *button, unsigned int stat) {
    BUTTON *b = (BUTTON *)button;
    BUTTON **grop;
    unsigned int index;
    if (1 == stat) {
        grop = (BUTTON **)(b->group);
        for (int i = 0;; i++) {
            if (grop[i] == NULL) {
                break;
            }
            if (grop[i] != b) {
                if (grop[i]->pushed == 1) {
                    grop[i]->pushed = 0;
                    grop[i]->statChanged = 1;
                }
            } else {
                index = i;
            }
        }
        ufileindex = index;
    } else {
        ufileindex = -1;
    }
    char strbuf[120];
    if (ufileindex != -1) {
        time_t time = udiskfileinfolist[ufilegroupindex * INAND_UDISK_BUTTON_COUNT + ufileindex].modtime;
        struct tm *tm_time = gmtime(&time);
        sprintf(strbuf, "%s     %04d-%02d-%02d %02d:%02d:%02d",
                udiskfileinfolist[ufilegroupindex * INAND_UDISK_BUTTON_COUNT + ufileindex].filename,
                tm_time->tm_year + 1900, tm_time->tm_mon + 1,
                tm_time->tm_mday, tm_time->tm_hour, tm_time->tm_min, tm_time->tm_sec);
        statBarPrint(0, strbuf);
    } else {
        statBarPrint(0, NULL);
    }
}


extern mmcsdCtrlInfo mmcsdctr[2];
extern BURN_RET burnBootloader(const TCHAR *path);

static BOOL burnboot_ui(char *path) {
    int ret;
    BURN_RET retburn;
    unsigned int buf[512 / 4];
    buf[0] = ~HWREG(SOC_CONTROL_REGS + CONTROL_MAC_ID_LO(0));
    buf[1] = ~HWREG(SOC_CONTROL_REGS + CONTROL_MAC_ID_HI(0));
    buf[2] = ~HWREG(SOC_CONTROL_REGS + CONTROL_MAC_ID_LO(1));
    buf[3] = ~HWREG(SOC_CONTROL_REGS + CONTROL_MAC_ID_HI(1));
    ret = MMCSDP_Write(mmcsdctr, buf, UID_SAVE_SECTOR, 1);
    if (ret == FALSE) {
        goto ERROR;
    }
    retburn = burnBootloader(path);
    if (retburn != BURN_OK) {
        goto ERROR;
    }
    statBarPrint(0, "burn bootloader success");
    return 1;
ERROR:
    statBarPrint(1, "burn bootloader fail");
    return 0;
}


static void  tscClearHandler(void *button, unsigned int stat) {
    labelRedraw(&tsTestLabel1, 1);
}

static void burnboothandler(void *button, unsigned int stat) {
    if (-1 == ufileindex) {
        statBarPrint(1, "please select a file");
        return;
    }
    char pathbuf[30];
    memcpy(pathbuf, "2:/", 4);
    strcat(pathbuf, udiskfileinfolist[ufilegroupindex * INAND_UDISK_BUTTON_COUNT + ufileindex].filename);
    burnboot_ui(pathbuf);
}


extern BURN_RET burnAPP(TCHAR *path);
extern BOOL burnFont(const TCHAR *path);
static  void burnautohandler(void *button, unsigned int stat) {
    statBarPrint(0, "format idisk");
    delay(600);
    idiskformathandler(NULL, 1);
    delay(600);
    int ret;
    //burn bootloader
    if(!f_fileExit("2:/default.mbt")){
       statBarPrint(1, "auto burn fail ,not find boot file default.mbt");
       return;
    }
    ret = burnboot_ui("2:/default.mbt");
    if (FALSE == ret) {
        return;
    }
    delay(600);
//burn app
    if(!f_fileExit("2:/default.out")){
       return;
    }
    statBarPrint(0, "file APP file default.out ,burn APP");
    ret = burnAPP("2:/default.out");
    if (BURN_OK != ret) {
       statBarPrint(1, "burn APP fail");
    } else {
       statBarPrint(0, "burn APP success");
    }
    delay(600);
    statBarPrint(0, "auto burn finish");
}


static void burnapphandler(void *button, unsigned int stat) {
    if (-1 == ufileindex) {
        statBarPrint(1, "please select a file");
        return;
    }
    char pathbuf[20] = "2:/";
    BURN_RET ret = burnAPP(strcat(pathbuf, udiskfileinfolist[ufilegroupindex * INAND_UDISK_BUTTON_COUNT + ufileindex].filename));
    if (BURN_OK == ret) {
        statBarPrint(0, "burn app success");
    } else if (BURN_FILE_ERROR == ret) {
        statBarPrint(1, "app file error,burn app fail");
    } else if (BURN_SRC_ERROR == ret) {
        statBarPrint(1, "read app file error ,burn app fail");
    } else if (BURN_DES_ERROR == ret) {
        statBarPrint(1, "write app error ,burn app fail");
    } else {
        statBarPrint(0, "burn app error");
    }
    return;
}

extern BOOL burnFont(const TCHAR *path);
static void burnfonthandler(void *button, unsigned int stat) {
    if (-1 == ufileindex) {
        statBarPrint(1, "please select a file");
        return;
    }
    unsigned int index = ufilegroupindex * INAND_UDISK_BUTTON_COUNT + ufileindex;
    if (!strendwith(udiskfileinfolist[index].filename, ".FNT")) {
        statBarPrint(1, "unknow font file");
        return;
    }
    char pathbuf[20] = "2:/";
    unsigned int ret = burnFont(strcat(pathbuf, udiskfileinfolist[index].filename));
    if (FALSE == ret) {
        statBarPrint(1, "burn font fail");
        return;
    }
    statBarPrint(0, "burn font success");
    ifilestatchagned = 1;
}

static void runapphandler(void *button, unsigned int stat) {
    if (!isIDok) return;
    int ret =  bootCopy();
    if (APP_UNAVAILABLE == ret) {
        statBarPrint(1, "app unavailable");
    } else if (APP_COPY_ERROR == ret) {
        statBarPrint(1, "app copy error");
    } else if (APP_SIZE_ERROR == ret) {
        statBarPrint(1, "app size error");
    } else if (APP_ENTRY_ERROR == ret) {
        statBarPrint(1, "app entry error");
    }
    if (0 == ret) {
        //LCDBackLightOFF();
        //LCDRasterEnd();
        //LCDReset();
        registKeyHandler(NULL);
        //EDMA3Deinit(SOC_EDMA30CC_0_REGS,0);
        LCDFbClear(C_BLACK);
        jumptoApp();
    }
}


static void ufileuphandler(void *button, unsigned int stat) {
    if (ufilegroupindex != 0) {
        ufilegroupindex--;
        if (ufileindex != -1) {
            buttonSetStat(&udiskButtons[ufileindex], 0);
            ufileindex = -1;
        }
        ufilestatchagned = 1;
    }
}


static void ufiledownhandler(void *button, unsigned int stat) {
    if (DIVUP(udiskFileCount, INAND_UDISK_BUTTON_COUNT) - 1 > ufilegroupindex) {
        ufilegroupindex++;
        if (ufileindex != -1) {
            buttonSetStat(&udiskButtons[ufileindex], 0);
            ufileindex = -1;
        }
        ufilestatchagned = 1;
    }
}

void hmiInit() {
    UNUSED(Ver);
    GUI_SetBkColor(C_BLUE);
    GUI_SetColor(C_WHITE);
    buttonRegistHandler(burnpagebuttons, tscalhandler);
    buttonRegistHandler(burnpagebuttons + 1, idiskformathandler);
    buttonRegistHandler(burnpagebuttons + 2, burnapphandler);
    buttonRegistHandler(burnpagebuttons + 3, burnfonthandler);
    buttonRegistHandler(burnpagebuttons + 4, runapphandler);
    buttonRegistHandler(burnpagebuttons + 5, burnboothandler);
    buttonRegistHandler(burnpagebuttons + 6, burnautohandler);
    buttonRegistHandler(&pagebuttons[1][0], ufileuphandler);
    buttonRegistHandler(&pagebuttons[1][1], ufiledownhandler);
    buttonRegistHandler(&tscTestButton, tscClearHandler);

    registLabel(&statLabel1);
    registLabel(&statLabelTime);
    registLabel(&frametitles[0]);

    registButton(&tscTestButton, NULL);
    for (int i = 0; i < NARRAY(burnpagebuttons); i++) {
        burnpagebuttons[i].haveFrame = 1;
        registButton(burnpagebuttons + i, NULL);
        burnpagebuttons[i].statChanged = 1;
    }
    for (int i = 0; i < NARRAY(pagebuttons); i++) {
        for (int j = 0; j < NARRAY(pagebuttons[i]); j++) {
            pagebuttons[i][j].haveFrame = 1;
            registButton(&pagebuttons[i][j], NULL);
            buttonShow(&pagebuttons[i][j]);
        }
    }
    buttonDisable(&pagebuttons[1][0]);
    buttonDisable(&pagebuttons[1][1]);
    for (int i = 0; i < NARRAY(inandButtons); i++) {
        inandButtons[i].x = frames[0].x + 1;
        inandButtons[i].y = frames[0].y + 1 + 40 * i;
        inandButtons[i].width = frames[0].width - 2;
        inandButtons[i].height = 40;
        inandButtons[i].checkable = 1;
        inandButtons[i].enable = 1;
        inandButtons[i].show = 0;
        inandButtons[i].shortkey = i;
        inandButtons[i].group = (unsigned int)buttonGroup1;
        buttonGroup1[i] = inandButtons + i;
        inandButtons[i].tabId = i;
        registButton(inandButtons + i, NULL);
        buttonRegistHandler(inandButtons + i, idiskbuttonhandler);
    }
    for (int i = 0; i < NARRAY(udiskButtons); i++) {
        udiskButtons[i].x = frames[1].x + 1;
        udiskButtons[i].y = frames[1].y + 1 + 40 * i;
        udiskButtons[i].width = frames[1].width - 2;
        udiskButtons[i].height = 40;
        udiskButtons[i].checkable = 1;
        udiskButtons[i].enable = 1;
        udiskButtons[i].show = 0;
        udiskButtons[i].shortkey = i;
        udiskButtons[i].group = (unsigned int)buttonGroup2;
        buttonGroup2[i] = udiskButtons + i;
        udiskButtons[i].tabId = i;
        buttonRegistHandler(udiskButtons + i, udiskbuttonhandler);
        registButton(udiskButtons + i, NULL);
    }
    hmishow();
    regestTouchedHandler(touchedHandler);
}




void shortcuthandler(int keycode) {
    static int groupactive = 1;
    BUTTON *pbutton;
    unsigned short x, y;
    if (KEY_NO == keycode) return;
    if (KEY_RIGHT ==  keycode) {
        groupactive = 1;
        labelSetFocus(&frames[1], 1);
        labelSetFocus(&frames[0], 0);
    } else if (KEY_LEFT ==  keycode) {
        groupactive = 0;
        labelSetFocus(&frames[0], 1);
        labelSetFocus(&frames[1], 0);
    } else if ((keycode >= KEY_0) && (keycode <= KEY_F)) {
        pbutton = (groupactive == 0) ? inandButtons : udiskButtons;
        for (int i = 0; i < NARRAY(inandButtons); i++) {
            if (pbutton[i].shortkey == keycode) {
                x = pbutton[i].x + pbutton[i].width / 2;
                y = pbutton[i].y + pbutton[i].height / 2;
                simulateTouch(x, y);
                return;
            }
        }
    } else if ((keycode == KEY_PD) || (keycode == KEY_PU)) {
        pbutton = (groupactive == 0) ? pagebuttons[0] : pagebuttons[1];
        for (int i = 0; i < 2; i++) {
            if (pbutton[i].shortkey == keycode) {
                x = pbutton[i].x + pbutton[i].width / 2;
                y = pbutton[i].y + pbutton[i].height / 2;
                simulateTouch(x, y);
                return;
            }
        }
    } else {
        for (int i = 0; i < NARRAY(burnpagebuttons); i++) {
            if (burnpagebuttons[i].shortkey == keycode) {
                pbutton = burnpagebuttons;
                x = pbutton[i].x + pbutton[i].width / 2;
                y = pbutton[i].y + pbutton[i].height / 2;
                simulateTouch(x, y);
                return;
            }
        }
    }
    return;
}
