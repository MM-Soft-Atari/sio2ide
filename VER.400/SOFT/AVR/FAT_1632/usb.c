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

//
// Data buffers
//
extern control_buffer_t          control_send_buffer;
extern control_buffer_t          control_receive_buffer;
//
// Endpoint status
//
extern CONST INT16               usbn9604_tx_endpoint_addr[NUM_OF_ENDPOINTS];
extern CONST INT16               usbn9604_rx_endpoint_addr[NUM_OF_ENDPOINTS];
//
extern CONST endpoint_status_t * endpoint_stat[NUM_OF_ENDPOINTS];

// ****************************************************************
//
//      Globals
//
// ****************************************************************

//
//  The USB device status
//
DEVICE_status_t         device_status;
Device_buffers_t        device_buffers;

//****************************************************************
//
//      Prototypes
//
//****************************************************************

STATIC VOID USB_req_reserved           ( USB_request_t * req );
STATIC VOID USB_req_undefined          ( USB_request_t * req );
STATIC VOID usb_dev_get_status         ( USB_request_t * req );
STATIC VOID usb_dev_clear_feature      ( USB_request_t * req );
STATIC VOID usb_dev_set_feature        ( USB_request_t * req );
STATIC VOID usb_dev_set_address        ( USB_request_t * req );
STATIC VOID usb_std_dev_get_descriptor ( USB_request_t * req );
STATIC VOID usb_std_dev_set_descriptor ( USB_request_t * req );
STATIC VOID usb_dev_get_config         ( USB_request_t * req );
STATIC VOID usb_dev_set_config         ( USB_request_t * req );
STATIC VOID usb_dev_get_interface      ( USB_request_t * req );
STATIC VOID usb_dev_set_interface      ( USB_request_t * req );
STATIC VOID usb_dev_sync_frame         ( USB_request_t * req );

STATIC VOID usb_dev_enable_ep          ( USB_endpoint_desc_t CONST * ep );
STATIC VOID usb_dev_disable_ep         ( USB_endpoint_desc_t CONST * ep );

//----------------------------------------------------------------
// Function :   USB_Init
// Notes    :   Initializes the USBN9604
// History  :
//----------------------------------------------------------------

BOOL USB_Init(VOID)
{
  VOLATILE UINT16 i;

  // --------------
  // Hardware reset
  // Setup MSP430 related HW
  // ---------------

  hal4usb_init();

  // -----------------------------------
  // USB node should have 2^14 cycles
  // of idle run after reset
  // ---------------------------------
  for ( i = 0; i < 0x4000; i++ );

  // ------------------------------------------------
  // check if chip stabilized if not reset the chip
  // ------------------------------------------------
  for ( i = 0; i < 0x8000; i++ )
  {
    write_usb(CCONF, i % 0x10);
    if ( read_usb(CCONF) != (i % 0x10) )
    {
      return FALSE;
    }
  }
  // --------------------------------------------------------------
  // Initialize the clock generator as input for the SCANPSC100F
  // prior to this point, the clock output will be 4 Mhz.  After,
  // it will be (48 MHz/CLKDIV)
  // -------------------------------------------------------------
//  if ( GET_DIP_SW1() & 0x80 )
    write_usb(CCONF, CLKDIV_SLOW-1);
//  else
//    write_usb(CCONF, CLKDIV_FAST-1);
//    write_usb(CCONF, 8-1);

  // ---------------------------------------------------------------
  // Give a software reset, then set ints to active high push pull
  // --------------------------------------------------------------
  write_usb(MCNTRL, SRST);

  // ------------------------------------
  // Wait for end of the initiated reset
  // ------------------------------------
  while ( read_usb(MCNTRL) & SRST );

  // ----------------------------------------------------
  // Set Low Level interrupt type and internal voltage
  // ----------------------------------------------------
  write_usb(MCNTRL, INT_L_P | VGE);

  // -------------------------
  // mask all USB node events
  // ------------------------
  DISABLE_NODE_INTS

  // -----------------------
  // Set up interrupt masks
  // -----------------------
  // ----------------------
  // NAK OUT FIFO 0 evnt
  // ----------------------
  ENABLE_NAK_INTS(NAK_OUT0)
  // -------------------
  // enable TX  events
  // ------------------
  ENABLE_TX_INTS(TX_FIFO0|TX_FIFO1|TX_FIFO2|TX_FIFO3)
  // -----------------
  // enable RX  events
  // -----------------
  ENABLE_RX_INTS(RX_FIFO0|RX_FIFO1|RX_FIFO2|RX_FIFO3)
  // -----------------------------
  // ALT events include DMA event
  // -----------------------------
  ENABLE_ALT_INTS(ALT_SD3|ALT_RESET|ALT_DMA)

  // ----------------------------
  // Enable all below interrupts
  // ---------------------------
  ENABLE_NODE_INTS(/*INTR_E|*/RX_EV|NAK|TX_EV|ALT)
  // ----------------------------------------------
  // Workaround for Voltage Regulator Output issue
  // ----------------------------------------------
  write_usb(0x1f,0x40);

  // ---------------
  // Go operational
  // --------------
  GOTO_STATE(OPR_ST)

  USB_reset();

  ATTACH_NODE
  //
  for ( i = 0; i < 0xffff; i++ );
  //
  return TRUE;
}

//----------------------------------------------------------------
// Function :   USB_reset
// Notes    :   Reset USB
// History  :
//----------------------------------------------------------------

VOID USB_reset(VOID)
{
  // ------------------------------------
  // set default address for endpoint 0
  // -----------------------------------
  SET_EP_ADDRESS(EPC0, 0x0)
  // ----------------------------------------------
  // Set usb default device address (FAR register)
  // ---------------------------------------------
  SET_USB_DEVICE_ADDRESS(0x0)
  // -----------------------------------------
  // enable USB device address (FAR register)
  // ----------------------------------------
  USB_DEVICE_ADDRESS_ENABLE

  // ---------------------------------------------------------
  // enable responce to the default address
  // regardless to the value of the EPC0 and FAR registers
  // --------------------------------------------------------

//    Reset all endpoints
//      for (i=1; i<MAX_NUM_OF_ENDPOINTS; i++)
//      {
//        if (usb_dev_endpoints[i] != NULL)
//        {
//           usb_dev_disable_ep( usb_dev_endpoints[i] );
//        }
//      }

  FLUSHTX0  //ep0
  FLUSHTX1  //ep1
  FLUSHTX2  //ep3
  FLUSHTX3  //ep5

  FLUSHRX0  //ep0
  FLUSHRX1  //ep2
  FLUSHRX2  //ep4

  // ----------------------
  // Global initalizations
  // ---------------------
  clear_control_buffer(&control_send_buffer);
  clear_control_buffer(&control_receive_buffer);
  endpoint_status_init();

  // --------------------
  // Enable the receiver
  // ------------------
  ENABLE_RX0
}

//----------------------------------------------------------------
// Function :   USB_device_reset
// Notes    :   USB device reset
// History  :
//----------------------------------------------------------------

VOID USB_device_reset(VOID)
{
  device_status.last_req = RESERVED_REQ;
  if ( DEVICE_STATE(device_status) == DEV_ADDRESS || DEVICE_STATE(device_status) == DEV_CONFIGURED )
  {
    SET_DEVICE_STATE(device_status, DEV_ATTACHED);
  }
  device_buffers.zero_data = 0;
}

//****************************************************************
//
//                     USB Control Pipe Protocol Definitions
//
//****************************************************************

//****************************************************************
//
//                     USB device Descriptors
//
//****************************************************************

// ------------------------------
// Device USB device descriptor
//-----------------------------
CONST USB_device_desc_t usb_device_desc =
{
  sizeof(USB_device_desc_t),
  DEVICE_DESCRIPTOR,
  SWAP( USB_SPEC_VERSION ),
  CLASS_NOT_DEFINED,       //  CLASS_VENDOR, vendor specific
  0,                       //  Device Sub-Class
  0,                       //  Device Protocol
  EP0_FIFO_SIZE,           //  Max Packet Size for EndPoint Zero
  SWAP( NSC_ID_VENDOR ),
  SWAP( NSC_ID_PRODUCT ),
  SWAP( 0x0100 ),          //  device release number: 01.00 NSC_BCDDEVICE
  STR_MANUFACTURER,
  STR_PRODUCT,
  STR_SERIAL,              //  Device's serial number
  1                        //  Num of configurations
};

//****************************************************************
//
//                       Endpoints Descriptors
//
//****************************************************************

// -------------------------------------
// Device Long Configuration descriptor
// -----------------------------------
CONST USB_long_config_desc_t usb_dev_long_config_desc =
{
  // ---------------------------------
  // Device Configuration descriptor
  // --------------------------------
  {
    sizeof(USB_config_desc_t),        //  CONFIG_DESC_LENGTH
    CONFIG_DESCRIPTOR,
    SWAP( sizeof(USB_long_config_desc_t)),   //  TOTAL CONFIG_DESC_LENGTH
    ALTSET_NUM,                       //  interfaces supported
    1,                                //  Configuration number (0 - Dev Unconfigured)
    0,                                //  no descriptor string STR_PRODUCT
    0x80,                             //  bus powered SELF_POWERED,
    25                                //  (50mA)
  },
  // -------------------------------------------------
  // USB device interface descriptor setting 0
  // Bulk Only Interface
  // ------------------------------------------------
  {
    sizeof(USB_interface_desc_t),
    INTERFACE_DESCRIPTOR,
    0,                          //  The only interface concurrently supported by this configuration
    0,                          //  Alternate Setting
    NUM_OF_ENDPOINTS_FOR_ALT_0, //  Num of endpoints of this interface excluding endpoint zero
    CLASS_MASS_STORAGE,         //  Mass Storage Class
    SUBCLASS_CODE_SCSI,         //  Sub class SCSI
    PROTOCOL_CODE_BULK,         //  Interface Protocol Bulk
    0                           //
  },
  {
    // ------------------------------------------------
    // The IN endpoint 1 is used for bulk data transfer
    // ----------------------------------------------
    {
      sizeof(USB_endpoint_desc_t),
      ENDPOINT_DESCRIPTOR,
      {
        ENDPOINT_1,
        0,
        D_IN
      },
      BULK_ENDPOINT,
      SWAP( TX_BULK_EP_FIFO_SIZE ),   //  Max Packet Size
      0                               //  Irrelevant
    },
    // -------------------------------------------------
    // The OUT endpoint 2 is used for bulk data transfer
    // -----------------------------------------------
    {
      sizeof(USB_endpoint_desc_t),
      ENDPOINT_DESCRIPTOR,
      {
        ENDPOINT_2,
        0,
        D_OUT
      },
      BULK_ENDPOINT,
      SWAP( RX_BULK_EP_FIFO_SIZE ),   //  Max Packet Size
      0                               //  Irrelevant
    }
  },
#if ALTSET_NUM==2
  // -----------------------------------------
  // USB device interface descriptor setting 1.
  // This setting is used for the demo and contains
  // all the endpoints (including Isochronus).
  //----------------------------------------
  {
    sizeof(USB_interface_desc_t),
    INTERFACE_DESCRIPTOR,       //
    0,                          //  The only interface concurrently supported by this configuration
    1,                          //  Alternate Setting
    NUM_OF_ENDPOINTS_FOR_ALT_1, //  Num of endpoints of this interface excluding endpoint zero
    CLASS_VENDOR,               //  Vendor specific
    0,                          //  Sub class
    CLASS_VENDOR,               //  Vendor Specific Interface Protocol
    0                           //
  },
  {
    // ------------------------------------------------
    // The IN endpoint 1 is used for bulk data transfer
    // ----------------------------------------------
    {
      sizeof(USB_endpoint_desc_t),
      ENDPOINT_DESCRIPTOR,
      {
        ENDPOINT_1,
        0,
        D_IN
      },
      BULK_ENDPOINT,
      SWAP( TX_BULK_EP_FIFO_SIZE ),   //  Max Packet Size
      0                               //  Irrelevant
    },
    // -------------------------------------------------
    // The OUT endpoint 2 is used for bulk data transfer
    // ------------------------------------------------
    {
      sizeof(USB_endpoint_desc_t),
      ENDPOINT_DESCRIPTOR,
      {
        ENDPOINT_2,
        0,
        D_OUT
      },
      BULK_ENDPOINT,
      SWAP( RX_BULK_EP_FIFO_SIZE ),   //  Max Packet Size
      0                               //  Irrelevant
    },
    // --------------------------------------------------------
    // The IN endpoint 3 is used for isochronous data transfer
    // -------------------------------------------------------
    {
      sizeof(USB_endpoint_desc_t),
      ENDPOINT_DESCRIPTOR,
      {
        ENDPOINT_3,
        0,
        D_IN
      },
      ISOCHRONOUS_ENDPOINT,
      SWAP( TX_ISO_EP_FIFO_SIZE ),     //  Max Packet Size
      1                                //  Irrelevant
    },
    // --------------------------------------------------------
    // The OUT endpoint 4 is used for isochronous data transfer
    // --------------------------------------------------------
    {
      sizeof(USB_endpoint_desc_t),
      ENDPOINT_DESCRIPTOR,
      {
        ENDPOINT_4,
        0,
        D_OUT
      },
      ISOCHRONOUS_ENDPOINT,
      SWAP( RX_ISO_EP_FIFO_SIZE ),     //  Max Packet Size
      1                                //  Irrelevant
    },
    // -----------------------------------------------------
    // The IN endpoint 5 is used for interrupt data transfer
    // -----------------------------------------------------
    {
      sizeof(USB_endpoint_desc_t),
      ENDPOINT_DESCRIPTOR,
      {
        ENDPOINT_5,
        0,
        D_IN
      },
      INTERRUPT_ENDPOINT,
      SWAP( TX_INTR_EP_FIFO_SIZE ), //  Max Packet Size
      0x1                           //  Interrupt Interval, 1ms
    }
  }
#endif
};

// ----------------------------
// List of endpoint descriptors
// ---------------------------
CONST USB_endpoint_desc_t CONST * usb_dev_endpoints[ALTSET_NUM][7] =
{
  {
    0,                                                              //  Endpoint 0
    &usb_dev_long_config_desc.usb_dev_endpoint_alt_0_desc[0],       //  Endpoint 1
    &usb_dev_long_config_desc.usb_dev_endpoint_alt_0_desc[1],       //  Endpoint 2
    0,                                                              //  Endpoint 3
    0,                                                              //  Endpoint 4
    0,                                                              //  Endpoint 5
    0                                                               //  Endpoint 6
  },
#if ALTSET_NUM==2
  {
    0,                                                              //  Endpoint 0
    &usb_dev_long_config_desc.usb_dev_endpoint_alt_1_desc[0],       //  Endpoint 1
    &usb_dev_long_config_desc.usb_dev_endpoint_alt_1_desc[1],       //  Endpoint 2
    &usb_dev_long_config_desc.usb_dev_endpoint_alt_1_desc[2],       //  Endpoint 3
    &usb_dev_long_config_desc.usb_dev_endpoint_alt_1_desc[3],       //  Endpoint 4
    &usb_dev_long_config_desc.usb_dev_endpoint_alt_1_desc[4],       //  Endpoint 5
    0                                                               //  Endpoint 6
  }
#endif
};

//****************************************************************
//
//                            String Descriptors
//
//****************************************************************

// -------------------
// String descriptors
// ------------------
CONST struct
{
  UINT8        bLength;
  UINT8        bDescriptorType;
  UINT16      bstring;
} langid_str_desc =
  {
    sizeof(langid_str_desc),
    STRING_DESCRIPTOR,
    ENGLISH_US
  };

CONST struct
{
  UINT8        bLength;
  UINT8        bDescriptorType;
  char         bstring[MANUFACTURER_STR_S];
} manufacturer_str_desc =
  {
    sizeof(manufacturer_str_desc),
    STRING_DESCRIPTOR,
    MANUFACTURER_STR
  };

CONST struct
{
  UINT8        bLength;
  UINT8        bDescriptorType;
  char         bstring[PRODUCT_STR_S];
} product_str_desc =
  {
    sizeof(product_str_desc),
    STRING_DESCRIPTOR,
    PRODUCT_STR
  };

CONST struct
{
  UINT8        bLength;
  UINT8        bDescriptorType;
  char         bstring[SERIAL_STR_S];
} serial_str_desc =
  {
    sizeof(serial_str_desc),
    STRING_DESCRIPTOR,
    SERIAL_STR
  };

// ------------------------------------------------------
// List of all string descriptors, Be sure that the order
// of the list is the same as String_index_t enum
// ------------------------------------------------------
CONST USB_string_desc_t CONST * string_descs[] =
{
  (USB_string_desc_t CONST *)&langid_str_desc,
  (USB_string_desc_t CONST *)&manufacturer_str_desc,
  (USB_string_desc_t CONST *)&product_str_desc,
  (USB_string_desc_t CONST *)&serial_str_desc
};

//----------------------------------------------------------------
// Function :   build_string_desc
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC VOID build_string_desc( USB_string_desc_t * dsc, UINT8 * dsc_buff )
{
  INT16   i;
  UINT8 * dsc_buff_p;

  dsc_buff[0] = dsc->bLength;
  dsc_buff[1] = dsc->bDescriptorType;

  // -------------------------------
  // make the string UNICODE encoded
  // each char is two bytes long
  // -------------------------------
  dsc_buff_p = (UINT8 *)(dsc_buff+2);
  for ( i=0;i<dsc->bLength; i++ )
  {
    *dsc_buff_p++ = dsc->bstring[i];
    *dsc_buff_p++ = 0x00;
  }
}

//****************************************************************
//
//             Standard & Class device request handlers
//
//     requests' sequence is according to USB 1.1 spec values
//
//****************************************************************

CONST USB_req_handler_t usb_std_device_req[] =
{
  usb_dev_get_status,
  usb_dev_clear_feature,
  USB_req_reserved,
  usb_dev_set_feature,
  USB_req_reserved,
  usb_dev_set_address,
  usb_std_dev_get_descriptor,
  usb_std_dev_set_descriptor,
  usb_dev_get_config,
  usb_dev_set_config,
  usb_dev_get_interface,
  usb_dev_set_interface,
  usb_dev_sync_frame,
  USB_req_reserved,
  USB_req_reserved,
  USB_req_reserved
};

extern VOID MSBOT_ResetATA     ( USB_request_t * req );
extern VOID MSBOT_GetMaxLUN    ( USB_request_t * req );

CONST USB_req_handler_t usb_class_device_req[] =
{
  MSBOT_ResetATA,
  MSBOT_GetMaxLUN
};

//****************************************************************
//
//          Standard USB device request handlers
//
//                 USB Chapter 9 requests
//
//****************************************************************

//----------------------------------------------------------------
// Function :   usb_dev_get_status
// Purpose  :   Handles the GET_STATUS device request from the USB host.
//
//              The GET_STATUS device request can go to one of three recipients:
//              the device, the current interface, or a particular endpoint.
//              1. The device returns its self-powered status and its remote
//                 wake-up status.
//              2. The current interface returns, as defined in USB spec, 0.
//              3. The selected endpoint returns its stall status.
//
// Inputs   :   req - pointer to struct of the received USB request
// Outputs  :   None.
// Returns  :   None.
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC VOID usb_dev_get_status( USB_request_t * req )
{
  UINT8 ep_no = REQ_INDEX(req).endpoint.ep_num;

  if ( DEVICE_STATE(device_status) == DEV_ATTACHED )
  {
    USB_STALL_EP0();
    return;
  }
  if ( IS_REQ_VALUE_NOT_ZERO(req) )
  {
    USB_STALL_EP0();
    return;
  }
  if ( REQ_LENGTH(req) != 2 )
  {
    USB_STALL_EP0();
    return;
  }
  // -----------------------
  // Clear all reserved bits
  // -----------------------
  device_buffers.status.msb.as_byte = 0;
  switch ( REQ_RECIPIENT(req) )
  {
  case DEVICE_REQ:
    if ( REQ_INDEX(req).as_bytes.lsb != 0 )
    {
      // -------------------------------------
      // device's behaviour is not specified
      // ------------------------------------
      USB_STALL_EP0();
      return;
    }
    // ---------------------------------
    // disable to request remote wakeup
    // -------------------------------
    device_buffers.status.msb.device.wakeup = OFF;
    // ------------
    // bus powered
    // -----------
    device_buffers.status.msb.device.selfpowered = OFF;
    break;
  case INTERFACE_REQ:
    if ( REQ_INDEX(req).interface.inf_no != 0 )
    {
      // ------------------------------------
      // device's behaviour is not specified
      // ------------------------------------
      USB_STALL_EP0();
      return;
    }
    // ----------
    // Reserved
    // ---------
    device_buffers.status.msb.interface.value = 0;
    break;
  case ENDPOINT_REQ:
    switch ( ep_no )
    {
    case ENDPOINT_0:
      device_buffers.status.msb.endpoint.stalled = (IS_EP0_STALLED)? ON : OFF;
      break;
    case ENDPOINT_1:
    case ENDPOINT_2:
    case ENDPOINT_3:
    case ENDPOINT_4:
    case ENDPOINT_5:
      device_buffers.status.msb.endpoint.stalled = (IS_EP_STALLED(ep_no))? ON : OFF;
      break;
      // ----------------------
      // endpoints not in use
      // ---------------------
    case ENDPOINT_6:
    default:
      // --------------------
      // undefined endpoint
      // --------------------
      USB_STALL_EP0();
      return;
    }
    break;
  case OTHER_REQ:
  default:
    // ---------------------
    // undefined recipient
    // -------------------
    USB_STALL_EP0();
    return;
  }
  // ---------
  // Reserved
  // ---------

#ifdef DEBUG
  Printf( "\nStat: %X", sizeof(USB_device_status_t) );
#endif

  device_buffers.status.lsb = 0;
  send_control_data( (UINT8 *)&device_buffers.status, NULL, sizeof(USB_device_status_t));
}

//----------------------------------------------------------------
// Function :   usb_dev_clear_feature
// Purpose  :   Handles the CLEAR_FEATURE device request from the USB host.
//
//              The CLEAR_FEATURE device request can go to one of two recipients:
//              the device, or a particular endpoint.
//              1. The device returns a 0 length packet to complete the handshake
//                 with the host.
//              2. The selected endpoint only respond to clear stall commands.
//
// Inputs   :   req - pointer to struct of USB request
// Outputs  :   None.
// Returns  :   None.
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC VOID usb_dev_clear_feature( USB_request_t * req )
{
  UINT8 ep_no = REQ_INDEX(req).endpoint.ep_num;

  if ( DEVICE_STATE(device_status) == DEV_ATTACHED )
  {
    USB_STALL_EP0();
    return;
  }
  if ( REQ_LENGTH(req) != 0 )
  {
    USB_STALL_EP0();
    return;
  }

  switch ( REQ_RECIPIENT(req) )
  {
  case DEVICE_REQ:
    // --------------------------------
    // wakeup feature is not supported
    // ------------------------------
    if ( REQ_VALUE(req).feature.bSelector == DEVICE_REMOTE_WAKEUP )
      USB_STALL_EP0();
    break;
  case INTERFACE_REQ:
    break;
  case ENDPOINT_REQ:
    // -------------------------------------------
    // clear stall state of appropriate endpoint
    // -----------------------------------------
    if ( REQ_VALUE(req).feature.bSelector != ENDPOINT_STALL ||
         ep_no >= ENDPOINT_LAST || usb_dev_endpoints[device_status.curAltSetting][ep_no] == 0 )
      USB_STALL_EP0();
    else
    {
#ifdef DEBUG
      Printf( "\nClrF" );
#endif

      (ep_no == ENDPOINT_0) ? CLEAR_STALL_EP0 : CLEAR_STALL_EP(ep_no);
      endpoint_stat[ ep_no ]->stalled = FALSE;
      zero_length_data_response(ENDPOINT_0);
    }
    break;
  case OTHER_REQ:
  default:
    USB_STALL_EP0();
    break;
  }
}

//----------------------------------------------------------------
// Function :   usb_dev_set_feature
// Purpose  :   Handles the SET_FEATURE device request from the USB host.
//
//              The SET_FEATURE device request can go to one of two recipients:
//              the device, or a particular endpoint.
//              1. The device responds to set remote wake-up commands
//              2. The selected endpoint responds to set stall commands.
//
// Inputs   :   req - pointer to struct of USB request
// Outputs  :   None.
// Returns  :   None.
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC VOID usb_dev_set_feature( USB_request_t * req )
{
  UINT8 ep_no = REQ_INDEX(req).endpoint.ep_num;

  if ( DEVICE_STATE(device_status) == DEV_ATTACHED )
  {
    USB_STALL_EP0();
    return;
  }
  if ( REQ_LENGTH(req) != 0 )
  {
    USB_STALL_EP0();
    return;
  }

  switch ( REQ_RECIPIENT(req) )
  {
  case DEVICE_REQ:
    if ( REQ_VALUE(req).feature.bSelector == DEVICE_REMOTE_WAKEUP )
      // --------------------------------
      // remote wakeup is not supported
      // -------------------------------
      USB_STALL_EP0();
    break;
  case INTERFACE_REQ:
    break;
  case ENDPOINT_REQ:
    if ( REQ_VALUE(req).feature.bSelector != ENDPOINT_STALL ||
         ep_no >= ENDPOINT_LAST || usb_dev_endpoints[device_status.curAltSetting][ep_no] == 0 )
      USB_STALL_EP0();
    else
    {
#ifdef DEBUG
      Printf( "\nSetF" );
#endif

      // -----------------------------------------
      // set appropriate endpoint to stall state
      // ---------------------------------------
      (ep_no == ENDPOINT_0)? USB_STALL_EP0() : STALL_EP(ep_no);
//      endpoint_stat[ ep_num ]->stalled = TRUE;
      zero_length_data_response(ENDPOINT_0);
    }
    break;
  case OTHER_REQ:
  default:
    USB_STALL_EP0();
    break;
  }
}

//----------------------------------------------------------------
// Function :   usb_dev_set_address
// Purpose  :   Handles the SET_ADDRESS device request from the USB host.
//
// Inputs   :   req - pointer to struct of USB request
// Outputs  :   None.
// Returns  :   None.
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC VOID usb_dev_set_address( USB_request_t * req )
{
  UINT8 address;

  if ( IS_REQ_INDEX_NOT_ZERO(req) )
  {
    // -----------------------------------------
    // behavior of the device is not specified
    // ---------------------------------------
    USB_STALL_EP0();
    return;
  }

  if ( REQ_LENGTH(req) != 0x0 )
  {
    // ----------------------------------------
    // behavior of the device is not specified
    // ---------------------------------------
    USB_STALL_EP0();
    return;
  }

  if ( DEVICE_STATE(device_status) == DEV_CONFIGURED )
  {
    // ---------------------------------------
    // behavior of the device is not specified
    // --------------------------------------
    USB_STALL_EP0();
    return;
  }

  switch ( REQ_RECIPIENT(req) )
  {
  case DEVICE_REQ:
    // ------------------------
    // ENABLE_DEFAULT_ADDRESS;
    // -----------------------
    write_usb(EPC0, read_usb(EPC0) | DEF);
    // -------------------------------------------------------
    // The setting of device address is delayed in order to
    // complete successfully the Status stage of this request
    // ------------------------------------------------------
    address = REQ_VALUE(req).as_bytes.lsb;
    // -----------------------
    // set new device address
    // ----------------------
    write_usb(FAR, address|AD_EN);
    // ---------------------------------
    // Enable answer to the set address
    // --------------------------------
    if ( REQ_VALUE(req).as_bytes.lsb == 0x0 )
      SET_DEVICE_STATE(device_status, DEV_ATTACHED);
    else
    {
      SET_DEVICE_STATE(device_status, DEV_ADDRESS);
      // ------------------
      // Set usb board leds
      // ------------------
//      SET_ID_LEDS(GET_DIP_SW1());
//      SET_USB_LED();
    }

#ifdef DEBUG
    Printf( "\nSetA" );
#endif

    // ---------------------------------
    // enable zero length data responce
    // ---------------------------------
    usbn9604_tx_enable(ENDPOINT_0);
    break;
  case INTERFACE_REQ:
  case ENDPOINT_REQ:
  case OTHER_REQ:
  default:
    USB_STALL_EP0();
    break;
  }
}

//----------------------------------------------------------------
// Function :   usb_std_dev_get_descriptor
// Purpose  :   Handles the standard GET_DESCRIPTOR device request
//              from the USB host.
//
// Inputs   :   req - pointer to struct of USB request
// Outputs  :   None.
// Returns  :   None.
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC VOID usb_std_dev_get_descriptor( USB_request_t * req )
{
  INT16         desc_length = 0;
  INT16         desc_index = REQ_VALUE(req).descriptor.bDescriptorIndex;
  UINT8 CONST * desc_buf = NULL;
  INT16         max_desc_length = REQ_LENGTH(req);

  switch ( REQ_RECIPIENT(req) )
  {
  case DEVICE_REQ:
    switch ( REQ_VALUE(req).descriptor.bDescriptorType )
    {
    case DEVICE_DESCRIPTOR:
      desc_length = usb_device_desc.bLength;
      desc_buf = (UINT8 CONST *)&usb_device_desc;
      break;
    case CONFIG_DESCRIPTOR:
      desc_length = usb_dev_long_config_desc.usb_dev_config_desc.wTotalLength;
      desc_buf = (UINT8 CONST * )&usb_dev_long_config_desc;
      break;
    case STRING_DESCRIPTOR:
      // ---------------------------------------------------------------
      // String index 0 for all languages returns an array of two-UINT8
      // LANGID codes supported by the device
      // -------------------------------------------------------------
      if ( desc_index < STR_LAST_INDEX && desc_index >= 0 )
      {
        // ---------------------------------------------------------
        // If the descriptor is longer than the wLength field,
        // only the initial bytes of the descriptor are returned.
        // If the descriptor is shorter than the wLength field,
        // the device indicates the end of the control transfer
        // by sending NULL character
        // -----------------------------------------------------

        desc_length = string_descs[desc_index]->bLength;
        desc_buf = (UINT8 CONST *)string_descs[desc_index];
        break;
      }
      // -----------------------------------------------
      // In case of the wrong string index, stall EP0
      // ---------------------------------------------
    default:
      USB_STALL_EP0();
      return;
    }
    desc_length = (desc_length < max_desc_length)? desc_length : max_desc_length;
    if ( desc_length%EP0_FIFO_SIZE )
      // -------------------------------------
      // zero lada length will not be required
      // --------------------------------------
      device_buffers.zero_data=0;
    else
      // ----------------------------------
      // zero lada length will be required
      // ----------------------------------
      device_buffers.zero_data=1;

#ifdef DEBUG
    Printf( "\nDesc: %X", REQ_VALUE(req).descriptor.bDescriptorType );
#endif

    send_control_data( NULL, desc_buf, desc_length );

    break;
  case INTERFACE_REQ:
  case ENDPOINT_REQ:
  case OTHER_REQ:
  default:
    USB_STALL_EP0();
    return;
  }
}

//----------------------------------------------------------------
// Function :   usb_std_dev_set_descriptor
// Purpose  :   Handles the SET_DESCRIPTOR device request from the USB host.
//              Not supported request. Stall Endpoint Zero.
//
// Inputs   :   req - pointer to struct of USB request
// Outputs  :   None.
// Returns  :   None.
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC VOID usb_std_dev_set_descriptor( USB_request_t * req )
{
  req = req;
  USB_STALL_EP0();
}

//----------------------------------------------------------------
// Function :   usb_dev_get_config
// Purpose  :   Handles the GET_CONFIGURATION device request from the USB host.
//
// Inputs   :   req - pointer to struct of USB request
// Outputs  :   None.
// Returns  :   None.
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC VOID usb_dev_get_config( USB_request_t * req )
{
  if ( DEVICE_STATE(device_status) == DEV_ATTACHED )
  {
    USB_STALL_EP0();
    return;
  }
  // -------------------------------------------
  // wValue, wIndex, wLength must be zero.
  // Otherwize device behavior is not specified.
  // --------------------------------------------
  if ( IS_REQ_VALUE_NOT_ZERO(req) )
  {
    USB_STALL_EP0();
    return;
  }
  if ( IS_REQ_INDEX_NOT_ZERO(req) )
  {
    USB_STALL_EP0();
    return;
  }
  if ( REQ_LENGTH(req) != 0x1 )
  {
    USB_STALL_EP0();
    return;
  }
  switch ( REQ_RECIPIENT(req) )
  {
  case DEVICE_REQ:

#ifdef DEBUG
    Printf( "\nCfg: %X", sizeof(device_buffers.state) );
#endif

    // ----------------------------------------
    // returns configuration number as appears
    // at the device configuration register
    // ---------------------------------------
    device_buffers.state = (DEVICE_STATE(device_status) == DEV_CONFIGURED)?
                           usb_dev_long_config_desc.usb_dev_config_desc.bConfigurationValue : 0;
    send_control_data( (UINT8 *)&device_buffers.state, NULL, sizeof(device_buffers.state) );
    break;
  case INTERFACE_REQ:
  case ENDPOINT_REQ:
  case OTHER_REQ:
  default:
    USB_STALL_EP0();
    break;
  }
}

//----------------------------------------------------------------
// Function :   usb_dev_set_config
// Purpose  :   Handles the SET_CONFIGURATION device request from the USB host.
//
// Inputs   :   req - pointer to struct of USB request
// Outputs  :   None.
// Returns  :   None.
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC VOID usb_dev_set_config( USB_request_t * req )
{
  INT16   i;

  // -------------------------------------------
  // wValue, wIndex, wLength must be zero.
  // Otherwize device behavior is not specified.
  //--------------------------------------------

  if ( IS_REQ_INDEX_NOT_ZERO(req) )
  {
    USB_STALL_EP0();
    return;
  }
  if ( REQ_LENGTH(req) != 0 )
  {
    USB_STALL_EP0();
    return;
  }
  switch ( REQ_RECIPIENT(req) )
  {
  case DEVICE_REQ:
    if ( DEVICE_STATE(device_status) == DEV_ATTACHED )
    {
      USB_STALL_EP0();
      break;
    }
    if ( REQ_VALUE(req).as_bytes.lsb == 0 )
    {
      // --------------------------------------------------------
      // The zero value places the device in unconfigured state
      // -------------------------------------------------------
      if ( DEVICE_STATE(device_status) == DEV_CONFIGURED )
      {

        // ----------------------------------
        // Deactivate current configuration
        // ---------------------------------
        for ( i=1; i<MAX_NUM_OF_ENDPOINTS; i++ )
        {
          if ( usb_dev_endpoints[device_status.curAltSetting][i] != 0 )
            usb_dev_disable_ep(usb_dev_endpoints[device_status.curAltSetting][i]);
        }

#ifdef DEBUG
        Printf( "\nSetC0" );
#endif

        SET_DEVICE_STATE(device_status, DEV_ADDRESS);
        zero_length_data_response(ENDPOINT_0);
      }
      else
        USB_STALL_EP0();
    }
    else if ( REQ_VALUE(req).as_bytes.lsb == usb_dev_long_config_desc.usb_dev_config_desc.bConfigurationValue )
    {
      // ----------------------------------
      // Deactivate previous configuration
      // ---------------------------------
      for ( i=1; i<MAX_NUM_OF_ENDPOINTS; i++ )
      {
        if ( usb_dev_endpoints[device_status.curAltSetting][i] != 0 )
          usb_dev_disable_ep(usb_dev_endpoints[device_status.curAltSetting][i]);
      }
      device_status.curAltSetting = 0;
      // ---------------------------
      // Activate this configuration
      // ---------------------------
      for ( i=0; i<MAX_NUM_OF_ENDPOINTS; i++ )
        if ( usb_dev_endpoints[device_status.curAltSetting][i] != 0 )
        {
          usb_dev_enable_ep(usb_dev_endpoints[device_status.curAltSetting][i]);
        }

#ifdef DEBUG
      Printf( "\nSetC1" );
#endif

      endpoint_status_init();
      SET_DEVICE_STATE(device_status, DEV_CONFIGURED);
      zero_length_data_response(ENDPOINT_0);
    }
    else
      // ---------------------------
      // wrong configuration value
      // -------------------------
      USB_STALL_EP0();
    break;
  case INTERFACE_REQ:
  case ENDPOINT_REQ:
  case OTHER_REQ:
  default:
    // --------------------------------------------------------
    // If configuration value is unsupported STALL Enpoint Zero
    // ---------------------------------------------------------
    USB_STALL_EP0();
    break;
  }
}

//----------------------------------------------------------------
// Function :   usb_dev_get_interface
// Purpose  :   Handles the GET_INTERFACE device request from the USB host.
//
// Inputs   :   req - pointer to struct of USB request
// Outputs  :   None.
// Returns  :   None.
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC VOID usb_dev_get_interface( USB_request_t * req )
{
  UINT8 interface_no  = REQ_INDEX(req).as_bytes.lsb;

  if ( DEVICE_STATE(device_status) != DEV_CONFIGURED )
  {
    USB_STALL_EP0();
    return;
  }

  if ( interface_no != 0 )
  {
    // ------------------------------------
    // only single interface is supported
    // -----------------------------------
    USB_STALL_EP0();
    return;
  }
  if ( IS_REQ_VALUE_NOT_ZERO(req) )
  {
    USB_STALL_EP0();
    return;
  }
  if ( REQ_LENGTH(req) != 0x1 )
  {
    USB_STALL_EP0();
    return;
  }

  switch ( REQ_RECIPIENT(req) )
  {
  case INTERFACE_REQ:

#ifdef DEBUG
    Printf( "\nInt: %X", sizeof(device_buffers.state) );
#endif

    device_buffers.state = device_status.curAltSetting;
    send_control_data( (UINT8 *)&device_buffers.state, NULL, sizeof(device_buffers.state) );
    break;
    //
    //  If Interface value is unsupported STALL Enpoint Zero
    //
  case DEVICE_REQ:
  case ENDPOINT_REQ:
  case OTHER_REQ:
  default:
    USB_STALL_EP0();
    break;
  }
}

//----------------------------------------------------------------
// Function :   usb_dev_set_interface
// Purpose  :   Handles the SET_INTERFACE device request from the USB host.
//
// Inputs   :   req - pointer to struct of USB request
// Outputs  :   None.
// Returns  :   None.
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC VOID usb_dev_set_interface( USB_request_t * req )
{
  UINT8         interface_no  = REQ_INDEX(req).as_bytes.lsb;
  UINT8         altSet = REQ_VALUE(req).as_bytes.lsb;
  INT16         i;

  if ( DEVICE_STATE(device_status) != DEV_CONFIGURED )
  {
    USB_STALL_EP0();
    return;
  }

  if ( interface_no != 0 )
  {
    // ------------------------------------
    // only single interface is supported
    // -----------------------------------
    USB_STALL_EP0();
    return;
  }
  if ( IS_REQ_VALUE_NOT_ZERO(req) )
  {
    USB_STALL_EP0();
    return;
  }
  if ( REQ_LENGTH(req) != 0 )
  {
    USB_STALL_EP0();
    return;
  }

  switch ( REQ_RECIPIENT(req) )
  {
  case INTERFACE_REQ:

    if ( altSet > (ALTSET_NUM-1) )
    {
      // ------------------------------------
      // Not supported Alternative setting
      // -----------------------------------
      USB_STALL_EP0();
      break;
    }
    if ( altSet != device_status.curAltSetting )
    {
      // ----------------
      // Change setting
      // ----------------

      // ----------------------------------
      // Deactivate previous setting
      // ---------------------------------
      for ( i=1; i<MAX_NUM_OF_ENDPOINTS; i++ )
      {
        if ( usb_dev_endpoints[device_status.curAltSetting][i] != 0 )
          usb_dev_disable_ep(usb_dev_endpoints[device_status.curAltSetting][i]);
      }
      device_status.curAltSetting = altSet;
      // ---------------------------
      // Activate this configuration
      // ---------------------------
      for ( i=0; i<MAX_NUM_OF_ENDPOINTS; i++ )
        if ( usb_dev_endpoints[device_status.curAltSetting][i] != 0 )
        {
          usb_dev_enable_ep(usb_dev_endpoints[device_status.curAltSetting][i]);
        }
      endpoint_status_init();
    }

#ifdef DEBUG
    Printf( "\nSetI" );
#endif

    zero_length_data_response(ENDPOINT_0);
    break;
    //
    //  If Interface value is unsupported STALL Enpoint Zero
    //
  case DEVICE_REQ:
  case ENDPOINT_REQ:
  case OTHER_REQ:
  default:
    USB_STALL_EP0();
    break;
  }
}

//----------------------------------------------------------------
// Function :   usb_dev_sync_frame
// Purpose  :   Handles the SYNCH_FRAME device request from the USB host.
//
//              The SYNCH_FRAME device request is not a supported device request
//              and stalls endpoint 0 immediately.
//
// Inputs   :   req - pointer to struct of USB request
// Outputs  :   None.
// Returns  :   None.
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC VOID usb_dev_sync_frame( USB_request_t * req )
{
  req = req;
  USB_STALL_EP0();
}

//----------------------------------------------------------------
// Function :   USB_req_reserved
// Purpose  :   Place holder for undefined functions in Standard and Vendor device
//              request arrays. When it's called that wrong request has been received,
//              then stall endpoint 0. The input req is for compatibility only
//
// Inputs   :   req - pointer to struct of USB request
// Outputs  :   None.
// Returns  :   None.
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC VOID USB_req_reserved( USB_request_t * req )
{
  req = req;
  USB_STALL_EP0();
}

//----------------------------------------------------------------
// Function :   USB_req_undefined
// Notes    :
// History  :
//----------------------------------------------------------------

STATIC VOID USB_req_undefined( USB_request_t * req )
{
  // ----------------------------
  // undefined but don't stall;
  // ---------------------------
  req = req;
}

//----------------------------------------------------------------
// Function :   usb_dev_enable_ep
// Notes    :   Enables the IN/OUT endpoints
// History  :
//----------------------------------------------------------------

STATIC VOID usb_dev_enable_ep( USB_endpoint_desc_t CONST * ep )
{
  endpoint_t ep_no = ep->bEndpointAddress.address;

  if ( ep->bEndpointAddress.direction == D_IN )
    FLUSH_TXEP(ep_no);
  else
    FLUSH_RXEP(ep_no);

  // ---------------------------------
  // enable endpoint & set its address
  // ---------------------------------
  if ( ep->bmAttributes == ISOCHRONOUS_ENDPOINT )
    write_usb(EPC_ADDR(ep_no), (UINT8)ep_no | EP_EN | ISO) ;
  else
    write_usb(EPC_ADDR(ep_no), (UINT8)ep_no | EP_EN) ;

  if ( ep->bEndpointAddress.direction == D_OUT )
    ENABLE_RX(ep_no);
}

//----------------------------------------------------------------
// Function :   usb_dev_disable_ep
// Notes    :   Disables the IN/OUT endpoints
// History  :
//----------------------------------------------------------------

STATIC VOID usb_dev_disable_ep( USB_endpoint_desc_t CONST * ep )
{
  if ( ep->bEndpointAddress.direction == D_IN )
    FLUSH_TXEP(ep->bEndpointAddress.address);
  else
    FLUSH_RXEP(ep->bEndpointAddress.address);

  write_usb(EPC_ADDR(ep->bEndpointAddress.address),0);
}

//----------------------------------------------------------------
// Function :   USB_device_req_handler
// Notes    :   Handles the device requests from the USB host.
//
//              Check the request, if the request has at least one error, it stalls
//              endpoint 0. Calls the request specific function.
// History  :
//----------------------------------------------------------------

VOID USB_device_req_handler( VOID )
{
  UINT8         * msg = control_receive_buffer.dataRam;
  USB_request_t * req = (USB_request_t *)msg;

  if ( msg == NULL ) return;

#ifdef DEBUG
  Printf( "\nReq:%X Typ:%X Dev:%X,", REQ_RECIPIENT(req), REQ_TYPE(req), REQ_DEVICE(req) );
#endif
  {
    UINT8 i = control_receive_buffer.bytes_counter;
    while( i-- )
    {
#ifdef DEBUG
      Printf( " %X", *msg++ );
#endif
    }
  }

  switch ( REQ_RECIPIENT(req) )
  {
    case DEVICE_REQ:
      switch ( REQ_TYPE(req) )
      {
        case STANDARD_REQ:
          (*usb_std_device_req[REQ_DEVICE(req)])(req);
          device_status.last_req = REQ_DEVICE(req);
        break;
        case CLASS_REQ:
        case VENDOR_REQ:
        default:
          USB_STALL_EP0();
        break;
      };
    break;
    case INTERFACE_REQ:
      switch ( REQ_TYPE(req) )
      {
        case CLASS_REQ:
          switch( REQ_VENDOR(req) )
          {
            case 0xFF:
//            case 0x00:
              (*usb_class_device_req[0])(req);
            break;
            case 0xFE:
              (*usb_class_device_req[1])(req);
            break;
            default:
              USB_STALL_EP0();
            break;
          };
        break;
        case STANDARD_REQ:
        case VENDOR_REQ:
        default:
          USB_STALL_EP0();
        break;
      };
    break;
    case ENDPOINT_REQ:
      (*usb_std_device_req[REQ_DEVICE(req)])(req);
      device_status.last_req = REQ_DEVICE(req);
//      switch ( REQ_DEVICE(req) )
//      {
//        case CLEAR_FEATURE:
//#define ep_no  REQ_INDEX(req).endpoint.ep_num
//          ( ep_no == ENDPOINT_0) ? CLEAR_STALL_EP0 : CLEAR_STALL_EP(ep_no);
//          endpoint_stat[ ep_no ]->stalled = FALSE;
//#undef ep_no
//        break;
//        default:
//          USB_STALL_EP0();
//        break;
//      };
      break;
    default:
      USB_STALL_EP0();
    break;
  };
#ifdef DEBUG
//  Printf( " rOK" );
#endif
}

//----------------------------------------------------------------
// Function :   USB_STALL_EP0
// Notes    :
// History  :
//----------------------------------------------------------------

VOID USB_STALL_EP0( VOID )
{
#ifdef DEBUG
  Printf( " St0" );
#endif

  write_usb(EPC0, read_usb(EPC0) | STALL);
  ENABLE_TX( ENDPOINT_0 );
}

//----------------------------------------------------------------
// Function :   USB_STALL_EP0
// Notes    :
// History  :
//----------------------------------------------------------------

VOID USB_STALL_EPN( endpoint_t ep_num )
{
#ifdef DEBUG
  Printf( " St%d", ep_num );
#endif

  STALL_EP( ep_num );
  endpoint_stat[ ep_num ]->stalled = TRUE;
}

//----------------------------------------------------------------
// Function :   USB_EPN_STALLED
// Notes    :
// History  :
//----------------------------------------------------------------

BOOL USB_EPN_STALLED( endpoint_t ep_num )
{
  return endpoint_stat[ ep_num ]->stalled;
}

//      End
