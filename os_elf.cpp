#include <elf.h>
#include <SD.h>

#include "os_elf.h"
#include "psram_alloc.h"

void elf_get_strtab(File &app, int e_shoff, int e_shstrndx, char *strtab)
{
  Elf32_Shdr shdr;
  app.seek(e_shoff + (sizeof(shdr) * e_shstrndx));
  app.read((uint8_t *)&shdr,sizeof(shdr));
  app.seek(shdr.sh_offset);
  app.read((uint8_t *)strtab,shdr.sh_size);
}

uint32_t elf_place_in_ram(File &app, int offset, int filesize, int ramsize)
{
  uint8_t *mem = (uint8_t *)psram_get_blocks((ramsize+PSRAM_BLOCKSIZE-1)/PSRAM_BLOCKSIZE);
  app.seek(offset);
  app.read(mem,filesize);
  return (uint32_t)mem;
}

/*uint32_t elf_load_sections(File &app, int phoff, int phnum, uint32_t entry)
{
  int i;
  uint32_t *reloc_table;

  uint32_t codeaddr;
  uint32_t codevaddr;
  uint32_t codepaddr;
  uint32_t codeend;
  uint32_t code_offset;

  uint32_t dataaddr;
  uint32_t datavaddr;
  uint32_t dataend;
  uint32_t data_offset;

  int num_pointers;
  int entry_offset;
  Elf32_Phdr phdr;
  if(phnum != 2)
  {
    Serial.printf("loader assumptions false :(\n");
    return false;
  }
  app.seek(phoff);
  app.read((uint8_t *)&phdr,sizeof(phdr));
  num_pointers = (entry-phdr.p_vaddr)/sizeof(uint32_t);
  phoff += sizeof(phdr); //this is wrong but I like to assume things
  codeaddr = elf_place_in_ram(app,phdr.p_offset,phdr.p_filesz,phdr.p_memsz);
  codepaddr = codeaddr + 0x6000000;
  codevaddr = phdr.p_vaddr;
  codeend = phdr.p_memsz + codevaddr;
  code_offset = codevaddr - codepaddr;
  entry_offset = entry - phdr.p_vaddr;
  entry = codeaddr + entry_offset + 0x6000000;

  app.seek(phoff);
  app.read((uint8_t *)&phdr,sizeof(phdr));
  dataaddr = elf_place_in_ram(app,phdr.p_offset,phdr.p_filesz,phdr.p_memsz);
  datavaddr = phdr.p_vaddr;
  dataend = phdr.p_memsz + datavaddr;
  data_offset = datavaddr - dataaddr;
  reloc_table = (uint32_t *)codeaddr;
  Serial.printf("datavaddr %08X dataend %08X\n", datavaddr, dataend);
  for(i=0;i<num_pointers;i++)
  {
    if(reloc_table[i] >= codevaddr && reloc_table[i] <= codeend)
    {
      Serial.printf("reloc %08X",reloc_table[i]);
      reloc_table[i] -= code_offset;
      Serial.printf(" to %08X\n",reloc_table[i]);
    }
    else if(reloc_table[i] >= datavaddr && reloc_table[i] <= dataend)
    {
      Serial.printf("reloc %08X",reloc_table[i]);
      reloc_table[i] -= data_offset;
      Serial.printf(" to %08X\n",reloc_table[i]);
    }
  }
  Serial.printf("new entry %08X\n",entry);
  reloc_table = (uint32_t *)(entry-0x6000000);
  for(i=0;i<16;i++)
  {
    Serial.printf("%08X ",reloc_table[i]);
  }
  Serial.printf("\n");
  return entry;
}*/

uint32_t elf_load_sections(File &app, uint32_t e_entry, int e_shoff, int e_shnum, char *strtab)
{
  int i;
  Elf32_Shdr shdr;
  int offset;

  uint32_t *global_table;
  int global_table_size;

  uint32_t text = 0;
  uint32_t text_len;
  uint32_t text_vaddr;

  uint32_t rodata = 0;
  uint32_t rodata_len;
  uint32_t rodata_vaddr;

  uint32_t bss = 0;
  uint32_t bss_len;
  uint32_t bss_vaddr;

  offset = e_shoff;
  for(i=0;i<e_shnum;i++)
  {
    app.seek(offset);
    offset += sizeof(shdr);
    app.read((uint8_t *)&shdr,sizeof(shdr));
    if(!strcmp(&strtab[shdr.sh_name],".text"))
    {
      text = elf_place_in_ram(app,shdr.sh_offset,shdr.sh_size,shdr.sh_size);
      text_len = shdr.sh_size;
      text_vaddr = shdr.sh_addr;
      //relocate entry to new address
    }
    if(!strcmp(&strtab[shdr.sh_name],".rodata"))
    {
      rodata = elf_place_in_ram(app,shdr.sh_offset,shdr.sh_size,shdr.sh_size);
      rodata_len = shdr.sh_size;
      rodata_vaddr = shdr.sh_addr;
    }
    if(!strcmp(&strtab[shdr.sh_name],".bss"))
    {
      bss = elf_place_in_ram(app,shdr.sh_offset,shdr.sh_size,shdr.sh_size);
      bss_len = shdr.sh_size;
      bss_vaddr = shdr.sh_addr;
    }
    if(!strcmp(&strtab[shdr.sh_name],".xt.lit"))
    {
      uint32_t got_meta[4];
      app.seek(shdr.sh_offset);
      app.read((uint8_t *)got_meta,16);
      Serial.printf(".XT.LIT\n");
      global_table_size = got_meta[1];
      global_table_size += got_meta[3];
      global_table_size /= 4;
      Serial.printf("global table size %d\n",global_table_size);
    }
  }
  Serial.printf("Text %08X => %08X\n",text_vaddr, text);
  Serial.printf("Rodata %08X\n",rodata);
  Serial.printf("Bss %08X\n",bss);
  global_table = (uint32_t *)text;
  for(i=0;i<global_table_size;i++)
  {
    if(global_table[i] >= text_vaddr && global_table[i] <= text_vaddr + text_len)
    {
      Serial.printf("function pointer %08X\n",global_table[i]);
      global_table[i] -= text_vaddr;
      global_table[i] += text;
      global_table[i] += 0x6000000; //hardware thing
    }
    if(global_table[i] >= rodata_vaddr && global_table[i] <= rodata_vaddr + rodata_len)
    {
      Serial.printf("rodata pointer %08X\n",global_table[i]);
      global_table[i] -= rodata_vaddr;
      global_table[i] += rodata;
    }
    if(global_table[i] >= bss_vaddr && global_table[i] <= bss_vaddr + bss_len)
    {
      Serial.printf("bss pointer %08X\n",global_table[i]);
      global_table[i] -= bss_vaddr;
      global_table[i] += bss;
    }
    Serial.printf("global pointer %08X\n",global_table[i]);
  }
  e_entry -= text_vaddr;
  e_entry += text;
  e_entry += 0x6000000; //hardware thing
  Serial.printf("new entry %08X\n",e_entry);
  return e_entry;
}

uint32_t loadElf(fs::FS &fs, const char *filename)
{
  char strtab[256];
  uint32_t entry;
  Elf32_Ehdr e32_hdr;
  File app = fs.open(filename);

  if(!app)
  {
    Serial.printf("App not found\n");
    return 0;
  }
  if(app.read((uint8_t *)&e32_hdr,sizeof(e32_hdr))!=sizeof(e32_hdr))
  {
    Serial.printf("App not valid\n");
    goto FAILURE;
  }
  elf_get_strtab(app,e32_hdr.e_shoff,e32_hdr.e_shstrndx,strtab);
  entry = elf_load_sections(app,e32_hdr.e_entry,e32_hdr.e_shoff,e32_hdr.e_shnum,strtab);
  //entry = elf_load_sections(app,e32_hdr.e_phoff,e32_hdr.e_phnum,e32_hdr.e_entry);
  app.close();
  return entry;
FAILURE:
  app.close();
  return 0;
}