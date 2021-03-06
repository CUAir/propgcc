/* p2load.c - propeller ii two-stage loader */
/*
    Copyright (c) 2012-2013, David Betz
    
    Based on code by Chip Gracey from the
    Propeller II ROM loader
      Copyright (c) 2012, Parallax, Inc.
      
    MIT License
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>
#include <ctype.h>
#include "loadelf.h"
#include "p2_loadimage.h"
#include "osint.h"

#ifndef TRUE
#define TRUE    1
#define FALSE   0
#endif

/* port prefix */
#if defined(CYGWIN) || defined(WIN32) || defined(MINGW)
  #define PORT_PREFIX ""
#elif defined(LINUX)
  #define PORT_PREFIX "ttyUSB"
#elif defined(MACOSX)
  #define PORT_PREFIX "cu.usbserial"
#else
  #define PORT_PREFIX ""
#endif

/* base of hub ram */
#define BASE        0x0e80

/* hub address of main program COG image */
#define COGIMAGE_LO BASE
#define COGIMAGE_HI 0x1000

/* parameters for starting the ROM MONITOR_ADDR */
#define MONITOR_ADDR    0x70c
#define MONITOR_PARAM   ((90 << 9) | 91)                        

/* defaults */
#define BAUD_RATE   230400 // 115200

/* CheckPort state structure */
typedef struct {
    int baud;
    int verbose;
    char *actualport;
} CheckPortInfo;

/* CheckPort result codes */
enum {
    CHECK_PORT_OK,
    CHECK_PORT_OPEN_FAILED,
    CHECK_PORT_NO_PROPELLER
};

/* locals */
static int version;

/* prototypes */
static int LoadFile(char *infile, uint32_t loadAddr, int strip, int writeBootImage, uint32_t flashAddr, uint32_t *pSize, uint32_t *pCogImage);
static void Usage(void);
static void *LoadElfFile(FILE *fp, ElfHdr *hdr, int *pImageSize);

static int ShowPort(const char* port, void* data);
static void ShowPorts(char *prefix);
static int CheckPort(const char* port, void* data);
static int InitPort(char *prefix, char *port, int baud, int verbose, char *actualport);
static int OpenPort(const char* port, int baud);

int main(int argc, char *argv[])
{
    char actualPort[PATH_MAX], *port, *p, *p2, *p3;
    int baudRate, baudRate2, verbose, strip, startMonitor, terminalMode, pstMode, err, i;
    uint32_t flashaddr, hubaddr, count;
    uint32_t loadAddr = BASE;
    uint32_t cogAddr = COGIMAGE_LO;
    uint32_t runAddr = COGIMAGE_LO;
    uint32_t runParam = 0x20000; // top of hub memory for C stack pointer
    uint32_t flashAddr;
    int runParamsSet = FALSE;
    int writeBootImage = FALSE;
    int cogId = 0;
    
    /* initialize */
    baudRate = baudRate2 = BAUD_RATE;
    verbose = strip = startMonitor = terminalMode = pstMode = FALSE;
    port = NULL;
    
    /* process the position-independent arguments */
    for (i = 1; i < argc; ++i) {

        /* handle switches */
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
            case 'b':
                if (argv[i][2])
                    p = &argv[i][2];
                else if (++i < argc)
                    p = argv[i];
                else
                    Usage();
                if (*p != ':')
                    baudRate = baudRate2 = atoi(p);
                if ((p = strchr(p, ':')) != NULL) {
                    if (*++p != ':' && *p != '\0')
                        baudRate2 = atoi(p);
                }
                break;
            case 'c':
            case 'f':
                if (argv[i][2])
                    p = &argv[i][2];
                else if (++i < argc)
                    p = argv[i];
                else
                    Usage();
                break;
            case 'h':
                cogAddr = COGIMAGE_HI;
                break;
            case 'm':
                startMonitor = TRUE;
                break;
            case 'n':
                runParam = 0x8000;
                break;
            case 'p':
                if (argv[i][2])
                    port = &argv[i][2];
                else if (++i < argc)
                    port = argv[i];
                else
                    Usage();
#if defined(CYGWIN) || defined(WIN32) || defined(LINUX)
                if (isdigit((int)port[0])) {
#if defined(CYGWIN) || defined(WIN32)
                    static char buf[10];
                    sprintf(buf, "COM%d", atoi(port));
                    port = buf;
#endif
#if defined(LINUX)
                    static char buf[64];
                    sprintf(buf, "/dev/ttyUSB%d", atoi(port));
                    port = buf;
#endif
                }
#endif
#if defined(MACOSX)
                if (port[0] != '/') {
                    static char buf[64];
                    sprintf(buf, "/dev/cu.usbserial-%s", port);
                    port = buf;
                }
#endif
                break;
            case 'P':
                ShowPorts(PORT_PREFIX);
                break;
            case 'r':
                if (argv[i][2])
                    p = &argv[i][2];
                else if (++i < argc)
                    p = argv[i];
                else
                    Usage();
                if (!(p2 = strchr(p, ',')))
                    cogId = 0; // lets hope the ROM always launches the loader in COG 0!
                else {
                    *p2++ = '\0';
                    cogId = atoi(p);
                    p = p2;
                }
                if (*p != '\0') {
                    if ((p2 = strchr(p, ':')) == NULL)
                        Usage();
                    *p2++ = '\0';
                    runAddr = (uint32_t)strtoul(p, NULL, 16);
                    runParam = (uint32_t)strtoul(p2, NULL,16);
                    runParamsSet = TRUE;
                }
                break;
            case 's':
                /* handle this later with the position-dependent options */
                break;
            case 'T':
                pstMode = TRUE;
                // fall through
            case 't':
                terminalMode = TRUE;
                break;
            case 'v':
                verbose = TRUE;
                break;
            case 'w':
                writeBootImage = TRUE;
                break;
            case '?':
                /* fall through */
            default:
                Usage();
                break;
            }
        }
    }
    
    switch (InitPort(PORT_PREFIX, port, baudRate, verbose, actualPort)) {
    case CHECK_PORT_OK:
        printf("Found propeller version %d on %s\n", version, actualPort);
        break;
    case CHECK_PORT_OPEN_FAILED:
        printf("error: opening serial port '%s'\n", port);
        perror("Error is ");
        return 1;
    case CHECK_PORT_NO_PROPELLER:
        if (port)
            printf("error: no propeller chip on port '%s'\n", port);
        else
            printf("error: can't find a port with a propeller chip\n");
        return 1;
    }
    
    /* load the helper */
    if ((err = p2_InitLoader(baudRate)) != 0)
        return err;

    /* write the booter image to flash if we're creating a bootable image */
    if (writeBootImage) {
        if ((err = p2_FlashBooter(baudRate)) != 0)
            return err;
        flashAddr = 0x800; // just past the booter COG image
    }
            
    /* process the position-dependent arguments */
    for (i = 1; i < argc; ++i) {
        int id, addr, param;

        /* handle switches */
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
            case 'b':   /* skip over the arguments for these options */
            case 'p':
            case 'r':
                if (argv[i][2])
                    p = &argv[i][2];
                else if (++i < argc)
                    p = argv[i];
                break;
            case 'c':
                if (argv[i][2])
                    p = &argv[i][2];
                else if (++i < argc)
                    p = argv[i];
                if (!(p2 = strchr(p, ',')))
                    id = 8;
                else {
                    *p2++ = '\0';
                    id = atoi(p);
                    p = p2;
                }
                if (!(p2 = strchr(p, ':')))
                    param = 0;
                else {
                    *p2++ = '\0';
                    param = (uint32_t)strtoul(p2, NULL,16);
                }
                addr = (uint32_t)strtoul(p, NULL, 16);
                if (id == 0) {
                    runAddr = addr;
                    runParam = param;
                    runParamsSet = TRUE;
                }
                else {
                    if (writeBootImage) {
                        StartCmd startCmd;
                        startCmd.cmd = (id << 8) | CMD_COGINIT;
                        startCmd.addr = addr;
                        startCmd.param = param;
                        if ((err = p2_FlashBuffer((uint8_t *)&startCmd, sizeof(startCmd), flashAddr, FALSE)))
                            return err;
                        flashAddr += sizeof(startCmd);
                    }
                    else {
                        if ((err = p2_StartCog(id, addr, param)) != 0)
                            return err;
                    }
                }
                break;
            case 'f':
                if (argv[i][2])
                    p = &argv[i][2];
                else if (++i < argc)
                    p = argv[i];
                if (!(p2 = strchr(p, ',')))
                    Usage();
                *p2++ = '\0';
                if (!(p3 = strchr(p, ','))) {
                    flashaddr = (uint32_t)strtoul(p2, NULL, 16);
                    if ((err = p2_FlashFile(p, flashaddr)) != 0)
                        return err;
                }
                else {
                    *p3++ = '\0';
                    flashaddr = (uint32_t)strtoul(p, NULL, 16);
                    hubaddr = (uint32_t)strtoul(p2, NULL, 16);
                    count = (uint32_t)strtoul(p3, NULL, 16);
                    if ((err = p2_Flash(flashaddr, hubaddr, count)) != 0)
                        return err;
                }
                break;
            case 's':
                strip = TRUE;
                break;
            default:
                /* bad options detected with the position-independent options */
                break;
            }
        }

        /* handle the input filename */
        else {
            char *infile, *p;
            uint32_t size;
            infile = argv[i];
            if ((p = strchr(infile, ',')) != NULL) {
                *p++ = '\0';
                loadAddr = strtoul(p, &p, 16);
            }
            printf("Loading '%s' at 0x%08x\n", infile, loadAddr);
            if ((err = LoadFile(infile, loadAddr, strip, writeBootImage, flashAddr, &size, &cogAddr)) != 0)
                return err;
            flashAddr += size;
            loadAddr = BASE;
            strip = FALSE;
        }
    }

    if (startMonitor) {
        if (writeBootImage) {
            StartCmd startCmd;
            startCmd.cmd = (cogId << 8) | CMD_START;
            startCmd.addr = MONITOR_ADDR;
            startCmd.param = MONITOR_PARAM;
            if ((err = p2_FlashBuffer((uint8_t *)&startCmd, sizeof(startCmd), flashAddr, FALSE)))
                return err;
            flashAddr += sizeof(startCmd);
        }
        else {
            if ((err = p2_StartImage(cogId, MONITOR_ADDR, MONITOR_PARAM)) != 0)
                return err;
        }
    }
    else {
        if (!runParamsSet)
            runAddr = cogAddr;
        if (writeBootImage) {
            StartCmd startCmd;
            startCmd.cmd = (cogId << 8) | CMD_START;
            startCmd.addr = runAddr;
            startCmd.param = runParam;
            if ((err = p2_FlashBuffer((uint8_t *)&startCmd, sizeof(startCmd), flashAddr, FALSE)))
                return err;
            flashAddr += sizeof(startCmd);
        }
        else {
            if ((err = p2_StartImage(cogId, runAddr, runParam)) != 0)
                return err;
        }
    }
    
    /* enter terminal mode if requested */
    if (terminalMode) {
        printf("[ Entering terminal mode. Type ESC or Control-C to exit. ]\n");
        fflush(stdout);
        if (baudRate2 != baudRate)
            serial_baud(baudRate2);
        terminal_mode(FALSE, pstMode);
    }

    return 0;
}

/* Usage - display a usage message and exit */
static void Usage(void)
{
printf("\
p2load - a loader for the propeller 2 - version 0.008, 2013-04-23\n\
usage: p2load\n\
         [ -b baud ]               baud rate (default is %d)\n\
         [ -c addr[:param] ]       load a free COG with image at addr and parameter param\n\
         [ -c n,addr[:param] ]     load COG n with image at addr and parameter param\n\
         [ -f file,faddr ]         write contents of file to flash at faddr\n\
         [ -f faddr,haddr,count ]  write count bytes of hub data at haddr to flash at faddr\n\
         [ -h ]                    cog image is at $1000 instead of $0e80\n\
         [ -m ]                    start the ROM monitor instead of the program\n\
         [ -n ]                    set stack top to $8000 for the DE0-Nano\n\
         [ -p port ]               serial port (default is to auto-detect the port)\n\
         [ -P ]                    list available serial ports\n\
         [ -r addr[:param] ]       run program in COG 0 from addr with parameter param\n\
         [ -r n, ]                 run program in COG n\n\
         [ -r n,addr[:param] ]     run program in COG n from addr with parameter param\n\
         [ -s ]                    strip $e80 bytes from start of the file before loading\n\
         [ -t ]                    enter terminal mode after running the program\n\
         [ -T ]                    enter PST-compatible terminal mode\n\
         [ -v ]                    verbose output\n\
         [ -w ]                    write a bootable image to flash\n\
         [ -? ]                    display a usage message and exit\n\
         file[,addr]...            files to load\n", BAUD_RATE);
    exit(1);
}

static int LoadFile(char *infile, uint32_t loadAddr, int strip, int writeBootImage, uint32_t flashAddr, uint32_t *pSize, uint32_t *pCogImage)
{
    uint8_t *imageBuf, *ptr;
    int imageSize, err;
    ElfHdr elfHdr;
    FILE *fp;
    
    /* open the binary */
    if (!(fp = fopen(infile, "rb"))) {
        printf("error: can't open '%s'\n", infile);
        return 1;
    }
    
    /* check for an elf file */
    if (ReadAndCheckElfHdr(fp, &elfHdr)) {
        
        /* load the elf image */
        if (!(imageBuf = LoadElfFile(fp, &elfHdr, &imageSize))) {
            printf("error: can't load elf file '%s'\n", infile);
            return 1;
        }
        
        /* skip over the rom image at the start */
        ptr = imageBuf + BASE;
        imageSize -= BASE;
        
        /* propgcc always creates images that start at 0x1000 */
        *pCogImage = COGIMAGE_HI;
    }
    
    /* load a binary file */
    else {
    
        /* get the size of the binary file */
        fseek(fp, 0, SEEK_END);
        imageSize = (int)ftell(fp);
    
        /* strip off the space occupied by the ROM */
        if (strip) {
            if (fseek(fp, BASE, SEEK_SET) != 0) {
                printf("error: can't skip past ROM space\n");
                return 1;
            }
            if ((imageSize -= BASE) < 0) {
                printf("error: file too small to strip\n");
                return 1;
            }
        }
        
        /* load the complete file */
        else {
            fseek(fp, 0, SEEK_SET);
        }
        
        /* allocate a buffer big enough for the entire image */
        if (!(imageBuf = (uint8_t *)malloc(imageSize))) {
            printf("error: insufficient memory\n");
            return 1;
        }
        ptr = imageBuf;
            
        /* read the binary file */
        if (fread(imageBuf, 1, imageSize, fp) != imageSize) {
            printf("error: can't load binary file '%s'\n", infile);
            return 1;
        }
        
        /* close the binary file */
        fclose(fp);
    }
    
    if (writeBootImage) {
        LoadCmd loadCmd;
        loadCmd.cmd = CMD_LOAD;
        loadCmd.addr = loadAddr;
        loadCmd.count = imageSize;
        if ((err = p2_FlashBuffer((uint8_t *)&loadCmd, sizeof(loadCmd), flashAddr, FALSE)) != 0)
            return err;
        flashAddr += sizeof(loadCmd);
        if ((err = p2_FlashBuffer(ptr, imageSize, flashAddr, TRUE)) != 0)
            return err;
        *pSize = sizeof(loadCmd) + imageSize;
    }
    else {
        if ((err = p2_LoadImage(ptr, loadAddr, imageSize)) != 0)
            return err;
    }
    
    /* free the image buffer */
    free(imageBuf);
        
    return 0;
}

static void *LoadElfFile(FILE *fp, ElfHdr *hdr, int *pImageSize)
{
    uint32_t start, imageSize, cogImagesSize;
    uint8_t *imageBuf, *buf;
    ElfContext *c;
    ElfProgramHdr program;
    int i;

    /* open the elf file */
    if (!(c = OpenElfFile(fp, hdr)))
        return NULL;
        
    /* get the total size of the program */
    if (!GetProgramSize(c, &start, &imageSize, &cogImagesSize)) {
        CloseElfFile(c);
        return NULL;
    }
        
    /* check to see if cog images in eeprom are allowed */
    if (cogImagesSize > 0) {
        CloseElfFile(c);
        return NULL;
    }
    
    /* allocate a buffer big enough for the entire image */
    if (!(imageBuf = (uint8_t *)malloc(imageSize)))
        return NULL;
    memset(imageBuf, 0, imageSize);
        
    /* load each program section */
    for (i = 0; i < c->hdr.phnum; ++i) {
        if (!LoadProgramTableEntry(c, i, &program)
        ||  !(buf = LoadProgramSegment(c, &program))) {
            CloseElfFile(c);
            free(imageBuf);
            return NULL;
        }
        if (program.paddr < COG_DRIVER_IMAGE_BASE)
            memcpy(&imageBuf[program.paddr - start], buf, program.filesz);
    }
    
    /* close the elf file */
    CloseElfFile(c);
    
    /* return the image */
    *pImageSize = imageSize;
    return (void *)imageBuf;
}

static int ShowPort(const char* port, void* data)
{
    printf("%s\n", port);
    return 1;
}

static void ShowPorts(char *prefix)
{
    serial_find(prefix, ShowPort, NULL);
}

static int CheckPort(const char* port, void* data)
{
    CheckPortInfo* info = (CheckPortInfo*)data;
    int rc;
    if (info->verbose)
        printf("Trying %s                    \n", port); fflush(stdout);
    if ((rc = OpenPort(port, info->baud)) != CHECK_PORT_OK)
        return rc;
    if (info->actualport) {
        strncpy(info->actualport, port, PATH_MAX - 1);
        info->actualport[PATH_MAX - 1] = '\0';
    }
    return 0;
}

static int InitPort(char *prefix, char *port, int baud, int verbose, char *actualport)
{
    int result;
    
    if (port) {
        if (actualport) {
            strncpy(actualport, port, PATH_MAX - 1);
            actualport[PATH_MAX - 1] = '\0';
        }
        result = OpenPort(port, baud);
    }
    else {
        CheckPortInfo info;
        info.baud = baud;
        info.verbose = verbose;
        info.actualport = actualport;
        if (serial_find(prefix, CheckPort, &info) == 0)
            result = CHECK_PORT_OK;
        else
            result = CHECK_PORT_NO_PROPELLER;
    }
        
    return result;
}

static int OpenPort(const char* port, int baud)
{
    /* open the port */
    if (serial_init(port, baud) == 0)
        return CHECK_PORT_OPEN_FAILED;
        
    /* check for a propeller on this port */
    if (!p2_HardwareFound(&version)) {
        serial_done();
        return CHECK_PORT_NO_PROPELLER;
    }

    return CHECK_PORT_OK;
}
