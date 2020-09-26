//****************************************************************
// Copyright (C), 2002 MMSoft, All rights reserved
//****************************************************************

//****************************************************************
//
// SOURCE FILE: USBDRV.H
//
// MODULE NAME: USBDRV
//
// PURPOSE:     USBN9603 driver header file.
//
// AUTHOR:      Marek Mikolajewski (MM)
//
// REVIEWED BY:
//
// HISTORY:     Ver   Date       Sign   Description
//
//              001   19-06-2002 MM     Created
//
//****************************************************************

#ifndef __USBDRV_H__
  #define __USBDRV_H__

//#define USB_USEDMA

//****************************************************************
//
// Macros
//
//****************************************************************

//
// Flush and disable the USB TX 0/1/2
//
  #define FLUSHTX0 {write_usb(TXC0,FLUSH);}
  #define FLUSHTX1 {write_usb(TXC1,FLUSH);}
  #define FLUSHTX2 {write_usb(TXC2,FLUSH);}
  #define FLUSHTX3 {write_usb(TXC3,FLUSH);}
//
// Flush and disable the USB RX 0/1/2
//
  #define FLUSHRX0 {write_usb(RXC0,FLUSH);}
  #define FLUSHRX1 {write_usb(RXC1,FLUSH);}
  #define FLUSHRX2 {write_usb(RXC2,FLUSH);}

  #define ENABLE_RX0 write_usb(RXC0,RX_EN);
  #define ENABLE_RX1 write_usb(RXC1,RX_EN);
  #define ENABLE_RX2 write_usb(RXC2,RX_EN);

  #define ENABLE_TX0 write_usb(TXC0,TX_EN);
  #define ENABLE_TX1 write_usb(TXC1,TX_EN);
  #define ENABLE_TX2 write_usb(TXC2,TX_EN);

  #define FLUSH_TXEP(ep_num)              write_usb(EP_TXC(ep_num), FLUSH)
  #define FLUSH_RXEP(ep_num)              write_usb(EP_RXC(ep_num), FLUSH)

  #define ENABLE_RX(ep_num)               write_usb(EP_RXC(ep_num), RX_EN)
  #define DISABLE_RX(ep_num)              write_usb(EP_RXC(ep_num), read_usb(EP_RXC(ep_num)) & ~RX_EN)
  #define ENABLE_TX(ep_num)               write_usb(EP_TXC(ep_num), TX_EN)
  #define DISABLE_TX(ep_num)              write_usb(EP_TXC(ep_num), read_usb(EP_TXC(ep_num)) & ~TX_EN)

  #define GOTO_STATE(state)               write_usb(NFSR, (state));

  #define ATTACH_NODE                     write_usb(MCNTRL, read_usb(MCNTRL) | NAT);
  #define DETACH_NODE                     write_usb(MCNTRL, read_usb(MCNTRL) & ~NAT);

  #define ENABLE_NODE_INTS(intr)          write_usb(MAMSK, (intr));
  #define DISABLE_NODE_INTS               write_usb(MAMSK, 0);

  #define ENABLE_ALT_INTS(intr)           write_usb(ALTMSK, (intr));
  #define DISABLE_ALT_INTS(intr)          write_usb(ALTMSK, read_usb(ALTMSK)&~(intr));

  #define ENABLE_NAK_INTS(intr)           write_usb(NAKMSK, (intr));
  #define DISABLE_NAK_INTS(intr)          write_usb(NAKMSK, read_usb(NAKMSK)&~(intr));

  #define ENABLE_TX_INTS(intr)            write_usb(TXMSK, read_usb(TXMSK)|(intr));
  #define DISABLE_TX_INTS(intr)           write_usb(TXMSK, read_usb(TXMSK)&~(intr));

  #define ENABLE_RX_INTS(intr)            write_usb(RXMSK, read_usb(RXMSK)|(intr));
  #define DISABLE_RX_INTS(intr)           write_usb(RXMSK, read_usb(RXMSK)&~(intr));

  #define ENABLE_VGE                      write_usb(MCNTRL, read_usb(MCNTRL) | VGE);

  #define SET_EP_ADDRESS(ep, addr)        write_usb((ep), (addr));

  #define USB_DEVICE_ADDRESS_ENABLE       write_usb(FAR, read_usb(FAR) | AD_EN);
  #define USB_DEVICE_ADDRESS_DISABLE      write_usb(FAR, read_usb(FAR) & ~AD_EN);
  #define SET_USB_DEVICE_ADDRESS(addr)    write_usb(FAR, read_usb(FAR) | addr);//((addr) & ~AD_EN));

  #define ENABLE_DEFAULT_ADDRESS          write_usb(EPC0, read_usb(EPC0) | DEF);
  #define CLEAR_DEFAULT_ADDRESS           write_usb(EPC0, read_usb(EPC0) & ~DEF);

  #define SET_DEVICE_STATE(dev, st)       (dev.state = st)
  #define DEVICE_STATE(dev)               dev.state
  #define SET_DEVICE_ADDRESS(dev, addr)   (dev.address = addr)
  #define DEVICE_ADDRESS(dev)             dev.address

  #define EP_FIFO_WRITE(ep_num, data)     write_usb(EP_TXD(ep_num), (data))
  #define EP_FIFO_READ(ep_num)            read_usb(EP_RXD(ep_num))

  #define CLEAR_STALL_EP0                 write_usb(EPC0, read_usb(EPC0) & ~STALL)
  #define IS_EP0_STALLED                  ((read_usb(EPC0) & STALL) == STALL)

//
// The macro may be used only for ENDPOINT_1 .. ENDPOINT_6 endpoints,
// It's assumpted that diff between two EPC# and EPC#+1 is 4 bytes
//
  #define EPC_ADDR(ep_num)                EPC1 + (((ep_num) - ENDPOINT_1) << 2)
  #define STALL_EP(ep_num)                write_usb(EPC_ADDR(ep_num), read_usb(EPC_ADDR(ep_num)) | STALL)
  #define CLEAR_STALL_EP(ep_num)          write_usb(EPC_ADDR(ep_num), read_usb(EPC_ADDR(ep_num)) & ~STALL)
  #define IS_EP_STALLED(ep_num)           ((read_usb(EPC_ADDR(ep_num)) & STALL) == STALL)

  #define IS_EP_ISO(ep_num)               ((read_usb(EPC_ADDR(ep_num)) & ISO) == ISO)
  #define IS_EP_IN(ep_num)                (ep_num%2)
  #define ENABLE_EP(ep_num)               write_usb(EPC_ADDR(ep_num), read_usb(EPC_ADDR(ep_num)) | EP_EN)
  #define DISABLE_EP(ep_num)              write_usb(EPC_ADDR(ep_num), read_usb(EPC_ADDR(ep_num)) & ~EP_EN)

  #define EP_TXSTATUS(ep_num)             read_usb(EP_TXS(ep_num))
  #define EP_RXSTATUS(ep_num)             read_usb(EP_RXS(ep_num))

  #define ALT_EVENTS                      read_usb(ALTEV)
  #define DMA_EVENTS                      read_usb(DMAEV)
  #define NAK_EVENTS                      read_usb(NAKEV)
  #define TX_EVENTS                       read_usb(TXEV)
  #define RX_EVENTS                       read_usb(RXEV)

  #define IS_REQ_INDEX_NOT_ZERO(req)      (REQ_INDEX(req).as_bytes.msb != 0x0 &&  REQ_INDEX(req).as_bytes.lsb != 0x0)
  #define IS_REQ_VALUE_NOT_ZERO(req)      (REQ_VALUE(req).as_bytes.msb != 0x0 &&  REQ_VALUE(req).as_bytes.lsb != 0x0)

  #define IS_TX_ENABLE(ep_num)            (read_usb(EP_TXC(ep_num) & TX_EN))
  #define IS_RX_ENABLE(ep_num)            (read_usb(EP_RXC(ep_num) & RX_EN))

  #define REFILL_FIFO(ep_num)             write_usb(EP_TXC(ep_num), read_usb(EP_TXC(ep_num) | RFF))
  #define SET_LAST(ep_num)                write_usb(EP_TXC(ep_num), read_usb(EP_TXC(ep_num) | LAST))

  #define TXEP_ADDR(ep_num)               usbn9604_tx_endpoint_addr[(ep_num)]
  #define RXEP_ADDR(ep_num)               usbn9604_rx_endpoint_addr[(ep_num)]

  #define EP_TXD(ep_num)                  TXEP_ADDR(ep_num)
// It's assumpted that TXS# is TXD#+1
  #define EP_TXS(ep_num)                  TXEP_ADDR(ep_num) + 1
// It's assumpted that TXC# is TXD#+2
  #define EP_TXC(ep_num)                  TXEP_ADDR(ep_num) + 2

  #define EP_RXD(ep_num)                  RXEP_ADDR(ep_num)
// It's assumpted that RXS# is RXD#+1
  #define EP_RXS(ep_num)                  RXEP_ADDR(ep_num) + 1
// It's assumpted that RXC# is RXD#+2
  #define EP_RXC(ep_num)                  RXEP_ADDR(ep_num) + 2

// 20 MHz
// 4 cycles takes single for loop
// 10s waiting threshold
  #define WAITING_THRESHOLD               50000000

  #define TX_EP_COUNTER(ep_num)           (read_usb(EP_TXS(ep_num)) & 0x1F)//20)
  #define RX_EP_COUNTER(ep_num)           (read_usb(EP_RXS(ep_num)) & 0xF) //10)

  #define NUM_OF_ENDPOINTS                7

  #define EP0_FIFO_SIZE                   8
  #define RX_BULK_EP_FIFO_SIZE            64
  #define TX_BULK_EP_FIFO_SIZE            64

  #define RX_ISO_EP_FIFO_SIZE             64
  #define TX_ISO_EP_FIFO_SIZE             64

  #define RX_INTR_EP_FIFO_SIZE            2
  #define TX_INTR_EP_FIFO_SIZE            2

  #define BZERO(buf)                      memset(&(buf), 0, sizeof(buf))

//============================================================================
//                             Definitons
//============================================================================

typedef enum
{
  ENDPOINT_0,
  ENDPOINT_1,
  ENDPOINT_2,
  ENDPOINT_3,
  ENDPOINT_4,
  ENDPOINT_5,
  ENDPOINT_6,
  ENDPOINT_LAST
} endpoint_t;

typedef struct
{
  UINT8        * dataRam;
  UINT8  CONST * dataRom;
  UINT16         bytes_counter;      // counter of data bytes
                                     // currently stored in the buffer
}control_buffer_t;

typedef enum
{
  EMPTY,
  FULL,
  WAIT_FOR_ACK
}FIFO_status_t;

typedef struct
{
  FIFO_status_t FIFO_status;
  UINT8         toggle_bit;
  BOOL          stalled;
}endpoint_status_t;

//
// RX/TX Events values
//
  #define EVT_USB_BULK_RX         0x01
  #define EVT_USB_BULK_TX         0x02
  #define EVT_TIMER_INT           0x04
  #define EVT_USB_ISO_RX          0x08
  #define EVT_USB_ISO_TX          0x10

  #define EVT_MASK                (EVT_USB_BULK_RX | EVT_USB_BULK_TX | EVT_TIMER_INT | EVT_USB_ISO_RX | EVT_USB_ISO_TX)

extern VOLATILE UINT8 event_table;

//
// Send event prototype
//
  #define send_event( X )   (event_table |= (X))
  #define clear_event( X )       \
{                                \
  disable_interrupt();           \
  event_table &= ~(X);           \
  enable_interrupt();            \
}

//****************************************************************
//
// External Interface
//
//****************************************************************

VOID  usb_node_handler                  ( VOID );
VOID  usb_drq_handler                   ( VOID );

UINT8 USB_Receive_Data                  ( INT16 data_size, UINT8 * data_ptr, endpoint_t ep_num );
UINT8 USB_Transmit_Data                 ( INT16 data_size, UINT8 * data_ram, UINT8 CONST * data_rom, endpoint_t ep_num );

#ifdef USB_USEDMA
VOID  USB_init_dma                      ( endpoint_t endPoint );
VOID  USB_start_dma                     ( INT16 DCOUNT );
#endif

VOID  usbn9604_tx_enable                ( INT16 ep_num );
VOID  usbn9604_tx_retransmit_enable     ( INT16 ep_num );
VOID  endpoint_status_init              ( VOID );
VOID  zero_length_data_response         ( endpoint_t ep_num );
VOID  send_control_data                 ( UINT8 * data_ram, UINT8 CONST * data_rom, INT16 data_size );
VOID  clear_control_buffer              ( control_buffer_t * control_buff );

#endif // __usbdrv_h__



