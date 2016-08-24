/*
* exif

Copyright (c) 2016, tamachan
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
* 
* library:
* exif c library (Apache License 2.0) (https://github.com/mkttanabe/exif): Copyright (C) 2013 KLab Inc.
* sqlite (public domain)
 */

#ifdef _MSC_VER
#include <windows.h>
#endif
#include <stdio.h>

#include "exif.h"
#include "sqlite3.h"

// sample functions
int sample_removeExifSegment(const char *srcJpgFileName, const char *outJpgFileName);
int sample_removeSensitiveData(const char *srcJpgFileName, const char *outJpgFileName);
int sample_queryTagExists(const char *srcJpgFileName);
int sample_updateTagData(const char *srcJpgFileName, const char *outJpgFileName);



int printTags(const char *srcJpgFileName)
{
    int result;
    void **ifdTableArray = createIfdTableArray(srcJpgFileName, &result);
    if (!ifdTableArray) {
        printf("createIfdTableArray: ret=%d\n", result);
        return result;
	}

	result = queryTagNodeIsExist(ifdTableArray, IFD_0TH, TAG_Make);
	if(result)
	{
		TagNodeInfo *tag = getTagInfo(ifdTableArray, IFD_0TH, TAG_Make);
		if(tag!=NULL)
		{
			if(!tag->error)
			{
				printf("Maker: %s\n", (char*)(tag->byteData));
			}
            freeTagInfo(tag);
		}
	}

    freeIfdTableArray(ifdTableArray);
    return 1;
}

int procImage(const char *srcJpgFileName)
{
    void **ifdArray;
	TagNodeInfo *tag;
	int i, result;
	
    // parse the JPEG header and create the pointer array of the IFD tables
    ifdArray = createIfdTableArray(srcJpgFileName, &result);

    // check result status
    switch (result) {
    case 0: // no IFDs
        printf("[%s] does not seem to contain the Exif segment.\n", srcJpgFileName);
        break;
    case ERR_READ_FILE:
        printf("failed to open or read [%s].\n", srcJpgFileName);
        break;
    case ERR_INVALID_JPEG:
        printf("[%s] is not a valid JPEG file.\n", srcJpgFileName);
        break;
    case ERR_INVALID_APP1HEADER:
        printf("[%s] does not have valid Exif segment header.\n", srcJpgFileName);
        break;
    case ERR_INVALID_IFD:
        printf("[%s] contains one or more IFD errors. use -v for details.\n", srcJpgFileName);
        break;
    default:
        //printf("[%s] createIfdTableArray: result=%d\n", av[1], result);
        break;
    }

    if (!ifdArray) {
        return 0;
	}
	
	
	
	printf("[%s] ", srcJpgFileName);

    // dump all IFD tables
    //for (i = 0; ifdArray[i] != NULL; i++) {
    //    dumpIfdTable(ifdArray[i]);
    //}
    // or dumpIfdTableArray(ifdArray);

    //printf("\n");
	
	unsigned char bCat = 0;
    tag = getTagInfo(ifdArray, IFD_0TH, TAG_Make);
    if (tag) {
        if (!tag->error) {
			printf("[%s](%d) ", tag->byteData, strlen(tag->byteData));
			bCat = 1;
        }
        freeTagInfo(tag);
	}
	if(!bCat)printf("[] ");
	bCat = 0;
	
    tag = getTagInfo(ifdArray, IFD_0TH, TAG_Model);
    if (tag) {
        if (!tag->error) {
			printf("[%s] ", tag->byteData);
			bCat = 1;
        }
        freeTagInfo(tag);
	}
	if(!bCat)printf("[] ");
	bCat = 0;
	
    tag = getTagInfo(ifdArray, IFD_EXIF, TAG_FocalLength);
	if (tag) {
		if (!tag->error && tag->type==TYPE_RATIONAL && tag->count==1) {
			float focuslength = tag->numData[0];
			focuslength /= (float)(tag->numData[1]);
			printf("[%f] ", focuslength);
			bCat = 1;
        }
        freeTagInfo(tag);
	}
	if(!bCat)printf("[] ");
	bCat = 0;
	
    tag = getTagInfo(ifdArray, IFD_EXIF, TAG_FocalLengthIn35mmFormat);
	if (tag) {
		if (!tag->error && tag->type==TYPE_SHORT && tag->count==1) {
			printf("[%hu]\n", (unsigned short)tag->numData[0]);
			bCat = 1;
        }
        freeTagInfo(tag);
	}
	if(!bCat)printf("[]\n");

    // free IFD table array
    freeIfdTableArray(ifdArray);

	
	//result = printTags(av[1]);
    // sample function A: remove the Exif segment in a JPEG file
    // result = sample_removeExifSegment(av[1], "removeExif.jpg");

    // sample function B: remove sensitive Exif data in a JPEG file
    // result = sample_removeSensitiveData(av[1], "removeSensitive.jpg");

    // sample function C: check if "GPSLatitude" tag exists in GPS IFD
    // result = sample_queryTagExists(av[1]);

    // sample function D: Update the value of "Make" tag in 0th IFD
    // result = sample_updateTagData(av[1], "updateTag.jpg");

    // sample function E: Write Exif thumbnail data to file
    // result = sample_saveThumbnail(av[1], "thumbnail.jpg");
	
	return 1;
}

// sample
int main(int ac, char *av[])
{

#ifdef _MSC_VER
#ifdef _DEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF|_CRTDBG_LEAK_CHECK_DF);
#endif
#endif

    if (ac < 2) {
        printf("usage: %s <JPEG FileName> [-v]erbose\n", av[0]);
        return 0;
    }

    // -v option
    if (ac >= 3) {
        if ((*av[2] == '-' || *av[2] == '/') && (*(av[2]+1) == 'v')) {
            setVerbose(1);
        }
	}
	
	const char *zFilename = "cameras.sqlite";
	sqlite3 *pDb = NULL;
	
	int rc = sqlite3_open(zFilename, &pDb);
	if(rc != SQLITE_OK)
	{
		printf("sqlite3_open error!\n");
		return 0;
	}
	
	for(int i=1; i<ac; i++)procImage(av[i]);
	
	sqlite3_close(pDb);

    return 1;
}



/**
 * sample_updateTagData()
 *
 * Update the value of "Make" tag in 0th IFD
 *
 */
int sample_updateTagData(const char *srcJpgFileName, const char *outJpgFileName)
{
    TagNodeInfo *tag;
    int sts, result;
    void **ifdTableArray = createIfdTableArray(srcJpgFileName, &result);

    if (ifdTableArray != NULL) {
        if (queryTagNodeIsExist(ifdTableArray, IFD_0TH, TAG_Make)) {
            removeTagNodeFromIfdTableArray(ifdTableArray, IFD_0TH, TAG_Make);
        }
    } else { // Exif segment not exists
        // create new IFD table
        ifdTableArray = insertIfdTableToIfdTableArray(NULL, IFD_0TH, &result);
        if (!ifdTableArray) {
            printf("insertIfdTableToIfdTableArray: ret=%d\n", result);
            return 0;
        }
    }
    // create a tag info
    tag = createTagInfo(TAG_Make, TYPE_ASCII, 6, &result);
    if (!tag) {
        printf("createTagInfo: ret=%d\n", result);
        freeIfdTableArray(ifdTableArray);
        return result;
    }
    // set tag data
    strcpy((char*)tag->byteData, "ABCDE");
    // insert to IFD table
    insertTagNodeToIfdTableArray(ifdTableArray, IFD_0TH, tag);
    freeTagInfo(tag);

    // write file
    sts = updateExifSegmentInJPEGFile(srcJpgFileName, outJpgFileName, ifdTableArray);

    if (sts < 0) {
        printf("updateExifSegmentInJPEGFile: ret=%d\n", sts);
    }
    freeIfdTableArray(ifdTableArray);
    return sts;
}
