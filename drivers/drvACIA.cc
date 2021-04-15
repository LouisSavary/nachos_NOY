/* \file drvACIA.cc
   \brief Routines of the ACIA device driver
//
//      The ACIA is an asynchronous device (requests return 
//      immediately, and an interrupt happens later on).  
//      This is a layer on top of the ACIA.
//      Two working modes are to be implemented in assignment 2:
//      a Busy Waiting mode and an Interrupt mode. The Busy Waiting
//      mode implements a synchronous IO whereas IOs are asynchronous
//      IOs are implemented in the Interrupt mode (see the Nachos
//      roadmap for further details).
//
//  Copyright (c) 1999-2000 INSA de Rennes.
//  All rights reserved.  
//  See copyright_insa.h for copyright notice and limitation 
//  of liability and disclaimer of warranty provisions.
//
*/

/* Includes */

#include "kernel/system.h"         // for the ACIA object
#include "kernel/synch.h"
#include "machine/ACIA.h"
#include "drivers/drvACIA.h"

//-------------------------------------------------------------------------
// DriverACIA::DriverACIA()
/*! Constructor.
  Initialize the ACIA driver. 
  In the ACIA Interrupt mode, 
  initialize the reception index and semaphores and allow 
  reception interrupts. 
  In the ACIA Busy Waiting mode, simply initialize the ACIA 
  working mode and create the semaphore.
  */
//-------------------------------------------------------------------------

#ifndef ETUDIANTS_TP
DriverACIA::DriverACIA()
{
  printf("**** Warning: contructor of the ACIA driver not implemented yet\n");
  exit(-1);
}
#endif
#ifdef  ETUDIANTS_TP
DriverACIA::DriverACIA(){
  char name1[] = "send_semaACIA";
  char name2[] = "rece_semaACIA"; //oblige, sinon warning
  send_sema    = new Semaphore(name1, 1);
  receive_sema = new Semaphore(name2, 1);
  ind_send = 0;
  ind_rec  = 0;
}
#endif

//-------------------------------------------------------------------------
// DriverACIA::TtySend(char* buff)
/*! Routine to send a message through the ACIA (Busy Waiting or Interrupt mode)
  */
//-------------------------------------------------------------------------

#ifndef ETUDIANTS_TP
int DriverACIA::TtySend(char* buff)
{ 
  printf("**** Warning: method Tty_Send of the ACIA driver not implemented yet\n");
  exit(-1);
  return 0;
}
#endif
#ifdef  ETUDIANTS_TP
int DriverACIA::TtySend(char* buff){
  send_sema->P();
  
  int nb_char_send = 0;
  
  // filling send_buffer --------------
  int i = -1;
  do {
    i += 1;
    send_buffer[i] = buff[i];
  } while (buff[i]!='\0' && i < BUFFER_SIZE-1);
  send_buffer[i] = '\0';
  // send_buffer filled


  if (g_cfg->ACIA == ACIA_BUSY_WAITING) {
    
    ind_send = -1;

    do{
      while(g_machine->acia->GetOutputStateReg() == FULL);
      g_machine->acia->PutChar(send_buffer[++ind_send]);
    } while (ind_send < i);

    nb_char_send = i+1;

    send_sema->V();

  } else if (g_cfg->ACIA == ACIA_INTERRUPT) {
    ind_send = 0;
    //ecrire le premier caractère
    g_machine->acia->PutChar(send_buffer[ind_send++]);

  }
  return nb_char_send;
}
#endif
//-------------------------------------------------------------------------
// DriverACIA::TtyReceive(char* buff,int length)
/*! Routine to reveive a message through the ACIA 
//  (Busy Waiting and Interrupt mode).
  */
//-------------------------------------------------------------------------

#ifndef ETUDIANTS_TP
int DriverACIA::TtyReceive(char* buff,int lg)
{
   printf("**** Warning: method Tty_Receive of the ACIA driver not implemented yet\n");
  exit(-1);
  return 0;
}
#endif
#ifdef  ETUDIANTS_TP
int DriverACIA::TtyReceive(char* buff,int lg)
{
  receive_sema->P();
  int nb_read = 0;
  if (g_cfg->ACIA == ACIA_BUSY_WAITING) {
    ind_rec = 0;
  char temp;
    do {
      while (g_machine->acia->GetInputStateReg() == EMPTY);
      
      temp = g_machine->acia->GetChar();
      if (nb_read < lg)
        buff[nb_read] = temp;
      
      nb_read ++;
    } while (temp != '\0');

    buff[nb_read-1] = '\0';
    receive_sema->V();

  }

  if (g_cfg->ACIA == ACIA_INTERRUPT) {
    
    for (int i = 0; i < lg && i < ind_rec; i++)
      buff[i] = receive_buffer[i];
    ind_rec = 0;

  }

  return nb_read;
}
#endif

//-------------------------------------------------------------------------
// DriverACIA::InterruptSend()
/*! Emission interrupt handler.
  Used in the ACIA Interrupt mode only. 
  Detects when it's the end of the message (if so, releases the send_sema semaphore), else sends the next character according to index ind_send.
  */
//-------------------------------------------------------------------------

void DriverACIA::InterruptSend()
{
#ifndef ETUDIANTS_TP
  printf("**** Warning: send interrupt handler not implemented yet\n");
  exit(-1);
#endif

#ifdef ETUDIANTS_TP
  
#endif
}

//-------------------------------------------------------------------------
// DriverACIA::Interrupt_receive()
/*! Reception interrupt handler.
  Used in the ACIA Interrupt mode only. Reveices a character through the ACIA. 
  Releases the receive_sema semaphore and disables reception 
  interrupts when the last character of the message is received 
  (character '\0').
  */
//-------------------------------------------------------------------------

void DriverACIA::InterruptReceive()
{
#ifndef ETUDIANTS_TP
  printf("**** Warning: receive interrupt handler not implemented yet\n");
  exit(-1);
#endif

#ifdef ETUDIANTS_TP
  receive_buffer[ind_rec] = g_machine->acia->GetChar();
  if (receive_buffer[ind_rec++] == '\0') {
    //bloquer interrupt
    receive_sema->V();
  }
#endif
}
