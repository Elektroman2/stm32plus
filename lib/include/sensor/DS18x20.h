#pragma once

namespace stm32plus
{
class DS18x20 : public Owi{


public :

    typedef struct fract_t
    {
        operator uint32_t()const {return (integ<<16 |fract);}
        fract_t(uint32_t data) { integ= ((data& 0xFFFF0000)>>16); fract= (data & 0x0000FFFF); }
        fract_t(){integ=0;fract=0;}
        int16_t integ;
        int16_t fract;
    }fract_t ;

    typedef union {
            struct
            {
                uint8_t Temp_lsb;
                uint8_t Temp_msb;
                uint8_t Treshhold_h;
                uint8_t Treshhold_l;
                uint8_t Config_reg_res1;
                uint8_t res2;
                uint8_t Count_remain_res3;
                uint8_t Count_persec_res4;
                uint8_t crc;
            }ScratchPad;
            uint8_t SP[9];
        }   DS18x20_SP_t;





    uint8_t Convert( id_t& id);
    uint8_t Convert(){return Convert(_id);}

    uint8_t StartConversion( id_t& id);
    uint8_t StartConversion() {return StartConversion(_id);}

    uint8_t ReadSP( DS18x20_SP_t& sp , id_t& id);
    uint8_t ReadSP( DS18x20_SP_t& sp ){return ReadSP( sp , _id);}
    uint8_t ReadSP(){return ReadSP( _SP , _id);}


    int16_t Calculate_raw_temp_DS18S20( DS18x20_SP_t& SP);
    int16_t Calculate_raw_temp_DS18B20( DS18x20_SP_t& SP);
    fract_t Get_Temp_C( int16_t rawTemperature);
    fract_t Get_Temp_C();
    fract_t ReadTemp();
    fract_t GetTemp();

    DS18x20(id_t& id, Owi& owi);


private :

    enum DS18x20_Commands : uint8_t
    {
        SKIP               = 0xCC,
        ALARM_SEARCH       = 0xEC,
        CONVERT            = 0x44,
        WRITE_SCRATCHPAD   = 0x4E,
        READ_SCRATCHPAD    = 0xBE,
        COPY_SCRATCHPAD    = 0x48,
        RECALL             = 0xB8,
        READ_POWER_SUPPLY  = 0xB4
    };
    enum DS18x20_Families : uint8_t
    {
        DS18S20 = 0x10,
        DS18B20 = 0x28
    };

    id_t _id;
    DS18x20_SP_t _SP;
    typedef std::vector<DS18x20*> DS18x20list_t;

};

inline DS18x20::DS18x20(id_t& id, Owi& owi ) : Owi(owi), _id(id)
{}

inline int16_t DS18x20::Calculate_raw_temp_DS18S20(DS18x20_SP_t& SP)
{
    int16_t rawTemperature = ((SP.ScratchPad.Temp_msb ) << 8) | SP.ScratchPad.Temp_lsb ;
    rawTemperature = ((rawTemperature & 0xFFFE) << 3) + 12 - SP.ScratchPad.Count_remain_res3;
    return rawTemperature;
}

inline int16_t DS18x20::Calculate_raw_temp_DS18B20(DS18x20_SP_t& SP)
{
    int16_t rawTemperature = ((SP.ScratchPad.Temp_msb ) << 8) | SP.ScratchPad.Temp_lsb ;
    return rawTemperature;
}


inline DS18x20::fract_t DS18x20::Get_Temp_C(){

    DS18x20::fract_t temp;
    uint16_t rawTemperature=0;
    if(_id.x[0]== DS18S20)
        rawTemperature = Calculate_raw_temp_DS18S20(_SP);
    if(_id.x[0]== DS18B20)
        rawTemperature = Calculate_raw_temp_DS18B20(_SP);

    temp.integ = rawTemperature/16;
    temp.fract = (rawTemperature%16)*625;
    return temp;
}


inline DS18x20::fract_t DS18x20::ReadTemp()
{
    Convert();
    ReadSP();
    return Get_Temp_C();

}

inline DS18x20::fract_t DS18x20::GetTemp()
{

    ReadSP();
    return Get_Temp_C();

}

inline uint8_t DS18x20::Convert(id_t& id)
{
    uint8_t err=writeCommand(DS18x20_Commands::CONVERT,id);
    set_High();
    _timing.Delay_ms(800);
    set_In();
    return err;
}

inline uint8_t DS18x20::StartConversion(id_t& id)
{
    uint8_t err=writeCommand(DS18x20_Commands::CONVERT,id);
    return err;
}




inline uint8_t DS18x20::ReadSP( DS18x20_SP_t& sp , id_t& id)
{
    uint8_t i,err;

    err=writeCommand(DS18x20_Commands::READ_SCRATCHPAD,id);
    for (i=0;i<9;i++)
        sp.SP[i]=readByte();
    return err;
}

}
