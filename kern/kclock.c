/* See COPYRIGHT for copyright information. */

#include <inc/x86.h>
#include <kern/kclock.h>
#include <kern/timer.h>
#include <kern/trap.h>
#include <kern/picirq.h>

/* HINT: Note that selected CMOS
 * register is reset to the first one
 * after first access, i.e. it needs to be selected
 * no every access.
 *
 * Don't forget to disable NMI for the time of
 * operation (look up for the appropriate constant in kern/kclock.h)
 *
 * Why it is necessary?
 */

uint8_t
cmos_read8(uint8_t reg) {
    /* MC146818A controller */
    // LAB 4: Your code here

    uint8_t res = 0;
    nmi_disable();

    outb(IO_RTC_CMND, reg);
    res = inb(IO_RTC_DATA);

    nmi_enable();
    return res;
}

void
cmos_write8(uint8_t reg, uint8_t value) {
    // LAB 4: Your code here
    nmi_disable();

    outb(IO_RTC_CMND, reg);
    outb(IO_RTC_DATA, value);

    nmi_enable();
}

uint16_t
cmos_read16(uint8_t reg) {
    return cmos_read8(reg) | (cmos_read8(reg + 1) << 8);
}

static void
rtc_timer_pic_interrupt(void) {
    pic_irq_unmask(IRQ_CLOCK);
}

static void
rtc_timer_pic_handle(void) {
    rtc_check_status();
    pic_send_eoi(IRQ_CLOCK);
}

struct Timer timer_rtc = {
        .timer_name = "rtc",
        .timer_init = rtc_timer_init,
        .enable_interrupts = rtc_timer_pic_interrupt,
        .handle_interrupts = rtc_timer_pic_handle,
};

void
rtc_timer_init(void) {
    // LAB 4: Your code here
    // (use cmos_read8/cmos_write8)
    
    uint8_t reg_b = cmos_read8(RTC_BREG);
    reg_b |= RTC_PIE;
    cmos_write8(RTC_BREG, reg_b);

    uint8_t reg_a = cmos_read8(RTC_AREG);
    reg_a = RTC_SET_NEW_RATE(reg_a, RTC_500MS_RATE);
    cmos_write8(RTC_AREG, reg_a);

}

uint8_t
rtc_check_status(void) {
    // LAB 4: Your code here
    // (use cmos_read8)
    
    return cmos_read8(RTC_CREG);
}
