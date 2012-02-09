#include "headers.h"
#define ELF_FILE "E:\\Projects\\CppProjects\\20110817_leon3\\source\\firmware\\HelloWorld\\target_VIRTEX_ML605\\elf\\HelloWorld.elf"
//****************************************************************************
ElfFile::ElfFile()
{
  using namespace std;
  bFileOpened = false;
  iElfSize = 0;
  ifstream *pisElf = new ifstream(ELF_FILE,ios::binary);
  if (pisElf->is_open())
  {
    bFileOpened = true;
    pisElf->seekg(0,ios::end);
    iElfSize = (int32)pisElf->tellg();
    if(iElfSize)
    {
      arrElf = (char*)malloc(iElfSize);
      pisElf->seekg(0,ios::beg);
      pisElf->read(arrElf,iElfSize);
    }
    pisElf->close();
  }
  free(pisElf);
}

ElfFile::~ElfFile()
{
  if(bFileOpened) free(arrElf);
  if((Elf32_Ehdr.e_shoff)&&(Elf32_Ehdr.e_shnum)) free(pSectionHeader);
}

//****************************************************************************
void ElfFile::Update()
{
  if(!bFileOpened) return;

  ReadElfHeader();
}

//****************************************************************************
void ElfFile::ReadElfHeader()
{
  memcpy((char*)&Elf32_Ehdr,arrElf,sizeof(ElfHeaderType));
  char chMagic[5] = {0x7f,'E','L','F',0};
  bool bMagicPass = true;
  for(int32 i=0; i<4; i++)
  {
    if(Elf32_Ehdr.e_ident[i]!=chMagic[i]) bMagicPass=false;
  }
  if(bMagicPass) PUT_STRING("{elf} ELF file has been opened\n");
  else           PUT_STRING("{elf} Err: \"%s\" isn't ELF file\n",ELF_FILE);

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

  if(Elf32_Ehdr.e_phoff) ReadProgramHeader();
  if(Elf32_Ehdr.e_shoff) ReadSectionHeader();

}

//****************************************************************************
void ElfFile::ReadProgramHeader()
{
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
  
  // Print implemented sections:
  for(int32 i=0; i<Elf32_Ehdr.e_shnum; i++)
  {
    if(pSectionHeader[i].sh_addr)
    {

      switch(pSectionHeader[i].sh_type)
      {
        case SectionHeaderType::SHT_NULL: S_STRING(chTmp,"inactive ");break;
        case SectionHeaderType::SHT_PROGBITS: S_STRING(chTmp,"defined program");break;
        case SectionHeaderType::SHT_SYMTAB:
        case SectionHeaderType::SHT_DYNSYM: S_STRING(chTmp,"Symbol table ");break;
        case SectionHeaderType::SHT_STRTAB: S_STRING(chTmp,"String table ");break;
        case SectionHeaderType::SHT_RELA:   S_STRING(chTmp,"Relocation table ");break;
        case SectionHeaderType::SHT_HASH: S_STRING(chTmp,"Hash table ");break;
        case SectionHeaderType::SHT_DYNAMIC: S_STRING(chTmp,"Dynamic section ");break;
        case SectionHeaderType::SHT_NOTE: S_STRING(chTmp,"Note section ");break;
        case SectionHeaderType::SHT_NOBITS: S_STRING(chTmp,"Allocate space ");break;
        case SectionHeaderType::SHT_REL: S_STRING(chTmp,"Relocation entries ");break;
        case SectionHeaderType::SHT_SHLIB: S_STRING(chTmp,"Reserved ");break;
        case SectionHeaderType::SHT_HIUSER:S_STRING(chTmp,"user "); break;

        default: S_STRING(chTmp,"processor specific ");
      }

      PUT_STRING("{elf} Sec %i: 0x%08x..0x%08x %s (%s)\n",
        i,
        pSectionHeader[i].sh_addr,
        pSectionHeader[i].sh_addr+pSectionHeader[i].sh_size,
        pStr[pSectionHeader[i].sh_name],
        chTmp
      );
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


