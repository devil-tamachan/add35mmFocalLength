/*
* add35mmFocalLength

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

char* myGetPathBaseName(char *path)
{
	char *basename = (char *)malloc(strlen(path)+2);
	strcpy(basename, path);
	char *pExt = strrchr(basename, '.');
	if(pExt!=NULL)pExt[0]=NULL;
	char *p1 = strrchr(basename, '/');
	char *p2 = strrchr(basename, '\\');
	char *p3 = NULL;
	if(p1 && p2==NULL)p3 = p1+1;
	else if(p1==NULL && p2)p3 = p2+1;
	else if(p1==NULL && p2==NULL)p3 = basename;
	char *basename2 = (char *)malloc(strlen(p3)+2);
	strcpy(basename2, p3);
	free(basename);
	return basename2;
}


int updateTagData(const char *srcJpgFileName, const char *outJpgFileName, short new35mmFocalLength)
{
    TagNodeInfo *tag;
    int sts, result;
    void **ifdTableArray = createIfdTableArray(srcJpgFileName, &result);

    if (ifdTableArray != NULL) {
        if (queryTagNodeIsExist(ifdTableArray, IFD_EXIF, TAG_FocalLengthIn35mmFormat)) {
            removeTagNodeFromIfdTableArray(ifdTableArray, IFD_EXIF, TAG_FocalLengthIn35mmFormat);
        }
    } else { // Exif segment not exists
        // create new IFD table
        ifdTableArray = insertIfdTableToIfdTableArray(NULL, IFD_EXIF, &result);
        if (!ifdTableArray) {
            printf("insertIfdTableToIfdTableArray: ret=%d\n", result);
            return -1;
        }
    }
    // create a tag info
    tag = createTagInfo(TAG_FocalLengthIn35mmFormat, TYPE_SHORT, 1, &result);
    if (!tag) {
        printf("createTagInfo: ret=%d\n", result);
        freeIfdTableArray(ifdTableArray);
        return -1;
    }
	// set tag data
	tag->numData[0] = new35mmFocalLength;
    // insert to IFD table
    insertTagNodeToIfdTableArray(ifdTableArray, IFD_EXIF, tag);
    freeTagInfo(tag);

    // write file
    sts = updateExifSegmentInJPEGFile(srcJpgFileName, outJpgFileName, ifdTableArray);

    if (sts < 0) {
		printf("updateExifSegmentInJPEGFile: ret=%d\n", sts);
    }
    freeIfdTableArray(ifdTableArray);
	return (sts<0)?sts:1;
}

int procImage(const char *srcJpgFileName, sqlite3 *pDb, sqlite3_stmt *pSearchDiagonalStmt)
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
	
	char *pMaker = NULL;
	char *pModel = NULL;
	double dblFocalLen = 0.0f;
	
	unsigned char bCat = 0;
    tag = getTagInfo(ifdArray, IFD_0TH, TAG_Make);
    if (tag) {
        if (!tag->error) {
			printf("[%s] ", tag->byteData);
			pMaker = (char*)malloc(strlen((const char*)tag->byteData)+2);
			if(pMaker)
			{
				strcpy(pMaker, (const char*)tag->byteData);
				bCat = 1;
			}
        }
        freeTagInfo(tag);
	}
	if(!bCat)
	{
		printf("[] ");
		goto procImage_ERR1;
	}
	bCat = 0;
	
    tag = getTagInfo(ifdArray, IFD_0TH, TAG_Model);
    if (tag) {
        if (!tag->error) {
			printf("[%s] ", tag->byteData);
			pModel = (char*)malloc(strlen((const char*)tag->byteData)+2);
			if(pMaker)
			{
				strcpy(pModel, (const char*)tag->byteData);
				bCat = 1;
			}
        }
        freeTagInfo(tag);
	}
	if(!bCat)
	{
		printf("[] ");
		goto procImage_ERR1;
	}
	bCat = 0;
	
    tag = getTagInfo(ifdArray, IFD_EXIF, TAG_FocalLength);
	if (tag) {
		if (!tag->error && tag->type==TYPE_RATIONAL && tag->count==1) {
			dblFocalLen = tag->numData[0];
			dblFocalLen /= (double)(tag->numData[1]);
			printf("[%lf] ", dblFocalLen);
			bCat = 1;
        }
        freeTagInfo(tag);
	}
	if(!bCat)
	{
		printf("[] ");
		goto procImage_ERR1;
	}
	bCat = 0;
	
    tag = getTagInfo(ifdArray, IFD_EXIF, TAG_FocalLengthIn35mmFormat);
	if (tag) {
		if (!tag->error && tag->type==TYPE_SHORT && tag->count==1) {
			printf("[%hu]\n", (unsigned short)tag->numData[0]);
			bCat = 1;
			goto procImage_ERR1;
        }
        freeTagInfo(tag);
	}
	if(!bCat)
	{
		printf("[]\n");
	}
	
	if(pMaker==NULL||pModel==NULL)goto procImage_ERR1;
	
	int rc = 0;
	
	sqlite3_reset(pSearchDiagonalStmt);
	double dblDiagonal = -1.0;
	sqlite3_bind_text(pSearchDiagonalStmt, 1, pMaker, -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(pSearchDiagonalStmt, 2, pModel, -1, SQLITE_TRANSIENT);
	do {
		rc = sqlite3_step(pSearchDiagonalStmt);
		if(rc==SQLITE_ROW)
		{
			dblDiagonal = sqlite3_column_double(pSearchDiagonalStmt, 0);
			break;
		}
		if(rc==SQLITE_OK || rc==SQLITE_BUSY || rc==SQLITE_LOCKED ){
          sqlite3_sleep(5);
		}
	} while(rc==SQLITE_OK || rc==SQLITE_BUSY || rc==SQLITE_LOCKED);
	sqlite3_reset(pSearchDiagonalStmt);
	if(dblDiagonal==-1.0)goto procImage_ERR1;
	
	const double dbl35mmDiagonal = 43.2666153056;//sqrt(35.0*35.0 + 24.0*24.0); fullsize sensor's diagonal
	
	short i35mmFocalLenght = (short)((dblFocalLen * (dbl35mmDiagonal / dblDiagonal)) + 0.5);
	//printf("[%d(%lf) = (short)((%lf * (%lf / %lf)) + 0.5)]\n", i35mmFocalLenght, ((dblFocalLen * (dbl35mmDiagonal / dblDiagonal)) + 0.5), dblFocalLen, dbl35mmDiagonal, dblDiagonal);
	printf("[35mmFocalLength: %d]\n", i35mmFocalLenght);
	
	char *tmpname = NULL;
#ifdef _MSC_VER
  if((tmpname=_tempnam(NULL, "a35"))!=NULL)
#else
	if((tmpname=tmpnam(NULL)) != NULL)
#endif
	{
		int r = updateTagData(srcJpgFileName, tmpname, i35mmFocalLenght);
		if(r==1)
		{
			if (remove(srcJpgFileName)!=0)
			{
				remove(tmpname);
				goto procImage_ERR1;
			}
			if (rename(tmpname, srcJpgFileName) != 0)
			{
				printf("\n!!!!!!rename error. lost original file!!!!!!!\nbackup file here: \"%s\"\n", tmpname);
				goto procImage_ERR1;
			}
		}
	} else {
		printf("tmpnam error\n");
		goto procImage_ERR1;
	}
	if(pMaker)free(pMaker);
	if(pModel)free(pModel);
    freeIfdTableArray(ifdArray);
	
	return 1;

	// free IFD table array
  procImage_ERR1:
	if(pMaker)free(pMaker);
	if(pModel)free(pModel);
    freeIfdTableArray(ifdArray);
	
	return -1;
}

#ifndef ADD35MMFOCAL_NOMAIN

int main(int ac, char *av[])
{
    if (ac < 2) {
        printf("usage: %s <JPEG FileName>\n", av[0]);
        return 0;
    }
	
	const char *zFilename = "exif.sqlite";
	sqlite3 *pDb = NULL;
	char *errMsg = NULL;
	
	int rc = sqlite3_open(zFilename, &pDb);
	if(rc != SQLITE_OK)
	{
		printf("%s\n", errMsg);
		sqlite3_free(errMsg);errMsg = NULL;
		return 0;
	}
	
	sqlite3_stmt *pSearchDiagonalStmt = NULL;
	rc = sqlite3_prepare_v2(pDb, "SELECT models.diagonal, models.sensorwidth, models.id from models INNER JOIN makers ON models.makerid = makers.id WHERE makers.name = ?1 AND models.name = ?2 LIMIT 1;", -1, &pSearchDiagonalStmt, NULL);
	if(rc != SQLITE_OK)
	{
		sqlite3_finalize(pSearchDiagonalStmt);
		return 0;
	}
	
	for(int i=1; i<ac; i++)procImage(av[i], pDb, pSearchDiagonalStmt);
	
	sqlite3_finalize(pSearchDiagonalStmt);
	
	sqlite3_close(pDb);
	
	return 1;
}

#endif
