/**
 * @file hal_ra4m1.cpp
 * @brief Timers de ignição e IAC no RA4M1.
 *
 * Ignição: GPT0 (32 bits) em PCLKD/64. No UNO R4 o PCLKD é 48 MHz, então
 * o clock do timer é 750 kHz e 12 contagens de hardware são 16 µs — o
 * mesmo tick de US_TO_TIMER1. O período é 65536 ticks lógicos
 * (GTPR+1 = 786432 contagens), o wrap de 1,048576 s do Timer1 do 328P.
 * GTCCRA/GTCCRB não vão para pino: D4/D5 continuam GPIO. O canal 0 está
 * marcado pelo variant como PWM do D6; a saída GTIOC fica desligada para
 * a bomba de combustível continuar em digitalWrite.
 *
 * IAC: AGT1 em PCLKB (24 MHz no UNO R4, divisor 2 do HOCO de 48 MHz).
 * 6048 contagens = 252 µs, o tick do Timer2 CTC do 328P (OCR2A=62,
 * prescaler 64). AGT0 permanece com o millis/micros do core.
 */

#include "hal_ra4m1.h"

#if defined(BOARD_RA4M1)

#include "FspTimer.h"
#include "scheduler.h"

static const uint8_t RA4M1_IGN_CHANNEL = 0;
static const uint8_t RA4M1_IAC_CHANNEL = 1;
static const uint32_t RA4M1_TICK_HZ = 62500UL;       // 16 µs
static const uint32_t RA4M1_GPT_DIV = 64UL;
static const uint32_t RA4M1_IAC_TICK_US = 252UL;     // 63 * 4 µs no AVR
static const uint8_t RA4M1_IRQ_PRIORITY = 12;        // igual ao IRQ externo do core

static FspTimer s_ignTimer;
static FspTimer s_iacTimer;

static bool s_ignReady = false;
static bool s_iacOpen = false;
static bool s_iacRunning = false;
static uint32_t s_countsPerTick = 12;

static void (*volatile s_compareA)() = nullptr;
static void (*volatile s_compareB)() = nullptr;
static void (*volatile s_iacTick)() = nullptr;

static void ignUnlock() {
  R_GPT0->GTWP = (uint32_t)(0xA5u << R_GPT0_GTWP_PRKEY_Pos);
}

extern "C" void ra4m1GptCompareAIsr(void) {
  // O IR do ICU fica setado até ser limpo. Sem isto o compare reentra
  // imediatamente e o núcleo não volta ao loop.
  IRQn_Type irq = R_FSP_CurrentIrqGet();
  R_BSP_IrqStatusClear(irq);
  R_GPT0->GTST_b.TCFA = 0;
  void (*fn)() = s_compareA;
  if (fn != nullptr) {
    fn();
  }
}

extern "C" void ra4m1GptCompareBIsr(void) {
  IRQn_Type irq = R_FSP_CurrentIrqGet();
  R_BSP_IrqStatusClear(irq);
  R_GPT0->GTST_b.TCFB = 0;
  void (*fn)() = s_compareB;
  if (fn != nullptr) {
    fn();
  }
}

static bool ignClockScale(uint32_t *countsPerTick, uint32_t *periodCounts) {
  uint32_t pclkd = R_FSP_SystemClockHzGet(FSP_PRIV_CLOCK_PCLKD);
  uint32_t timerHz = pclkd / RA4M1_GPT_DIV;
  if ((timerHz % RA4M1_TICK_HZ) != 0u) {
    return false;
  }
  uint32_t scale = timerHz / RA4M1_TICK_HZ;
  if (scale == 0u || scale > 4096u) {
    return false;
  }
  uint64_t period = 65536ull * (uint64_t)scale;
  if (period > 0xFFFFFFFFull) {
    return false;
  }
  *countsPerTick = scale;
  *periodCounts = (uint32_t)period;
  return true;
}

bool ra4m1IgnitionBegin(void (*compareA)(), void (*compareB)()) {
  s_ignReady = false;
  s_compareA = compareA;
  s_compareB = compareB;

  uint32_t scale = 0;
  uint32_t periodCounts = 0;
  if (!ignClockScale(&scale, &periodCounts)) {
    return false;
  }

  // TIMER_MODE_PWM só para aceitar o canal que o variant marcou como PWM.
  // enable_pwm_channel() não é chamado: GTIOCA/B permanecem desligados.
  if (!s_ignTimer.begin(TIMER_MODE_PWM, GPT_TIMER, RA4M1_IGN_CHANNEL,
                        periodCounts, 0, TIMER_SOURCE_DIV_64, nullptr, nullptr)) {
    return false;
  }
  if (s_ignTimer.get_channel() != RA4M1_IGN_CHANNEL) {
    return false;
  }
  if (!s_ignTimer.setup_capture_a_irq(RA4M1_IRQ_PRIORITY, ra4m1GptCompareAIsr)) {
    return false;
  }
  if (!s_ignTimer.setup_capture_b_irq(RA4M1_IRQ_PRIORITY, ra4m1GptCompareBIsr)) {
    return false;
  }
  if (!s_ignTimer.open()) {
    return false;
  }
  s_ignTimer.stop();

  ignUnlock();
  // BD0/BD1 = 1 desliga o buffer de GTCCR e GTPR. Sem isso o compare
  // novo só vale no fim do período e o dwell atrasa ~1 s.
  R_GPT0->GTBER_b.BD0 = 1;
  R_GPT0->GTBER_b.BD1 = 1;
  R_GPT0->GTBER_b.CCRA = 0;
  R_GPT0->GTBER_b.CCRB = 0;
  R_GPT0->GTPR = periodCounts - 1u;
  R_GPT0->GTCNT = 0;
  R_GPT0->GTIOR_b.OAE = 0;
  R_GPT0->GTIOR_b.OBE = 0;
  // GTITC.ITLA/ITLB ligam o compare match ao IRQ. O header CMSIS deste
  // dispositivo não nomeia bits de enable no GTINTAD (reservados).
  R_GPT0->GTITC_b.ITLA = 1;
  R_GPT0->GTITC_b.ITLB = 1;

  s_countsPerTick = scale;
  uint32_t idleCompare = 0xFFFFu * scale;
  R_GPT0->GTCCR[0] = idleCompare;
  R_GPT0->GTCCR[1] = idleCompare;
  R_GPT0->GTST_b.TCFA = 0;
  R_GPT0->GTST_b.TCFB = 0;

  if (!s_ignTimer.start()) {
    return false;
  }
  s_ignReady = true;
  return true;
}

bool ra4m1IgnitionReady() {
  return s_ignReady;
}

uint16_t getTimer1Count() {
  uint32_t hw = R_GPT0->GTCNT;
  return (uint16_t)(hw / s_countsPerTick);
}

void setTimer1CompareA(uint16_t value) {
  R_GPT0->GTCCR[0] = (uint32_t)value * s_countsPerTick;
}

void setTimer1CompareB(uint16_t value) {
  R_GPT0->GTCCR[1] = (uint32_t)value * s_countsPerTick;
}

static void iacCallback(timer_callback_args_t *args) {
  (void)args;
  void (*fn)() = s_iacTick;
  if (fn != nullptr) {
    fn();
  }
}

bool ra4m1IacBegin(void (*tick)()) {
  s_iacOpen = false;
  s_iacRunning = false;
  s_iacTick = tick;

  uint32_t pclkb = R_FSP_SystemClockHzGet(FSP_PRIV_CLOCK_PCLKB);
  uint64_t product = (uint64_t)pclkb * (uint64_t)RA4M1_IAC_TICK_US;
  if ((product % 1000000ull) != 0ull) {
    return false;
  }
  uint32_t counts = (uint32_t)(product / 1000000ull);
  if (counts == 0u || counts > 65536u) {
    return false;
  }

  if (!s_iacTimer.begin(TIMER_MODE_PERIODIC, AGT_TIMER, RA4M1_IAC_CHANNEL,
                        counts, 0, TIMER_SOURCE_DIV_1, iacCallback, nullptr)) {
    return false;
  }
  if (s_iacTimer.get_channel() != RA4M1_IAC_CHANNEL) {
    return false;
  }
  if (!s_iacTimer.setup_overflow_irq(RA4M1_IRQ_PRIORITY, nullptr)) {
    return false;
  }
  if (!s_iacTimer.open()) {
    return false;
  }
  s_iacTimer.stop();
  s_iacOpen = true;
  return true;
}

void ra4m1IacStart() {
  if (s_iacOpen && !s_iacRunning) {
    if (s_iacTimer.start()) {
      s_iacRunning = true;
    }
  }
}

void ra4m1IacStop() {
  if (s_iacRunning) {
    s_iacTimer.stop();
    s_iacRunning = false;
  }
}

bool ra4m1IacRunning() {
  return s_iacRunning;
}

#endif
