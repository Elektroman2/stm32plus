#pragma once

namespace stm32plus
{
/**
 *  LOW LEVEL CLASS FOR THE 1-WIRE BUS
 */

/**
 * \brief LOW LEVEL CLASS FOR THE 1-WIRE BUS
 * \param owipin GpioPinRef for 1-Wire bus
 * \param timer Reference to a Timer device, which is used for low level timing
 *
 */
class OwiBase
{
    public:

    GpioPinRef _owiPin;
    OwiDelay _timing;
    public:
    OwiBase(GpioPinRef& owipin,  Timer* timer);
    OwiBase(GpioPinRef& owipin, OwiDelay& timing);



    void set_Low();
    void set_High();
    void set_In();
    uint8_t get_Pin();

};


/**
 * Constructor
 */

inline OwiBase::OwiBase(GpioPinRef& owipin, Timer* timer) :
        _owiPin(owipin), _timing(timer)
{

}

inline OwiBase::OwiBase(GpioPinRef& owipin, OwiDelay& timing) :
        _owiPin(owipin),_timing(timing)
{
}



/**
 * \brief Drives the 1-Wire bus LOW
 *
 * Sets the IO direction to OUT, then drives LOW
 */
inline void OwiBase::set_Low()
{

    if (Gpio::OUTPUT != _owiPin.getMode()) _owiPin.setMode(Gpio::OUTPUT, Gpio::PUSH_PULL);
    _owiPin.reset();
}


/**
 * \brief Drives the 1-Wire bus HIGH
 *
 * Sets the IO direction to OUT, then drives HIGH
 */
inline void OwiBase::set_High()
{
    if (Gpio::OUTPUT != _owiPin.getMode()) _owiPin.setMode(Gpio::OUTPUT, Gpio::PUSH_PULL);
    _owiPin.set();
}

/**
 * \brief Sets the IO direction to IN
 */
inline void OwiBase::set_In()
{
    if (Gpio::INPUT != _owiPin.getMode()) _owiPin.setMode(Gpio::INPUT, Gpio::OPEN_DRAIN,Gpio::PUPD_UP);
}

/**
 * \brief Reads the 1-Wire bus
 *
 * Sets the IO direction to IN, then reads the bus
 */
inline uint8_t OwiBase::get_Pin()
{
    if (Gpio::INPUT != _owiPin.getMode()) _owiPin.setMode(Gpio::INPUT, Gpio::OPEN_DRAIN,Gpio::PUPD_UP);
    return _owiPin.read();
}


}
