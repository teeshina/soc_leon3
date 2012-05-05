//****************************************************************************
// Property:    GNSS Sensor Limited
// Author:      Khabarov Sergey
// License:     GPL
// Contact:     sergey.khabarov@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#include "headers.h"

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

    strcpy_s(chAsmFile, FILE_NAME_STRING_MAX,pchElfFile);
    strcat_s(chAsmFile, FILE_NAME_STRING_MAX, ".asm");
    posAsmFile = new ofstream(chAsmFile,ios::out);
  }
  free(pisElf);

  memset(&image,0,sizeof(SrcImage));

  pStrSections = 0;
  pStrSymbols  = 0;
}

ElfFile::~ElfFile()
{
  if(bFileOpened)
  {
    free(arrElf);

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
    PUT_STRING("{elf} YOU SHOULD COMPILE SOURCE FILES AND SET PATH TO ELF-FILE\n");
    PUT_STRING("{elf} Error: File not found. Use \"settings.txt\" file to correct path.\n");
    return;
  }

  if (ReadElfHeader())
  {
    PUT_STRING("{elf} Error: \"%s\" file isn't correct\n", chElfFile);
    return;
  }

  if(Elf32_Ehdr.e_shoff) ReadSectionHeader();
  if(Elf32_Ehdr.e_phoff) ReadProgramHeader();

  clSparcV8.Disassemler(&image, posAsmFile);
  PUT_STRING("{elf} \"%s\" generated\n",chAsmFile);
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


  PUT_STRING("{elf} \"%s\" has been opened\n",chElfFile);

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
  iTotalSymbols = 0;

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
      if(pStrSections==0) pStrSections = &arrElf[pSectionHeader[i].sh_offset];
      else                pStrSymbols  = &arrElf[pSectionHeader[i].sh_offset];
    }

  }

  // Read symbols:
  for(int32 i=0; i<Elf32_Ehdr.e_shnum; i++)
  {
    if( (pSectionHeader[i].sh_type==SectionHeaderType::SHT_SYMTAB)
     || (pSectionHeader[i].sh_type==SectionHeaderType::SHT_DYNSYM) )
    {
      uint32 uiSmbLength=0;
      while(uiSmbLength<pSectionHeader[i].sh_size)
      {
        pSymbols[iTotalSymbols] = ((SymbolTableType*)&arrElf[pSectionHeader[i].sh_offset+uiSmbLength]);
        SwapBytes(pSymbols[iTotalSymbols]->st_name);
        SwapBytes(pSymbols[iTotalSymbols]->st_value);
        SwapBytes(pSymbols[iTotalSymbols]->st_size);
        SwapBytes(pSymbols[iTotalSymbols]->st_shndx);

        if(pSectionHeader[i].sh_entsize)          uiSmbLength += pSectionHeader[i].sh_entsize;    // section with elements of fixed size
        else if(pSymbols[iTotalSymbols]->st_size) uiSmbLength += pSymbols[iTotalSymbols]->st_size;
        else                                      uiSmbLength += sizeof(SymbolTableType);
        iTotalSymbols++;
      }
    }
  }
  
  // Select non-zero sections and put it into virtual image:
  for(int32 i=0; i<Elf32_Ehdr.e_shnum; i++)
  {
    if(pSectionHeader[i].sh_addr)
      CreateImage(&pSectionHeader[i]);
  }
  CheckImageIntegrity();


  // Attach symbols data to the image:
  for(uint32 i=0; i<iTotalSymbols; i++)
  {
    AttachSymbol(pSymbols[i]);
  }


  // Copy image into internal RAM of the SOC:
  for(int32 i=0; i<image.iSizeWords; i++)
  {
    if(image.arr[i].pSectionName)
      PUT_STRING("{elf} Loading section \"%s\" at 0x%08x\n",image.arr[i].pSectionName, image.arr[i].adr);

    LibBackDoorLoadRAM(image.arr[i].adr, image.arr[i].val);
  }

  // WARNING!! DEBUG purpose only
  CreateVhdlRomImage();
  
#if 0
  std::ofstream osMemInit("e:\\meminit0.txt");
  char chX[32];
  int32 i;
  
  for (int32 byte=0; byte<4; byte++)
  {
    sprintf_s(chX,"constant ram%02i : ram_type := (\n", byte);
    osMemInit << chX;
    i=0;
    while(i<image.iSizeWords)
    {
      for (int32 k=0; k<8; k++)
      {
        sprintf_s(chX,"  X\"%02X\",", (image.arr[i].val>>(8*byte))&0xFF);
        osMemInit << chX;
        i++;
      }
      osMemInit << "\n";
    }
    osMemInit << "  others => X\"00\" );\n\n\n";
  }
#endif
}

//****************************************************************************
void ElfFile::CreateImage(SectionHeaderType *p)
{
  // source code or data:
  uint32 word;
  int32 MsbOrder[4]={0,1,2,3};
  int32 LsbOrder[4]={3,2,1,0};
  int32 *byte;
  // select byte order:
  if(Elf32_Ehdr.e_ident[ElfHeaderType::EI_DATA]==ElfHeaderType::ELFDATA2MSB) byte = MsbOrder;
  else                                                                       byte = LsbOrder;
  
  image.entry = Elf32_Ehdr.e_entry;

  // print Program defined section:
  if( (p->sh_type==SectionHeaderType::SHT_PROGBITS) // intructions
   || (p->sh_type==SectionHeaderType::SHT_NOBITS) ) // data
  {
    image.arr[image.iSizeWords].pSectionName = (pStrSections + p->sh_name);
    image.iSizeWords = (p->sh_addr-image.entry)/sizeof(uint32);


    for(uint32 n=0; n<p->sh_size;n+=4)
    {
      if(p->sh_type==SectionHeaderType::SHT_PROGBITS)
      {
        word = uint32(arrElf[p->sh_offset+n+byte[0]])<<24;
        word |= uint32(arrElf[p->sh_offset+n+byte[1]])<<16;
        word |= uint32(arrElf[p->sh_offset+n+byte[2]])<<8;
        word |= uint32(arrElf[p->sh_offset+n+byte[3]])<<0;
      }else if(p->sh_type==SectionHeaderType::SHT_NOBITS)
        word = 0;

      if(image.iSizeWords<ELF_IMAGE_MAXSIZE) 
      {
        image.arr[image.iSizeWords].adr = p->sh_addr+n;
        image.arr[image.iSizeWords].val = word;
        image.arr[image.iSizeWords].bInit = true;
        image.iSizeWords++;
      }else
        PUT_STRING("{elf} Error: Internal Image maximum address is overrun\n");

    }
  }
}

//****************************************************************************
void ElfFile::AttachSymbol(SymbolTableType *pS)
{
  int32 ind = 0;
  ind =(pS->st_value - image.entry)>>2;
  if((ind >= ELF_IMAGE_MAXSIZE)||(ind<0))
    return;


  uint8 *px1 = pStrSections + pS->st_name;
  uint8 *px2 = pStrSymbols + pS->st_name;
  switch(ELF32_ST_TYPE(pS->st_info))
  {
    case SymbolTableType::STT_OBJECT:
      image.arr[ind].pDataName = pStrSymbols + pS->st_name;
    break;
    case SymbolTableType::STT_FUNC:
      image.arr[ind].pFuncName = pStrSymbols + pS->st_name;
    break;
    case SymbolTableType::STT_FILE:
      image.arr[ind].pFileName = pStrSymbols + pS->st_name;
    break;
    default:;
  }
}

//****************************************************************************
void ElfFile::CheckImageIntegrity()
{
  for(int32 i=0; i<image.iSizeWords; i++)
  {
    if(image.arr[i].adr != ((i<<2)+image.entry))
    {
      if(!image.arr[i].bInit)
      {
        image.arr[i].bInit = true;
        image.arr[i].adr   = ((uint32(i)<<2)+image.entry);
      }else
        PUT_STRING("{elf} Wrong Addres: 0x%08x\n",image.arr[i].adr);

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

//****************************************************************************
void ElfFile::CreateVhdlRomImage()
{
  char chRomFile[1024];
  strcpy_s(chRomFile, FILE_NAME_STRING_MAX,chElfFile);
  chRomFile[strlen(chRomFile)-4]='\0';
  strcat_s(chRomFile,FILE_NAME_STRING_MAX,".vhd");

  std::ofstream *posRomVhdl = new std::ofstream(chRomFile,std::ios::out);

  sprintf_s(chRomFile,"%s","\
----------------------------------------------------------------------------\n\
--  INFORMATION:  http://www.GNSS-sensor.com\n\
--  PROPERTY:     GNSS Sensor Ltd\n\
--  E-MAIL:       alex.kosin@gnss-sensor.com\n\
--  DESCRIPTION:  This file contains copy of firmware image\n\
------------------------------------------------------------------------------\n\
--  WARNING:      \n\
------------------------------------------------------------------------------\n\
library ieee;\n\
use ieee.std_logic_1164.all;\n\
library grlib;\n\
use grlib.amba.all;\n\
use grlib.stdlib.all;\n\
--use grlib.devices.all;\n\n");

  *posRomVhdl << chRomFile;



sprintf_s(chRomFile,"%s","\
entity FwRomImage is\n\
  generic (\n\
    hindex  : integer := 0;\n\
    haddr   : integer := 0;\n\
    hmask   : integer := 16#fff#);\n\
  port (\n\
    rst     : in  std_ulogic;\n\
    clk     : in  std_ulogic;\n\
    ahbsi   : in  ahb_slv_in_type;\n\
    ahbso   : out ahb_slv_out_type\n\
  );\n\
end;\n\
\n\
architecture rtl of FwRomImage is\n\n");

  *posRomVhdl << chRomFile;

  sprintf_s(chRomFile,"%s","\
constant VENDOR_GNSSSENSOR     : integer := 16#F1#; -- TODO: move to devices.vhd\n\
constant GNSSSENSOR_ROM_IMAGE  : integer := 16#07B#;\n\
constant REVISION              : integer := 1;\n\
\n\
constant hconfig : ahb_config_type := (\n\
  0 => ahb_device_reg ( VENDOR_GNSSSENSOR, GNSSSENSOR_ROM_IMAGE, 0, REVISION, 0),\n\
  4 => ahb_membar(haddr, '1', '1', hmask), others => zero32);\n\
\n\
signal romdata : std_logic_vector(31 downto 0);\n\
signal addr : std_logic_vector(15 downto 0);\n\
signal hsel, hready : std_ulogic;\n\
\n\
begin\n\n");

  *posRomVhdl << chRomFile;

  sprintf_s(chRomFile,"%s","\
  ahbso.hresp   <= \"00\";\n\
  ahbso.hsplit  <= (others => '0');\n\
  ahbso.hirq    <= (others => '0');\n\
  ahbso.hcache  <= '1';\n\
  ahbso.hconfig <= hconfig;\n\
  ahbso.hindex  <= hindex;\n\
  \n\
  reg : process (clk) begin\n\
    if rising_edge(clk) then addr <= ahbsi.haddr(17 downto 2); end if;\n\
  end process;\n\
  \n\
  ahbso.hrdata  <= romdata;\n\
  ahbso.hready  <= '1';\n\
  \n\
  comb : process (addr)\n\
  begin\n\
    case conv_integer(addr) is\n");

  *posRomVhdl << chRomFile;
  
  for(int32 i=0; i<ELF_IMAGE_MAXSIZE; i++)
  {
    if(image.arr[i].val!=0)
    {
      sprintf_s(chRomFile,"    when 16#%04X# => romdata <= X\"%08X\";\n",i,image.arr[i].val);
      *posRomVhdl << chRomFile;
    }
  }
    sprintf_s(chRomFile,"%s","\
    when others => romdata <= (others => '0');\n\
    end case;\n\
  end process;\n\
end;\n");


  *posRomVhdl << chRomFile;

  posRomVhdl->close();
  free(posRomVhdl);

}


