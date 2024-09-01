#include "loader.h"

Elf32_Ehdr *ehdr;
Elf32_Phdr *phdr;
void* virt;
unsigned int memsza;

/*
 * release memory and other cleanups
 */
void loader_cleanup() 
{
	if(ehdr) //free elf header
	{
		free(ehdr);
		ehdr=NULL;	 
	}
	if(phdr) //free program header
	{
		free(phdr);
		phdr=NULL;
	}
	if(virt) //free virtual memory
	{
		munmap(virt, memsza);
		virt = NULL;
	}
}

/*
 * Load and run the ELF executable file
 */
void load_and_run_elf(const char* exe) 
{
    FILE *object_file = fopen(exe, "rb");
    if (object_file == NULL)
	{
		fprintf(stderr, "Unable to open file\n");
		fclose(object_file);
		exit(1);
    }

	ehdr = malloc(sizeof(Elf32_Ehdr));
    fread(ehdr, sizeof(Elf32_Ehdr),1,object_file);

	if(memcmp(ehdr->e_ident,ELFMAG,SELFMAG)!=0)
	{
		fprintf(stderr,"Invalid ELF header!\n");
		loader_cleanup();
		fclose(object_file);
		exit(1);
    }
    
	if (ehdr->e_ident[EI_CLASS]!=ELFCLASS32|| ehdr->e_ident[EI_DATA]!=ELFDATA2LSB)
	{
		fprintf(stderr,"Not 32 bit and little endian\n");
		loader_cleanup();
		fclose(object_file);
		exit(1);
	}

	if (ehdr->e_type!=ET_EXEC)
	{
		fprintf(stderr,"Not Executable file");
		loader_cleanup();
		fclose(object_file);	
		exit(1);
    }
	unsigned short phcount = ehdr->e_phnum;
	unsigned short phsize = ehdr->e_phentsize;
	phdr = malloc(phcount* sizeof(Elf32_Phdr));

    // printf("phoff %x", ehdr->e_phoff);
    fseek(object_file, ehdr->e_phoff, SEEK_SET);
	fread(phdr, phcount * phsize, 1, object_file);

    // printf("ehdr entry is %x\n", ehdr->e_entry);
	for(unsigned short i = 0; i<phcount; i++)
	{
        // printf("type is %d\n", phdr[i].p_type);
		if(phdr[i].p_type==PT_LOAD && phdr[i].p_flags==PF_R+PF_X)
		{
			// printf("found");
			memsza=phdr[i].p_memsz;
			virt = mmap(NULL, phdr[i].p_memsz, PROT_EXEC | PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);
			if (virt == MAP_FAILED)
			{
                fprintf(stderr, "Memory mapping failed\n");
                loader_cleanup();
                exit(1);
            }
            // printf("offset is %x\n", phdr[i].p_offset);
            // printf("size is %x\n", phdr[i].p_memsz);
			fseek(object_file, phdr[i].p_offset, SEEK_SET);
			fread(virt, phdr[i].p_filesz, 1, object_file);
			fclose(object_file);
            // printf("virt = %p\n", virt);

            typedef int (*func_ptr_t)(void); //function pointer type, returns int and typecastes void*

            func_ptr_t func = (func_ptr_t)(virt + (ehdr->e_entry - phdr[i].p_vaddr)); //lhs = function pointer, rhs = void pointer

            int ans = func();
  			printf("User _start return value = %d\n",ans);
			break;
		}
	}
}
// 1. Load entire binary content into the memory from the ELF file.
// 2. Iterate through the PHDR table and find the section of PT_LOAD 
//    type that contains the address of the entrypoint method in fib.c
// 3. Allocate memory of the size "p_memsz" using mmap function 
//    and then copy the segment content
// 4. Navigate to the entrypoint address into the segment loaded in the memory in above step
// 5. Typecast the address to that of function pointer matching "_start" method in fib.c.
// 6. Call the "_start" method and print the value returned from the "_start"

