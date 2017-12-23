#include <kilolib.h>
#define DEBUG
#include <debug.h>

char x;
void setup()
{
    printf("Kilobot started\n");
}

void loop()
{
    set_motors(125, 125);
    delay(3000); //move forward for 3 sec.

    x = rand_hard();
    if(x>128)
    {
        set_color(RGB(1,0,0));
        set_motors(0, 125);
        delay(1000);
    }
    else
    {
        set_color(RGB(0,0,1));
        set_motors(125, 0);
        delay(1000);
    }
    set_color(RGB(0,0,0));
}

int main()
{
    kilo_init();
    debug_init();
    kilo_start(setup, loop);
    
    return 0;
}
