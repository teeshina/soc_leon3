#include "headers.h"

void mctrl::Update( uint32 rst,
                    uint32 clk,
                    memory_in_type memi,//      : in  memory_in_type;
                    memory_out_type& memo,//      : out memory_out_type;
                    ahb_slv_in_type ahbsi,//     : in  ahb_slv_in_type;
                    ahb_slv_out_type& ahbso,//     : out ahb_slv_out_type;
                    apb_slv_in_type apbi,//      : in  apb_slv_in_type;
                    apb_slv_out_type& apbo,//      : out apb_slv_out_type;
                    wprot_out_type wpo,//       : in  wprot_out_type;
                    sdram_out_type& sdo//       : out sdram_out_type
                   )
{
  if(scantest&ahbsi.testen) arst = ahbsi.testrst;
  else                      arst = rst;

  // Variable default settings to avoid latches

  v = r.Q; 
  wprothitx = 0;
  v.ready8 = 0;
  v.iosn &= ~0x1; 
  v.iosn |= ((r.Q.iosn>>1)&0x1);

  ready = 0;
  addrerr = 0;
  regsd = 0;
  csen = 0;

  v.ready = 0;
  v.echeck = 0;
  //merrtype = "---";
  bready = 1;

  vbdrive = rbdrive.Q;
  vsbdrive = rsbdrive.Q; 

  v.data = memi.data;
  v.bexcn = memi.bexcn;
  v.brdyn = memi.brdyn;
  if (
      (((r.Q.brdyn & r.Q.mcfg1.brdyen)) & (((r.Q.area>>io)&0x1))) |
      ( ((r.Q.brdyn & r.Q.mcfg2.brdyen)) & (((r.Q.area>>ram)&0x1)) &
        (!((r.Q.ramsn>>4)&0x1)) & RAMSEL5 )
     )
    bready = 0;
  else bready = 1;

  v.hresp = HRESP_OKAY;

#ifdef SDRAM_ENABLE
  if (r.Q.hsel & !ahbsi.hready)
  {
    haddr = r.Q.haddr;
    hsize = r.Q.size;
    hburst = r.Q.hburst;
    htrans = r.Q.htrans;
    hwrite = r.Q.hwrite;
    area = r.Q.area;
  }
  if (fast)
  {
    sdhsel = ((ahbsi.hsel>>hindex)&0x1) & ((ahbsi.haddr>>SDRASEL)&0x1)
           & ((ahbsi.htrans>>1)&0x1) & ((ahbsi.hmbsel>>2)&0x1);
  }else
  {
    sdhsel = ((ahbsi.hsel>>hindex)&0x1) & ((ahbsi.htrans>>1)&0x1) & r.Q.mcfg2.sdren 
           & ((ahbsi.hmbsel>>2)&0x1) & (((ahbsi.haddr>>SDRASEL)&0x1) | r.Q.mcfg2.srdis);
  }
  srhsel = ((ahbsi.hsel>>hindex)&0x1) & !sdhsel;
#else
  haddr = ahbsi.haddr;
  hsize = ahbsi.hsize&0x3;
  hburst = ahbsi.hburst;
  htrans = ahbsi.htrans;
  hwrite = ahbsi.hwrite;
  area = ahbsi.hmbsel&0x7;//(0 to 2);

  sdhsel = 0;
  srhsel = ((ahbsi.hsel>>hindex)&0x1); 
#endif

// decode memory area parameters
  leadin = 0;
  //rws := "----"; wws := "----"; adec := "--";
  //busw = (others => '-');
  brmw = 0;
  if ((area>>rom)&0x1)
    busw = r.Q.mcfg1.romwidth;

  uint32 tmpMsk29_13 = 0x3FFFE000;  // sdrasel = 29
  haddrsel = 0; 
  haddrsel &= ~tmpMsk29_13;  // mask = [29:13]
  haddrsel |= (haddr&tmpMsk29_13);
  if ((area>>ram)&0x1)
  {
//    adec = genmux(r.Q.mcfg2.rambanksz, haddrsel(sdrasel downto 14)) &
//           genmux(r.Q.mcfg2.rambanksz, haddrsel(sdrasel-1 downto 13));
    adec = (haddrsel>>(13+r.Q.mcfg2.rambanksz))&0x3;

    if (sdhsel)
    {
      busw = 0x2;//"10";
    }else
    {
      busw = r.Q.mcfg2.ramwidth;
      if (
          (r.Q.mcfg2.rmw & hwrite) &&
          ( (BUS16EN&&(busw==0x1)&&(hsize==0)) || (((busw>>1)&0x1)&!((hsize>>1)&0x1)) )
         )
        brmw = 1;  // do a read-modify-write cycle
    }
  }
  if ((area>>io)&0x1)
  {
    leadin = 1;
    busw = r.Q.mcfg1.iowidth;
  }

// decode waitstates, illegal access and cacheability
  if ((r.Q.area>>rom)&0x1) 
  {
    rws = r.Q.mcfg1.romrws;
    wws = r.Q.mcfg1.romwws;
    if ((r.Q.mcfg1.romwrite|r.Q.read)==0) addrerr = 1;
  }
  if ((r.Q.area>>ram)&0x1)
  {
    rws = r.Q.mcfg2.ramrws;
    wws = r.Q.mcfg2.ramwws;
  }
  if((r.Q.area>>io)&0x1)
  {
    rws = r.Q.mcfg1.iows;
    wws = r.Q.mcfg1.iows;
    if (!r.Q.mcfg1.ioen) addrerr = 1;
  }

// generate data buffer enables
  bdrive = 0xF;
  switch(r.Q.busw)
  {
    case 0: if(BUS8EN) bdrive = 0x1; break;
    case 1: if(BUS16EN) bdrive = 0x3; break;
    default:;
  }

// generate chip select and output enable
  rams = adec ? (1<<adec) : 0;//decode(adec)
  switch(srbanks)
  {
    case 0: rams = 0x00; break;
    case 1: rams = 0x01; break;
    case 2: rams = ((rams>>2)&0x3) | (rams&0x3); break;
    default:
      if (RAMSEL5 & ((haddr>>SDRASEL)&0x1)) rams = 0x10;
  }

  iosn = 1;
  ramsn = 0x1F;
  romsn = 0x3;
  if ((area>>rom)&0x1)
    romsn = (((~haddr)>>(romasel-1))&0x2) | ((haddr>>romasel)&0x1);

  if((area>>ram)&0x1) ramsn = (~rams)&0x1F;
  if((area>>io)&0x1)  iosn = 0;

// generate write strobe
  wrn = 0;
  switch(r.Q.busw)
  {
    case 0: if (BUS8EN) wrn = 0xE; break;
    case 1:
      if(BUS16EN)
      {
        if ((r.Q.size == 0) && (r.Q.brmw == 0))
	        wrn = 0xC | ((!(r.Q.address&0x1))<<1) | (r.Q.address&0x1);
        else wrn = 0xC;
      }
    break;
    case 2: case 3:
      switch(r.Q.size)
      {
        case 0:
          switch(r.Q.address&0x3)
          {
            case 0:  wrn = 0xE; break;
            case 1:  wrn = 0xD; break;
            case 2:  wrn = 0xB; break;
            default: wrn = 0x7;
          }
        break;
        case 1:
          wrn = ((!((r.Q.address>>1)&0x1))<<3) | ((!((r.Q.address>>1)&0x1))<<2) 
              | (((r.Q.address>>1)&0x1)<<1) | ((r.Q.address>>1)&0x1);
        break;
        default:;
      }
    break;
    default:;
  }

  if ((r.Q.mcfg2.rmw)&((r.Q.area>>ram)&0x1)) wrn = !bdrive;

  if (ahbsi.hready & ((ahbsi.hsel>>hindex)&0x1) & ((ahbsi.htrans>>1)&0x1))
  {
    v.area = area;
    v.address  = haddr; 
    if ((busw == 0) && (!hwrite) && !((area>>io)&0x1) && BUS8EN)
      v.address &= ~0x3;
    if ((busw == 1) && (!hwrite) && !((area>>io)&0x1) && BUS16EN)
      v.address &= ~0x3;
    if (brmw) v.read = 1;
    else      v.read = !hwrite;
    v.busw = busw; 
    v.brmw = brmw;
  }

#ifdef SDRAM_ENABLE
  if (sdmo.aload & r.Q.hsel)
  {
    v.address  = haddr;
    if ((busw = 0x0) && (!hwrite) && !((area>>io)&0x1) && BUS8EN)
      v.address &= ~0x3;
    if ((busw = 0x1) && (!hwrite) && !((area>>io)&0x1) && BUS16EN)
      v.address &= ~0x3;
  }
#endif

// Select read data depending on bus width
  if (BUS8EN && (r.Q.busw == 0))
    memdata = (r.Q.readdata<<8) | ((r.Q.data>>24)&0xFF);
  else if(BUS16EN && (r.Q.busw == 1))
    memdata = (r.Q.readdata<<16) | ((r.Q.data>>16)&0xFFFF);
  else
    memdata = r.Q.data;

  bwdata = memdata;

  // Merge data during byte write
#if 0
  if(CORE_ACDM) ahbselectdata(ahbsi.hwdata, ((r.Q.address>>2)&0x7));
  else 
#endif
  writedata = ahbsi.hwdata;

  if (r.Q.brmw & ((r.Q.busw>>1)&0x1))
  {
    switch(r.Q.address&0x3)
    {
      case 0:
        writedata &= ~0xFFFF;
        writedata |= (bwdata&0xFFFF);
        if (r.Q.size == 0)
        {
          writedata &= ~0x00FF0000;
	        writedata |= (bwdata&0x00FF0000);
        }
      break;
      case 1:
        writedata &= ~0xFF00FFFF;
        writedata |= (bwdata&0xFF00FFFF);
      break;
      case 2:
        writedata &= ~0xFFFF0000;
        writedata |= (bwdata&0xFFFF0000);
        if (r.Q.size == 0)
        {
          writedata &= ~0x000000FF;
	        writedata |= (bwdata&0x000000FF);
        }
      break;
      default:
        writedata &= ~0xFFFFFF00;
        writedata |= (bwdata&0xFFFFFF00);
    }
  }
  if ((r.Q.brmw) && (r.Q.busw == 1) && BUS16EN)
  {
    if ((r.Q.address>>1)&0x1)
    {
      writedata &= ~0xFFFF0000;
      writedata |= ((writedata&0xFFFF)<<16);
    }
    if (!(r.Q.address&0x1))
    {
      writedata &= ~0x00FF0000;
      writedata |=  (r.Q.data&0x00FF0000);
    }else
    {
      writedata &= ~0xFF000000;
      writedata |=  (r.Q.data&0xFF000000);
    }
  }

  // save read data during 8/16 bit reads
  if(BUS8EN && r.Q.ready8 && (r.Q.busw == 0))
    v.readdata = (v.readdata<<8) | ((r.Q.data>>24)&0xFF);
  else if(BUS16EN && r.Q.ready8 && (r.Q.busw == 1))
    v.readdata = (v.readdata<<16) | ((r.Q.data>>16)&0xFFFF);


  // Ram, rom, IO access FSM
  if (r.Q.read) wsnew = rws;
  else          wsnew = wws;

  switch(r.Q.bstate)
  {
    case idle:
      v.ws = wsnew;
      if (r.Q.bdrive&0x1)
      {
        if((r.Q.busw>>1)&0x1)
        {
          v.writedata = (v.writedata&0x0000FFFF) | (writedata&0xFFFF0000);
        }else if (r.Q.busw == 1)
        {
          if (!((r.Q.address>>1)&0x1) | r.Q.brmw)
            v.writedata = (v.writedata&0x0000FFFF) | (writedata&0xFFFF0000);
          else
            v.writedata = (v.writedata&0x0000FFFF) | ((writedata&0x0000FFFF)<<16);
        }else
        {
          switch(r.Q.address&0x3)
          {
            case 0: v.writedata = (v.writedata&0x0000FFFF) | (writedata&0xFFFF0000); break;
            case 1: v.writedata = (v.writedata&0x00FFFFFF) | ((writedata&0x00FF0000)<<8); break;
            case 2: v.writedata = (v.writedata&0x0000FFFF) | ((writedata&0x0000FFFF)<<16); break;
            case 3: v.writedata = (v.writedata&0x00FFFFFF) | ((writedata&0x000000FF)<<24); break;
            default:;
          }
        }
        v.writedata = (v.writedata&0xFFFF0000) |  (writedata&0x0000FFFF);
        if (!((r.Q.busw>>1)&0x1))
          v.writedata8 = writedata&0xFF;
      }
#ifdef SDRAM_ENABLE
      if (r.Q.srhsel & !sdmo.busy)
#else
      if (r.Q.srhsel)
#endif
      {
        if (WPROTEN) wprothitx = wpo.wprothit;
        if (wprothitx | addrerr)
        {
	        v.hresp = HRESP_ERROR;
          v.bstate = berr; 
          v.bdrive = 0xF;
        }else if (!r.Q.read)
        {
	        if ((r.Q.busw == 0) && !((r.Q.area>>io)&0x1) && BUS8EN)
	          v.bstate = bwrite8;
	        else if ((r.Q.busw == 1) && !((r.Q.area>>io)&0x1) && BUS16EN)
	          v.bstate = bwrite16;
   	      else 
            v.bstate = bwrite;
	        v.wrn = wrn;
          v.writen = 0;
          v.bdrive = (~bdrive)&0xF;
        }else
        {
	        if (r.Q.oen)
          { 
            v.ramoen = r.Q.ramsn;
            v.oen = 0;
	        }else
          {
	          if ((r.Q.busw == 0) && !((r.Q.area>>io)&0x1) && BUS8EN)      v.bstate = bread8;
	          else if((r.Q.busw == 1) && !((r.Q.area>>io)&0x1) && BUS16EN) v.bstate = bread16;
	          else                                                         v.bstate = bread;
	        }
        }
      }
    break;
    case berr:
      v.bstate = idle;
      ready = 1;
      v.hresp = HRESP_ERROR;
      v.ramsn = 0x1F; 
      v.romsn = 0x3;
      v.ramoen = 0x1F;
      v.oen = 1;
      v.iosn = 0x3;
      v.bdrive = 0xF;
    break;
    case bread:
      if ((r.Q.ws == 0) && !r.Q.ready && bready)
      {
        if (!r.Q.brmw)
        {
	        ready = 1;
          v.echeck = 1;
          if (!((r.Q.area>>io)&0x1))
            v.address = ahbsi.haddr;
        }
        if ( !((ahbsi.hsel>>hindex)&0x1) || (ahbsi.htrans!=HTRANS_SEQ)
           || (r.Q.hburst == HBURST_SINGLE) || ((r.Q.area>>io)&0x1) )
        {
  	      v.ramoen = 0x1F;
          v.oen = 1;
          v.iosn = 0x3;
   	      v.bstate = idle;
          v.read = !r.Q.hwrite;
	        if (!r.Q.brmw)
          {
	          v.ramsn = 0x1F;
            v.romsn = 0x3;
	        }else
	          v.echeck = 1;
        }
      }
      if (r.Q.ready)
      {
        v.ws = rws;
      }else
      {
        if(r.Q.ws != 0) 
          v.ws = r.Q.ws - 1;
      }
    break;
    case bwrite:
      if ((r.Q.ws == 0) && bready)
      {
        ready = 1;
        v.wrn = 0xF;
        v.writen = 1;
        v.echeck = 1;
        v.ramsn = 0x1F;
        v.romsn = 0x3;
        v.iosn = 0x3;
        v.bdrive = 0xF;
        v.bstate = idle;
      }
      if (r.Q.ws != 0) v.ws = r.Q.ws - 1;
    break;
    case bread8:
      if (BUS8EN)
      {
        if ((r.Q.ws == 0) && !r.Q.ready8 && bready)
        {
	        v.ready8 = 1;
          v.ws = rws;
          v.address &= ~0x3;
	        v.address |= (((r.Q.address&0x3) + 1)&0x3);

          if ((r.Q.address&0x3) == 3)
          {
	          ready = 1;
            v.address = ahbsi.haddr;
            v.echeck = 1;

            if ( !((ahbsi.hsel>>hindex)&0x1) || (ahbsi.htrans != HTRANS_SEQ)
	              || (r.Q.hburst == HBURST_SINGLE) )
            {
  	          v.ramoen = 0x1F;
              v.oen = 1;
              v.iosn = 0x3;
   	          v.bstate = idle;
      	      v.ramsn = 0x1F;
              v.romsn = 0x3;
      	    }
          }
        }
        if (r.Q.ready8)       v.ws = rws;
        else if (r.Q.ws != 0) v.ws = r.Q.ws - 1;
      }else
      {
        v.bstate = idle;
      }
    break;
    case bwrite8:
      if (BUS8EN)
      {
        if ((r.Q.ws == 0) && !r.Q.ready8 && bready)
        {
	        v.ready8 = 1;
          v.wrn = 0xF;
          v.writen = 1;
        }

        uint32 tmpLsb = (r.Q.address&0x3);
        if( (r.Q.ws == 0) && bready
         && ( (tmpLsb == 3) || ((tmpLsb == 1) && (r.Q.size == 1)) || (r.Q.size == 0) )
          )
        {
      	  ready = 1;
          v.wrn = 0xF;
          v.writen = 1;
          v.echeck = 1;
	        v.ramsn = 0x1F;
          v.romsn = 0x3;
          v.iosn = 0x3;
	        v.bdrive = 0xF;
          v.bstate = idle;
        }
        if (r.Q.ready8)
        {
          v.address &= ~0x3;
	        v.address |= ((r.Q.address + 1)&0x3);
          v.ws = rws;
	        v.writedata = (v.writedata8&0xFFFF) 
            | ((r.Q.writedata & 0x00FF0000)<<8) | ((r.Q.writedata8 & 0xFF00)<<8);
	        v.writedata8 = (v.writedata8&0xFFFF00FF) | ((r.Q.writedata8&0xFF)<<8);
	        v.bstate = idle;
        }
        if (r.Q.ws != 0) v.ws = r.Q.ws - 1;
      }else
      {
        v.bstate = idle;
      }
    break;
    case bread16:
      if (BUS16EN)
      {
        if ( (r.Q.ws == 0) && bready && (((r.Q.address>>1)&0x1) | r.Q.brmw) && !r.Q.ready8 )
        {
	        if (!r.Q.brmw)
          {
	          ready = 1; 
            v.address = ahbsi.haddr;
            v.echeck = 1;
	        }
          if ( (!((ahbsi.hsel>>hindex)&0x1) || (ahbsi.htrans != HTRANS_SEQ)) 
	            || (r.Q.hburst == HBURST_SINGLE) )
          {
	          if (!r.Q.brmw)
            {
  	          v.ramsn = 0x1F;
              v.romsn = 0x3;
	          }
            v.ramoen = 0x1F;
            v.oen = 1;
            v.iosn = 0x3;
   	        v.bstate = idle;
            v.read = !r.Q.hwrite;
	        }
        }
        if ((r.Q.ws == 0) && bready && !r.Q.ready8)
        {
	        v.ready8 = 1;
          v.ws = rws;
	        if (!r.Q.brmw)
          {
            v.address &= ~0x2;
            v.address |= ((~r.Q.address)&0x2);
          }
        }
        if (r.Q.ready8)       v.ws = rws;
        else if(r.Q.ws != 0)  v.ws = r.Q.ws - 1;
      }else
      {
        v.bstate = idle;
      }
    break;
    case bwrite16:
      if (BUS16EN)
      {
        if ( (r.Q.ws == 0) && bready &&
            (((r.Q.address&0x3) == 0x2) || !((r.Q.size>>1)&0x1)) )
        {
	        ready = 1;
          v.wrn = 0xF;
          v.writen = 1;
          v.echeck = 1;
	        v.ramsn = 0x1F;
          v.romsn = 0x3;
          v.iosn = 0x3;
	        v.bdrive = 0xF;
          v.bstate = idle;
        }
        if ((r.Q.ws == 0) && bready && !r.Q.ready8)
        {
	        v.ready8 = 1;
          v.wrn = 0xF;
          v.writen = 1;
        }
        if (r.Q.ready8)
        {
          v.address &= ~0x2;
	        v.address |= ((~r.Q.address)&0x2);
          v.ws = rws;
	        v.writedata = (v.writedata&0xFFFF) | ((r.Q.writedata8&0xFFFF)<<16);
	        v.bstate = idle;
        }
        if (r.Q.ws != 0) v.ws = r.Q.ws - 1;
      }else
      {
        v.bstate = idle;
      }
    break;
    default:;
  }

  // if BUSY or IDLE cycle seen, or if de-selected, return to idle state
  if (ahbsi.hready)
  {
    if ( !((ahbsi.hsel>>hindex)&0x1) || (ahbsi.htrans == HTRANS_BUSY) 
        || (ahbsi.htrans == HTRANS_IDLE) )
    {
      v.bstate = idle;
      v.ramsn = 0x1F;
      v.romsn = 0x3;
      v.ramoen = 0x1F;
      v.oen = 1;
      v.iosn = 0x3;
      v.bdrive = 0xF;
      v.wrn = 0xF;
      v.writen = 1;
      v.hsel = 0;
      ready = (ahbsi.hsel>>hindex)&0x1;
      v.srhsel = 0;
    }else if (srhsel)
    {
      v.romsn = romsn;
      v.ramsn &= ~0x1F;
      v.ramsn |= (ramsn&0x1F);
      v.iosn = ((iosn<<1) | 1)&0x3;
      if (v.read)
      {
        v.ramoen &= ~0x1F;
        v.ramoen |= (ramsn&0x1F);
        v.oen = leadin;
      }
    }
  }

// error checking and reporting

  noerror = 1;
  if (r.Q.echeck & r.Q.mcfg1.bexcen & !r.Q.bexcn)
  {
    noerror = 0;
    v.bstate = berr;
    v.hresp = HRESP_ERROR;
    v.bdrive = 0xF;
    v.wrn = 0xF;
    v.writen = 1;
  }

// APB register access
  uint32 tmp;
  switch((apbi.paddr>>2)&0x3)
  {
    case 0:
      tmp = r.Q.mcfg1.romrws&0xF;// 4 bits
      tmp |= ((r.Q.mcfg1.romwws&0xF)<<4);//4 bits
      tmp |= ((r.Q.mcfg1.romwidth&0x3)<<8);//2 bits + 1 zero
      tmp |= ((r.Q.mcfg1.romwrite)<<11);//1 bit + 7 zeroz
      tmp |= ((r.Q.mcfg1.ioen)<<19);// 1 bit
      tmp |= ((r.Q.mcfg1.iows&0xF)<<20);//4 bits + 1 zero
      tmp |= ((r.Q.mcfg1.bexcen)<<25);//1 bit
      tmp |= ((r.Q.mcfg1.brdyen)<<26);//1 bit
      tmp |= ((r.Q.mcfg1.iowidth&0x3)<<27);//2 bits
      regsd = tmp;//[28:0]
    break;
    case 1:
#ifdef SDRAM_ENABLE
      regsd = (sdmo.prdata&0xFFFF0000)|(regsd&0xFFFF);
      if (BUS64) regsd |= (1<<18);
      regsd &= ~(1<<13);
      regsd |= (r.Q.mcfg2.srdis<<13);
      regsd &= ~(1<<14);
      regsd |= (r.Q.mcfg2.sdren<<14);
#endif
      regsd &= ~(0xF<<9);
      regsd |= ((r.Q.mcfg2.rambanksz&0xF)<<9);
      if(RAMSEL5)
      {
        regsd &= ~(1<<7);
        regsd |= (r.Q.mcfg2.brdyen<<7);
      }
      tmp = r.Q.mcfg2.ramrws&0x3;
      tmp |= ((r.Q.mcfg2.ramwws&0x3)<<2);
      tmp |= ((r.Q.mcfg2.ramwidth&0x3)<<4);
      regsd = (r.Q.mcfg2.rmw<<6) | tmp;
    break;
    case 2:
#ifdef SDRAM_ENABLE
      regsd &= ~(0x7FFF<<12);
      regsd |= (sdmo.prdata&(0x7FFF<<12));
#endif
    break;
    case 3:
#ifdef SDRAM_ENABLE
      regsd = sdmo.prdata;
#endif
    break;
    default: regsd = 0;
  }

    apbo.prdata = regsd;

    if (((apbi.psel>>pindex)&0x1) && apbi.penable && apbi.pwrite)
    {
      switch((apbi.paddr>>2)&0xF)
      {
        case 0:
          v.mcfg1.romrws      =  apbi.pwdata&0xF;
          v.mcfg1.romwws      =  (apbi.pwdata>>4)&0xF;
          v.mcfg1.romwidth    =  (apbi.pwdata>>8)&0x3;
          v.mcfg1.romwrite    =  (apbi.pwdata>>11)&0x1;

          v.mcfg1.ioen        =  (apbi.pwdata>>19)&0x1;
          v.mcfg1.iows        =  (apbi.pwdata>>20)&0xF;
          v.mcfg1.bexcen      =  (apbi.pwdata>>25)&0x1;
          v.mcfg1.brdyen      =  (apbi.pwdata>>26)&0x1;
          v.mcfg1.iowidth     =  (apbi.pwdata>>27)&0x3;
        break;
        case 1:
          v.mcfg2.ramrws      = apbi.pwdata&0x3;
          v.mcfg2.ramwws      = (apbi.pwdata>>2)&0x3;
          v.mcfg2.ramwidth    = (apbi.pwdata>>4)&0x3;
          v.mcfg2.rmw         = (apbi.pwdata>>6)&0x1;
          v.mcfg2.brdyen      = (apbi.pwdata>>7)&0x1;
          v.mcfg2.rambanksz   = (apbi.pwdata>>9)&0xF;
#ifdef SDRAM_ENABLE
          v.mcfg2.srdis     = ((apbi.pwdata>>13)&0x1);
          v.mcfg2.sdren     = ((apbi.pwdata>>14)&0x1);
#endif
        break;
        default:;
      }
    }

  // select appropriate data during reads
  if (((r.Q.area>>rom)&0x1) | ((r.Q.area>>ram)&0x1)) 
    dataout = memdata;
  else
  {
    if (BUS8EN && (r.Q.busw == 0))
    {
      tmp = (r.Q.data>>24)&0xFF;
      dataout = (tmp<<24) | (tmp<<16) | (tmp<<8) | tmp;
    }else if(BUS16EN && (r.Q.busw == 1))
    {
      tmp = (r.Q.data>>16)&0xffff;
      dataout = (tmp<<16) | tmp; 
    }else 
      dataout = r.Q.data;
  }

  v.ready = ready;
  v.srhsel = r.Q.srhsel & !ready;
  if (ahbsi.hready)
    v.hsel = ((ahbsi.hsel>>hindex)&0x1);

  if (ahbsi.hready & ((ahbsi.hsel>>hindex)&0x1) )
  {
    v.size = ahbsi.hsize&0x3;
    v.hwrite = ahbsi.hwrite;
    v.hburst = ahbsi.hburst;
    v.htrans = ahbsi.htrans;
    if ((ahbsi.htrans>>1)&0x1)
    {
      v.hsel = 1;
      v.srhsel = srhsel;
    }
#ifdef SDRAM_ENABLE
      v.haddr = ahbsi.haddr;
      v.sdhsel = sdhsel;
#endif
  }
  // sdram synchronisation
#ifdef SDRAM_ENABLE
  v.sa = sdmo.address;
  v.sd = memi.sd;
  tmp = ((r.Q.ramsn&0x1F)<<4) | ((r.Q.romsn&0x3)<<2) | (r.Q.iosn&0x3);
  if ((r.Q.bstate != idle) || (tmp != 0x1FF) )
    bidle = 0;
  else
  {
    bidle = 1;
    if (sdmo.busy & !sdmo.aload)
    {
      if (!SDSEPBUS)
      {
        //v.address(sdlsb + 14 downto sdlsb) := sdmo.address;
        v.address = sdmo.address; // SH !!!!
      }
      v.romsn   = 0x3;
      v.ramsn   = 0x1F;
      v.iosn    = 0x3; 
      v.ramoen  = 0x1F;;
      v.oen     = 1;
      v.bdrive  = (~((sdmo.bdrive<<12) | (sdmo.bdrive<<8) | (sdmo.bdrive<<4) | sdmo.bdrive))&0xF; // SH ????????????????
      if (r.Q.sdhsel)
        v.hresp = sdmo.hresp;
    }
  }
  if (sdmo.aload & r.Q.srhsel)
  {
    v.romsn = romsn; 
    v.ramsn = ramsn;
    v.iosn = ((iosn<<1)| 0x1)&0x3;
    if (v.read)
    {
      v.ramoen = ramsn;
      v.oen = leadin;
    }
  }
  if (sdmo.hsel)
  {
    v.writedata = writedata;
    v.sdwritedata = (uint64)writedata;
    if (BUS64 && (sdmo.bsel == 1))
    {
      v.sdwritedata &= ~0xFFFFFFFF00000000;
      v.sdwritedata |= ((uint64)writedata<<32);
    }
    hready = sdmo.hready & noerror & !r.Q.brmw;
    if (SDSEPBUS)
    {
      if (BUS64 && (sdmo.bsel == 1)) dataout = (uint32)(r.Q.sd>>32);
      else dataout = (uint32)r.Q.sd;
    }
  }else hready = r.Q.ready & noerror;
#else
  hready = r.Q.ready & noerror;
#endif

    
  if (v.read) v.mben = 0;
  else        v.mben = v.wrn;

  v.nbdrive = (~v.bdrive)&0xF;

  if (oepol == 0)
  {
    bdrive_sel = r.Q.bdrive;
    vbdrive  = ((v.bdrive&0x1) ? 0xFF000000: 0x0);
    vbdrive |= ((v.bdrive&0x2) ? 0x00FF0000: 0x0);
    vbdrive |= ((v.bdrive&0x4) ? 0x0000FF00: 0x0);
    vbdrive |= ((v.bdrive&0x8) ? 0x000000FF: 0x0);

    vsbdrive  = ((v.bdrive&0x1) ? 0x00000000FF000000: 0x0);
    vsbdrive |= ((v.bdrive&0x2) ? 0x0000000000FF0000: 0x0);
    vsbdrive |= ((v.bdrive&0x4) ? 0x000000000000FF00: 0x0);
    vsbdrive |= ((v.bdrive&0x8) ? 0x00000000000000FF: 0x0);
    
    vsbdrive |= ((v.bdrive&0x1) ? 0xFF00000000000000: 0x0);
    vsbdrive |= ((v.bdrive&0x2) ? 0x00FF000000000000: 0x0);
    vsbdrive |= ((v.bdrive&0x4) ? 0x0000FF0000000000: 0x0);
    vsbdrive |= ((v.bdrive&0x8) ? 0x000000FF00000000: 0x0);
  }else
  {
    bdrive_sel = r.Q.nbdrive;
    vbdrive  = ((v.nbdrive&0x1) ? 0xFF000000: 0x0);
    vbdrive |= ((v.nbdrive&0x2) ? 0x00FF0000: 0x0);
    vbdrive |= ((v.nbdrive&0x4) ? 0x0000FF00: 0x0);
    vbdrive |= ((v.nbdrive&0x8) ? 0x000000FF: 0x0);

    vsbdrive  = ((v.nbdrive&0x1) ? 0x00000000FF000000: 0x0);
    vsbdrive |= ((v.nbdrive&0x2) ? 0x0000000000FF0000: 0x0);
    vsbdrive |= ((v.nbdrive&0x4) ? 0x000000000000FF00: 0x0);
    vsbdrive |= ((v.nbdrive&0x8) ? 0x00000000000000FF: 0x0);
    
    vsbdrive |= ((v.nbdrive&0x1) ? 0xFF00000000000000: 0x0);
    vsbdrive |= ((v.nbdrive&0x2) ? 0x00FF000000000000: 0x0);
    vsbdrive |= ((v.nbdrive&0x4) ? 0x0000FF0000000000: 0x0);
    vsbdrive |= ((v.nbdrive&0x8) ? 0x000000FF00000000: 0x0);
  }
        
  // reset
  if (!rst)
  {
    v.bstate           = idle; 
    v.read             = 1; 
    v.wrn              = 0xf;
    v.writen           = 1; 
    v.mcfg1.romwrite   = 0;
    v.mcfg1.ioen       = 0;
    v.mcfg1.brdyen     = 0;
    v.mcfg1.bexcen     = 0;
    v.hsel             = 0;
    v.srhsel           = 0;
    v.ready            = 1;
    v.mcfg1.iows       = 0;
    v.mcfg2.ramrws     = 0;
    v.mcfg2.ramwws     = 0;
    v.mcfg1.romrws     = 0xF;
    v.mcfg1.romwws     = 0xF;
    v.mcfg1.romwidth   = memi.bwidth;
    v.mcfg2.srdis      = 0;
    v.mcfg2.sdren      = 0;
    if (syncrst == 1)
    {
      v.ramsn   = 0x1F;
      v.romsn   = 0x3;
      v.oen     = 1;
      v.iosn    = 0x3;
      v.ramoen  = 0x1F;
      v.bdrive  = 0xF;
      v.nbdrive = 0;
      if (oepol == 0) 
      {
        vbdrive  = 0xFFFFFFFF;
        vsbdrive = 0xFFFFFFFFFFFFFFFF;
      }else 
      {
        vbdrive  = 0;
        vsbdrive = 0;
      }
    }
  }

  // optional feeb-back from write stobe to data bus drivers
  if (oepol == 0)
  {
    if (WENDFB) bdrive = r.Q.bdrive & memi.wrn;
    else        bdrive = r.Q.bdrive;
  }else
  {
    if (WENDFB) bdrive = r.Q.nbdrive | ((~memi.wrn)&0xF);
    else        bdrive = r.Q.nbdrive;
  }

// pragma translate_off
//    for i in dataout'range loop --'
//      if is_x(dataout(i)) then dataout(i) := '1'; end if;
//    end loop;
// pragma translate_on

// scan support
  if (syncrst & !rst)
  {
    memo.ramsn    = 0xFF;
    memo.ramoen   = 0xFF;
    memo.romsn    = 0xFF;
    memo.iosn     = 1;
    memo.oen      = 1;
    if ((scantest==1) && ahbsi.testen)
    {
      memo.bdrive   = ahbsi.testoen ? 0xF: 0x0;
      memo.vbdrive  = ahbsi.testoen ? 0xFFFFFFFF: 0x0;
      memo.svbdrive = ahbsi.testoen ? 0xFFFFFFFFFFFFFFFF: 0x0;
    }else
    {
      if (oepol == 0)
      {
        memo.bdrive   = 0xF;
        memo.vbdrive  = 0xFFFFFFFF;
        memo.svbdrive = 0xFFFFFFFFFFFFFFFF;
      }else
      {
        memo.bdrive   = 0;
        memo.vbdrive  = 0;
        memo.svbdrive = 0;
      }
    }
  }else
  {
    memo.ramsn    = (0x7<<5) | r.Q.ramsn;
    memo.ramoen   = (0x7<<5) | r.Q.ramoen;
    memo.romsn    = (0x7<<2) | r.Q.romsn;
    memo.iosn     = r.Q.iosn&0x1;
    memo.oen      = r.Q.oen;
    if ((scantest == 1) && ahbsi.testen)
    {
      memo.bdrive   = ahbsi.testoen ? 0xF: 0x0;
      memo.vbdrive  = ahbsi.testoen ? 0xFFFFFFFF: 0x0;
      memo.svbdrive = ahbsi.testoen ? 0xFFFFFFFFFFFFFFFF: 0x0;
    }else
    {
      memo.bdrive   = bdrive;
      memo.vbdrive  = rbdrive.Q;
      memo.svbdrive = rrsbdrive.Q;
    }
  }

  // drive various register inputs and external outputs
  ri = v;

  ribdrive = vbdrive;
  risbdrive = vsbdrive; 
  
  ahbso.hcache  = !((r.Q.area>>io)&0x1);
  memo.address  = r.Q.address;
  memo.read     = r.Q.read;
  memo.wrn      = r.Q.wrn;
  memo.writen   = r.Q.writen;
  memo.data     = r.Q.writedata;
  memo.mben     = r.Q.mben;
  memo.svcdrive = 0;
  memo.vcdrive  = 0;
  memo.scb      = 0;
  memo.cb       = 0;
  memo.romn     = 1;
  memo.ramn     = 1;
  sdi.idle      = bidle;
  sdi.haddr     = haddr;
  sdi.rhaddr    = r.Q.haddr;
  sdi.nhtrans   = htrans;
  sdi.rhtrans   = r.Q.htrans;
  sdi.htrans    = ahbsi.htrans;
  sdi.hready    = ahbsi.hready;
  sdi.hsize     = r.Q.size;
  sdi.hwrite    = r.Q.hwrite;
  sdi.hsel      = sdhsel;
  sdi.enable    = r.Q.mcfg2.sdren;
  sdi.srdis     = r.Q.mcfg2.srdis;
  sdi.edac      = 0;
  sdi.brmw      = 0;
  sdi.error     = 0;

  ahbso.hrdata = dataout;//SH: ahbdrivedata(dataout);
  ahbso.hready = hready;
  ahbso.hresp  = r.Q.hresp;


  r.CLK = clk;  
  r.D   = ri;
  if(rst) r.D.ws = r.Q.ws = 0;

  rbdrive.CLK = clk;
  rbdrive.D = ribdrive;

  rsbdrive.CLK = clk;
  rsbdrive.D = risbdrive;

  if (!syncrst & !arst)
  {
    r.D.ramsn   = r.Q.ramsn   = 0x1F;
    r.D.romsn   = r.Q.romsn   = 0x3;
    r.D.oen     = r.Q.oen     = 1;
    r.D.iosn    = r.Q.iosn    = 0x3;
    r.D.ramoen  = r.Q.ramoen  = 0x1F;
    r.D.bdrive  = r.Q.bdrive  = 0xF;
    r.D.nbdrive = r.Q.nbdrive = 0;
    if (oepol==0)
    {
      rbdrive.D  = rbdrive.Q  = 0xFFFFFFFF;
      rsbdrive.D = rsbdrive.Q = 0xFFFFFFFFFFFFFFFF;
    }else
    {
      rbdrive.D  = rbdrive.Q  = 0;
      rsbdrive.D = rsbdrive.Q = 0;
    }
  }

  ahbso.hsplit  = 0;
  ahbso.hconfig = hconfig;
  ahbso.hirq    = 0;
  ahbso.hindex  = hindex;
  apbo.pconfig  = pconfig;
  apbo.pirq     = 0;
  apbo.pindex   = pindex;

  // optional sdram controller
#ifdef SDRAM_ENABLE
/*  sdmctrl sdctrl;
  sdctrl Update(// generate:
                pindex,
                invclk,
                fast,
                wprot,
                sdbits,
                pageburst,
                mobile,
                //variables:
                rst,
                clk,
                sdi,
                sdo,
                apbi,
                wpo,
                sdmo);
  */
  if (invclk == 0)
  {
    memo.sa   = r.Q.sa; 
    sdo       = lsdo; 
    rrsbdrive.D = rsbdrive.Q;
    memo.sddata  = r.Q.sdwritedata;
  }

  if (invclk == 1)
  {
    //if (falling_edge(clk))
    {
      memo.sa   = r.Q.sa;
      sdo       = lsdo;
      rrsbdrive.D = rsbdrive.Q;
      memo.sddata = r.Q.sdwritedata;
      if (!syncrst & !arst)
      {
        if (oepol==0) rrsbdrive.D = 0xFFFFFFFFFFFFFFFF;
        else          rrsbdrive.D = 0;
      }
    }
  }
  
#else

  sdo.sdcke = 0;
  sdo.sdcsn = 0x3;
  sdo.sdwen = 1;
  sdo.rasn  = 1;
  sdo.casn  = 1;
  sdo.dqm   = 0xFF;

  sdmo.prdata = 0;
  sdmo.address= 0;
  sdmo.busy   = 0;
  sdmo.aload  = 0;
  sdmo.bdrive = 0;
  sdmo.hready = 1;
  sdmo.hresp  = 0x3;
#endif


}

