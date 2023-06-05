#include <limits.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <locale.h>
#include <wchar.h>

#include "str.h"
#include "endian.h"
#include "message.h"
#include "file.h"

struct ItemID{
    uint16_t size;
    int type;
    uint8_t* name;
    wchar_t* fname;
};

//TODO: Fix link info
//TODO: Fix extra data

int main(int argc, const char **argv) {
    if (argc < 2) {
        printf("-------------------------------------------\n");
        printf("Lnk Packer\n");
        printf("Creates windows LNK file from exe file\n\n");
        printf("Usage: lnkpack <output_lnk> <input_ext> [-d <input_description>] [-i <input_ico>]\n\n");
        printf("Example: \n");
        printf("lnkpack \"Hello App\" assets\\HelloApp.exe -d \"Hello App Link\" -i assets\\icon.ico\n");
        printf("\n");
        printf("Note: You can use \"{UserDesktop}\" as shortcut to desktop location\n");
        printf("-------------------------------------------\n");
        return 1;
    }

    uint8_t* path;
    uint32_t fsize;
    char* ext = strrchr (argv[2], '.');
    if(!ext || strcmp(ext+1, "exe") != 0) {                
        sendMessage("warn", "Invalid input exe file: ", argv[2]);
        return 1;
    }else{
        FILE* fd = fopen(argv[2], "rb");
        if (fd == NULL) {
            sendMessage("warn", "Failed to open input exe file: ", argv[2]);
            return 1;
        }else{
            path = get_realpath(argv[2]);

            fseek(fd, 0, SEEK_END);
            fsize = ftell(fd);

            fclose(fd);
        }
    }

    char* icon = NULL;
    char* description = NULL;    
    if(argc > 3){
        for (int i = 3; i < argc; i++) {

            if(argv[i][0] != '-' )
                continue;

            const char* value = argv[i+1];
            switch (argv[i][1]) { 
                case 'i':   
                    char* ext = strrchr (value, '.');
                    if(!ext || strcmp(ext+1, "ico") != 0) {                
                        sendMessage("warn", "Invalid input ico file: ", value);
                        return 1;
                    }else{
                        FILE* fd = fopen(argv[2], "rb");
                        if (fd == NULL) {
                            sendMessage("warn", "Failed to open input ico file: ", value);
                            return 1;
                        }else{
                            icon = get_realpath(value);
                            fclose(fd);
                        }
                    }
                break;               
                case 'd':   
                    int len = strlen(value);
                    description = calloc(len+1, 1);
                    memcpy(description, value, len);
                break;
                default:
                    sendMessage("warn", "Invalid param: ", argv[i]);
                    return 1;
            }
        }
    }

    if(description == NULL){
        description = str_prbrk (argv[2], "/\\", false);
    }

    char* location = strdup(argv[1]);
    if(stristr(location, "{UserDesktop}") != NULL){
        strreplace(location, "{UserDesktop}", get_user_path(), true);
    }
    location = strcat(location, ".lnk");

    setlocale(LC_ALL, "");
    sendMessage("info", "Creating lnk for ", location);

    FILE* fd = fopen(location, "w+b");
    if (fd == NULL) {
        sendMessage("warn", "Failed to open output file: ", location);
        return 1;
    }

    //-------------------- File Header ---------------------

    sendMessage("info", "Writing headers... ", NULL);
    
    fwrite_uint32(0x4C, fd);                //Header size

    static uint8_t guid[16] = {0};
	clsid_tdat("00021401-0000-0000-C000-000000000046", guid);
    fwrite(guid, 16, 1, fd);                 //GUID

    //---------------- Link Flags ----------------
    int dataFlags = 0x1;                    //HasTargetIDList
    //dataFlags += 0x2;                     //HasLinkInfo
    dataFlags += 0x4;                       //HasName
    dataFlags += 0x10;                      //HasWorkingDir
    dataFlags += 0x80;                      //IsUnicode
    dataFlags += 0x80000;                   //EnableTargetMetadata
    if(icon != NULL)
        dataFlags += 0x40 + 0x4000;         //HasIconLocation + HasExpIcon

    fwrite_uint32(dataFlags, fd);           //Data flags

    //------------- File Attributes --------------
    int attrFlags = 0x20;                   //Should be archived
    fwrite_uint32(attrFlags, fd);           //Attr flags

    //-------------------------------------------
    time_t ctime = time(NULL);
    fwrite_time_win(ctime, fd);             //Creation date
    fwrite_time_win(ctime, fd);             //Last access date
    fwrite_time_win(ctime, fd);             //Last modification

    fwrite_uint32(fsize, fd);               //File size

    fwrite_uint32(0, fd);                   //Icon index

    fwrite_uint32(0x1, fd);                 //Show Command : SW_SHOWNORMAL

    fwrite_uint16(0, fd);                   //HotKey

    fwrite_uint16(0, fd);                   //Reserved
    fwrite_uint32(0, fd);                   //Reserved
    fwrite_uint32(0, fd);                   //Reserved

    //---------------- Link target id list ----------------

    sendMessage("info", "Writing targets... ", NULL);

    //---------- Collect link data --------------
    uint16_t idListSize = 2                     //ID list size : Header  
                        +4+16                   //My Computer size 
                        +2+1+2+20;              //Disc size

    size_t itemCount = str_cnt(path, "/\\");  //Without disc letter
    struct ItemID idList[itemCount];

    //----------- Path -------------
    int k = 0;
    char *tok = strdup(path);

    char* discLetter = strtok(tok, "/\\");
    char* pathDir = strtok(NULL, "/\\");
    while ( pathDir != NULL )
    {   
        idList[k].size = strlen(pathDir);         //Link section size
        idList[k].type = (k == itemCount-1 ? 0x32 : 0x31 );
        idList[k].name = strdup(pathDir);         //Path part

        idList[k].fname = str_utf16(idList[k].name);

        idList[k].size += wcslen(idList[k].fname)*2;
        idList[k].size += 16+50;

        idListSize += idList[k].size;

        pathDir = strtok( NULL, "/\\" );
        k++;
    }

    //---------- Write link data --------------
    fwrite_uint16(idListSize, fd);              //ID list size   

    //------ My Computer Item --------
    char* clsid = malloc(16);                   //Path(GUID) of My Computer (Computer)    
	clsid_tdat("20D04FE0-3AEA-1069-A2D8-08002B30309D", clsid); 

    fwrite_uint16(4+16, fd);                    //Section size
    fwrite_uint8(0x1f, fd);                     //Section type
    fwrite_uint8(0x50, fd);                     //Section sorting 
    fwrite(clsid, 16, 1, fd);                   //Section GUID

    //------ Disc Item --------
    fwrite_uint16(2+1+2+20, fd);                //Section size
    fwrite_uint8(0x2f, fd);                     //Section type  
    fwrite(discLetter, 2, 1, fd);               //Disc Letter
    fwrite_uint8(0x5c, fd);                     //BackSlash
    fwrite_uint8(0, fd);                        //Null termination
    fwrite_uint32(0, fd);                       //Some data
    fwrite_uint16(0, fd);                       //Some nulls
    fwrite_uint16(0, fd);                       //Some data
    fwrite_char(0, 6, 1, fd);                   //Some nulls
    fwrite_uint16(0, fd);                       //Some data
    fwrite_char(0, 2, 1, fd);                   //Some nulls


    //--------- Path Items------------
    for (size_t i = 0; i < itemCount; i++) {
        int isFile = i == itemCount-1;      

        fwrite_uint16(idList[i].size, fd);                          //Section size
        fwrite_uint8(idList[i].type, fd);                           //Section type
        fwrite_uint8(0, fd);                                        //Unknown (Empty value)
        fwrite_uint32((isFile ? fsize : 0), fd);                    //File size
        fwrite_time_fat(ctime, fd);                                 //Last modification date and time
        fwrite_uint16((isFile ? 0x20 : 0x10), fd);                  //File attribute flags
        fwrite(idList[i].name, strlen(idList[i].name), 1, fd);      //Short name
        fwrite_char(0, 2, 1, fd);                                   //Null termination
       
        //---------- Extension block
        fwrite_uint16(wcslen(idList[i].fname)*2+50, fd);            //Extension size
        fwrite_uint16(0x9, fd);                                     //Extension version
        fwrite_uint32(0xbeef0004, fd);                              //Extension signature
        fwrite_time_fat(ctime, fd);                                 //Extension creation date
        fwrite_time_fat(ctime, fd);                                 //Extension last access date
        fwrite_uint32(0x2e, fd);                                    //Unknown (version or identifier)  

        //---------- MFT entry/sequence
        fwrite_char(0, 6, 1, fd);                                   //MFT entry index
        fwrite_uint16(0, fd);                                       //Sequence number
        //-----------------------------
        fwrite_uint64(0, fd);                                       //Unknown
        fwrite_uint16(0, fd);                                       //Long string size : 0 if no localized name is present
        fwrite_uint32(0, fd);                                       //Unknown (empty values)
        fwrite_uint32(0, fd);                                       //Unknown (have data)
        fwrite(idList[i].fname, wcslen(idList[i].fname)*2, 1, fd);  //Long name

        fwrite_uint16(0, fd);                                       //Null termination
        fwrite_uint16(strlen(idList[i].name)+16, fd);               //First extension block offset without size field
        //-----------------------------
    }

    //--------- Terminal ID ----------
    fwrite_uint16(0, fd);

    //------------------------ LinkInfo --------------------------
    /* 
    sendMessage("info", "Writing link info... ", NULL);

    uint16_t linkSize = 0x2D+strlen(path)+0x1;

    fwrite_uint16(linkSize, fd);    //LinkInfoSize
    fwrite_uint16(0x1C, fd);        //LinkInfoHeaderSize
    fwrite_uint16(0x1, fd);         //LinkInfoFlags

    fwrite_uint16(0x1C, fd);        //VolumeIDOffset
    fwrite_uint16(0x2D, fd);        //LocalBasePathOffset
    fwrite_uint16(0, fd);           //CommonNetworkRelativeLinkOffset : CommonNetworkRelativeLink is not present
    fwrite_uint16(0, fd);           //CommonPathSuffixOffset

    //--------- Volume ID --------
    fwrite_uint16(0x11, fd);        //VolumeIDSize
    fwrite_uint16(0x3, fd);         //DriveType : DRIVE_FIXED
    fwrite_uint16(0x302A, fd);      //DriveSerialNumber : random serial
    fwrite_uint16(0x10, fd);        //VolumeLabelOffset
    fwrite_char(0, 1, 1, fd);       //Data : Empty

    //-----------------------------
    fwrite(path, strlen(path), 1, fd);          //LocalBasePath
    fwrite_uint8(0, fd);                        //CommonPathSuffix

    */

    //--------- Description --------
    sendMessage("info", "Writing description... ", NULL);
    
    wchar_t* utfdescription = str_utf16(description);

    int utfdescriptions = wcslen(utfdescription);
    fwrite_uint16(utfdescriptions, fd);                     //Section size
    fwrite(utfdescription, utfdescriptions*2, 1, fd);       //Section data

    //--------- Working Dir -------- 
    sendMessage("info", "Writing workdir... ", NULL);

    int dirEnd = str_rchr(path, "/\\");
    uint8_t* workdir = calloc(dirEnd+1, 1);
    workdir = strncat(workdir, path, dirEnd);

    wchar_t* utfworkdir = str_utf16(workdir);

    int utfworkdirs = wcslen(utfworkdir);
    fwrite_uint16(utfworkdirs, fd);                         //StringData size
    fwrite(utfworkdir, utfworkdirs*2, 1, fd);               //StringData data

    //--------- Icon File --------
    int utficons = 0;
    wchar_t* utficon;

    if(icon){
        sendMessage("info", "Writing icon... ", NULL);

        utficon = str_utf16(icon);

        utficons = wcslen(utficon);
        fwrite_uint16(utficons, fd);                 //Section size
        fwrite(utficon, utficons*2, 1, fd);          //Section data
    }

    //----------------------- Data -------------------------

    //sendMessage("info", "Writing data... ", NULL);

    //----------------------- Extra Data -------------------------

    sendMessage("info", "Writing extra data... ", NULL);

    //-------- Link tracker -------
    /* 

    fwrite_uint32(0x6, fd);                 //Block Size
    fwrite_uint32(0xA000003, fd);           //Block Signature
    fwrite_uint32(0x58, fd);                //Length
    fwrite_uint32(0, fd);                   //Version 
    fwrite_char(0, 16, 1, fd);              //MachineID : None
    fwrite(str_uuid(), 16, 1, fd);          //Droid 1
    fwrite(str_uuid(), 16, 1, fd);          //Droid 2
    fwrite(str_uuid(), 16, 1, fd);          //DroidBirth 1
    fwrite(str_uuid(), 16, 1, fd);          //DroidBirth 2 

    */

    //-------- Icon tracker -------
    if(icon){
        fwrite_uint32(0x314, fd);                   //Size of the data : always 788
        fwrite_uint32(0xa0000007, fd);              //Signature

        int icons = strlen(icon);
        fwrite(icon, icons, 1, fd);                 //Icon location
        fwrite_char(0, 260-icons, 1, fd);           //Icon location nulls

        //Icon location nulls
        fwrite(utficon, utficons, 1, fd);           //Unicode icon location
        fwrite_char(0, 260-utficons*2, 1, fd);      //Icon location nulls
    }

    //-------- Terminal Block -------
    fwrite_uint32(0, fd);

    //-------------------------- Done ----------------------------

    sendMessage("info", "Lnk created at: ", get_realpath(location));

    fclose(fd);
}