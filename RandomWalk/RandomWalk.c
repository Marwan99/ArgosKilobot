#include <kilolib.h>
#include <stdio.h>

long unsigned int cur_time, prev_time;
char state;
int rand;

void setup()
{
    printf("Hello World\n");
    prev_time=kilo_ticks;
    state=1;
}

void loop()
{
    //printf("%d\n",rand_soft());
    cur_time = kilo_ticks;
    //printf("%d %d\n",cur_time, prev_time);
    if(state == 1 && cur_time-prev_time >120)
    {
        state = 2;
        rand = rand_soft()%2;
        printf("rand: %d\n",rand);
        prev_time = cur_time;
    }

    else if(state == 2 && cur_time-prev_time > 40)
    {
        state = 1;
        prev_time = cur_time;
    }

    if(state == 1)
    {
        spinup_motors();
        state = 1;
        set_color(RGB(0,1,0));
    }
    else if(state == 2)
    {
        set_color(RGB(0,0,1));
        if(rand)
        {
            spinup_motors();
            set_motors(0, kilo_turn_right);
        }
        else
        {
            spinup_motors();
            set_motors(kilo_turn_left, 0);
        }
    }
}

int main()
{
    kilo_init();
    kilo_start(setup, loop);
    return 0;
}
