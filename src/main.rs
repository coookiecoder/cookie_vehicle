#![no_std]
#![no_main]

#![allow(unused_imports)]

use panic_halt;

use stm32f4xx_hal as hal;

use hal::pac;
use hal::spi::SpiSlave;
use hal::spi::Polarity as SpiPolarity;
use hal::spi::Mode as SpiMode;
use hal::spi::Phase as SpiPhase;

use hal::timer::PwmExt;

use hal::prelude::_stm32f4xx_hal_rcc_RccExt;
use hal::prelude::_stm32f4xx_hal_gpio_GpioExt;
use hal::prelude::_fugit_RateExtU32;

use hal::rcc::Config;

use hal::hal_02::PwmPin;

pub fn fake_exit() -> ! {
    loop {
        cortex_m::asm::nop();
    }
}

pub fn fake_debug_exit() -> ! {
    loop {
        cortex_m::asm::bkpt();
    }
}

use cortex_m_rt::entry;

#[entry]
fn main() -> ! {
    let _dp = pac::Peripherals::take().expect("cannot take peripherals");
    let mut _cp = pac::CorePeripherals::take().expect("cannot take core peripherals");

    _cp.DCB.enable_trace();
    _cp.DWT.enable_cycle_counter();

    let mut rcc = _dp.RCC.freeze(Config::default());
    let _clocks = rcc.clocks;

    let mut _gpio_a = _dp.GPIOA.split(&mut rcc);
    let _gpio_b = _dp.GPIOB.split(&mut rcc);
    let _gpio_c = _dp.GPIOC.split(&mut rcc);
    let _gpio_d = _dp.GPIOD.split(&mut rcc);
    let _gpio_e = _dp.GPIOE.split(&mut rcc);
    
    let (_pwm, (_ch1, ch2, ch3, _ch4)) = _dp.TIM2.pwm_hz(1.kHz(), &mut rcc);

    let mut steering_pin = ch2.with(_gpio_a.pa1);
    let mut speed_pin = ch3.with(_gpio_a.pa2);

    steering_pin.enable();
    speed_pin.enable();

    let sck = _gpio_a.pa5.into_alternate();
    let miso = _gpio_a.pa6.into_alternate();
    let mosi = _gpio_a.pa7.into_alternate();
    let nss = _gpio_a.pa4.into_alternate();

    let mut spi = SpiSlave::new(_dp.SPI1, (Some(sck), Some(miso), Some(mosi), Some(nss)), SpiMode { polarity: SpiPolarity::IdleLow, phase: SpiPhase::CaptureOnFirstTransition, }, &mut rcc);

    let mut speed: u16 = 0;
    let mut steering: u16 = 127;

    update_pwm(&mut speed_pin, &speed);
    update_pwm(&mut steering_pin, &steering);

    loop {
        let mut data_from_pi_0 = [0u8; 2];
        let mut data_to_pi_0 = [0u8; 2];

        spi.transfer(&mut data_from_pi_0, &data_to_pi_0).ok();

        speed = u16::from_be_bytes([data_from_pi_0[0], data_from_pi_0[1]])
        steering = u16::from_be_bytes([data_from_pi_0[2], data_from_pi_0[3]])

        update_pwm(&mut speed_pin, &speed);
        update_pwm(&mut steering_pin, &steering);
    }
}

fn update_pwm<P: PwmPin<Duty = u16>>(pin: &mut P, value: &u16) {
    let max = pin.get_max_duty();
    let duty = (*value as u32 * max as u32 / 255) as u16;
    pin.set_duty(duty);
}