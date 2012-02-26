#include "headers.h"
//#define PRINT_MAP_FILE

//****************************************************************************
ElfFile::ElfFile(char *pchElfFile)
{
  using namespace std;
  bFileOpened = false;
  iElfSize = 0;
  ifstream *pisElf = new ifstream(pchElfFile,ios::binary);
  if (pisElf->is_open())
  {
    bFileOpened = true;
    pisElf->seekg(0,ios::end);
    iElfSize = (int32)pisElf->tellg();
    if(iElfSize)
    {
      arrElf = (uint8*)malloc(iElfSize);
      pisElf->seekg(0,ios::beg);
      pisElf->read((int8*)arrElf,iElfSize);
    }
    pisElf->close();
    strcpy_s(chElfFile, FILE_NAME_STRING_MAX,pchElfFile);
#ifdef PRINT_MAP_FILE
    strcpy_s(chMapFile, FILE_NAME_STRING_MAX,pchElfFile);
    strcat_s(chMapFile, FILE_NAME_STRING_MAX, ".map");
    posMapFile = new ofstream(chMapFile,ios::out);
#endif
    strcpy_s(chAsmFile, FILE_NAME_STRING_MAX,pchElfFile);
    strcat_s(chAsmFile, FILE_NAME_STRING_MAX, ".asm");
    posAsmFile = new ofstream(chAsmFile,ios::out);
  }
  free(pisElf);

  memset(image,0,ELF_IMAGE_MAXSIZE*sizeof(uint32));
  iImageBytes = 0;
}

ElfFile::~ElfFile()
{
  if(bFileOpened)
  {
    free(arrElf);
#ifdef PRINT_MAP_FILE
    posMapFile->close();
    free(posMapFile);
#endif
    posAsmFile->close();
    free(posAsmFile);
  }
  if((Elf32_Ehdr.e_shoff)&&(Elf32_Ehdr.e_shnum)) free(pSectionHeader);
  if((Elf32_Ehdr.e_phoff)&&(Elf32_Ehdr.e_phnum)) free(pProgramHeader);
}

//****************************************************************************
void ElfFile::Load()
{
  if(!bFileOpened)
  {
    PUT_STRING("{elf} Error: File not found\n");
    return;
  }

  if (ReadElfHeader())
  {
    PUT_STRING("{elf} Error: \"%s\" file isn't correct\n", chElfFile);
    return;
  }

  if(Elf32_Ehdr.e_shoff) ReadSectionHeader();
  if(Elf32_Ehdr.e_phoff) ReadProgramHeader();

  clSparcV8.Disassemler(Elf32_Ehdr.e_entry, image, iImageBytes, posAsmFile);
}

//****************************************************************************
int32 ElfFile::ReadElfHeader()
{
  int32 iErr = 0;
  memcpy((char*)&Elf32_Ehdr,arrElf,sizeof(ElfHeaderType));
  char chMagic[5] = {0x7f,'E','L','F',0};
  bool bMagicPass = true;
  for(int32 i=0; i<4; i++)
  {
    if(Elf32_Ehdr.e_ident[i]!=chMagic[i]) bMagicPass=false;
  }
  if(!bMagicPass) return (iErr=1);


  PUT_STRING("{elf} ELF file has been opened\n");

  switch(Elf32_Ehdr.e_ident[ElfHeaderType::EI_CLASS])
  {
    case ElfHeaderType::ELFCLASS32:  PUT_STRING("{elf} 32-bits objects\n"); break;
    case ElfHeaderType::ELFCLASS64:  PUT_STRING("{elf} 64-bits objects\n"); break;
    default:                          PUT_STRING("{elf} Err: Invalid class\n"); break;
  }

  switch(Elf32_Ehdr.e_ident[ElfHeaderType::EI_DATA])
  {
    case ElfHeaderType::ELFDATA2LSB: PUT_STRING("{elf} Least significant byte read last\n"); break;
    case ElfHeaderType::ELFDATA2MSB: PUT_STRING("{elf} Most significant byte read last\n"); break;
    default:                          PUT_STRING("{elf} Err: Invalid Data Encoding\n"); break;
  }

  switch(Elf32_Ehdr.e_ident[ElfHeaderType::EI_VERSION])
  {
    case ElfHeaderType::EV_CURRENT: PUT_STRING("{elf} Current version\n"); break;
    default:                         PUT_STRING("{elf} Err: Invalid version\n"); break;
  }

  SwapBytes(Elf32_Ehdr.e_type);
  switch(Elf32_Ehdr.e_type)
  {
    case ElfHeaderType::ET_NONE: PUT_STRING("{elf} No file type\n"); break;
    case ElfHeaderType::ET_REL:  PUT_STRING("{elf} Relocable file\n"); break;
    case ElfHeaderType::ET_EXEC: PUT_STRING("{elf} Executable file\n"); break;
    case ElfHeaderType::ET_DYN:  PUT_STRING("{elf} Shared object file\n"); break;
    case ElfHeaderType::ET_CORE: PUT_STRING("{elf} Core file\n"); break;
    default:                      PUT_STRING("{elf} Processor-specific file type\n"); break;
  }

  SwapBytes(Elf32_Ehdr.e_machine);
  switch(Elf32_Ehdr.e_machine)
  {
    case ElfHeaderType::EM_NONE:  PUT_STRING("{elf} No machine type\n"); break;
    case ElfHeaderType::EM_M32:   PUT_STRING("{elf} AT&T WE 32100\n"); break;
    case ElfHeaderType::EM_SPARC: PUT_STRING("{elf} SPARC\n"); break;
    case ElfHeaderType::EM_386:   PUT_STRING("{elf} Intel 80386\n"); break;
    case ElfHeaderType::EM_68K:   PUT_STRING("{elf} Motorola 68000\n"); break;
    case ElfHeaderType::EM_88K:   PUT_STRING("{elf} Motorola 88000\n"); break;
    case ElfHeaderType::EM_860:   PUT_STRING("{elf} Intel 80860\n"); break;
    default:                       PUT_STRING("{elf} Err: Machine unknown\n"); break;
  }

  SwapBytes(Elf32_Ehdr.e_entry);
  PUT_STRING("{elf} Entry point = 0x%08x\n",Elf32_Ehdr.e_entry);
  
  SwapBytes(Elf32_Ehdr.e_phoff);//Program header offset
  SwapBytes(Elf32_Ehdr.e_shoff);
  SwapBytes(Elf32_Ehdr.e_flags);
  SwapBytes(Elf32_Ehdr.e_ehsize);   
  SwapBytes(Elf32_Ehdr.e_phentsize);// size of one entry in the Program header. All entries are the same size
  SwapBytes(Elf32_Ehdr.e_phnum);    // number of entries in a Program header
  SwapBytes(Elf32_Ehdr.e_shentsize);//section header size. all entries are the same size
  SwapBytes(Elf32_Ehdr.e_shnum);    // number of section headers
  SwapBytes(Elf32_Ehdr.e_shstrndx);

  return iErr;
}

//****************************************************************************
void ElfFile::ReadProgramHeader()
{
  if(Elf32_Ehdr.e_phnum)
  {
    pProgramHeader = (ProgramHeaderType*)malloc(Elf32_Ehdr.e_phnum*sizeof(ProgramHeaderType));
    memcpy((char*)pProgramHeader, &arrElf[Elf32_Ehdr.e_phoff], Elf32_Ehdr.e_phnum*sizeof(ProgramHeaderType));
  }
  
  for(int32 i=0; i<Elf32_Ehdr.e_phnum; i++)
  {
    SwapBytes(pProgramHeader[i].p_type);
    SwapBytes(pProgramHeader[i].p_offset);
    SwapBytes(pProgramHeader[i].p_vaddr);
    SwapBytes(pProgramHeader[i].p_paddr);
    SwapBytes(pProgramHeader[i].p_filesz);
    SwapBytes(pProgramHeader[i].p_memsz);
    SwapBytes(pProgramHeader[i].p_flags);
    SwapBytes(pProgramHeader[i].p_align);
  }
}

//****************************************************************************
void ElfFile::ReadSectionHeader()
{
  iTotalStrings = 0;
  if(Elf32_Ehdr.e_shnum)
  {
    pSectionHeader = (SectionHeaderType*)malloc(Elf32_Ehdr.e_shnum*sizeof(SectionHeaderType));
    memcpy((char*)pSectionHeader, &arrElf[Elf32_Ehdr.e_shoff], Elf32_Ehdr.e_shnum*sizeof(SectionHeaderType));
  }
  for(int32 i=0; i<Elf32_Ehdr.e_shnum; i++)
  {
    SwapBytes(pSectionHeader[i].sh_name);//Index in a header section table (gives name of section)
    SwapBytes(pSectionHeader[i].sh_type);
    SwapBytes(pSectionHeader[i].sh_flags);
    SwapBytes(pSectionHeader[i].sh_addr);
    SwapBytes(pSectionHeader[i].sh_offset);
    SwapBytes(pSectionHeader[i].sh_size);
    SwapBytes(pSectionHeader[i].sh_link);
    SwapBytes(pSectionHeader[i].sh_info);
    SwapBytes(pSectionHeader[i].sh_addralign);
    SwapBytes(pSectionHeader[i].sh_entsize);
    
    if(pSectionHeader[i].sh_type==SectionHeaderType::SHT_STRTAB)
    {
      uint32 iStrLength=0;
      uint32 iStrInc =0;
      while(iStrLength<pSectionHeader[i].sh_size)
      {
        pStr[iTotalStrings] = &arrElf[pSectionHeader[i].sh_offset + iStrLength];
        iStrInc = S_STRING(chTmp,"%s",&arrElf[pSectionHeader[i].sh_offset + iStrLength]);
        if(iStrInc==0) iStrLength++;
        else           iStrLength+=iStrInc;
        iTotalStrings++;
      }
    }

  }

  // Read symbols:
  for(int32 i=0; i<Elf32_Ehdr.e_shnum; i++)
  {
    if(pSectionHeader[i].sh_type==SectionHeaderType::SHT_SYMTAB)
    {
      SymbolTableType clSymbol;
      pStr[pSectionHeader[pSectionHeader[i].sh_link].sh_name];
      uint32 uiSmbLength=0;
      while(uiSmbLength<pSectionHeader[i].sh_size)
      {
        clSymbol = *((SymbolTableType*)&arrElf[pSectionHeader[i].sh_offset+uiSmbLength]);
        SwapBytes(clSymbol.st_name);
        SwapBytes(clSymbol.st_value);
        SwapBytes(clSymbol.st_size);
        SwapBytes(clSymbol.st_shndx);

        // !!! Something wrong. I'm not correct read symbols!!!!!!!! ERROR!!!!!!
        pStr[clSymbol.st_name];
        uiSmbLength += sizeof(SymbolTableType);
      }
    }
  }
  
  // Print implemented sections:
  for(int32 i=0; i<Elf32_Ehdr.e_shnum; i++)
  {
    //If the section will appear in the memory image of a process, this member gives the
    //address at which the section’s first byte should reside. Otherwise, the member contains 0.
    if(pSectionHeader[i].sh_addr)
    {
      PUT_STRING("{elf} Loading section \"%s\": 0x%08x..0x%08x\n",
        pStr[pSectionHeader[i].sh_name],
        pSectionHeader[i].sh_addr,
        pSectionHeader[i].sh_addr+pSectionHeader[i].sh_size
      );
      
      WriteMemoryMap(&pSectionHeader[i]);
    }
  }
}

//****************************************************************************
void ElfFile::WriteMemoryMap(SectionHeaderType *p)
{
  uint32 ImgAddr;
  // source code or data:
  uint32 word;
  int32 MsbOrder[4]={0,1,2,3};
  int32 LsbOrder[4]={3,2,1,0};
  int32 *byte;
  // select byte order:
  if(Elf32_Ehdr.e_ident[ElfHeaderType::EI_DATA]==ElfHeaderType::ELFDATA2MSB) byte = MsbOrder;
  else                                                                       byte = LsbOrder;
  
  // print Program defined section:
  if(p->sh_type==SectionHeaderType::SHT_PROGBITS)
  {
    for(uint32 n=0; n<p->sh_size;n+=4)
    {
      word = uint32(arrElf[p->sh_offset+n+byte[0]])<<24;
      word |= uint32(arrElf[p->sh_offset+n+byte[1]])<<16;
      word |= uint32(arrElf[p->sh_offset+n+byte[2]])<<8;
      word |= uint32(arrElf[p->sh_offset+n+byte[3]])<<0;

      ImgAddr = (p->sh_addr+n - Elf32_Ehdr.e_entry)/sizeof(uint32);
      if(ImgAddr<ELF_IMAGE_MAXSIZE) image[ImgAddr] = word;
      else                          PUT_STRING("{elf} Error: Internal Image maximum address is overrun\n");

      LibBackDoorLoadRAM(p->sh_addr+n, word);

      // Store image size using maximal address value:
      if(uint32(iImageBytes) < (p->sh_addr+n - Elf32_Ehdr.e_entry))
        iImageBytes = int32(p->sh_addr+n - Elf32_Ehdr.e_entry);

#ifdef PRINT_MAP_FILE
      S_STRING(chTmp,"0x%08x: 0x%08x ", p->sh_addr+n, word );
      *posMapFile << chTmp;

      if(n==0)*posMapFile << pStr[p->sh_name];
      *posMapFile << "\n";
#endif
    }
  }
  
  // print allocated space:
  if(p->sh_type==SectionHeaderType::SHT_NOBITS)
  {
    for(uint32 n=0; n<p->sh_size;n+=4)
    {
      LibBackDoorLoadRAM(p->sh_addr+n, 0);
#ifdef PRINT_MAP_FILE
      S_STRING(chTmp,"0x%08x: 0x%08x ", p->sh_addr+n, 0);
      *posMapFile << chTmp;
      if(n==0)*posMapFile << pStr[p->sh_name] ;
      *posMapFile << "\n";
#endif
    }
  }

}

//****************************************************************************
void ElfFile::SwapBytes(Elf32_Half& v)
{
  Elf32_Half tmp=v;
  switch(Elf32_Ehdr.e_ident[ElfHeaderType::EI_DATA])
  {
    case ElfHeaderType::ELFDATA2LSB: break;  //Intel native format
    case ElfHeaderType::ELFDATA2MSB: v = ((tmp>>8)&0xff)|((tmp&0xFF)<<8); break;
    default:;
  }
}

//****************************************************************************
void ElfFile::SwapBytes(uint32& v)
{
  uint32 tmp=v;
  switch(Elf32_Ehdr.e_ident[ElfHeaderType::EI_DATA])
  {
    case ElfHeaderType::ELFDATA2LSB: break;  //Intel native format
    case ElfHeaderType::ELFDATA2MSB: v = ((tmp>>24)&0xff)|((tmp>>8)&0xff00)|((tmp<<8)&0xff0000)|((tmp&0xFF)<<24); break;
    default:;
  }
}

//****************************************************************************
int32 ElfFile::PUT_STRING(const char *_format, ... )
{
  int32 iRet = 0;
  va_list arg;
  va_start( arg, _format );
  
  iRet = vsprintf_s( chPutString, PUT_STRING_SIZE, _format, arg );
  va_end( arg );
  
  iRet = printf_s("%s", chPutString);
  return iRet;
}

//****************************************************************************
int32 ElfFile::S_STRING(char*s, const char *_format, ... )
{
  int32 iRet = 0;
  va_list arg;
  va_start( arg, _format );
  
  iRet = vsprintf_s( s, PUT_STRING_SIZE, _format, arg );
  va_end( arg );
  return iRet;
}


