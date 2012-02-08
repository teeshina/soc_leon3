//****************************************************************************
// Author:      Khabarov Sergey
// License:     GNU2
// Contact:     sergey.khabarov@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#pragma once

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
    struct Elf32_EhdrType
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
    Elf32_EhdrType Elf32_Ehdr;

  private:
    bool bFileOpened;
    int32 iElfSize;
    char  *arrElf;
  public:
    ElfFile();
    ~ElfFile();

    void Update();

    void ReadElfHeader();
    void SwapBytes(Elf32_Half&);
    void SwapBytes(uint32&);
  
  public:
    static const int32 PUT_STRING_SIZE = 1024;
    char chPutString[PUT_STRING_SIZE];
    void PUT_STRING(const char *_format, ... );
};


