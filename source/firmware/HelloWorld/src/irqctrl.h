#ifndef __IRQTEST_H__
#define __IRQTEST_H__

#include "stdtypes.h"

struct irqmp_fields {
        volatile uint32 irqlevel;    /* 0x00 */
        volatile uint32 irqpend;	   /* 0x04 */
        volatile uint32 irqforce;	   /* 0x08 */
        volatile uint32 irqclear;	   /* 0x0C */
        volatile uint32 mpstatus;	   /* 0x10 */
        volatile uint32 broadcast;   /* 0x14 */
        volatile uint32 dummy0;      /* 0x18 */
        volatile uint32 wdogctrl;    /* 0x1C (IRQ(A)MP) */
        volatile uint32 asmpctrl;    /* 0x20 (IRQ(A)MP) */
        volatile uint32 icsel0;	   /* 0x24 (IRQ(A)MP) */
        volatile uint32 icsel1; 	   /* 0x28 */
        volatile uint32 dummy1[5];   /* 0x2c - 0x3C */
        volatile uint32 irqmask;     /* 0x40 */
};


class IrqControl
{
	friend class dbg;
	private:
		irqmp_fields *irqmp;

	public:
		void Init();

};

#endif //__IRQTEST_H__
