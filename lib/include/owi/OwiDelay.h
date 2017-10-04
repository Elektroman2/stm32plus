#pragma once


namespace stm32plus {

class OwiDelay{
    protected :


    uint16_t _counterEnd;
    Timer* _timer;


    public :
        OwiDelay(Timer* timer);
        void Delay_us(uint16_t us);
        void Delay_ms(uint16_t ms);


};

inline OwiDelay::OwiDelay(Timer* timer){

    _timer=timer;
    _timer->setTimeBaseByFrequency(9000000,0xFFFFFFFF);
    _timer->enablePeripheral();
    _counterEnd=0;

}

inline void OwiDelay::Delay_us(uint16_t us)
{
volatile uint16_t delay;
    delay=us;

   switch (delay){
     case 1:
         _counterEnd = 1;
         break;
     case 2:
         _counterEnd = 10;
         break;
     case 3:
         _counterEnd = 18 ;
         break;
     case 4:
         _counterEnd = 28;
         break;
     case 5:
         _counterEnd = 35;
         break;
     case 6:
         _counterEnd = 45;
         break;
     case 7:
         _counterEnd = 54 ;
         break;
     case 8:
         _counterEnd = 62;
         break;
     case 9:
         _counterEnd = 72;
         break;
     case 10:
         _counterEnd = 82;
         break;
     case 11:
         _counterEnd = 90 ;
         break;
     case 12:
         _counterEnd = 98;
         break;


     default:

             _counterEnd = (us)*9;

         break;

   }

  for ( _timer->setCounter(0);_counterEnd > _timer->getCounter(););

}

inline void OwiDelay::Delay_ms(uint16_t ms)
{
   volatile uint16_t i;
    for (i=0; i<ms; i++) Delay_us(1000);
}


}
