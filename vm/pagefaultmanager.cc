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
// #ifndef ETUDIANTS_TP
//   printf("**** Warning: page fault manager is not implemented yet\n");
//     exit(-1);
//     return ((ExceptionType)0);
// //#else
  //gestion bits IO (peut-etre plus tard)
  AddrSpace * addrspace = g_current_thread->GetProcessOwner()->addrspace;
  TranslationTable *ttable = addrspace->translationTable;
  
  while (ttable->getBitIo(virtualPage)) g_current_thread->Yield();
  if (ttable->getBitValid(virtualPage)) return NO_EXCEPTION;


  ttable->setBitIo(virtualPage);
  char buf [g_cfg->PageSize];

  if (ttable->getBitSwap(virtualPage) == 1) {
    while(ttable->getAddrDisk(virtualPage) == -1) 
      g_current_thread->Yield();
    
    g_swap_manager->GetPageSwap(
        ttable->getAddrDisk(virtualPage), 
        buf
    );

  } else if (addrspace->translationTable->getAddrDisk(virtualPage) != -1) {
    //pas anonyme et pas dans le swap
    // The section has an image in the executable file
    // Read it from the disk

    g_current_thread->GetProcessOwner()->exec_file->ReadAt(
      buf,
      g_cfg->PageSize, 
      ttable->getAddrDisk(virtualPage));

  } else {
    //page anonyme pas encore dans le swap
    memset(buf, 0, g_cfg->PageSize);
  }


  //donner une page physique et l'initialiser
  g_physical_mem_manager->AddPhysicalToVirtualMapping(addrspace, virtualPage);
  
  // for (int i = 0; i < g_cfg->PageSize; i++)
  //   g_machine->mmu->WriteMem(
  //     virtualPage +i,
  //     1,
  //     buf[i]
  //   );
  
  memcpy(
    &(g_machine->mainMemory[ttable->getPhysicalPage(virtualPage)*g_cfg->PageSize]),
    buf,
    g_cfg->PageSize);

  ttable->setBitValid(virtualPage);
  ttable->clearBitM(virtualPage);
  ttable->clearBitIo(virtualPage);
  
  g_physical_mem_manager->UnlockPage(ttable->getPhysicalPage(virtualPage));
  
  return NO_EXCEPTION;
  
// #endif
}




