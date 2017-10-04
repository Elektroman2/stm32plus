

#include "config/stm32plus.h"
#include "config/owi.h"



namespace stm32plus{

uint8_t Owi::Search(id_t& id)
{

    int id_bit_number;
    int last_zero, rom_byte_number, search_result;
    int id_bit, cmp_id_bit;
    unsigned char rom_byte_mask, search_direction;

    // initialize for search
    id_bit_number = 1;
    last_zero = 0;
    rom_byte_number = 0;
    rom_byte_mask = 1;
    search_result = 0;

    // if the last call was not the last one
    if (!(LastDeviceFlag))
    {
        // 1-Wire reset
         if (resetPulse() != return_type::OK)
         {
         // reset the search
         LastDiscrepancy = 0;
         LastDeviceFlag = 0;
         LastFamilyDiscrepancy = 0;
         return 0;
         }


        // issue the search command
        writeByte(OW_commands::SEARCH_ROM);

        // loop to do the search
        do
        {
            // read a bit and its complement
            _timing.Delay_us(120);
            id_bit = readBit();
            _timing.Delay_us(120);
            cmp_id_bit = readBit();
            _timing.Delay_us(120);
            // check for no devices on 1-wire
            if ((id_bit == 1) && (cmp_id_bit == 1))
                break;
            else
            {
                // all devices coupled have 0 or 1
                if (id_bit != cmp_id_bit)
                    search_direction = id_bit;  // bit write value for search
                else
                {
                    // if this discrepancy if before the Last Discrepancy
                    // on a previous next then pick the same as last time
                    if (id_bit_number < LastDiscrepancy)
                        search_direction = ((id.x[rom_byte_number] & rom_byte_mask) > 0);
                    else
                        // if equal to last pick 1, if not then pick 0
                        search_direction = (id_bit_number == LastDiscrepancy);

                    // if 0 was picked then record its position in LastZero
                    if (search_direction == 0)
                    {
                        last_zero = id_bit_number;

                        // check for Last discrepancy in family
                        if (last_zero < 9)
                            LastFamilyDiscrepancy = last_zero;
                    }
                }

                // set or clear the bit in the ROM byte rom_byte_number
                // with mask rom_byte_mask
                if (search_direction == 1)
                    id.x[rom_byte_number] |= rom_byte_mask;
                else
                    id.x[rom_byte_number] &= ~rom_byte_mask;

                // serial number search direction write bit
                writeBit(search_direction);

                // increment the byte counter id_bit_number
                // and shift the mask rom_byte_mask
                id_bit_number++;
                rom_byte_mask <<= 1;

                // if the mask is 0 then go to new SerialNum byte rom_byte_number and reset mask
                if (rom_byte_mask == 0)
                {
                    //docrc8(ROM_NO[rom_byte_number]);  // accumulate the CRC
                    rom_byte_number++;
                    rom_byte_mask = 1;
                }
            }
        } while (rom_byte_number < 8);  // loop until through all ROM bytes 0-7

        // if the search was successful then
        if (!((id_bit_number < 65)))
        {
            // search successful so set LastDiscrepancy,LastDeviceFlag,search_result
            LastDiscrepancy = last_zero;

            // check for last device
            if (LastDiscrepancy == 0)
                LastDeviceFlag = 1;

            search_result = 1;
        }
    }

    // if no device found then reset counters so next 'search' will be like a first
    if (!search_result || !id.x[0])
    {
        LastDiscrepancy = 0;
        LastDeviceFlag = 0;
        LastFamilyDiscrepancy = 0;
        search_result = 0;
    }

    return search_result;
}




}
