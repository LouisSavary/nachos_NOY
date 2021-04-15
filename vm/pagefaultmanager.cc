/*! \file pagefaultmanager.cc
Routines for the page fault managerPage Fault Manager
*/
//
//  Copyright (c) 1999-2000 INSA de Rennes.
//  All rights reserved.  
//  See copyright_insa.h for copyright notice and limitation 
//  of liability and disclaimer of warranty provisions.
//

#include "kernel/thread.h"
#include "vm/swapManager.h"
#include "vm/physMem.h"
#include "vm/pagefaultmanager.h"

PageFaultManager::PageFaultManager() {
}

// PageFaultManager::~PageFaultManager()
/*! Nothing for now
*/
PageFaultManager::~PageFaultManager() {
}

// ExceptionType PageFault(uint32_t virtualPage)
/*! 	
//	This method is called by the Memory Management Unit when there is a 
//      page fault. This method loads the page from :
//      - read-only sections (text,rodata) $\Rightarrow$ executive
//        file
//      - read/write sections (data,...) $\Rightarrow$ executive
//        file (1st time only), or swap file
//      - anonymous mappings (stack/bss) $\Rightarrow$ new
//        page from the MemoryManager (1st time only), or swap file
//
//	\param virtualPage the virtual page subject to the page fault
//	  (supposed to be between 0 and the
//        size of the address space, and supposed to correspond to a
//        page mapped to something [code/data/bss/...])
//	\return the exception (generally the NO_EXCEPTION constant)
*/  
ExceptionType PageFaultManager::PageFault(uint32_t virtualPage) 
{
#ifndef ETUDIANTS_TP
  printf("**** Warning: page fault manager is not implemented yet\n");
    exit(-1);
    return ((ExceptionType)0);
//#else
  //gestion bits IO et lock (peut-etre plus tard)


  AddrSpace * addrspace = g_current_thread->GetProcessOwner()->addrspace;
  TranslationTable *ttable = addrspace->translationTable;
  //donner une page physique
  g_physical_mem_manager->AddPhysicalToVirtualMapping(addrspace, virtualPage);
  
  
  if (ttable->getBitSwap(virtualPage) == 1) {
    //ne fonctionne pas car pas d'horloge
      //while(ttable->getAddrDisk(virtualPage) == -1);
    
    g_swap_manager->GetPageSwap(
        ttable->getAddrDisk(virtualPage), 
        (char *)&(g_machine->mainMemory[ttable->getPhysicalPage(virtualPage)*g_cfg->PageSize])
    );

  } else {
    if (addrspace->translationTable->getAddrDisk(virtualPage) != -1) {
      // The section has an image in the executable file
      // Read it from the disk
      int addrdisk = ttable->getAddrDisk(virtualPage);

      // page temporaire pour synchronisation
      char buf [g_cfg->PageSize];
      g_current_thread->GetProcessOwner()->exec_file->ReadAt(
        buf,
        g_cfg->PageSize, 
        addrdisk);

      memcpy(
        (char *)&(g_machine->mainMemory[ttable->getPhysicalPage(virtualPage)*g_cfg->PageSize]),
        buf,
        g_cfg->PageSize);
    }
    else {
      // The section does not have an image in the executable
      // Fill it with zeroes
      memset(&(g_machine->mainMemory[addrspace->translationTable->getPhysicalPage(virtualPage)*g_cfg->PageSize]),
      0, g_cfg->PageSize);
    }
  }
  ttable->setBitValid(virtualPage);
  return NO_EXCEPTION;
  


#endif
}




