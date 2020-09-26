// ****************************************************************
// Copyright (C), 2002 MMSoft, All rights reserved
// ****************************************************************

// ****************************************************************
//
// SOURCE FILE: USB.C
//
// MODULE NAME: USB
//
// PURPOSE:     USB related functions.
//
// AUTHOR:      Marek Mikolajewski (MM)
//
// REVIEWED BY:
//
// HISTORY:     Ver   Date       Sign   Description
//
//              001   19-06-2002 MM     Created
//
// ****************************************************************

#include <platform.h>
#include "hal4usb.h"
#include "usb_reg.h"

#ifdef DEBUG
  #undef DEBUG
#endif

extern Device_buffers_t         device_buffers;

//****************************************************************
//
// Prototypes
//
//****************************************************************

VOID  fill_control_fifo                 ( VOID );
VOID  USBN9604_tx_event_handler         ( VOID );
VOID  USBN9604_alt_event_handler        ( VOID );
VOID  USBN9604_rx_event_handler         ( VOID );

// ====================================================================
//                        USB Driver Definitions
//=====================================================================

VOLATILE UINT8          event_table;

endpoint_t              DMA_ep;

endpoint_status_t       tx0_ep_stat;
endpoint_status_t       tx1_ep_stat, tx2_ep_stat;
endpoint_status_t       rx1_ep_stat, rx2_ep_stat;
endpoint_status_t       tx3_ep_stat;

CONST endpoint_status_t     * endpoint_stat[NUM_OF_ENDPOINTS] =
{
  &tx0_ep_stat,
  &tx1_ep_stat,
  &rx1_ep_stat,
  &tx2_ep_stat,
  &rx2_ep_stat,
  &tx3_ep_stat,
  NULL
};

//----------------------------------------------------------------
// Function :   endpoint_status_init
// Notes    :
// History  :
//----------------------------------------------------------------

VOID endpoint_status_init( VOID )
{
  INT16 i;

  for ( i=0;i<NUM_OF_ENDPOINTS;i++ )
  {
    endpoint_stat[i]->stalled = FALSE;
    //
    if ( endpoint_stat[i] != NULL )
    {
      endpoint_stat[i]->FIFO_status = EMPTY;
      if ( i==0 )
        // ----------------
        // control endpoint
        // ----------------
        endpoint_stat[i]->toggle_bit = 0x01;
      else
        // ----------------------
        // tx & rx data endpoints
        // ----------------------
        endpoint_stat[i]->toggle_bit = 0x00;
    }
  }
}

CONST INT16 usbn9604_tx_endpoint_addr[NUM_OF_ENDPOINTS] =
{
  TXD0,     //  Endpoint 0
  TXD1,     //  Endpoint 1
  0xFF,
  TXD2,     //  Endpoint 3
  0xFF,
  TXD3,     //  Endpoint 5
  0xFF
};

CONST INT16 usbn9604_rx_endpoint_addr[NUM_OF_ENDPOINTS] =
{
  RXD0,     //  Endpoint 0
  0xFF,
  RXD1,     //  Endpoint 2
  0xFF,
  RXD2,     //  Endpoint 4
  0xFF,
  RXD3      //  Endpoint 6
};

CONST INT16 fifo_sizes[] =
{
  EP0_FIFO_SIZE,          //  control tx0, rx0
  TX_BULK_EP_FIFO_SIZE,   //  bulk tx
  RX_BULK_EP_FIFO_SIZE,   //  bulk rx
  TX_ISO_EP_FIFO_SIZE,    //  ISO tx
  RX_ISO_EP_FIFO_SIZE,    //  ISO rx
  TX_INTR_EP_FIFO_SIZE,   //  interrupt tx
  RX_INTR_EP_FIFO_SIZE    //  interrupt rx
};

CONST INT16 endpoint_to_fifo[] =
{
  TX_FIFO0,               //  endpoint0 TX/RX FIFO0
  TX_FIFO1,               //  endpoint1
  RX_FIFO1,               //  endpoint2
  TX_FIFO2,               //  endpoint3
  RX_FIFO2,               //  endpoint4
  TX_FIFO3,               //  endpoint5
  RX_FIFO3                //  endpoint6
};

// =============================================================================
//                         Data Buffer Definitions
// =============================================================================

control_buffer_t        control_send_buffer;
control_buffer_t        control_receive_buffer;

UINT8                   request_buffer[EP0_FIFO_SIZE];

//----------------------------------------------------------------
// Function :   clear_control_buffer
// Notes    :
// History  :
//----------------------------------------------------------------

VOID clear_control_buffer( control_buffer_t * control_buff )
{
  control_buff->dataRam = NULL;
  control_buff->dataRom = NULL;
  control_buff->bytes_counter = 0;
}

//****************************************************************
//
//                   USB Driver Implementations
//
//****************************************************************

//----------------------------------------------------------------
// Function :   usb_node_handler
// Notes    :   USB interrupt handler.
// History  :
//----------------------------------------------------------------

VOID usb_node_handler( VOID )
{
  UINT8 usbn_event;

//  disable_interrupt();

  while ( usbn_event = read_usb(MAEV) )
  {
    if ( usbn_event & RX_EV )
      USBN9604_rx_event_handler();
    if ( usbn_event & TX_EV )
      USBN9604_tx_event_handler();
    if ( usbn_event & ALT )
      USBN9604_alt_event_handler();
    if ( usbn_event & NAK )
    {
      if ( read_usb(NAKEV) & 0x10 )
      {
#ifdef DEBUG
//        Printf( "\nNAK" );
#endif

        // ----------
        // NAK OUT
        // ----------
        FLUSHTX0;
        FLUSHRX0;
        // ------------------
        // re enable receving
        // ------------------
        DISABLE_TX(ENDPOINT_0);
        ENABLE_RX(ENDPOINT_0);
      }
    }
  }

//  enable_interrupt();
}

//----------------------------------------------------------------
// Function :   USBN9604_tx_event_handler
// Notes    :   TX event handler
// History  :
//----------------------------------------------------------------

VOID USBN9604_tx_event_handler( VOID )
{
  VOLATILE UINT8        tx_event = TX_EVENTS;
           UINT8        evnt_mask = 1;
           UINT8        txstat;

#ifdef DEBUG
  Printf( "\nTe%X", tx_event );
#endif

  while ( tx_event )
  {
    switch ( tx_event & evnt_mask )
    {
    case TX_FIFO0:
      // -----------
      // endpoint 0
      // zero tx endpoint
      // -----------
      txstat = EP_TXSTATUS(ENDPOINT_0);
      if ( txstat & ACK_STAT )
      {

#ifdef DEBUG
        Printf( " Tx(%X) ", control_send_buffer.bytes_counter );
#endif

        // --------------------------------------
        // previous data packet from current
        // ep was received successfully by host
        // -------------------------------------
        endpoint_stat[ENDPOINT_0]->FIFO_status = EMPTY;
        if ( control_send_buffer.bytes_counter > 0 )
        {
          // -------------------------------------
          // there is additional data to be sent
          // ------------------------------------
          fill_control_fifo();
          usbn9604_tx_enable(ENDPOINT_0);
        }
        else if ( device_buffers.zero_data == 1 )
        {
#ifdef DEBUG
          Printf( " TxOK" );
#endif

          // -------------------
          // zero data required
          // -------------------
          device_buffers.zero_data = 0;
          FLUSH_TXEP(ENDPOINT_0);
          usbn9604_tx_enable(ENDPOINT_0);
        }
        else
        {
#ifdef DEBUG
          Printf( " TxOK" );
#endif

          FLUSH_TXEP(ENDPOINT_0);
          ENABLE_RX(ENDPOINT_0);
        }
      }
      else
        // -------------------
        // there is no ACK
        // Re-enable receiving
        // --------------------
        ENABLE_RX(ENDPOINT_0);
      break;
    case TX_FIFO1:
      // ----------------
      // endpoint 1
      // bulk tx endpoint
      // ----------------
      txstat = EP_TXSTATUS(ENDPOINT_1);
      if ( txstat & ACK_STAT )
      {
        // -----------------------------------------------------------------------
        // previous data packet from current ep was received successfully by host
        // -----------------------------------------------------------------------
        FLUSHTX1;
        send_event(EVT_USB_BULK_TX);
      }
      else
        // ------------------------------------
        // there is no ACK
        // retransmit the previous data packet
        // ------------------------------------
        usbn9604_tx_retransmit_enable(ENDPOINT_1);
      break;
    case TX_FIFO2:
      // -----------------------
      // endpoint 3
      // isochronous tx endpoint
      // -----------------------
      txstat = EP_TXSTATUS(ENDPOINT_3);
      //                      PCDOUT &= ~0x2;
      //                      PCDOUT |=  0x2;
      if ( txstat & ACK_STAT )
      {
        // -----------------------------------------
        // Data was sent in response to an IN token
        // -----------------------------------------
        FLUSHTX2;
        send_event(EVT_USB_ISO_TX);
      }
      else
      {
        // ------------------------
        // ISO frame was discarded
        // Do nothing
        // ------------------------
      }
      break;
    case TX_FIFO3:
      // -----------------------
      // endpoint 5
      // interrupt tx endpoint
      // -----------------------
      txstat = EP_TXSTATUS(ENDPOINT_5);
      if ( txstat & ACK_STAT )
      {
        // -----------------------------------------------------------------------
        // previous data packet from current ep was received successfully by host
        // -----------------------------------------------------------------------
        FLUSHTX3;
      }
      else
        // ------------------------------------
        // there is no ACK
        // retransmit the previous data packet
        // ------------------------------------
        usbn9604_tx_retransmit_enable(ENDPOINT_5);
      break;
    case TX_UDRN0:
    case TX_UDRN1:
    case TX_UDRN2:
    case TX_UDRN3:
      break;
    default:
      // -------------------------------
      //  No event with this event mask
      // -------------------------------
      break;
    }
    tx_event &= ~evnt_mask;
    evnt_mask = evnt_mask << 1;
  }
}

//----------------------------------------------------------------
// Function :   USBN9604_alt_event_handler
// Notes    :   ALT event handler
// History  :
//----------------------------------------------------------------

VOID USBN9604_alt_event_handler(VOID)
{
  VOLATILE UINT8        alt_event = ALT_EVENTS;
  VOLATILE UINT8        dma_event = 0;
           UINT8        evnt_mask = 1;
           UINT8        tmp = 0;

#ifdef DEBUG
  Printf( "\nAe%X", alt_event );
#endif

  while ( alt_event )
  {
    switch ( alt_event & evnt_mask )
    {
    case ALT_WKUP:
      write_usb(WKUP,0x0C);
    break;

#ifdef USB_USEDMA

    case ALT_DMA:
      // ------------
      //  DMA events
      // ------------
      dma_event = DMA_EVENTS ;
      // --------------------------------
      // clear all dma events to be sure
      //--------------------------------
      write_usb(DMAEV,0x00);
      switch ( dma_event&0xf )
      {
      case DMA_DSIZ:
        // ----------------------------------------------------------
        // Only for DMA receive operation
        // It indicates that a packet has been received which is less
        // then the full length of the FIFO. This normally indicates
        // the end of transfer.
        // ---------------------------------------------------------
        // ------------------
        //  update toggle bit
        // ------------------
        if ( DMA_NTGL&dma_event )
          endpoint_stat[DMA_ep]->toggle_bit = 1;
        else
          endpoint_stat[DMA_ep]->toggle_bit = 0;

        // ------------------------
        // Insert your code here
        // -----------------------
        break;
      case DMA_DCNT:
        // -------------------------------------
        // DMA count (DMA Count) regiser is 0
        // ADMA is stopped
        // ------------------------------------
        // -----------------
        // update toggle bit
        // ------------------
        tmp = read_usb(DMACNTRL);
        if ( DMA_NTGL&dma_event )
        {
          tmp |= DMA_DTGL;
          endpoint_stat[DMA_ep]->toggle_bit = 1;
        }
        else
        {
          tmp &= ~DMA_DTGL;
          endpoint_stat[DMA_ep]->toggle_bit = 0;
        }
        // ----------------------------
        // re-enable USB DMA operation
        // ------------------------------
        write_usb(DMACNTRL,tmp);
        USB_start_dma( 9 );             //  may be any number from 0 to 255
        // ------------------------
        // Insert your code here
        // ------------------------
        break;
      case DMA_DERR:
        break;
      case DMA_DSHLT:
        break;
      }
    break;

#else

    case ALT_DMA:
    break;

#endif

    case ALT_RESET:
      CLEAR_STALL_EP0;
      GOTO_STATE(RST_ST);
      /*--------------------
      * set default address
      *--------------------*/
      write_usb(FAR,AD_EN+0);
      /*----------------
      * enable EP0 only
      *----------------*/
      write_usb(EPC0, 0x00);
      /*---------------
      * Go Reset state
      *---------------*/
      USB_reset();
      USB_device_reset();
      /*--------------
      * Go operational
      *---------------*/
      GOTO_STATE(OPR_ST);
      break;
    case ALT_SD5:
    case ALT_SD3:
      // ------------------
      // Enable Resume int
      // ------------------
      ENABLE_ALT_INTS(ALT_SD3|ALT_RESET|ALT_RESUME|ALT_DMA);
      // -------------------
      // enter suspend state
      // ------------------
      GOTO_STATE(SUS_ST);
      break;
    case ALT_RESUME:
      // ------------------
      // Disabe Resume int
      // ------------------
      ENABLE_ALT_INTS(ALT_SD3|ALT_RESET|ALT_DMA);
      // -----------------
      // Operational state
      // -----------------
      GOTO_STATE(OPR_ST);
      // ----------------
      // Enable receving
      // ---------------
      ENABLE_RX(ENDPOINT_0);
      break;
    case ALT_EOP:
      break;
    default:
      // ------------------------------
      // No event with this event mask
      // ------------------------------
      break;
    }
    alt_event &= ~evnt_mask;
    evnt_mask = evnt_mask << 1;
  }
}

//----------------------------------------------------------------
// Function :   USBN9604_rx_event_handler
// Notes    :   RX event handler
// History  :
//----------------------------------------------------------------

VOID USBN9604_rx_event_handler( VOID )
{
  VOLATILE UINT8        rx_event = RX_EVENTS;
  VOLATILE UINT8        rxstat;
           UINT8        evnt_mask = 1;
           UINT8        Toggle;
           UINT8        i;
           INT16        request_length;
           endpoint_t   ep_num = ENDPOINT_0;

#ifdef DEBUG
  Printf( "\nRe%X", rx_event );
#endif

  while ( rx_event )
  {
    switch ( rx_event & evnt_mask )
    {
    case RX_FIFO0:
      // -----------
      // endpoint 0
      // -----------
      ep_num = ENDPOINT_0;
      clear_control_buffer(&control_receive_buffer);
      // -----------------------
      // Is this a setup packet?
      // -----------------------
      rxstat = EP_RXSTATUS(ENDPOINT_0);
      if ( rxstat & SETUP_R )
      {
        // ----------------------
        // Clear STALL condition
        // ----------------------

        endpoint_stat[0]->toggle_bit = 0x01;

        CLEAR_STALL_EP0;
        request_length = min(fifo_sizes[ep_num], RX_EP_COUNTER(ep_num));
        // -------------------------
        // Receive request from hub
        // -------------------------
        for ( i=0; i<request_length; i++ )
        {
          request_buffer[i] = EP_FIFO_READ(ENDPOINT_0);
        }

        control_receive_buffer.dataRam = (UINT8 *)request_buffer;
        control_receive_buffer.bytes_counter = request_length;

        FLUSH_TXEP(ENDPOINT_0);
        FLUSH_RXEP(ENDPOINT_0);
        //
        USB_device_req_handler();

      }
      // ------------------------------------------------------------
      // OUT packet of Status stage in control read/write sequence
      // ----------------------------------------------------------
      else
      {
        // --------------------
        // Re-enable receiving
        // -------------------
        FLUSHTX0;
        FLUSHRX0;
        ENABLE_RX(ENDPOINT_0);
        clear_control_buffer(&control_send_buffer);
      }
      break;
    case RX_FIFO1:

#ifdef DEBUG
      Printf( "\nR2 " );
#endif

      // -------------------
      // endpoint 2 bulk OUT
      // -------------------
      ep_num = ENDPOINT_2;

      rxstat = EP_RXSTATUS(ep_num);
      if ( rxstat & RX_ERR )
      {
#ifdef DEBUG
        Printf( " err" );
#endif

        // -------------------
        // receive media error
        // --------------------
        FLUSH_RXEP(ep_num);
        ENABLE_RX(ep_num);
        break;
      }
      Toggle = endpoint_stat[ep_num]->toggle_bit;
      Toggle <<= 5;
      if ( (rxstat & RX_LAST) && ((rxstat & RX_TOGL) == Toggle ) )
      {
#ifdef DEBUG
        Printf( " OK" );
#endif

        // -------------------------
        // received data is correct
        // -------------------------
        endpoint_stat[ep_num]->toggle_bit = endpoint_stat[ep_num]->toggle_bit ? 0 : 1;
        send_event(EVT_USB_BULK_RX);
      }
      else
      {
#ifdef DEBUG
        Printf( " Terr" );
#endif

        // --------------------------------
        // response to the toggle bit error
        // response to the receive error
        // --------------------------------
        FLUSH_RXEP(ep_num);
        ENABLE_RX(ep_num);
      }
      break;
    case RX_FIFO2:
       // ----------------
       // endpoint 4
       // isochronous OUT
       // ----------------
      rxstat = EP_RXSTATUS(ENDPOINT_4);
      if ( (rxstat & RX_ERR) &&!(rxstat & RX_LAST) )
      {
        // -------------------
        // receive media error
        // --------------------
        FLUSH_RXEP(ENDPOINT_4);
        ENABLE_RX(ENDPOINT_4);
        break;
      }
      // -------------------------
      // received data is correct
      // -------------------------
      send_event(EVT_USB_ISO_RX);
      break;
    case RX_FIFO3:
      break;
    case RX_OVRN0:
      break;
    case RX_OVRN1:
      break;
    case RX_OVRN2:
      break;
    case RX_OVRN3:
      break;
    default:
      // ------------------------------
      // No event with this event mask
      // ------------------------------
      break;
    }
    rx_event &= ~evnt_mask;
    evnt_mask = evnt_mask << 1;
  }
}

//****************************************************************
//
//         Usb module external interface implementations
//
//****************************************************************

//----------------------------------------------------------------
// Function :   USB_Transmit_Data
// Purpose  :   Fill corresponding endpoint with the new data.
//              The data size to be written to the FIFO is the minimum
//              between requested size and FIFO size.
// Inputs   :   data_size - size of data (in bytes) to be sent
//              data_ptr  - pointer to data to be sent
//              ep_num    - number of transmit endpoint
// Outputs  :   None.
// Returns  :   Number of byes that was write to the fifo
// Notes    :
// History  :
//----------------------------------------------------------------

UINT8 USB_Transmit_Data ( INT16 data_size, UINT8 * data_ram, UINT8 CONST * data_rom, endpoint_t ep_num )
{
  UINT8         size, count;

//  disable_USBint();
  disable_interrupt();

  FLUSH_TXEP(ep_num);

  // -------------------------------------------------
  // assure that data size is no more then fifo length
  // --------------------------------------------------
  size = count = min(data_size, fifo_sizes[ep_num]);

#ifdef DEBUG
  Printf( " S(%X)", size );
#endif

  // -------------------------------------------
  // Now data size is no more than the fifo length,
  // transfer the data directly to the fifo
  // --------------------------------------------
  if( data_ram != NULL )
  {
    burstw_start_usb( EP_TXD( ep_num ) );
    while ( count-- )
    {
#ifdef DEBUG
//      Printf( " %X", *data_ram );
#endif

      _burstw_put_usb( *data_ram++ );   // FIFO write
    }
    burstw_stop_usb();
  }
  if( data_rom != NULL )
  {
    burstw_start_usb( EP_TXD( ep_num ) );
    while ( count-- )
    {
#ifdef DEBUG
//      Printf( " %X", *data_rom );
#endif

      _burstw_put_usb( *data_rom++ );   // FIFO write
    }
    burstw_stop_usb();
  }
  // ---------------------------
  // enable  data transmittion
  // ---------------------------
  usbn9604_tx_enable(ep_num);

//  enable_USBint();
  enable_interrupt();

#ifdef DEBUG
  Printf( " OK" );
#endif

  return (size);
}

//----------------------------------------------------------------
// Function :   USB_Receive_Data
// Purpose  :   Read whole corresponding endpoint to the local buffer.
//
// Inputs   :   data_size - size of data (in bytes) to be read
//              data_ptr  - pointer to the local buffer
//              ep_num    - number of receive endpoint
// Outputs  :   None.
// Returns  :   Number of byes that was read from the FIFO.
// Notes    :
// History  :
//----------------------------------------------------------------

UINT8 USB_Receive_Data ( INT16 data_size, UINT8 * data_ptr, endpoint_t ep_num )
{
  UINT8         bytes_count;
  UINT8         bytes_sum = 0;
  INT16         i;

//  disable_USBint();
  disable_interrupt();

#ifdef DEBUG
  Printf( "\nR(%X)", data_size );
#endif

  // -------------------------------------------
  // Read data from the fifo until it's empty
  // -------------------------------------------
  do
  {
    // -----------------------------------------
    // Read count of bytes presently in the FIFO
    // -----------------------------------------
    bytes_count =  RX_EP_COUNTER(ep_num);

#ifdef DEBUG
    Printf( "c%Xs%X", bytes_count, bytes_sum );
#endif

    if( (bytes_sum + bytes_count) > data_size )
    {
      bytes_count = data_size - bytes_sum;
    }

#ifdef DEBUG
    Printf( "r%X", bytes_count );
#endif

    // -----------------------------------
    // read data directly from the fifo
    // -----------------------------------
    burstr_start_usb( EP_RXD( ep_num ) );
    //
    for ( i = 0; i < bytes_count; i++ )
    {
#ifdef USB_NOSPI
      _burstr_get_usb( *data_ptr++ );           // FIFO read
#endif
#ifdef USB_SPI
      *data_ptr++ = _burstr_get_usb();          // FIFO read
#endif
    }
    burstr_stop_usb();
    //
    bytes_sum += bytes_count;

    if( bytes_sum == data_size ) break;
  }
  // -----------------------------------------------------
  // If the FIFO containes more than 15 bytes, a value 15 is
  // reported in the counter. Therefore continue reading from the
  // fifo until the counter is less then 15.
  // ------------------------------------------------------
  while ( bytes_count == 0xf );

  // ----------------------------------------------
  // Flush the fifo and re-enable receive operation
  // -----------------------------------------------
  FLUSH_RXEP(ep_num);
  ENABLE_RX(ep_num);

//  enable_USBint();
  enable_interrupt();

  return (bytes_sum);
}

//****************************************************************
//
//                  Usb module services implementations
//
//****************************************************************

//----------------------------------------------------------------
// Function :   send_control_data
// Purpose  :   Sends data through the control pipe
//
// Inputs   :   data_ptr - pointer to the data buffer to be send
//              data_size - data buffer size
// Outputs  :   None.
// Returns  :   None.
// Notes    :
// History  :
//----------------------------------------------------------------

VOID  send_control_data( UINT8 * data_ram, UINT8 CONST * data_rom, INT16 data_size )
{
  ASSERT( (UINT16)data_ram != (UINT16)data_rom );

  FLUSHTX0;
  control_send_buffer.dataRom = data_rom;
  control_send_buffer.dataRam = data_ram;
  control_send_buffer.bytes_counter = data_size;
  fill_control_fifo();
  usbn9604_tx_enable(ENDPOINT_0);
}

//----------------------------------------------------------------
// Function :   zero_length_data_response
// Notes    :   Generates zero length data packet
// History  :
//----------------------------------------------------------------

VOID zero_length_data_response( endpoint_t ep_num )
{
  FLUSH_TXEP(ep_num);
//      FLUSH_RXEP(ep_num);
  usbn9604_tx_enable(ep_num);
}

//----------------------------------------------------------------
// Function :   fill_control_fifo
// Notes    :   Transfers data from the control buffer to zero endpoint control fifo.
// History  :
//----------------------------------------------------------------

VOID fill_control_fifo( VOID )
{
  UINT8         * data_ram = control_send_buffer.dataRam;
  UINT8   CONST * data_rom = control_send_buffer.dataRom;

  // -----------------------------------
  // number of bytes actually to be sent
  // -----------------------------------
  INT16   count = min(control_send_buffer.bytes_counter, EP0_FIFO_SIZE);

  FLUSH_RXEP(ENDPOINT_0);
  FLUSH_TXEP(ENDPOINT_0);

  // ----------------------------------
  // update control buffer parameters
  // ----------------------------------
  control_send_buffer.bytes_counter -= count;
  if( data_ram != NULL )
  {
    control_send_buffer.dataRam += count;
    while ( count-- )
    {
#ifdef DEBUG
//      Printf( " %X", *data_ram );
#endif

      EP_FIFO_WRITE(ENDPOINT_0, *data_ram++);
    }
  }
  if( data_rom != NULL )
  {
    control_send_buffer.dataRom += count;
    while ( count-- )
    {
#ifdef DEBUG
//      Printf( " %X", *data_rom );
#endif

      EP_FIFO_WRITE(ENDPOINT_0, *data_rom++);
    }
  }
}

//----------------------------------------------------------------
// Function :   usbn9604_tx_enable
// Notes    :   Enable tx endpoint transmission
// History  :
//----------------------------------------------------------------

VOID usbn9604_tx_enable( INT16 ep_num )
{
  UINT8 Toggle = endpoint_stat[ep_num]->toggle_bit;

  // -------------------------------------------
  // update toggle bit of appropriate endpoint
  // -------------------------------------------
  endpoint_stat[ep_num]->toggle_bit = !endpoint_stat[ep_num]->toggle_bit;
  Toggle = Toggle << 2;

  switch ( ep_num )
  {
  case ENDPOINT_0:
    write_usb(EP_TXC(ep_num), Toggle|TX_EN);
    write_usb(EP_RXC(ENDPOINT_0), 0x0);
    break;
  case ENDPOINT_1:
  case ENDPOINT_5:
    write_usb(EP_TXC(ep_num), Toggle|TX_LAST|TX_EN);
    break;
  case ENDPOINT_3:      //  isochronous
    write_usb(EP_TXC(ep_num), Toggle|TX_LAST|IGN_ISOMSK|TX_EN);
    break;
  }
}

//----------------------------------------------------------------
// Function :   usbn9604_tx_retransmit_enable
// Notes    :   Enable tx data retransmission
// History  :
//----------------------------------------------------------------

VOID usbn9604_tx_retransmit_enable( INT16 ep_num )
{
  // ------------------------------
  // use previous toggle bit value
  // ------------------------------
  UINT8 Toggle = endpoint_stat[ep_num]->toggle_bit;
  Toggle = Toggle << 2;

  if ( ep_num == ENDPOINT_0 )
    // -------------------------------------------------
    // there is no retransmission from the endpoint zero
    // ------------------------------------------------
    return;
  else
    write_usb(EP_TXC(ep_num), Toggle|TX_LAST|TX_EN|RFF);
}

//extern USB_endpoint_desc_t * usb_dev_endpoints[];

#ifdef USB_USEDMA

//----------------------------------------------------------------
// Function :   USB_init_dma
// Notes    :   This is a sample code for dma initialization. This function
//              should be called before starting  DMA operation.
// History  :
//----------------------------------------------------------------

VOID USB_init_dma( endpoint_t endPoint )
{
  UINT8         dmaControl = 0; //  value of DMA control register

  DMA_ep = endPoint;

  // -------------------------------
  //   update DMA source field
  // -------------------------------
  dmaControl = ((endPoint - 1)&3);

  // ----------------------------------
  // update DMA Toggle bit if needeed
  // -----------------------------------
  if ( IS_EP_ISO(endPoint) )
  {
    // ----------------------
    // Isochronous endpoint.
    // -----------------------
    if ( IS_EP_IN(endPoint) )
      // ------------------------------------------------
      // Transmit endpoint
      // It seems that in this mode ISO Mask should
      // better be ignored.
      // -----------------------------------------------
      write_usb(EP_TXC(endPoint), IGN_ISOMSK);
    else
      // ------------------------------------------------
      // Receive endpoint
      // It seems that in this mode toggle compare should
      // better be disabled.
      // -----------------------------------------------
      dmaControl |= IGNRXTGL;
  }
  else
  {
    // ----------------------------------
    //  update DMA Toggle bit if needeed
    // -----------------------------------
    if ( endpoint_stat[endPoint]->toggle_bit )
      dmaControl |= DMA_DTGL;
  }

  // ------------------------------
  // write to DMA control register
  // ------------------------------
  write_usb(DMACNTRL,dmaControl);

  // --------------------------------
  // clear all dma events to be sure
  // -------------------------------
  write_usb(DMAEV,0x00);
  // -------------------------------------
  // do automatic error handling if needed
  // ------------------------------------
  write_usb(DMAERR,0x88);

  // -------------------------
  // enable DMA interrupts
  // -----------------------
  write_usb(DMAMSK,(DMA_DCNT|DMA_DSIZ|DMA_DERR |DMA_DSHLT ));

  // -----------------------------------------------
  // enable DMA Request interrupt for DMA emulation
  // ------------------------------------------------
//      ICU_UNMASK_INT(DRQ_INT);
}

//----------------------------------------------------------------
// Function :   USB_start_dma
// Purpose  :   This sample code for strat dma process. Call this function each time DMA fills/empties a page.
//              Note, after calling this function you must set endpoint Enable bit of respective endpoint
//              using  ENABLE_EP(endpoint number) macro.
//              Before starting DMA process, disable interrupt of respective endpoint using
//              DISABLE_TX_INTS(TX_FIFO[number of FIFO]) for transfer operations or
//              DISABLE_RX_INTS(RX_FIFO[number of FIFO]) for receive operations
//
// Inputs   :   DCOUNT - (Number of packets to transfer)
// Outputs  :   None.
// Returns  :   None.
// Notes    :
// History  :
//----------------------------------------------------------------

VOID USB_start_dma( INT16 DCOUNT )
{
  UINT8  dmaControl = 0;        //  value of DMA control register
  // -----------------------------------
  // write number of packets to transfer
  // to DMA count register
  // ----------------------------------
  write_usb(DMACNT,(DCOUNT -1));

  // ----------------------------------------------
  // Set Atomatic DMA Mode and enable DMA operation
  // ------------------------------------------------
  dmaControl = read_usb(DMACNTRL);
  dmaControl |= (DMA_ADMA | DMA_DEN);
  write_usb(DMACNTRL,dmaControl);

}

// ----------------------------------------------------
// For test use
//----------------------------------------------------
//
//#pragma interrupt(usb_drq_handler)
//VOID usb_drq_handler(VOID)
//{
//  ICU_MASK_INT(DRQ_INT);
//  PFDOUT = (PFDOUT & 0x7f);
//  while ( ICU_STATUS_INT(DRQ_INT) )
//    USBDATA = 0x14;
//  PFDOUT = (PFDOUT | 0x80);
//  ICU_UNMASK_INT(DRQ_INT);
//}

#endif

//      End

