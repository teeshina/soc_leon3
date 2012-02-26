//****************************************************************************
// Author:      Khabarov Sergey
// License:     GNU2
// Contact:     sergey.khabarov@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#pragma once

#include <fstream>
#include "stdtypes.h"
#include "sparcv8.h"

class ElfFile
{
  friend class dbg;
  private:
    //#define EI_NIDENT 16
    typedef unsigned int   Elf32_Addr;
    typedef unsigned short Elf32_Half;
    typedef unsigned int   Elf32_Off;
    typedef signed   int   Elf32_Sword;
    typedef unsigned int   Elf32_Word;
  private:
    struct ElfHeaderType
    {
      enum E_EI
      {
        EI_MAG0,
        EI_MAG1,
        EI_MAG2,
        EI_MAG3,
        EI_CLASS,
        EI_DATA,
        EI_VERSION,
        EI_PAD,
        EI_NIDENT=16
      };
      static const uint8 ELFCLASSNONE = 0;
      static const uint8 ELFCLASS32   = 1;
      static const uint8 ELFCLASS64   = 2;

      static const uint8 ELFDATANONE  = 0;
      static const uint8 ELFDATA2LSB  = 1;
      static const uint8 ELFDATA2MSB  = 2;

      static const uint8 EV_NONE      = 0;  // Invalid version
      static const uint8 EV_CURRENT   = 1;  // Current version
      //etype values:
      static const Elf32_Half ET_NONE      = 0; // no file type
      static const Elf32_Half ET_REL       = 1; // relocatable file
      static const Elf32_Half ET_EXEC      = 2; // executable file
      static const Elf32_Half ET_DYN       = 3; // shared object file
      static const Elf32_Half ET_CORE      = 4; // core file
      static const Elf32_Half ET_LOPROC    = 0xff00; // Processor-specific
      static const Elf32_Half ET_HIPROC    = 0xffff; // Processor-specific
      //emachine values:
      static const Elf32_Half EM_NONE      = 0;  // No machine
      static const Elf32_Half EM_M32       = 1;  // AT&T WE 32100
      static const Elf32_Half EM_SPARC     = 2;  // SPARC
      static const Elf32_Half EM_386       = 3;  // Intel 386
      static const Elf32_Half EM_68K       = 4;  // Motorola 68000
      static const Elf32_Half EM_88K       = 5;  // Motorola 88000
      static const Elf32_Half EM_860       = 7;  // Intel 80860
      static const Elf32_Half EM_MIPS      = 8;  // MIPS RS3000


      unsigned char e_ident[EI_NIDENT];
      Elf32_Half e_type;
      Elf32_Half e_machine;
      Elf32_Word e_version;
      Elf32_Addr e_entry;
      Elf32_Off  e_phoff;
      Elf32_Off  e_shoff;
      Elf32_Word e_flags;
      Elf32_Half e_ehsize;
      Elf32_Half e_phentsize;
      Elf32_Half e_phnum;
      Elf32_Half e_shentsize;
      Elf32_Half e_shnum;
      Elf32_Half e_shstrndx;
    };
    ElfHeaderType Elf32_Ehdr;
    
    struct SectionHeaderType
    {
      Elf32_Word    sh_name;//Index in a header section table (gives name of section)
      Elf32_Word    sh_type;
      Elf32_Word    sh_flags;
      Elf32_Addr    sh_addr;
      Elf32_Off     sh_offset;
      Elf32_Word    sh_size;
      Elf32_Word    sh_link;
      Elf32_Word    sh_info;
      Elf32_Word    sh_addralign;
      Elf32_Word    sh_entsize;
      
      //sh_type:
      static const Elf32_Word SHT_NULL      = 0;
      static const Elf32_Word SHT_PROGBITS  = 1;
      static const Elf32_Word SHT_SYMTAB    = 2;
      static const Elf32_Word SHT_STRTAB    = 3;
      static const Elf32_Word SHT_RELA      = 4;
      static const Elf32_Word SHT_HASH      = 5;
      static const Elf32_Word SHT_DYNAMIC   = 6;
      static const Elf32_Word SHT_NOTE      = 7;
      static const Elf32_Word SHT_NOBITS    = 8;
      static const Elf32_Word SHT_REL       = 9;
      static const Elf32_Word SHT_SHLIB     = 10;
      static const Elf32_Word SHT_DYNSYM    = 11;
      static const Elf32_Word SHT_LOPROC    = 0x70000000;
      static const Elf32_Word SHT_HIPROC    = 0x7fffffff;
      static const Elf32_Word SHT_HIUSER    = 0xffffffff;
      //sh_flags:
      static const Elf32_Word SHF_WRITE     = 0x1;
      static const Elf32_Word SHF_ALLOC     = 0x2;
      static const Elf32_Word SHF_EXECINSTR = 0x4;
      static const Elf32_Word SHF_MASKPROC  = 0xf0000000;
      
    };
    SectionHeaderType *pSectionHeader;

    struct SymbolTableType
    {
      Elf32_Word    st_name;
      Elf32_Addr    st_value;
      Elf32_Word    st_size;
      unsigned char st_info;
      unsigned char st_other;
      Elf32_Half    st_shndx;
    };

    struct ProgramHeaderType
    {
      Elf32_Word    p_type;
      Elf32_Off     p_offset;
      Elf32_Addr    p_vaddr;
      Elf32_Addr    p_paddr;
      Elf32_Word    p_filesz;
      Elf32_Word    p_memsz;
      Elf32_Word    p_flags;
      Elf32_Word    p_align;

      //p_type:
      static const Elf32_Word PT_NULL     = 0;
      static const Elf32_Word PT_LOAD     = 1;
      static const Elf32_Word PT_DYNAMIC  = 2;
      static const Elf32_Word PT_INTERP   = 3;
      static const Elf32_Word PT_NOTE     = 4;
      static const Elf32_Word PT_SHLIB    = 5;
      static const Elf32_Word PT_PHDR     = 6;
      static const Elf32_Word PT_LOPROC   = 0x70000000;
      static const Elf32_Word PT_HIPROC   = 0x7fffffff;
    };
    ProgramHeaderType *pProgramHeader;

  private:
    static const int32 FILE_NAME_STRING_MAX = 1024;
    char chElfFile[FILE_NAME_STRING_MAX];
    char chMapFile[FILE_NAME_STRING_MAX];
    char chAsmFile[FILE_NAME_STRING_MAX];
    bool bFileOpened;
    int32 iElfSize;
    uint8 *arrElf;
    std::ofstream *posMapFile;
    std::ofstream *posAsmFile;
    
    static const int32 STRINGS_IN_SECTION_MAX = 1<<16;
    uint32 iTotalStrings;
    uint8  *pStr[STRINGS_IN_SECTION_MAX];
    
    static const int32 ELF_IMAGE_MAXSIZE = 1<<16; // words
    uint32 image[ELF_IMAGE_MAXSIZE];
    SparcV8 clSparcV8;

    int32 iImageBytes;

  public:
    ElfFile(char *pchElfFile);
    ~ElfFile();

    void Load();

    int32 ReadElfHeader();  // 0=elf file OK; otherwise error
    void ReadProgramHeader();
    void ReadSectionHeader();
    void RunDisassembler();
    
    void WriteMemoryMap(SectionHeaderType *);
    
    void SwapBytes(Elf32_Half&);
    void SwapBytes(uint32&);
  
  public:
    static const int32 PUT_STRING_SIZE = 1024;
    char chPutString[PUT_STRING_SIZE];
    int32 PUT_STRING(const char *_format, ... );
    int32 S_STRING(char *s,const char *_format, ... );

    char chTmp[PUT_STRING_SIZE];

};


