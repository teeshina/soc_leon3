#include "lheaders.h"

//****************************************************************************
void AhbMaster::Update( uint32 inNRst,
                        SClock inClk,
                        // DMA request:
                        uint32 inDmaAdr,
                        uint32 inDmaData,//           : std_logic_vector(AHBDW-1 downto 0);
                        uint32 inDmaStart,//           : std_ulogic;
                        uint32 inDmaBurst,//           : std_ulogic;
                        uint32 inDmaWrite,//           : std_ulogic;
                        uint32 inDmaBusy,//            : std_ulogic;
                        uint32 inDmaIrq,//             : std_ulogic;
                        uint32 inDmaSize,//            : std_logic_vector(2 downto 0);
                        // DMA response:
                        uint32& outDmaStart,//           : std_ulogic;
                        uint32& outDmaActive,//          : std_ulogic;
                        uint32& outDmaReady,//           : std_ulogic;
                        uint32& outDmaRetry,//           : std_ulogic;
                        uint32& outDmaMexc,//            : std_ulogic;
                        uint32& outDmaAdr,//           : std_logic_vector(9 downto 0);
                        uint32& outDmaRdData,//           : std_logic_vector(AHBDW-1 downto 0);
                        // AHB bus response:
                        uint32 inAhbGrant,//[0:15]  : (0 to AHB_MASTERS_MAX-1);     -- bus grant
                        uint32 inAhbReady,//  ;                           -- transfer done
                        uint32 inAhbResponse,//[1:0] : (1 downto 0);   -- response type
                        uint32 inAhbRdData,//[31:0]  : (AHBDW-1 downto 0);   -- read data bus
                        // Request to AHB:
                        ahb_mst_out_type  &ahbo
                      )
{
  // Address control:
  wbAdrInc = (0x1<<inDmaSize);
  if(ADDR_INC>0)  wbNextAdr = (BITS32(inDmaAdr,9,0) + wbAdrInc) & MSK32(9,0);
  else            wbNextAdr = BITS32(inDmaAdr,9,0);
  
  if(inDmaStart & inDmaBurst & rActive.Q & !rRetry.Q)
  {
    wbCurAdr &= ~MSK32(9,0);
    wbCurAdr |= wbNextAdr;
  }else
    wbCurAdr = inDmaAdr;

  // state control:
  if(rRetry.Q | !inDmaStart)        wbTransferType = HTRANS_IDLE;
  else if(!rActive.Q | !inDmaBurst) wbTransferType = HTRANS_NONSEQ;
  else if(inDmaBusy)                wbTransferType = HTRANS_BUSY;
  else                              wbTransferType = HTRANS_SEQ;


  // Ready bit
  if(rActive.Q&&inAhbReady&&((inAhbResponse==HRESP_OKAY)||(inAhbResponse==HRESP_ERROR)))
    wReady = 1;
  else wReady = 0;

  // Retry bit:
  if(rActive.Q&&inAhbReady&&((inAhbResponse==HRESP_RETRY)||(inAhbResponse==HRESP_SPLIT)) )
    wRetry = 1;
  else wRetry = 0;

  // Error state occures:
  if(rActive.Q&&inAhbReady&&(inAhbResponse==HRESP_ERROR) )
    wErrResponse = 1;
  else wErrResponse = 0;


  
  // bus granted to "hindex" master:
  rGrant.CLK = inClk;
  if(inAhbReady) rGrant.D = BIT32(inAhbGrant,HMST_INDEX);
  
  // Address has been read by AHB then set Active=1:
  rActive.CLK = inClk;
  if(!inNRst)                                        rActive.D = 0;
  else if(inAhbReady&&(wbTransferType!=HTRANS_IDLE)) rActive.D = rGrant.Q;
  else if(inAhbReady)                                rActive.D = 0;

  // master start pulse:
  rStart.CLK = inClk;
  if(inAhbReady&&(wbTransferType!=HTRANS_IDLE)) rStart.D = rGrant.Q;
  else                                          rStart.D = 0;

  // retry strob:
  rRetry.CLK = inClk;
  if(!inNRst)                                                                                        rRetry.D = 0;
  else if(rActive.Q && !inAhbReady && ((inAhbResponse==HRESP_RETRY)||(inAhbResponse==HRESP_SPLIT)) ) rRetry.D = 1;
  else if(rActive.Q)                                                                                 rRetry.D = 0;

#if 1
  if((iClkCnt>=136)&&(inClk.eClock==SClock::CLK_POSEDGE))
    bool stop = true;
#endif
  
  ahbo.haddr   = wbCurAdr;
  ahbo.htrans  = wbTransferType;
  ahbo.hbusreq = inDmaStart;
  ahbo.hwdata  = inDmaData;
  ahbo.hconfig = hconfig;
  ahbo.hlock   = 0;
  ahbo.hwrite  = inDmaWrite;
  ahbo.hsize   = HSIZE_WORD;
  ahbo.hburst  = inDmaBurst ? HBURST_INCR: HBURST_SINGLE; // 3 bit width state value
  ahbo.hprot   = BITS32(PROTECT_BITS,3,0);               // non-cached supervise data
  ahbo.hirq    = inDmaIrq<<IRQ_BIT_INDEX;
  ahbo.hindex  = HMST_INDEX;

  outDmaStart   = rStart.Q;
  outDmaActive  = rActive.Q;
  outDmaReady   = wReady;
  outDmaMexc    = wErrResponse;
  outDmaRetry   = wRetry;
  outDmaAdr     = wbNextAdr;
  outDmaRdData  = inAhbRdData;

}


