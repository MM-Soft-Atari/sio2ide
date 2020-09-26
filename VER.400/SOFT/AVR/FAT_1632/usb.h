//****************************************************************
// Copyright (C), 2002 MMSoft, All rights reserved
//****************************************************************

//****************************************************************
//
// SOURCE FILE: USB.H
//
// MODULE NAME: USB
//
// PURPOSE:     USB related definitions.
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

#ifndef __USB_H__
  #define __USB_H__

//****************************************************************
//
// Definitions
//
//****************************************************************

  #define ALTSET_NUM       1             // Number of Alternate Settings

  #define CLKDIV           5
  #define CLKDIV_SLOW      12
  #define CLKDIV_FAST      5

  #define USB_SPEC_VERSION 0x0110         /* 1.1 */
//  #define NSC_ID_VENDOR    0x0EA0         /* assigned by USB Forum */
//  #define NSC_ID_PRODUCT   0x6803         /* assigned by NSC */
//  #define NSC_ID_VENDOR    0x0584         /* assigned by USB Forum */
//  #define NSC_ID_PRODUCT   0x0001         /* assigned by NSC */
  #define NSC_ID_VENDOR    0x0400         /* assigned by USB Forum */
  #define NSC_ID_PRODUCT   0x0001         /* assigned by NSC */
  #define NSC_BCDDEVICE    1              /* assigned by developer */

  #define MANUFACTURER_STR        \
  {                               \
    'M',0,                        \
    'M',0,                        \
    ' ',0,                        \
    'S',0,                        \
    'o',0,                        \
    'f',0,                        \
    't',0,                        \
    'w',0,                        \
    'a',0,                        \
    'r',0,                        \
    'e',0                         \
  }
  #define MANUFACTURER_STR_S    22

  #define PRODUCT_STR             \
  {                               \
    'U',0,                        \
    'S',0,                        \
    'B',0,                        \
    '-',0,                        \
    'I',0,                        \
    'D',0,                        \
    'E',0,                        \
    ' ',0,                        \
    'a',0,                        \
    'd',0,                        \
    'a',0,                        \
    'p',0,                        \
    't',0,                        \
    'e',0,                        \
    'r',0                         \
  }
  #define PRODUCT_STR_S         30

  #define VERSION_STR             \
  {                               \
    '1',0,                        \
    '_',0,                        \
    '1',0                         \
  }
  #define VERSION_STR_S         6

  #define SERIAL_STR              \
  {                               \
    '0',0,                        \
    '0',0,                        \
    '0',0,                        \
    '0',0,                        \
    '0',0,                        \
    '0',0,                        \
    '0',0,                        \
    '0',0,                        \
    '0',0,                        \
    '0',0,                        \
    '0',0,                        \
    '0',0                         \
  }
  #define SERIAL_STR_S          24

//****************************************************************
//
//  USB Endpoints/Interface Descriptors
//
//****************************************************************

typedef struct
{
  UINT8        bLength;
  UINT8        bDescriptorType;
  struct
  {
    UINT8      address:4;
    UINT8      reserved:3;
    UINT8      direction:1;
  } bEndpointAddress;
  UINT8        bmAttributes;
  UINT16       wMaxPacketSize;
  UINT8        bInterval;
} USB_endpoint_desc_t;

typedef struct
{
  UINT8        bLength;
  UINT8        bDescriptorType;
  UINT8        bInterfaceNumber;
  UINT8        bAlternateSetting;
  UINT8        bNumEndpoints;
  UINT8        bInterfaceClass;
  UINT8        bInterfaceSubClass;
  UINT8        bInterfaceProtocol;
  UINT8        iInterface;
} USB_interface_desc_t;

  #define MAX_NUM_OF_ENDPOINTS            6
  #define NUM_OF_ENDPOINTS_FOR_ALT_0      2 // current number of endpoints excluding
                                            // endpoint zero for default setting
  #define NUM_OF_ENDPOINTS_FOR_ALT_1      5 // current number of endpoints excluding
                                            // endpoint zero for 1 alternate setting.
                                            // This setting is used for the demo purpose
                                            // and contains all the endpoints (including Isochronus)

  #define CONFIG_DESC_LENGTH              0x9
  #define INTERFACE_DESC_LENGTH           0x9
  #define TOTAL_CONF_DESC_LEN CONFIG_DESC_LENGTH+INTERFACE_DESC_LENGTH+sizeof(USB_endpoint_desc_t)*CURR_NUM_OF_ENDPOINTS

typedef struct
{
  UINT8        bLength;
  UINT8        bDescriptorType;
  INT8       * bstring;
} USB_string_desc_t;

//
// The length of a string descriptor is size of a string and
// two bytes: bLength, bDescriptorType
//
  #define STRING_DESC_LENGTH              (2 * sizeof(UINT8))

typedef struct
{
  union
  {
    struct
    {
      UINT8     selfpowered:1;
      UINT8     wakeup:1;
      UINT8     :6;
    } device;
    struct
    {
      UINT8     value;
    } interface;
    struct
    {
      UINT8     stalled:1;
      UINT8     :7;
    } endpoint;
    UINT8       as_byte;
  } msb;
  UINT8         lsb;
} USB_device_status_t;

//
// The language ID (LANGID) defined by Microsoft as described
// in Developing Intrnational Software for Windows 95/NT.
// The LANGID is a 16-bit value that is the combination of a
// primary and sublanguage ID. The bits are:
//      0-9   Primary language ID
//      10-15 Sublanguage ID
// The enum is defined in little-endian order (the first UINT8 is lsb)
//
typedef enum
{
  NEUTRAL             = 0x0000,
  SYTEM_DEFAULT       = 0x0400,
  ENGLISH_NEUTRAL     = 0x0009,
  ENGLISH_US          = 0x0409,
  ENGLISH_UK          = 0x0809,
  RUSSIAN_NEUTRAL     = 0x0019
} LANGID_t;

  #define LANGID_LENGTH   2

//
//===============================================================
//
typedef enum
{
  DEV_DETACHED          = 0,
  DEV_ATTACH_WAITING,
  DEV_ATTACHED,
  DEV_ADDRESS,
  DEV_CONFIGURED
} USB_device_state_t;

typedef enum
{
  GET_STATUS            = 0,
  CLEAR_FEATURE         = 1,
  RESERVED_REQ          = 2,
  SET_FEATURE           = 3,
  SET_ADDRESS           = 5,
  GET_DESCRIPTOR        = 6,
  SET_DESCRIPTOR        = 7,
  GET_CONFIG            = 8,
  SET_CONFIG            = 9,
  GET_INTERFACE         = 10,
  SET_INTERFACE         = 11,
  SYNC_FRAME            = 12
} USB_device_Request_t;


typedef struct
{
  USB_device_state_t   state;
  USB_device_Request_t last_req;
  INT16                curAltSetting;
} DEVICE_status_t;

  #define MAX_STRING_LENGTH  0xFF

typedef union
{
  USB_device_status_t  status;         // used in GET_STATUS request
  UINT8                state;          // used in GET_CONFIGURATION request
  UINT8                zero_data;      // used while data sending
                                       //in GET_DESCRIPTOR request
  UINT8                desc;
} Device_buffers_t;

typedef enum
{
  VENDOR_USB_ID
} VENDOR_Request_t;


typedef enum
{
  STANDARD_REQ          = 0,
  CLASS_REQ             = 1,
  VENDOR_REQ            = 2
} USB_request_type_t;

typedef enum
{
  DEVICE_REQ            = 0,
  INTERFACE_REQ,
  ENDPOINT_REQ,
  OTHER_REQ
} USB_request_recipient_t;

typedef enum
{
  ENDPOINT_STALL        = 0,
  DEVICE_REMOTE_WAKEUP  = 1
} USB_feature_selector_t;

typedef enum
{
  D_OUT                 = 0,
  D_IN
} USB_ep_direction_t;

typedef struct
{
  UINT8         recipient:5;           // Request Recipient
  UINT8         type:2;                // Request Type
  UINT8         xfer:1;                // Data xfer direction
} bmRequestType_t;

typedef enum
{
  DEVICE_DESCRIPTOR     = 1,
  CONFIG_DESCRIPTOR,
  STRING_DESCRIPTOR,
  INTERFACE_DESCRIPTOR,
  ENDPOINT_DESCRIPTOR,
  USB_DEVICE_DESCRIPTOR = 0x21
} USB_descriptor_type_t;

typedef struct
{
  bmRequestType_t bmRequestType;
  union
  {
    UINT8       Device_req;
    UINT8       Vendor_req;
  } bRequest;
  union
  {
    struct
    {
      UINT8     bDescriptorIndex;   // bStringIndex;
      UINT8     bDescriptorType;
    } descriptor;
    struct
    {
      UINT8     bSelector;
      UINT8     msb;
    } feature;
    struct
    {
      UINT8     lsb;
      UINT8     msb;
    } as_bytes;
    UINT16      lsw;
  } wValue;
  union
  {
    struct
    {
      UINT8     ep_num:4;
      UINT8     :3;
      UINT8     direction:1;
      UINT8     :8;
    } endpoint;
    struct
    {
      UINT8     inf_no;
      UINT8     msb;
    } interface;
    struct
    {
      UINT8     lsb;
      UINT8     msb;
    } as_bytes;
    UINT16      msw;
  } wIndex;
  UINT16        wLength;
} USB_request_t;

  #define REQ_DIRECTION(req)      ((USB_xfer_direction_t)(req->bmRequestType.xfer))
  #define REQ_TYPE(req)           ((USB_request_type_t)(req->bmRequestType.type))
  #define REQ_RECIPIENT(req)      ((USB_request_recipient_t)(req->bmRequestType.recipient))
  #define REQ_DEVICE(req)         ((USB_device_Request_t)(req->bRequest.Device_req & 0x0F))
  #define REQ_VENDOR_TYPE(req)    ((req->bRequest.Vendor_req & 0xF0)>>4)
  #define REQ_VENDOR(req)         (req->bRequest.Vendor_req & 0xFF)
  #define REQ_VALUE(req)          (req->wValue)
  #define REQ_INDEX(req)          (req->wIndex)
  #define REQ_LENGTH(req)         (SWAP(req->wLength))


typedef enum
{
  CONTROL_ENDPOINT      = 0,
  ISOCHRONOUS_ENDPOINT,
  BULK_ENDPOINT,
  INTERRUPT_ENDPOINT
} Endpoint_type_t;

typedef enum
{
  CLASS_NOT_DEFINED     = 0x0,
  CLASS_AUDIO           = 0x01,
  CLASS_COMMUNICATION   = 0x02,
  CLASS_HID             = 0x03,
  CLASS_MONITOR         = 0x04,
  CLASS_PRINTING        = 0x07,
  CLASS_MASS_STORAGE    = 0x08,
  CLASS_HUB             = 0x09,
  CLASS_VENDOR          = 0xFF
} Device_class_t;

typedef enum
{
  SUBCLASS_CODE_RBC          = 0x01,
  SUBCLASS_CODE_SFF8020I     = 0x02,
  SUBCLASS_CODE_QIC157       = 0x03,
  SUBCLASS_CODE_UFI          = 0x04,
  SUBCLASS_CODE_SFF8070I     = 0x05,
  SUBCLASS_CODE_SCSI         = 0x06
} Interface_subclass_t;

typedef enum
{
  PROTOCOL_CODE_CBI0         = 0x00,
  PROTOCOL_CODE_CBI1         = 0x01,
  PROTOCOL_CODE_BULK         = 0x50
} Interface_protocol_t;

typedef enum
{
  BUS_POWERED           = 0x80,
  SELF_POWERED          = 0x40,
  REMOTE_WAKEUP         = 0x20
} Power_config_t;

typedef enum
{
  STR_LANGID            = 0,
  STR_MANUFACTURER      = 1,
  STR_PRODUCT           = 2,
  STR_SERIAL            = 3,
  STR_LAST_INDEX        = 4
} String_index_t;

typedef struct
{
  UINT8        bLength;
  UINT8        bDescriptorType;
  UINT16       bcdUSB;
  UINT8        bDeviceClass;
  UINT8        bDeviceSubClass;
  UINT8        bDeviceProtocol;
  UINT8        bMaxPacketSize;
  UINT16       idVendor;
  UINT16       idProduct;
  UINT16       bcdDevice;
  UINT8        iManufacturer;
  UINT8        iProduct;
  UINT8        iSerialNumber;
  UINT8        bNumConfigs;
} USB_device_desc_t;

typedef struct
{
  UINT8        bLength;
  UINT8        bDescriptorType;
  UINT16       wTotalLength;
  UINT8        bNumInterfaces;
  UINT8        bConfigurationValue;
  UINT8        iConfiguration;
  UINT8        bmAttributes;
  UINT8        MaxPower;
} USB_config_desc_t;

typedef struct
{
  // Device Config
  USB_config_desc_t       usb_dev_config_desc;
  // Interface 0
  USB_interface_desc_t    usb_interface_0_alt_0_desc;
  USB_endpoint_desc_t     usb_dev_endpoint_alt_0_desc[NUM_OF_ENDPOINTS_FOR_ALT_0];
#if ALTSET_NUM==2
  // Interface 1
  USB_interface_desc_t    usb_interface_0_alt_1_desc;
  USB_endpoint_desc_t     usb_dev_endpoint_alt_1_desc[NUM_OF_ENDPOINTS_FOR_ALT_1];
#endif
} USB_long_config_desc_t;

//****************************************************************
//
//  Interface
//
//****************************************************************

typedef VOID (*USB_req_handler_t)(USB_request_t *);

//
// External Parsers
//
//VOID BulkMSR_Parser         ( USB_request_t * req );

//
// Interface
//
BOOL USB_Init                          ( VOID );
VOID USB_device_req_handler            ( VOID );
VOID USB_STALL_EP0                     ( VOID );
VOID USB_STALL_EPN                     ( endpoint_t ep_num );
BOOL USB_EPN_STALLED                   ( endpoint_t ep_num );
VOID USB_reset                         ( VOID );
VOID USB_device_reset                  ( VOID );

#endif // __usb_h__

