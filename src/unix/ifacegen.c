/* 
 *  Arnold emulator (c) Copyright, Kevin Thacker 1995-2001
 *  
 *  This file is part of the Arnold emulator source code distribution.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
/* generic interface code */
#include "ifacegen.h"
#include "global.h"
#include "../cpc/cpc.h"
#include "../cpc/diskimage/diskimg.h"
#include <string.h>
#include <ctype.h>
#include "../cpc/host.h"
#include <stdio.h>
#include <stdlib.h>
#include "../cpc/snapshot.h"
#include "../cpc/tzx.h"

BOOL Multiface_LoadRomFromFile(const MULTIFACE_ROM_TYPE RomType, const char *pFilename)
{
    unsigned long   RomSize;
    unsigned char   *pRomData;
	int Status = 0;

    /* attempt to load rom data */
    LoadFile((const char *)pFilename,&pRomData, &RomSize);

	if (pRomData!=NULL)
	{
		Status = Multiface_SetRomData(RomType, pRomData, RomSize);

		free(pRomData);
	
		return TRUE;
	}

	return FALSE;
}


BOOL GenericInterface_InsertDiskImage(int DriveID, const char*Filename)
{
	unsigned char *pDiskImage;
	unsigned long DiskImageLength;

    /* load disk image file to memory */
        if(LoadFile(Filename, &pDiskImage, &DiskImageLength))
	{
		BOOL bStatus = ARNOLD_STATUS_OK ==
		DiskImage_InsertDisk(DriveID, pDiskImage, DiskImageLength); 

		free(pDiskImage);

		return bStatus;
	} else

	return FALSE;
}

BOOL	GenericInterface_LoadRom(int RomIndex, const char*FilenameBuffer)
{
	unsigned char *pRomData;
	unsigned long RomDataSize;

	LoadFile(FilenameBuffer, &pRomData, &RomDataSize);
	if (pRomData!=NULL)
	{
		ExpansionRom_SetRomData(pRomData, RomDataSize, RomIndex);
	
		free(pRomData);

		return TRUE;
	}

	return FALSE;
}

BOOL	GenericInterface_LoadSnapshot(const char*SnapshotFilename)
{
	unsigned char *pSnapshotData;
	unsigned long SnapshotDataLength;

	LoadFile(SnapshotFilename, &pSnapshotData, &SnapshotDataLength);

	if (pSnapshotData!=NULL)
	{
		BOOL bStatus;

		bStatus = Snapshot_Insert(pSnapshotData, SnapshotDataLength);

		free(pSnapshotData);
	
		return bStatus;
	}

	return FALSE;
}

BOOL	GenericInterface_SnapshotSave(const char*FilenameBuffer, int SnapshotVersion,int SnapshotSize)
{
	unsigned long nLength;
	unsigned char *pSnapshotData;
	
	nLength = Snapshot_CalculateOutputSize(SnapshotSize, SnapshotVersion);

	pSnapshotData = malloc(nLength);

	if (pSnapshotData!=NULL)
	{
		Snapshot_GenerateOutputData(pSnapshotData, SnapshotSize, SnapshotVersion);

		SaveFile(FilenameBuffer, pSnapshotData, nLength);
	
		free(pSnapshotData);
	}

	return TRUE;
}

BOOL	GenericInterface_InsertTape(const char*Filename)
{
	unsigned char *pTapeImage;
	unsigned long TapeImageLength;

	LoadFile(Filename, &pTapeImage, &TapeImageLength);

	if (pTapeImage!=NULL)
	{
		BOOL bStatus;

		bStatus = TapeImage_Insert(pTapeImage, TapeImageLength);

		free(pTapeImage);
	
		if (!bStatus)
		{
			// now attempt samples..
			if (Sample_Load(Filename))
			{
				return TRUE;
			}
		}

		return FALSE;
	}

	return FALSE;
}

BOOL	GenericInterface_InsertCartridge(const char*pFilename)
{
	unsigned char *pCartridgeData;
	unsigned long CartridgeLength;

	LoadFile(pFilename, &pCartridgeData, &CartridgeLength);

	if (pCartridgeData!=NULL)
	{
		Cartridge_AttemptInsert(pCartridgeData, CartridgeLength);

		free(pCartridgeData);
		return TRUE;
	}

	return FALSE;
}


void	GenericInterface_RemoveCartridge(void)
{
	/* remove old cartridge */
	Cartridge_Remove();
}
