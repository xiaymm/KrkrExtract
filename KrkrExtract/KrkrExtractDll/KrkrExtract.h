#pragma once

#include "tp_stub.h"
#include "my.h"
#include "KrkrHeaders.h"
#include <atomic>

#define WM_UUPAK_OK (WM_USER + 1919)

enum INIT_TYPE
{
	InitedByExeModule,
	InitedByDllModule
};

enum PNG_DECODE_INFO
{
	PNG_RAW,
	PNG_SYS,
	PNG_ALL
};

enum TEXT_DECODE_INFO
{
	TEXT_RAW,
	TEXT_DECODE,
	TEXT_ALL
};

enum PSB_DECODE_INFO
{
	PSB_RAW   = 1UL,
	PSB_TEXT  = 1UL << 1,
	PSB_DECOM = 1UL << 2,
	PSB_IMAGE = 1UL << 3,
	PSB_ANM   = 1UL << 4,
	PSB_JSON  = 1UL << 5,
	PSB_ALL   = 0xFFFFFFFFUL
};

enum TJS2_DECODE_INFO
{
	TJS2_RAW,
	TJS2_DECOM
};

enum AMV_DECODE_INFO
{
	AMV_JPG,
	AMV_PNG,
	AMV_MNG,
	AMV_RAW
};


enum PBD_DECODE_INFO
{
	PBD_RAW,
	PBD_JSON
};

enum TLG_DECODE_INFO
{
	TLG_RAW,
	TLG_BUILDIN,
	TLG_SYS,
	TLG_PNG,
	TLG_JPG
};

enum ModuleVersion
{
	Krkr2,
	Krkrz, //M2
	Krkr3  
};


template<class PtrType> PtrType Nt_EncodePointer(PtrType Pointer, ULONG_PTR Cookie)
{
	return (PtrType)_rotr((ULONG_PTR)PtrXor(Pointer, Cookie), Cookie & 0x1F);
}

template<class PtrType> PtrType Nt_DecodePointer(PtrType Pointer, ULONG_PTR Cookie)
{
	return (PtrType)PtrXor(_rotl((ULONG_PTR)Pointer, Cookie & 0x1F), Cookie);
}

ULONG64 FASTCALL GenerationCRC64(ULONG64 crc, LPCBYTE s, ULONG_PTR l);

typedef HRESULT(_stdcall *tV2Link)(iTVPFunctionExporter *);
typedef void(__stdcall * XP3Filter)(tTVPXP3ArchiveExtractionFilter);
typedef iTVPFunctionExporter*   (__stdcall *TVPGetFunctionExporterFunc)();
typedef VOID(WINAPI *TVPXP3ArchiveExtractionFilterFunc)(tTVPXP3ExtractionFilterInfo *info);
typedef tTJSBinaryStream* (FASTCALL * FuncCreateStream)(const ttstr &, tjs_uint32);
typedef PVOID (CDECL * FuncHostAlloc)(ULONG);

#define szApplicationName   L"[X'moe]Welcome to KrkrExtract(version : %s, built on : %s)"
#define szWindowClassName   L"XP3ExtractMainClass"

//A.B.C.D : 0 <= X <= 255
#define _XP3ExtractVersion_ L"Ver 4.0.1.4"


class MemEntry
{
public:
	MemEntry() : Buffer(NULL), Size(0), Hash(0){}

	MemEntry& operator = (const MemEntry& Other)
	{
		Buffer = Other.Buffer;
		Size   = Other.Size;
		Hash   = Other.Hash;

		return *this;
	}

	PVOID   Buffer;
	ULONG   Size;
	ULONG64 Hash;
};

class Xp3FileEntry
{
public:
	HANDLE hFile;
	BOOL   Status;

	Xp3FileEntry() :
		hFile(INVALID_HANDLE_VALUE),
		Status(FALSE)
	{
	}
};

class GlobalData
{
	GlobalData();
	~GlobalData();

	VOID NTAPI ExitKrkr();

	static GlobalData* Handle;

public:
	static GlobalData* GetGlobalData();
	
public:
	BOOL                            InitedByModule;
	iTVPFunctionExporter *          TVPFunctionExporter;
	tTVPXP3ArchiveExtractionFilter  pfGlobalXP3Filter;
	HMODULE                         hSelfModule;
	HMODULE                         hHostModule;
	ULONG                           ModuleType;
	BOOL                            HasConsole;
	std::atomic<BOOL>               isRunning;
	BOOL                            DebugOn;
	BOOL                            IsAllPackReaded;
	static BOOL                     WindowIsInited;

	std::vector<std::wstring>              FileNameList;
	std::map<std::wstring, Xp3FileEntry>   FileInitList;

	API_POINTER(GetProcAddress)         StubGetProcAddress;
	API_POINTER(CreateProcessInternalW) StubCreateProcessInternalW;
	ULONG_PTR                           OriginalReturnAddress, FirstSectionAddress;
	ULONG                               SizeOfImage;
	tV2Link                             StubV2Link;
	FuncCreateStream                    StubTVPCreateStream;
	FuncHostAlloc                       StubHostAlloc;
	ULONG_PTR						    IStreamAdapterVtable;

private:
	ULONG                           PngFlag;
	ULONG                           TlgFlag;
	ULONG                           TextFlag;
	ULONG                           PsbFlag;
	ULONG                           TjsFlag;
	ULONG                           AmvFlag;
	ULONG                           PbdFlag;

	WCHAR                           Folder   [MAX_PATH];
	WCHAR                           GuessPack[MAX_PATH];
	WCHAR                           OutPack  [MAX_PATH];
	
public:
	WCHAR                           SelfPath[MAX_PATH];
	WCHAR                           DragFileName[MAX_PATH];

	BOOL                            InheritIcon;
	BOOL                            IsProtection;

public:
	ULONG     SetPngFlag(ULONG Flag);
	ULONG     GetPngFlag();

	ULONG     SetTlgFlag(ULONG Flag);
	ULONG     GetTlgFlag();

	ULONG     SetTextFlag(ULONG Flag);
	ULONG     GetTextFlag();

	ULONG     SetAmvFlag(ULONG Flag);
	ULONG     GetAmvFlag();

	ULONG     SetPbdFlag(ULONG Flag);
	ULONG     GetPbdFlag();

	ULONG     SetTjsFlag(ULONG Flag);
	ULONG     GetTjsFlag();

	ULONG     AddPsbFlag(ULONG Flag);
	ULONG     DeletePsbFlag(ULONG Flag);
	BOOL      PsbFlagOn(ULONG Flag);
	VOID      DebugPsbFlag();

	VOID      SetFolder(LPCWSTR Name);
	VOID      SetGuessPack(LPCWSTR Name);
	VOID      SetOutputPack(LPCWSTR Name);

	VOID      GetFolder(LPWSTR Name, ULONG BufferMaxLength);
	VOID      GetGuessPack(LPWSTR Name, ULONG BufferMaxLength);
	VOID      GetOutputPack(LPWSTR Name, ULONG BufferMaxLength);

	BOOL      FindCodeSlow(PCCHAR Start, ULONG Size, PCCHAR Pattern, ULONG PatternLen);

	VOID      AddFileEntry(PCWSTR FileName, ULONG Length);
	VOID      VirtualConsolePrint(PCWSTR Format, ...);

	NTSTATUS NTAPI InitWindow();
	NTSTATUS NTAPI InitHook(LPCWSTR ModuleName, PVOID ImageBase);
	NTSTATUS NTAPI InitHookNull();

	VOID NTAPI EnableAll(HWND hWnd);
	VOID NTAPI DisableAll(HWND hWnd);
	VOID NTAPI AdjustCP(HWND hWnd);
	VOID NTAPI SetProcess(HWND hWnd, ULONG Value);
	VOID NTAPI SetCount(DWORD Count);
	VOID NTAPI SetCurFile(DWORD iPos);
	VOID NTAPI ForceReset();
	VOID NTAPI Reset();
	VOID NTAPI SetDllModule(HMODULE hModule);

	ForceInline static wstring FASTCALL GetUpperExtName(wstring& FileName)
	{
		auto Index = FileName.find_last_of(L'.');
		if (Index != std::wstring::npos)
		{
			wstring Result;
			for (auto& EachChar : FileName.substr(Index + 1, std::wstring::npos))
			{
				Result += CHAR_UPPER(EachChar);
			}
			return Result;
		}

		return {};
	}

	HWND                        MainWindow;
	WCHAR                       WinText[1000];
	BOOL                        Inited;
	BOOL                        IsM2Format;
	DWORD                       M2ChunkMagic;
	BOOL                        RawExtract;
	ULONG_PTR                   CountFile;
	vector<XP3Index>            ItemVector;
	HANDLE                      WorkerThread;
	HANDLE                      CurrentTempHandle;
	wstring                     CurrentTempFileName;
	BOOL                        FakePngWorkerInited;
	WCHAR                       CurFileName[MAX_PATH];
	
	std::map<ULONG64, MemEntry> SpecialChunkMap;
	std::map<ULONG, MemEntry>   SpecialChunkMapBySize;


	using SpecialChunkDecoderFunc = ULONG(CDECL*) (PBYTE, PBYTE, ULONG);
	
	BOOL                        IsSpcialChunkEncrypted;
	SpecialChunkDecoderFunc     SpecialChunkDecoder;
};

//Init once
NTSTATUS NTAPI InitExporter(iTVPFunctionExporter *Exporter);
NTSTATUS NTAPI InitExporterByExeModule(iTVPFunctionExporter *Exporter);
