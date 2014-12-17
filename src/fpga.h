// Copyright (c) 2014 Quanta Research Cambridge, Inc.
// Original author: John Ankcorn

// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use, copy,
// modify, merge, publish, distribute, sublicense, and/or sell copies
// of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
// BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#define DITEM(...) ((uint8_t[]){sizeof((uint8_t[]){ __VA_ARGS__ }), __VA_ARGS__})
#define INT16(A)           M(A), M((A) >> 8)
#define INT32(A)           INT16(A), INT16((A) >> 16)
#define BSWAP(A) ((((A) & 1) << 7) | (((A) & 2) << 5) | (((A) & 4) << 3) | (((A) & 8) << 1) \
         | (((A) & 0x10) >> 1) | (((A) & 0x20) >> 3) | (((A) & 0x40) >> 5) | (((A) & 0x80) >> 7))
#define SINT16(A)          BSWAP((A) >> 8), BSWAP(A)
#define SINT32(A)          SINT16((A) >> 16), SINT16(A)

/*
 * FTDI constants
 */
#define MWRITE    (MPSSE_LSB|MPSSE_WRITE_NEG)
#define DWRITE    (MPSSE_DO_WRITE | MWRITE)
#define DREAD     (MPSSE_DO_READ  | MPSSE_LSB|MPSSE_READ_NEG)

#define TMSW      (MPSSE_WRITE_TMS      |MWRITE|MPSSE_BITMODE)//4b
#define TMSRW     (MPSSE_WRITE_TMS|DREAD|MWRITE|MPSSE_BITMODE)//6f

#define DATAWBIT  (DWRITE|MPSSE_BITMODE)       //1b
#define DATARBIT  (DREAD |MPSSE_BITMODE)       //2e
#define DATARWBIT (DREAD |DWRITE|MPSSE_BITMODE)//3f
#define DATAW(READA, A) (DWRITE|(READA)), INT16((A)-1) //(0)->19 (DREAD)->3d
#define DATAR(A)        DREAD, INT16((A)-1) //2c

#define IDLE_TO_SHIFT_IR  "IS1100"      /* Idle -> Shift-IR */
#define IDLE_TO_SHIFT_DR  "ID100"       /* Idle -> Shift-DR */
#define EXIT1_TO_IDLE     "EI10"        /* Exit1/Exit2 -> Update -> Idle */
#define IDLE_TO_RESET     "IR111"       /* Idle -> Reset */
#define RESET_TO_IDLE     "RI0"         /* Reset -> Idle */
#define PAUSE_TO_SHIFT    "PS10"        /* Pause-DR -> Shift-DR */
#define FORCE_RETURN_TO_RESET "XR11111" /* go back to TMS reset state */
#define RESET_TO_SHIFT_DR "RD0100"      /* Reset -> Shift-DR */
#define SHIFT_TO_EXIT1    "SE1"         /* Shift-IR -> Exit1-IR */
#define SHIFT_TO_PAUSE    "SP10"        /* Shift-IR -> Pause-IR */
#define SHIFT_TO_UPDATE   "SU11"        /* Shift-DR -> Update-DR */
#define SHIFT_TO_IDLE     "SI110"       /* Shift-DR -> Update-DR -> Idle */

#define EXTEND_EXTRA            0xc0

/*
 * Xilinx constants
 */
#ifdef USE_LOGGING // when comparing to old libftdi output, use slower clock
#define CLOCK_FREQUENCY      15000000
#else
#define CLOCK_FREQUENCY      30000000
#endif

#define XILINX_IR_LENGTH         6 /* 6 bits in Xilinx IR register */
#define CORTEX_IR_LENGTH         4 /* 4 bits in ARM Cortex IR register: IHI0031C, 3.3.1 */
/* FPGA JTAG registers */
#define IRREG_USER2            0xc3
#define IRREG_CFG_OUT          0x04
#define IRREG_CFG_IN           0x05
#define IRREG_USERCODE         0xc8
#define IRREG_IDCODE           0x09
#define IRREG_JSHUTDOWN        0x0d
#define IRREG_JPROGRAM         0x0b
#define IRREG_JSTART           0x0c
#define IRREG_ISC_NOOP         0x14
#define IRREG_BYPASS           0x3f // even on PCIE, this has an extra bit
#define IRREG_BYPASS_EXTEND  0xffff // even on PCIE, this has an extra bit

/* Status values */
#define FIRST_TIME    ((found_cortex | jtag_index) ? 0x8a : 0x20)
#define INPROGRAMMING ((idcode_count > 1 && jtag_index == 0) ? 0x10 : 0x88)
#define PROGRAMMED    ((idcode_count > 1) ? 0xae : 0xbc)
#define FINISHED      ((idcode_count > 1 && jtag_index == 0) ? 0x5c : 0xac)

/*
 * Xilinx Configuration Packets
 *
 * In ug470_7Series_Config.pdf, this is described on pages 89ff.
 */

// Type 1 Packet, Table 5-17
#define CONFIG_TYPE_MASK     0xe0000000
#define CONFIG_TYPE1_OPCODE_SHIFT       27
#define CONFIG_TYPE1_OPCODE_MASK       0x3
#define CONFIG_TYPE1_REG_SHIFT          13
#define CONFIG_TYPE1_REG_MASK       0x3fff
#define CONFIG_TYPE1_WORDCNT_MASK    0x7ff

#define CONFIG_TYPE1(OPCODE,REG,COUNT) \
    SINT32(0x20000000 | ((OPCODE) << CONFIG_TYPE1_OPCODE_SHIFT) | ((REG) << CONFIG_TYPE1_REG_SHIFT) | (COUNT))

// Type 1 OPCODE Format, Table 5-18
#define CONFIG_OP_NOP         0
#define CONFIG_OP_READ        1
#define CONFIG_OP_WRITE       2
#define CONFIG_OP_RESERVED    3

// Type 1 Packet Registers, Table 5-20
#define CONFIG_REG_CRC     0x00
#define CONFIG_REG_FAR     0x01
#define CONFIG_REG_FDRI    0x02
#define CONFIG_REG_FDRO    0x03
#define CONFIG_REG_CMD     0x04  // CMD register, Table 5-22
#define     CONFIG_CMD_NULL     SINT32(0x00)
#define     CONFIG_CMD_WCFG            0x01
#define     CONFIG_CMD_MFW      SINT32(0x02)
#define     CONFIG_CMD_DGHIGH   SINT32(0x03)
#define     CONFIG_CMD_RCFG     SINT32(0x04)
#define     CONFIG_CMD_START    SINT32(0x05)
#define     CONFIG_CMD_RCAP     SINT32(0x06)
#define     CONFIG_CMD_RCRC     SINT32(0x07)
#define     CONFIG_CMD_AGHIGH   SINT32(0x08)
#define     CONFIG_CMD_SWITCH   SINT32(0x09)
#define     CONFIG_CMD_GRESTORE SINT32(0x0a)
#define     CONFIG_CMD_SHUTDOWN SINT32(0x0b)
#define     CONFIG_CMD_GCAPTURE SINT32(0x0c)
#define     CONFIG_CMD_DESYNC   SINT32(0x0d)  // end of configuration procedure
#define     CONFIG_CMD_IPROG    SINT32(0x0f)
#define     CONFIG_CMD_CRCC     SINT32(0x10)
#define     CONFIG_CMD_LTIMER   SINT32(0x11)
#define CONFIG_REG_CTL0    0x05
#define CONFIG_REG_MASK    0x06
#define CONFIG_REG_STAT    0x07  // STAT register, Table 5-25
#define CONFIG_REG_LOUT    0x08
#define CONFIG_REG_COR0    0x09
#define CONFIG_REG_MFWR    0x0a
#define CONFIG_REG_CBC     0x0b
#define CONFIG_REG_IDCODE  0x0c
#define CONFIG_REG_AXSS    0x0d
#define CONFIG_REG_COR1    0x0e
#define CONFIG_REG_WBSTAR  0x10
#define CONFIG_REG_TIMER   0x11
#define CONFIG_REG_BOOTSTS 0x16  // BOOTSTS register, Table 5-35
#define CONFIG_REG_CTL1    0x18

// Type 2 Packet (must follow a Type 1 packet and is used for long blocks)
//
#define CONFIG_TYPE2_RAW(LEN) (0x40000000 | (LEN))
#define CONFIG_TYPE2(LEN) SINT32(CONFIG_TYPE2_RAW(LEN))

// Constants used in accessing Configuration Registers
#define CONFIG_DUMMY           SINT32(0xffffffff)
#define CONFIG_SYNC            SINT32(0xaa995566)

/*
 * ARM Cortex constants
 * IHI0031C_debug_interface_as.pdf, Figure 7-1
 */
#define CORTEX_IDCODE 0x4ba00477

/* ARM JTAG-DP registers */
#define IRREGA_ABORT         0xf8   /* 35 bit register */
#define IRREGA_DPACC         0xfa   /* Debug Port access, 35 bit register */
#define IRREGA_APACC         0xfb   /* Access Port access, 35 bit register */
    #define AP_CSW           0                           /* MEM-AP registers */
    #define AP_TAR           2
    #define AP_DRW           6
#define IRREGA_IDCODE        0xfe   /* 32 bit register */
#define IRREGA_BYPASS        0xff

/* Cortex request extra 3 bit field */
/* 2 bits of register selector */
#define DPACC_CTRL     (1 << 1)
    // Coresight: Figure 2-14
    #define CORTEX_DEFAULT_STATUS 0xf0000001
    // CSYSPWRUPACK,CSYSPWRUPREQ,CDBGPWRUPACK,CDBGPWRUPREQ,ORUNDETECT
#define DPACC_SELECT   (2 << 1)
#define DPACC_RDBUFF   (3 << 1)
/* 1 bit of Write/nRead */
#define DPACC_WRITE        0x1

/* Cortex response extra 3 bit field */
#define DPACC_RESPONSE_OK 0x2

#define DEFAULT_CSW   0xe0000042
                      // Coresight: Table 2-20
                      // DbgStatus=1 -> AHB transfers permitted
                      // Size=2      -> 32 bits
#define SELECT_DEBUG  0x01000000

/* MEM-AP accessable registers */
// From DDI0388I_cortex_a9_r4p1_trm.pdf, Table 10-1
// Detailed description of each register is also in:
//   DDI0406B_arm_architecture_reference_manual_errata_markup_10_0.pdf (but no table!!)
#define DEBUG_REGISTER_BASE 0x80090000

#define DBGDIDR    0x000  /* ARMARM, C10.2.1: Debug ID */
#define DBGPRSR    0x314  /* ARMARM, C10.3.5: Device Power-down and Reset Status */
#define DBGITR     0x084  /* ARMARM, C10.4.3: Instruction Transfer */
#define DBGDSCRext 0x088  /* ARMARM, C10.3.1: Debug Status and Control */
#define DBGPCSR    0x028  /* ARMARM, C10.3.6: Program Counter Sampling */

/*
 * Zynq constants
 * (Addresses from ug585-Zynq-7000-TRM.pdf)
 */
#define ADDRESS_DEVCFG_MCTRL      0xf8007080
#define ADDRESS_SLCR_ARM_PLL_CTRL 0xf8000100
#define ADDRESS_SLCR_ARM_CLK_CTRL 0xf8000120
enum {DEVICE_OTHER=0, DEVICE_AC701=0x03636093, DEVICE_ZC706=0x03731093, DEVICE_ZC702=0x03727093, DEVICE_ZEDBOARD=0x83727093,
      DEVICE_VC707=0x03687093, DEVICE_KC705=0x03651093};

/*
 * Bit file constants
 */
#define BITFILE_ITEMSIZE (101 * sizeof(uint32_t))

#define SEND_SINGLE_FRAME     99999

void write_irreg(struct ftdi_context *ftdi, int read, int command, int flip, char *tail);
void write_creg(struct ftdi_context *ftdi, int regname);
void cortex_bypass(struct ftdi_context *ftdi, int cortex_nowait);
void process_command_list(struct ftdi_context *ftdi);
void write_bytes(struct ftdi_context *ftdi, uint8_t read_param,
    char *tail, uint8_t *ptrin, int size, int max_frame_size, int opttail, int swapbits, int default_ext);
int found_cortex;
void write_tail(char *tail);
void check_state(char required);
