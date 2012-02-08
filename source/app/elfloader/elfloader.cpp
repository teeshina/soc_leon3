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
  memcpy((char*)&Elf32_Ehdr,arrElf,sizeof(Elf32_EhdrType));
  char chMagic[5] = {0x7f,'E','L','F',0};
  bool bMagicPass = true;
  for(int32 i=0; i<4; i++)
  {
    if(Elf32_Ehdr.e_ident[i]!=chMagic[i]) bMagicPass=false;
  }
  if(bMagicPass) PUT_STRING("{elf} ELF file has been opened\n");
  else           PUT_STRING("{elf} Err: \"%s\" isn't ELF file\n",ELF_FILE);

  switch(Elf32_Ehdr.e_ident[Elf32_EhdrType::EI_CLASS])
  {
    case Elf32_EhdrType::ELFCLASS32:  PUT_STRING("{elf} 32-bits objects\n"); break;
    case Elf32_EhdrType::ELFCLASS64:  PUT_STRING("{elf} 64-bits objects\n"); break;
    default:                          PUT_STRING("{elf} Err: Invalid class\n"); break;
  }

  switch(Elf32_Ehdr.e_ident[Elf32_EhdrType::EI_DATA])
  {
    case Elf32_EhdrType::ELFDATA2LSB: PUT_STRING("{elf} Least significant byte read last\n"); break;
    case Elf32_EhdrType::ELFDATA2MSB: PUT_STRING("{elf} Most significant byte read last\n"); break;
    default:                          PUT_STRING("{elf} Err: Invalid Data Encoding\n"); break;
  }

  switch(Elf32_Ehdr.e_ident[Elf32_EhdrType::EI_VERSION])
  {
    case Elf32_EhdrType::EV_CURRENT: PUT_STRING("{elf} Current version\n"); break;
    default:                         PUT_STRING("{elf} Err: Invalid version\n"); break;
  }

  SwapBytes(Elf32_Ehdr.e_type);
  switch(Elf32_Ehdr.e_type)
  {
    case Elf32_EhdrType::ET_NONE: PUT_STRING("{elf} No file type\n"); break;
    case Elf32_EhdrType::ET_REL:  PUT_STRING("{elf} Relocable file\n"); break;
    case Elf32_EhdrType::ET_EXEC: PUT_STRING("{elf} Executable file\n"); break;
    case Elf32_EhdrType::ET_DYN:  PUT_STRING("{elf} Shared object file\n"); break;
    case Elf32_EhdrType::ET_CORE: PUT_STRING("{elf} Core file\n"); break;
    default:                      PUT_STRING("{elf} Processor-specific file type\n"); break;
  }

  SwapBytes(Elf32_Ehdr.e_machine);
  switch(Elf32_Ehdr.e_machine)
  {
    case Elf32_EhdrType::EM_NONE:  PUT_STRING("{elf} No machine type\n"); break;
    case Elf32_EhdrType::EM_M32:   PUT_STRING("{elf} AT&T WE 32100\n"); break;
    case Elf32_EhdrType::EM_SPARC: PUT_STRING("{elf} SPARC\n"); break;
    case Elf32_EhdrType::EM_386:   PUT_STRING("{elf} Intel 80386\n"); break;
    case Elf32_EhdrType::EM_68K:   PUT_STRING("{elf} Motorola 68000\n"); break;
    case Elf32_EhdrType::EM_88K:   PUT_STRING("{elf} Motorola 88000\n"); break;
    case Elf32_EhdrType::EM_860:   PUT_STRING("{elf} Intel 80860\n"); break;
    default:                       PUT_STRING("{elf} Err: Machine unknown\n"); break;
  }

  SwapBytes(Elf32_Ehdr.e_entry);
  PUT_STRING("{elf} Entry point = 0x%08x\n",Elf32_Ehdr.e_entry);
  
  SwapBytes(Elf32_Ehdr.e_phoff);
  SwapBytes(Elf32_Ehdr.e_shoff);
  SwapBytes(Elf32_Ehdr.e_flags);
  SwapBytes(Elf32_Ehdr.e_ehsize);
  SwapBytes(Elf32_Ehdr.e_phentsize);
  SwapBytes(Elf32_Ehdr.e_phnum);
  SwapBytes(Elf32_Ehdr.e_shentsize);
  SwapBytes(Elf32_Ehdr.e_shnum);
  SwapBytes(Elf32_Ehdr.e_shstrndx);

}

//****************************************************************************
void ElfFile::SwapBytes(Elf32_Half& v)
{
  Elf32_Half tmp=v;
  switch(Elf32_Ehdr.e_ident[Elf32_EhdrType::EI_DATA])
  {
    case Elf32_EhdrType::ELFDATA2LSB: break;  //Intel native format
    case Elf32_EhdrType::ELFDATA2MSB: v = ((tmp>>8)&0xff)|((tmp&0xFF)<<8); break;
    default:;
  }
}

//****************************************************************************
void ElfFile::SwapBytes(uint32& v)
{
  uint32 tmp=v;
  switch(Elf32_Ehdr.e_ident[Elf32_EhdrType::EI_DATA])
  {
    case Elf32_EhdrType::ELFDATA2LSB: break;  //Intel native format
    case Elf32_EhdrType::ELFDATA2MSB: v = ((tmp>>24)&0xff)|((tmp>>8)&0xff00)|((tmp<<8)&0xff0000)|((tmp&0xFF)<<24); break;
    default:;
  }
}

//****************************************************************************
void ElfFile::PUT_STRING(const char *_format, ... )
{
  va_list arg;
  va_start( arg, _format );
  
  vsprintf_s( chPutString, PUT_STRING_SIZE, _format, arg );
  va_end( arg );
  
  printf_s("%s", chPutString);
}


