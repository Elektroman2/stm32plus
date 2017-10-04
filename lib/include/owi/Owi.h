#pragma once

namespace stm32plus
{

class Owi: public OwiBase
{
    protected:

    static const uint32_t resetPulseDelay = 480; /* master reset pulse time in [us] */
    static const uint32_t masterBitstart = 2; /* delay time for bit start by master */
    static const uint32_t recoveryDelay = 5; /* delay time for bus recovery,increase for longer wire */
    static const uint32_t presenceWait = 40; /* delay after master reset pulse in [us] */
    static const uint32_t presenceFin = 480; /* delay after reading of presence pulse [us] */
    static const uint32_t bitReadDelay = 5; /* bit read delay */
    static const uint32_t betweenbitsDelay = 120;/* Wait between bits delay */
    static const uint32_t bitWriteDelay = 100;/* bit write delay */

    static const uint32_t OW_ROMCODE_SIZE = 8;

    enum OW_commands : uint8_t
        {   SEARCH_ROM = 0xF0, READ_ROM = 0x33, MATCH = 0x55, SKIP = 0xCC   };
    enum return_type  : uint8_t
        {   OK, Presence_error, Short_circuit_to_GND, ID_err    };

    uint8_t LastDiscrepancy;
    uint8_t LastDeviceFlag;
    uint8_t LastFamilyDiscrepancy;

    public:
    typedef struct id_type
    {
        uint8_t x[8];
    } id_t;
    typedef std::vector<id_t> IDs_t;

    Owi(Owi& owi);
    Owi(GpioPinRef owipin, Timer* timer);

    void writeBit(uint8_t bit);
    void writeByte(uint8_t byte);
    uint8_t readBit();
    uint8_t readByte();
    Owi::return_type resetPulse();
    uint8_t Search(id_t& id);
    uint8_t FindSensors(Owi::IDs_t& IDs);
    Owi::return_type writeCommand(uint8_t command, id_t& id);

};

inline Owi::Owi(Owi& owi) :
        OwiBase(owi._owiPin, owi._timing)
{
}

inline Owi::Owi(GpioPinRef owipin, Timer* timer) :
        OwiBase(owipin, timer)
{

    _owiPin = owipin;
    _owiPin.setMode(Gpio::OUTPUT, Gpio::PUSH_PULL);
    _owiPin.set();
    LastDiscrepancy = LastDeviceFlag = LastFamilyDiscrepancy = 0;
}

inline void Owi::writeByte(uint8_t byte)
{
    uint8_t i, temp;

    for (i = 0; i < 8; i++)
    {
        temp = byte >> i;
        temp &= 0x01;
        writeBit(temp);
    }
    _timing.Delay_us(105);
}

inline void Owi::writeBit(uint8_t bit)
{
    set_Low();
    _timing.Delay_us(masterBitstart);
    if (bit != 0)
    {
        set_High(); // To write 1 to the bus let the pull-up to pull up the bus
    }

    _timing.Delay_us(bitWriteDelay); // wait for the sample time if master want to read 15us-2us-delay
    set_High(); // Drive for Parasite-Power
    _timing.Delay_us(recoveryDelay); //Recovery Time
}

inline uint8_t Owi::readBit()
{
    uint8_t bit;
    set_Low();
    _timing.Delay_us(masterBitstart);
    set_In();
    _timing.Delay_us(bitReadDelay);
    bit = get_Pin();
    return bit;
}

inline uint8_t Owi::readByte()
{
    uint8_t i;
    uint8_t value = 0;
    for (i = 0; i < 8; i++)
    {
        if (readBit())
        {
            value |= (1 << i);
        }
        _timing.Delay_us(120);
    }
    return value;

}

inline Owi::return_type Owi::writeCommand(uint8_t command, id_t& id)
{
    return_type err;
    uint8_t i;

    err = resetPulse();
    if (err)
        return err;
    else
    {
        if (id.x[0] != 0)
        {
            writeByte(OW_commands::MATCH);
            for (i = 0; i < OW_ROMCODE_SIZE; i++)
            {
                writeByte(id.x[i]);
            }
        }
        else
        {
            writeByte(OW_commands::SKIP);
        }
        writeByte(command);
        return err;
    }
}

inline Owi::return_type Owi::resetPulse()
{
    //
    // DS18x20 dq pin be set as output
    //
    volatile uint8_t not_present, Short_Circ;
    set_Low(); // Master reset pulse
    _timing.Delay_us(resetPulseDelay); // 480 us minimum, Datasheet Figure 10.
    set_High();
    _timing.Delay_us(presenceWait);
    set_In();
    not_present = get_Pin(); // 0: Slave pulled down, 1: Still High, no slave
    _timing.Delay_us(480); // Master must stay sum 480us as Input
    Short_Circ = !get_Pin(); // Everybody should leave the bus High, else short circuit to GND

    if (Short_Circ)
        return Owi::Short_circuit_to_GND;
    else if (not_present)
        return Owi::Presence_error;
    else
        return Owi::OK;
}

inline uint8_t Owi::FindSensors(Owi::IDs_t& IDs)
{
    id_t ID = { 0, 0, 0, 0, 0, 0, 0, 0 };
    return_type reset;
    uint8_t number=0;
    volatile uint8_t result = 0;

    // char* temp=nullptr;
    LastDiscrepancy = 0;
    LastDeviceFlag = 0;
    LastFamilyDiscrepancy = 0;

    reset = resetPulse();
    IDs.clear();

    if (reset == return_type::OK)
    {
        do
        {

            result = Search(ID);
            if (result)
            {
                IDs.push_back(ID);
                number++;
            }
        } while (result);

    }
return number;
}



}
